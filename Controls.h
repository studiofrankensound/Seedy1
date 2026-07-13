#pragma once
#include "daisy_pod.h"
#include "Tempo.h"

class Controls
{
  public:
    struct Range { float min, max; };

    void Init(daisy::DaisyPod *pod, float sample_rate, float maxDelaySamples, Tempo *tempo);
    void Process();

    float GetOverdriveDrive() const { return overdriveDrive_; }
    float GetOverdriveLevel() const { return overdriveLevel_; }
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
    int   GetDelayNoteIndex()     const { return delayNoteIndex_; }
    int   GetDelayModifierIndex() const { return delayModifierIndex_; }
    float GetDelayTarget()   const { return delayTarget_; }
    float GetReverbMix()      const { return reverbMix_; }
    float GetReverbFeedback() const { return reverbFeedback_; }
    float GetReverbDamping()  const { return reverbDamping_; }
    float GetReverbPreDelay() const { return reverbPreDelaySamples_; }
    int   GetMode()          const { return mode_; }
    const char* GetModeName() const;
    bool  GetBypassed()      const { return bypassed_; }

    // Real configured range for each parameter -- the single source of truth,
    // set from the exact same values passed into each Parameter::Init() call.
    Range GetOverdriveDriveRange()  const { return { overdriveDriveMin_, overdriveDriveMax_ }; }
    Range GetOverdriveLevelRange()  const { return { overdriveLevelMin_, overdriveLevelMax_ }; }
    Range GetCompThresholdRange()   const { return { compThresholdMin_, compThresholdMax_ }; }
    Range GetCompRatioRange()       const { return { compRatioMin_, compRatioMax_ }; }
    Range GetBassGainRange()        const { return { bassGainMin_, bassGainMax_ }; }
    Range GetTrebleGainRange()      const { return { trebleGainMin_, trebleGainMax_ }; }
    Range GetPitchMixRange()        const { return { pitchMixMin_, pitchMixMax_ }; }
    Range GetPitchSemitonesRange()  const { return { pitchSemitonesMin_, pitchSemitonesMax_ }; }
    Range GetLfoDepthRange()        const { return { lfoDepthMin_, lfoDepthMax_ }; }
    Range GetLfoDivisionRange()     const { return { 0.0f, 7.0f }; }   // post-conversion index range, not the raw knob's 0-1
    Range GetFilterDepthRange()     const { return { filterDepthMin_, filterDepthMax_ }; }
    Range GetFilterDivisionRange()  const { return { 0.0f, 7.0f }; }   // same -- these are separate quantities from the knob Parameter
    Range GetChorusDepthRange()     const { return { chorusDepthMin_, chorusDepthMax_ }; }
    Range GetChorusRateRange()      const { return { chorusRateMin_, chorusRateMax_ }; }
    Range GetDrywetRange()          const { return { drywetMin_, drywetMax_ }; }
    Range GetFeedbackRange()        const { return { feedbackMin_, feedbackMax_ }; }
    Range GetDampingRange()         const { return { dampingMin_, dampingMax_ }; }
    Range GetSpreadRange()          const { return { spreadMin_, spreadMax_ }; }
    Range GetDelayNoteRange()       const { return { 0.0f, 3.0f }; }   // post-conversion index range -- quarter..thirty-second
    Range GetDelayModifierRange()   const { return { 0.0f, 2.0f }; }   // post-conversion index range -- straight/dotted/triplet
    Range GetReverbMixRange()       const { return { reverbMixMin_, reverbMixMax_ }; }
    Range GetReverbFeedbackRange()  const { return { reverbFeedbackMin_, reverbFeedbackMax_ }; }
    Range GetReverbDampingRange()   const { return { reverbDampingMin_, reverbDampingMax_ }; }
    Range GetReverbPreDelayRange()  const { return { reverbPreDelayMin_, reverbPreDelayMax_ }; }

