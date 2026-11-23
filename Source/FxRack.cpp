#include "FxRack.h"
#include <cmath>

namespace
{
constexpr float kPi = juce::MathConstants<float>::pi;
constexpr float kTwoPi = juce::MathConstants<float>::twoPi;

inline float dbToGain (float db) { return std::pow (10.0f, db * 0.05f); }
inline float gainToDb (float g) { return 20.0f * std::log10 (juce::jmax (1.0e-6f, g)); }
inline float onePoleCoef (float hz, double sr) { return 1.0f - std::exp (-kTwoPi * juce::jlimit (1.0f, (float) (sr * 0.45), hz) / (float) sr); }
inline float expMap (float v, float lo, float hi) { return lo * std::pow (hi / lo, v); }

const float divBeats[] = { 1.0f, 0.5f, 0.25f, 0.125f, 1.0f / 3.0f, 1.0f / 6.0f };
const char* const divNames[] = { "1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T" };

const juce::uint16 gatePatterns[8] = { 0xFFFF, 0xAAAA, 0xEEEE, 0x9292, 0xDADA, 0x8888, 0xF0F0, 0xBBBB };

// Voyelles : formants F1, F2, F3 (A E I O U)
const float vowels[5][3] = { { 800.0f, 1150.0f, 2900.0f }, { 400.0f, 1600.0f, 2700.0f }, { 350.0f, 1700.0f, 2700.0f },
                             { 450.0f, 800.0f, 2830.0f }, { 325.0f, 700.0f, 2700.0f } };
const char* const vowelNames[] = { "A", "E", "I", "O", "U" };

// Deux chaines d'allpass qui donnent 90 degres de dephasage (transformee de Hilbert), pour le decaleur de frequence
const float hilbertA1[4] = { 0.6923878f, 0.9360654322959f, 0.9882295226860f, 0.9987488452737f };
const float hilbertA2[4] = { 0.4021921162426f, 0.8561710882420f, 0.9722909545651f, 0.9952884791278f };

struct Allpass2
{
    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    float process (float coef, float x)
    {
        const float c = coef * coef;
        const float y = c * (x + y2) - x2;
        x2 = x1; x1 = x; y2 = y1; y1 = y;
        return y;
    }
};

// Compresseur de bus (doux ou dur), avec filtre de detection et release automatique optionnel
struct BusComp
{
    float envFast = 0.0f, envSlow = 0.0f, hp = 0.0f;
    void reset() { envFast = envSlow = hp = 0.0f; }

    void process (float* l, float* r, int n, double sr, float thr, float ratio, float attMs, float relMs,
                  float kneeDb, float scHz, bool autoRelease, float makeupDb)
    {
        const float aA = 1.0f - std::exp (-1.0f / juce::jmax (1.0f, attMs * 0.001f * (float) sr));
        const float aR = 1.0f - std::exp (-1.0f / juce::jmax (1.0f, relMs * 0.001f * (float) sr));
        const float aS = 1.0f - std::exp (-1.0f / juce::jmax (1.0f, relMs * 4.0f * 0.001f * (float) sr));
        const float hpA = onePoleCoef (scHz, sr);
        const float makeup = dbToGain (makeupDb);

        for (int i = 0; i < n; ++i)
        {
            const float det = juce::jmax (std::abs (l[i]), std::abs (r[i]));
            hp += hpA * (det - hp);
            const float sc = juce::jmax (det - hp, 0.0f);       // filtre passe-haut simple sur l'enveloppe
            const float lvl = gainToDb (juce::jmax (sc, det * 0.25f));
            const float over = lvl - thr;
            float gr = 0.0f;
            if (2.0f * over > kneeDb)
                gr = (1.0f - 1.0f / ratio) * over;
            else if (2.0f * over > -kneeDb)
            {
                const float t = over + kneeDb * 0.5f;
                gr = (1.0f - 1.0f / ratio) * t * t / (2.0f * juce::jmax (0.01f, kneeDb));
            }

            envFast += (gr > envFast ? aA : aR) * (gr - envFast);
            envSlow += (gr > envSlow ? aA : aS) * (gr - envSlow);
            const float env = autoRelease ? 0.5f * (envFast + envSlow) : envFast;
            const float g = dbToGain (-env) * makeup;
            l[i] *= g;
            r[i] *= g;
        }
    }
};
} // namespace

//==============================================================================
struct FxRack::Slot
{
    int lastType = -1;
    juce::Random rng { 1234 };

    // BITCRUSH
    float held[2] = { 0, 0 }, crushLp[2] = { 0, 0 };
    float crushCnt = 0.0f;
    // RING / SHIFTER / ROTARY / PANTREM
    double ringPhase = 0.0, ringLfo = 0.0;
    double shiftPhase[2] = { 0, 0 };
    Allpass2 hilA[2][4], hilB[2][4];
    float hilDelay[2] = { 0, 0 };
    float rotSpeed = 0.5f;
    double rotPhase = 0.0;
    double panPhase = 0.0;
    // lignes a retard (comb, shimmer, tape, rotary)
    std::vector<float> delayBuf[2];
    int dW = 0;
    float dLp[2] = { 0, 0 };
    double tapPhase = 0.0, wowPhase = 0.0, flutPhase = 0.0;
    // formants
    juce::dsp::IIR::Filter<float> formant[2][3];
    double vowelLfo = 0.0;
    float formLastQ = -1.0f;
    // gate
    float gateGain = 1.0f;
    // stutter
    std::vector<float> rec[2];
    int recW = 0;
    juce::int64 lastStep = -1;
    bool stutActive = false;
    int stutStart = 0, stutLen = 1, stutPos = 0;
    float stutEngage = 0.0f;
    // lofi
    float loHp[2] = { 0, 0 }, loLp1[2] = { 0, 0 }, loLp2[2] = { 0, 0 }, crackle = 0.0f;
    // shaper / dc
    float dcX[2] = { 0, 0 }, dcY[2] = { 0, 0 };
    // wah
    juce::dsp::StateVariableTPTFilter<float> wah[2];
    float wahEnv = 0.0f;
    // dynamique
    juce::dsp::Limiter<float> limiter;
    BusComp comp;
    // exciter
    juce::dsp::IIR::Filter<float> excHp[2];
    float excLow[2] = { 0, 0 };
    float excLastFreq = -1.0f;
    // imager
    float imLp1[2] = { 0, 0 }, imLp2[2] = { 0, 0 };

