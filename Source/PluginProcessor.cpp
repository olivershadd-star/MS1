#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Wavetables
//==============================================================================
float WavetableBank::harmonicAmp (int frame, int h)
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

WavetableBank::WavetableBank()
{
    data.assign ((size_t) numFrames * numLevels * tableSize, 0.0f);

    std::vector<float> sine (tableSize);
    for (int i = 0; i < tableSize; ++i)
        sine[(size_t) i] = std::sin (juce::MathConstants<float>::twoPi * (float) i / (float) tableSize);

    for (int frame = 0; frame < numFrames; ++frame)
    {
        float scale = 1.0f;

        for (int level = 0; level < numLevels; ++level)
        {
            float* t = const_cast<float*> (table (frame, level));
            const int maxHarmonics = 512 >> level;     // moins d'harmoniques pour les notes aiguës

            for (int h = 1; h <= maxHarmonics; ++h)
            {
                const float a = harmonicAmp (frame, h);
                if (a == 0.0f)
                    continue;

                for (int i = 0; i < tableSize; ++i)
                    t[i] += a * sine[(size_t) ((h * i) & (tableSize - 1))];
            }

            if (level == 0)    // on normalise toutes les versions avec le même facteur
            {
                float peak = 0.0f;
                for (int i = 0; i < tableSize; ++i)
                    peak = juce::jmax (peak, std::abs (t[i]));
                scale = peak > 0.0f ? 1.0f / peak : 1.0f;
            }

            for (int i = 0; i < tableSize; ++i)
                t[i] *= scale;
        }
    }
}

int WavetableBank::levelForFrequency (double hz) noexcept
{
    return juce::jlimit (0, numLevels - 1, (int) std::floor (std::log2 (juce::jmax (1.0, hz) / 20.0)));
}

