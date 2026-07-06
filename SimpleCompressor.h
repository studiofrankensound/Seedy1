#pragma once
#include "daisysp-lgpl.h"

class SimpleCompressor
{
  public:
    void Init(float sample_rate);

    // Updates internal gain from one shared detector signal (linked stereo),
    // then call Apply() separately per channel using that same gain.
    void UpdateEnvelope(float detectorSignal);
    float Apply(float in);

    void SetThresholdDb(float thresholdDb);   // 0 = no compression .. -40 = heavy
    void SetRatio(float ratio);               // 1.0 (off) .. 8.0 (heavy)

  private:
    daisysp::Compressor comp_;
};
