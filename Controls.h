#pragma once
#include "daisy_pod.h"

class Controls
{
  public:
    void Init(daisy::DaisyPod *pod, float sample_rate, float maxDelaySamples);
    void Process();

    float GetFeedback()    const { return feedback_; }
    float GetDrywet()      const { return drywet_; }
    float GetBassGain()    const { return bassGain_; }
    float GetTrebleGain()  const { return trebleGain_; }
    float GetDelayTarget() const { return delayTarget_; }
    float GetDampingAmount() const { return dampingAmount_; }
    int   GetMode() const { return mode_; }

  private:
    enum { PAGE1 = 0, PAGE2 = 1 };

    void UpdateKnobs();
    void UpdatePage();
    void UpdateDelayRate();
    void UpdateLeds();

    daisy::DaisyPod *pod_ = nullptr;
    int mode_ = PAGE1;

    daisy::Parameter feedbackParam_, drywetParam_, toneParam_;
    daisy::Parameter dampingParam_;

  
    float feedback_ = 0.0f, drywet_ = 0.4f;
    float bassGain_ = 1.0f, trebleGain_ = 1.0f;
    float delayTarget_ = 0.0f;
    float dampingAmount_ = 0.3f;

    float pickupThreshold_ = 0.02f;
    float delayStepSamples_ = 480.0f;
    float minDelaySamples_ = 0.0f, maxDelaySamples_ = 0.0f;

    float knob1LastRaw_[2] = { 0.0f, 0.0f };
    float knob2LastRaw_[2] = { 0.0f, 0.0f };
    bool  knob1Armed_[2]   = { false, false };
    bool  knob2Armed_[2]   = { false, false };

    float k1_ = 0.0f, k2_ = 0.0f;
};