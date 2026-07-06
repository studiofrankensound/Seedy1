#pragma once
#include "daisysp.h"

class SimpleChorusEngine
{
  public:
    static const size_t kMaxDelaySamples = 2400;   // ~50ms at 48kHz -- plenty for chorus depth range

    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetDepth(float depth) { depth_ = depth; }   // also doubles as wet/dry mix -- 0 is transparent
    void SetRate(float rateHz);                      // free-running, not tempo-synced

  private:
    daisysp::DelayLine<float, kMaxDelaySamples> delL_, delR_;
    daisysp::Oscillator lfoL_, lfoR_;
    float depth_;
    float sampleRate_;
};
