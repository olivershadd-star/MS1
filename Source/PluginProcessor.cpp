#include "PluginProcessor.h"
#include "FactoryBank.h"
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
    for (int i = 0; i < 32; ++i)
        drawParams[(size_t) i] = apvts.getRawParameterValue ("draw" + juce::String (i + 1));
}

void SynthVoice::prepare (double sampleRate, int blockSize)
{
    sr = sampleRate;
    adsr.setSampleRate (sr);
    modEnv.setSampleRate (sr);
    fltEnv.setSampleRate (sr);
    position.reset (sr, 0.02);   // lissage de 20 ms pour éviter les clics
    oscDec.design (2);
    driveOs.reset();
    distOs.reset();

    juce::dsp::ProcessSpec spec { sr, (juce::uint32) blockSize, 2 };   // filtres stéréo
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    filter2.prepare (spec);
    filter2.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    filter2.setResonance (0.7071f);

    for (auto* f : { &f2a, &f2b, &f2c })
    {
        f->prepare (spec);
        f->setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    }
}

float SynthVoice::nextNoise (NoiseGen& n, int type, float tone)
{
    const float w = rng.nextFloat() * 2.0f - 1.0f;
    float x = w;

    if (type == 1)                       // rose (filtre de Paul Kellet)
    {
        n.b[0] = 0.99886f * n.b[0] + w * 0.0555179f;
        n.b[1] = 0.99332f * n.b[1] + w * 0.0750759f;
        n.b[2] = 0.96900f * n.b[2] + w * 0.1538520f;
        n.b[3] = 0.86650f * n.b[3] + w * 0.3104856f;
        n.b[4] = 0.55000f * n.b[4] + w * 0.5329522f;
        n.b[5] = -0.7616f * n.b[5] - w * 0.0168980f;
        x = (n.b[0] + n.b[1] + n.b[2] + n.b[3] + n.b[4] + n.b[5] + n.b[6] + w * 0.5362f) * 0.11f;
        n.b[6] = w * 0.115926f;
    }
    else if (type == 2)                  // sale : valeurs tenues de longueur aléatoire (crépitement numérique)
    {
        if (n.hold <= 0)
        {
            n.held = w;
            n.hold = 1 + rng.nextInt (12);
        }
        --n.hold;
        x = n.held;
    }

    const float k = 0.02f + 0.98f * tone * tone;        // ton : 1 = brillant, 0 = sombre
    n.lp += (x - n.lp) * k;
    return n.lp;
}

