#include "SimpleChorusEngine.h"

using namespace daisysp;

void SimpleChorusEngine::Init(float sample_rate)
{
    sampleRate_ = sample_rate;
    depth_ = 0.0f;

    delL_.Init();
    delR_.Init();
    delL_.SetDelay(sample_rate * 0.015f);   // ~15ms base
    delR_.SetDelay(sample_rate * 0.015f);

    lfoL_.Init(sample_rate);
    lfoL_.SetWaveform(Oscillator::WAVE_SIN);
    lfoL_.SetAmp(1.0f);
    lfoL_.SetFreq(0.5f);

    lfoR_.Init(sample_rate);
    lfoR_.SetWaveform(Oscillator::WAVE_SIN);
    lfoR_.SetAmp(1.0f);
    lfoR_.SetFreq(0.5f);
}

void SimpleChorusEngine::SetRate(float rateHz)
{
    lfoL_.SetFreq(rateHz);
    lfoR_.SetFreq(rateHz * 1.03f);   // tiny L/R offset for stereo width, classic chorus trick
}

void SimpleChorusEngine::Process(float inl, float inr, float &outl, float &outr)
{
    float baseDelay = sampleRate_ * 0.015f;          // 15ms center
    float modRange  = depth_ * sampleRate_ * 0.0025f; // up to +/-2.5ms swing at full depth

    float delayTimeL = baseDelay + lfoL_.Process() * modRange;
    float delayTimeR = baseDelay + lfoR_.Process() * modRange;
    if(delayTimeL < 1.0f) delayTimeL = 1.0f;
    if(delayTimeR < 1.0f) delayTimeR = 1.0f;

    delL_.SetDelay(delayTimeL);
    delR_.SetDelay(delayTimeR);

    delL_.Write(inl);
    delR_.Write(inr);

    float wetL = delL_.Read();
    float wetR = delR_.Read();

    // depth_ does double duty: widens the modulation swing AND blends the
    // chorused signal in, so depth=0 stays genuinely transparent.
    outl = inl + depth_ * (wetL - inl);
    outr = inr + depth_ * (wetR - inr);
}
