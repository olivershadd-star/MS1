#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Wavetables : 8 tables de 8 formes d'onde
//==============================================================================
static float basicAmp (int frame, int h)
{
    const float fh = (float) h;
    constexpr float pi = juce::MathConstants<float>::pi;

    switch (frame)
    {
        case 0:  return h == 1 ? 1.0f : 0.0f;                                          // sinus
        case 1:  return (h & 1) ? ((((h - 1) / 2) & 1) ? -1.0f : 1.0f) / (fh * fh)     // triangle
                                : 0.0f;
        case 2:  return (h & 1) ? 1.0f / fh : 0.0f;                                    // carré
        case 3:  return 1.0f / fh;                                                     // dent de scie
        case 4:  return std::sin (pi * fh * 0.15f) / fh;                               // impulsion étroite
        case 5:  return (1.0f / fh) * (1.0f + 3.0f * std::exp (-((fh - 8.0f) * (fh - 8.0f)) / 8.0f)); // formant
        case 6:  return (h == 1 || ! (h & 1)) ? 1.0f / fh : 0.0f;                      // harmoniques paires
        default: return h <= 24 ? 1.0f / std::sqrt (fh) : 0.0f;                        // brillant / buzzy
    }
}

juce::StringArray WavetableBank::tableNames()
{
    return { "BASIQUE", "VOCAL", "PWM", "SPECTRE", "ORGUE", "RESONANCE", "PEIGNE", "CHAOS", "USER 1", "USER 2" };
}

float WavetableBank::harmonicAmp (int tableIndex, int frame, int h)
{
    const float fh = (float) h;
    const float t = (float) frame / (float) (numFrames - 1);          // 0..1 le long de la table
    constexpr float pi = juce::MathConstants<float>::pi;

    auto gauss = [] (float x, float centre, float width)
    {
        const float d = (x - centre) / width;
        return std::exp (-0.5f * d * d);
    };

    switch (tableIndex)
    {
        case 0:                                    // BASIQUE : sinus -> triangle -> carré -> saw -> ...
            return basicAmp (frame, h);

        case 1:                                    // VOCAL : voyelles A E I O U ...
        {
            static const float f1[numFrames] = { 800.0f, 450.0f, 300.0f, 500.0f, 320.0f, 650.0f, 450.0f, 300.0f };
            static const float f2[numFrames] = { 1200.0f, 2000.0f, 2400.0f, 900.0f, 800.0f, 1700.0f, 1400.0f, 1700.0f };
            const float c1 = f1[frame] / 110.0f;
            const float c2 = f2[frame] / 110.0f;
            return (1.0f / std::sqrt (fh)) * (gauss (fh, c1, 1.4f) + 0.7f * gauss (fh, c2, 2.5f) + 0.03f);
        }

        case 2:                                    // PWM : de carré à impulsion très étroite
        {
            const float w = 0.5f - 0.45f * t;
            return std::sin (pi * fh * w) / fh;
        }

        case 3:                                    // SPECTRE : de très doux à très brillant
        {
            const float p = 2.5f - 2.3f * t;
            return 1.0f / std::pow (fh, p);
        }

        case 4:                                    // ORGUE : tirettes d'harmoniques 1 2 3 4 6 8
        {
            static const float w[numFrames][6] = {
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.6f, 0.5f, 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.8f, 0.6f, 0.4f, 0.0f, 0.0f },
                { 1.0f, 1.0f, 0.8f, 0.6f, 0.4f, 0.0f },
                { 1.0f, 0.8f, 0.8f, 0.8f, 0.6f, 0.4f },
                { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
                { 0.8f, 0.4f, 0.9f, 0.3f, 1.0f, 0.8f } };
            const int idx = h == 1 ? 0 : (h == 2 ? 1 : (h == 3 ? 2 : (h == 4 ? 3 : (h == 6 ? 4 : (h == 8 ? 5 : -1)))));
            return idx >= 0 ? w[frame][idx] : 0.0f;
        }

        case 5:                                    // RESONANCE : un pic qui monte dans le spectre
        {
            const float peak = 2.0f + 14.0f * t;
            return (1.0f / std::pow (fh, 0.9f)) * (0.4f + 3.0f * gauss (fh, peak, 1.6f));
        }

        case 6:                                    // PEIGNE : multiples de 2, 3, 4 ... mis en avant
        {
            const int m = frame + 2;
            return ((h % m) == 0 ? 1.0f : 0.08f) / std::sqrt (fh);
        }

        default:                                   // CHAOS : spectres pseudo-aléatoires
        {
            juce::uint32 x = (juce::uint32) (frame * 7919 + h * 104729 + 12345);
            x = x * 1664525u + 1013904223u;
            x ^= x >> 15;
            x *= 2246822519u;
            x ^= x >> 13;
            const float r = (float) (x >> 8) / 16777216.0f;
            return (r * r) / std::pow (fh, 0.7f);
        }
    }
}

WavetableBank::WavetableBank()
{
    data.assign ((size_t) numTables * numFrames * numLevels * tableSize, 0.0f);

    std::vector<float> sine (tableSize);
    for (int i = 0; i < tableSize; ++i)
        sine[(size_t) i] = std::sin (juce::MathConstants<float>::twoPi * (float) i / (float) tableSize);

    std::vector<float> acc ((size_t) tableSize);

    for (int t = 0; t < numTables; ++t)
    {
        for (int frame = 0; frame < numFrames; ++frame)
        {
            std::fill (acc.begin(), acc.end(), 0.0f);

            // On construit de la version la plus pauvre (1 harmonique) à la plus riche (512),
            // en ajoutant les harmoniques au fur et à mesure.
            int h = 1;
            for (int level = numLevels - 1; level >= 0; --level)
            {
                const int maxHarmonics = 512 >> level;

                for (; h <= maxHarmonics; ++h)
                {
                    const float a = harmonicAmp (t, frame, h);
                    if (std::abs (a) < 1.0e-5f)
                        continue;

                    for (int i = 0; i < tableSize; ++i)
                        acc[(size_t) i] += a * sine[(size_t) ((h * i) & (tableSize - 1))];
                }

                std::copy (acc.begin(), acc.end(), data.begin() + (std::ptrdiff_t) offsetOf (t, frame, level));
            }

            // Même facteur de normalisation pour toutes les versions d'une forme d'onde
            float peak = 0.0f;
            const auto fullOffset = offsetOf (t, frame, 0);
            for (int i = 0; i < tableSize; ++i)
                peak = juce::jmax (peak, std::abs (data[fullOffset + (size_t) i]));

            const float scale = peak > 0.0f ? 1.0f / peak : 1.0f;
            for (int level = 0; level < numLevels; ++level)
            {
                const auto off = offsetOf (t, frame, level);
                for (int i = 0; i < tableSize; ++i)
                    data[off + (size_t) i] *= scale;
            }
        }
    }
}

int WavetableBank::levelForFrequency (double hz) noexcept
{
    return juce::jlimit (0, numLevels - 1, (int) std::floor (std::log2 (juce::jmax (1.0, hz) / 20.0)));
}

float WavetableBank::getSample (int tableIndex, float pos, int level, double phase) const noexcept
{
    const int idx = juce::jlimit (0, totalTables - 1, tableIndex);

    const float* base = nullptr;
    int frames = numFrames;

    if (idx < numTables)
    {
        base = data.data() + (size_t) idx * numFrames * numLevels * tableSize;
    }
    else
    {
        const auto& slot = userSlots[(size_t) (idx - numTables)];
        const int act = slot.active.load (std::memory_order_acquire);
        frames = slot.frames[(size_t) act];
        if (frames <= 0)
            return 0.0f;                                   // emplacement vide : silence

        base = slot.buffers[(size_t) act].data();
    }

    const float fpos = juce::jlimit (0.0f, 1.0f, pos) * (float) (frames - 1);
    const int f0 = (int) fpos;
    const int f1 = juce::jmin (f0 + 1, frames - 1);
    const float fm = fpos - (float) f0;

    const float x = (float) (phase * tableSize);
    const int i0 = (int) x & (tableSize - 1);
    const int i1 = (i0 + 1) & (tableSize - 1);
    const float fr = x - std::floor (x);

    const float* a = base + (size_t) (f0 * numLevels + level) * tableSize;
    const float* b = base + (size_t) (f1 * numLevels + level) * tableSize;

    const float sa = a[i0] + (a[i1] - a[i0]) * fr;
    const float sb = b[i0] + (b[i1] - b[i0]) * fr;
    return sa + (sb - sa) * fm;
}

