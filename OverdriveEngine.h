#pragma once
#include "daisysp.h"

class OverdriveEngine
{
  public:
    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetDrive(float drive) { drive_ = drive; }   // also doubles as wet/dry -- 0 is transparent
    void SetLevel(float level) { level_ = level; }

  private:
    float ProcessChannel(float in, float &toneState, float &preState);
    static float OnePoleLowpass(float in, float &state, float coeff);

    float drive_;
    float level_;
    float toneStateL_, toneStateR_;
    float toneCoeff_;
    float preStateL_, preStateR_;
    float preCoeff_;
};
