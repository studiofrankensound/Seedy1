#pragma once
#include "daisysp.h"

class PitchShiftEngine
{
  public:
    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetSemitones(float semi) { semitones_ = semi; }
    void SetMix(float mix)        { mix_ = mix; }

  private:
    daisysp::PitchShifter shifterL_, shifterR_;
    float semitones_ = 0.0f;
    float mix_ = 0.0f;
};
