#include "SimpleCompressor.h"

void SimpleCompressor::Init(float sample_rate)
{
    comp_.Init(sample_rate);
    comp_.SetAttack(0.005f);
    comp_.SetRelease(0.15f);
    comp_.SetThreshold(0.0f);
    comp_.SetRatio(1.0f);
    comp_.AutoMakeup(true);
}

void SimpleCompressor::UpdateEnvelope(float detectorSignal)
{
    comp_.Process(detectorSignal);
}

float SimpleCompressor::Apply(float in)
{
    return comp_.Apply(in);
}

void SimpleCompressor::SetThresholdDb(float thresholdDb)
{
    comp_.SetThreshold(thresholdDb);
}

void SimpleCompressor::SetRatio(float ratio)
{
    comp_.SetRatio(ratio);
}
