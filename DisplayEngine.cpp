#include "DisplayEngine.h"
#include <cstdio>
#include <cstring>
#include <cctype>

using namespace daisy;

void DisplayEngine::FormatRemapped(char *buf, size_t bufSize, float raw,
                                    float rawMin, float rawMax,
                                    float dispMin, float dispMax)
{
    float t = (raw - rawMin) / (rawMax - rawMin);
    float displayValue = dispMin + t * (dispMax - dispMin);

    // Round to nearest instead of truncating toward zero -- (int)99.7 gives
    // 99, not 100, which is what caused values to consistently fall one
    // short of their true endpoint. Works correctly for negative ranges too.
    int rounded = (displayValue >= 0.0f) ? (int)(displayValue + 0.5f)
                                         : (int)(displayValue - 0.5f);
    snprintf(buf, bufSize, "%d", rounded);
}

void DisplayEngine::WriteCenteredString(int columnX, int columnWidth, int y, const char *str, FontDef font)
{
    int textWidth = (int)strlen(str) * font.FontWidth;
    int x = columnX + (columnWidth - textWidth) / 2;
    if(x < columnX) x = columnX;   // clamp if the string is too wide for the column

    display_.SetCursor(x, y);
    display_.WriteString(str, font, true);
}

void DisplayEngine::WriteBoldString(int x, int y, const char *str)
{
    char upper[24];
    size_t i = 0;
    for(; str[i] != '\0' && i < sizeof(upper) - 1; i++)
        upper[i] = (char)toupper((unsigned char)str[i]);
    upper[i] = '\0';

    // libDaisy's font set only has different sizes, no bold weight -- the
    // standard trick for 1-bit pixel fonts is drawing the same string twice
    // with a 1px horizontal offset, which thickens each stroke.
    display_.SetCursor(x, y);
    display_.WriteString(upper, Font_7x10, true);
    display_.SetCursor(x + 1, y);
    display_.WriteString(upper, Font_7x10, true);
}

void DisplayEngine::Init()
{
    Oled::Config cfg;
    // Defaults already target I2C1 (D11/D12 on the Pod header) and address 0x3C
    display_.Init(cfg);
}

