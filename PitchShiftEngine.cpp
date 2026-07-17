#include "PitchShiftEngine.h"

using namespace daisysp;

void PitchShiftEngine::Init(float sample_rate)
{
    shifterL_.Init(sample_rate);
    shifterR_.Init(sample_rate);
    semitones_ = 0.0f;
    mix_ = 0.0f;

    // Smaller grain window = lower latency, at the cost of more audible
    // warble on larger pitch shifts. Library default is 16384 samples
    // (~341ms at 48kHz) which feels laggy; 4096 (~85ms) is a much more
    // playable starting point. Try smaller (2048, 1024) for even less
    // latency if the extra warble is acceptable for your use case.
    shifterL_.SetDelSize(4096);
    shifterR_.SetDelSize(4096);

    // DaisySP's PitchShifter::Init() reads its own transpose_ member before
    // ever setting it -- force a known value explicitly rather than relying
    // on that memory happening to already be zero.
    shifterL_.SetTransposition(0.0f);
    shifterR_.SetTransposition(0.0f);
}

void PitchShiftEngine::Process(float inl, float inr, float &outl, float &outr)
{
    shifterL_.SetTransposition(semitones_);
    shifterR_.SetTransposition(semitones_);

    float shiftedL = shifterL_.Process(inl);
    float shiftedR = shifterR_.Process(inr);

    outl = inl + mix_ * (shiftedL - inl);
    outr = inr + mix_ * (shiftedR - inr);
}
