#include "FilterEngine.h"

using namespace daisysp;

void FilterEngine::Init(float sample_rate, Tempo *tempo)
{
    tempo_ = tempo;
    depth_ = 0.0f;
    divisionIndex_ = 2;   // quarter note by default

    divisions_[0] = 4.0f;      // whole note      -- slowest
    divisions_[1] = 2.0f;      // half note
    divisions_[2] = 1.0f;      // quarter note
    divisions_[3] = 0.75f;     // dotted eighth
    divisions_[4] = 0.5f;      // eighth
    divisions_[5] = 0.3333f;   // triplet
    divisions_[6] = 0.25f;     // sixteenth
    divisions_[7] = 0.125f;    // thirty-second    -- fastest

    filterL_.Init(sample_rate);
    filterR_.Init(sample_rate);
    filterL_.SetRes(0.4f);    // fixed, moderate wah-like resonance
    filterR_.SetRes(0.4f);
    filterL_.SetFreq(1000.0f);
    filterR_.SetFreq(1000.0f);

    lfo_.Init(sample_rate);
    lfo_.SetWaveform(Oscillator::WAVE_SIN);
    lfo_.SetAmp(1.0f);
    lfo_.SetFreq(2.0f);
}

void FilterEngine::SetDivisionIndex(int idx)
{
    if(idx < 0) idx = 0;
    if(idx > 7) idx = 7;
    divisionIndex_ = idx;
}

void FilterEngine::Process(float inl, float inr, float &outl, float &outr)
{
    float noteSeconds = (60.0f / tempo_->GetBpm()) * divisions_[divisionIndex_];
    lfo_.SetFreq(1.0f / noteSeconds);

    float lfoVal = (lfo_.Process() + 1.0f) * 0.5f;   // 0..1

    float floorHz = 200.0f;
    float ceilingHz = floorHz + depth_ * 7800.0f;
    float cutoff = floorHz + lfoVal * (ceilingHz - floorHz);

    filterL_.SetFreq(cutoff);
    filterR_.SetFreq(cutoff);

    filterL_.Process(inl);
    filterR_.Process(inr);

    float wetL = filterL_.Band();
    float wetR = filterR_.Band();

    // depth_ still does double duty: widens the sweep range AND blends the
    // filtered signal in, so depth=0 stays genuinely transparent.
    outl = inl + depth_ * (wetL - inl);
    outr = inr + depth_ * (wetR - inr);
}
