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

    overdriveDriveMin_ = 0.0f;    overdriveDriveMax_ = 1.0f;
    overdriveDriveParam_.Init(pod_->knob1, overdriveDriveMin_, overdriveDriveMax_, Parameter::LINEAR);   // PAGE_OVERDRIVE knob1 -- drive
    overdriveLevelMin_ = 0.1f;    overdriveLevelMax_ = 0.7f;
    overdriveLevelParam_.Init(pod_->knob2, overdriveLevelMin_, overdriveLevelMax_, Parameter::LINEAR);   // PAGE_OVERDRIVE knob2 -- level

    compThresholdMin_ = 0.0f;     compThresholdMax_ = -40.0f;
    compThresholdParam_.Init(pod_->knob1, compThresholdMin_, compThresholdMax_, Parameter::LINEAR);   // PAGE_COMP knob1 -- threshold
    compRatioMin_ = 1.0f;         compRatioMax_ = 8.0f;
    compRatioParam_.Init(pod_->knob2, compRatioMin_, compRatioMax_, Parameter::LINEAR);         // PAGE_COMP knob2 -- ratio

    bassGainMin_ = 0.5f;          bassGainMax_ = 1.5f;
    eqBassParam_.Init(pod_->knob1, bassGainMin_, bassGainMax_, Parameter::LINEAR);            // PAGE_EQ knob1 -- bass
    trebleGainMin_ = 0.5f;        trebleGainMax_ = 2.5f;
    eqTrebleParam_.Init(pod_->knob2, trebleGainMin_, trebleGainMax_, Parameter::LINEAR);          // PAGE_EQ knob2 -- treble

    pitchMixMin_ = 0.0f;          pitchMixMax_ = 1.0f;
    pitchMixParam_.Init(pod_->knob1, pitchMixMin_, pitchMixMax_, Parameter::LINEAR);         // PAGE_PITCH knob1 -- wet/dry
    pitchSemitonesMin_ = -12.0f;  pitchSemitonesMax_ = 12.0f;
    pitchSemitoneParam_.Init(pod_->knob2, pitchSemitonesMin_, pitchSemitonesMax_, Parameter::LINEAR); // PAGE_PITCH knob2 -- pitch

    lfoDepthMin_ = 0.0f;          lfoDepthMax_ = 1.0f;
    lfoDepthParam_.Init(pod_->knob1, lfoDepthMin_, lfoDepthMax_, Parameter::LINEAR);         // PAGE_LFO knob1 -- depth
    lfoDivisionMin_ = 0.0f;       lfoDivisionMax_ = 1.0f;
    lfoDivisionParam_.Init(pod_->knob2, lfoDivisionMin_, lfoDivisionMax_, Parameter::LINEAR);      // PAGE_LFO knob2 -- rate/division

    filterDepthMin_ = 0.0f;       filterDepthMax_ = 1.0f;
    filterDepthParam_.Init(pod_->knob1, filterDepthMin_, filterDepthMax_, Parameter::LINEAR);      // PAGE_FILTER knob1 -- cutoff depth
    filterDivisionMin_ = 0.0f;    filterDivisionMax_ = 1.0f;
    filterDivisionParam_.Init(pod_->knob2, filterDivisionMin_, filterDivisionMax_, Parameter::LINEAR);   // PAGE_FILTER knob2 -- sweep speed (tempo division)

    chorusDepthMin_ = 0.0f;       chorusDepthMax_ = 1.0f;
    chorusDepthParam_.Init(pod_->knob1, chorusDepthMin_, chorusDepthMax_, Parameter::LINEAR);      // PAGE_CHORUS knob1 -- depth
    chorusRateMin_ = 0.05f;       chorusRateMax_ = 16.0f;
    chorusRateParam_.Init(pod_->knob2, chorusRateMin_, chorusRateMax_, Parameter::LINEAR);      // PAGE_CHORUS knob2 -- rate (Hz, not tempo-synced)

    drywetMin_ = 0.0f;            drywetMax_ = 0.5f;
    drywetParam_.Init(pod_->knob1, drywetMin_, drywetMax_, Parameter::LINEAR);           // PAGE_DELAY1 knob1 -- wet/dry
    feedbackMin_ = 0.0f;          feedbackMax_ = 0.9f;
    feedbackParam_.Init(pod_->knob2, feedbackMin_, feedbackMax_, Parameter::CUBE);           // PAGE_DELAY1 knob2 -- feedback

    dampingMin_ = 0.0f;           dampingMax_ = 1.0f;
    dampingParam_.Init(pod_->knob1, dampingMin_, dampingMax_, Parameter::LINEAR);          // PAGE_DELAY2 knob1 -- damping
    spreadMin_ = 0.0f;            spreadMax_ = 1.0f;
    spreadParam_.Init(pod_->knob2, spreadMin_, spreadMax_, Parameter::LINEAR);           // PAGE_DELAY2 knob2 -- spread -- mono to ping-pong (mystery)

    delayNoteMin_ = 0.0f;         delayNoteMax_ = 1.0f;
    delayNoteParam_.Init(pod_->knob1, delayNoteMin_, delayNoteMax_, Parameter::LINEAR);      // PAGE_DELAY3 knob1 -- note fraction (whole..thirty-second)
    delayModifierMin_ = 0.0f;     delayModifierMax_ = 1.0f;
    delayModifierParam_.Init(pod_->knob2, delayModifierMin_, delayModifierMax_, Parameter::LINEAR);   // PAGE_DELAY3 knob2 -- straight/dotted/triplet

    reverbMixMin_ = 0.0f;         reverbMixMax_ = 1.0f;
    reverbMixParam_.Init(pod_->knob1, reverbMixMin_, reverbMixMax_, Parameter::LINEAR);        // PAGE_REVERB1 knob1 -- wet/dry
    reverbFeedbackMin_ = 0.0f;    reverbFeedbackMax_ = 0.98f;
    reverbFeedbackParam_.Init(pod_->knob2, reverbFeedbackMin_, reverbFeedbackMax_, Parameter::LINEAR);  // PAGE_REVERB1 knob2 -- time/size

    reverbDampingMin_ = 1000.0f;  reverbDampingMax_ = 18000.0f;
    reverbDampingParam_.Init(pod_->knob1, reverbDampingMin_, reverbDampingMax_, Parameter::LOGARITHMIC);  // PAGE_REVERB2 knob1 -- damping/tone
    reverbPreDelayMin_ = 1.0f;    reverbPreDelayMax_ = 4800.0f;
    reverbPreDelayParam_.Init(pod_->knob2, reverbPreDelayMin_, reverbPreDelayMax_, Parameter::LINEAR);      // PAGE_REVERB2 knob2 -- pre-delay (samples, ~100ms max)

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
        case PAGE_OVERDRIVE:
            if(knob1Armed_[mode_]) overdriveDrive_ = overdriveDriveParam_.Process();
            if(knob2Armed_[mode_]) overdriveLevel_ = overdriveLevelParam_.Process();
            break;
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
        case PAGE_DELAY3:
            if(knob1Armed_[mode_])
            {
                float raw = delayNoteParam_.Process();
                int idx = (int)(raw * 4.0f);
                if(idx > 3) idx = 3;
                delayNoteIndex_ = idx;
            }
            if(knob2Armed_[mode_])
            {
                float raw = delayModifierParam_.Process();
                int idx = (int)(raw * 3.0f);
                if(idx > 2) idx = 2;
                delayModifierIndex_ = idx;
            }
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

    static const float kNoteBase[4] = {
        1.0f,     // quarter    -- slowest (whole/half dropped -- exceed the 0.75s delay buffer at real tempos)
        0.5f,     // eighth
        0.25f,    // sixteenth
        0.125f    // thirty-second -- fastest
    };

    float multiplier = kNoteBase[delayNoteIndex_];
    if(delayModifierIndex_ == 1)      multiplier *= 1.5f;        // dotted
    else if(delayModifierIndex_ == 2) multiplier *= 2.0f / 3.0f; // triplet

    delayTarget_ = tempo_->GetNoteSamples(multiplier);
    delayTarget_ = fclamp(delayTarget_, minDelaySamples_, maxDelaySamples_);
}