void WavetableBank::setUserTable (int slot, const std::vector<float>& frames, int numUserFrames)
{
    if (! juce::isPositiveAndBelow (slot, numUserTables) || numUserFrames < 1
        || frames.size() < (size_t) numUserFrames * tableSize)
        return;

    auto& s = userSlots[(size_t) slot];
    const int target = 1 - s.active.load (std::memory_order_acquire);     // on écrit dans le tampon inactif
    auto& out = s.buffers[(size_t) target];
    out.assign ((size_t) numUserFrames * numLevels * tableSize, 0.0f);

    std::vector<float> sine (tableSize);
    for (int i = 0; i < tableSize; ++i)
        sine[(size_t) i] = std::sin (juce::MathConstants<float>::twoPi * (float) i / (float) tableSize);

    constexpr int quarter = tableSize / 4;
    std::vector<float> acc ((size_t) tableSize), ca (513), cb (513);

    for (int f = 0; f < numUserFrames; ++f)
    {
        const float* x = frames.data() + (size_t) f * tableSize;

        // Analyse : amplitude (sinus et cosinus) des harmoniques 1 à 512
        for (int h = 1; h <= 512; ++h)
        {
            float sa = 0.0f, sb = 0.0f;
            for (int i = 0; i < tableSize; ++i)
            {
                const int idx = (h * i) & (tableSize - 1);
                sa += x[i] * sine[(size_t) idx];
                sb += x[i] * sine[(size_t) ((idx + quarter) & (tableSize - 1))];
            }
            ca[(size_t) h] = sa * (2.0f / (float) tableSize);
            cb[(size_t) h] = sb * (2.0f / (float) tableSize);
        }

        // Synthèse : les mêmes 10 versions limitées en bande que pour les tables intégrées
        std::fill (acc.begin(), acc.end(), 0.0f);
        int harmonic = 1;

        for (int level = numLevels - 1; level >= 0; --level)
        {
            const int maxHarmonics = 512 >> level;

            for (; harmonic <= maxHarmonics; ++harmonic)
            {
                const float a = ca[(size_t) harmonic];
                const float b = cb[(size_t) harmonic];
                if (std::abs (a) < 1.0e-6f && std::abs (b) < 1.0e-6f)
                    continue;

                for (int i = 0; i < tableSize; ++i)
                {
                    const int idx = (harmonic * i) & (tableSize - 1);
                    acc[(size_t) i] += a * sine[(size_t) idx] + b * sine[(size_t) ((idx + quarter) & (tableSize - 1))];
                }
            }

            std::copy (acc.begin(), acc.end(),
                       out.begin() + (std::ptrdiff_t) ((size_t) (f * numLevels + level) * tableSize));
        }

        // Normalisation : même facteur pour toutes les versions d'une forme d'onde
        const auto fullOffset = (size_t) (f * numLevels) * tableSize;
        float peak = 0.0f;
        for (int i = 0; i < tableSize; ++i)
            peak = juce::jmax (peak, std::abs (out[fullOffset + (size_t) i]));

        const float scale = peak > 1.0e-6f ? 1.0f / peak : 0.0f;
        for (int level = 0; level < numLevels; ++level)
        {
            const auto off = (size_t) (f * numLevels + level) * tableSize;
            for (int i = 0; i < tableSize; ++i)
                out[off + (size_t) i] *= scale;
        }
    }

    s.frames[(size_t) target] = numUserFrames;
    s.active.store (target, std::memory_order_release);                    // bascule : la nouvelle table joue
}

//==============================================================================
// Séquenceur : données
//==============================================================================
StepSequencerData::StepSequencerData()
{
    for (size_t i = 0; i < (size_t) maxSteps; ++i)
    {
        on[i] = false;
        note[i] = 48;
    }

    // Petit motif de départ sur la première ligne
    for (int s : { 0, 4, 8, 12 })
        on[(size_t) s] = true;
    note[8] = 55;
    note[12] = 53;
}

juce::ValueTree StepSequencerData::toValueTree() const
{
    juce::String bits;
    juce::StringArray notes;

    for (size_t i = 0; i < (size_t) maxSteps; ++i)
    {
        bits += on[i].load() ? "1" : "0";
        notes.add (juce::String (note[i].load()));
    }

    juce::ValueTree t ("SEQ");
    t.setProperty ("on", bits, nullptr);
    t.setProperty ("notes", notes.joinIntoString (","), nullptr);
    return t;
}

void StepSequencerData::fromValueTree (const juce::ValueTree& t)
{
    const auto bits = t["on"].toString();
    const auto notes = juce::StringArray::fromTokens (t["notes"].toString(), ",", "");

    for (int i = 0; i < maxSteps; ++i)
    {
        on[(size_t) i] = i < bits.length() && bits[i] == '1';
        note[(size_t) i] = i < notes.size() ? juce::jlimit (24, 96, notes[i].getIntValue()) : 48;
    }

    ++revision;
}

//==============================================================================
// Voix
//==============================================================================
static float lfoValue (int shape, double ph)
{
    switch (shape)
    {
        case 0:  return std::sin ((float) (ph * juce::MathConstants<double>::twoPi));   // sinus
        case 1:  return 1.0f - 4.0f * std::abs ((float) ph - 0.5f);                      // triangle
        case 2:  return (float) (2.0 * ph - 1.0);                                        // dent de scie
        default: return ph < 0.5 ? 1.0f : -1.0f;                                         // carré
    }
}

SynthVoice::SynthVoice (juce::AudioProcessorValueTreeState& state, const WavetableBank& wavetables,
                        std::atomic<float>& modWheelRef, std::atomic<double>& bpmRef)
    : apvts (state), bank (wavetables), modWheel (modWheelRef), hostBpm (bpmRef)
{
    for (int s = 0; s < numMixSlots; ++s)
    {
        const juce::String n = "mx" + juce::String (s + 1);
        mixParams[(size_t) (s * 3)]     = apvts.getRawParameterValue (n + "Src");
        mixParams[(size_t) (s * 3 + 1)] = apvts.getRawParameterValue (n + "Dst");
        mixParams[(size_t) (s * 3 + 2)] = apvts.getRawParameterValue (n + "Amt");
    }
}

void SynthVoice::prepare (double sampleRate, int blockSize)
{
    sr = sampleRate;
    adsr.setSampleRate (sr);
    modEnv.setSampleRate (sr);
    fltEnv.setSampleRate (sr);
    position.reset (sr, 0.02);   // lissage de 20 ms pour éviter les clics

    juce::dsp::ProcessSpec spec { sr, (juce::uint32) blockSize, 2 };   // filtres stéréo
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    filter2.prepare (spec);
    filter2.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    filter2.setResonance (0.7071f);
}

void SynthVoice::startNote (int midiNote, float vel, juce::SynthesiserSound*, int pitchWheelPosition)
{
    currentNote = targetNote = (double) midiNote;
    bendNorm = (float) (pitchWheelPosition - 8192) / 8192.0f;
    velocity = vel;
    noteRandom = rng.nextFloat() * 2.0f - 1.0f;
    keyDown = true;

    phasesA[0] = 0.0;
    phasesB[0] = 0.0;
    for (size_t i = 1; i < phasesA.size(); ++i)
    {
        phasesA[i] = rng.nextDouble();       // phases aléatoires = son plus large
        phasesB[i] = rng.nextDouble();
    }
    phaseSub = 0.0;
    lfoPhase = 0.0;
    lfo2Phase = 0.0;

    position.setCurrentAndTargetValue (param ("position"));
    filter.reset();
    filter2.reset();

    adsr.noteOn();
    modEnv.noteOn();
    fltEnv.noteOn();
}

