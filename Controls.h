#pragma once
#include "daisy_pod.h"
#include "Tempo.h"

class Controls
{
  public:
    void Init(daisy::DaisyPod *pod, float sample_rate, float maxDelaySamples, Tempo *tempo);
    void Process();

    float GetCompThreshold() const { return compThreshold_; }
    float GetCompRatio()     const { return compRatio_; }
    float GetBassGain()      const { return bassGain_; }
    float GetTrebleGain()    const { return trebleGain_; }
    float GetPitchSemitones()const { return pitchSemitones_; }
    float GetPitchMix()      const { return pitchMix_; }
    float GetLfoDepth()      const { return lfoDepth_; }
    int   GetLfoDivisionIndex() const { return lfoDivisionIndex_; }
    float GetFilterDepth()      const { return filterDepth_; }
    int   GetFilterDivisionIndex() const { return filterDivisionIndex_; }
    float GetChorusDepth()   const { return chorusDepth_; }
    float GetChorusRate()    const { return chorusRate_; }
    float GetFeedback()      const { return feedback_; }
    float GetDrywet()        const { return drywet_; }
    float GetDampingAmount() const { return dampingAmount_; }
    float GetSpread()         const { return spread_; }
    float GetDelayTarget()   const { return delayTarget_; }
    float GetReverbMix()      const { return reverbMix_; }
    float GetReverbFeedback() const { return reverbFeedback_; }
    float GetReverbDamping()  const { return reverbDamping_; }
    float GetReverbPreDelay() const { return reverbPreDelaySamples_; }
    int   GetMode()          const { return mode_; }
    const char* GetModeName() const;
    bool  GetBypassed()      const { return bypassed_; }

  private:
    enum {
        PAGE_COMP = 0, PAGE_EQ = 1, PAGE_PITCH = 2, PAGE_LFO = 3, PAGE_FILTER = 4,
        PAGE_CHORUS = 5, PAGE_DELAY1 = 6, PAGE_DELAY2 = 7, PAGE_REVERB1 = 8, PAGE_REVERB2 = 9
    };
    static const int kNumEffects = 10;

    void UpdateKnobs();
    void UpdateEncoderButton();
    void UpdatePage();
    void UpdateTapTempo();
    void UpdateDelayRate();
    void UpdateLeds();

    daisy::DaisyPod *pod_ = nullptr;
    Tempo           *tempo_ = nullptr;

    int mode_ = PAGE_COMP;

    daisy::Parameter compThresholdParam_, compRatioParam_;
    daisy::Parameter eqBassParam_, eqTrebleParam_;
    daisy::Parameter pitchMixParam_, pitchSemitoneParam_;
    daisy::Parameter lfoDivisionParam_, lfoDepthParam_;
    daisy::Parameter filterDepthParam_, filterDivisionParam_;
    daisy::Parameter chorusDepthParam_, chorusRateParam_;
    daisy::Parameter drywetParam_, feedbackParam_;
    daisy::Parameter dampingParam_, spreadParam_;
    daisy::Parameter reverbMixParam_, reverbFeedbackParam_;
    daisy::Parameter reverbDampingParam_, reverbPreDelayParam_;

    float compThreshold_ = 0.0f;
    float compRatio_ = 1.0f;
    float bassGain_ = 1.0f;
    float trebleGain_ = 1.0f;
    float pitchSemitones_ = 0.0f;
    float pitchMix_ = 0.0f;
    float lfoDepth_ = 0.0f;
    int   lfoDivisionIndex_ = 0;
    float filterDepth_ = 0.0f;
    int   filterDivisionIndex_ = 2;
    float chorusDepth_ = 0.0f;
    float chorusRate_ = 0.5f;
    float feedback_ = 0.0f;
    float drywet_ = 0.0f;
    float dampingAmount_ = 0.5f;
    float spread_ = 0.0f;
    float delayTarget_ = 0.0f;
    float reverbMix_ = 0.0f;
    float reverbFeedback_ = 0.5f;
    float reverbDamping_ = 10000.0f;
    float reverbPreDelaySamples_ = 1.0f;

    bool bypassed_ = false;

    float movementThreshold_ = 0.015f;   // filters ADC jitter, reacts to real movement
    float minDelaySamples_ = 0.0f, maxDelaySamples_ = 0.0f;

    float knob1EntryRef_ = 0.0f, knob2EntryRef_ = 0.0f;
    bool  knob1Armed_[kNumEffects]   = { false, false, false, false, false, false, false, false, false, false };
    bool  knob2Armed_[kNumEffects]   = { false, false, false, false, false, false, false, false, false, false };

    float k1_ = 0.0f, k2_ = 0.0f;
};