const char* Controls::GetModeName() const
{
    static const char* names[kNumEffects] = {
        "Drive", "Compressor", "Eq", "Pitch", "Lfo1", "Lfo2", "Chorus", "Delay", "Delay", "Delay", "Reverb", "Reverb"
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

    struct Style { float r, g, b; bool isA; bool isBoth; };
    static const Style pageStyles[kNumEffects] = {
        {1.0f, 0.4f, 0.4f,  true,  false},   // PAGE_OVERDRIVE - light red, a
        {0.0f, 0.0f, 1.0f,  true,  false},   // PAGE_COMP     - blue,  a
        {1.0f, 0.3f, 0.6f,  true,  false},   // PAGE_EQ       - pink,  a
        {0.0f, 0.7f, 0.6f,  true,  false},   // PAGE_PITCH    - teal,  a
        {1.0f, 1.0f, 0.0f,  true,  false},   // PAGE_LFO      - yellow, a
        {1.0f, 0.35f, 0.0f, true,  false},   // PAGE_FILTER   - orange, a
        {0.85f, 0.0f, 0.6f, true,  false},   // PAGE_CHORUS   - violet (magenta-leaning), a
        {0.4f, 0.0f, 1.0f,  true,  false},   // PAGE_DELAY1   - purple (blue-leaning), a
        {0.4f, 0.0f, 1.0f,  true,  true},    // PAGE_DELAY3   - purple, both LEDs
        {0.4f, 0.0f, 1.0f,  false, false},   // PAGE_DELAY2   - purple, b
        {0.0f, 1.0f, 0.0f,  true,  false},   // PAGE_REVERB1  - green,  a
        {0.0f, 1.0f, 0.0f,  false, false},   // PAGE_REVERB2  - green,  b
    };

    Style s = pageStyles[mode_];
    if(s.isBoth)
    {
        pod_->led1.Set(s.r, s.g, s.b);
        pod_->led2.Set(s.r, s.g, s.b);
    }
    else if(s.isA)
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