void SynthVoice::legatoTo (int midiNote, float vel)
{
    targetNote = (double) midiNote;      // la note glisse vers la nouvelle (selon le réglage Glide)
    velocity = vel;
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    keyDown = false;

    if (allowTailOff)
    {
        adsr.noteOff();
        modEnv.noteOff();
        fltEnv.noteOff();
    }
    else
    {
        adsr.reset();
        modEnv.reset();
        fltEnv.reset();
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved (int newValue)
{
    bendNorm = (float) (newValue - 8192) / 8192.0f;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    // --- Enveloppes (les durées de decay peuvent être modulées par la matrice, voir plus bas)
    const float ampA = param ("attack"),    ampD = param ("decay"),    ampS = param ("sustain"),    ampR = param ("release");
    const float fltA = param ("fltAttack"), fltD = param ("fltDecay"), fltS = param ("fltSustain"), fltR = param ("fltRelease");
    adsr.setParameters   ({ ampA, ampD, ampS, ampR });
    modEnv.setParameters ({ param ("modAttack"), param ("modDecay"), param ("modSustain"), param ("modRelease") });
    fltEnv.setParameters ({ fltA, fltD, fltS, fltR });

    // --- Filtre
    using FilterType = juce::dsp::StateVariableTPTFilterType;
    const int   filterType = (int) param ("filterType");      // 0 LP12, 1 LP24, 2 HP, 3 BP
    const float baseCutoff = param ("cutoff");
    const float baseRes    = param ("resonance");
    const float fltAmount  = param ("fltEnvAmount");
    const float drive      = param ("drive");
    const bool  twoPole    = (filterType == 1);

    filter.setType (filterType == 2 ? FilterType::highpass
                                    : (filterType == 3 ? FilterType::bandpass : FilterType::lowpass));
    filter.setResonance (baseRes);

    // --- Vélocité
    const float velAmp    = param ("velAmp");
    const float velFilter = param ("velFilter");
    const float velTerm   = velFilter * (velocity - 0.5f) * 6.0f;                 // en octaves
    const float ampVel    = (1.0f - velAmp) + velAmp * velocity;
    const float wheel     = modWheel.load();

    // --- Matrice de modulation : sources 0..12, cibles 0..17
    struct Slot { int src; int dst; float amt; };
    std::array<Slot, numMixSlots> slots {};
    int numSlots = 0;
    bool resonanceModulated = false;
    bool envTimeModulated = false;

    for (int s = 0; s < numMixSlots; ++s)
    {
        const Slot sl { (int) mixParams[(size_t) (s * 3)]->load(),
                        (int) mixParams[(size_t) (s * 3 + 1)]->load(),
                        mixParams[(size_t) (s * 3 + 2)]->load() };

        if (sl.src > 0 && sl.dst > 0 && sl.amt != 0.0f)
        {
            slots[(size_t) numSlots++] = sl;
            if (sl.dst == 4)
                resonanceModulated = true;
            if (sl.dst == 12 || sl.dst == 13)
                envTimeModulated = true;
        }
    }

    // --- Modulation de la position de wavetable (LFO + enveloppe)
    position.setTargetValue (param ("position"));
    const float  modAmount = param ("modAmount");
    const float  lfoDepth  = param ("lfoDepth");
    const int    lfoShape  = (int) param ("lfoShape");

    // Vitesse d'un LFO : libre (en Hz) ou synchronisée sur le tempo (durée d'un cycle en temps)
    const double bpm = juce::jmax (20.0, hostBpm.load());
    auto lfoIncrement = [&] (float rateHz, int syncIndex) -> double
    {
        static const double beatsPerCycle[] = { 0.0, 16.0, 8.0, 4.0, 2.0, 1.0, 0.5, 0.25, 0.125 };
        const int idx = juce::jlimit (0, 8, syncIndex);
        if (idx == 0)
            return (double) rateHz / sr;
        return (bpm / 60.0) / beatsPerCycle[idx] / sr;
    };

    const double lfoInc    = lfoIncrement (param ("lfoRate"),  (int) param ("lfoSync"));
    const int    lfo2Shape = (int) param ("lfo2Shape");
    const double lfo2Inc   = lfoIncrement (param ("lfo2Rate"), (int) param ("lfo2Sync"));
    const std::array<float, 4> macros { param ("macro1"), param ("macro2"), param ("macro3"), param ("macro4") };

    // --- Tables
    const int tabA = (int) param ("tableA");
    const int tabB = (int) param ("tableB");

    // --- Unison : N oscillateurs désaccordés et répartis dans le champ stéréo
    const int   uni    = juce::jlimit (1, maxUnison, (int) std::round (param ("unison")));
    const float detune = param ("detune");
    const float spread = param ("spread");
    const float norm   = 1.0f / std::sqrt ((float) uni);

    std::array<float, maxUnison> offsets {};
    std::array<float, maxUnison> gainL {}, gainR {};

    for (int u = 0; u < uni; ++u)
    {
        const float offset = uni > 1 ? -1.0f + 2.0f * (float) u / (float) (uni - 1) : 0.0f;
        offsets[(size_t) u] = offset;

        const float angle = (offset * spread + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        gainL[(size_t) u] = std::cos (angle) * juce::MathConstants<float>::sqrt2 * norm;
        gainR[(size_t) u] = std::sin (angle) * juce::MathConstants<float>::sqrt2 * norm;
    }

    // --- Oscillateur B et sub
    const float  levelB  = param ("oscBLevel");
    const float  posB    = param ("oscBPos");
    const double factorB = std::pow (2.0, std::round ((double) param ("oscBOct")) + (double) param ("oscBTune") / 1200.0);
    const float  subAmt  = param ("subLevel");

    // --- Warp, FM, ring
    const int   warpModeA  = (int) param ("warpModeA");
    const int   warpModeB  = (int) param ("warpModeB");
    const float warpAmtA   = param ("warpAmtA");
    const float warpAmtB   = param ("warpAmtB");
    const float fmAmount   = param ("fmAmount");
    const float ringAmount = param ("ringAmount");

    // --- Hauteur : glide, pitch bend, modulation
    const double glideSec  = param ("glide");
    const float  bendRange = param ("bendRange");
    float modPitch = 0.0f;           // demi-tons, fourni par la matrice
    float detuneNow = detune;
    float keyNorm = 0.0f;

    std::array<double, maxUnison> incA {}, incB {};
    double incSub = 0.0;
    int mipA = 0, mipB = 0;

    auto updatePitch = [&] (int elapsed)
    {
        const double a = glideSec > 0.0005 ? 1.0 - std::exp (-(double) elapsed / (glideSec * 0.25 * sr)) : 1.0;
        currentNote += (targetNote - currentNote) * a;
        keyNorm = juce::jlimit (-1.0f, 1.0f, (float) ((currentNote - 60.0) / 36.0));

        const double note  = currentNote + (double) (bendNorm * bendRange) + (double) modPitch;
        const double freq  = 440.0 * std::pow (2.0, (note - 69.0) / 12.0);
        const double freqB = freq * factorB;

        for (int u = 0; u < uni; ++u)
        {
            const auto k = (size_t) u;
            const double ratio = std::pow (2.0, (double) (offsets[k] * detuneNow) / 1200.0);
            incA[k] = freq / sr * ratio;
            incB[k] = freqB / sr * ratio;
        }
        incSub = freq / sr * 0.5;                              // sub : une octave en dessous

        const double maxRatio = std::pow (2.0, (double) detuneNow / 1200.0);
        mipA = WavetableBank::levelForFrequency (freq * maxRatio);
        mipB = WavetableBank::levelForFrequency (freqB * maxRatio);
    };

    for (int i = 0; i < numSamples; ++i)
    {
        // Sources de modulation
        const float lfo = lfoValue (lfoShape, lfoPhase);
        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0)
            lfoPhase -= 1.0;

        const float lfo2 = lfoValue (lfo2Shape, lfo2Phase);
        lfo2Phase += lfo2Inc;
        if (lfo2Phase >= 1.0)
            lfo2Phase -= 1.0;

        const float mEnv = modEnv.getNextSample();
        const float fEnv = fltEnv.getNextSample();

        std::array<float, 13> src {};
        src[1] = lfo;  src[2] = mEnv;  src[3] = fEnv;  src[4] = velocity;
        src[5] = keyNorm;  src[6] = wheel;  src[7] = noteRandom;
        src[8] = lfo2;
        src[9] = macros[0];  src[10] = macros[1];  src[11] = macros[2];  src[12] = macros[3];

        std::array<float, 18> tgt {};
        for (int s = 0; s < numSlots; ++s)
        {
            const auto& sl = slots[(size_t) s];
            tgt[(size_t) sl.dst] += sl.amt * src[(size_t) sl.src];
        }

        if ((i & 31) == 0)
        {
            modPitch  = tgt[5] * 12.0f;
            detuneNow = juce::jlimit (0.0f, 100.0f, detune + tgt[6] * 50.0f);
            updatePitch (32);

            if (envTimeModulated)       // un LFO ou une macro peut faire varier la durée des enveloppes
            {
                adsr.setParameters   ({ ampA, juce::jlimit (0.001f, 20.0f, ampD * std::exp2 (tgt[12] * 3.0f)), ampS, ampR });
                fltEnv.setParameters ({ fltA, juce::jlimit (0.001f, 20.0f, fltD * std::exp2 (tgt[13] * 3.0f)), fltS, fltR });
            }
        }

        // Position dans la wavetable = réglage de base + LFO + enveloppe de modulation + matrice
        const float posA = position.getNextValue() + lfoDepth * lfo + (modAmount + tgt[11]) * mEnv + tgt[1];
        const float posBNow = posB + tgt[2];
        const float levelBNow = juce::jlimit (0.0f, 1.0f, levelB + tgt[7]);

        // Oscillateurs A et B (avec unison) : warp, FM (B fait vibrer la phase de A) et ring (A x B)
        const float warpA = juce::jlimit (0.0f, 1.0f, warpAmtA + tgt[14]);
        const float warpB = juce::jlimit (0.0f, 1.0f, warpAmtB + tgt[15]);
        const float fm    = juce::jlimit (0.0f, 1.0f, fmAmount + tgt[16]);
        const float ring  = juce::jlimit (0.0f, 1.0f, ringAmount + tgt[17]);
        const bool  needB = levelBNow > 0.0f || fm > 0.001f || ring > 0.001f;

        float l = 0.0f, r = 0.0f;
        for (int u = 0; u < uni; ++u)
        {
            const auto k = (size_t) u;

            float sB = 0.0f;
            if (needB)
                sB = bank.getSample (tabB, posBNow, mipB, warpPhase (warpModeB, warpB, phasesB[k]));

            double phaseA = phasesA[k];
            if (fm > 0.001f)
            {
                phaseA += (double) (fm * 1.5f * sB);
                phaseA -= std::floor (phaseA);
            }

            const float sA = bank.getSample (tabA, posA, mipA, warpPhase (warpModeA, warpA, phaseA));

            float s = sA + levelBNow * sB;
            if (ring > 0.001f)
                s += ring * (sA * sB * 1.4f - s);

            l += s * gainL[k];
            r += s * gainR[k];

            phasesA[k] += incA[k];
            if (phasesA[k] >= 1.0) phasesA[k] -= 1.0;
            phasesB[k] += incB[k];
            if (phasesB[k] >= 1.0) phasesB[k] -= 1.0;
        }

        // Sub (sinus au centre)
        if (subAmt > 0.0f)
        {
            const float sub = std::sin ((float) (phaseSub * juce::MathConstants<double>::twoPi)) * subAmt;
            l += sub;
            r += sub;
        }
        phaseSub += incSub;
        if (phaseSub >= 1.0)
            phaseSub -= 1.0;

        // Saturation avant le filtre
        const float driveNow = juce::jlimit (0.0f, 1.0f, drive + tgt[8]);
        if (driveNow > 0.001f)
        {
            const float d = 1.0f + driveNow * 10.0f;
            const float inv = 1.0f / std::tanh (d);
            l += driveNow * (std::tanh (l * d) * inv - l);
            r += driveNow * (std::tanh (r * d) * inv - r);
        }

        // Filtre : enveloppe (jusqu'à +/- 5 octaves), matrice et vélocité
        const float octaves = (fltAmount + tgt[10]) * fEnv * 5.0f + tgt[3] * 5.0f + velTerm;
        const float cutoff = juce::jlimit (20.0f, 20000.0f, baseCutoff * std::exp2 (octaves));
        filter.setCutoffFrequency (cutoff);
        if (twoPole)
            filter2.setCutoffFrequency (cutoff);
        if (resonanceModulated)
            filter.setResonance (juce::jlimit (0.5f, 10.0f, baseRes + tgt[4] * 4.0f));

        l = filter.processSample (0, l);
        r = filter.processSample (1, r);
        if (twoPole)
        {
            l = filter2.processSample (0, l);
            r = filter2.processSample (1, r);
        }

        // Volume
        const float ampMod = juce::jlimit (0.0f, 2.0f, 1.0f + tgt[9]);
        const float env = adsr.getNextSample() * ampVel * ampMod * 0.25f;
        l *= env;
        r *= env;

        if (buffer.getNumChannels() > 1)
        {
            buffer.addSample (0, startSample + i, l);
            buffer.addSample (1, startSample + i, r);
        }
        else
        {
            buffer.addSample (0, startSample + i, 0.5f * (l + r));
        }

        if (! adsr.isActive())
        {
            keyDown = false;
            clearCurrentNote();
            break;
        }
    }
}

//==============================================================================
// Mode mono / polyphonique
//==============================================================================
void MonoCapableSynth::setMono (bool shouldBeMono)
{
    if (shouldBeMono != mono)
    {
        const juce::ScopedLock sl (lock);
        mono = shouldBeMono;
        held.clear();
    }
}

void MonoCapableSynth::noteOn (int midiChannel, int midiNoteNumber, float velocity)
{
    if (! mono)
    {
        juce::Synthesiser::noteOn (midiChannel, midiNoteNumber, velocity);
        return;
    }

    const juce::ScopedLock sl (lock);

    held.removeFirstMatchingValue (midiNoteNumber);
    held.add (midiNoteNumber);

    auto* voice = dynamic_cast<SynthVoice*> (getVoice (0));
    auto sound = getSound (0);
    if (voice == nullptr || sound == nullptr)
        return;

    if (voice->isVoiceActive() && voice->isKeyDown())
        voice->legatoTo (midiNoteNumber, velocity);                 // legato : pas de redéclenchement
    else
        startVoice (voice, sound.get(), midiChannel, midiNoteNumber, velocity);
}

void MonoCapableSynth::noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
{
    if (! mono)
    {
        juce::Synthesiser::noteOff (midiChannel, midiNoteNumber, velocity, allowTailOff);
        return;
    }

    const juce::ScopedLock sl (lock);
    held.removeFirstMatchingValue (midiNoteNumber);

    auto* voice = dynamic_cast<SynthVoice*> (getVoice (0));
    if (voice == nullptr)
        return;

    if (held.isEmpty())
    {
        if (voice->isKeyDown())
            stopVoice (voice, velocity, allowTailOff);
    }
    else
    {
        voice->legatoTo (held.getLast(), velocity);                 // retour à la dernière note encore tenue
    }
}

//==============================================================================
// Plugin
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MonSyntheProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    auto addFloat = [&p] (const char* id, const char* name, NormalisableRange<float> range, float def)
    {
        p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, range, def));
    };
    auto addInt = [&p] (const char* id, const char* name, int lo, int hi, int def)
    {
        p.push_back (std::make_unique<AudioParameterInt> (ParameterID { id, 1 }, name, lo, hi, def));
    };
    auto addChoice = [&p] (const char* id, const char* name, StringArray choices, int def)
    {
        p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, choices, def));
    };
    auto addBool = [&p] (const char* id, const char* name, bool def)
    {
        p.push_back (std::make_unique<AudioParameterBool> (ParameterID { id, 1 }, name, def));
    };

    const NormalisableRange<float> time (0.001f, 5.0f, 0.0f, 0.4f);
    const NormalisableRange<float> unit (0.0f, 1.0f);

    // Oscillateur A
    addChoice ("tableA", "Table A", WavetableBank::tableNames(), 0);
    addFloat  ("position", "Wavetable", unit, 0.3f);

    // Oscillateur B + sub
    addChoice ("tableB",   "Table B", WavetableBank::tableNames(), 0);
    addFloat ("oscBPos",   "B Wavetable", unit, 0.5f);
    addInt   ("oscBOct",   "B Octave", -2, 2, 0);
    addFloat ("oscBTune",  "B Accord (cents)", NormalisableRange<float> (-100.0f, 100.0f), 0.0f);
    addFloat ("oscBLevel", "B Niveau", unit, 0.0f);
    addFloat ("subLevel",  "Sub Niveau", unit, 0.0f);

    // Warp, FM et ring : de quoi travailler le son bien plus loin
    addChoice ("warpModeA", "Warp A Mode", warpModeNames(), 0);
    addFloat  ("warpAmtA",  "Warp A", unit, 0.0f);
    addChoice ("warpModeB", "Warp B Mode", warpModeNames(), 0);
    addFloat  ("warpAmtB",  "Warp B", unit, 0.0f);
    addFloat  ("fmAmount",  "FM (B vers A)", unit, 0.0f);
    addFloat  ("ringAmount", "Ring (A x B)", unit, 0.0f);

    // Unison
    addInt   ("unison", "Unison", 1, 7, 3);
    addFloat ("detune", "Desaccord (cents)", NormalisableRange<float> (0.0f, 100.0f), 15.0f);
    addFloat ("spread", "Stereo", unit, 0.6f);

    // Filtre
    addChoice ("filterType", "Filtre Type", StringArray { "LP 12", "LP 24", "HP", "BP" }, 0);
    addFloat  ("cutoff",    "Cutoff", NormalisableRange<float> (20.0f, 20000.0f, 0.0f, 0.3f), 2000.0f);
    addFloat  ("resonance", "Resonance", NormalisableRange<float> (0.5f, 10.0f, 0.0f, 0.5f), 0.7f);
    addFloat  ("drive",     "Drive", unit, 0.0f);
    addFloat  ("fltEnvAmount", "Filtre Env Quantite", NormalisableRange<float> (-1.0f, 1.0f), 0.0f);
    addFloat  ("fltAttack",  "Filtre Attack", time, 0.01f);
    addFloat  ("fltDecay",   "Filtre Decay", time, 0.4f);
    addFloat  ("fltSustain", "Filtre Sustain", unit, 0.3f);
    addFloat  ("fltRelease", "Filtre Release", time, 0.3f);

    // Enveloppe de volume
    addFloat ("attack",  "Attack", time, 0.01f);
    addFloat ("decay",   "Decay", time, 0.2f);
    addFloat ("sustain", "Sustain", unit, 0.7f);
    addFloat ("release", "Release", time, 0.3f);

    // LFO -> position de la wavetable
    addChoice ("lfoShape", "LFO Forme", StringArray { "Sinus", "Triangle", "Saw", "Carre" }, 0);
    addFloat  ("lfoRate",  "LFO Vitesse (Hz)", NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 1.0f);
    addFloat  ("lfoDepth", "LFO Intensite", unit, 0.2f);
    addChoice ("lfoSync",  "LFO Sync", lfoSyncNames(), 0);

    // LFO 2 (utilisable uniquement via la matrice) et macros
    addChoice ("lfo2Shape", "LFO2 Forme", StringArray { "Sinus", "Triangle", "Saw", "Carre" }, 0);
    addFloat  ("lfo2Rate",  "LFO2 Vitesse (Hz)", NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 0.5f);
    addChoice ("lfo2Sync",  "LFO2 Sync", lfoSyncNames(), 0);
    addFloat  ("macro1", "Macro 1", unit, 0.0f);
    addFloat  ("macro2", "Macro 2", unit, 0.0f);
    addFloat  ("macro3", "Macro 3", unit, 0.0f);
    addFloat  ("macro4", "Macro 4", unit, 0.0f);

    // Enveloppe de modulation -> position de la wavetable
    addFloat ("modAttack",  "Mod Attack", time, 0.01f);
    addFloat ("modDecay",   "Mod Decay", time, 0.8f);
    addFloat ("modSustain", "Mod Sustain", unit, 0.0f);
    addFloat ("modRelease", "Mod Release", time, 0.3f);
    addFloat ("modAmount",  "Mod Quantite", NormalisableRange<float> (-1.0f, 1.0f), 0.5f);

    // Vélocité
    addFloat ("velAmp",    "Velocite Volume", unit, 1.0f);
    addFloat ("velFilter", "Velocite Filtre", unit, 0.0f);

    // Matrice de modulation : 6 routes (source -> cible, quantité)
    for (int i = 1; i <= SynthVoice::numMixSlots; ++i)
    {
        const String n = "mx" + String (i);
        p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { n + "Src", 1 }, "Mod " + String (i) + " Source",
                                                             modSourceNames(), 0));
        p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { n + "Dst", 1 }, "Mod " + String (i) + " Cible",
                                                             modTargetNames(), 0));
        p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { n + "Amt", 1 }, "Mod " + String (i) + " Quantite",
                                                            NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    }

    // Jeu
    addBool  ("mono",      "Mono", false);
    addFloat ("glide",     "Glide (s)", NormalisableRange<float> (0.0f, 2.0f, 0.0f, 0.4f), 0.08f);
    addInt   ("bendRange", "Pitch Bend (demi-tons)", 1, 24, 2);
    addFloat ("master",    "Volume", NormalisableRange<float> (0.0f, 1.5f), 1.0f);

    // Effets
    addFloat ("chorusMix",   "Chorus Mix", unit, 0.0f);
    addFloat ("chorusRate",  "Chorus Vitesse", NormalisableRange<float> (0.1f, 5.0f, 0.0f, 0.5f), 0.8f);
    addFloat ("chorusDepth", "Chorus Profondeur", unit, 0.4f);

    addFloat ("delayTime",     "Delay Temps (ms)", NormalisableRange<float> (20.0f, 1000.0f, 0.0f, 0.5f), 350.0f);
    addFloat ("delayFeedback", "Delay Feedback", NormalisableRange<float> (0.0f, 0.9f), 0.35f);
    addFloat ("delayMix",      "Delay Mix", unit, 0.0f);

    addFloat ("reverbSize", "Reverb Taille", unit, 0.5f);
    addFloat ("reverbDamp", "Reverb Amorti", unit, 0.5f);
    addFloat ("reverbMix",  "Reverb Mix", unit, 0.0f);

    // Séquenceur
    addBool   ("seqOn", "Sequenceur", false);
    addChoice ("seqBars", "Seq Longueur", StringArray { "1 mesure", "2 mesures", "3 mesures", "4 mesures" }, 0);
    addChoice ("seqRate", "Seq Vitesse", StringArray { "1/8", "1/16", "1/32" }, 1);
    addFloat  ("seqGate",  "Seq Gate", NormalisableRange<float> (0.05f, 1.0f), 0.5f);
    addFloat  ("seqTempo", "Seq Tempo (libre)", NormalisableRange<float> (40.0f, 240.0f, 1.0f), 120.0f);

    return { p.begin(), p.end() };
}

