#include "DelayEngine.h"

using namespace daisysp;

void DelayEngine::Init(float sample_rate)
{
    dell_.Init();
    delr_.Init();

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

    dell_.Write(inl + (feedback_ * dlyL));
    delr_.Write(inr + (feedback_ * dlyR));

    outl = inl + currentDrywet_ * (dlyL - inl);
    outr = inr + currentDrywet_ * (dlyR - inr);
}