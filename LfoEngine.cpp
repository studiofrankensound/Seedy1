#include "LfoEngine.h"

using namespace daisysp;

void LfoEngine::Init(float sample_rate, Tempo *tempo)
{
    sampleRate_ = sample_rate;
    tempo_ = tempo;
    depth_ = 0.0f;
    divisionIndex_ = 0;

    divisions_[0] = 4.0f;      // whole note      -- slowest
    divisions_[1] = 2.0f;      // half note
    divisions_[2] = 1.0f;      // quarter note
    divisions_[3] = 0.75f;     // dotted eighth
    divisions_[4] = 0.5f;      // eighth
    divisions_[5] = 0.3333f;   // triplet
    divisions_[6] = 0.25f;     // sixteenth
    divisions_[7] = 0.125f;    // thirty-second    -- fastest

    osc_.Init(sample_rate);
    osc_.SetWaveform(Oscillator::WAVE_SIN);
    osc_.SetAmp(1.0f);
    osc_.SetFreq(2.0f);
}

void LfoEngine::SetDivisionIndex(int idx)
{
    if(idx < 0) idx = 0;
    if(idx > 7) idx = 7;
    divisionIndex_ = idx;
}

void LfoEngine::Process(float inl, float inr, float &outl, float &outr)
{
    float noteSeconds = (60.0f / tempo_->GetBpm()) * divisions_[divisionIndex_];
    float freq = 1.0f / noteSeconds;
    osc_.SetFreq(freq);

    float lfo = osc_.Process();   // -1..1
    float mod = 1.0f - depth_ * 0.5f * (1.0f - lfo);   // tremolo: 1.0 at peak, (1-depth) at trough

    outl = inl * mod;
    outr = inr * mod;
}
