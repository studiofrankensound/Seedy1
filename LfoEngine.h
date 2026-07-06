#pragma once
#include "daisysp.h"
#include "Tempo.h"

class LfoEngine
{
  public:
    void Init(float sample_rate, Tempo *tempo);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetDepth(float depth) { depth_ = depth; }
    void SetDivisionIndex(int idx);

  private:
    daisysp::Oscillator osc_;
    Tempo *tempo_ = nullptr;
    float depth_ = 0.0f;
    float sampleRate_ = 48000.0f;
    int   divisionIndex_ = 0;
    float divisions_[8];   // quarter-note multipliers, slowest to fastest
};
