#include "Tempo.h"

void Tempo::Init(float sample_rate)
{
    sampleRate_    = sample_rate;
    bpm_           = 120.0f;
    sampleCounter_ = 0;
    lastTapSample_ = 0;
    hasTap_        = false;
}

void Tempo::Advance(size_t numSamples)
{
    sampleCounter_ += (uint32_t)numSamples;
}

void Tempo::TapBeat()
{
    if(hasTap_)
    {
        uint32_t interval = sampleCounter_ - lastTapSample_;
        if(interval > 0)
        {
            float seconds = (float)interval / sampleRate_;
            float newBpm  = 60.0f / seconds;
            if(newBpm > 30.0f && newBpm < 300.0f)   // sanity clamp, ignore stray taps
                bpm_ = newBpm;
        }
    }
    lastTapSample_ = sampleCounter_;
    hasTap_ = true;
}

float Tempo::GetNoteSamples(float quarterNoteMultiplier) const
{
    float quarterNoteSeconds = 60.0f / bpm_;
    return quarterNoteSeconds * quarterNoteMultiplier * sampleRate_;
}
