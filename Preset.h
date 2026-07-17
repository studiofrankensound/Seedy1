#pragma once
#include <cstring>

// Plain-data snapshot of every user-facing effect parameter. Mirrors the
// live state in Controls -- kept as a flat struct (no pointers/containers)
// so it can be written to QSPI flash as a raw byte blob via PersistentStorage.
struct PresetData
{
    float overdriveDrive = 0.0f;
    float overdriveLevel = 0.6f;
    float compThreshold = 0.0f;
    float compRatio = 1.0f;
    float bassGain = 1.0f;
    float trebleGain = 1.0f;
    float pitchSemitones = 0.0f;
    float pitchMix = 0.0f;
    float lfoDepth = 0.0f;
    int   lfoDivisionIndex = 0;
    float filterDepth = 0.0f;
    int   filterDivisionIndex = 2;
    float chorusDepth = 0.0f;
    float chorusRate = 0.5f;
    float feedback = 0.0f;
    float drywet = 0.0f;
    float dampingAmount = 0.5f;
    float spread = 0.0f;
    int   delayNoteIndex = 0;
    int   delayModifierIndex = 0;
    float reverbMix = 0.0f;
    float reverbFeedback = 0.5f;
    float reverbDamping = 10000.0f;
    float reverbPreDelaySamples = 1.0f;
};

struct PresetBank
{
    static const int kNumPresets = 10;
    PresetData slots[kNumPresets];

    // Required by PersistentStorage to decide whether a write is actually
    // needed -- plain byte comparison is fine since this struct is POD.
    bool operator!=(const PresetBank &other) const
    {
        return memcmp(this, &other, sizeof(PresetBank)) != 0;
    }
};
