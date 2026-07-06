#include "ToneStack.h"
#include "daisysp.h"

using namespace daisysp;

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
    fonepole(currentBassGain_, bassGain_, .001f);
    fonepole(currentTrebleGain_, trebleGain_, .001f);

    float bassL = OnePoleLowpass(inl, bassStateL_, bassCoeff_);
    float bassR = OnePoleLowpass(inr, bassStateR_, bassCoeff_);
    float trebleL = inl - bassL;
    float trebleR = inr - bassR;

    outl = (currentBassGain_ * bassL) + (currentTrebleGain_ * trebleL);
    outr = (currentBassGain_ * bassR) + (currentTrebleGain_ * trebleR);
}
