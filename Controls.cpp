#include "Controls.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

void Controls::Init(DaisyPod *pod, float sample_rate, float maxDelaySamples, Tempo *tempo)
{
    pod_ = pod;
    tempo_ = tempo;
    maxDelaySamples_ = maxDelaySamples;
    minDelaySamples_ = sample_rate * 0.02f;

    compThresholdParam_.Init(pod_->knob1, 0.0f, -40.0f, Parameter::LINEAR);   // PAGE_COMP knob1 -- threshold
    compRatioParam_.Init(pod_->knob2, 1.0f, 8.0f, Parameter::LINEAR);         // PAGE_COMP knob2 -- ratio

    eqBassParam_.Init(pod_->knob1, 0.5f, 1.5f, Parameter::LINEAR);            // PAGE_EQ knob1 -- bass
    eqTrebleParam_.Init(pod_->knob2, 0.5f, 2.5f, Parameter::LINEAR);          // PAGE_EQ knob2 -- treble

    pitchMixParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);         // PAGE_PITCH knob1 -- wet/dry
    pitchSemitoneParam_.Init(pod_->knob2, -12.0f, 12.0f, Parameter::LINEAR); // PAGE_PITCH knob2 -- pitch

    lfoDepthParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);         // PAGE_LFO knob1 -- depth
    lfoDivisionParam_.Init(pod_->knob2, 0.0f, 1.0f, Parameter::LINEAR);      // PAGE_LFO knob2 -- rate/division

    filterDepthParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);      // PAGE_FILTER knob1 -- cutoff depth
    filterDivisionParam_.Init(pod_->knob2, 0.0f, 1.0f, Parameter::LINEAR);   // PAGE_FILTER knob2 -- sweep speed (tempo division)

    chorusDepthParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);      // PAGE_CHORUS knob1 -- depth
    chorusRateParam_.Init(pod_->knob2, 0.05f, 5.0f, Parameter::LINEAR);      // PAGE_CHORUS knob2 -- rate (Hz, not tempo-synced)

    drywetParam_.Init(pod_->knob1, 0.0f, 0.5f, Parameter::LINEAR);           // PAGE_DELAY1 knob1 -- wet/dry
    feedbackParam_.Init(pod_->knob2, 0.0f, 0.9f, Parameter::CUBE);           // PAGE_DELAY1 knob2 -- feedback

    dampingParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);          // PAGE_DELAY2 knob1 -- damping
    spreadParam_.Init(pod_->knob2, 0.0f, 1.0f, Parameter::LINEAR);           // PAGE_DELAY2 knob2 -- spread -- mono to ping-pong (mystery)

    reverbMixParam_.Init(pod_->knob1, 0.0f, 1.0f, Parameter::LINEAR);        // PAGE_REVERB1 knob1 -- wet/dry
    reverbFeedbackParam_.Init(pod_->knob2, 0.0f, 0.98f, Parameter::LINEAR);  // PAGE_REVERB1 knob2 -- time/size

    reverbDampingParam_.Init(pod_->knob1, 1000.0f, 18000.0f, Parameter::LOGARITHMIC);  // PAGE_REVERB2 knob1 -- damping/tone
    reverbPreDelayParam_.Init(pod_->knob2, 1.0f, (float)4800, Parameter::LINEAR);      // PAGE_REVERB2 knob2 -- pre-delay (samples, ~100ms max)

    delayTarget_ = tempo_->GetNoteSamples(1.0f);
    delayTarget_ = fclamp(delayTarget_, minDelaySamples_, maxDelaySamples_);
}

void Controls::Process()
{
    pod_->ProcessAnalogControls();
    pod_->ProcessDigitalControls();

    k1_ = pod_->knob1.Process();
    k2_ = pod_->knob2.Process();

    UpdateEncoderButton();
    UpdateTapTempo();
    UpdatePage();
    UpdateKnobs();
    UpdateDelayRate();
    UpdateLeds();
}