MonSyntheProcessor::MonSyntheProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    for (auto& v : scopeRing)
        v.store (0.0f);

    {
        const auto themeFile = getPresetDirectory().getParentDirectory().getChildFile ("theme.txt");
        if (themeFile.existsAsFile())
            themeHue = juce::jlimit (0.0f, 1.0f, themeFile.loadFileAsString().trim().getFloatValue());
    }

    synth.addSound (new SynthSound());
    for (int i = 0; i < 8; ++i)               // 8 voix de polyphonie
        synth.addVoice (new SynthVoice (apvts, bank, modWheel, hostBpm));
}

void MonSyntheProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            v->prepare (sampleRate, samplesPerBlock);

    // Effets
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) juce::jmax (samplesPerBlock, 8192), 2 };
    chorus.prepare (spec);
    chorus.setCentreDelay (7.0f);
    chorus.setFeedback (0.0f);

    delayLine.setMaximumDelayInSamples ((int) (sampleRate * 1.3) + 8);
    delayLine.prepare (spec);
    delayLine.reset();

    reverb.setSampleRate (sampleRate);
    reverb.reset();

    lastMaster = param ("master");

    seqNote = -1;
    sampleClock = 0;
    freeBeat = 0.0;
}

void MonSyntheProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const int numSamples = buffer.getNumSamples();

    for (const auto metadata : midi)                         // molette de modulation (CC1)
    {
        const auto msg = metadata.getMessage();
        if (msg.isController() && msg.getControllerNumber() == 1)
            modWheel = (float) msg.getControllerValue() / 127.0f;
    }

    double bpmNow = param ("seqTempo");                       // à défaut d'hôte : le tempo libre
    bool playingNow = false;
    double ppqNow = 0.0;
    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto hostTempo = pos->getBpm())
                bpmNow = *hostTempo;
            playingNow = pos->getIsPlaying();
            if (auto ppq = pos->getPpqPosition())
                ppqNow = *ppq;
        }
    }
    hostBpm = bpmNow;
    hostPlaying = playingNow;
    hostPpq = ppqNow;

    synth.setMono (param ("mono") > 0.5f);
    generateSequencerMidi (midi, numSamples);
    synth.renderNextBlock (buffer, midi, 0, numSamples);

    applyEffects (buffer);

    // Voyants : quelles voix jouent
    {
        int mask = 0;
        for (int i = 0; i < synth.getNumVoices() && i < 30; ++i)
            if (synth.getVoice (i)->isVoiceActive())
                mask |= (1 << i);
        activeVoiceMask = mask;
    }

    // Copie de la sortie (mono) pour l'analyseur de spectre
    {
        const int numCh = buffer.getNumChannels();
        const int w = scopeWrite.load (std::memory_order_relaxed);
        for (int i = 0; i < numSamples; ++i)
        {
            float v = buffer.getSample (0, i);
            if (numCh > 1)
                v = 0.5f * (v + buffer.getSample (1, i));
            scopeRing[(size_t) ((w + i) & (scopeSize - 1))].store (v, std::memory_order_relaxed);
        }
        scopeWrite.store ((w + numSamples) & (scopeSize - 1), std::memory_order_release);
    }
}

