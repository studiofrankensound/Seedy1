#pragma once
#include "daisysp.h"
#include "daisysp-lgpl.h"

class ReverbEngine
{
  public:
    static const size_t kMaxPreDelaySamples = 4800;   // 100ms at 48kHz

    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetMix(float mix)         { mix_ = mix; }
    void SetFeedback(float fb)     { rev_.SetFeedback(fb); }
    void SetDamping(float freqHz)  { rev_.SetLpFreq(freqHz); }
    void SetPreDelay(float samples);

  private:
    daisysp::ReverbSc rev_;
    daisysp::DelayLine<float, kMaxPreDelaySamples> preDelayL_, preDelayR_;
    float mix_;
    float preDelaySamples_;
};
