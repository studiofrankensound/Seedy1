#include "Controls.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

void Controls::Init(DaisyPod *pod, float sample_rate, float maxDelaySamples)
{
    pod_ = pod;
    maxDelaySamples_ = maxDelaySamples;
    minDelaySamples_ = sample_rate * 0.02f;

    feedbackParam_.Init(pod_->knob1, 0.0f, 0.9f, Parameter::CUBE);   // PAGE1 knob1
    drywetParam_.Init(pod_->knob2, 0.0f, 1.0f, Parameter::LINEAR);   // PAGE1 knob2
    bassParam_.Init(pod_->knob1, 0.0f, 2.0f, Parameter::LINEAR);     // PAGE2 knob1
    trebleParam_.Init(pod_->knob2, 0.0f, 2.0f, Parameter::LINEAR);   // PAGE2 knob2

    delayTarget_ = (sample_rate * 0.75f < maxDelaySamples_) ? sample_rate * 0.75f : maxDelaySamples_;
}

void Controls::Process()
{
    pod_->ProcessAnalogControls();
    pod_->ProcessDigitalControls();

    UpdateKnobs();
    UpdateDelayRate();
    UpdatePage();
    UpdateLeds();
}

void Controls::UpdateKnobs()
{
    k1_ = pod_->knob1.Process();
    k2_ = pod_->knob2.Process();

    if(!knob1Armed_[mode_] && fabsf(k1_ - knob1LastRaw_[mode_]) < pickupThreshold_)
        knob1Armed_[mode_] = true;
    if(!knob2Armed_[mode_] && fabsf(k2_ - knob2LastRaw_[mode_]) < pickupThreshold_)
        knob2Armed_[mode_] = true;

    switch(mode_)
    {
        case PAGE1:
            if(knob1Armed_[mode_]) { feedback_ = feedbackParam_.Process(); knob1LastRaw_[mode_] = k1_; }
            if(knob2Armed_[mode_]) { drywet_   = drywetParam_.Process();   knob2LastRaw_[mode_] = k2_; }
            break;
        case PAGE2:
            if(knob1Armed_[mode_]) { bassGain_   = bassParam_.Process();   knob1LastRaw_[mode_] = k1_; }
            if(knob2Armed_[mode_]) { trebleGain_ = trebleParam_.Process(); knob2LastRaw_[mode_] = k2_; }
            break;
    }
}

void Controls::UpdatePage()
{
    if(pod_->button2.RisingEdge())
    {
        knob1Armed_[mode_] = false;
        knob2Armed_[mode_] = false;
        mode_ = (mode_ == PAGE1) ? PAGE2 : PAGE1;
    }
}

void Controls::UpdateDelayRate()
{
    int inc = pod_->encoder.Increment();
    if(inc != 0)
    {
        delayTarget_ += inc * delayStepSamples_;
        delayTarget_ = fclamp(delayTarget_, minDelaySamples_, maxDelaySamples_);
    }
}

void Controls::UpdateLeds()
{
    pod_->led1.Set(k1_ * (mode_ == 2), k1_ * (mode_ == 1), k1_ * (mode_ == 0 || mode_ == 2));
    pod_->led2.Set(k2_ * (mode_ == 2), k2_ * (mode_ == 1), k2_ * (mode_ == 0 || mode_ == 2));
    pod_->UpdateLeds();
}