void MonSyntheProcessor::copyRecentSamples (float* dest, int numSamples) const
{
    const int n = juce::jmin (numSamples, scopeSize);
    const int end = scopeWrite.load (std::memory_order_acquire);

    for (int k = 0; k < n; ++k)
        dest[k] = scopeRing[(size_t) ((end - n + k + scopeSize) & (scopeSize - 1))].load (std::memory_order_relaxed);
}

//==============================================================================
// Effets : chorus -> delay -> reverb -> volume général
//==============================================================================
void MonSyntheProcessor::applyEffects (juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numCh = juce::jmin (2, buffer.getNumChannels());
    const double sr = getSampleRate();

    // Chorus
    const float chorusMix = param ("chorusMix");
    if (chorusMix > 0.001f && numCh == 2)
    {
        chorus.setRate (param ("chorusRate"));
        chorus.setDepth (param ("chorusDepth"));
        chorus.setMix (chorusMix);

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        chorus.process (context);
    }

    // Delay stéréo (le canal droit est un peu plus long)
    const float delayMix = param ("delayMix");
    if (delayMix > 0.001f)
    {
        const float feedback = param ("delayFeedback");
        const float timeL = (float) (param ("delayTime") * 0.001 * sr);
        const float timeR = timeL * 1.08f;

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* x = buffer.getWritePointer (ch);
            const float time = ch == 0 ? timeL : timeR;

            for (int i = 0; i < numSamples; ++i)
            {
                const float in = x[i];
                const float d = delayLine.popSample (ch, time, true);
                delayLine.pushSample (ch, in + d * feedback);
                x[i] = in + d * delayMix;
            }
        }
    }

    // Reverb
    const float reverbMix = param ("reverbMix");
    if (reverbMix > 0.001f)
    {
        juce::Reverb::Parameters rp;
        rp.roomSize   = param ("reverbSize");
        rp.damping    = param ("reverbDamp");
        rp.wetLevel   = 0.33f * reverbMix;
        rp.dryLevel   = 0.5f * (1.0f - 0.4f * reverbMix);
        rp.width      = 1.0f;
        rp.freezeMode = 0.0f;
        reverb.setParameters (rp);

        if (buffer.getNumChannels() >= 2)
            reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), numSamples);
        else
            reverb.processMono (buffer.getWritePointer (0), numSamples);
    }

    // Volume général (avec rampe pour éviter les clics)
    const float master = param ("master");
    buffer.applyGainRamp (0, numSamples, lastMaster, master);
    lastMaster = master;
}