void SynthVoice::startNote (int midiNote, float vel, juce::SynthesiserSound*, int pitchWheelPosition)
{
    currentNote = targetNote = (double) midiNote;
    bendNorm = (float) (pitchWheelPosition - 8192) / 8192.0f;
    velocity = vel;
    noteRandom = rng.nextFloat() * 2.0f - 1.0f;
    keyDown = true;

    // Si la voix sonne encore (note volée ou queue longue), on la reprend sans coupure :
    // phases, filtres et LFO continuent, seule la hauteur change. Cela supprime le « tac » au début des notes.
    const bool cont = adsr.isActive();
    fastRel = false;

    // Départ de phase : ALEATOIRE (son large), FIXE (toujours identique : kicks et basses précises), LIBRE (on ne touche à rien)
    const int phaseMode = (int) param ("phaseMode");
    const double phase0 = (double) param ("oscPhase");
    if (cont && phaseMode != 1)             // reprise douce : on ne touche pas aux phases (sauf mode FIXE, pour les kicks)
    {
    }
    else if (phaseMode == 0)
    {
        phasesA[0] = phase0;
        phasesB[0] = phase0;
        for (size_t i = 1; i < phasesA.size(); ++i)
        {
            phasesA[i] = rng.nextDouble();       // phases aléatoires = son plus large
            phasesB[i] = rng.nextDouble();
        }
        phaseSub = 0.0;
    }
    else if (phaseMode == 1)
    {
        for (size_t i = 0; i < phasesA.size(); ++i)
            phasesA[i] = phasesB[i] = phase0;
        phaseSub = phase0;
    }

    if (phaseMode != 2 && ! cont)            // décalage de phase de B par rapport à A (présence, phasing)
    {
        const double offB = (double) param ("oscBPhase");
        for (auto& p : phasesB)
        {
            p += offB;
            p -= std::floor (p);
        }
    }

    pitchEnvVal = 1.0f;
    noiseEnvVal = 1.0f;
    if (! cont)
    {
        for (auto& n : noiseGen)
            n = NoiseGen();
        crushCount = 0;
        lfoPhase = 0.0;
        lfo2Phase = 0.0;
        lfo3Phase = 0.0;
        lfo4Phase = 0.0;
        drawPhase = 0.0;
        oscDec.reset();
        driveOs.reset();
        distOs.reset();
    }
    for (auto& b : combBuf)
        b.fill (0.0f);
    combPos = 0;

    // Corde pincee : on remplit la boucle avec une salve de bruit, filtree (brillance) puis peignee (position du pincement)
    strWrite = 0;
    strPrev = 0.0f;
    strBuf.fill (0.0f);
    if (param ("strLevel") > 0.001f && sr > 0.0)
    {
        const double f0 = 440.0 * std::pow (2.0, (currentNote - 69.0) / 12.0 + (double) param ("strTune") / 1200.0);
        const int n = juce::jlimit (4, strSize - 4, (int) std::ceil (sr / f0) + 2);
        const float bright = juce::jlimit (0.03f, 1.0f, param ("strBright") * (0.4f + 0.8f * velocity));
        float lp = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            lp += bright * ((rng.nextFloat() * 2.0f - 1.0f) - lp);
            strBuf[(size_t) i] = lp;
        }
        const int k = juce::jlimit (0, n - 1, (int) std::lround (param ("strPos") * 0.5f * (float) n));
        if (k > 0)
            for (int i = n - 1; i >= k; --i)
                strBuf[(size_t) i] -= strBuf[(size_t) (i - k)];
        float peak = 0.0001f;
        for (int i = 0; i < n; ++i)
            peak = juce::jmax (peak, std::abs (strBuf[(size_t) i]));
        for (int i = 0; i < n; ++i)
            strBuf[(size_t) i] *= 0.8f / peak;
        strWrite = n;
    }

    position.setCurrentAndTargetValue (param ("position"));
    if (! cont)
    {
        filter.reset();
        filter2.reset();
    }

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
        // Coupure (note volée ou arrêt d'urgence) : fondu de 5 ms au lieu d'un silence instantané.
        // Si une nouvelle note démarre sur cette voix, elle reprend le signal sans coupure (voir startNote).
        fastRel = true;
        adsr.noteOff();
        modEnv.noteOff();
        fltEnv.noteOff();
        if (! adsr.isActive())
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
    const float ampA = param ("attack"),    ampD = param ("decay"),    ampS = param ("sustain"),    ampR = fastRel ? 0.005f : param ("release");
    const float fltA = param ("fltAttack"), fltD = param ("fltDecay"), fltS = param ("fltSustain"), fltR = param ("fltRelease");
    adsr.setParameters   ({ ampA, ampD, ampS, ampR });
    modEnv.setParameters ({ param ("modAttack"), param ("modDecay"), param ("modSustain"), param ("modRelease") });
    fltEnv.setParameters ({ fltA, fltD, fltS, fltR });

    // --- Qualité : 0 ECO (aucun suréchantillonnage), 1 NORMAL (2x), 2 HAUTE (4x sur les oscillateurs)
    {
        const int q = (int) param ("quality");
        osF = q >= 2 ? 4 : (q == 1 ? 2 : 1);
        if (osF > 1 && oscDec.factor != osF)
            oscDec.design (osF);
    }

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
    const bool notch = (filterType == 4);        // NOTCH = passe-bas + passe-haut en parallele
    const bool comb  = (filterType == 5);        // COMB  = filtre en peigne accorde sur le cutoff
    filter2.setType (notch ? FilterType::highpass : FilterType::lowpass);
    if (notch)
        filter2.setResonance (baseRes);
    else
        filter2.setResonance (0.7071f);

    // --- Filtre 2 (0 off, 1 LP, 2 HP, 3 BP, 4 NOTCH, 5 FORMANTS) : en série ou en parallèle du filtre 1
    const int   f2Type   = (int) param ("f2Type");
    const bool  f2Par    = param ("f2Mode") > 0.5f;
    const float f2Cut    = param ("f2Cutoff");
    const float f2Res    = param ("f2Res");
    const float f2Mix    = param ("f2Mix");
    const float f2Vowel  = param ("f2Vowel");
    {
        using FT = juce::dsp::StateVariableTPTFilterType;
        switch (f2Type)
        {
            case 1: f2a.setType (FT::lowpass);  break;
            case 2: f2a.setType (FT::highpass); break;
            case 3: f2a.setType (FT::bandpass); break;
            case 4: f2a.setType (FT::lowpass);  f2b.setType (FT::highpass); break;
            case 5: f2a.setType (FT::bandpass); f2b.setType (FT::bandpass); f2c.setType (FT::bandpass); break;
            default: break;
        }
        const float q = f2Type == 5 ? 2.0f + f2Res * 2.5f : f2Res;
        f2a.setResonance (q);
        f2b.setResonance (f2Type == 5 ? q : f2Res);
        f2c.setResonance (q);
    }

    // --- Vélocité
    const float velAmp    = param ("velAmp");
    const float velFilter = param ("velFilter");
    const float velTerm   = velFilter * (velocity - 0.5f) * 6.0f;                 // en octaves
    const float ampVel    = (1.0f - velAmp) + velAmp * velocity;
    const float wheel     = modWheel.load();

    // --- Bruit, enveloppe de hauteur, distorsion
    const float noiseLevel = param ("noiseLevel");
    const int   noiseType  = (int) param ("noiseType");
    const float noiseTone  = param ("noiseTone");
    const float noiseCoef  = std::exp (-1.0f / (juce::jmax (0.002f, param ("noiseDecay")) * (float) sr));
    const float pitchAmt   = param ("pitchEnvAmt");
    const float pitchCoef  = std::exp (-1.0f / (juce::jmax (0.002f, param ("pitchEnvDecay")) * (float) sr));
    const int   distType   = (int) param ("distType");
    const float distAmt    = param ("distAmt");
    const float distMix    = param ("distMix");

    // --- Matrice de modulation : sources 0..15, cibles 0..20
    struct Slot { int src; int dst; float amt; };
    std::array<Slot, numMixSlots> slots {};
    int numSlots = 0;
    bool resonanceModulated = false;
    bool envTimeModulated = false;
    bool pitchModulated = false;

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
            if (sl.dst == 20)
                pitchModulated = true;
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
    const int    lfo3Shape = (int) param ("lfo3Shape");
    const double lfo3Inc   = lfoIncrement (param ("lfo3Rate"), (int) param ("lfo3Sync"));
    const int    lfo4Shape = (int) param ("lfo4Shape");
    const double lfo4Inc   = lfoIncrement (param ("lfo4Rate"), (int) param ("lfo4Sync"));
    const double drawInc   = lfoIncrement (param ("dwRate"), (int) param ("dwSync"));
    const int    drawSteps = juce::jlimit (2, 32, (int) std::round (param ("dwSteps")));
    const float  drawSmooth = param ("dwSmooth");
    const std::array<float, 4> macros { param ("macro1"), param ("macro2"), param ("macro3"), param ("macro4") };

    // --- Tables
    const int tabA = (int) param ("tableA");
    const int tabB = (int) param ("tableB");

    // --- Unison : N oscillateurs désaccordés et répartis dans le champ stéréo
    const int   uni    = juce::jlimit (1, maxUnison, (int) std::round (param ("unison")));
    const float detune = param ("detune");
    const float spread = param ("spread");
    const float norm   = 1.0f / std::sqrt ((float) uni);
    const float uniBlend = param ("uniBlend");

    std::array<float, maxUnison> offsets {};
    std::array<float, maxUnison> gainL {}, gainR {};

    for (int u = 0; u < uni; ++u)
    {
        const float offset = uni > 1 ? -1.0f + 2.0f * (float) u / (float) (uni - 1) : 0.0f;
        offsets[(size_t) u] = offset;

        const float angle = (offset * spread + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        const float side = std::abs (offset) < 0.001f ? 1.0f : uniBlend;      // blend : niveau des voix laterales
        gainL[(size_t) u] = std::cos (angle) * juce::MathConstants<float>::sqrt2 * norm * side;
        gainR[(size_t) u] = std::sin (angle) * juce::MathConstants<float>::sqrt2 * norm * side;
    }

    // --- Oscillateur B et sub
    const float  levelA  = param ("oscALevel");
    const float  voiceGain = juce::Decibels::decibelsToGain (param ("voiceGain"));
    const float  levelB  = param ("oscBLevel");
    const float  posB    = param ("oscBPos");
    const double factorA = std::pow (2.0, std::round ((double) param ("oscAOct")) + (double) param ("oscATune") / 1200.0);
    const double factorB = std::pow (2.0, std::round ((double) param ("oscBOct")) + (double) param ("oscBTune") / 1200.0);
    const float  subAmt  = param ("subLevel");
    const float  strAmt  = param ("strLevel");
    const float  strDecaySec = param ("strDecay");
    const float  strA = 0.5f + 0.5f * param ("strDamp");
    const double strFactor = std::pow (2.0, (double) param ("strTune") / 1200.0);

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
        const double freqBase = 440.0 * std::pow (2.0, (note - 69.0) / 12.0);
        const double freq  = freqBase * factorA;
        const double freqB = freqBase * factorB;

        for (int u = 0; u < uni; ++u)
        {
            const auto k = (size_t) u;
            const double ratio = std::pow (2.0, (double) (offsets[k] * detuneNow) / 1200.0);
            incA[k] = freq / sr * ratio;
            incB[k] = freqB / sr * ratio;
        }
        incSub = freqBase / sr * 0.5;                              // sub : une octave en dessous
        if (strAmt > 0.001f)
        {
            const double fs = freqBase * strFactor;
            strDelay = (float) juce::jlimit (2.0, (double) (strSize - 4), sr / fs - 0.5);
            strGain = std::exp (-6.9078f / juce::jmax (0.05f, strDecaySec * (float) fs));    // -60 dB apres strDecaySec
        }

        const double maxRatio = std::pow (2.0, (double) detuneNow / 1200.0);
        mipA = WavetableBank::levelForFrequency (freq * maxRatio);
        mipB = WavetableBank::levelForFrequency (freqB * maxRatio);
    };

    const int pitchStep = (std::abs (pitchAmt) > 0.01f || pitchModulated) ? 8 : 32;     // mise à jour plus fine pour une chute de hauteur rapide

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

        const float lfo3 = lfoValue (lfo3Shape, lfo3Phase);
        lfo3Phase += lfo3Inc;
        if (lfo3Phase >= 1.0)
            lfo3Phase -= 1.0;

        const float lfo4 = lfoValue (lfo4Shape, lfo4Phase);
        lfo4Phase += lfo4Inc;
        if (lfo4Phase >= 1.0)
            lfo4Phase -= 1.0;

        // LFO dessiné : 2 à 32 pas, du escalier (lissage 0) à la rampe continue (lissage 1)
        float drawVal;
        {
            const double x = drawPhase * (double) drawSteps;
            const int i0 = juce::jmin (drawSteps - 1, (int) x);
            const float fr = (float) (x - (double) i0);
            const float a = drawParams[(size_t) i0]->load();
            const float b = drawParams[(size_t) ((i0 + 1) % drawSteps)]->load();
            const float t = drawSmooth <= 0.001f ? 0.0f : juce::jlimit (0.0f, 1.0f, (fr - (1.0f - drawSmooth)) / drawSmooth);
            drawVal = a + (b - a) * (t * t * (3.0f - 2.0f * t));
            drawPhase += drawInc;
            if (drawPhase >= 1.0)
                drawPhase -= 1.0;
        }

        const float mEnv = modEnv.getNextSample();
        const float fEnv = fltEnv.getNextSample();

        std::array<float, 17> src {};
        src[1] = lfo;  src[2] = mEnv;  src[3] = fEnv;  src[4] = velocity;
        src[5] = keyNorm;  src[6] = wheel;  src[7] = noteRandom;
        src[8] = lfo2;
        src[9] = macros[0];  src[10] = macros[1];  src[11] = macros[2];  src[12] = macros[3];
        src[13] = pitchEnvVal;
        src[14] = lfo3;  src[15] = lfo4;  src[16] = drawVal;

        std::array<float, 23> tgt {};
        for (int s = 0; s < numSlots; ++s)
        {
            const auto& sl = slots[(size_t) s];
            tgt[(size_t) sl.dst] += sl.amt * src[(size_t) sl.src];
        }

        if ((i % pitchStep) == 0)
        {
            modPitch  = tgt[5] * 12.0f + (pitchAmt + tgt[20] * 48.0f) * pitchEnvVal;
            detuneNow = juce::jlimit (0.0f, 100.0f, detune + tgt[6] * 50.0f);
            updatePitch (pitchStep);

            if (envTimeModulated)       // un LFO ou une macro peut faire varier la durée des enveloppes
            {
                adsr.setParameters   ({ ampA, juce::jlimit (0.001f, 20.0f, ampD * std::exp2 (tgt[12] * 3.0f)), ampS, ampR });
                fltEnv.setParameters ({ fltA, juce::jlimit (0.001f, 20.0f, fltD * std::exp2 (tgt[13] * 3.0f)), fltS, fltR });
            }
        }

        pitchEnvVal *= pitchCoef;
        const float noiseEnvNow = noiseEnvVal;
        noiseEnvVal *= noiseCoef;

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
        auto oscStage = [&] (double step, float& oL, float& oR)
        {
        oL = oR = 0.0f;
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

            float s = levelA * sA + levelBNow * sB;
            if (ring > 0.001f)
                s += ring * (sA * sB * 1.4f - s);

            oL += s * gainL[k];
            oR += s * gainR[k];

            phasesA[k] += incA[k] * step;
            if (phasesA[k] >= 1.0) phasesA[k] -= 1.0;
            phasesB[k] += incB[k] * step;
            if (phasesB[k] >= 1.0) phasesB[k] -= 1.0;
        }
        };

        if (osF > 1)
        {
            for (int j = 0; j < osF; ++j)
            {
                float a, b;
                oscStage (1.0 / (double) osF, a, b);
                oscDec.push (a, b);
            }
            oscDec.out (l, r);
        }
        else
            oscStage (1.0, l, r);

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

        // Corde pincee : ligne a retard + filtre passe-bas dans la boucle
        if (strAmt > 0.001f)
        {
            const float rp = (float) strWrite - strDelay;
            const int i0 = (int) std::floor (rp);
            const float fr = rp - (float) i0;
            const float s = strBuf[(size_t) (i0 & (strSize - 1))] * (1.0f - fr)
                          + strBuf[(size_t) ((i0 + 1) & (strSize - 1))] * fr;
            const float filtered = strA * s + (1.0f - strA) * strPrev;
            strPrev = s;
            const float y = filtered * strGain * (keyDown ? 1.0f : 0.995f);
            strBuf[(size_t) (strWrite & (strSize - 1))] = y;
            ++strWrite;
            l += y * strAmt;
            r += y * strAmt;
        }

        // Bruit (avec sa propre enveloppe de decay)
        const float nLevel = juce::jlimit (0.0f, 1.0f, noiseLevel + tgt[19]);
        if (nLevel > 0.001f)
        {
            const float nAmp = nLevel * noiseEnvNow * 0.5f;
            l += nextNoise (noiseGen[0], noiseType, noiseTone) * nAmp;
            r += nextNoise (noiseGen[1], noiseType, noiseTone) * nAmp;
        }

        // Saturation avant le filtre
        const float driveNow = juce::jlimit (0.0f, 1.0f, drive + tgt[8]);
        if (osF > 1)
        {
            const float d = 1.0f + driveNow * 10.0f;
            const float inv = 1.0f / std::tanh (d);
            float wl, wr, d0l, d0r;
            driveOs.run (l, r, driveNow > 0.001f, [&] (float x) { return std::tanh (x * d) * inv; }, wl, wr, d0l, d0r);
            l = d0l + driveNow * (wl - d0l);
            r = d0r + driveNow * (wr - d0r);
        }
        else if (driveNow > 0.001f)
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
        if (twoPole || notch)
            filter2.setCutoffFrequency (cutoff);
        if (resonanceModulated)
        {
            const float resNow = juce::jlimit (0.5f, 10.0f, baseRes + tgt[4] * 4.0f);
            filter.setResonance (resNow);
            if (notch)
                filter2.setResonance (resNow);
        }

        const float preL = l, preR = r;       // signal avant le filtre 1 (entrée du filtre 2 en mode parallèle)

        if (comb)
        {
            // y[n] = x[n] + g * y[n - d], d = periode du cutoff (interpolation lineaire)
            const float resNow = juce::jlimit (0.5f, 10.0f, baseRes + (resonanceModulated ? tgt[4] * 4.0f : 0.0f));
            const float fb = juce::jlimit (0.0f, 0.93f, 0.25f + (resNow - 0.5f) * 0.07f);
            const float d = juce::jlimit (2.0f, 4090.0f, (float) sr / juce::jmax (40.0f, cutoff));
            const int di = (int) d;
            const float fr = d - (float) di;
            const float outGain = 1.0f - 0.5f * fb;
            float* in[2] = { &l, &r };
            for (int ch = 0; ch < 2; ++ch)
            {
                auto& buf = combBuf[(size_t) ch];
                const int p1 = (combPos - di + 4096) & 4095;
                const int p2 = (combPos - di - 1 + 4096) & 4095;
                const float delayed = buf[(size_t) p1] * (1.0f - fr) + buf[(size_t) p2] * fr;
                const float y = *in[ch] + fb * delayed;
                buf[(size_t) combPos] = y;
                *in[ch] = y * outGain;
            }
            combPos = (combPos + 1) & 4095;
        }
        else if (notch)
        {
            const float l0 = l, r0 = r;
            l = filter.processSample (0, l0) + filter2.processSample (0, l0);
            r = filter.processSample (1, r0) + filter2.processSample (1, r0);
        }
        else
        {
            l = filter.processSample (0, l);
            r = filter.processSample (1, r);
        }

        if (twoPole)
        {
            l = filter2.processSample (0, l);
            r = filter2.processSample (1, r);
        }

        // Filtre 2
        if (f2Type > 0 && f2Mix > 0.001f)
        {
            const float c2 = juce::jlimit (20.0f, 20000.0f, f2Cut * std::exp2 (tgt[21] * 5.0f));
            float inL = f2Par ? preL : l, inR = f2Par ? preR : r;
            float oL, oR;

            if (f2Type == 5)
            {
                static const float vw[5][3] = { { 800.0f, 1150.0f, 2900.0f }, { 400.0f, 1600.0f, 2700.0f }, { 350.0f, 1700.0f, 2700.0f },
                                                { 450.0f, 800.0f, 2830.0f }, { 325.0f, 700.0f, 2700.0f } };
                static const float vg[3] = { 1.6f, 1.2f, 0.8f };
                const float pos = juce::jlimit (0.0f, 1.0f, f2Vowel + tgt[22]) * 4.0f;
                const int i0 = juce::jlimit (0, 3, (int) pos);
                const float fr = pos - (float) i0;
                const float shift = juce::jlimit (0.25f, 4.0f, c2 / 1000.0f);
                juce::dsp::StateVariableTPTFilter<float>* fs[3] = { &f2a, &f2b, &f2c };
                oL = oR = 0.0f;
                for (int k = 0; k < 3; ++k)
                {
                    fs[k]->setCutoffFrequency (juce::jlimit (60.0f, 12000.0f, (vw[i0][k] * (1.0f - fr) + vw[i0 + 1][k] * fr) * shift));
                    oL += vg[k] * fs[k]->processSample (0, inL);
                    oR += vg[k] * fs[k]->processSample (1, inR);
                }
            }
            else if (f2Type == 4)
            {
                f2a.setCutoffFrequency (c2);
                f2b.setCutoffFrequency (c2);
                oL = f2a.processSample (0, inL) + f2b.processSample (0, inL);
                oR = f2a.processSample (1, inR) + f2b.processSample (1, inR);
            }
            else
            {
                f2a.setCutoffFrequency (c2);
                oL = f2a.processSample (0, inL);
                oR = f2a.processSample (1, inR);
            }

            l += f2Mix * (oL - l);
            r += f2Mix * (oR - r);
        }

        // Distorsion (après le filtre)
        const float dAmt = juce::jlimit (0.0f, 1.0f, distAmt + tgt[18]);
        if (osF > 1 && distType != 3)
        {
            float wl, wr, d0l, d0r;
            distOs.run (l, r, dAmt > 0.001f, [&] (float x) { return distShape (distType, dAmt, x); }, wl, wr, d0l, d0r);
            l = d0l + distMix * (wl - d0l);
            r = d0r + distMix * (wr - d0r);
        }
        else if (dAmt > 0.001f)
        {
            float wl, wr;
            if (distType == 3)
            {
                if (crushCount <= 0)
                {
                    crushHeld[0] = l;
                    crushHeld[1] = r;
                    crushCount = 1 + (int) (dAmt * dAmt * 24.0f);
                }
                --crushCount;
                wl = distShape (3, dAmt, crushHeld[0]);
                wr = distShape (3, dAmt, crushHeld[1]);
            }
            else
            {
                wl = distShape (distType, dAmt, l);
                wr = distShape (distType, dAmt, r);
            }
            l += distMix * (wl - l);
            r += distMix * (wr - r);
        }

        // Volume
        const float ampMod = juce::jlimit (0.0f, 2.0f, 1.0f + tgt[9]);
        const float env = adsr.getNextSample() * ampVel * ampMod * 0.45f * voiceGain;     // +5 dB par rapport à avant : les sons seuls (kicks, hats) étaient trop bas
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
    addFloat  ("oscALevel", "A Niveau", unit, 1.0f);

    // Oscillateur B + sub
    addChoice ("tableB",   "Table B", WavetableBank::tableNames(), 0);
    addFloat ("oscBPos",   "B Wavetable", unit, 0.5f);
    addInt   ("oscAOct",   "A Octave", -2, 2, 0);
    addFloat ("oscATune",  "A Accord (cents)", NormalisableRange<float> (-100.0f, 100.0f), 0.0f);
    addFloat ("uniBlend",  "Unison Blend", unit, 1.0f);
    addInt   ("oscBOct",   "B Octave", -2, 2, 0);
    addFloat ("oscBTune",  "B Accord (cents)", NormalisableRange<float> (-100.0f, 100.0f), 0.0f);
    addFloat ("oscBLevel", "B Niveau", unit, 0.0f);
    addFloat ("oscBPhase", "B Phase", unit, 0.0f);
    addFloat ("subLevel",  "Sub Niveau", unit, 0.0f);
    addFloat ("strLevel",  "Corde Niveau", unit, 0.0f);
    addFloat ("strDecay",  "Corde Duree (s)", NormalisableRange<float> (0.1f, 12.0f, 0.0f, 0.4f), 2.0f);
    addFloat ("strDamp",   "Corde Amorti", unit, 0.6f);
    addFloat ("strPos",    "Corde Position pincage", unit, 0.3f);
    addFloat ("strBright", "Corde Brillance", unit, 0.7f);
    addFloat ("strTune",   "Corde Accord (cents)", NormalisableRange<float> (-100.0f, 100.0f), 0.0f);

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
    addChoice ("filterType", "Filtre Type", StringArray { "LP 12", "LP 24", "HP", "BP", "NOTCH", "COMB" }, 0);
    // LFO dessiné
    addFloat  ("dwRate",   "Dessin Vitesse (Hz)", NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 1.0f);
    addChoice ("dwSync",   "Dessin Sync", lfoSyncNames(), 0);
    addFloat  ("dwSmooth", "Dessin Lissage", NormalisableRange<float> (0.0f, 1.0f), 0.0f);
    addFloat  ("dwSteps",  "Dessin Pas", NormalisableRange<float> (2.0f, 32.0f, 1.0f), 16.0f);
    for (int i = 1; i <= 32; ++i)
        addFloat (("draw" + String (i)).toRawUTF8(), ("Dessin " + String (i)).toRawUTF8(),
                  NormalisableRange<float> (-1.0f, 1.0f), 1.0f - 2.0f * (float) (i - 1) / 31.0f);

    addChoice ("f2Type", "Filtre 2 Type", StringArray { "OFF", "LP 12", "HP", "BP", "NOTCH", "FORMANTS" }, 0);
    addChoice ("f2Mode", "Filtre 2 Mode", StringArray { "SERIE", "PARALLELE" }, 0);
    addFloat  ("f2Cutoff", "Filtre 2 Cutoff", NormalisableRange<float> (20.0f, 20000.0f, 0.0f, 0.3f), 1000.0f);
    addFloat  ("f2Res",    "Filtre 2 Reso", NormalisableRange<float> (0.5f, 10.0f, 0.0f, 0.5f), 0.7f);
    addFloat  ("f2Mix",    "Filtre 2 Mix", NormalisableRange<float> (0.0f, 1.0f), 1.0f);
    addFloat  ("f2Vowel",  "Filtre 2 Voyelle", NormalisableRange<float> (0.0f, 1.0f), 0.0f);
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
    addChoice ("lfo3Shape", "LFO3 Forme", StringArray { "Sinus", "Triangle", "Saw", "Carre" }, 1);
    addFloat  ("lfo3Rate",  "LFO3 Vitesse (Hz)", NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 2.0f);
    addChoice ("lfo3Sync",  "LFO3 Sync", lfoSyncNames(), 0);
    addChoice ("lfo4Shape", "LFO4 Forme", StringArray { "Sinus", "Triangle", "Saw", "Carre" }, 2);
    addFloat  ("lfo4Rate",  "LFO4 Vitesse (Hz)", NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 0.25f);
    addChoice ("lfo4Sync",  "LFO4 Sync", lfoSyncNames(), 0);
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

    // EQ 8 bandes : pour chaque bande, on / type / fréquence / gain / Q
    {
        static const int   eqTypes[8] = { 0, 1, 2, 2, 2, 2, 3, 4 };
        static const float eqFreqs[8] = { 40.0f, 100.0f, 300.0f, 800.0f, 2000.0f, 5000.0f, 9000.0f, 18000.0f };
        for (int b = 1; b <= 8; ++b)
        {
            const String n = "eq" + String (b);
            addBool   ((n + "On").toRawUTF8(), ("EQ " + String (b) + " On").toRawUTF8(), false);
            addChoice ((n + "Type").toRawUTF8(), ("EQ " + String (b) + " Type").toRawUTF8(),
                       StringArray { "COUPE-BAS", "SHELF GRAVE", "CLOCHE", "SHELF AIGU", "COUPE-HAUT" }, eqTypes[b - 1]);
            addFloat  ((n + "Freq").toRawUTF8(), ("EQ " + String (b) + " Freq").toRawUTF8(),
                       NormalisableRange<float> (20.0f, 20000.0f, 0.0f, 0.3f), eqFreqs[b - 1]);
            addFloat  ((n + "Gain").toRawUTF8(), ("EQ " + String (b) + " Gain").toRawUTF8(),
                       NormalisableRange<float> (-15.0f, 15.0f), 0.0f);
            addFloat  ((n + "Q").toRawUTF8(), ("EQ " + String (b) + " Q").toRawUTF8(),
                       NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.4f), 0.71f);
        }
        addFloat ("eqOut", "EQ Sortie (dB)", NormalisableRange<float> (-12.0f, 12.0f), 0.0f);
    }

    // Rack d'effets : 4 emplacements
    for (int s = 1; s <= 4; ++s)
    {
        const String n = "fx" + String (s);
        const String nm = "FX " + String (s) + " ";
        addChoice ((n + "Type").toRawUTF8(), (nm + "Type").toRawUTF8(), FxRack::typeNames(), 0);
        addBool   ((n + "On").toRawUTF8(), (nm + "On").toRawUTF8(), true);
        addFloat  ((n + "Mix").toRawUTF8(), (nm + "Mix").toRawUTF8(), NormalisableRange<float> (0.0f, 1.0f), 0.5f);
        const char* ks[4] = { "A", "B", "C", "D" };
        for (auto* k : ks)
            addFloat ((n + k).toRawUTF8(), (nm + k).toRawUTF8(), NormalisableRange<float> (0.0f, 1.0f), 0.5f);
    }

    // Gain de voix et page MASTER (compresseur, limiteur, largeur)
    addFloat ("voiceGain", "Gain de voix (dB)", NormalisableRange<float> (-12.0f, 12.0f), 0.0f);
    addFloat ("compThresh",  "Comp Seuil (dB)", NormalisableRange<float> (-40.0f, 0.0f), 0.0f);
    addFloat ("compRatio",   "Comp Ratio", NormalisableRange<float> (1.0f, 20.0f, 0.0f, 0.5f), 3.0f);
    addFloat ("compAttack",  "Comp Attaque (ms)", NormalisableRange<float> (1.0f, 100.0f, 0.0f, 0.5f), 10.0f);
    addFloat ("compRelease", "Comp Release (ms)", NormalisableRange<float> (20.0f, 500.0f, 0.0f, 0.5f), 120.0f);
    addFloat ("compMakeup",  "Comp Makeup (dB)", NormalisableRange<float> (0.0f, 24.0f), 0.0f);
    addFloat ("compMix",     "Comp Mix", unit, 1.0f);
    addFloat ("limCeil",     "Limiteur Plafond (dB)", NormalisableRange<float> (-12.0f, 0.0f), -1.0f);
    addFloat ("width",       "Largeur stereo", NormalisableRange<float> (0.0f, 2.0f), 1.0f);

    // Vélocité
    addFloat ("velAmp",    "Velocite Volume", unit, 1.0f);
    addFloat ("velFilter", "Velocite Filtre", unit, 0.0f);

    // Bruit, hauteur (kicks), départ de phase, distorsion
    addFloat  ("noiseLevel", "Bruit Niveau", unit, 0.0f);
    addChoice ("noiseType",  "Bruit Type", StringArray { "BLANC", "ROSE", "SALE" }, 0);
    addFloat  ("noiseTone",  "Bruit Ton", unit, 1.0f);
    addFloat  ("noiseDecay", "Bruit Decay", NormalisableRange<float> (0.005f, 5.0f, 0.0f, 0.35f), 5.0f);
    addFloat  ("pitchEnvAmt",   "Pitch Env Quantite (demi-tons)", NormalisableRange<float> (-60.0f, 60.0f), 0.0f);
    addFloat  ("pitchEnvDecay", "Pitch Env Decay", NormalisableRange<float> (0.003f, 2.0f, 0.0f, 0.4f), 0.08f);
    addChoice ("phaseMode",  "Phase Mode", StringArray { "ALEATOIRE", "FIXE", "LIBRE" }, 0);
    addFloat  ("oscPhase",   "Phase Depart", unit, 0.0f);
    addChoice ("distType",   "Disto Type", StringArray { "SOFT", "HARD", "FOLD", "CRUSH", "ASYM" }, 0);
    addFloat  ("distAmt",    "Disto Quantite", unit, 0.0f);
    addFloat  ("distMix",    "Disto Mix", unit, 1.0f);

    // Matrice de modulation : 12 routes (source -> cible, quantité)
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
    addChoice ("quality",   "Qualite", StringArray { "ECO", "NORMAL", "HAUTE" }, 1);

    // Effets
    addFloat ("chorusMix",   "Chorus Mix", unit, 0.0f);
    addFloat ("chorusRate",  "Chorus Vitesse", NormalisableRange<float> (0.1f, 5.0f, 0.0f, 0.5f), 0.8f);
    addFloat ("chorusDepth", "Chorus Profondeur", unit, 0.4f);

    addFloat ("delayTime",     "Delay Temps (ms)", NormalisableRange<float> (20.0f, 1000.0f, 0.0f, 0.5f), 350.0f);
    addFloat ("delayFeedback", "Delay Feedback", NormalisableRange<float> (0.0f, 0.9f), 0.35f);
    addFloat ("delayMix",      "Delay Mix", unit, 0.0f);

    addFloat ("chorusDelay",    "Chorus Delai (ms)", NormalisableRange<float> (1.0f, 30.0f, 0.0f, 0.5f), 7.0f);
    addFloat ("chorusFeedback", "Chorus Feedback", NormalisableRange<float> (-0.95f, 0.95f), 0.0f);
    addChoice ("delaySync", "Delay Sync", StringArray { "LIBRE", "1/4", "1/8", "1/8 PT", "1/16", "1/16 T", "1/2" }, 0);
    addChoice ("delayMode", "Delay Mode", StringArray { "STEREO", "PING-PONG" }, 0);
    addFloat ("delayLP", "Delay Filtre Aigu", NormalisableRange<float> (200.0f, 20000.0f, 0.0f, 0.3f), 20000.0f);
    addFloat ("delayHP", "Delay Filtre Grave", NormalisableRange<float> (20.0f, 2000.0f, 0.0f, 0.4f), 20.0f);
    addFloat ("reverbPre", "Reverb Pre-delay (ms)", NormalisableRange<float> (0.0f, 200.0f), 0.0f);
    addFloat ("reverbLP", "Reverb Filtre Aigu", NormalisableRange<float> (500.0f, 20000.0f, 0.0f, 0.3f), 20000.0f);
    addFloat ("reverbHP", "Reverb Filtre Grave", NormalisableRange<float> (20.0f, 2000.0f, 0.0f, 0.4f), 20.0f);
    addFloat ("reverbWidth", "Reverb Largeur", unit, 1.0f);
    addFloat ("phaserMix",   "Phaser Mix", unit, 0.0f);
    addFloat ("phaserRate",  "Phaser Vitesse", NormalisableRange<float> (0.05f, 8.0f, 0.0f, 0.4f), 0.5f);
    addFloat ("phaserDepth", "Phaser Profondeur", unit, 0.6f);
    addFloat ("phaserFeedback", "Phaser Feedback", NormalisableRange<float> (-0.9f, 0.9f), 0.3f);
    addFloat ("phaserCentre", "Phaser Centre (Hz)", NormalisableRange<float> (100.0f, 6000.0f, 0.0f, 0.4f), 1200.0f);
    addFloat ("reverbSize", "Reverb Taille", unit, 0.5f);
    addFloat ("reverbDamp", "Reverb Amorti", unit, 0.5f);
    addFloat ("reverbMix",  "Reverb Mix", unit, 0.0f);

    // Séquenceur
    addBool   ("seqOn", "Sequenceur", false);
    addBool   ("seqRec", "Seq Enregistrement", false);
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

    for (int b = 0; b < 8; ++b)
    {
        const juce::String n = "eq" + juce::String (b + 1);
        eqParams[(size_t) (b * 5)]     = apvts.getRawParameterValue (n + "On");
        eqParams[(size_t) (b * 5 + 1)] = apvts.getRawParameterValue (n + "Type");
        eqParams[(size_t) (b * 5 + 2)] = apvts.getRawParameterValue (n + "Freq");
        eqParams[(size_t) (b * 5 + 3)] = apvts.getRawParameterValue (n + "Gain");
        eqParams[(size_t) (b * 5 + 4)] = apvts.getRawParameterValue (n + "Q");
    }
    eqOutParam = apvts.getRawParameterValue ("eqOut");

    for (int s = 0; s < 4; ++s)
    {
        const juce::String n = "fx" + juce::String (s + 1);
        const char* ids[7] = { "Type", "On", "Mix", "A", "B", "C", "D" };
        for (int k = 0; k < 7; ++k)
            fxParams[(size_t) (s * 7 + k)] = apvts.getRawParameterValue (n + ids[k]);
    }

    synth.addSound (new SynthSound());
    for (int i = 0; i < 16; ++i)              // 16 voix de polyphonie (les queues longues des nappes en consomment beaucoup)
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

    phaser.prepare (spec);
    phaser.setMix (0.5f);
    preDelay.setMaximumDelayInSamples ((int) (sampleRate * 0.25) + 8);
    preDelay.prepare (spec);
    preDelay.reset();
    reverbBuf.setSize (2, juce::jmax (samplesPerBlock, 8192));
    for (int i = 0; i < 2; ++i)
        dlLp[i] = dlHp[i] = rvLp[i] = rvHp[i] = 0.0f;

    for (auto& band : eqBands)
    {
        band.f[0].reset();
        band.f[1].reset();
        band.lastType = -1;
    }
    lastEqOut = 1.0f;

    if (! midiMapLoaded)
        loadMidiMap();

    fxRack.prepare (sampleRate, juce::jmax (samplesPerBlock, 8192));
    capSr = sampleRate;
    capBuf.assign ((size_t) (sampleRate * 3.0) + 1, 0.0f);
    capPos = 0;
    capState = 0;

    compressor.prepare (spec);
    limiter.prepare (spec);
    limiter.setRelease (60.0f);
    dryBuffer.setSize (2, juce::jmax (samplesPerBlock, 8192));

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
    // MIDI learn / correspondances CC -> réglage
    for (const auto meta : midi)
    {
        const auto m = meta.getMessage();
        if (! m.isController() || m.getControllerNumber() >= 120)
            continue;
        const int cc = m.getControllerNumber();
        if (auto* lp = learnParam.exchange (nullptr))
        {
            for (auto& slot : ccParam)                                  // un réglage n'a qu'un seul CC
                if (slot.load() == lp)
                    slot.store (nullptr);
            ccParam[(size_t) cc].store (lp);
            lastLearnedName = lp->getName (24);
            lastLearnedCc.store (cc);
            triggerAsyncUpdate();
        }
        else if (auto* p = ccParam[(size_t) cc].load())
            p->setValueNotifyingHost ((float) m.getControllerValue() / 127.0f);
    }

    keyboardState.processNextMidiBuffer (midi, 0, numSamples, true);
    generateSequencerMidi (midi, numSamples);
    for (const auto meta : midi)
        if (meta.getMessage().isNoteOn())
            lastNoteHz = (float) juce::MidiMessage::getMidiNoteInHertz (meta.getMessage().getNoteNumber());
    synth.renderNextBlock (buffer, midi, 0, numSamples);

    if (capState.load (std::memory_order_acquire) == 1)
    {
        int pos = capPos.load (std::memory_order_relaxed);
        const int cap = (int) capBuf.size();
        const float* l = buffer.getReadPointer (0);
        const float* r = buffer.getReadPointer (buffer.getNumChannels() > 1 ? 1 : 0);
        for (int i = 0; i < numSamples && pos < cap; ++i)
            capBuf[(size_t) pos++] = 0.5f * (l[i] + r[i]);
        capPos.store (pos, std::memory_order_release);
        if (pos >= cap)
            capState.store (2, std::memory_order_release);
    }

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

    // Chorus / flanger (delai court + feedback)
    const float chorusMix = param ("chorusMix");
    if (chorusMix > 0.001f && numCh == 2)
    {
        chorus.setRate (param ("chorusRate"));
        chorus.setDepth (param ("chorusDepth"));
        chorus.setCentreDelay (param ("chorusDelay"));
        chorus.setFeedback (param ("chorusFeedback"));
        chorus.setMix (chorusMix);

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        chorus.process (context);
    }

    // Phaser
    const float phaserMix = param ("phaserMix");
    if (phaserMix > 0.001f && numCh == 2)
    {
        phaser.setRate (param ("phaserRate"));
        phaser.setDepth (param ("phaserDepth"));
        phaser.setCentreFrequency (param ("phaserCentre"));
        phaser.setFeedback (param ("phaserFeedback"));
        phaser.setMix (phaserMix);

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        phaser.process (context);
    }

    // Delay : libre ou synchronise, stereo ou ping-pong, filtres dans le feedback
    const float delayMix = param ("delayMix");
    if (delayMix > 0.001f)
    {
        const float feedback = param ("delayFeedback");
        const int syncIdx = (int) param ("delaySync");
        const bool ping = (int) param ("delayMode") == 1 && numCh == 2;
        static const float beats[] = { 0.0f, 1.0f, 0.5f, 0.75f, 0.25f, 1.0f / 6.0f, 2.0f };

        float timeL = (float) (param ("delayTime") * 0.001 * sr);
        if (syncIdx > 0)
            timeL = (float) (beats[juce::jlimit (0, 6, syncIdx)] * 60.0 / juce::jmax (20.0, getHostBpm()) * sr);
        timeL = juce::jlimit (8.0f, (float) (sr * 1.28), timeL);
        const float timeR = ping || syncIdx > 0 ? timeL : juce::jmin ((float) (sr * 1.28), timeL * 1.08f);

        const float lpA = 1.0f - std::exp (-juce::MathConstants<float>::twoPi * param ("delayLP") / (float) sr);
        const float hpA = 1.0f - std::exp (-juce::MathConstants<float>::twoPi * param ("delayHP") / (float) sr);

        auto fbFilter = [&] (int ch, float v)
        {
            dlLp[ch] += lpA * (v - dlLp[ch]);
            dlHp[ch] += hpA * (dlLp[ch] - dlHp[ch]);
            return dlLp[ch] - dlHp[ch];
        };

        if (numCh == 2)
        {
            auto* xl = buffer.getWritePointer (0);
            auto* xr = buffer.getWritePointer (1);
            for (int i = 0; i < numSamples; ++i)
            {
                const float inL = xl[i], inR = xr[i];
                const float dL = delayLine.popSample (0, timeL, true);
                const float dR = delayLine.popSample (1, timeR, true);
                const float fL = fbFilter (0, dL) * feedback;
                const float fR = fbFilter (1, dR) * feedback;

                if (ping)
                {
                    delayLine.pushSample (0, (inL + inR) * 0.5f + fR);
                    delayLine.pushSample (1, fL);
                }
                else
                {
                    delayLine.pushSample (0, inL + fL);
                    delayLine.pushSample (1, inR + fR);
                }

                xl[i] = inL + dL * delayMix;
                xr[i] = inR + dR * delayMix;
            }
        }
        else
        {
            auto* x = buffer.getWritePointer (0);
            for (int i = 0; i < numSamples; ++i)
            {
                const float in = x[i];
                const float d = delayLine.popSample (0, timeL, true);
                delayLine.pushSample (0, in + fbFilter (0, d) * feedback);
                x[i] = in + d * delayMix;
            }
        }
    }

    // Reverb : pre-delay, filtres sur le signal reverbere, largeur
    const float reverbMix = param ("reverbMix");
    if (reverbMix > 0.001f && numCh == 2 && numSamples <= reverbBuf.getNumSamples())
    {
        juce::Reverb::Parameters rp;
        rp.roomSize   = param ("reverbSize");
        rp.damping    = param ("reverbDamp");
        rp.wetLevel   = 0.33f;
        rp.dryLevel   = 0.0f;
        rp.width      = 1.0f;
        rp.freezeMode = 0.0f;
        reverb.setParameters (rp);

        const float preSamples = juce::jlimit (0.0f, (float) (sr * 0.24), (float) (param ("reverbPre") * 0.001 * sr));
        const float rLpA = 1.0f - std::exp (-juce::MathConstants<float>::twoPi * param ("reverbLP") / (float) sr);
        const float rHpA = 1.0f - std::exp (-juce::MathConstants<float>::twoPi * param ("reverbHP") / (float) sr);
        const float width = param ("reverbWidth");

        for (int ch = 0; ch < 2; ++ch)
        {
            const float* src = buffer.getReadPointer (ch);
            float* dst = reverbBuf.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                preDelay.pushSample (ch, src[i]);
                dst[i] = preDelay.popSample (ch, preSamples, true);
            }
        }

        reverb.processStereo (reverbBuf.getWritePointer (0), reverbBuf.getWritePointer (1), numSamples);

        for (int ch = 0; ch < 2; ++ch)
        {
            float* wet = reverbBuf.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                rvLp[ch] += rLpA * (wet[i] - rvLp[ch]);
                rvHp[ch] += rHpA * (rvLp[ch] - rvHp[ch]);
                wet[i] = rvLp[ch] - rvHp[ch];
            }
        }

        auto* wl = reverbBuf.getWritePointer (0);
        auto* wr = reverbBuf.getWritePointer (1);
        auto* xl = buffer.getWritePointer (0);
        auto* xr = buffer.getWritePointer (1);
        const float dryGain = 1.0f - 0.4f * reverbMix;
        for (int i = 0; i < numSamples; ++i)
        {
            const float mid = 0.5f * (wl[i] + wr[i]), side = 0.5f * (wl[i] - wr[i]) * width;
            xl[i] = xl[i] * dryGain + (mid + side) * reverbMix;
            xr[i] = xr[i] * dryGain + (mid - side) * reverbMix;
        }
    }

    // EQ 8 bandes (chaque bande active filtre les deux canaux)
    {
        const double eqRate = sr > 0.0 ? sr : 44100.0;
        const float eqOutGain = juce::Decibels::decibelsToGain (eqOutParam->load());
        bool any = false;

        for (size_t b = 0; b < 8; ++b)
        {
            if (eqParams[b * 5]->load() < 0.5f)
                continue;

            any = true;
            auto& band = eqBands[b];
            const int   type = (int) eqParams[b * 5 + 1]->load();
            const float freq = eqParams[b * 5 + 2]->load();
            const float gain = eqParams[b * 5 + 3]->load();
            const float q    = eqParams[b * 5 + 4]->load();

            if (type != band.lastType || freq != band.lastFreq || gain != band.lastGain || q != band.lastQ)
            {
                const auto coeffs = makeEqCoefficients (type, eqRate, freq, gain, q);
                band.f[0].coefficients = coeffs;
                band.f[1].coefficients = coeffs;
                band.lastType = type;  band.lastFreq = freq;  band.lastGain = gain;  band.lastQ = q;
            }

            for (int ch = 0; ch < numCh; ++ch)
            {
                auto* x = buffer.getWritePointer (ch);
                for (int i = 0; i < numSamples; ++i)
                    x[i] = band.f[(size_t) ch].processSample (x[i]);
            }
        }

        if (any || std::abs (eqOutGain - 1.0f) > 0.001f || std::abs (lastEqOut - 1.0f) > 0.001f)
            buffer.applyGainRamp (0, numSamples, lastEqOut, eqOutGain);
        lastEqOut = eqOutGain;
    }

    // RACK D'EFFETS (4 emplacements en série)
    if (numCh == 2)
    {
        std::array<FxSlotParams, FxRack::numSlots> sp;
        bool anyFx = false;
        for (int s = 0; s < FxRack::numSlots; ++s)
        {
            auto& q = sp[(size_t) s];
            q.type = (int) fxParams[(size_t) (s * 7)]->load();
            q.on   = fxParams[(size_t) (s * 7 + 1)]->load() > 0.5f;
            q.mix  = fxParams[(size_t) (s * 7 + 2)]->load();
            q.a = fxParams[(size_t) (s * 7 + 3)]->load();
            q.b = fxParams[(size_t) (s * 7 + 4)]->load();
            q.c = fxParams[(size_t) (s * 7 + 5)]->load();
            q.d = fxParams[(size_t) (s * 7 + 6)]->load();
            anyFx = anyFx || (q.type != 0 && q.on);
        }
        if (anyFx)
            fxRack.process (buffer, sp, hostBpm.load(), hostPlaying.load() ? hostPpq.load() : -1.0, lastNoteHz.load());
    }

    // MASTER : compresseur (mix parallèle), largeur stéréo
    const float compThresh = param ("compThresh");
    if (compThresh < -0.1f && numCh == 2)
    {
        const float mix = param ("compMix");
        if (dryBuffer.getNumSamples() < numSamples)
            dryBuffer.setSize (2, numSamples, false, false, true);
        for (int ch = 0; ch < 2; ++ch)
            dryBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

        compressor.setThreshold (compThresh);
        compressor.setRatio (param ("compRatio"));
        compressor.setAttack (param ("compAttack"));
        compressor.setRelease (param ("compRelease"));

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        compressor.process (context);

        const float makeup = juce::Decibels::decibelsToGain (param ("compMakeup"));
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* wet = buffer.getWritePointer (ch);
            const auto* dry = dryBuffer.getReadPointer (ch);
            for (int i = 0; i < numSamples; ++i)
                wet[i] = dry[i] * (1.0f - mix) + wet[i] * makeup * mix;
        }
    }

    const float width = param ("width");
    if (numCh == 2 && std::abs (width - 1.0f) > 0.01f)
    {
        auto* l = buffer.getWritePointer (0);
        auto* r = buffer.getWritePointer (1);
        for (int i = 0; i < numSamples; ++i)
        {
            const float mid = 0.5f * (l[i] + r[i]);
            const float side = 0.5f * (l[i] - r[i]) * width;
            l[i] = mid + side;
            r[i] = mid - side;
        }
    }

    // Volume général (avec rampe pour éviter les clics)
    const float master = param ("master");
    buffer.applyGainRamp (0, numSamples, lastMaster, master);
    lastMaster = master;

    // Fondu pendant un changement de preset (évite le coup de volume pendant que les réglages se mettent en place)
    {
        const float target = presetLoading.load() ? 0.0f : 1.0f;
        if (loadGain != 1.0f || target != 1.0f)
            buffer.applyGainRamp (0, numSamples, loadGain, target);
        loadGain = target;
    }

    // Limiteur de sortie : protège contre les saturations, surtout avec les presets plus forts
    if (numCh == 2)
    {
        limiter.setThreshold (param ("limCeil"));
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        limiter.process (context);
    }
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

    // REC : les notes jouees (clavier a l'ecran ou MIDI) sont quantifiees sur le pas le plus proche
    if (param ("seqRec") > 0.5f)
    {
        bool changed = false;
        for (const auto meta : midi)
        {
            const auto msg = meta.getMessage();
            if (! msg.isNoteOn())
                continue;

            const double ppq = ppqStart + (double) meta.samplePosition / samplesPerBeat;
            const juce::int64 k = (juce::int64) std::llround (ppq / stepBeats);
            const int recStep = (int) (((k % totalSteps) + totalSteps) % totalSteps);
            sequencer.on[(size_t) recStep] = true;
            sequencer.note[(size_t) recStep] = msg.getNoteNumber();
            changed = true;
        }
        if (changed)
            ++sequencer.revision;
    }

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