float WavetableBank::getSample (float pos, int level, double phase) const noexcept
{
    const float fpos = juce::jlimit (0.0f, 1.0f, pos) * (float) (numFrames - 1);
    const int f0 = (int) fpos;
    const int f1 = juce::jmin (f0 + 1, numFrames - 1);
    const float fm = fpos - (float) f0;

    const float x = (float) (phase * tableSize);
    const int i0 = (int) x & (tableSize - 1);
    const int i1 = (i0 + 1) & (tableSize - 1);
    const float fr = x - std::floor (x);

    const float* a = table (f0, level);
    const float* b = table (f1, level);

    const float sa = a[i0] + (a[i1] - a[i0]) * fr;
    const float sb = b[i0] + (b[i1] - b[i0]) * fr;
    return sa + (sb - sa) * fm;
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

void SynthVoice::startNote (int midiNote, float velocity, juce::SynthesiserSound*, int pitchWheelPosition)
{
    currentNote = targetNote = (double) midiNote;
    bendNorm = (float) (pitchWheelPosition - 8192) / 8192.0f;
    gain = velocity;
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

    position.setCurrentAndTargetValue (param ("position"));
    filter.reset();
    filter2.reset();

    adsr.noteOn();
    modEnv.noteOn();
    fltEnv.noteOn();
}

void SynthVoice::legatoTo (int midiNote, float velocity)
{
    targetNote = (double) midiNote;      // la note glisse vers la nouvelle (selon le réglage Glide)
    gain = velocity;
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

    // --- Enveloppes
    adsr.setParameters   ({ param ("attack"),    param ("decay"),    param ("sustain"),    param ("release") });
    modEnv.setParameters ({ param ("modAttack"), param ("modDecay"), param ("modSustain"), param ("modRelease") });
    fltEnv.setParameters ({ param ("fltAttack"), param ("fltDecay"), param ("fltSustain"), param ("fltRelease") });

    // --- Filtre
    using FilterType = juce::dsp::StateVariableTPTFilterType;
    const int   filterType = (int) param ("filterType");      // 0 LP12, 1 LP24, 2 HP, 3 BP
    const float baseCutoff = param ("cutoff");
    const float fltAmount  = param ("fltEnvAmount");
    const float drive      = param ("drive");
    const bool  twoPole    = (filterType == 1);

    filter.setType (filterType == 2 ? FilterType::highpass
                                    : (filterType == 3 ? FilterType::bandpass : FilterType::lowpass));
    filter.setResonance (param ("resonance"));
    filter.setCutoffFrequency (baseCutoff);
    filter2.setCutoffFrequency (baseCutoff);

    // --- Modulation de la position de wavetable (LFO + enveloppe)
    position.setTargetValue (param ("position"));
    const float  modAmount = param ("modAmount");
    const float  lfoDepth  = param ("lfoDepth");
    const int    lfoShape  = (int) param ("lfoShape");
    const double lfoInc    = (double) param ("lfoRate") / sr;

    // --- Unison : N oscillateurs désaccordés et répartis dans le champ stéréo
    const int   uni    = juce::jlimit (1, maxUnison, (int) std::round (param ("unison")));
    const float detune = param ("detune");
    const float spread = param ("spread");
    const float norm   = 1.0f / std::sqrt ((float) uni);

    std::array<double, maxUnison> ratio {};
    std::array<float, maxUnison> gainL {}, gainR {};

    for (int u = 0; u < uni; ++u)
    {
        const float offset = uni > 1 ? -1.0f + 2.0f * (float) u / (float) (uni - 1) : 0.0f;
        ratio[(size_t) u] = std::pow (2.0, (double) (offset * detune) / 1200.0);

        const float angle = (offset * spread + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        gainL[(size_t) u] = std::cos (angle) * juce::MathConstants<float>::sqrt2 * norm;
        gainR[(size_t) u] = std::sin (angle) * juce::MathConstants<float>::sqrt2 * norm;
    }
    const double maxRatio = std::pow (2.0, (double) detune / 1200.0);

    // --- Oscillateur B et sub
    const float  levelB = param ("oscBLevel");
    const float  posB   = param ("oscBPos");
    const double factorB = std::pow (2.0, std::round ((double) param ("oscBOct")) + (double) param ("oscBTune") / 1200.0);
    const float  subAmt = param ("subLevel");

    // --- Hauteur : glide et pitch bend
    const double glideSec  = param ("glide");
    const float  bendRange = param ("bendRange");

    std::array<double, maxUnison> incA {}, incB {};
    double incSub = 0.0;
    int tableA = 0, tableB = 0;

    auto updatePitch = [&] (int elapsed)
    {
        const double a = glideSec > 0.0005 ? 1.0 - std::exp (-(double) elapsed / (glideSec * 0.25 * sr)) : 1.0;
        currentNote += (targetNote - currentNote) * a;

        const double note  = currentNote + (double) (bendNorm * bendRange);
        const double freq  = 440.0 * std::pow (2.0, (note - 69.0) / 12.0);
        const double freqB = freq * factorB;

        for (int u = 0; u < uni; ++u)
        {
            incA[(size_t) u] = freq / sr * ratio[(size_t) u];
            incB[(size_t) u] = freqB / sr * ratio[(size_t) u];
        }
        incSub = freq / sr * 0.5;                              // sub : une octave en dessous

        tableA = WavetableBank::levelForFrequency (freq * maxRatio);
        tableB = WavetableBank::levelForFrequency (freqB * maxRatio);
    };

    for (int i = 0; i < numSamples; ++i)
    {
        if ((i & 31) == 0)
            updatePitch (32);

        // Position dans la wavetable = réglage de base + LFO + enveloppe de modulation
        const float lfo = lfoValue (lfoShape, lfoPhase);
        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0)
            lfoPhase -= 1.0;

        const float posA = position.getNextValue()
                           + lfoDepth * lfo
                           + modAmount * modEnv.getNextSample();

        // Oscillateurs A et B (avec unison)
        float l = 0.0f, r = 0.0f;
        for (int u = 0; u < uni; ++u)
        {
            const auto k = (size_t) u;
            float s = bank.getSample (posA, tableA, phasesA[k]);
            if (levelB > 0.0f)
                s += levelB * bank.getSample (posB, tableB, phasesB[k]);

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
        if (drive > 0.001f)
        {
            const float d = 1.0f + drive * 10.0f;
            const float inv = 1.0f / std::tanh (d);
            l += drive * (std::tanh (l * d) * inv - l);
            r += drive * (std::tanh (r * d) * inv - r);
        }

        // Filtre avec son enveloppe (jusqu'à +/- 5 octaves)
        const float fEnv = fltEnv.getNextSample();
        if (fltAmount != 0.0f)
        {
            const float c = juce::jlimit (20.0f, 20000.0f, baseCutoff * std::exp2 (fltAmount * fEnv * 5.0f));
            filter.setCutoffFrequency (c);
            if (twoPole)
                filter2.setCutoffFrequency (c);
        }

        l = filter.processSample (0, l);
        r = filter.processSample (1, r);
        if (twoPole)
        {
            l = filter2.processSample (0, l);
            r = filter2.processSample (1, r);
        }

        // Volume
        const float env = adsr.getNextSample() * gain * 0.25f;
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
    addFloat ("position", "Wavetable", unit, 0.3f);

    // Oscillateur B + sub
    addFloat ("oscBPos",   "B Wavetable", unit, 0.5f);
    addInt   ("oscBOct",   "B Octave", -2, 2, 0);
    addFloat ("oscBTune",  "B Accord (cents)", NormalisableRange<float> (-100.0f, 100.0f), 0.0f);
    addFloat ("oscBLevel", "B Niveau", unit, 0.0f);
    addFloat ("subLevel",  "Sub Niveau", unit, 0.0f);

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

    // Enveloppe de modulation -> position de la wavetable
    addFloat ("modAttack",  "Mod Attack", time, 0.01f);
    addFloat ("modDecay",   "Mod Decay", time, 0.8f);
    addFloat ("modSustain", "Mod Sustain", unit, 0.0f);
    addFloat ("modRelease", "Mod Release", time, 0.3f);
    addFloat ("modAmount",  "Mod Quantite", NormalisableRange<float> (-1.0f, 1.0f), 0.5f);

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
    synth.addSound (new SynthSound());
    for (int i = 0; i < 8; ++i)               // 8 voix de polyphonie
        synth.addVoice (new SynthVoice (apvts, bank));
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

    synth.setMono (param ("mono") > 0.5f);
    generateSequencerMidi (midi, numSamples);
    synth.renderNextBlock (buffer, midi, 0, numSamples);

    applyEffects (buffer);
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

    apvts.replaceState (copy);
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

juce::AudioProcessorEditor* MonSyntheProcessor::createEditor()
{
    return new MonSyntheEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MonSyntheProcessor();
}