//==============================================================================
// Séquenceur : génération des notes MIDI (synchronisé sur l'hôte quand il joue)
//==============================================================================
void MonSyntheProcessor::generateSequencerMidi (juce::MidiBuffer& midi, int numSamples)
{
    const bool enabled = param ("seqOn") > 0.5f;

    if (! enabled)
    {
        if (seqNote >= 0)
        {
            midi.addEvent (juce::MidiMessage::noteOff (1, seqNote), 0);
            seqNote = -1;
        }
        currentStep = -1;
        return;
    }

    // Tempo et position : ceux de l'hôte s'il joue, sinon horloge interne
    double bpm = param ("seqTempo");
    double ppqStart = freeBeat;

    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (pos->getIsPlaying())
                if (auto ppq = pos->getPpqPosition())
                {
                    ppqStart = *ppq;
                    if (auto hostBpm = pos->getBpm())
                        bpm = *hostBpm;
                }

    static const double rates[] = { 0.5, 0.25, 0.125 };      // 1/8, 1/16, 1/32 (en temps)
    const double stepBeats = rates[juce::jlimit (0, 2, (int) param ("seqRate"))];
    const int bars = juce::jlimit (1, StepSequencerData::maxBars, (int) param ("seqBars") + 1);
    const int totalSteps = bars * StepSequencerData::stepsPerBar;

    const double samplesPerBeat = 60.0 / juce::jmax (1.0, bpm) * getSampleRate();
    const double gate = param ("seqGate");
    const auto gateSamples = (juce::int64) juce::jmax (1.0, gate * stepBeats * samplesPerBeat);

    const double ppqEnd = ppqStart + (double) numSamples / samplesPerBeat;
    constexpr double eps = 1.0e-6;
    const juce::int64 base = sampleClock;

    auto flushOff = [&] (juce::int64 upTo)
    {
        if (seqNote < 0)
            return;

        const juce::int64 rel = seqNoteOffAbs - base;
        if (rel <= upTo)
        {
            midi.addEvent (juce::MidiMessage::noteOff (1, seqNote), (int) juce::jmax<juce::int64> (0, rel));
            seqNote = -1;
        }
    };

    for (juce::int64 k = (juce::int64) std::ceil ((ppqStart - eps) / stepBeats);
         (double) k * stepBeats < ppqEnd - eps; ++k)
    {
        const double offBeats = (double) k * stepBeats - ppqStart;
        const int offset = juce::jlimit (0, numSamples - 1, (int) std::round (offBeats * samplesPerBeat));
        const int step = (int) (((k % totalSteps) + totalSteps) % totalSteps);

        currentStep = step;

        flushOff (offset);
        if (seqNote >= 0)                       // note précédente encore tenue : on la coupe
        {
            midi.addEvent (juce::MidiMessage::noteOff (1, seqNote), offset);
            seqNote = -1;
        }

        if (sequencer.on[(size_t) step])
        {
            seqNote = sequencer.note[(size_t) step];
            midi.addEvent (juce::MidiMessage::noteOn (1, seqNote, 0.8f), offset);
            seqNoteOffAbs = base + offset + gateSamples;
        }
    }

    flushOff ((juce::int64) numSamples - 1);
    freeBeat = ppqEnd;
    sampleClock += numSamples;
}

//==============================================================================
// État complet (réglages + séquenceur) et banque de presets
//==============================================================================
juce::ValueTree MonSyntheProcessor::makeState()
{
    auto state = apvts.copyState();
    state.removeChild (state.getChildWithName ("SEQ"), nullptr);
    state.appendChild (sequencer.toValueTree(), nullptr);
    return state;
}

void MonSyntheProcessor::applyState (const juce::ValueTree& tree)
{
    if (! tree.hasType (apvts.state.getType()))
        return;

    auto copy = tree.createCopy();

    const auto seq = copy.getChildWithName ("SEQ");
    if (seq.isValid())
    {
        sequencer.fromValueTree (seq);
        copy.removeChild (seq, nullptr);
    }

    // Chemins des wavetables importées, mémorisés dans le preset ou le projet
    juce::String paths[WavetableBank::numUserTables];
    for (int slot = 0; slot < WavetableBank::numUserTables; ++slot)
        paths[slot] = copy.getProperty ("userTable" + juce::String (slot + 1)).toString();

    apvts.replaceState (copy);

    for (int slot = 0; slot < WavetableBank::numUserTables; ++slot)
    {
        if (paths[slot].isNotEmpty() && paths[slot] != userTablePaths[slot])
        {
            juce::String error;
            loadUserWavetable (slot, juce::File (paths[slot]), error);     // si le fichier a disparu, on garde l'état actuel
        }
    }
}

