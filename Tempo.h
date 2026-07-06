#pragma once
#include <cstdint>
#include <cstddef>

class Tempo
{
  public:
    void Init(float sample_rate);
    void Advance(size_t numSamples);   // call once per audio block
    void TapBeat();                    // call when a tap is registered

    float GetBpm() const { return bpm_; }
    void  SetBpm(float bpm) { bpm_ = bpm; }
    float GetNoteSamples(float quarterNoteMultiplier) const;

  private:
    float    sampleRate_ = 48000.0f;
    float    bpm_        = 120.0f;
    uint32_t sampleCounter_ = 0;
    uint32_t lastTapSample_ = 0;
    bool     hasTap_ = false;
};
