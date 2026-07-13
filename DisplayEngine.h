#pragma once
#include "daisy_pod.h"
#include "dev/oled_ssd130x.h"
#include "FastOledI2C.h"
#include "Controls.h"
#include "Tempo.h"

class DisplayEngine
{
  public:
    using Oled = daisy::OledDisplay<daisy::SSD130xDriver<128, 64, FastOledI2C>>;

    void Init();
    void Update(Controls &controls, Tempo &tempo);

  private:
    void WriteBoldString(int x, int y, const char *str);
    void WriteCenteredString(int columnX, int columnWidth, int y, const char *str, FontDef font);
    static void FormatRemapped(char *buf, size_t bufSize, float raw,
                               float rawMin, float rawMax,
                               float dispMin, float dispMax);

    Oled display_;
};
