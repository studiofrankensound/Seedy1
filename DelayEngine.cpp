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

    float bassDepth   = 0.7f;
    float trebleDepth = 0.5f;

    // Cut-only: neither gain may exceed 1.0, otherwise this filter adds gain
    // into the feedback loop and repeats in that band sustain longer than
    // the feedback knob implies (or never fully decay) at extreme settings.
    dampBassGain_   = (tilt > 0.0f) ? (1.0f - bassDepth * tilt)   : 1.0f;
    dampTrebleGain_ = (tilt < 0.0f) ? (1.0f + trebleDepth * tilt) : 1.0f;
}

void DelayEngine::Init(float sample_rate)
{
    dell_.Init();
    delr_.Init();

    sampleRate_ = sample_rate;
    dampStateL_ = dampStateR_ = 0.0f;
    crossoverCoeff_ = 1.0f - expf(-2.0f * PI_F * 800.0f / sampleRate_);
    dampBassGain_ = dampTrebleGain_ = 1.0f;
    SetDamping(0.5f);   // flat/neutral tilt by default

    feedback_ = 0.0f;
    drywet_ = currentDrywet_ = 0.0f;
    spread_ = 0.0f;   // fully independent stereo (mono-in-mono-out for a mono source) until turned up

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

    // Stereo spread: at 0, channels behave independently (existing behavior).
    // At 1, left chains into right (classic ping-pong): left is fed from
    // input + feedback of right's last repeat, right is fed directly from
    // left's current repeat -- creating true alternating L-R-L-R bounce,
    // which a symmetric crossfade cannot produce for a mono source since
    // dlyL and dlyR would stay identical and "crossing" them would be a no-op.
    float indepL = inl + (feedback_ * dampedL);
    float indepR = inr + (feedback_ * dampedR);

    float monoIn = (inl + inr) * 0.5f;
    float pingL = monoIn + (feedback_ * dampedR);
    float pingR = dampedL;

    float writeL = indepL + spread_ * (pingL - indepL);
    float writeR = indepR + spread_ * (pingR - indepR);

    dell_.Write(writeL);
    delr_.Write(writeR);

    outl = inl + currentDrywet_ * (dlyL - inl);
    outr = inr + currentDrywet_ * (dlyR - inr);
}