    void allocate (int maxDelay, int maxRec)
    {
        for (auto& b : delayBuf)
            b.assign ((size_t) maxDelay, 0.0f);
        for (auto& b : rec)
            b.assign ((size_t) maxRec, 0.0f);
    }

    void clear()
    {
        for (auto& b : delayBuf) std::fill (b.begin(), b.end(), 0.0f);
        for (auto& b : rec) std::fill (b.begin(), b.end(), 0.0f);
        dW = recW = 0;
        for (int c = 0; c < 2; ++c)
        {
            held[c] = crushLp[c] = dLp[c] = loHp[c] = loLp1[c] = loLp2[c] = dcX[c] = dcY[c] = excLow[c] = imLp1[c] = imLp2[c] = 0.0f;
            hilDelay[c] = 0.0f;
            shiftPhase[c] = 0.0;
            for (auto& s : hilA[c]) s = Allpass2();
            for (auto& s : hilB[c]) s = Allpass2();
            for (auto& f : formant[c]) f.reset();
            wah[c].reset();
            excHp[c].reset();
        }
        crushCnt = 0.0f;
        ringPhase = ringLfo = rotPhase = panPhase = tapPhase = wowPhase = flutPhase = vowelLfo = 0.0;
        gateGain = 1.0f;
        lastStep = -1;
        stutActive = false;
        stutEngage = 0.0f;
        wahEnv = 0.0f;
        crackle = 0.0f;
        formLastQ = -1.0f;
        excLastFreq = -1.0f;
        limiter.reset();
        comp.reset();
    }
};

//==============================================================================
FxRack::FxRack()
{
    for (int i = 0; i < numSlots; ++i)
        slots.push_back (std::make_unique<Slot>());
}

FxRack::~FxRack() = default;

void FxRack::prepare (double sampleRate, int maxBlockSize)
{
    sr = sampleRate > 0.0 ? sampleRate : 44100.0;
    maxBlock = juce::jmax (64, maxBlockSize);
    dry.setSize (2, maxBlock);

    const int maxDelay = 1 << (sr > 100000.0 ? 15 : 14);          // ~340 ms
    const int maxRec = 1 << (sr > 100000.0 ? 18 : 17);            // ~2,7 s
    juce::dsp::ProcessSpec spec { sr, (juce::uint32) maxBlock, 2 };

    for (auto& s : slots)
    {
        s->allocate (maxDelay, maxRec);
        s->limiter.prepare (spec);
        for (int c = 0; c < 2; ++c)
        {
            s->wah[c].prepare (spec);
            s->wah[c].setType (juce::dsp::StateVariableTPTFilterType::bandpass);
        }
        s->clear();
        s->lastType = -1;
    }
}

void FxRack::reset()
{
    for (auto& s : slots)
    {
        s->clear();
        s->lastType = -1;
    }
}

//==============================================================================
// Textes
//==============================================================================
juce::StringArray FxRack::typeNames()
{
    return { "OFF", "BITCRUSH", "RING MOD", "DECALEUR FREQ", "RESONATEUR", "FORMANTS", "GATE RYTHMIQUE", "STUTTER", "SHIMMER",
             "BANDE MAGNETIQUE", "LO-FI", "ROTARY", "PAN / TREMOLO", "WAVESHAPER", "AUTO-WAH", "LIMITEUR", "COMPRESSEUR",
             "GLUE", "MAXIMIZER", "EXCITER", "IMAGER" };
}

juce::String FxRack::label (int type, int k)
{
    static const char* const l[NumTypes][4] = {
        { "-", "-", "-", "-" },
        { "BITS", "ECHANT.", "GAIN IN", "FILTRE" },
        { "FREQ", "FORME", "DERIVE", "VIT.DER." },
        { "DECALAGE", "FIN", "STEREO", "-" },
        { "FREQ", "FEEDBACK", "AMORTI", "SUIT NOTE" },
        { "VOYELLE", "RESO", "TAILLE", "MOUVEMENT" },
        { "DIVISION", "PROFONDEUR", "LISSAGE", "MOTIF" },
        { "DIVISION", "REPETITION", "PROBA", "INVERSE" },
        { "TRANSPOSE", "FEEDBACK", "TAILLE", "BRILLANCE" },
        { "WOW", "FLUTTER", "SATURATION", "TON" },
        { "BANDE", "CRACKLE", "CRUSH", "SOUFFLE" },
        { "VITESSE", "PROFONDEUR", "ECARTEMENT", "DRIVE" },
        { "VITESSE", "PROFONDEUR", "FORME", "MODE" },
        { "DRIVE", "TYPE", "BIAIS", "SORTIE" },
        { "SENSIBIL.", "FREQ", "RESO", "PLAGE" },
        { "PLAFOND", "GAIN IN", "RELEASE", "-" },
        { "SEUIL", "RATIO", "ATTAQUE", "RELEASE" },
        { "SEUIL", "RATIO", "ATTAQUE", "RELEASE" },
        { "DRIVE", "PLAFOND", "RELEASE", "SOFT CLIP" },
        { "DRIVE", "FREQ", "AIR", "CHALEUR" },
        { "LARGEUR", "BASSES MONO", "MILIEU", "BALANCE" } };
    return l[juce::jlimit (0, (int) NumTypes - 1, type)][juce::jlimit (0, 3, k)];
}

