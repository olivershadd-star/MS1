#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include "FxRack.h"

// Déformation de la phase d'un oscillateur ("warp") :
// 0 OFF, 1 BEND+, 2 BEND-, 3 SYNC, 4 MIRROR, 5 ASYM, 6 FLIP, 7 QUANT, 8 PWM, 9 WOBBLE
inline double warpPhase (int mode, float amount, double p) noexcept
{
    if (mode <= 0 || amount <= 0.0001f)
        return p;

    const double w = (double) amount;

    switch (mode)
    {
        case 1:                                  // BEND+ : on s'attarde au début du cycle
        {
            const double p2 = p * p;
            return w < 0.5 ? p + (p2 - p) * (2.0 * w) : p2 + (p2 * p - p2) * (2.0 * w - 1.0);
        }
        case 2:                                  // BEND- : on s'attarde à la fin du cycle
        {
            const double q = 1.0 - p, q2 = q * q;
            const double r = w < 0.5 ? q + (q2 - q) * (2.0 * w) : q2 + (q2 * q - q2) * (2.0 * w - 1.0);
            return 1.0 - r;
        }
        case 3:                                  // SYNC : le cycle se répète plus vite
        {
            const double t = p * (1.0 + 7.0 * w);
            return t - std::floor (t);
        }
        case 4:                                  // MIRROR : le cycle est replié en miroir
        {
            const double t = p * (1.0 + 3.0 * w);
            const double f = t - std::floor (t);
            return 1.0 - std::abs (2.0 * f - 1.0);
        }
        case 5:                                  // ASYM : la première moitié s'étire, la seconde se comprime
        {
            const double b = 0.5 + 0.45 * w;
            return p < 0.5 ? p * 2.0 * b : b + (p - 0.5) * 2.0 * (1.0 - b);
        }
        case 6:                                  // FLIP : la fin du cycle est lue à l'envers
        {
            const double t = 1.0 - 0.5 * w;
            return p < t ? p : 2.0 * t - p;
        }
        case 7:                                  // QUANT : la phase est quantifiée en marches (son crunchy)
        {
            const double n = 2.0 + 126.0 * (1.0 - w) * (1.0 - w);
            return std::floor (p * n) / n;
        }
        case 8:                                  // PWM : le point milieu du cycle se déplace (largeur d'impulsion)
        {
            const double d = 0.5 - 0.45 * w;
            return p < d ? p * 0.5 / d : 0.5 + (p - d) * 0.5 / (1.0 - d);
        }
        case 9:                                  // WOBBLE : la phase ondule (effet vocal / métallique)
        {
            const double t = p + 0.08 * w * std::sin (p * 4.0 * juce::MathConstants<double>::pi);
            return t - std::floor (t);
        }
        default:
            return p;
    }
}

// Distorsion : 0 SOFT, 1 HARD, 2 FOLD, 3 CRUSH, 4 ASYM. amt : 0..1
inline float distShape (int type, float amt, float x) noexcept
{
    const float g = std::exp2 (amt * 5.0f);                       // gain de 1 à 32
    switch (type)
    {
        case 0:  return std::tanh (g * x) * 0.9f;                                         // saturation douce
        case 1:  return juce::jlimit (-1.0f, 1.0f, g * x) * 0.8f;                         // écrêtage dur
        case 2:  return std::sin (g * x * 1.5707963f) * 0.9f;                             // repli d'onde
        case 3:                                                                           // bitcrusher (la réduction de fréquence est faite par la voix)
        {
            const float levels = std::exp2 (14.0f - amt * 12.0f);
            return std::round (x * levels) / levels;
        }
        default:                                                                          // asymétrique : harmoniques paires
        {
            const float b = 0.6f * amt;
            return (std::tanh (g * x + b) - std::tanh (b)) * 0.9f;
        }
    }
}

