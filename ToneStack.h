#pragma once

class ToneStack
{
  public:
    void Init(float sample_rate, float crossoverHz = 600.0f);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetBassGain(float g)   { bassGain_ = g; }
    void SetTrebleGain(float g) { trebleGain_ = g; }

  private:
    static float OnePoleLowpass(float in, float &state, float coeff);

    float bassStateL_ = 0.0f, bassStateR_ = 0.0f;
    float bassCoeff_ = 0.0f;

    float bassGain_ = 1.0f, trebleGain_ = 1.0f;
    float currentBassGain_ = 1.0f, currentTrebleGain_ = 1.0f;
};
