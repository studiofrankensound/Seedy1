#include "daisysp.h"
#include "daisy_pod.h"
#include "DelayEngine.h"
#include "ToneStack.h"
#include "Controls.h"

using namespace daisysp;
using namespace daisy;

static DaisyPod pod;
static DelayEngine DSY_SDRAM_BSS delayEngine;  // large buffers -- must live in SDRAM
static ToneStack toneStack;
static Controls controls;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                    AudioHandle::InterleavingOutputBuffer out,
                    size_t                                size)
{
    controls.Process();

    delayEngine.SetFeedback(controls.GetFeedback());
    delayEngine.SetDrywet(controls.GetDrywet());
    delayEngine.SetDelayTarget(controls.GetDelayTarget());
    toneStack.SetBassGain(controls.GetBassGain());
    toneStack.SetTrebleGain(controls.GetTrebleGain());
    delayEngine.SetDamping(controls.GetDampingAmount());

    for(size_t i = 0; i < size; i += 2)
    {
        float inl = in[i];
        float inr = in[i + 1];

        float dlyL, dlyR, eqL, eqR;
        delayEngine.Process(inl, inr, dlyL, dlyR);
        toneStack.Process(dlyL, dlyR, eqL, eqR);

        out[i]     = eqL;
        out[i + 1] = eqR;
    }
}

int main(void)
{
    pod.Init();
    pod.seed.StartLog(false);  
    pod.SetAudioBlockSize(4);
    float sample_rate = pod.AudioSampleRate();

    delayEngine.Init(sample_rate);
    toneStack.Init(sample_rate, 600.0f);
    controls.Init(&pod, sample_rate, (float)DelayEngine::kMaxDelaySamples);

    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1) {
        pod.seed.PrintLine("mode:%d fb:%d dw:%d bass:%d treb:%d delrate:%d damp:%d",
                            controls.GetMode(),
                            (int)(controls.GetFeedback() * 100),
                            (int)(controls.GetDrywet() * 100),
                            (int)(controls.GetBassGain() * 100),
                            (int)(controls.GetTrebleGain() * 100),
                            (int)controls.GetDelayTarget(),
                            (int)(controls.GetDampingAmount() * 100));

        System::Delay(250);  // print ~4x/sec instead of flooding or blocking anything time-critical
    }
}