float FxRack::defaultMix (int type)
{
    switch (type)
    {
        case Limiter: case Comp: case Glue: case Maximizer: case Imager: return 1.0f;
        case Off: return 1.0f;
        default: return 0.5f;
    }
}

float FxRack::defaultValue (int type, int k)
{
    static const float d[NumTypes][4] = {
        { 0.5f, 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.3f, 0.2f, 0.3f },          // crush
        { 0.45f, 0.2f, 0.0f, 0.3f },         // ring
        { 0.55f, 0.5f, 0.0f, 0.0f },         // shifter
        { 0.35f, 0.7f, 0.4f, 0.0f },         // reso
        { 0.2f, 0.6f, 0.5f, 0.3f },          // formants
        { 0.4f, 0.8f, 0.2f, 0.2f },          // gate
        { 0.4f, 0.3f, 0.6f, 0.0f },          // stutter
        { 0.75f, 0.5f, 0.5f, 0.6f },         // shimmer (+12)
        { 0.4f, 0.3f, 0.4f, 0.7f },          // tape
        { 0.4f, 0.3f, 0.3f, 0.2f },          // lofi
        { 0.4f, 0.6f, 0.4f, 0.1f },          // rotary
        { 0.3f, 0.6f, 0.0f, 0.0f },          // pan/trem
        { 0.3f, 0.0f, 0.5f, 0.5f },          // shaper
        { 0.5f, 0.3f, 0.5f, 0.5f },          // wah
        { 0.9f, 0.3f, 0.3f, 0.0f },          // limiter
        { 0.45f, 0.3f, 0.3f, 0.4f },         // comp
        { 0.45f, 0.2f, 0.4f, 1.0f },         // glue
        { 0.4f, 0.95f, 0.4f, 0.4f },         // maximizer
        { 0.4f, 0.4f, 0.4f, 0.2f },          // exciter
        { 0.5f, 0.0f, 0.5f, 0.5f } };        // imager
    return d[juce::jlimit (0, (int) NumTypes - 1, type)][juce::jlimit (0, 3, k)];
}

juce::String FxRack::valueText (int type, int k, float v)
{
    auto pct = [] (float x) { return juce::String (juce::roundToInt (x * 100.0f)) + "%"; };
    auto hz = [] (float f) { return f >= 1000.0f ? juce::String (f / 1000.0f, 1) + "k" : juce::String (juce::roundToInt (f)) + "Hz"; };
    auto ms = [] (float t) { return t >= 1000.0f ? juce::String (t / 1000.0f, 1) + "s" : juce::String (juce::roundToInt (t)) + "ms"; };
    auto db = [] (float x) { return juce::String (x, 1) + "dB"; };

    switch (type)
    {
        case Crush:
            if (k == 0) return juce::String (juce::roundToInt (16.0f - 14.0f * v)) + " bits";
            if (k == 1) return "/" + juce::String (juce::roundToInt (1.0f + 63.0f * v));
            if (k == 2) return db (v * 18.0f);
            return hz (20000.0f * std::pow (0.05f, v));
        case Ring:
            if (k == 0) return hz (expMap (v, 20.0f, 8000.0f));
            if (k == 1) return v < 0.1f ? "SINUS" : (v > 0.9f ? "CARRE" : pct (v));
            if (k == 2) return pct (v);
            return juce::String (0.1f + 9.9f * v, 1) + "Hz";
        case Shifter:
            if (k == 0) return juce::String (juce::roundToInt ((v - 0.5f) * 2000.0f)) + "Hz";
            if (k == 1) return juce::String ((v - 0.5f) * 100.0f, 1) + "Hz";
            if (k == 2) return pct (v);
            return "-";
        case Reso:
            if (k == 0) return hz (expMap (v, 40.0f, 4000.0f));
            if (k == 1 || k == 2) return pct (v);
            return v > 0.5f ? "OUI" : "NON";
        case Formant:
            if (k == 0) { const float p = v * 4.0f; return juce::String (vowelNames[juce::jlimit (0, 4, juce::roundToInt (p))]); }
            if (k == 1) return juce::String (3.0f + 17.0f * v, 1);
            if (k == 2) return juce::String (0.7f + 0.7f * v, 2) + "x";
            return v < 0.03f ? "FIXE" : juce::String (v * 4.0f, 1) + "Hz";
        case Gate:
            if (k == 0) return divNames[juce::jlimit (0, 5, (int) (v * 5.999f))];
            if (k == 1 || k == 2) return k == 1 ? pct (v) : ms (0.5f + v * 40.0f);
            return "#" + juce::String (1 + juce::jlimit (0, 7, (int) (v * 7.999f)));
        case Stutter:
        {
            static const char* const rep[] = { "1", "1/2", "1/4", "1/8" };
            if (k == 0) return divNames[juce::jlimit (0, 3, (int) (v * 3.999f))];
            if (k == 1) return rep[juce::jlimit (0, 3, (int) (v * 3.999f))];
            if (k == 2) return pct (v);
            return v > 0.5f ? "OUI" : "NON";
        }
        case Shimmer:
            if (k == 0) return juce::String (juce::roundToInt ((v - 0.5f) * 24.0f)) + " st";
            if (k == 1) return pct (v * 0.85f);
            if (k == 2) return ms (20.0f + 80.0f * v);
            return hz (expMap (v, 1500.0f, 16000.0f));
        case Tape:
            if (k == 3) return hz (expMap (v, 3000.0f, 18000.0f));
            return pct (v);
        case LoFi:
            return pct (v);
        case Rotary:
            if (k == 0) return juce::String (0.4f + 7.0f * v, 1) + "Hz";
            return pct (v);
        case PanTrem:
            if (k == 0) return juce::String (0.1f * std::pow (160.0f, v), 1) + "Hz";
            if (k == 3) return v < 0.5f ? "PAN" : "TREMOLO";
            return pct (v);
        case Shaper:
            if (k == 0) return db (v * 30.0f);
            if (k == 1) { static const char* const t[] = { "TANH", "FOLD", "SINUS", "ASYM", "DUR" }; return t[juce::jlimit (0, 4, (int) (v * 4.999f))]; }
            if (k == 2) return juce::String ((v - 0.5f) * 2.0f, 2);
            return db ((v - 0.5f) * 24.0f);
        case Wah:
            if (k == 1) return hz (expMap (v, 100.0f, 2000.0f));
            if (k == 2) return juce::String (1.0f + 11.0f * v, 1);
            if (k == 3) return juce::String (0.5f + 4.5f * v, 1) + " oct";
            return pct (v);
        case Limiter:
            if (k == 0) return db (-12.0f + 12.0f * v);
            if (k == 1) return db (v * 18.0f);
            if (k == 2) return ms (expMap (v, 10.0f, 500.0f));
            return "-";
        case Comp:
        case Glue:
        {
            if (k == 0) return db (-40.0f + 40.0f * v);
            if (type == Comp)
            {
                if (k == 1) return juce::String (1.0f + 19.0f * v * v, 1) + ":1";
                if (k == 2) return ms (expMap (v, 0.1f, 100.0f));
                return ms (expMap (v, 10.0f, 1000.0f));
            }
            static const float att[] = { 0.1f, 0.3f, 1.0f, 3.0f, 10.0f, 30.0f };
            if (k == 1) return juce::String (1.5f + 6.5f * v, 1) + ":1";
            if (k == 2) return ms (att[juce::jlimit (0, 5, (int) (v * 5.999f))]);
            return v > 0.8f ? "AUTO" : ms (100.0f + v * 1500.0f);
        }
        case Maximizer:
            if (k == 0) return db (v * 20.0f);
            if (k == 1) return db (-6.0f + 6.0f * v);
            if (k == 2) return ms (expMap (v, 10.0f, 300.0f));
            return pct (v);
        case Exciter:
            if (k == 1) return hz (expMap (v, 1500.0f, 9000.0f));
            return pct (v);
        case Imager:
            if (k == 0) return pct (v * 2.0f);
            if (k == 1) return v < 0.03f ? "OFF" : hz (40.0f + 260.0f * v);
            if (k == 2) return db ((v - 0.5f) * 12.0f);
            return juce::String (juce::roundToInt ((v - 0.5f) * 200.0f));
        default: break;
    }
    return pct (v);
}