void MonSyntheProcessor::startResample()
{
    if (capBuf.empty())
        return;
    capState.store (0);
    capPos.store (0);
    capState.store (1, std::memory_order_release);
}

bool MonSyntheProcessor::finishResample (int slot, juce::String& error)
{
    capState.store (0);
    const int n = capPos.load();
    const double sr = capSr;
    const auto& x = capBuf;

    float peak = 0.0f;
    for (int i = 0; i < n; ++i)
        peak = juce::jmax (peak, std::abs (x[(size_t) i]));
    if (peak < 0.002f)
    {
        error = "RESAMPLE : AUCUN SON CAPTE";
        return false;
    }

    // zone active : de la première à la dernière fenêtre de 20 ms au-dessus de 3 % du pic
    const int blk = juce::jmax (16, (int) (sr * 0.02));
    int s = -1, e = -1;
    for (int i = 0; i + blk <= n; i += blk)
    {
        float m = 0.0f;
        for (int k = 0; k < blk; ++k)
            m = juce::jmax (m, std::abs (x[(size_t) (i + k)]));
        if (m > 0.03f * peak)
        {
            if (s < 0) s = i;
            e = i + blk;
        }
    }
    if (s < 0 || e - s < (int) (sr * 0.15))
    {
        error = "RESAMPLE : SON TROP COURT (tiens la note)";
        return false;
    }

    // période de départ : hauteur de la dernière note jouée, sinon recherche libre par autocorrélation
    auto corr = [&] (int start, double P)
    {
        const int len = (int) P;
        double ab = 0.0, aa = 1.0e-9, bb = 1.0e-9;
        const int shift = (int) std::round (P);
        for (int i = 0; i < len; ++i)
        {
            const double a = x[(size_t) (start + i)], b = x[(size_t) (start + i + shift)];
            ab += a * b; aa += a * a; bb += b * b;
        }
        return ab / std::sqrt (aa * bb);
    };

    const int probe = s + (e - s) / 10;
    double P0 = 0.0;
    const float noteHz = lastNoteHz.load();
    const double pMin = sr / 2000.0, pMax = juce::jmin (sr / 30.0, (double) (e - s) / 3.0);
    {
        double lo = pMin, hi = pMax;
        if (noteHz > 20.0f)
        {
            lo = sr / noteHz * 0.94;
            hi = sr / noteHz * 1.06;
        }
        double best = -2.0;
        for (double P = lo; P <= hi; P += (noteHz > 20.0f ? 0.1 : 0.5))
        {
            if (probe + (int) (2.0 * P) + 2 >= n)
                break;
            const double c = corr (probe, P);
            if (c > best) { best = c; P0 = P; }
        }
        if (P0 <= 0.0 || best < 0.3)
        {
            error = "RESAMPLE : HAUTEUR NON DETECTEE (son trop bruite)";
            return false;
        }
    }

    constexpr int N = WavetableBank::tableSize;
    const int F = 32;
    std::vector<float> frames ((size_t) F * N);
    float maxAbs = 1.0e-6f;
    double P = P0;
    const int last = e - (int) (2.2 * P0);
    if (last <= s)
    {
        error = "RESAMPLE : SON TROP COURT";
        return false;
    }

    for (int f = 0; f < F; ++f)
    {
        const int c0 = s + (int) ((double) (last - s) * f / (F - 1));
        // affine la période autour de la précédente (suit une hauteur qui dérive)
        double bestC = -2.0, bestP = P;
        for (double q = P * 0.97; q <= P * 1.03; q += P * 0.002)
            if (c0 + (int) (2.0 * q) + 2 < n)
            {
                const double c = corr (c0, q);
                if (c > bestC) { bestC = c; bestP = q; }
            }
        if (bestC > 0.5)
            P = bestP;

        // alignement de phase : premier passage montant par zéro
        int st = c0;
        for (int i = c0; i < c0 + (int) P && i + 1 < n; ++i)
            if (x[(size_t) i] <= 0.0f && x[(size_t) (i + 1)] > 0.0f) { st = i; break; }

        float* out = frames.data() + (size_t) f * N;
        double mean = 0.0;
        for (int i = 0; i < N; ++i)
        {
            const double pos = (double) st + (double) i * P / N;
            const int i0 = juce::jlimit (0, n - 2, (int) pos);
            const float fr = (float) (pos - i0);
            out[i] = x[(size_t) i0] + (x[(size_t) (i0 + 1)] - x[(size_t) i0]) * fr;
            mean += out[i];
        }
        mean /= N;
        for (int i = 0; i < N; ++i)
        {
            out[i] -= (float) mean;
            maxAbs = juce::jmax (maxAbs, std::abs (out[i]));
        }
    }

    for (auto& v : frames)
        v *= 0.9f / maxAbs;

    // on écrit un .wav (32 formes d'onde de 2048) : il se recharge avec le preset et se réutilise ailleurs
    auto dir = getPresetDirectory().getParentDirectory().getChildFile ("Resampled");
    dir.createDirectory();
    const auto file = dir.getNonexistentChildFile ("resample", ".wav");

    {
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::FileOutputStream> stream (file.createOutputStream());
        if (stream == nullptr)
        {
            error = "RESAMPLE : ECRITURE IMPOSSIBLE";
            return false;
        }
        std::unique_ptr<juce::AudioFormatWriter> writer (wav.createWriterFor (stream.get(), 44100.0, 1, 24, {}, 0));
        if (writer == nullptr)
        {
            error = "RESAMPLE : ECRITURE IMPOSSIBLE";
            return false;
        }
        stream.release();
        juce::AudioBuffer<float> out (1, F * N);
        out.copyFrom (0, 0, frames.data(), F * N);
        writer->writeFromAudioSampleBuffer (out, 0, F * N);
    }

    return loadUserWavetable (slot, file, error);
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
    return { "OFF", "BEND+", "BEND-", "SYNC", "MIRROR", "ASYM", "FLIP", "QUANT", "PWM", "WOBBLE" };
}

