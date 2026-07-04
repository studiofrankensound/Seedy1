#include "DelayEngine.h"

using namespace daisysp;

float DelayEngine::OnePoleLowpass(float in, float &state, float coeff)
{
    state += coeff * (in - state);
    return state;
}

void DelayEngine::SetDamping(float amount)
{
    float tilt = (amount - 0.5f) * 2.0f;   // -1 (full dark) .. +1 (full bright)

    float bassDepth   = 0.7f;   // was 0.5f -- push higher for more dramatic bass cut/boost
    float trebleDepth = 0.5f;

    dampBassGain_   = 1.0f - bassDepth * tilt;
    dampTrebleGain_ = 1.0f + trebleDepth * tilt;
}

void DelayEngine::Init(float sample_rate)
{
    dell_.Init();
    delr_.Init();

    sampleRate_ = sample_rate;
    dampStateL_ = dampStateR_ = 0.0f;
    crossoverCoeff_ = 1.0f - expf(-2.0f * PI_F * 800.0f / sampleRate_);
    dampBassGain_ = dampTrebleGain_ = 1.0f;
    SetDamping(0.3f);


    feedback_ = 0.0f;
    drywet_ = currentDrywet_ = 0.4f;

    currentDelay_ = delayTarget_ = (sample_rate * 0.75f < (float)kMaxDelaySamples)
                                  ? sample_rate * 0.75f
                                  : (float)kMaxDelaySamples;

    dell_.SetDelay(currentDelay_);
    delr_.SetDelay(currentDelay_);
}

void DelayEngine::Process(float inl, float inr, float &outl, float &outr)
{
    fonepole(currentDelay_, delayTarget_, .00007f);
    fonepole(currentDrywet_, drywet_, .0005f);

    dell_.SetDelay(currentDelay_);
    delr_.SetDelay(currentDelay_);

    float dlyL = dell_.Read();
    float dlyR = delr_.Read();

    float bassL = OnePoleLowpass(dlyL, dampStateL_, crossoverCoeff_);
    float bassR = OnePoleLowpass(dlyR, dampStateR_, crossoverCoeff_);
    float trebleL = dlyL - bassL;
    float trebleR = dlyR - bassR;

    float dampedL = dampBassGain_ * bassL + dampTrebleGain_ * trebleL;
    float dampedR = dampBassGain_ * bassR + dampTrebleGain_ * trebleR;

dell_.Write(inl + (feedback_ * dampedL));
delr_.Write(inr + (feedback_ * dampedR));

    outl = inl + currentDrywet_ * (dlyL - inl);
    outr = inr + currentDrywet_ * (dlyR - inr);
}