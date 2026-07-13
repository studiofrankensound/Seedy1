#pragma once
#include "daisysp.h"

class DelayEngine
{
  public:
    // 3s covers the longest tempo-synced delay the note-division page can ask
    // for -- a dotted quarter note at the slowest allowed tempo (30 BPM) is
    // 1.5 * 60/30 = 3s. Anything shorter used to clamp and stop tracking tempo.
    static const size_t kMaxDelaySamples = static_cast<size_t>(48000 * 3.0f);

    void Init(float sample_rate);
    void Process(float inl, float inr, float &outl, float &outr);

    void SetFeedback(float fb)   { feedback_ = fb; }
    void SetDrywet(float dw)     { drywet_ = dw; }
    void SetDelayTarget(float t) { delayTarget_ = t; }
    void SetDamping(float amount);   // 0 = dark repeats, 1 = bright repeats
    void SetSpread(float amount) { spread_ = amount; }   // 0 = normal stereo, 1 = full ping-pong

  private:
    daisysp::DelayLine<float, kMaxDelaySamples> dell_, delr_;
    float currentDelay_, delayTarget_;
    float feedback_, drywet_, currentDrywet_;

    static float OnePoleLowpass(float in, float &state, float coeff);

    float dampStateL_, dampStateR_;
    float crossoverCoeff_;
    float dampBassGain_, dampTrebleGain_;
    float sampleRate_;

    float spread_;
};