void Controls::UpdateKnobs()
{
    if(!knob1Armed_[mode_] && fabsf(k1_ - knob1EntryRef_) > movementThreshold_)
        knob1Armed_[mode_] = true;
    if(!knob2Armed_[mode_] && fabsf(k2_ - knob2EntryRef_) > movementThreshold_)
        knob2Armed_[mode_] = true;

    switch(mode_)
    {
        case PAGE_COMP:
            if(knob1Armed_[mode_]) compThreshold_ = compThresholdParam_.Process();
            if(knob2Armed_[mode_]) compRatio_     = compRatioParam_.Process();
            break;
        case PAGE_EQ:
            if(knob1Armed_[mode_]) bassGain_   = eqBassParam_.Process();
            if(knob2Armed_[mode_]) trebleGain_ = eqTrebleParam_.Process();
            break;
        case PAGE_PITCH:
            if(knob1Armed_[mode_]) pitchMix_       = pitchMixParam_.Process();
            if(knob2Armed_[mode_]) pitchSemitones_ = pitchSemitoneParam_.Process();
            break;
        case PAGE_LFO:
            if(knob1Armed_[mode_]) lfoDepth_ = lfoDepthParam_.Process();
            if(knob2Armed_[mode_])
            {
                float raw = lfoDivisionParam_.Process();
                int idx = (int)(raw * 8.0f);
                if(idx > 7) idx = 7;
                lfoDivisionIndex_ = idx;
            }
            break;
        case PAGE_FILTER:
            if(knob1Armed_[mode_]) filterDepth_ = filterDepthParam_.Process();
            if(knob2Armed_[mode_])
            {
                float raw = filterDivisionParam_.Process();
                int idx = (int)(raw * 8.0f);
                if(idx > 7) idx = 7;
                filterDivisionIndex_ = idx;
            }
            break;
        case PAGE_CHORUS:
            if(knob1Armed_[mode_]) chorusDepth_ = chorusDepthParam_.Process();
            if(knob2Armed_[mode_]) chorusRate_  = chorusRateParam_.Process();
            break;
        case PAGE_DELAY1:
            if(knob1Armed_[mode_]) drywet_   = drywetParam_.Process();
            if(knob2Armed_[mode_]) feedback_ = feedbackParam_.Process();
            break;
        case PAGE_DELAY2:
            if(knob1Armed_[mode_]) dampingAmount_ = dampingParam_.Process();
            if(knob2Armed_[mode_]) spread_        = spreadParam_.Process();
            break;
        case PAGE_REVERB1:
            if(knob1Armed_[mode_]) reverbMix_      = reverbMixParam_.Process();
            if(knob2Armed_[mode_]) reverbFeedback_ = reverbFeedbackParam_.Process();
            break;
        case PAGE_REVERB2:
            if(knob1Armed_[mode_]) reverbDamping_        = reverbDampingParam_.Process();
            if(knob2Armed_[mode_]) reverbPreDelaySamples_ = reverbPreDelayParam_.Process();
            break;
    }
}

void Controls::UpdateEncoderButton()
{
    if(pod_->encoder.FallingEdge())
    {
        bypassed_ = !bypassed_;
    }
}

void Controls::UpdatePage()
{
    if(pod_->button2.RisingEdge())
    {
        mode_ = (mode_ + 1) % kNumEffects;

        knob1Armed_[mode_] = false;
        knob2Armed_[mode_] = false;
        knob1EntryRef_ = k1_;
        knob2EntryRef_ = k2_;
    }
}

void Controls::UpdateTapTempo()
{
    if(pod_->button1.RisingEdge())
    {
        tempo_->TapBeat();
    }
}

void Controls::UpdateDelayRate()
{
    int inc = pod_->encoder.Increment();
    if(inc != 0)
    {
        float bpm = tempo_->GetBpm() + (float)inc * 1.0f;
        bpm = fclamp(bpm, 30.0f, 300.0f);
        tempo_->SetBpm(bpm);
    }

    delayTarget_ = tempo_->GetNoteSamples(1.0f);
    delayTarget_ = fclamp(delayTarget_, minDelaySamples_, maxDelaySamples_);
}

const char* Controls::GetModeName() const
{
    static const char* names[kNumEffects] = {
        "Comp", "Eq", "Pitch", "Lfo1", "Lfo2", "Chorus", "Delay-a", "Delay-b", "Verb-a", "Verb-b"
    };
    return names[mode_];
}

void Controls::UpdateLeds()
{
    if(bypassed_)
    {
        pod_->led1.Set(1.0f, 0.0f, 0.0f);
        pod_->led2.Set(1.0f, 0.0f, 0.0f);
        pod_->UpdateLeds();
        return;
    }

    struct Style { float r, g, b; bool isA; };
    static const Style pageStyles[kNumEffects] = {
        {0.0f, 0.0f, 1.0f,  true},   // PAGE_COMP     - blue,  a
        {1.0f, 0.3f, 0.6f,  true},   // PAGE_EQ       - pink,  a
        {0.0f, 0.7f, 0.6f,  true},   // PAGE_PITCH    - teal,  a
        {1.0f, 1.0f, 0.0f,  true},   // PAGE_LFO      - yellow, a
        {1.0f, 0.35f, 0.0f, true},   // PAGE_FILTER   - orange, a
        {0.8f, 0.5f, 1.0f,  true},   // PAGE_CHORUS   - lilac,  a
        {0.6f, 0.0f, 1.0f,  true},   // PAGE_DELAY1   - purple, a
        {0.6f, 0.0f, 1.0f,  false},  // PAGE_DELAY2   - purple, b
        {0.0f, 1.0f, 0.0f,  true},   // PAGE_REVERB1  - green,  a
        {0.0f, 1.0f, 0.0f,  false},  // PAGE_REVERB2  - green,  b
    };

    Style s = pageStyles[mode_];
    if(s.isA)
    {
        pod_->led1.Set(s.r, s.g, s.b);
        pod_->led2.Set(0.0f, 0.0f, 0.0f);
    }
    else
    {
        pod_->led1.Set(0.0f, 0.0f, 0.0f);
        pod_->led2.Set(s.r, s.g, s.b);
    }

    pod_->UpdateLeds();
}
