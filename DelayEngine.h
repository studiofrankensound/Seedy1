#pragma once
#include "daisysp.h"

class DelayEngine
{
  public:
    static const size_t kMaxDelaySamples = static_cast<size_t>(48000 * 0.75f);

    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetFeedback(float fb)   { feedback_ = fb; }
    void SetDrywet(float dw)     { drywet_ = dw; }
    void SetDelayTarget(float t) { delayTarget_ = t; }

  private:
    daisysp::DelayLine<float, kMaxDelaySamples> dell_, delr_;
    float currentDelay_, delayTarget_;
    float feedback_, drywet_, currentDrywet_;
};