//==============================================================================
// Traitement
//==============================================================================
void FxRack::process (juce::AudioBuffer<float>& buffer, const std::array<FxSlotParams, numSlots>& params,
                      double bpm, double hostBeat, float noteHz)
{
    const int n = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || n <= 0 || n > dry.getNumSamples())
        return;

    const double spb = 60.0 / juce::jmax (20.0, bpm) * sr;          // echantillons par temps
    const double beat0 = hostBeat >= 0.0 ? hostBeat : freeBeat;
    if (hostBeat < 0.0)
        freeBeat += (double) n / spb;

    float* L = buffer.getWritePointer (0);
    float* R = buffer.getWritePointer (1);

    for (int si = 0; si < numSlots; ++si)
    {
        const auto& p = params[(size_t) si];
        auto& s = *slots[(size_t) si];

        if (p.type != s.lastType)
        {
            s.clear();
            s.lastType = p.type;
        }
        if (p.type == Off || ! p.on)
            continue;

        std::memcpy (dry.getWritePointer (0), L, sizeof (float) * (size_t) n);
        std::memcpy (dry.getWritePointer (1), R, sizeof (float) * (size_t) n);

        const float a = p.a, b = p.b, c = p.c, d = p.d;
        const int dsize = (int) s.delayBuf[0].size();
        const int dmask = dsize - 1;

        switch (p.type)
        {
            //------------------------------------------------------------------ BITCRUSH
            case Crush:
            {
                const float levels = std::pow (2.0f, 15.0f - 14.0f * a);
                const float factor = 1.0f + 63.0f * b;
                const float pre = dbToGain (c * 18.0f);
                const float lpA = onePoleCoef (20000.0f * std::pow (0.05f, d), sr);
                for (int i = 0; i < n; ++i)
                {
                    s.crushCnt += 1.0f;
                    const bool grab = s.crushCnt >= factor;
                    if (grab) s.crushCnt -= factor;
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        if (grab)
                            s.held[ch] = std::round (juce::jlimit (-1.0f, 1.0f, x[i] * pre) * levels) / levels;
                        s.crushLp[ch] += lpA * (s.held[ch] - s.crushLp[ch]);
                        x[i] = s.crushLp[ch];
                    }
                }
                break;
            }

            //------------------------------------------------------------------ RING MOD
            case Ring:
            {
                const float f0 = expMap (a, 20.0f, 8000.0f);
                const double lfoInc = (0.1 + 9.9 * (double) d) / sr;
                for (int i = 0; i < n; ++i)
                {
                    const float f = f0 * (1.0f + 0.5f * c * std::sin ((float) (s.ringLfo * (double) kTwoPi)));
                    s.ringLfo += lfoInc;
                    if (s.ringLfo >= 1.0) s.ringLfo -= 1.0;
                    const float sn = std::sin ((float) (s.ringPhase * (double) kTwoPi));
                    const float car = (1.0f - b) * sn + b * std::tanh (5.0f * sn);
                    s.ringPhase += (double) f / sr;
                    if (s.ringPhase >= 1.0) s.ringPhase -= 1.0;
                    L[i] *= car;
                    R[i] *= car;
                }
                break;
            }

            //------------------------------------------------------------------ DECALEUR DE FREQUENCE
            case Shifter:
            {
                const float shift = (a - 0.5f) * 2000.0f + (b - 0.5f) * 100.0f;
                const float shiftR = shift * (1.0f - 2.0f * c);
                for (int i = 0; i < n; ++i)
                {
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        float p1 = x[i], p2 = x[i];
                        for (int k = 0; k < 4; ++k)
                        {
                            p1 = s.hilA[ch][k].process (hilbertA1[k], p1);
                            p2 = s.hilB[ch][k].process (hilbertA2[k], p2);
                        }
                        const float real = p2;                       // chemin 2
                        const float imag = s.hilDelay[ch];           // chemin 1 retarde d'un echantillon
                        s.hilDelay[ch] = p1;
                        const float ph = (float) (s.shiftPhase[ch] * (double) kTwoPi);
                        x[i] = real * std::cos (ph) - imag * std::sin (ph);
                        s.shiftPhase[ch] += (double) (ch == 0 ? shift : shiftR) / sr;
                        s.shiftPhase[ch] -= std::floor (s.shiftPhase[ch]);
                    }
                }
                break;
            }

            //------------------------------------------------------------------ RESONATEUR (peigne accorde)
            case Reso:
            {
                float f = expMap (a, 40.0f, 4000.0f);
                if (d > 0.5f && noteHz > 20.0f)
                    f = noteHz;
                const float delay = juce::jlimit (2.0f, (float) (dsize - 4), (float) sr / f - 0.5f);
                const float fb = b * 0.97f;
                const float lpC = 1.0f - 0.9f * c;
                const float outG = 1.0f - 0.4f * fb;
                for (int i = 0; i < n; ++i)
                {
                    const float rp = (float) s.dW - delay;
                    const int i0 = (int) std::floor (rp);
                    const float fr = rp - (float) i0;
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        auto& buf = s.delayBuf[ch];
                        const float del = buf[(size_t) (i0 & dmask)] * (1.0f - fr) + buf[(size_t) ((i0 + 1) & dmask)] * fr;
                        s.dLp[ch] += lpC * (del - s.dLp[ch]);
                        const float y = x[i] + fb * s.dLp[ch];
                        buf[(size_t) (s.dW & dmask)] = juce::jlimit (-4.0f, 4.0f, y);
                        x[i] = y * outG;
                    }
                    ++s.dW;
                }
                break;
            }

            //------------------------------------------------------------------ FORMANTS (voyelles)
            case Formant:
            {
                const float q = 3.0f + 17.0f * b;
                const float scale = 0.7f + 0.7f * c;
                float pos = a * 4.0f;
                if (d > 0.03f)
                {
                    pos = 2.0f + 2.0f * std::sin ((float) (s.vowelLfo * (double) kTwoPi));
                    s.vowelLfo += (double) n * (double) d * 4.0 / sr;
                    s.vowelLfo -= std::floor (s.vowelLfo);
                }
                const int i0 = juce::jlimit (0, 3, (int) pos);
                const float fr = juce::jlimit (0.0f, 1.0f, pos - (float) i0);
                for (int k = 0; k < 3; ++k)
                {
                    const float f = (vowels[i0][k] * (1.0f - fr) + vowels[i0 + 1][k] * fr) * scale;
                    const auto co = juce::dsp::IIR::Coefficients<float>::makeBandPass (sr, juce::jlimit (60.0f, (float) (sr * 0.45), f), q);
                    for (int ch = 0; ch < 2; ++ch)
                        s.formant[ch][k].coefficients = co;
                }
                const float gains[3] = { 1.6f, 1.2f, 0.8f };
                for (int ch = 0; ch < 2; ++ch)
                {
                    float* x = ch == 0 ? L : R;
                    for (int i = 0; i < n; ++i)
                    {
                        const float in = x[i];
                        float y = 0.0f;
                        for (int k = 0; k < 3; ++k)
                            y += gains[k] * s.formant[ch][k].processSample (in);
                        x[i] = y;
                    }
                }
                break;
            }

            //------------------------------------------------------------------ GATE RYTHMIQUE
            case Gate:
            {
                const float step = divBeats[juce::jlimit (0, 5, (int) (a * 5.999f))];
                const juce::uint16 pattern = gatePatterns[juce::jlimit (0, 7, (int) (d * 7.999f))];
                const float depth = b;
                const float smooth = 1.0f - std::exp (-1.0f / juce::jmax (1.0f, (0.0005f + c * 0.04f) * (float) sr));
                for (int i = 0; i < n; ++i)
                {
                    const double beat = beat0 + (double) i / spb;
                    const auto idx = (juce::int64) std::floor (beat / (double) step);
                    const bool open = (pattern >> (int) (((idx % 16) + 16) % 16)) & 1;
                    const float target = open ? 1.0f : 1.0f - depth;
                    s.gateGain += smooth * (target - s.gateGain);
                    L[i] *= s.gateGain;
                    R[i] *= s.gateGain;
                }
                break;
            }

            //------------------------------------------------------------------ STUTTER
            case Stutter:
            {
                static const float steps[] = { 1.0f, 0.5f, 0.25f, 0.125f };
                static const float reps[] = { 1.0f, 0.5f, 0.25f, 0.125f };
                const float step = steps[juce::jlimit (0, 3, (int) (a * 3.999f))];
                const float rep = reps[juce::jlimit (0, 3, (int) (b * 3.999f))];
                const int recSize = (int) s.rec[0].size();
                const int recMask = recSize - 1;
                const bool reverse = d > 0.5f;
                const float engageC = 1.0f - std::exp (-1.0f / (0.002f * (float) sr));

                for (int i = 0; i < n; ++i)
                {
                    s.rec[0][(size_t) (s.recW & recMask)] = L[i];
                    s.rec[1][(size_t) (s.recW & recMask)] = R[i];

                    const double beat = beat0 + (double) i / spb;
                    const auto idx = (juce::int64) std::floor (beat / (double) step);
                    if (idx != s.lastStep)
                    {
                        s.lastStep = idx;
                        s.stutActive = s.rng.nextFloat() < c;
                        s.stutLen = juce::jlimit (64, recSize / 2, (int) (spb * (double) step * (double) rep));
                        s.stutStart = s.recW - s.stutLen;
                        s.stutPos = 0;
                    }

                    s.stutEngage += engageC * ((s.stutActive ? 1.0f : 0.0f) - s.stutEngage);
                    if (s.stutEngage > 0.001f)
                    {
                        const int off = reverse ? (s.stutLen - 1 - s.stutPos) : s.stutPos;
                        const int ri = (s.stutStart + off) & recMask;
                        float fade = 1.0f;
                        const int edge = 48;
                        if (s.stutPos < edge) fade = (float) s.stutPos / (float) edge;
                        else if (s.stutPos > s.stutLen - edge) fade = (float) (s.stutLen - s.stutPos) / (float) edge;
                        const float rl = s.rec[0][(size_t) ri] * fade;
                        const float rr = s.rec[1][(size_t) ri] * fade;
                        L[i] = L[i] * (1.0f - s.stutEngage) + rl * s.stutEngage;
                        R[i] = R[i] * (1.0f - s.stutEngage) + rr * s.stutEngage;
                        if (++s.stutPos >= s.stutLen)
                            s.stutPos = 0;
                    }
                    ++s.recW;
                }
                break;
            }

            //------------------------------------------------------------------ SHIMMER (transposition + feedback)
            case Shimmer:
            {
                const float ratio = std::pow (2.0f, (a - 0.5f) * 2.0f);
                const float win = juce::jlimit (64.0f, (float) (dsize / 2 - 8), (0.02f + 0.08f * c) * (float) sr);
                const float fb = b * 0.85f;
                const float lpC = onePoleCoef (expMap (d, 1500.0f, 16000.0f), sr);
                const double inc = (double) (1.0f - ratio) / (double) win;
                for (int i = 0; i < n; ++i)
                {
                    const float ph1 = (float) s.tapPhase;
                    float ph2 = ph1 + 0.5f;
                    if (ph2 >= 1.0f) ph2 -= 1.0f;
                    const float d1 = 8.0f + ph1 * win, d2 = 8.0f + ph2 * win;
                    const float g1 = 1.0f - std::abs (2.0f * ph1 - 1.0f), g2 = 1.0f - std::abs (2.0f * ph2 - 1.0f);

                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        auto& buf = s.delayBuf[ch];
                        auto tap = [&] (float dl)
                        {
                            const float rp = (float) s.dW - dl;
                            const int i0 = (int) std::floor (rp);
                            const float fr = rp - (float) i0;
                            return buf[(size_t) (i0 & dmask)] * (1.0f - fr) + buf[(size_t) ((i0 + 1) & dmask)] * fr;
                        };
                        const float out = tap (d1) * g1 + tap (d2) * g2;
                        s.dLp[ch] += lpC * (out - s.dLp[ch]);
                        buf[(size_t) (s.dW & dmask)] = juce::jlimit (-4.0f, 4.0f, x[i] + fb * s.dLp[ch]);
                        x[i] = out;
                    }
                    ++s.dW;
                    s.tapPhase += inc;
                    s.tapPhase -= std::floor (s.tapPhase);
                }
                break;
            }

            //------------------------------------------------------------------ BANDE MAGNETIQUE
            case Tape:
            {
                const float wowDepth = a * 0.006f * (float) sr;       // jusqu'a 6 ms
                const float flutDepth = b * 0.0006f * (float) sr;      // jusqu'a 0,6 ms
                const float base = 0.008f * (float) sr;
                const float drive = 1.0f + c * 6.0f;
                const float norm = 1.0f / std::tanh (drive);
                const float lpC = onePoleCoef (expMap (d, 3000.0f, 18000.0f), sr);
                for (int i = 0; i < n; ++i)
                {
                    const float mod = std::sin ((float) (s.wowPhase * (double) kTwoPi)) * wowDepth
                                    + std::sin ((float) (s.flutPhase * (double) kTwoPi)) * flutDepth;
                    s.wowPhase += 0.55 / sr;  if (s.wowPhase >= 1.0) s.wowPhase -= 1.0;
                    s.flutPhase += 7.3 / sr;  if (s.flutPhase >= 1.0) s.flutPhase -= 1.0;
                    const float dl = juce::jlimit (2.0f, (float) (dsize - 4), base + mod);
                    const float rp = (float) s.dW - dl;
                    const int i0 = (int) std::floor (rp);
                    const float fr = rp - (float) i0;
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        auto& buf = s.delayBuf[ch];
                        buf[(size_t) (s.dW & dmask)] = x[i];
                        float y = buf[(size_t) (i0 & dmask)] * (1.0f - fr) + buf[(size_t) ((i0 + 1) & dmask)] * fr;
                        y = std::tanh (y * drive) * norm;
                        s.dLp[ch] += lpC * (y - s.dLp[ch]);
                        x[i] = s.dLp[ch];
                    }
                    ++s.dW;
                }
                break;
            }

            //------------------------------------------------------------------ LO-FI
            case LoFi:
            {
                const float hpC = onePoleCoef (80.0f + 700.0f * a, sr);
                const float lpC = onePoleCoef (9000.0f - 7500.0f * a, sr);
                const float levels = std::pow (2.0f, 12.0f - 8.0f * c);
                const float hiss = d * 0.05f;
                const float crackProb = b * 40.0f / (float) sr;
                for (int i = 0; i < n; ++i)
                {
                    if (s.rng.nextFloat() < crackProb)
                        s.crackle = (s.rng.nextFloat() * 2.0f - 1.0f) * 0.4f;
                    s.crackle *= 0.97f;
                    const float noise = (s.rng.nextFloat() * 2.0f - 1.0f) * hiss + s.crackle;
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        float y = std::round (juce::jlimit (-1.0f, 1.0f, x[i]) * levels) / levels;
                        s.loHp[ch] += hpC * (y - s.loHp[ch]);
                        y -= s.loHp[ch];
                        s.loLp1[ch] += lpC * (y - s.loLp1[ch]);
                        s.loLp2[ch] += lpC * (s.loLp1[ch] - s.loLp2[ch]);
                        x[i] = s.loLp2[ch] + noise;
                    }
                }
                break;
            }

            //------------------------------------------------------------------ ROTARY
            case Rotary:
            {
                const float target = 0.4f + 7.0f * a;
                const float depthSamp = b * 0.0025f * (float) sr;
                const float spread = c * 0.5f;
                const float drive = 1.0f + d * 4.0f;
                for (int i = 0; i < n; ++i)
                {
                    s.rotSpeed += 0.00015f * (target - s.rotSpeed);
                    s.rotPhase += (double) s.rotSpeed / sr;
                    s.rotPhase -= std::floor (s.rotPhase);
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        auto& buf = s.delayBuf[ch];
                        buf[(size_t) (s.dW & dmask)] = std::tanh (x[i] * drive) / std::tanh (drive);
                        const float ph = (float) s.rotPhase + (ch == 0 ? 0.0f : 0.25f + spread);
                        const float sn = std::sin (ph * kTwoPi);
                        const float dl = juce::jlimit (2.0f, (float) (dsize - 4), 0.002f * (float) sr + depthSamp * (1.0f + sn));
                        const float rp = (float) s.dW - dl;
                        const int i0 = (int) std::floor (rp);
                        const float fr = rp - (float) i0;
                        const float y = buf[(size_t) (i0 & dmask)] * (1.0f - fr) + buf[(size_t) ((i0 + 1) & dmask)] * fr;
                        x[i] = y * (1.0f - 0.35f * b * (0.5f - 0.5f * std::cos (ph * kTwoPi)));
                    }
                    ++s.dW;
                }
                break;
            }

            //------------------------------------------------------------------ PAN / TREMOLO
            case PanTrem:
            {
                const double inc = 0.1 * std::pow (160.0, (double) a) / sr;
                const bool trem = d >= 0.5f;
                for (int i = 0; i < n; ++i)
                {
                    const float sn = std::sin ((float) (s.panPhase * (double) kTwoPi));
                    const float sh = (1.0f - c) * sn + c * std::tanh (6.0f * sn);
                    s.panPhase += inc;
                    if (s.panPhase >= 1.0) s.panPhase -= 1.0;
                    if (trem)
                    {
                        const float g = 1.0f - b * (0.5f - 0.5f * sh);
                        L[i] *= g;
                        R[i] *= g;
                    }
                    else
                    {
                        const float pan = b * sh;                          // -1 gauche .. +1 droite
                        const float ang = (pan + 1.0f) * kPi * 0.25f;
                        L[i] *= std::cos (ang) * 1.4142f;
                        R[i] *= std::sin (ang) * 1.4142f;
                    }
                }
                break;
            }

            //------------------------------------------------------------------ WAVESHAPER
            case Shaper:
            {
                const float pre = dbToGain (a * 30.0f);
                const int type = juce::jlimit (0, 4, (int) (b * 4.999f));
                const float bias = (c - 0.5f) * 2.0f * 0.6f;
                const float post = dbToGain ((d - 0.5f) * 24.0f) * 0.7f;
                for (int i = 0; i < n; ++i)
                {
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        const float in = x[i] * pre + bias;
                        float y;
                        switch (type)
                        {
                            case 0:  y = std::tanh (in); break;
                            case 1:  { float t = in * 0.25f + 0.25f; t = t - std::floor (t); y = 4.0f * std::abs (t - 0.5f) - 1.0f; y = in > 1.0f || in < -1.0f ? y : in; break; }
                            case 2:  y = std::sin (in * 1.5707963f); break;
                            case 3:  y = in >= 0.0f ? std::tanh (in) : std::tanh (in * 0.4f) * 1.6f; break;
                            default: y = juce::jlimit (-1.0f, 1.0f, in); break;
                        }
                        const float outv = y * post;
                        s.dcY[ch] = outv - s.dcX[ch] + 0.995f * s.dcY[ch];         // retire le decalage continu
                        s.dcX[ch] = outv;
                        x[i] = s.dcY[ch];
                    }
                }
                break;
            }

            //------------------------------------------------------------------ AUTO-WAH
            case Wah:
            {
                const float sens = 1.0f + 19.0f * a;
                const float base = expMap (b, 100.0f, 2000.0f);
                const float q = 1.0f + 11.0f * c;
                const float range = 0.5f + 4.5f * d;
                const float aC = 1.0f - std::exp (-1.0f / (0.003f * (float) sr));
                const float rC = 1.0f - std::exp (-1.0f / (0.08f * (float) sr));
                for (int ch = 0; ch < 2; ++ch)
                    s.wah[ch].setResonance (q);
                for (int i = 0; i < n; ++i)
                {
                    const float lvl = juce::jmin (1.0f, 0.5f * (std::abs (L[i]) + std::abs (R[i])) * sens);
                    s.wahEnv += (lvl > s.wahEnv ? aC : rC) * (lvl - s.wahEnv);
                    const float f = juce::jlimit (60.0f, (float) (sr * 0.4), base * std::pow (2.0f, s.wahEnv * range));
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        s.wah[ch].setCutoffFrequency (f);
                        x[i] = s.wah[ch].processSample (ch, x[i]) * (1.0f + 0.15f * q);
                    }
                }
                break;
            }

            //------------------------------------------------------------------ LIMITEUR
            case Limiter:
            {
                const float gain = dbToGain (b * 18.0f);
                buffer.applyGain (0, n, 1.0f);
                for (int i = 0; i < n; ++i) { L[i] *= gain; R[i] *= gain; }
                s.limiter.setThreshold (-12.0f + 12.0f * a);
                s.limiter.setRelease (expMap (c, 10.0f, 500.0f));
                float* ch[2] = { L, R };
                juce::dsp::AudioBlock<float> blk (ch, 2, (size_t) n);
                juce::dsp::ProcessContextReplacing<float> ctx (blk);
                s.limiter.process (ctx);
                break;
            }

            //------------------------------------------------------------------ COMPRESSEUR
            case Comp:
            {
                const float thr = -40.0f + 40.0f * a;
                const float ratio = 1.0f + 19.0f * b * b;
                s.comp.process (L, R, n, sr, thr, ratio, expMap (c, 0.1f, 100.0f), expMap (d, 10.0f, 1000.0f),
                                1.0f, 20.0f, false, -thr * (1.0f - 1.0f / ratio) * 0.4f);
                break;
            }

            //------------------------------------------------------------------ GLUE
            case Glue:
            {
                static const float att[] = { 0.1f, 0.3f, 1.0f, 3.0f, 10.0f, 30.0f };
                const float thr = -40.0f + 40.0f * a;
                const float ratio = 1.5f + 6.5f * b;
                const bool autoRel = d > 0.8f;
                s.comp.process (L, R, n, sr, thr, ratio, att[juce::jlimit (0, 5, (int) (c * 5.999f))],
                                autoRel ? 200.0f : 100.0f + d * 1500.0f, 8.0f, 120.0f, autoRel, -thr * (1.0f - 1.0f / ratio) * 0.35f);
                break;
            }

            //------------------------------------------------------------------ MAXIMIZER
            case Maximizer:
            {
                const float gain = dbToGain (a * 20.0f);
                for (int i = 0; i < n; ++i)
                {
                    const float xl = L[i] * gain, xr = R[i] * gain;
                    L[i] = (1.0f - d) * xl + d * std::tanh (xl);
                    R[i] = (1.0f - d) * xr + d * std::tanh (xr);
                }
                s.limiter.setThreshold (-6.0f + 6.0f * b);
                s.limiter.setRelease (expMap (c, 10.0f, 300.0f));
                float* ch[2] = { L, R };
                juce::dsp::AudioBlock<float> blk (ch, 2, (size_t) n);
                juce::dsp::ProcessContextReplacing<float> ctx (blk);
                s.limiter.process (ctx);
                break;
            }

            //------------------------------------------------------------------ EXCITER
            case Exciter:
            {
                const float f = expMap (b, 1500.0f, 9000.0f);
                if (std::abs (f - s.excLastFreq) > 1.0f)
                {
                    const auto co = juce::dsp::IIR::Coefficients<float>::makeHighPass (sr, f, 0.707f);
                    s.excHp[0].coefficients = co;
                    s.excHp[1].coefficients = co;
                    s.excLastFreq = f;
                }
                const float drive = 1.0f + a * 8.0f;
                const float air = c * 1.5f;
                const float lowC = onePoleCoef (200.0f, sr);
                for (int i = 0; i < n; ++i)
                {
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        float* x = ch == 0 ? L : R;
                        const float in = x[i];
                        const float hi = s.excHp[ch].processSample (in);
                        const float harm = std::tanh (hi * drive) - hi;
                        s.excLow[ch] += lowC * (in - s.excLow[ch]);
                        const float warm = std::tanh (s.excLow[ch] * 3.0f) / 3.0f * 3.0f - s.excLow[ch];
                        x[i] = in + harm * air + warm * d * 0.6f;
                    }
                }
                break;
            }

            //------------------------------------------------------------------ IMAGER
            case Imager:
            {
                const float width = a * 2.0f;
                const bool bassMono = b > 0.03f;
                const float lowC = onePoleCoef (40.0f + 260.0f * b, sr);
                const float midG = dbToGain ((c - 0.5f) * 12.0f);
                const float bal = (d - 0.5f) * 2.0f;
                const float gl = bal > 0.0f ? 1.0f - bal : 1.0f, gr = bal < 0.0f ? 1.0f + bal : 1.0f;
                for (int i = 0; i < n; ++i)
                {
                    float l = L[i], r = R[i];
                    if (bassMono)
                    {
                        s.imLp1[0] += lowC * (l - s.imLp1[0]);  s.imLp2[0] += lowC * (s.imLp1[0] - s.imLp2[0]);
                        s.imLp1[1] += lowC * (r - s.imLp1[1]);  s.imLp2[1] += lowC * (s.imLp1[1] - s.imLp2[1]);
                        const float mono = 0.5f * (s.imLp2[0] + s.imLp2[1]);
                        l = (l - s.imLp2[0]) + mono;
                        r = (r - s.imLp2[1]) + mono;
                    }
                    const float mid = 0.5f * (l + r) * midG;
                    const float side = 0.5f * (l - r) * width;
                    L[i] = (mid + side) * gl;
                    R[i] = (mid - side) * gr;
                }
                break;
            }

            default: break;
        }

        // mix sec / traite
        const float m = juce::jlimit (0.0f, 1.0f, p.mix);
        if (m < 0.999f)
        {
            const float* dl = dry.getReadPointer (0);
            const float* dr = dry.getReadPointer (1);
            for (int i = 0; i < n; ++i)
            {
                L[i] = dl[i] + m * (L[i] - dl[i]);
                R[i] = dr[i] + m * (R[i] - dr[i]);
            }
        }

        // securite : jamais de valeur absurde en sortie d'un effet
        for (int i = 0; i < n; ++i)
        {
            if (! std::isfinite (L[i])) L[i] = 0.0f;
            if (! std::isfinite (R[i])) R[i] = 0.0f;
            L[i] = juce::jlimit (-16.0f, 16.0f, L[i]);
            R[i] = juce::jlimit (-16.0f, 16.0f, R[i]);
        }
    }
}
