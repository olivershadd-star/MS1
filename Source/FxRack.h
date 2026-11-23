#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

// Rack de 4 effets en serie, place juste avant le master.
// Chaque emplacement : un type, un bypass, un mix et 4 reglages A..D (0..1) dont le sens depend du type.
struct FxSlotParams
{
    int type = 0;            // 0 = OFF
    bool on = true;
    float mix = 1.0f;
    float a = 0.5f, b = 0.5f, c = 0.5f, d = 0.5f;
};

class FxRack
{
public:
    enum Type { Off = 0, Crush, Ring, Shifter, Reso, Formant, Gate, Stutter, Shimmer, Tape, LoFi, Rotary,
                PanTrem, Shaper, Wah, Limiter, Comp, Glue, Maximizer, Exciter, Imager, NumTypes };

    static constexpr int numSlots = 4;

    FxRack();
    ~FxRack();

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    // beat : position de l'hote en temps si elle joue, sinon -1 (horloge interne)
    void process (juce::AudioBuffer<float>& buffer, const std::array<FxSlotParams, numSlots>& params,
                  double bpm, double hostBeat, float noteHz);

    // textes pour l'interface
    static juce::StringArray typeNames();
    static juce::String label (int type, int knob);             // nom du reglage A..D (knob 0..3)
    static juce::String valueText (int type, int knob, float v); // valeur lisible
    static float defaultValue (int type, int knob);
    static float defaultMix (int type);

private:
    struct Slot;
    std::vector<std::unique_ptr<Slot>> slots;
    juce::AudioBuffer<float> dry;
    double sr = 44100.0;
    double freeBeat = 0.0;
    int maxBlock = 512;
};
