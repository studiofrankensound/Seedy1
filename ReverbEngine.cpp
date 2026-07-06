#include "ReverbEngine.h"

void ReverbEngine::Init(float sample_rate)
{
    rev_.Init(sample_rate);
    rev_.SetFeedback(0.5f);
    rev_.SetLpFreq(10000.0f);
    mix_ = 0.0f;

    preDelayL_.Init();
    preDelayR_.Init();
    preDelaySamples_ = 0.0f;
    preDelayL_.SetDelay(1.0f);
    preDelayR_.SetDelay(1.0f);
}

void ReverbEngine::SetPreDelay(float samples)
{
    if(samples < 1.0f) samples = 1.0f;
    if(samples > (float)kMaxPreDelaySamples) samples = (float)kMaxPreDelaySamples;
    preDelaySamples_ = samples;
}

void ReverbEngine::Process(float inl, float inr, float &outl, float &outr)
{
    preDelayL_.SetDelay(preDelaySamples_);
    preDelayR_.SetDelay(preDelaySamples_);

    preDelayL_.Write(inl);
    preDelayR_.Write(inr);

    float predL = preDelayL_.Read();
    float predR = preDelayR_.Read();

    float wetL, wetR;
    rev_.Process(predL, predR, &wetL, &wetR);

    outl = inl + mix_ * (wetL - inl);
    outr = inr + mix_ * (wetR - inr);
}
