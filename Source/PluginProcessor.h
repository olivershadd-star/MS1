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
    static constexpr int numTables = 8;         // 8 wavetables intégrées ...
    static constexpr int numFrames = 8;         // ... de 8 formes d'onde chacune
    static constexpr int numLevels = 10;
    static constexpr int numUserTables = 2;     // + 2 emplacements pour tes propres fichiers .wav
    static constexpr int totalTables = numTables + numUserTables;
    static constexpr int maxUserFrames = 64;

    WavetableBank();

    static int levelForFrequency (double hz) noexcept;
    static juce::StringArray tableNames();

    // tableIndex : 0..9, position : 0..1 (balaie les formes d'onde), phase : 0..1
    float getSample (int tableIndex, float position, int level, double phase) const noexcept;

    // Construit une table utilisateur à partir de formes d'onde de 2048 échantillons (numUserFrames à la suite).
    // À appeler depuis le thread de l'interface : la table est préparée à part puis activée d'un coup.
    void setUserTable (int slot, const std::vector<float>& frames, int numUserFrames);

private:
    size_t offsetOf (int t, int frame, int level) const noexcept
    {
        return (size_t) ((t * numFrames + frame) * numLevels + level) * tableSize;
    }

    static float harmonicAmp (int tableIndex, int frame, int harmonic);

    std::vector<float> data;

    struct UserSlot
    {
        std::array<std::vector<float>, 2> buffers;     // double tampon : on prépare l'un pendant que l'autre joue
        std::array<int, 2> frames { 0, 0 };
        std::atomic<int> active { 0 };
    };
    std::array<UserSlot, numUserTables> userSlots;
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
    static constexpr int maxUnison = 7;
    static constexpr int numMixSlots = 6;

    SynthVoice (juce::AudioProcessorValueTreeState& state, const WavetableBank& wavetables,
                std::atomic<float>& modWheelRef, std::atomic<double>& bpmRef);

    void prepare (double sampleRate, int blockSize);

    bool canPlaySound (juce::SynthesiserSound* s) override
    {
        return dynamic_cast<SynthSound*> (s) != nullptr;
    }

    void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int pitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

    // Mode mono / legato : change la note sans redéclencher les enveloppes
    void legatoTo (int midiNote, float velocity);
    bool isKeyDown() const noexcept { return keyDown; }

private:
    float param (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    juce::AudioProcessorValueTreeState& apvts;
    const WavetableBank& bank;
    std::atomic<float>& modWheel;
    std::atomic<double>& hostBpm;
    std::array<std::atomic<float>*, numMixSlots * 3> mixParams {};   // source, cible, quantité de chaque route

    juce::ADSR adsr;      // volume
    juce::ADSR modEnv;    // position de la wavetable
    juce::ADSR fltEnv;    // fréquence de coupure du filtre
    juce::dsp::StateVariableTPTFilter<float> filter, filter2;   // filter2 : 2e étage pour le LP 24 dB
    juce::SmoothedValue<float> position;

    double sr = 44100.0;
    std::array<double, maxUnison> phasesA {}, phasesB {};
    double phaseSub = 0.0;
    double lfoPhase = 0.0;
    double lfo2Phase = 0.0;
    juce::Random rng;

    double currentNote = 60.0;     // note actuelle (peut glisser vers targetNote)
    double targetNote = 60.0;
    float bendNorm = 0.0f;         // pitch bend, de -1 à +1
    float velocity = 0.0f;
    float noteRandom = 0.0f;       // valeur aléatoire tirée à chaque note (-1..1)
    bool keyDown = false;
};

//==============================================================================
// Synthé qui peut fonctionner en polyphonie ou en mono (une seule voix, legato)
class MonoCapableSynth : public juce::Synthesiser
{
public:
    void setMono (bool shouldBeMono);

    void noteOn (int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override;

private:
    bool mono = false;
    juce::Array<int> held;       // notes tenues, dans l'ordre d'appui
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
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    const WavetableBank& getBank() const noexcept { return bank; }

    // Retour visuel : derniers échantillons de sortie (les plus récents à la fin)
    void copyRecentSamples (float* dest, int numSamples) const;

    // Séquenceur
    StepSequencerData sequencer;
    std::atomic<int> currentStep { -1 };

    // Wavetables importées depuis un fichier audio (emplacements USER 1 et USER 2)
    bool loadUserWavetable (int slot, const juce::File& file, juce::String& error);
    juce::String getUserTableName (int slot) const;

    // Presets d'usine
    static int getNumFactoryPresets();
    static juce::String getFactoryPresetName (int index);
    static juce::String getFactoryPresetCategory (int index);
    void loadFactoryPreset (int index);

    // Listes de la matrice de modulation
    static juce::StringArray lfoSyncNames();
    static juce::StringArray modSourceNames();
    static juce::StringArray modTargetNames();

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
    void applyEffects (juce::AudioBuffer<float>&);

    float param (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    std::atomic<float> modWheel { 0.0f };    // molette de modulation (CC1), lue par les voix
    std::atomic<double> hostBpm { 120.0 };   // tempo de l'hôte, lu par les voix (LFO synchronisés)
    WavetableBank bank;      // doit rester déclaré avant "synth"
    MonoCapableSynth synth;

    // Effets
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::Reverb reverb;
    float lastMaster = 1.0f;

    juce::String userTableNames[WavetableBank::numUserTables];
    juce::String userTablePaths[WavetableBank::numUserTables];

    // Anneau de visualisation (lu par l'analyseur de l'interface)
    static constexpr int scopeSize = 4096;
    std::array<std::atomic<float>, scopeSize> scopeRing;
    std::atomic<int> scopeWrite { 0 };

    // Séquenceur : état interne
    int seqNote = -1;                 // note actuellement tenue par le séquenceur
    juce::int64 seqNoteOffAbs = 0;    // instant (en échantillons) de son note-off
    juce::int64 sampleClock = 0;
    double freeBeat = 0.0;            // position en temps quand l'hôte ne joue pas

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonSyntheProcessor)
};
