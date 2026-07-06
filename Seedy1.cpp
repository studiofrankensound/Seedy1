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

using namespace daisysp;
using namespace daisy;

static DaisyPod pod;
static DelayEngine DSY_SDRAM_BSS delayEngine;   // large buffers -- must live in SDRAM
static PitchShiftEngine pitchShiftEngine;   // regular RAM -- guarantees zero-initialized state on boot
static ToneStack toneStack;
static Controls controls;
static Tempo tempo;
static SimpleCompressor comp;
static LfoEngine lfoEngine;
static FilterEngine filterEngine;
static SimpleChorusEngine chorusEngine;
static ReverbEngine DSY_SDRAM_BSS reverbEngine;

static volatile float peakCompL = 0, peakCompR = 0;
static volatile float peakEqL = 0, peakEqR = 0;
static volatile float peakPitchL = 0, peakPitchR = 0;
static volatile float peakLfoL = 0, peakLfoR = 0;
static volatile float peakFiltL = 0, peakFiltR = 0;
static volatile float peakChorusL = 0, peakChorusR = 0;
static volatile float peakDlyL = 0, peakDlyR = 0;
static volatile float peakRevL = 0, peakRevR = 0;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                    AudioHandle::InterleavingOutputBuffer out,
                    size_t                                size)
{
    controls.Process();
    tempo.Advance(size);

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
            float peak = fmaxf(fabsf(inl), fabsf(inr));
            comp.UpdateEnvelope(peak);
            float compedL = comp.Apply(inl);
            float compedR = comp.Apply(inr);
            if(fabsf(compedL) > peakCompL) peakCompL = fabsf(compedL);
            if(fabsf(compedR) > peakCompR) peakCompR = fabsf(compedR);

            float eqL, eqR;
            toneStack.Process(compedL, compedR, eqL, eqR);
            if(fabsf(eqL) > peakEqL) peakEqL = fabsf(eqL);
            if(fabsf(eqR) > peakEqR) peakEqR = fabsf(eqR);

            float pitchL, pitchR;
            pitchShiftEngine.Process(eqL, eqR, pitchL, pitchR);
            if(fabsf(pitchL) > peakPitchL) peakPitchL = fabsf(pitchL);
            if(fabsf(pitchR) > peakPitchR) peakPitchR = fabsf(pitchR);

            float lfoL, lfoR;
            lfoEngine.Process(pitchL, pitchR, lfoL, lfoR);
            if(fabsf(lfoL) > peakLfoL) peakLfoL = fabsf(lfoL);
            if(fabsf(lfoR) > peakLfoR) peakLfoR = fabsf(lfoR);

            float filtL, filtR;
            filterEngine.Process(lfoL, lfoR, filtL, filtR);
            if(fabsf(filtL) > peakFiltL) peakFiltL = fabsf(filtL);
            if(fabsf(filtR) > peakFiltR) peakFiltR = fabsf(filtR);

            float chorusL, chorusR;
            chorusEngine.Process(filtL, filtR, chorusL, chorusR);
            if(fabsf(chorusL) > peakChorusL) peakChorusL = fabsf(chorusL);
            if(fabsf(chorusR) > peakChorusR) peakChorusR = fabsf(chorusR);

            float dlyL, dlyR;
            delayEngine.Process(chorusL, chorusR, dlyL, dlyR);
            if(fabsf(dlyL) > peakDlyL) peakDlyL = fabsf(dlyL);
            if(fabsf(dlyR) > peakDlyR) peakDlyR = fabsf(dlyR);

            float revL, revR;
            reverbEngine.Process(dlyL, dlyR, revL, revR);
            if(fabsf(revL) > peakRevL) peakRevL = fabsf(revL);
            if(fabsf(revR) > peakRevR) peakRevR = fabsf(revR);

            out[i]     = revL;
            out[i + 1] = revR;
        }
    }
}

int main(void)
{
    pod.Init();
    pod.seed.StartLog(false);
    pod.SetAudioBlockSize(48);
    float sample_rate = pod.AudioSampleRate();

    tempo.Init(sample_rate);
    delayEngine.Init(sample_rate);
    toneStack.Init(sample_rate, 600.0f);
    comp.Init(sample_rate);
    pitchShiftEngine.Init(sample_rate);
    lfoEngine.Init(sample_rate, &tempo);
    filterEngine.Init(sample_rate, &tempo);
    chorusEngine.Init(sample_rate);
    reverbEngine.Init(sample_rate);
    controls.Init(&pod, sample_rate, (float)DelayEngine::kMaxDelaySamples, &tempo);

    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1)
    {
        pod.seed.PrintLine("mode:%s bypass:%s bpm:%d",
                            controls.GetModeName(),
                            controls.GetBypassed() ? "On" : "Off",
                            (int)tempo.GetBpm());
        System::Delay(5);

        pod.seed.PrintLine("  comp-thresh:%d comp-ratio:%d  eq-bass:%d eq-treble:%d",
                            (int)controls.GetCompThreshold(),
                            (int)(controls.GetCompRatio() * 10),
                            (int)(controls.GetBassGain() * 100),
                            (int)(controls.GetTrebleGain() * 100));
        System::Delay(5);

        pod.seed.PrintLine("  pitch-wd:%d pitch-pitch:%d  lfo-depth:%d lfo-rate:%d",
                            (int)(controls.GetPitchMix() * 100),
                            (int)controls.GetPitchSemitones(),
                            (int)(controls.GetLfoDepth() * 100),
                            controls.GetLfoDivisionIndex());
        System::Delay(5);

        pod.seed.PrintLine("  filt-depth:%d filt-rate:%d",
                            (int)(controls.GetFilterDepth() * 100),
                            controls.GetFilterDivisionIndex());
        System::Delay(5);

        pod.seed.PrintLine("  chorus-depth:%d chorus-rate:%d",
                            (int)(controls.GetChorusDepth() * 100),
                            (int)(controls.GetChorusRate() * 100));
        System::Delay(5);

        pod.seed.PrintLine("  del-wd:%d del-fb:%d del-damp:%d del-x:%d",
                            (int)(controls.GetDrywet() * 100),
                            (int)(controls.GetFeedback() * 100),
                            (int)(controls.GetDampingAmount() * 100),
                            (int)(controls.GetSpread() * 100));
        System::Delay(5);

        pod.seed.PrintLine("  verb-wd:%d verb-time:%d verb-tone:%d verb-pd:%d",
                            (int)(controls.GetReverbMix() * 100),
                            (int)(controls.GetReverbFeedback() * 100),
                            (int)controls.GetReverbDamping(),
                            (int)controls.GetReverbPreDelay());
        System::Delay(5);

        pod.seed.PrintLine(" ");   // blank line between updates for visual separation

        peakCompL = peakCompR = 0;
        peakEqL = peakEqR = 0;
        peakPitchL = peakPitchR = 0;
        peakLfoL = peakLfoR = 0;
        peakFiltL = peakFiltR = 0;
        peakChorusL = peakChorusR = 0;
        peakDlyL = peakDlyR = 0;
        peakRevL = peakRevR = 0;

        System::Delay(1000);
    }
}
