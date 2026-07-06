#pragma once
#include "daisysp.h"
#include "Tempo.h"

class FilterEngine
{
  public:
    void Init(float sample_rate, Tempo *tempo);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetDepth(float depth) { depth_ = depth; }
    void SetDivisionIndex(int idx);

  private:
    daisysp::Svf filterL_, filterR_;
    daisysp::Oscillator lfo_;
    Tempo *tempo_ = nullptr;
    float depth_ = 0.0f;
    int   divisionIndex_ = 2;   // defaults to quarter note
    float divisions_[8];        // quarter-note multipliers, slowest to fastest -- matches LfoEngine
};