void DisplayEngine::Update(Controls &controls, Tempo &tempo)
{
    display_.Fill(false);

    if(controls.GetBypassed())
    {
        const char *msg = "Bypassed";
        int textWidth = 8 * 7;   // "Bypassed" is 8 chars, Font_7x10 is 7px wide per char
        int x = (128 - textWidth) / 2;
        int y = (64 - 10) / 2;   // Font_7x10 is 10px tall

        display_.SetCursor(x, y);
        display_.WriteString(msg, Font_7x10, true);
        display_.Update();
        return;
    }

    char value1[16];
    char label1[16];
    char value2[16];
    char label2[16];

    switch(controls.GetMode())
    {
        case 0:  // PAGE_OVERDRIVE
        {
            Controls::Range r1 = controls.GetOverdriveDriveRange();
            Controls::Range r2 = controls.GetOverdriveLevelRange();
            //                    real value                     [ real range, live from Controls ]  [ display range -- edit freely ]
            FormatRemapped(value1, sizeof(value1), controls.GetOverdriveDrive(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Drive");
            FormatRemapped(value2, sizeof(value2), controls.GetOverdriveLevel(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Level");
            break;
        }
        case 1:  // PAGE_COMP
        {
            Controls::Range r1 = controls.GetCompThresholdRange();
            Controls::Range r2 = controls.GetCompRatioRange();
            FormatRemapped(value1, sizeof(value1), controls.GetCompThreshold(), r1.min, r1.max, 0.0f, -40.0f);
            snprintf(label1, sizeof(label1), "Threshold");
            FormatRemapped(value2, sizeof(value2), controls.GetCompRatio(), r2.min, r2.max, 10.0f, 80.0f);
            snprintf(label2, sizeof(label2), "Ratio");
            break;
        }
        case 2:  // PAGE_EQ
        {
            Controls::Range r1 = controls.GetBassGainRange();
            Controls::Range r2 = controls.GetTrebleGainRange();
            FormatRemapped(value1, sizeof(value1), controls.GetBassGain(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Bass");
            FormatRemapped(value2, sizeof(value2), controls.GetTrebleGain(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Treble");
            break;
        }
        case 3:  // PAGE_PITCH
        {
            Controls::Range r1 = controls.GetPitchMixRange();
            Controls::Range r2 = controls.GetPitchSemitonesRange();
            FormatRemapped(value1, sizeof(value1), controls.GetPitchMix(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Wet/Dry");
            FormatRemapped(value2, sizeof(value2), controls.GetPitchSemitones(), r2.min, r2.max, -12.0f, 12.0f);
            snprintf(label2, sizeof(label2), "Pitch");
            break;
        }
        case 4:  // PAGE_LFO
        {
            Controls::Range r1 = controls.GetLfoDepthRange();
            Controls::Range r2 = controls.GetLfoDivisionRange();
            FormatRemapped(value1, sizeof(value1), controls.GetLfoDepth(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Depth");
            FormatRemapped(value2, sizeof(value2), (float)controls.GetLfoDivisionIndex(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Rate");
            break;
        }
        case 5:  // PAGE_FILTER
        {
            Controls::Range r1 = controls.GetFilterDepthRange();
            Controls::Range r2 = controls.GetFilterDivisionRange();
            FormatRemapped(value1, sizeof(value1), controls.GetFilterDepth(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Depth");
            FormatRemapped(value2, sizeof(value2), (float)controls.GetFilterDivisionIndex(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Rate");
            break;
        }
        case 6:  // PAGE_CHORUS
        {
            Controls::Range r1 = controls.GetChorusDepthRange();
            Controls::Range r2 = controls.GetChorusRateRange();
            FormatRemapped(value1, sizeof(value1), controls.GetChorusDepth(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Depth");
            FormatRemapped(value2, sizeof(value2), controls.GetChorusRate(), r2.min, r2.max, 1.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Rate");
            break;
        }
        case 7:  // PAGE_DELAY1
        {
            Controls::Range r1 = controls.GetDrywetRange();
            Controls::Range r2 = controls.GetFeedbackRange();
            FormatRemapped(value1, sizeof(value1), controls.GetDrywet(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Wet/Dry");
            FormatRemapped(value2, sizeof(value2), controls.GetFeedback(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Feedback");
            break;
        }
        case 8:  // PAGE_DELAY3
        {
            static const char* noteNames[4] = { "1/4", "1/8", "1/16", "1/32" };
            static const char* modNames[3]  = { "Str8", "Dot", "Trip" };
            snprintf(value1, sizeof(value1), "%s", noteNames[controls.GetDelayNoteIndex()]);
            snprintf(label1, sizeof(label1), "Note");
            snprintf(value2, sizeof(value2), "%s", modNames[controls.GetDelayModifierIndex()]);
            snprintf(label2, sizeof(label2), "Type");
            break;
        }
        case 9:  // PAGE_DELAY2
        {
            Controls::Range r1 = controls.GetDampingRange();
            Controls::Range r2 = controls.GetSpreadRange();
            FormatRemapped(value1, sizeof(value1), controls.GetDampingAmount(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Damping");
            FormatRemapped(value2, sizeof(value2), controls.GetSpread(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Spread");
            break;
        }
        case 10: // PAGE_REVERB1
        {
            Controls::Range r1 = controls.GetReverbMixRange();
            Controls::Range r2 = controls.GetReverbFeedbackRange();
            FormatRemapped(value1, sizeof(value1), controls.GetReverbMix(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Wet/Dry");
            FormatRemapped(value2, sizeof(value2), controls.GetReverbFeedback(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "Time");
            break;
        }
        case 11: // PAGE_REVERB2
        {
            Controls::Range r1 = controls.GetReverbDampingRange();
            Controls::Range r2 = controls.GetReverbPreDelayRange();
            FormatRemapped(value1, sizeof(value1), controls.GetReverbDamping(), r1.min, r1.max, 0.0f, 10.0f);
            snprintf(label1, sizeof(label1), "Tone");
            FormatRemapped(value2, sizeof(value2), controls.GetReverbPreDelay(), r2.min, r2.max, 0.0f, 10.0f);
            snprintf(label2, sizeof(label2), "PreDly");
            break;
        }
        default:
            snprintf(value1, sizeof(value1), " ");
            snprintf(label1, sizeof(label1), " ");
            snprintf(value2, sizeof(value2), " ");
            snprintf(label2, sizeof(label2), " ");
            break;
    }

    char bpmStr[12];
    snprintf(bpmStr, sizeof(bpmStr), "%d BPM", (int)tempo.GetBpm());

    // Top row: page name on the left, BPM right-aligned, larger font
    WriteBoldString(0, 0, controls.GetModeName());

    int bpmWidth = (int)strlen(bpmStr) * Font_7x10.FontWidth;
    display_.SetCursor(128 - bpmWidth, 0);
    display_.WriteString(bpmStr, Font_7x10, true);

    // Value row -- pushed further down, closer to the bottom of the screen.
    // PAGE_DELAY3's values are short note/type words rather than numbers, so
    // the full-size digit font reads oddly large next to them -- step down
    // to the mid-size font just for that page.
    FontDef valueFont = (controls.GetMode() == 8) ? Font_11x18 : Font_16x26;
    WriteCenteredString(0, 64, 24, value1, valueFont);
    WriteCenteredString(64, 64, 24, value2, valueFont);

    // Label row -- directly beneath the values, near the bottom edge
    WriteCenteredString(0, 64, 52, label1, Font_7x10);
    WriteCenteredString(64, 64, 52, label2, Font_7x10);

    display_.Update();
}
