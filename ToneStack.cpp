#include "ToneStack.h"
#include "daisysp.h"

void ToneStack::Init(float sample_rate, float crossoverHz)
{
    bassCoeff_ = 1.0f - expf(-2.0f * PI_F * crossoverHz / sample_rate);
}

float ToneStack::OnePoleLowpass(float in, float &state, float coeff)
{
    state += coeff * (in - state);
    return state;
}

void ToneStack::Process(float inl, float inr, float &outl, float &outr)
{
    float bassL = OnePoleLowpass(inl, bassStateL_, bassCoeff_);
    float bassR = OnePoleLowpass(inr, bassStateR_, bassCoeff_);
    float trebleL = inl - bassL;
    float trebleR = inr - bassR;

    outl = (bassGain_ * bassL) + (trebleGain_ * trebleL);
    outr = (bassGain_ * bassR) + (trebleGain_ * trebleR);
}