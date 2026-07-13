#pragma once
#include "per/i2c.h"

// Drop-in replacement for daisy::SSD130xI2CTransport that sends the whole
// framebuffer in a single I2C transaction instead of one byte at a time.
// The stock transport's SendData() issues a separate blocking transmission
// per byte (~1024 individual transactions per screen refresh), which is
// what caused the noticeable display lag -- batching it into one transfer
// removes essentially all of that per-transaction overhead.
class FastOledI2C
{
  public:
    struct Config
    {
        Config() { Defaults(); }
        daisy::I2CHandle::Config i2c_config;
        uint8_t                  i2c_address;
        void Defaults()
        {
            i2c_config.periph         = daisy::I2CHandle::Config::Peripheral::I2C_1;
            i2c_config.speed          = daisy::I2CHandle::Config::Speed::I2C_1MHZ;
            i2c_config.mode           = daisy::I2CHandle::Config::Mode::I2C_MASTER;
            i2c_config.pin_config.scl = daisy::Pin(daisy::PORTB, 8);
            i2c_config.pin_config.sda = daisy::Pin(daisy::PORTB, 9);
            i2c_address               = 0x3C;
        }
    };

    void Init(const Config &config)
    {
        i2c_address_ = config.i2c_address;
        i2c_.Init(config.i2c_config);
    }

    void SendCommand(uint8_t cmd)
    {
        uint8_t buf[2] = {0x00, cmd};
        i2c_.TransmitBlocking(i2c_address_, buf, 2, 1000);
    }

    void SendData(uint8_t *buff, size_t size)
    {
        // 0x40 marks "data stream follows" -- send it once, then the whole
        // buffer in one continuous transaction rather than per-byte.
        static uint8_t txBuf[1025];   // 1 control byte + 128*64/8 = 1024 data bytes
        if(size > sizeof(txBuf) - 1) size = sizeof(txBuf) - 1;

        txBuf[0] = 0x40;
        for(size_t i = 0; i < size; i++)
            txBuf[i + 1] = buff[i];

        i2c_.TransmitBlocking(i2c_address_, txBuf, size + 1, 1000);
    }

  private:
    daisy::I2CHandle i2c_;
    uint8_t          i2c_address_;
};
