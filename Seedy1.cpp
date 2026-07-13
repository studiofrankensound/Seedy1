#include "daisysp.h"
#include "daisy_pod.h"
#include "DelayEngine.h"
#include "ToneStack.h"
#include "Controls.h"
#include "Tempo.h"
#include "SimpleCompressor.h"
#include "PitchShiftEngine.h"
#include "LfoEngine.h"
#include "ReverbEngine.h"
#include "FilterEngine.h"
#include "SimpleChorusEngine.h"
#include "OverdriveEngine.h"
#include "DisplayEngine.h"

using namespace daisysp;
using namespace daisy;

static DaisyPod pod;
static DelayEngine DSY_SDRAM_BSS delayEngine;   // large buffers -- must live in SDRAM
static PitchShiftEngine pitchShiftEngine;   // regular RAM -- guarantees zero-initialized state on boot
static ToneStack toneStack;
static Controls controls;
static Tempo tempo;
static OverdriveEngine overdriveEngine;
static SimpleCompressor comp;
static LfoEngine lfoEngine;
static FilterEngine filterEngine;
static SimpleChorusEngine chorusEngine;
static ReverbEngine DSY_SDRAM_BSS reverbEngine;
static DisplayEngine displayEngine;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                    AudioHandle::InterleavingOutputBuffer out,
                    size_t                                size)
{
    controls.Process();
    tempo.Advance(size);

    overdriveEngine.SetDrive(controls.GetOverdriveDrive());
    overdriveEngine.SetLevel(controls.GetOverdriveLevel());

    comp.SetThresholdDb(controls.GetCompThreshold());
    comp.SetRatio(controls.GetCompRatio());

    toneStack.SetBassGain(controls.GetBassGain());
    toneStack.SetTrebleGain(controls.GetTrebleGain());

    pitchShiftEngine.SetSemitones(controls.GetPitchSemitones());
    pitchShiftEngine.SetMix(controls.GetPitchMix());

    lfoEngine.SetDepth(controls.GetLfoDepth());
    lfoEngine.SetDivisionIndex(controls.GetLfoDivisionIndex());

    filterEngine.SetDepth(controls.GetFilterDepth());
    filterEngine.SetDivisionIndex(controls.GetFilterDivisionIndex());

    chorusEngine.SetDepth(controls.GetChorusDepth());
    chorusEngine.SetRate(controls.GetChorusRate());

    delayEngine.SetFeedback(controls.GetFeedback());
    delayEngine.SetDrywet(controls.GetDrywet());
    delayEngine.SetDelayTarget(controls.GetDelayTarget());
    delayEngine.SetDamping(controls.GetDampingAmount());
    delayEngine.SetSpread(controls.GetSpread());

    reverbEngine.SetMix(controls.GetReverbMix());
    reverbEngine.SetFeedback(controls.GetReverbFeedback());
    reverbEngine.SetDamping(controls.GetReverbDamping());
    reverbEngine.SetPreDelay(controls.GetReverbPreDelay());

    for(size_t i = 0; i < size; i += 2)
    {
        float inl = in[i];
        float inr = in[i + 1];

        if(controls.GetBypassed())
        {
            out[i]     = inl;
            out[i + 1] = inr;
        }
        else
        {
            float odL, odR;
            overdriveEngine.Process(inl, inr, odL, odR);

            float peak = fmaxf(fabsf(odL), fabsf(odR));
            comp.UpdateEnvelope(peak);
            float compedL = comp.Apply(odL);
            float compedR = comp.Apply(odR);

            float eqL, eqR;
            toneStack.Process(compedL, compedR, eqL, eqR);

            float pitchL, pitchR;
            pitchShiftEngine.Process(eqL, eqR, pitchL, pitchR);

            float lfoL, lfoR;
            lfoEngine.Process(pitchL, pitchR, lfoL, lfoR);

            float filtL, filtR;
            filterEngine.Process(lfoL, lfoR, filtL, filtR);

            float chorusL, chorusR;
            chorusEngine.Process(filtL, filtR, chorusL, chorusR);

            float dlyL, dlyR;
            delayEngine.Process(chorusL, chorusR, dlyL, dlyR);

            float revL, revR;
            reverbEngine.Process(dlyL, dlyR, revL, revR);

            out[i]     = revL;
            out[i + 1] = revR;
        }
    }
}

int main(void)
{
    pod.Init();
    pod.SetAudioBlockSize(48);
    float sample_rate = pod.AudioSampleRate();

    tempo.Init(sample_rate);
    overdriveEngine.Init(sample_rate);
    delayEngine.Init(sample_rate);
    toneStack.Init(sample_rate, 600.0f);
    comp.Init(sample_rate);
    pitchShiftEngine.Init(sample_rate);
    lfoEngine.Init(sample_rate, &tempo);
    filterEngine.Init(sample_rate, &tempo);
    chorusEngine.Init(sample_rate);
    reverbEngine.Init(sample_rate);
    displayEngine.Init();
    controls.Init(&pod, sample_rate, (float)DelayEngine::kMaxDelaySamples, &tempo);

    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1)
    {
        displayEngine.Update(controls, tempo);
        System::Delay(100);
    }
}