juce::StringArray MonSyntheProcessor::lfoSyncNames()
{
    return { "LIBRE", "4 MES", "2 MES", "1 MES", "1/2", "1/4", "1/8", "1/16", "1/32" };
}

juce::StringArray MonSyntheProcessor::modSourceNames()
{
    return { "---", "LFO", "ENV MOD", "ENV FILTRE", "VELOCITE", "NOTE", "MOLETTE", "ALEA",
             "LFO 2", "MACRO 1", "MACRO 2", "MACRO 3", "MACRO 4", "ENV PITCH", "LFO 3", "LFO 4", "DESSIN" };
}

juce::StringArray MonSyntheProcessor::modTargetNames()
{
    // Les noms sont regroupes par module (les numeros internes ne changent pas : les presets restent valides)
    return { "---", "OSC A  POSITION", "OSC B  POSITION", "FILTRE  CUTOFF", "FILTRE  RESO", "PITCH  HAUTEUR",
             "UNISON  DESACCORD", "OSC B  NIVEAU", "FILTRE  DRIVE", "AMP  VOLUME",
             "ENV  QTE ENV FILTRE", "ENV  QTE ENV MOD", "ENV  DECAY VOLUME", "ENV  DECAY FILTRE",
             "OSC A  WARP", "OSC B  WARP", "FM/RING  FM", "FM/RING  RING",
             "DISTO  QUANTITE", "BRUIT  NIVEAU", "PITCH  QTE ENV PITCH",
             "FILTRE 2  CUTOFF", "FILTRE 2  VOYELLE" };
}

