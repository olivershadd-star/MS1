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
    return juce::jlimit (0, numLevels - 1, (int) std::floor (std::log2 (hz / 20.0)));
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
    position.reset (sr, 0.02);   // lissage de 20 ms pour éviter les clics

    juce::dsp::ProcessSpec spec { sr, (juce::uint32) blockSize, 2 };   // filtre stéréo
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
}

void SynthVoice::startNote (int midiNote, float velocity, juce::SynthesiserSound*, int)
{
    const double freq = juce::MidiMessage::getMidiNoteInHertz (midiNote);
    phaseInc = freq / sr;
    level = WavetableBank::levelForFrequency (freq);
    phases[0] = 0.0;
    for (size_t i = 1; i < phases.size(); ++i)
        phases[i] = rng.nextDouble();      // phases aléatoires = son plus large
    lfoPhase = 0.0;
    gain = velocity;

    position.setCurrentAndTargetValue (*apvts.getRawParameterValue ("position"));
    filter.reset();
    adsr.noteOn();
    modEnv.noteOn();
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
        modEnv.noteOff();
    }
    else
    {
        adsr.reset();
        modEnv.reset();
        clearCurrentNote();
    }
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    adsr.setParameters ({ *apvts.getRawParameterValue ("attack"),
                          *apvts.getRawParameterValue ("decay"),
                          *apvts.getRawParameterValue ("sustain"),
                          *apvts.getRawParameterValue ("release") });

    filter.setCutoffFrequency (*apvts.getRawParameterValue ("cutoff"));
    filter.setResonance (*apvts.getRawParameterValue ("resonance"));
    position.setTargetValue (*apvts.getRawParameterValue ("position"));

    modEnv.setParameters ({ *apvts.getRawParameterValue ("modAttack"),
                            *apvts.getRawParameterValue ("modDecay"),
                            *apvts.getRawParameterValue ("modSustain"),
                            *apvts.getRawParameterValue ("modRelease") });

    const float modAmount = *apvts.getRawParameterValue ("modAmount");
    const float lfoDepth  = *apvts.getRawParameterValue ("lfoDepth");
    const int   lfoShape  = (int) *apvts.getRawParameterValue ("lfoShape");
    const double lfoInc   = (double) *apvts.getRawParameterValue ("lfoRate") / sr;

    // Unison : N oscillateurs désaccordés et répartis dans le champ stéréo
    const int   uni      = juce::jlimit (1, maxUnison, (int) std::round (*apvts.getRawParameterValue ("unison")));
    const float detune   = *apvts.getRawParameterValue ("detune");
    const float spread   = *apvts.getRawParameterValue ("spread");
    const float norm     = 1.0f / std::sqrt ((float) uni);

    std::array<double, maxUnison> inc {};
    std::array<float, maxUnison> gainL {}, gainR {};

    for (int u = 0; u < uni; ++u)
    {
        const float offset = uni > 1 ? -1.0f + 2.0f * (float) u / (float) (uni - 1) : 0.0f;
        inc[(size_t) u] = phaseInc * std::pow (2.0, (double) (offset * detune) / 1200.0);

        const float angle = (offset * spread + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        gainL[(size_t) u] = std::cos (angle) * juce::MathConstants<float>::sqrt2 * norm;
        gainR[(size_t) u] = std::sin (angle) * juce::MathConstants<float>::sqrt2 * norm;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        // Position dans la wavetable = réglage de base + LFO + enveloppe de modulation
        const float lfo = lfoValue (lfoShape, lfoPhase);
        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0)
            lfoPhase -= 1.0;

        const float pos = position.getNextValue()
                          + lfoDepth * lfo
                          + modAmount * modEnv.getNextSample();

        float l = 0.0f, r = 0.0f;
        for (int u = 0; u < uni; ++u)
        {
            const auto idx = (size_t) u;
            const float s = bank.getSample (pos, level, phases[idx]);
            l += s * gainL[idx];
            r += s * gainR[idx];

            phases[idx] += inc[idx];
            if (phases[idx] >= 1.0)
                phases[idx] -= 1.0;
        }

        l = filter.processSample (0, l);
        r = filter.processSample (1, r);

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
            clearCurrentNote();
            break;
        }
    }
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
// Plugin
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MonSyntheProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "position", 1 }, "Wavetable",
                                                        NormalisableRange<float> (0.0f, 1.0f), 0.3f));

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "attack", 1 }, "Attack",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.01f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "decay", 1 }, "Decay",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.2f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "sustain", 1 }, "Sustain",
                                                        NormalisableRange<float> (0.0f, 1.0f), 0.7f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "release", 1 }, "Release",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.3f));

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "cutoff", 1 }, "Cutoff",
                                                        NormalisableRange<float> (20.0f, 20000.0f, 0.0f, 0.3f), 2000.0f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "resonance", 1 }, "Resonance",
                                                        NormalisableRange<float> (0.5f, 10.0f, 0.0f, 0.5f), 0.7f));

    // Séquenceur
    p.push_back (std::make_unique<AudioParameterBool> (ParameterID { "seqOn", 1 }, "Sequenceur", false));
    p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { "seqBars", 1 }, "Seq Longueur",
                                                         StringArray { "1 mesure", "2 mesures", "3 mesures", "4 mesures" }, 0));
    p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { "seqRate", 1 }, "Seq Vitesse",
                                                         StringArray { "1/8", "1/16", "1/32" }, 1));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "seqGate", 1 }, "Seq Gate",
                                                        NormalisableRange<float> (0.05f, 1.0f), 0.5f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "seqTempo", 1 }, "Seq Tempo (libre)",
                                                        NormalisableRange<float> (40.0f, 240.0f, 1.0f), 120.0f));

    // Unison
    p.push_back (std::make_unique<AudioParameterInt> (ParameterID { "unison", 1 }, "Unison", 1, 7, 3));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "detune", 1 }, "Desaccord (cents)",
                                                        NormalisableRange<float> (0.0f, 100.0f), 15.0f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "spread", 1 }, "Stereo",
                                                        NormalisableRange<float> (0.0f, 1.0f), 0.6f));

    // LFO -> position de la wavetable
    p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { "lfoShape", 1 }, "LFO Forme",
                                                         StringArray { "Sinus", "Triangle", "Saw", "Carre" }, 0));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "lfoRate", 1 }, "LFO Vitesse (Hz)",
                                                        NormalisableRange<float> (0.05f, 20.0f, 0.0f, 0.4f), 1.0f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "lfoDepth", 1 }, "LFO Intensite",
                                                        NormalisableRange<float> (0.0f, 1.0f), 0.2f));

    // Enveloppe de modulation -> position de la wavetable
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "modAttack", 1 }, "Mod Attack",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.01f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "modDecay", 1 }, "Mod Decay",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.8f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "modSustain", 1 }, "Mod Sustain",
                                                        NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "modRelease", 1 }, "Mod Release",
                                                        NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.4f), 0.3f));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "modAmount", 1 }, "Mod Quantite",
                                                        NormalisableRange<float> (-1.0f, 1.0f), 0.5f));

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
    seqNote = -1;
    sampleClock = 0;
    freeBeat = 0.0;
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            v->prepare (sampleRate, samplesPerBlock);
}

void MonSyntheProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    generateSequencerMidi (midi, buffer.getNumSamples());
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* MonSyntheProcessor::createEditor()
{
    return new MonSyntheEditor (*this);
}

//==============================================================================
// Séquenceur : génération des notes MIDI (synchronisé sur l'hôte quand il joue)
//==============================================================================
void MonSyntheProcessor::generateSequencerMidi (juce::MidiBuffer& midi, int numSamples)
{
    const bool enabled = *apvts.getRawParameterValue ("seqOn") > 0.5f;

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
    double bpm = *apvts.getRawParameterValue ("seqTempo");
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
    const double stepBeats = rates[juce::jlimit (0, 2, (int) *apvts.getRawParameterValue ("seqRate"))];
    const int bars = juce::jlimit (1, StepSequencerData::maxBars, (int) *apvts.getRawParameterValue ("seqBars") + 1);
    const int totalSteps = bars * StepSequencerData::stepsPerBar;

    const double samplesPerBeat = 60.0 / juce::jmax (1.0, bpm) * getSampleRate();
    const double gate = *apvts.getRawParameterValue ("seqGate");
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

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MonSyntheProcessor();
}