// Coefficients d'une bande d'EQ : 0 COUPE-BAS, 1 SHELF GRAVE, 2 CLOCHE, 3 SHELF AIGU, 4 COUPE-HAUT
inline juce::dsp::IIR::Coefficients<float>::Ptr makeEqCoefficients (int type, double sampleRate, float freq, float gainDb, float q)
{
    using C = juce::dsp::IIR::Coefficients<float>;
    const double f = juce::jlimit (20.0, sampleRate * 0.45, (double) freq);
    const float qq = juce::jlimit (0.1f, 10.0f, q);
    const float g = juce::Decibels::decibelsToGain (gainDb);

    switch (type)
    {
        case 0:  return C::makeHighPass (sampleRate, f, qq);
        case 1:  return C::makeLowShelf (sampleRate, f, qq, g);
        case 3:  return C::makeHighShelf (sampleRate, f, qq, g);
        case 4:  return C::makeLowPass (sampleRate, f, qq);
        default: return C::makePeakFilter (sampleRate, f, qq, g);
    }
}

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
    static constexpr int numMixSlots = 12;

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
    juce::dsp::StateVariableTPTFilter<float> f2a, f2b, f2c;      // FILTRE 2 (f2b/f2c : NOTCH et FORMANTS)
    juce::SmoothedValue<float> position;

    double sr = 44100.0;
    std::array<double, maxUnison> phasesA {}, phasesB {};
    double phaseSub = 0.0;
    double lfoPhase = 0.0;
    double lfo2Phase = 0.0;
    double lfo3Phase = 0.0;
    std::array<std::array<float, 4096>, 2> combBuf {};      // filtre en peigne (COMB)
    int combPos = 0;
    double lfo4Phase = 0.0;
    juce::Random rng;

    // Bruit, enveloppe de hauteur, distorsion
    struct NoiseGen { float b[7] {}; float lp = 0.0f; float held = 0.0f; int hold = 0; };
    std::array<NoiseGen, 2> noiseGen;
    // Corde pincee (Karplus-Strong) : 3eme source, a cote de l'OSC A et de l'OSC B
    static constexpr int strSize = 8192;
    std::array<float, strSize> strBuf {};
    int strWrite = 0;
    float strPrev = 0.0f;
    float strDelay = 100.0f;        // longueur de la boucle en echantillons (suit la hauteur)
    float strGain = 0.99f;          // gain par passage dans la boucle (donne la duree)
    float pitchEnvVal = 1.0f;       // 1 -> 0 (décroissance exponentielle depuis le début de la note)
    float noiseEnvVal = 1.0f;
    std::array<float, 2> crushHeld {};
    int crushCount = 0;

    float nextNoise (NoiseGen& n, int type, float tone);

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

    // Pour l'interface : couleur du thème et voyants d'activité
    float getThemeHue() const noexcept            { return themeHue.load(); }
    void setThemeHue (float hue, bool save);
    int getActiveVoiceMask() const noexcept       { return activeVoiceMask.load(); }
    double getHostPpq() const noexcept            { return hostPpq.load(); }
    bool isHostPlaying() const noexcept           { return hostPlaying.load(); }
    double getHostBpm() const noexcept            { return hostBpm.load(); }

    // Retour visuel : derniers échantillons de sortie (les plus récents à la fin)
    void copyRecentSamples (float* dest, int numSamples) const;

    // Séquenceur
    StepSequencerData sequencer;
    juce::MidiKeyboardState keyboardState;     // clavier a l'ecran : notes jouees a la souris, et MIDI recu (touches allumees)
    std::atomic<int> currentStep { -1 };

    // Wavetables importées depuis un fichier audio (emplacements USER 1 et USER 2)
    bool loadUserWavetable (int slot, const juce::File& file, juce::String& error);

    // Resample : capture ~3 s de la sortie des voix (avant effets), puis la transforme en wavetable USER
    void startResample();
    int getResampleState() const noexcept      { return capState.load(); }      // 0 repos, 1 capture, 2 prêt
    float getResampleProgress() const noexcept { return capBuf.empty() ? 0.0f : (float) capPos.load() / (float) capBuf.size(); }
    bool finishResample (int slot, juce::String& error);
    juce::String getUserTableName (int slot) const;

    // Presets d'usine
    static int getNumFactoryPresets();
    static juce::String getFactoryPresetName (int index);
    static juce::String getFactoryPresetCategory (int index);
    void loadFactoryPreset (int index);

    // Listes de la matrice de modulation
    static juce::StringArray warpModeNames();
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
    std::atomic<double> hostPpq { 0.0 };     // position de lecture de l'hôte, en temps
    std::atomic<bool> hostPlaying { false };
    std::atomic<int> activeVoiceMask { 0 };  // voix en cours de jeu (un bit par voix)
    std::atomic<float> themeHue { 0.52f };   // couleur centrale de l'interface
    WavetableBank bank;      // doit rester déclaré avant "synth"
    MonoCapableSynth synth;

    // Effets
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::Reverb reverb;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelay { 48000 };
    juce::AudioBuffer<float> reverbBuf;
    float dlLp[2] = { 0.0f, 0.0f }, dlHp[2] = { 0.0f, 0.0f };      // filtres du feedback de delay
    float rvLp[2] = { 0.0f, 0.0f }, rvHp[2] = { 0.0f, 0.0f };      // filtres de la reverb
    juce::dsp::Compressor<float> compressor;     // page MASTER
    juce::dsp::Limiter<float> limiter;
    juce::AudioBuffer<float> dryBuffer;          // copie du signal pour le mix parallèle du compresseur
    float lastMaster = 1.0f;

    // EQ 8 bandes (page EQ)
    struct EqBand
    {
        std::array<juce::dsp::IIR::Filter<float>, 2> f;
        float lastFreq = -1.0f, lastGain = -99.0f, lastQ = -1.0f;
        int lastType = -1;
    };
    std::array<EqBand, 8> eqBands;
    std::array<std::atomic<float>*, 40> eqParams {};      // par bande : on, type, freq, gain, Q
    std::atomic<float>* eqOutParam = nullptr;
    float lastEqOut = 1.0f;

    // Rack d'effets (4 emplacements, juste avant le master)
    FxRack fxRack;
    std::array<std::atomic<float>*, 4 * 7> fxParams {};   // par emplacement : type, on, mix, a, b, c, d
    std::atomic<float> lastNoteHz { 0.0f };
    std::vector<float> capBuf;
    std::atomic<int> capPos { 0 }, capState { 0 };
    double capSr = 44100.0;

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
