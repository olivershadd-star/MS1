#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

//==============================================================================
// Banque de wavetables : 8 formes d'onde ("frames"), chacune stockée en
// 10 versions limitées en bande (une par octave) pour éviter le repliement.
class WavetableBank
{
public:
    static constexpr int tableSize = 2048;
    static constexpr int numFrames = 8;
    static constexpr int numLevels = 10;

    WavetableBank();

    static int levelForFrequency (double hz) noexcept;

    // position : 0..1 (balaye les 8 formes d'onde), phase : 0..1
    float getSample (float position, int level, double phase) const noexcept;

private:
    const float* table (int frame, int level) const noexcept
    {
        return data.data() + (size_t) (frame * numLevels + level) * tableSize;
    }

    static float harmonicAmp (int frame, int harmonic);

    std::vector<float> data;
};

//==============================================================================
// Données du séquenceur pas à pas : 4 lignes (mesures) de 16 pas.
// Lues par le thread audio, modifiées par l'interface -> atomiques.
struct StepSequencerData
{
    static constexpr int stepsPerBar = 16;
    static constexpr int maxBars = 4;
    static constexpr int maxSteps = stepsPerBar * maxBars;

    StepSequencerData();

    juce::ValueTree toValueTree() const;
    void fromValueTree (const juce::ValueTree&);

    std::array<std::atomic<bool>, maxSteps> on;
    std::array<std::atomic<int>, maxSteps> note;
    std::atomic<int> revision { 0 };          // incrémenté à chaque modification (pour rafraîchir l'écran)
};

//==============================================================================
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override    { return true; }
    bool appliesToChannel (int) override { return true; }
};

//==============================================================================
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice (juce::AudioProcessorValueTreeState& state, const WavetableBank& wavetables)
        : apvts (state), bank (wavetables) {}

    void prepare (double sampleRate, int blockSize);

    bool canPlaySound (juce::SynthesiserSound* s) override
    {
        return dynamic_cast<SynthSound*> (s) != nullptr;
    }

    void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    const WavetableBank& bank;

    juce::ADSR adsr;      // enveloppe de volume
    juce::ADSR modEnv;    // enveloppe de modulation (wavetable)
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::SmoothedValue<float> position;

    double sr = 44100.0;
    static constexpr int maxUnison = 7;
    std::array<double, maxUnison> phases {};   // une phase par oscillateur d'unison
    juce::Random rng;
    double phaseInc = 0.0;
    double lfoPhase = 0.0;
    int level = 0;
    float gain = 0.0f;
};

//==============================================================================
class MonSyntheProcessor : public juce::AudioProcessor
{
public:
    MonSyntheProcessor();
    ~MonSyntheProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MS1"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    const WavetableBank& getBank() const noexcept { return bank; }

    // Séquenceur
    StepSequencerData sequencer;
    std::atomic<int> currentStep { -1 };

    // Banque de presets (fichiers .mspreset dans le dossier utilisateur)
    static juce::File getPresetDirectory();
    juce::StringArray getPresetNames() const;
    bool savePreset (const juce::String& name);
    bool loadPreset (const juce::String& name);
    bool deletePreset (const juce::String& name);

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::ValueTree makeState();
    void applyState (const juce::ValueTree&);
    void generateSequencerMidi (juce::MidiBuffer&, int numSamples);

    int seqNote = -1;                 // note actuellement tenue par le séquenceur
    juce::int64 seqNoteOffAbs = 0;    // instant (en échantillons) de son note-off
    juce::int64 sampleClock = 0;
    double freeBeat = 0.0;            // position en temps quand l'hôte ne joue pas

    WavetableBank bank;      // doit rester déclaré avant "synth"
    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonSyntheProcessor)
};