bool MonSyntheProcessor::loadUserWavetable (int slot, const juce::File& file, juce::String& error)
{
    if (! juce::isPositiveAndBelow (slot, WavetableBank::numUserTables))
    {
        error = "Emplacement invalide.";
        return false;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader (formats.createReaderFor (file));
    if (reader == nullptr)
    {
        error = "Fichier audio illisible.";
        return false;
    }

    const juce::int64 length = reader->lengthInSamples;
    if (length < 32)
    {
        error = "Fichier trop court.";
        return false;
    }
    if (length > 4194304)
    {
        error = "Fichier trop long (4 millions d'echantillons maximum).";
        return false;
    }

    // Lecture et passage en mono
    juce::AudioBuffer<float> audio ((int) juce::jmax (1u, reader->numChannels), (int) length);
    reader->read (&audio, 0, (int) length, 0, true, true);

    std::vector<float> mono ((size_t) length, 0.0f);
    const float channelGain = 1.0f / (float) audio.getNumChannels();
    for (int ch = 0; ch < audio.getNumChannels(); ++ch)
    {
        const float* in = audio.getReadPointer (ch);
        for (int i = 0; i < (int) length; ++i)
            mono[(size_t) i] += in[i] * channelGain;
    }

    // Taille d'une forme d'onde : 2048 en général (format de Serum), sinon 1024, 4096 ...
    constexpr int N = WavetableBank::tableSize;
    int frameLen = 0;
    for (int candidate : { 2048, 1024, 4096, 512, 8192, 256 })
    {
        if (length % candidate == 0)
        {
            frameLen = candidate;
            break;
        }
    }

    if (frameLen == 0)
    {
        if (length > 16384)
        {
            error = "Longueur non reconnue : il faut un multiple de 2048 echantillons (ou un cycle unique court).";
            return false;
        }
        frameLen = (int) length;                       // un seul cycle, de longueur quelconque
    }

    const int srcFrames = (int) (length / frameLen);
    const int outFrames = juce::jmin (srcFrames, WavetableBank::maxUserFrames);
    std::vector<float> frames ((size_t) outFrames * N);

    for (int f = 0; f < outFrames; ++f)
    {
        const int src = outFrames == 1 ? 0 : (int) std::round ((double) f * (srcFrames - 1) / (outFrames - 1));
        const float* in = mono.data() + (size_t) src * (size_t) frameLen;
        float* out = frames.data() + (size_t) f * N;

        for (int i = 0; i < N; ++i)                    // remise à 2048 échantillons (le cycle boucle sur lui-même)
        {
            const double pos = (double) i * frameLen / N;
            const int i0 = (int) pos;
            const float fr = (float) (pos - i0);
            const int i1 = (i0 + 1) % frameLen;
            out[i] = in[i0] + (in[i1] - in[i0]) * fr;
        }
    }

    bank.setUserTable (slot, frames, outFrames);

    userTableNames[slot] = file.getFileNameWithoutExtension();
    userTablePaths[slot] = file.getFullPathName();
    apvts.state.setProperty ("userTable" + juce::String (slot + 1), userTablePaths[slot], nullptr);
    return true;
}

juce::String MonSyntheProcessor::getUserTableName (int slot) const
{
    if (! juce::isPositiveAndBelow (slot, WavetableBank::numUserTables) || userTableNames[slot].isEmpty())
        return "(vide)";

    return userTableNames[slot];
}

void MonSyntheProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = makeState().createXml())
        copyXmlToBinary (*xml, dest);
}

void MonSyntheProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        applyState (juce::ValueTree::fromXml (*xml));
}

juce::File MonSyntheProcessor::getPresetDirectory()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
               .getChildFile ("MS1")
               .getChildFile ("Presets");
}

juce::StringArray MonSyntheProcessor::getPresetNames() const
{
    juce::StringArray names;
    for (auto& f : getPresetDirectory().findChildFiles (juce::File::findFiles, false, "*.mspreset"))
        names.add (f.getFileNameWithoutExtension());

    names.sortNatural();
    return names;
}

bool MonSyntheProcessor::savePreset (const juce::String& name)
{
    const auto legal = juce::File::createLegalFileName (name.trim());
    if (legal.isEmpty())
        return false;

    const auto dir = getPresetDirectory();
    if (! dir.createDirectory())
        return false;

    if (auto xml = makeState().createXml())
        return xml->writeTo (dir.getChildFile (legal + ".mspreset"));

    return false;
}

bool MonSyntheProcessor::loadPreset (const juce::String& name)
{
    const auto file = getPresetDirectory().getChildFile (name + ".mspreset");
    if (! file.existsAsFile())
        return false;

    if (auto xml = juce::XmlDocument::parse (file))
    {
        applyState (juce::ValueTree::fromXml (*xml));
        return true;
    }

    return false;
}

bool MonSyntheProcessor::deletePreset (const juce::String& name)
{
    return getPresetDirectory().getChildFile (name + ".mspreset").deleteFile();
}

//==============================================================================
// Matrice de modulation : noms des sources et des cibles
//==============================================================================
juce::StringArray MonSyntheProcessor::warpModeNames()
{
    return { "OFF", "BEND+", "BEND-", "SYNC", "MIRROR" };
}

juce::StringArray MonSyntheProcessor::lfoSyncNames()
{
    return { "LIBRE", "4 MES", "2 MES", "1 MES", "1/2", "1/4", "1/8", "1/16", "1/32" };
}

juce::StringArray MonSyntheProcessor::modSourceNames()
{
    return { "---", "LFO", "ENV MOD", "ENV FILTRE", "VELOCITE", "NOTE", "MOLETTE", "ALEA",
             "LFO 2", "MACRO 1", "MACRO 2", "MACRO 3", "MACRO 4" };
}

juce::StringArray MonSyntheProcessor::modTargetNames()
{
    return { "---", "POSITION A", "POSITION B", "CUTOFF", "RESO", "HAUTEUR", "DESACCORD", "NIVEAU B", "DRIVE", "VOLUME",
             "QTE ENV FILTRE", "QTE ENV MOD", "DECAY VOL", "DECAY FILTRE", "WARP A", "WARP B", "FM", "RING" };
}

//==============================================================================
// Presets d'usine
//   tableA / tableB : 0 BASIQUE 1 VOCAL 2 PWM 3 SPECTRE 4 ORGUE 5 RESONANCE 6 PEIGNE 7 CHAOS
//   filterType : 0 LP12  1 LP24  2 HP  3 BP        lfoShape : 0 sinus 1 triangle 2 saw 3 carré
//   mxNSrc : 1 LFO 2 ENV MOD 3 ENV FILTRE 4 VELOCITE 5 NOTE 6 MOLETTE 7 ALEA
//   mxNDst : 1 POS A 2 POS B 3 CUTOFF 4 RESO 5 HAUTEUR 6 DESACCORD 7 NIVEAU B 8 DRIVE 9 VOLUME
//==============================================================================
struct FactoryPreset
{
    const char* name;
    const char* category;
    std::vector<std::pair<const char*, float>> values;
};

