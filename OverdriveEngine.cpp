#include "OverdriveEngine.h"

using namespace daisysp;

void OverdriveEngine::Init(float sample_rate)
{
    drive_ = 0.0f;
    level_ = 0.3f;
    toneStateL_ = toneStateR_ = 0.0f;
    toneCoeff_ = 1.0f - expf(-2.0f * PI_F * 9000.0f / sample_rate);   // raised from 6kHz -- lets more bite through
    preStateL_ = preStateR_ = 0.0f;
    preCoeff_ = 1.0f - expf(-2.0f * PI_F * 1000.0f / sample_rate);    // pre-emphasis crossover
}

float OverdriveEngine::OnePoleLowpass(float in, float &state, float coeff)
{
    state += coeff * (in - state);
    return state;
}

float OverdriveEngine::ProcessChannel(float in, float &toneState, float &preState)
{
    // Pre-emphasis: boost highs slightly before clipping. Since clipping
    // harmonics are generated from whatever content goes INTO the clipper,
    // sharpening the treble first makes the resulting harmonic content
    // sharper and more present -- this is what actually reads as "bite"
    // rather than just "more distorted."
    float preBass = OnePoleLowpass(in, preState, preCoeff_);
    float preTreble = in - preBass;
    float emphasized = in + preTreble * 0.6f;

    float driveGain = 1.0f + drive_ * 24.0f;   // slightly more headroom than before
    float driven = emphasized * driveGain;

    float bias = drive_ * 0.15f;
    float clipped = SoftClip(driven + bias) - SoftClip(bias);

    float toned = OnePoleLowpass(clipped, toneState, toneCoeff_);

    float wet = toned * level_;

    return in + drive_ * (wet - in);
}

void OverdriveEngine::Process(float inl, float inr, float &outl, float &outr)
{
    outl = ProcessChannel(inl, toneStateL_, preStateL_);
    outr = ProcessChannel(inr, toneStateR_, preStateR_);
}