//==============================================================================
// Presets d'usine : la banque est dans FactoryBank.cpp
//==============================================================================
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

    // 0) fondu de sortie le temps de changer les réglages (le son qui joue ne subit pas les valeurs intermédiaires)
    presetLoading = true;
    juce::Thread::sleep (30);

    // 1) tout remettre aux valeurs par défaut (sauf le séquenceur, le volume général et la qualité)
    for (auto* prm : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (prm))
            if (! rp->paramID.startsWith ("seq") && rp->paramID != "master" && rp->paramID != "quality")
                rp->setValueNotifyingHost (rp->getDefaultValue());

    // 2) appliquer les réglages du preset
    for (const auto& kv : presets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (kv.first))
            rp->setValueNotifyingHost (rp->convertTo0to1 (kv.second));

    presetLoading = false;
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

//==============================================================================
// MIDI learn
//==============================================================================
juce::File MonSyntheProcessor::midiMapFile()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
               .getChildFile ("ANKS Labs").getChildFile ("MS1").getChildFile ("midimap.txt");
}

void MonSyntheProcessor::startMidiLearn (const juce::String& paramId)
{
    learnParam.store (dynamic_cast<juce::RangedAudioParameter*> (apvts.getParameter (paramId)));
}

int MonSyntheProcessor::getCcForParam (const juce::String& paramId) const
{
    auto* p = apvts.getParameter (paramId);
    for (int cc = 0; cc < 128; ++cc)
        if (p != nullptr && ccParam[(size_t) cc].load() == p)
            return cc;
    return -1;
}

void MonSyntheProcessor::forgetCcForParam (const juce::String& paramId)
{
    auto* p = apvts.getParameter (paramId);
    for (auto& slot : ccParam)
        if (p != nullptr && slot.load() == p)
            slot.store (nullptr);
    triggerAsyncUpdate();
}

void MonSyntheProcessor::saveMidiMap()
{
    juce::String text;
    for (int cc = 0; cc < 128; ++cc)
        if (auto* p = ccParam[(size_t) cc].load())
            text << cc << "=" << p->paramID << "\n";
    const auto f = midiMapFile();
    f.getParentDirectory().createDirectory();
    f.replaceWithText (text);
}

void MonSyntheProcessor::loadMidiMap()
{
    midiMapLoaded = true;
    const auto f = midiMapFile();
    if (! f.existsAsFile())
        return;
    for (const auto& line : juce::StringArray::fromLines (f.loadFileAsString()))
    {
        const int eq = line.indexOfChar ('=');
        if (eq < 1)
            continue;
        const int cc = line.substring (0, eq).getIntValue();
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*> (apvts.getParameter (line.substring (eq + 1).trim())))
            if (cc >= 0 && cc < 128)
                ccParam[(size_t) cc].store (p);
    }
}