  private:
    // PAGE_DELAY3 (note division) now sits before PAGE_DELAY2 (damping/spread)
    // in page order, so damping/spread is the last delay page -- only the
    // numbers swapped, the symbolic names still mean the same feature.
    enum {
        PAGE_OVERDRIVE = 0, PAGE_COMP = 1, PAGE_EQ = 2, PAGE_PITCH = 3, PAGE_LFO = 4, PAGE_FILTER = 5,
        PAGE_CHORUS = 6, PAGE_DELAY1 = 7, PAGE_DELAY3 = 8, PAGE_DELAY2 = 9, PAGE_REVERB1 = 10, PAGE_REVERB2 = 11
    };
    static const int kNumEffects = 12;

    void UpdateKnobs();
    void UpdateEncoderButton();
    void UpdatePage();
    void UpdateTapTempo();
    void UpdateDelayRate();
    void UpdateLeds();

    daisy::DaisyPod *pod_ = nullptr;
    Tempo           *tempo_ = nullptr;

    int mode_ = PAGE_OVERDRIVE;

    daisy::Parameter overdriveDriveParam_, overdriveLevelParam_;
    daisy::Parameter compThresholdParam_, compRatioParam_;
    daisy::Parameter eqBassParam_, eqTrebleParam_;
    daisy::Parameter pitchMixParam_, pitchSemitoneParam_;
    daisy::Parameter lfoDivisionParam_, lfoDepthParam_;
    daisy::Parameter filterDepthParam_, filterDivisionParam_;
    daisy::Parameter chorusDepthParam_, chorusRateParam_;
    daisy::Parameter drywetParam_, feedbackParam_;
    daisy::Parameter dampingParam_, spreadParam_;
    daisy::Parameter delayNoteParam_, delayModifierParam_;
    daisy::Parameter reverbMixParam_, reverbFeedbackParam_;
    daisy::Parameter reverbDampingParam_, reverbPreDelayParam_;

    float overdriveDrive_ = 0.0f;
    float overdriveLevel_ = 1.0f;
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
    int   delayNoteIndex_ = 0;       // quarter note by default
    int   delayModifierIndex_ = 0;   // straight by default
    float delayTarget_ = 0.0f;
    float reverbMix_ = 0.0f;
    float reverbFeedback_ = 0.5f;
    float reverbDamping_ = 10000.0f;
    float reverbPreDelaySamples_ = 1.0f;

    // Real configured ranges -- set once in Init(), feeding both the actual
    // Parameter::Init() calls and the getters above, so they can never drift apart.
    float overdriveDriveMin_, overdriveDriveMax_;
    float overdriveLevelMin_, overdriveLevelMax_;
    float compThresholdMin_, compThresholdMax_;
    float compRatioMin_, compRatioMax_;
    float bassGainMin_, bassGainMax_;
    float trebleGainMin_, trebleGainMax_;
    float pitchMixMin_, pitchMixMax_;
    float pitchSemitonesMin_, pitchSemitonesMax_;
    float lfoDepthMin_, lfoDepthMax_;
    float lfoDivisionMin_, lfoDivisionMax_;
    float filterDepthMin_, filterDepthMax_;
    float filterDivisionMin_, filterDivisionMax_;
    float chorusDepthMin_, chorusDepthMax_;
    float chorusRateMin_, chorusRateMax_;
    float drywetMin_, drywetMax_;
    float feedbackMin_, feedbackMax_;
    float dampingMin_, dampingMax_;
    float spreadMin_, spreadMax_;
    float delayNoteMin_, delayNoteMax_;
    float delayModifierMin_, delayModifierMax_;
    float reverbMixMin_, reverbMixMax_;
    float reverbFeedbackMin_, reverbFeedbackMax_;
    float reverbDampingMin_, reverbDampingMax_;
    float reverbPreDelayMin_, reverbPreDelayMax_;

    bool bypassed_ = false;

    float movementThreshold_ = 0.015f;   // filters ADC jitter, reacts to real movement
    float minDelaySamples_ = 0.0f, maxDelaySamples_ = 0.0f;

    float knob1EntryRef_ = 0.0f, knob2EntryRef_ = 0.0f;
    bool  knob1Armed_[kNumEffects]   = { false, false, false, false, false, false, false, false, false, false, false, false };
    bool  knob2Armed_[kNumEffects]   = { false, false, false, false, false, false, false, false, false, false, false, false };

    float k1_ = 0.0f, k2_ = 0.0f;
};