static const std::vector<FactoryPreset>& factoryPresets()
{
    static const std::vector<FactoryPreset> presets =
    {
        { "Init", "INIT", {} },

        { "Sub Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 0.9f }, { "drive", 0.15f },
            { "fltEnvAmount", 0.3f }, { "fltAttack", 0.003f }, { "fltDecay", 0.25f }, { "fltSustain", 0.4f },
            { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },

        { "Wobble Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 10.0f },
            { "spread", 0.4f }, { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 4.5f }, { "drive", 0.35f }, { "lfoShape", 1.0f }, { "lfoRate", 2.2f }, { "lfoDepth", 0.0f },
            { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.55f },
            { "attack", 0.005f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f } } },

        { "Classic Pluck", "PLUCKS", {
            { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "spread", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 500.0f }, { "resonance", 2.2f }, { "fltEnvAmount", 0.75f },
            { "fltAttack", 0.002f }, { "fltDecay", 0.28f }, { "fltSustain", 0.0f },
            { "attack", 0.002f }, { "decay", 0.4f }, { "sustain", 0.0f }, { "release", 0.25f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velFilter", 0.4f },
            { "chorusMix", 0.25f }, { "delayMix", 0.15f }, { "delayTime", 320.0f }, { "reverbMix", 0.15f } } },

        { "Supersaw Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.07f }, { "position", 0.43f }, { "unison", 7.0f }, { "detune", 32.0f },
            { "spread", 0.9f }, { "oscBLevel", 0.5f }, { "oscBPos", 0.43f }, { "oscBOct", -1.0f }, { "oscBTune", 7.0f },
            { "cutoff", 6000.0f }, { "resonance", 1.0f }, { "filterType", 0.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.3f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "chorusMix", 0.2f }, { "delayMix", 0.22f }, { "delayTime", 380.0f }, { "delayFeedback", 0.4f },
            { "reverbMix", 0.2f }, { "reverbSize", 0.6f } } },

        { "Warm Pad", "PADS", {
            { "position", 0.25f }, { "unison", 5.0f }, { "detune", 18.0f }, { "spread", 0.85f },
            { "oscBLevel", 0.45f }, { "oscBPos", 0.15f }, { "oscBTune", 6.0f },
            { "cutoff", 2200.0f }, { "resonance", 0.9f },
            { "attack", 1.2f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 1.8f },
            { "modAttack", 2.5f }, { "modDecay", 2.5f }, { "modSustain", 0.5f }, { "modAmount", 0.25f },
            { "lfoRate", 0.25f }, { "lfoDepth", 0.08f },
            { "chorusMix", 0.4f }, { "chorusRate", 0.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.85f }, { "reverbDamp", 0.55f } } },

        { "Vowel Choir", "PADS", {
            { "tableA", 1.0f }, { "position", 0.1f }, { "lfoShape", 1.0f }, { "lfoRate", 0.35f }, { "lfoDepth", 0.4f },
            { "modAmount", 0.0f }, { "unison", 4.0f }, { "detune", 12.0f }, { "spread", 0.7f }, { "cutoff", 7000.0f },
            { "attack", 0.35f }, { "decay", 0.5f }, { "sustain", 0.85f }, { "release", 0.9f },
            { "reverbMix", 0.35f }, { "reverbSize", 0.75f }, { "chorusMix", 0.25f } } },

        { "PWM Strings", "PADS", {
            { "tableA", 2.0f }, { "position", 0.3f }, { "lfoShape", 0.0f }, { "lfoRate", 0.6f }, { "lfoDepth", 0.35f },
            { "modAmount", 0.0f }, { "unison", 4.0f }, { "detune", 14.0f }, { "spread", 0.85f },
            { "cutoff", 3500.0f }, { "resonance", 0.8f },
            { "attack", 0.5f }, { "decay", 0.6f }, { "sustain", 0.8f }, { "release", 1.0f },
            { "chorusMix", 0.5f }, { "chorusRate", 0.7f }, { "reverbMix", 0.3f } } },

        { "Drawbar Organ", "CLAVIERS", {
            { "tableA", 4.0f }, { "position", 0.55f }, { "unison", 1.0f }, { "spread", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.004f }, { "decay", 0.1f }, { "sustain", 1.0f }, { "release", 0.08f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velAmp", 0.2f },
            { "chorusMix", 0.35f }, { "chorusRate", 4.5f }, { "chorusDepth", 0.3f }, { "reverbMix", 0.12f } } },

        { "Electric Keys", "CLAVIERS", {
            { "position", 0.1f }, { "oscBLevel", 0.35f }, { "oscBPos", 0.43f }, { "oscBOct", 1.0f },
            { "unison", 2.0f }, { "detune", 5.0f }, { "cutoff", 3500.0f },
            { "fltEnvAmount", 0.4f }, { "fltDecay", 0.5f }, { "fltSustain", 0.1f },
            { "attack", 0.003f }, { "decay", 1.4f }, { "sustain", 0.15f }, { "release", 0.5f },
            { "velAmp", 1.0f }, { "velFilter", 0.6f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.2f } } },

        { "Acid Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 350.0f }, { "resonance", 7.5f }, { "drive", 0.4f },
            { "fltEnvAmount", 0.85f }, { "fltAttack", 0.002f }, { "fltDecay", 0.22f }, { "fltSustain", 0.05f },
            { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 0.8f }, { "release", 0.1f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "delayMix", 0.25f }, { "delayTime", 375.0f }, { "delayFeedback", 0.45f } } },

        { "Glass Bells", "PLUCKS", {
            { "tableA", 6.0f }, { "position", 0.35f }, { "unison", 2.0f }, { "detune", 6.0f }, { "cutoff", 12000.0f },
            { "resonance", 0.7f }, { "attack", 0.002f }, { "decay", 2.0f }, { "sustain", 0.0f }, { "release", 1.5f },
            { "modAttack", 0.002f }, { "modDecay", 1.2f }, { "modSustain", 0.0f }, { "modAmount", 0.5f },
            { "lfoDepth", 0.0f }, { "reverbMix", 0.45f }, { "reverbSize", 0.8f }, { "delayMix", 0.2f } } },

        { "Chaos Texture", "TEXTURES", {
            { "tableA", 7.0f }, { "position", 0.2f }, { "lfoShape", 0.0f }, { "lfoRate", 0.15f }, { "lfoDepth", 0.6f },
            { "unison", 5.0f }, { "detune", 30.0f }, { "spread", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 2.0f },
            { "filterType", 3.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 2.5f },
            { "mx1Src", 7.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "modAmount", 0.0f },
            { "chorusMix", 0.4f }, { "delayMix", 0.3f }, { "delayTime", 520.0f }, { "delayFeedback", 0.5f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        { "Resonance Sweep", "TEXTURES", {
            { "tableA", 5.0f }, { "position", 0.2f }, { "modAttack", 1.5f }, { "modDecay", 1.5f }, { "modSustain", 0.6f },
            { "modAmount", 0.5f }, { "lfoDepth", 0.0f }, { "unison", 3.0f }, { "detune", 14.0f }, { "cutoff", 5000.0f },
            { "attack", 0.3f }, { "decay", 0.8f }, { "sustain", 0.8f }, { "release", 0.8f }, { "reverbMix", 0.3f } } },

        { "FM Bell", "PLUCKS", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.3f },
            { "unison", 1.0f }, { "spread", 0.0f }, { "cutoff", 12000.0f },
            { "attack", 0.002f }, { "decay", 1.8f }, { "sustain", 0.0f }, { "release", 1.2f },
            { "modAttack", 0.002f }, { "modDecay", 0.9f }, { "modSustain", 0.0f }, { "modAmount", 0.0f }, { "lfoDepth", 0.0f },
            { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.6f },
            { "reverbMix", 0.35f }, { "reverbSize", 0.8f } } },

        { "Sync Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "warpModeA", 3.0f }, { "warpAmtA", 0.25f },
            { "unison", 3.0f }, { "detune", 12.0f }, { "spread", 0.6f }, { "cutoff", 7000.0f },
            { "lfoShape", 1.0f }, { "lfoRate", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.35f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f }, { "delayMix", 0.2f } } },

        { "Ring Metal", "TEXTURES", {
            { "position", 0.43f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 15.0f }, { "ringAmount", 0.6f },
            { "unison", 2.0f }, { "detune", 6.0f }, { "cutoff", 5000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.003f }, { "decay", 0.9f }, { "sustain", 0.2f }, { "release", 0.6f },
            { "lfo2Rate", 0.3f }, { "mx1Src", 8.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.3f } } },

        { "Pulse Keys", "CLAVIERS", {
            { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "cutoff", 600.0f }, { "resonance", 2.0f },
            { "fltEnvAmount", 0.5f }, { "fltDecay", 0.35f }, { "fltSustain", 0.2f },
            { "attack", 0.003f }, { "decay", 0.8f }, { "sustain", 0.5f }, { "release", 0.4f },
            { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 10.0f }, { "mx1Amt", 0.6f } } },

        { "Velocity Brass", "LEADS", {
            { "position", 0.43f }, { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 0.0f }, { "cutoff", 700.0f },
            { "fltEnvAmount", 0.5f }, { "fltAttack", 0.12f }, { "fltDecay", 0.5f }, { "fltSustain", 0.5f },
            { "attack", 0.06f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f },
            { "velAmp", 0.6f }, { "velFilter", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "reverbMix", 0.15f } } }
    };

    return presets;
}

int MonSyntheProcessor::getNumFactoryPresets()
{
    return (int) factoryPresets().size();
}

juce::String MonSyntheProcessor::getFactoryPresetName (int index)
{
    const auto& presets = factoryPresets();
    return juce::isPositiveAndBelow (index, (int) presets.size()) ? juce::String (presets[(size_t) index].name)
                                                                   : juce::String();
}

juce::String MonSyntheProcessor::getFactoryPresetCategory (int index)
{
    const auto& presets = factoryPresets();
    return juce::isPositiveAndBelow (index, (int) presets.size()) ? juce::String (presets[(size_t) index].category)
                                                                   : juce::String();
}

void MonSyntheProcessor::loadFactoryPreset (int index)
{
    const auto& presets = factoryPresets();
    if (! juce::isPositiveAndBelow (index, (int) presets.size()))
        return;

    // 1) tout remettre aux valeurs par défaut (sauf le séquenceur et le volume général)
    for (auto* prm : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (prm))
            if (! rp->paramID.startsWith ("seq") && rp->paramID != "master")
                rp->setValueNotifyingHost (rp->getDefaultValue());

    // 2) appliquer les réglages du preset
    for (const auto& kv : presets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (kv.first))
            rp->setValueNotifyingHost (rp->convertTo0to1 (kv.second));
}

void MonSyntheProcessor::setThemeHue (float hue, bool save)
{
    themeHue = juce::jlimit (0.0f, 1.0f, hue);

    if (save)
    {
        const auto themeFile = getPresetDirectory().getParentDirectory().getChildFile ("theme.txt");
        themeFile.getParentDirectory().createDirectory();
        themeFile.replaceWithText (juce::String (themeHue.load(), 4));
    }
}

juce::AudioProcessorEditor* MonSyntheProcessor::createEditor()
{
    return new MonSyntheEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MonSyntheProcessor();
}
