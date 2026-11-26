#include "PluginEditor.h"

namespace
{
    void glowStroke (juce::Graphics& g, const juce::Path& p, juce::Colour c, float w)
    {
        g.setColour (c.withAlpha (0.18f));
        g.strokePath (p, juce::PathStrokeType (w * 3.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (c);
        g.strokePath (p, juce::PathStrokeType (w, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    float lfoShapeValue (int shape, double ph)
    {
        switch (shape)
        {
            case 0:  return std::sin ((float) (ph * juce::MathConstants<double>::twoPi));
            case 1:  return 1.0f - 4.0f * std::abs ((float) ph - 0.5f);
            case 2:  return (float) (2.0 * ph - 1.0);
            default: return ph < 0.5 ? 1.0f : -1.0f;
        }
    }

    juce::String fmtHz (float hz)
    {
        return hz >= 1000.0f ? juce::String (hz / 1000.0f, 1) + "k" : juce::String (juce::roundToInt (hz));
    }

    double nowSeconds() { return juce::Time::getMillisecondCounterHiRes() * 0.001; }
}

//==============================================================================
// Look & feel
//==============================================================================
void CyberLookAndFeel::refreshColours()
{
    setColour (juce::PopupMenu::backgroundColourId, Pal::panel);
    setColour (juce::PopupMenu::textColourId, Pal::dim);
    setColour (juce::PopupMenu::headerTextColourId, Pal::indigo);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Pal::cyan.withAlpha (0.25f));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::ComboBox::backgroundColourId, Pal::bg.darker (0.3f));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);

    setColour (juce::TextEditor::backgroundColourId, Pal::bg.darker (0.3f));
    setColour (juce::TextEditor::textColourId, Pal::text);
    setColour (juce::TextEditor::outlineColourId, Pal::line.brighter (0.3f));
    setColour (juce::TextEditor::focusedOutlineColourId, Pal::cyan);
    setColour (juce::TextEditor::highlightColourId, Pal::cyan.withAlpha (0.3f));
    setColour (juce::CaretComponent::caretColourId, Pal::cyan);
}

void CyberLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& slider)
{
    const auto b = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0f * scale);
    const float r = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f - 5.0f * scale;
    if (r < 6.0f)
        return;

    const float cx = b.getCentreX();
    const float cy = b.getCentreY();
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Graduations
    g.setColour (accent.withAlpha (0.35f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (float) i / 10.0f;
        const float s = std::sin (a), c = std::cos (a);
        const float r0 = r + 3.0f * scale;
        const float r1 = r + (i % 5 == 0 ? 6.5f : 4.8f) * scale;
        g.drawLine (cx + s * r0, cy - c * r0, cx + s * r1, cy - c * r1, 1.0f);
    }

    juce::Path track;
    track.addCentredArc (cx, cy, r, r, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (Pal::line.brighter (0.4f));
    g.strokePath (track, juce::PathStrokeType (2.0f * scale));

    juce::Path value;
    value.addCentredArc (cx, cy, r, r, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent.withAlpha (0.22f));                                       // halo lumineux
    g.strokePath (value, juce::PathStrokeType (7.0f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent);
    g.strokePath (value, juce::PathStrokeType (2.8f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float rb = r - 6.0f * scale;
    g.setGradientFill (juce::ColourGradient (Pal::panelHi.brighter (0.25f), cx, cy - rb, Pal::panel, cx, cy + rb, false));
    g.fillEllipse (cx - rb, cy - rb, rb * 2.0f, rb * 2.0f);
    g.setColour (accent.withAlpha (0.45f));
    g.drawEllipse (cx - rb, cy - rb, rb * 2.0f, rb * 2.0f, 1.0f);

    const float s = std::sin (angle), c = std::cos (angle);
    g.setColour (accent.brighter (0.4f));
    g.drawLine (cx + s * rb * 0.35f, cy - c * rb * 0.35f,
                cx + s * (rb - 2.0f * scale), cy - c * (rb - 2.0f * scale), 2.2f * scale);

    const float dot = 2.8f * scale;
    g.setColour (accent.brighter (0.6f));
    g.fillEllipse (cx + s * r - dot, cy - c * r - dot, dot * 2.0f, dot * 2.0f);
}

void CyberLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float, float, juce::Slider::SliderStyle,
                                         juce::Slider& slider)
{
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const float left = (float) x;
    const float cy = (float) y + (float) height * 0.5f;
    const float fillW = juce::jmax (0.0f, sliderPos - left);

    g.setColour (Pal::line.brighter (0.4f));
    g.fillRect (left, cy - 1.0f * scale, (float) width, 2.0f * scale);
    g.setColour (accent.withAlpha (0.25f));
    g.fillRect (left, cy - 3.0f * scale, fillW, 6.0f * scale);
    g.setColour (accent);
    g.fillRect (left, cy - 1.25f * scale, fillW, 2.5f * scale);

    const float d = 5.0f * scale;
    juce::Path diamond;
    diamond.addRectangle (-d, -d, d * 2.0f, d * 2.0f);
    g.setColour (accent.brighter (0.5f));
    g.fillPath (diamond, juce::AffineTransform::rotation (juce::MathConstants<float>::pi * 0.25f)
                             .translated (sliderPos, cy));
}

void CyberLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (juce::roundToInt (10.0f * scale), 1, box.getWidth() - juce::roundToInt (30.0f * scale),
                     box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
}

void CyberLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int,
                                     juce::ComboBox& box)
{
    const auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) w, (float) h).reduced (0.5f);
    const auto accent = box.findColour (juce::ComboBox::arrowColourId);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRect (r);
    g.setColour (accent.withAlpha (0.35f));
    g.drawRect (r, 1.0f);
    g.setColour (accent);
    g.fillRect (r.getX(), r.getY(), 2.5f * scale, r.getHeight());

    const float ax = r.getRight() - 12.0f * scale;
    const float ay = r.getCentreY();
    const float d = 3.5f * scale;
    juce::Path chevron;
    chevron.startNewSubPath (ax - d, ay - d * 0.5f);
    chevron.lineTo (ax, ay + d * 0.5f);
    chevron.lineTo (ax + d, ay - d * 0.5f);
    g.setColour (accent);
    g.strokePath (chevron, juce::PathStrokeType (1.5f * scale));
}

void CyberLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                             bool highlighted, bool down)
{
    const auto r = button.getLocalBounds().toFloat().reduced (0.5f);
    const auto accent = button.findColour (juce::TextButton::buttonOnColourId);

    float a = button.getToggleState() ? 0.26f : 0.0f;
    if (down)             a = 0.42f;
    else if (highlighted) a = juce::jmax (a, 0.14f);

    g.setColour (Pal::bg.darker (0.3f));
    g.fillRect (r);
    g.setColour (accent.withAlpha (a));
    g.fillRect (r);
    g.setColour (accent.withAlpha (button.getToggleState() ? 0.95f : 0.45f));
    g.drawRect (r, 1.0f);

    if (button.getToggleState())
    {
        g.setColour (accent);
        g.fillRect (r.getX(), r.getBottom() - 2.5f * scale, r.getWidth(), 2.5f * scale);
    }
}

//==============================================================================
// Analyseur de spectre
//==============================================================================
SpectrumView::SpectrumView (MonSyntheProcessor& p) : proc (p) {}

void SpectrumView::update()
{
    proc.copyRecentSamples (fftData.data(), fftSize);
    std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);
    window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    const double sr = proc.getSampleRate() > 0.0 ? proc.getSampleRate() : 44100.0;
    const double minHz = 40.0;
    const double maxHz = juce::jmin (16000.0, sr * 0.45);

    for (int bar = 0; bar < numBars; ++bar)
    {
        const double f0 = minHz * std::pow (maxHz / minHz, (double) bar / (double) numBars);
        const double f1 = minHz * std::pow (maxHz / minHz, (double) (bar + 1) / (double) numBars);
        const int bin0 = juce::jlimit (1, fftSize / 2 - 2, (int) (f0 / sr * fftSize));
        const int bin1 = juce::jlimit (bin0 + 1, fftSize / 2 - 1, (int) (f1 / sr * fftSize));

        float peak = 0.0f;
        for (int bin = bin0; bin <= bin1; ++bin)
            peak = juce::jmax (peak, fftData[(size_t) bin]);

        const float magnitude = peak / ((float) fftSize * 0.25f);
        const float db = juce::Decibels::gainToDecibels (magnitude, -80.0f);
        const float v = juce::jlimit (0.0f, 1.0f, juce::jmap (db, -66.0f, -6.0f, 0.0f, 1.0f));

        auto& level = levels[(size_t) bar];
        level = v > level ? v : level * 0.86f + v * 0.14f;
    }

    repaint();
}

void SpectrumView::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat();

    g.setColour (Pal::bg.darker (0.3f));
    g.fillRect (r);
    g.setColour (Pal::line.brighter (0.4f));
    g.drawRect (r, 1.0f);

    const auto inner = r.reduced (3.0f);
    const float barW = inner.getWidth() / (float) numBars;
    const float segH = inner.getHeight() / (float) numSegments;

    for (int bar = 0; bar < numBars; ++bar)
    {
        const float level = levels[(size_t) bar];
        const int lit = (int) std::ceil (level * (float) numSegments - 0.2f);

        for (int seg = 0; seg < numSegments; ++seg)
        {
            const float t = (float) seg / (float) (numSegments - 1);
            const auto colour = t < 0.7f ? Pal::azure.interpolatedWith (Pal::cyan, t / 0.7f)
                                         : Pal::cyan.interpolatedWith (Pal::violet, (t - 0.7f) / 0.3f);

            g.setColour (colour.withAlpha (seg < lit ? 1.0f : 0.08f));
            g.fillRect (inner.getX() + (float) bar * barW + 0.5f,
                        inner.getBottom() - (float) (seg + 1) * segH + 0.5f,
                        juce::jmax (1.0f, barW - 1.0f), juce::jmax (1.0f, segH - 1.0f));
        }
    }
}

//==============================================================================
// Grille du séquenceur
//==============================================================================
juce::Rectangle<float> StepGrid::cellBounds (int step) const
{
    const float labelW = 26.0f;
    const float cw = ((float) getWidth() - labelW) / (float) StepSequencerData::stepsPerBar;
    const float ch = (float) getHeight() / (float) StepSequencerData::maxBars;
    const int row = step / StepSequencerData::stepsPerBar;
    const int col = step % StepSequencerData::stepsPerBar;
    return { labelW + (float) col * cw + 2.0f, (float) row * ch + 2.0f, cw - 4.0f, ch - 4.0f };
}

int StepGrid::stepAt (juce::Point<int> p) const
{
    for (int s = 0; s < StepSequencerData::maxSteps; ++s)
        if (cellBounds (s).contains (p.toFloat()))
            return s;
    return -1;
}

void StepGrid::paint (juce::Graphics& g)
{
    const int bars = juce::jlimit (1, StepSequencerData::maxBars, (int) *proc.apvts.getRawParameterValue ("seqBars") + 1);
    const bool seqOn = *proc.apvts.getRawParameterValue ("seqOn") > 0.5f;
    const int playing = proc.currentStep.load();

    const float cellH = cellBounds (0).getHeight();
    const float fontSize = juce::jlimit (9.0f, 15.0f, cellH * 0.30f);

    g.setFont (Pal::mono (fontSize + 1.0f, true));
    for (int row = 0; row < StepSequencerData::maxBars; ++row)
    {
        g.setColour (accent.withAlpha (row < bars ? 0.9f : 0.3f));
        const auto first = cellBounds (row * StepSequencerData::stepsPerBar);
        g.drawText (juce::String (row + 1), juce::Rectangle<float> (0.0f, first.getY(), 22.0f, first.getHeight()),
                    juce::Justification::centred, false);
    }

    for (int step = 0; step < StepSequencerData::maxSteps; ++step)
    {
        const auto r = cellBounds (step);
        const int row = step / StepSequencerData::stepsPerBar;
        const int col = step % StepSequencerData::stepsPerBar;
        const float dim = row < bars ? 1.0f : 0.3f;
        const bool on = proc.sequencer.on[(size_t) step];

        g.setColour (((col / 4) % 2 == 0 ? Pal::panelHi : Pal::panel.brighter (0.15f)).withMultipliedAlpha (dim));
        g.fillRect (r);

        if (on)
        {
            g.setColour (accent.withAlpha (0.25f * dim));
            g.fillRect (r.expanded (2.0f));
            g.setColour (accent.withAlpha (0.92f * dim));
            g.fillRect (r);

            g.setColour (Pal::bg.darker (0.5f));
            g.setFont (Pal::mono (fontSize, true));
            g.drawText (juce::MidiMessage::getMidiNoteName (proc.sequencer.note[(size_t) step], true, true, 3),
                        r, juce::Justification::centred, false);
        }
        else
        {
            g.setColour (accent.withAlpha (0.16f * dim));
            g.drawRect (r, 1.0f);
        }

        if (seqOn && step == playing && row < bars)
        {
            g.setColour (juce::Colours::white.withAlpha (0.18f));
            g.fillRect (r);
            g.setColour (juce::Colours::white.withAlpha (0.95f));
            g.drawRect (r, 1.5f);
        }
    }
}

void StepGrid::mouseDown (const juce::MouseEvent& e)
{
    dragStep = stepAt (e.getPosition());
    if (dragStep < 0)
        return;

    const auto idx = (size_t) dragStep;

    if (e.mods.isPopupMenu())
    {
        proc.sequencer.on[idx] = false;
        ++proc.sequencer.revision;
        dragStep = -1;
        repaint();
        return;
    }

    wasOn = proc.sequencer.on[idx];
    dragMoved = false;
    dragStartNote = proc.sequencer.note[idx];

    if (! wasOn)
    {
        proc.sequencer.on[idx] = true;
        ++proc.sequencer.revision;
        repaint();
    }
}

void StepGrid::mouseDrag (const juce::MouseEvent& e)
{
    if (dragStep < 0)
        return;

    const int dy = e.getDistanceFromDragStartY();
    if (std::abs (dy) > 3)
        dragMoved = true;

    if (dragMoved)
    {
        proc.sequencer.note[(size_t) dragStep] = juce::jlimit (24, 96, dragStartNote - dy / 6);
        ++proc.sequencer.revision;
        repaint();
    }
}

void StepGrid::mouseUp (const juce::MouseEvent&)
{
    if (dragStep >= 0 && wasOn && ! dragMoved)
    {
        proc.sequencer.on[(size_t) dragStep] = false;
        ++proc.sequencer.revision;
        repaint();
    }
    dragStep = -1;
}

void StepGrid::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    const int step = stepAt (e.getPosition());
    if (step < 0 || ! proc.sequencer.on[(size_t) step])
        return;

    const int delta = wheel.deltaY > 0.0f ? 1 : (wheel.deltaY < 0.0f ? -1 : 0);
    proc.sequencer.note[(size_t) step] = juce::jlimit (24, 96, proc.sequencer.note[(size_t) step].load() + delta);
    ++proc.sequencer.revision;
    repaint();
}

void StepGrid::refresh()
{
    const int step = proc.currentStep.load();
    const int rev = proc.sequencer.revision.load();
    const int bars = (int) *proc.apvts.getRawParameterValue ("seqBars");
    const bool on = *proc.apvts.getRawParameterValue ("seqOn") > 0.5f;

    if (step != lastStep || rev != lastRevision || bars != lastBars || on != lastOn)
    {
        lastStep = step;
        lastRevision = rev;
        lastBars = bars;
        lastOn = on;
        repaint();
    }
}

//==============================================================================
// Éditeur
//==============================================================================
namespace
{
    void fillChoices (juce::AudioProcessorValueTreeState& apvts, juce::ComboBox& box, const char* id)
    {
        if (auto* c = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (id)))
            box.addItemList (c->choices, 1);
    }

    juce::String fmtTime (float s)
    {
        return s < 1.0f ? juce::String (juce::roundToInt (s * 1000.0f)) + "ms" : juce::String (s, 1) + "s";
    }
}

const MonSyntheEditor::MenuDef& MonSyntheEditor::menuAt (int index)
{
    // Ordre = ordre des boutons (2 colonnes, de gauche à droite puis de haut en bas)
    static const MenuDef defs[numMenus] =
    {
        { "OSC A",     0, VizScopeA, VizSpectrum, { { "position", "POSITION" }, { "warpAmtA", "WARP" },
                                                    { "unison", "UNISON" }, { "detune", "DESACCORD" },
                                                    { "oscAOct", "OCTAVE" }, { "oscATune", "ACCORD" },
                                                    { "subLevel", "SUB" }, { "oscALevel", "NIVEAU" } } },
        { "OSC B",     1, VizScopeB, VizFmRing,   { { "oscBLevel", "NIVEAU" }, { "oscBPos", "POSITION" },
                                                    { "warpAmtB", "WARP" }, { "oscBOct", "OCTAVE" },
                                                    { "oscBTune", "ACCORD" }, { "oscBPhase", "PHASE" },
                                                    { "fmAmount", "FM" }, { "ringAmount", "RING" } } },
        { "FM / RING", 2, VizFmRing, VizScopeB,   { { "fmAmount", "FM" }, { "ringAmount", "RING" },
                                                    { "oscBLevel", "NIVEAU B" }, { "subLevel", "SUB" } } },
        { "UNISON",    3, VizUnison, VizScopeA,   { { "unison", "VOIX" }, { "detune", "DESACCORD" },
                                                    { "spread", "STEREO" }, { "uniBlend", "BLEND" } } },
        { "BRUIT",     5, VizNoise, VizSpectrum,  { { "noiseLevel", "NIVEAU" }, { "noiseTone", "TON" },
                                                    { "noiseDecay", "DECAY" } } },
        { "CORDE",     3, VizString, VizSpectrum, { { "strLevel", "NIVEAU" }, { "strDecay", "DUREE" },
                                                    { "strDamp", "AMORTI" }, { "strPos", "PINCEMENT" },
                                                    { "strBright", "BRILLANCE" }, { "strTune", "ACCORD" } } },
        { "PITCH",     0, VizPitch, VizScopeA,    { { "pitchEnvAmt", "QUANTITE" }, { "pitchEnvDecay", "DECAY" },
                                                    { "oscPhase", "PHASE" } } },
        { "FILTRE",    4, VizFilter, VizEnvFilter,{ { "cutoff", "CUTOFF" }, { "resonance", "RESO" },
                                                    { "drive", "DRIVE" }, { "fltEnvAmount", "ENV > CUTOFF" },
                                                    { "fltAttack", "ATTACK FLT" }, { "fltDecay", "DECAY FLT" },
                                                    { "fltSustain", "SUSTAIN FLT" }, { "fltRelease", "RELEASE FLT" } } },
        { "FILTRE 2",  1, VizFilter2, VizSpectrum,{ { "f2Cutoff", "CUTOFF" }, { "f2Res", "RESO" },
                                                    { "f2Mix", "MIX" }, { "f2Vowel", "VOYELLE" } } },
        { "DISTO",     4, VizDisto, VizSpectrum,  { { "distAmt", "QUANTITE" }, { "distMix", "MIX" },
                                                    { "drive", "DRIVE PRE" } } },
        { "ENV VOLUME",5, VizEnvAmp, VizSpectrum, { { "attack", "ATTACK VOL" }, { "decay", "DECAY VOL" },
                                                    { "sustain", "SUSTAIN VOL" }, { "release", "RELEASE VOL" },
                                                    { "velAmp", "VEL > VOL" }, { "velFilter", "VEL > FILTRE" },
                                                    { "voiceGain", "GAIN" } } },
        { "ENV MOD",   0, VizEnvMod, VizSpectrum, { { "modAttack", "ATTACK MOD" }, { "modDecay", "DECAY MOD" },
                                                    { "modSustain", "SUSTAIN MOD" }, { "modRelease", "RELEASE MOD" },
                                                    { "modAmount", "QUANTITE" } } },
        { "LFO",       1, VizLfo1, VizLfo2,       { { "lfoRate", "VITESSE" }, { "lfoDepth", "INTENSITE" },
                                                    { nullptr, nullptr }, { nullptr, nullptr },
                                                    { "lfo2Rate", "VITESSE" } } },
        { "LFO 3-4",   1, VizLfo3, VizLfo4,       { { "lfo3Rate", "VITESSE" }, { nullptr, nullptr },
                                                    { nullptr, nullptr }, { nullptr, nullptr },
                                                    { "lfo4Rate", "VITESSE" } } },
        { "DESSIN",    3, VizNone, VizNone,       { { "dwRate", "VITESSE" }, { "dwSmooth", "LISSAGE" },
                                                    { "dwSteps", "PAS" } } },
        { "JEU",       2, VizGlide, VizSpectrum,  { { "glide", "GLIDE" }, { "bendRange", "BEND" } } },
        { "CHORUS",    3, VizChorus, VizSpectrum, { { "chorusMix", "MIX" }, { "chorusRate", "VITESSE" },
                                                    { "chorusDepth", "PROFONDEUR" }, { nullptr, nullptr },
                                                    { "chorusDelay", "DELAI" }, { "chorusFeedback", "FEEDBACK" } } },
        { "PHASER",    1, VizNone, VizSpectrum,   { { "phaserMix", "MIX" }, { "phaserRate", "VITESSE" },
                                                    { "phaserDepth", "PROFONDEUR" }, { "phaserFeedback", "FEEDBACK" },
                                                    { "phaserCentre", "CENTRE" } } },
        { "DELAY",     4, VizDelay, VizSpectrum,  { { "delayTime", "TEMPS" }, { "delayFeedback", "FEEDBACK" },
                                                    { "delayMix", "MIX" }, { nullptr, nullptr },
                                                    { "delayLP", "FILTRE AIGU" }, { "delayHP", "FILTRE GRAVE" } } },
        { "REVERB",    5, VizReverb, VizSpectrum, { { "reverbSize", "TAILLE" }, { "reverbDamp", "AMORTI" },
                                                    { "reverbMix", "MIX" }, { nullptr, nullptr },
                                                    { "reverbPre", "PRE-DELAY" }, { "reverbWidth", "LARGEUR" },
                                                    { "reverbLP", "FILTRE AIGU" }, { "reverbHP", "FILTRE GRAVE" } } },
        { "EQ",        3, VizNone, VizNone,       { { "eq1Freq", "FREQ" }, { "eq1Gain", "GAIN" },
                                                    { "eq1Q", "Q" }, { "eqOut", "SORTIE" } } },
        { "MASTER",    0, VizComp, VizSpectrum,   { { "compThresh", "SEUIL" }, { "compRatio", "RATIO" },
                                                    { "compAttack", "ATTAQUE" }, { "compRelease", "RELEASE" },
                                                    { "compMakeup", "MAKEUP" }, { "compMix", "MIX" },
                                                    { "limCeil", "LIMITEUR" }, { "width", "LARGEUR" } } },
        { "RACK FX",   2, VizNone, VizNone,       {} },
        { "MATRICE",   4, VizMatrix, VizMatrix,   {} }
    };
    return defs[juce::jlimit (0, numMenus - 1, index)];
}

float MonSyntheEditor::pv (const char* id) const
{
    return proc.apvts.getRawParameterValue (id)->load();
}

MonSyntheEditor::Knob* MonSyntheEditor::makeKnob (const juce::String& paramId, const juce::String& name,
                                                  int role, bool linear)
{
    auto k = std::make_unique<Knob>();
    k->role = role;
    k->linear = linear;
    k->paramId = paramId;

    auto& s = k->slider;
    s.setSliderStyle (linear ? juce::Slider::LinearHorizontal : juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (linear ? juce::Slider::TextBoxRight : juce::Slider::TextBoxBelow, false, 60, 16);
    s.setColour (juce::Slider::rotarySliderFillColourId, Pal::role (role));
    s.setColour (juce::Slider::textBoxTextColourId, Pal::text);
    s.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, true);

    k->label.setText (name, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setColour (juce::Label::textColourId, Pal::dim);
    k->label.setInterceptsMouseClicks (false, false);
    dimLabels.push_back (&k->label);

    if (paramId.isNotEmpty())
        k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, paramId, s);

    addAndMakeVisible (s);
    addAndMakeVisible (k->label);

    s.addMouseListener (this, false);
    auto* raw = k.get();
    s.onDragStart = [this, raw] { autoEnable (raw->paramId); };
    knobs.push_back (std::move (k));
    return raw;
}

void MonSyntheEditor::styleButton (juce::TextButton& b, int role)
{
    b.setColour (juce::TextButton::buttonOnColourId, Pal::role (role));
    b.setColour (juce::TextButton::textColourOffId, Pal::dim);
    b.setColour (juce::TextButton::textColourOnId, Pal::text);
    buttonRefs.push_back ({ &b, role });
    addAndMakeVisible (b);
}

MonSyntheEditor::MonSyntheEditor (MonSyntheProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p), grid (p, juce::Colours::white),
      headerSpectrum (p), screenSpectrum (p)
{
    Pal::update (proc.getThemeHue());
    laf.refreshColours();
    setLookAndFeel (&laf);

    // --- Potards : 8 fixes (réassignés par menu), macros, volume, séquenceur
    for (size_t i = 0; i < fixedKnobs.size(); ++i)
        fixedKnobs[i] = makeKnob ({}, {}, 0);

    for (int i = 0; i < 4; ++i)
        macroKnobs[(size_t) i] = makeKnob ("macro" + juce::String (i + 1), "MACRO " + juce::String (i + 1), 4);

    masterKnob = makeKnob ("master", "VOLUME", 0);
    masterKnob->slider.textFromValueFunction = [] (double v) { return v <= 0.0005 ? juce::String ("-inf dB") : juce::String (20.0 * std::log10 (v), 1) + " dB"; };
    masterKnob->slider.valueFromTextFunction = [] (const juce::String& t)
    {
        const double x = t.getDoubleValue();
        return t.containsIgnoreCase ("db") ? std::pow (10.0, x / 20.0) : x;
    };
    masterKnob->slider.updateText();
    gateKnob   = makeKnob ("seqGate", "GATE", 5, true);
    tempoKnob  = makeKnob ("seqTempo", "TEMPO", 5, true);

    // --- Listes déroulantes liées aux paramètres
    auto setupCombo = [this] (NamedCombo& nc, const char* id, int role)
    {
        fillChoices (proc.apvts, nc.box, id);
        nc.att = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, id, nc.box);
        nc.box.setColour (juce::ComboBox::arrowColourId, Pal::role (role));
        nc.box.setColour (juce::ComboBox::textColourId, Pal::text);
        comboRefs.push_back ({ &nc.box, role });
        addAndMakeVisible (nc.box);
    };

    setupCombo (tableA, "tableA", 0);        setupCombo (warpA, "warpModeA", 0);
    setupCombo (tableB, "tableB", 1);        setupCombo (warpB, "warpModeB", 1);
    setupCombo (filterType, "filterType", 4);
    setupCombo (dwSync, "dwSync", 3);
    setupCombo (f2Type, "f2Type", 1);        setupCombo (f2Mode, "f2Mode", 1);
    setupCombo (lfoShape, "lfoShape", 1);    setupCombo (lfoSync, "lfoSync", 1);
    setupCombo (lfo2Shape, "lfo2Shape", 2);  setupCombo (lfo2Sync, "lfo2Sync", 2);
    setupCombo (lfo3Shape, "lfo3Shape", 3);  setupCombo (lfo3Sync, "lfo3Sync", 3);
    setupCombo (lfo4Shape, "lfo4Shape", 4);  setupCombo (lfo4Sync, "lfo4Sync", 4);
    setupCombo (delaySync, "delaySync", 4);  setupCombo (delayMode, "delayMode", 4);
    setupCombo (noiseType, "noiseType", 5);  setupCombo (phaseMode, "phaseMode", 0);
    setupCombo (distType, "distType", 4);
    setupCombo (quality, "quality", 5);

    styleButton (monoButton, 2);
    monoButton.setButtonText ("MONO");
    monoButton.setClickingTogglesState (true);
    monoAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "mono", monoButton);

    // --- Menus (colonne de gauche)
    for (int i = 0; i < numMenus; ++i)
    {
        auto& b = menuButtons[(size_t) i];
        b.setButtonText (menuAt (i).name);
        b.setClickingTogglesState (false);
        b.setColour (juce::TextButton::buttonOnColourId, Pal::role (menuAt (i).role));
        b.setColour (juce::TextButton::textColourOffId, Pal::dim);
        b.setColour (juce::TextButton::textColourOnId, Pal::text);
        b.onClick = [this, i] { selectMenu (i); };
        addAndMakeVisible (b);
    }

    // --- EQ 8 bandes
    for (int i = 0; i < 8; ++i)
    {
        auto& b = eqBandButtons[(size_t) i];
        b.setButtonText ("BANDE " + juce::String (i + 1));
        b.setClickingTogglesState (false);
        styleButton (b, i % 6);
        b.onClick = [this, i] { selectEqBand (i); };
    }
    setWantsKeyboardFocus (true);

    styleButton (eqOnButton, 0);
    eqOnButton.setClickingTogglesState (true);
    fillChoices (proc.apvts, eqTypeBox, "eq1Type");
    eqTypeBox.setColour (juce::ComboBox::arrowColourId, Pal::cyan);
    eqTypeBox.setColour (juce::ComboBox::textColourId, Pal::text);
    comboRefs.push_back ({ &eqTypeBox, 0 });
    addAndMakeVisible (eqTypeBox);

    setupRack();

    // --- LFO dessiné
    addChildComponent (drawView);
    {
        const char* names[5] = { "RAMPE", "SINUS", "ALEA", "ESCALIER", "PLAT" };
        for (int i = 0; i < 5; ++i)
        {
            auto& b = drawShapeButtons[(size_t) i];
            b.setButtonText (names[i]);
            b.setClickingTogglesState (false);
            styleButton (b, 3);
            b.onClick = [this, i] { drawView.setShape (i); };
        }
    }

    // --- Matrice
    for (int i = 0; i < 12; ++i)
    {
        const juce::String n = "mx" + juce::String (i + 1);
        auto& src = mixSrcBox[(size_t) i];
        auto& dst = mixDstBox[(size_t) i];

        fillChoices (proc.apvts, src, (n + "Src").toRawUTF8());
        fillChoices (proc.apvts, dst, (n + "Dst").toRawUTF8());
        mixSrcAtt[(size_t) i] = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, n + "Src", src);
        mixDstAtt[(size_t) i] = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, n + "Dst", dst);

        src.setColour (juce::ComboBox::arrowColourId, Pal::azure);
        dst.setColour (juce::ComboBox::arrowColourId, Pal::violet);
        src.setColour (juce::ComboBox::textColourId, Pal::text);
        dst.setColour (juce::ComboBox::textColourId, Pal::text);
        comboRefs.push_back ({ &src, 1 });
        comboRefs.push_back ({ &dst, 4 });
        addAndMakeVisible (src);
        addAndMakeVisible (dst);

        mixAmt[(size_t) i] = makeKnob (n + "Amt", "QTE", 2, true);

        matrixComponents.push_back (&src);
        matrixComponents.push_back (&dst);
        matrixComponents.push_back (&mixAmt[(size_t) i]->slider);
        matrixComponents.push_back (&mixAmt[(size_t) i]->label);
    }

    for (auto* l : { &mixHdrSrc, &mixHdrDst, &mixHdrAmt })
    {
        l->setColour (juce::Label::textColourId, Pal::dim);
        l->setInterceptsMouseClicks (false, false);
        addAndMakeVisible (*l);
        dimLabels.push_back (l);
        matrixComponents.push_back (l);
    }
    mixHdrSrc.setText ("SOURCE", juce::dontSendNotification);
    mixHdrDst.setText ("CIBLE", juce::dontSendNotification);
    mixHdrAmt.setText ("QUANTITE", juce::dontSendNotification);
    mixHdrSrc.setJustificationType (juce::Justification::centredLeft);
    mixHdrDst.setJustificationType (juce::Justification::centredLeft);
    mixHdrAmt.setJustificationType (juce::Justification::centredLeft);

    // --- Séquenceur
    styleButton (seqButton, 0);
    seqButton.setButtonText ("SEQUENCEUR ON");
    seqButton.setClickingTogglesState (true);
    seqButtonAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "seqOn", seqButton);

    fillChoices (proc.apvts, barsBox, "seqBars");
    fillChoices (proc.apvts, rateBox, "seqRate");
    barsAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqBars", barsBox);
    rateAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqRate", rateBox);
    for (auto* c : { &barsBox, &rateBox })
    {
        c->setColour (juce::ComboBox::arrowColourId, Pal::teal);
        c->setColour (juce::ComboBox::textColourId, Pal::text);
        comboRefs.push_back ({ c, 5 });
        addAndMakeVisible (*c);
    }
    addAndMakeVisible (grid);

    // --- Clavier a l'ecran (3 octaves) et enregistrement
    keyboard = std::make_unique<juce::MidiKeyboardComponent> (proc.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange (36, 72);
    keyboard->setScrollButtonsVisible (false);
    keyboard->setMidiChannel (1);
    keyboard->setVelocity (0.8f, true);
    addChildComponent (*keyboard);
    applyKeyboardColours();

    styleButton (pianoButton, 1);
    pianoButton.setClickingTogglesState (true);
    pianoButton.onClick = [this]
    {
        pianoMode = pianoButton.getToggleState();
        grid.setVisible (! pianoMode);
        keyboard->setVisible (pianoMode);
        repaint();
    };
    addAndMakeVisible (pianoButton);

    styleButton (recButton, 3);
    recButton.setClickingTogglesState (true);
    recAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "seqRec", recButton);
    addAndMakeVisible (recButton);

    // --- En-tête : presets
    for (auto* b : { &prevButton, &nextButton, &saveButton, &deleteButton, &mutateButton })
        styleButton (*b, 0);
    addAndMakeVisible (mutateButton);
    mutateButton.onClick = [this]
    {
        const bool strong = juce::ModifierKeys::currentModifiers.isShiftDown();
        proc.mutateSound (strong ? 0.5f : 0.15f);
        statusMessage = strong ? "MUTATION FORTE" : "MUTATION DOUCE (Maj + clic = forte)";
        statusFrames = 90;
    };

    presetBox.setColour (juce::ComboBox::arrowColourId, Pal::cyan);
    presetBox.setColour (juce::ComboBox::textColourId, Pal::text);
    comboRefs.push_back ({ &presetBox, 0 });
    addAndMakeVisible (presetBox);

    nameEditor.setMultiLine (false);
    nameEditor.setTextToShowWhenEmpty ("nom du preset", Pal::dim);
    addAndMakeVisible (nameEditor);

    refreshPresetList();

    browser = std::make_unique<PresetBrowser> (proc,
                                               [this] (int id) { presetBox.setSelectedId (id, juce::sendNotificationSync); },
                                               [this] { browser->setVisible (false); browserButton.setToggleState (false, juce::dontSendNotification); });
    addChildComponent (*browser);
    styleButton (browserButton, 1);
    browserButton.setClickingTogglesState (true);
    browserButton.onClick = [this]
    {
        if (browserButton.getToggleState())
        {
            browser->rebuild();
            browser->setSelectedId (presetBox.getSelectedId());
            browser->setVisible (true);
            browser->toFront (false);
        }
        else
            browser->setVisible (false);
    };
    addAndMakeVisible (browserButton);

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (browser != nullptr)
            browser->setSelectedId (id);
        if (id >= 2000)
        {
            const auto name = presetBox.getText();
            if (proc.loadPreset (name))
                nameEditor.setText (name, false);
        }
        else if (id >= 1000)
        {
            proc.loadFactoryPreset (id - 1000);
            nameEditor.setText (presetBox.getText(), false);
        }
    };

    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (1); };

    saveButton.onClick = [this]
    {
        const auto name = nameEditor.getText().trim();
        if (name.isEmpty())
            statusMessage = "DONNE UN NOM AU PRESET";
        else if (proc.savePreset (name))
        {
            refreshPresetList (name);
            statusMessage = "SAUVEGARDE : " + name;
        }
        else
            statusMessage = "ECHEC DE LA SAUVEGARDE";
        statusFrames = 90;
    };

    deleteButton.onClick = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id < 2000)
        {
            statusMessage = "SEUL UN PRESET UTILISATEUR SE SUPPRIME";
            statusFrames = 90;
            return;
        }
        if (deleteArmed > 0)
        {
            proc.deletePreset (presetBox.getText());
            deleteArmed = 0;
            deleteButton.setButtonText ("SUPPR");
            refreshPresetList();
            statusMessage = "PRESET SUPPRIME";
            statusFrames = 60;
        }
        else
        {
            deleteArmed = 75;
            deleteButton.setButtonText ("SUR ?");
        }
    };

    // --- Import de wavetables
    styleButton (importAButton, 2);
    styleButton (importBButton, 2);
    importAButton.onClick = [this] { importWavetable (0); };
    importBButton.onClick = [this] { importWavetable (1); };
    styleButton (resampleAButton, 4);
    styleButton (resampleBButton, 4);
    resampleAButton.onClick = [this] { startResample (0); };
    resampleBButton.onClick = [this] { startResample (1); };

    // --- Visuels et couleur centrale
    addAndMakeVisible (headerSpectrum);
    addAndMakeVisible (screenSpectrum);
    addAndMakeVisible (hueBar);
    hueBar.setValue (Pal::hue);
    hueBar.onChange = [this] (float h, bool released)
    {
        proc.setThemeHue (h, released);
        Pal::update (h);
        laf.refreshColours();
        sendLookAndFeelChange();
        applyTheme();
    };

    selectMenu (0);
    applyTheme();

    setResizable (true, true);
    getConstrainer()->setFixedAspectRatio ((double) baseWidth / (double) baseHeight);
    setResizeLimits (640, 360, 2560, 1440);
    startTimerHz (30);
    setSize (baseWidth, baseHeight);
}

MonSyntheEditor::~MonSyntheEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

//==============================================================================
void MonSyntheEditor::selectMenu (int index)
{
    currentMenu = juce::jlimit (0, numMenus - 1, index);
    const auto& def = menuAt (currentMenu);

    for (size_t i = 0; i < fixedKnobs.size(); ++i)
    {
        auto* k = fixedKnobs[i];
        k->attachment.reset();
        k->paramId = {};

        if (def.p[i].id != nullptr)
        {
            k->role = i < 4 ? def.role : (def.role + 1) % 6;
            k->slider.setColour (juce::Slider::rotarySliderFillColourId, Pal::role (k->role));
            k->label.setText (def.p[i].label, juce::dontSendNotification);
            k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, def.p[i].id, k->slider);
            k->paramId = def.p[i].id;
        }
    }

    for (int i = 0; i < numMenus; ++i)
        menuButtons[(size_t) i].setToggleState (i == currentMenu, juce::dontSendNotification);

    if (currentMenu == mEq)
        selectEqBand (eqBand);

    updateMenuVisibility();
    repaint();
}

void MonSyntheEditor::updateMenuVisibility()
{
    const auto& def = menuAt (currentMenu);
    const bool isMatrix = currentMenu == matrixMenu;

    for (size_t i = 0; i < fixedKnobs.size(); ++i)
    {
        const bool show = ! isMatrix && def.p[i].id != nullptr;
        fixedKnobs[i]->slider.setVisible (show);
        fixedKnobs[i]->label.setVisible (show);
    }

    for (auto* c : matrixComponents)
        c->setVisible (isMatrix);

    tableA.box.setVisible (currentMenu == mOscA);
    warpA.box.setVisible (currentMenu == mOscA);
    importAButton.setVisible (currentMenu == mOscA);
    tableB.box.setVisible (currentMenu == mOscB);
    warpB.box.setVisible (currentMenu == mOscB);
    importBButton.setVisible (currentMenu == mOscB);
    resampleAButton.setVisible (currentMenu == mOscA);
    resampleBButton.setVisible (currentMenu == mOscB);
    filterType.box.setVisible (currentMenu == mFilter);
    drawView.setVisible (currentMenu == mDraw);
    dwSync.box.setVisible (currentMenu == mDraw);
    for (auto& b : drawShapeButtons)
        b.setVisible (currentMenu == mDraw);
    f2Type.box.setVisible (currentMenu == mFilter2);
    f2Mode.box.setVisible (currentMenu == mFilter2);
    lfoShape.box.setVisible (currentMenu == mLfo);
    lfoSync.box.setVisible (currentMenu == mLfo);
    lfo2Shape.box.setVisible (currentMenu == mLfo);
    lfo2Sync.box.setVisible (currentMenu == mLfo);
    delaySync.box.setVisible (currentMenu == mDelay);
    delayMode.box.setVisible (currentMenu == mDelay);
    lfo3Shape.box.setVisible (currentMenu == mLfo34);
    lfo3Sync.box.setVisible (currentMenu == mLfo34);
    lfo4Shape.box.setVisible (currentMenu == mLfo34);
    lfo4Sync.box.setVisible (currentMenu == mLfo34);
    monoButton.setVisible (currentMenu == mJeu);
    noiseType.box.setVisible (currentMenu == mNoise);
    for (auto& b : eqBandButtons)
        b.setVisible (currentMenu == mEq);
    for (auto& r : rack)
    {
        const bool v = currentMenu == mRack;
        r.onBtn.setVisible (v);  r.leftBtn.setVisible (v);  r.rightBtn.setVisible (v);  r.typeBox.setVisible (v);
        for (auto* k : r.k)
        {
            k->slider.setVisible (v);
            k->label.setVisible (v);
        }
    }
    eqOnButton.setVisible (currentMenu == mEq);
    eqTypeBox.setVisible (currentMenu == mEq);
    phaseMode.box.setVisible (currentMenu == mPitch);
    distType.box.setVisible (currentMenu == mDisto);
    quality.box.setVisible (currentMenu == mMaster);

    const bool specA = ! isMatrix && def.vizA == VizSpectrum;
    const bool specB = ! isMatrix && ! specA && def.vizB == VizSpectrum;
    screenSpectrum.setVisible (specA || specB);
    if (specA)      screenSpectrum.setBounds (gfxA.reduced (S (2.0f)));
    else if (specB) screenSpectrum.setBounds (gfxB.reduced (S (2.0f)));
}

void MonSyntheEditor::applyTheme()
{
    const auto& def = menuAt (currentMenu);

    for (auto& k : knobs)
    {
        int role = k->role;
        for (size_t i = 0; i < fixedKnobs.size(); ++i)
            if (fixedKnobs[i] == k.get())
                role = k->role = (def.p[i].id != nullptr ? (i < 4 ? def.role : (def.role + 1) % 6) : k->role);

        k->slider.setColour (juce::Slider::rotarySliderFillColourId, Pal::role (role));
        k->slider.setColour (juce::Slider::textBoxTextColourId, Pal::text);
        k->slider.repaint();
    }

    for (auto* l : dimLabels)
        l->setColour (juce::Label::textColourId, Pal::dim);

    for (auto& c : comboRefs)
    {
        c.box->setColour (juce::ComboBox::arrowColourId, Pal::role (c.role));
        c.box->setColour (juce::ComboBox::textColourId, Pal::text);
        c.box->repaint();
    }

    for (auto& b : buttonRefs)
    {
        b.button->setColour (juce::TextButton::buttonOnColourId, Pal::role (b.role));
        b.button->setColour (juce::TextButton::textColourOffId, Pal::dim);
        b.button->setColour (juce::TextButton::textColourOnId, Pal::text);
        b.button->repaint();
    }

    for (int i = 0; i < numMenus; ++i)
    {
        auto& b = menuButtons[(size_t) i];
        b.setColour (juce::TextButton::buttonOnColourId, Pal::role (menuAt (i).role));
        b.setColour (juce::TextButton::textColourOffId, Pal::dim);
        b.setColour (juce::TextButton::textColourOnId, Pal::text);
        b.repaint();
    }

    nameEditor.applyColourToAllText (Pal::text, true);
    grid.setAccent (Pal::cyan);
    applyKeyboardColours();
    repaint();
}

void MonSyntheEditor::updateLeds()
{
    const double t = nowSeconds();
    const bool active = proc.getActiveVoiceMask() != 0;
    const double bpm = proc.getHostBpm() > 1.0 ? proc.getHostBpm() : 120.0;

    auto pulse = [t] (double hz, double phase)
    {
        return 0.5f + 0.5f * std::sin ((float) (t * hz * juce::MathConstants<double>::twoPi + phase));
    };

    for (int i = 0; i < numMenus; ++i)
    {
        float lvl = 0.12f;

        switch (i)
        {
            case mOscA: case mOscB: case mFmRing: case mUnison:
                lvl = active ? 0.35f + 0.65f * pulse (1.3 + 0.4 * i, i) : 0.12f;  break;
            case mString: lvl = pv ("strLevel") > 0.01f ? 0.35f + 0.65f * pulse (2.0, 0.0) : 0.12f;  break;
            case mNoise:  lvl = pv ("noiseLevel") > 0.01f ? 0.3f + 0.7f * pulse (5.0, 0.0) : 0.12f;  break;
            case mPitch:  lvl = std::abs (pv ("pitchEnvAmt")) > 0.5f ? (active ? 1.0f : 0.6f) : 0.12f;  break;
            case mDraw:
            {
                int used = 0;
                for (int r = 1; r <= 12; ++r)
                    if ((int) pv (("mx" + juce::String (r) + "Src").toRawUTF8()) == 16 && std::abs (pv (("mx" + juce::String (r) + "Amt").toRawUTF8())) > 0.01f)
                        ++used;
                lvl = used > 0 ? 0.4f + 0.6f * pulse (juce::jmin (8.0, (double) pv ("dwRate")), 0.0) : 0.12f;
                break;
            }
            case mFilter2: lvl = (int) pv ("f2Type") > 0 ? (active ? 0.35f + 0.65f * pulse (2.6, 1.5) : 0.6f) : 0.12f;  break;
            case mFilter: lvl = active ? 0.35f + 0.65f * pulse (2.1, 0.5) : 0.12f;     break;
            case mDisto:  lvl = pv ("distAmt") > 0.01f ? 0.4f + 0.6f * pulse (7.0, 0.0) : 0.12f;  break;
            case mEnvAmp: lvl = active ? 1.0f : 0.12f;                                  break;
            case mEnvMod: lvl = active ? 0.35f + 0.65f * pulse (1.7, 1.0) : 0.12f;     break;
            case mLfo:
            {
                const double hz = pv ("lfoSync") > 0.5f ? bpm / 60.0 : (double) pv ("lfoRate");
                lvl = 0.12f + 0.88f * pulse (juce::jmin (hz, 8.0), 0.0);
                break;
            }
            case mLfo34:
            {
                const double hz = pv ("lfo3Sync") > 0.5f ? bpm / 60.0 : (double) pv ("lfo3Rate");
                lvl = 0.12f + 0.88f * pulse (juce::jmin (hz, 8.0), 0.0);
                break;
            }
            case mJeu:    lvl = pv ("mono") > 0.5f ? 0.9f : 0.25f;                      break;
            case mPhaser: lvl = pv ("phaserMix") > 0.01f ? 0.45f + 0.55f * pulse (pv ("phaserRate"), 0.0) : 0.12f; break;
            case mChorus: lvl = pv ("chorusMix") > 0.01f ? 0.45f + 0.55f * pulse (pv ("chorusRate"), 0.0) : 0.12f; break;
            case mDelay:  lvl = pv ("delayMix") > 0.01f
                              ? 0.2f + 0.8f * pulse (juce::jmin (1000.0 / (double) juce::jmax (20.0f, pv ("delayTime")), 8.0), 0.0)
                              : 0.12f;
                          break;
            case mReverb: lvl = pv ("reverbMix") > 0.01f ? 0.8f : 0.12f;                break;
            case mEq:
            {
                int on = 0;
                for (int b = 1; b <= 8; ++b)
                    if (pv (("eq" + juce::String (b) + "On").toRawUTF8()) > 0.5f)
                        ++on;
                lvl = on > 0 ? 0.5f + 0.5f * (float) on / 8.0f : 0.12f;
                break;
            }
            case mRack:
            {
                int on = 0;
                for (int s = 1; s <= 4; ++s)
                    if (pv (("fx" + juce::String (s) + "Type").toRawUTF8()) > 0.5f && pv (("fx" + juce::String (s) + "On").toRawUTF8()) > 0.5f)
                        ++on;
                lvl = on > 0 ? 0.4f + 0.6f * (float) on / 4.0f : 0.12f;
                break;
            }
            case mMaster: lvl = pv ("compThresh") < -0.1f ? 0.4f + 0.6f * pulse (3.0, 0.0) : (active ? 0.7f : 0.12f);  break;
            default:
            {
                int used = 0;
                for (int r = 1; r <= 12; ++r)
                    if (std::abs (pv (("mx" + juce::String (r) + "Amt").toRawUTF8())) > 0.01f)
                        ++used;
                lvl = used > 0 ? 0.5f + 0.5f * (float) used / 12.0f : 0.12f;
                break;
            }
        }

        if (i == currentMenu)
            lvl = 1.0f;

        menuButtons[(size_t) i].setLed (lvl, Pal::role (menuAt (i).role));
    }
}

//==============================================================================
void MonSyntheEditor::placeKnob (Knob* k, juce::Rectangle<int> b)
{
    if (k->linear)
    {
        k->label.setBounds (b.removeFromLeft (S (42.0f)));
        k->slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, S (46.0f), juce::jmax (8, b.getHeight()));
        k->slider.setBounds (b);
    }
    else
    {
        k->label.setBounds (b.removeFromTop (S (14.0f)));
        k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, S (70.0f), S (16.0f));
        k->slider.setBounds (b);
    }
}

void MonSyntheEditor::layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content)
{
    if (items.empty())
        return;

    const int w = content.getWidth() / (int) items.size();
    for (size_t i = 0; i < items.size(); ++i)
        placeKnob (items[i], juce::Rectangle<int> (content.getX() + (int) i * w, content.getY(), w, content.getHeight())
                                 .reduced (S (4.0f)));
}

void MonSyntheEditor::resized()
{
    if (masterKnob == nullptr)
        return;

    scale = (float) getWidth() / (float) baseWidth;
    laf.setScale (scale);

    auto R = [this] (int x, int y, int w, int h) { return juce::Rectangle<int> (S ((float) x), S ((float) y), S ((float) w), S ((float) h)); };

    // En-tête
    prevButton.setBounds (R (176, 12, 26, 28));
    presetBox.setBounds (R (206, 12, 180, 28));
    nextButton.setBounds (R (390, 12, 26, 28));
    nameEditor.setBounds (R (424, 12, 110, 28));
    saveButton.setBounds (R (538, 12, 48, 28));
    deleteButton.setBounds (R (590, 12, 56, 28));
    browserButton.setBounds (R (650, 12, 74, 28));
    mutateButton.setBounds (R (728, 12, 56, 28));
    headerSpectrum.setBounds (R (792, 8, 210, 38));
    if (browser != nullptr)
        browser->setBounds (R (184, 60, 828, 476));
    hueBar.setBounds (R (1020, 12, 240, 28));

    // Menus
    menuPanel = R (8, 60, 168, 476);
    for (int i = 0; i < numMenus; ++i)
        menuButtons[(size_t) i].setBounds (R (14 + (i % 2) * 78, 88 + (i / 2) * 37, 74, 34));

    // Écrans
    screenAPanel = R (184, 60, 410, 476);
    screenBPanel = R (602, 60, 410, 476);
    gfxA   = R (194, 90, 390, 236);   gfxB   = R (612, 90, 390, 236);
    stripA = R (194, 332, 390, 28);   stripB = R (612, 332, 390, 28);
    knobsA = R (194, 366, 390, 162);  knobsB = R (612, 366, 390, 162);
    scopeArea = gfxA;

    for (int s = 0; s < 2; ++s)
    {
        const auto strip = s == 0 ? stripA : stripB;
        const int w1 = juce::roundToInt (strip.getWidth() * 0.42f);
        const int w2 = juce::roundToInt (strip.getWidth() * 0.30f);
        const auto first  = juce::Rectangle<int> (strip.getX(), strip.getY(), w1 - S (4.0f), strip.getHeight());
        const auto second = juce::Rectangle<int> (strip.getX() + w1, strip.getY(), w2 - S (4.0f), strip.getHeight());
        const auto third  = juce::Rectangle<int> (strip.getX() + w1 + w2, strip.getY(), strip.getWidth() - w1 - w2, strip.getHeight());

        if (s == 0)
        {
            const auto t1 = third.withWidth (third.getWidth() / 2 - S (2.0f));
            const auto t2 = third.withTrimmedLeft (third.getWidth() / 2);
            tableA.box.setBounds (first);       warpA.box.setBounds (second);       importAButton.setBounds (t1);  resampleAButton.setBounds (t2);
            tableB.box.setBounds (first);       warpB.box.setBounds (second);       importBButton.setBounds (t1);  resampleBButton.setBounds (t2);
            filterType.box.setBounds (first);
            dwSync.box.setBounds (first);
            f2Type.box.setBounds (first);       f2Mode.box.setBounds (second);
            lfoShape.box.setBounds (first);     lfoSync.box.setBounds (second);
            lfo3Shape.box.setBounds (first);    lfo3Sync.box.setBounds (second);
            monoButton.setBounds (first);
            noiseType.box.setBounds (first);    phaseMode.box.setBounds (first);    distType.box.setBounds (first);
            quality.box.setBounds (first);
        }
        else
        {
            lfo2Shape.box.setBounds (first);    lfo2Sync.box.setBounds (second);
            delaySync.box.setBounds (first);    delayMode.box.setBounds (second);
            lfo4Shape.box.setBounds (first);    lfo4Sync.box.setBounds (second);
        }
    }

    for (int s = 0; s < 2; ++s)
    {
        std::vector<Knob*> row;
        for (int i = 0; i < 4; ++i)
            row.push_back (fixedKnobs[(size_t) (s * 4 + i)]);
        layoutRow (row, s == 0 ? knobsA : knobsB);
    }

    // EQ (graphique sur les deux écrans, bandes sous le graphique)
    eqGraphArea = gfxA.getUnion (gfxB);
    for (int i = 0; i < 8; ++i)
        eqBandButtons[(size_t) i].setBounds (R (194 + i * 101, 332, 97, 28));
    eqOnButton.setBounds (R (632, 384, 160, 34));
    eqTypeBox.setBounds (R (632, 430, 160, 30));

    // LFO dessiné : zone de dessin sur les deux écrans, boutons de formes dans la bande du bas
    drawView.setBounds (gfxA.getUnion (gfxB));
    for (int i = 0; i < 5; ++i)
        drawShapeButtons[(size_t) i].setBounds (R (352 + i * 110, 332, 104, 28));

    // Rack : 4 colonnes sur les deux écrans
    for (int s = 0; s < 4; ++s)
    {
        auto& r = rack[(size_t) s];
        const int x = 194 + s * 204, w = 196;
        r.area = R (x, 92, w, 436);
        r.onBtn.setBounds (R (x + 6, 114, 84, 26));
        r.leftBtn.setBounds (R (x + 96, 114, 44, 26));
        r.rightBtn.setBounds (R (x + 146, 114, 44, 26));
        r.typeBox.setBounds (R (x + 6, 146, w - 12, 28));
        placeKnob (r.k[0], R (x + 4, 182, 94, 98));
        placeKnob (r.k[1], R (x + 98, 182, 94, 98));
        placeKnob (r.k[2], R (x + 4, 284, 94, 98));
        placeKnob (r.k[3], R (x + 98, 284, 94, 98));
        placeKnob (r.k[4], R (x + 6, 392, w - 12, 28));
    }

    // Matrice (occupe les deux écrans)
    matrixArea = R (194, 92, 808, 436);
    {
        const int x0 = matrixArea.getX();
        mixHdrSrc.setBounds (x0, matrixArea.getY(), S (220.0f), S (22.0f));
        mixHdrDst.setBounds (x0 + S (256.0f), matrixArea.getY(), S (220.0f), S (22.0f));
        mixHdrAmt.setBounds (x0 + S (496.0f), matrixArea.getY(), S (312.0f), S (22.0f));

        for (int i = 0; i < 12; ++i)
        {
            const int y = matrixArea.getY() + S (26.0f) + i * S (34.0f) + S (2.0f);
            mixSrcBox[(size_t) i].setBounds (x0, y, S (220.0f), S (28.0f));
            mixDstBox[(size_t) i].setBounds (x0 + S (256.0f), y, S (220.0f), S (28.0f));
            placeKnob (mixAmt[(size_t) i], juce::Rectangle<int> (x0 + S (496.0f), y, S (312.0f), S (28.0f)));
        }
    }

    // Colonne de droite
    macrosPanel = R (1020, 60, 252, 250);
    for (int i = 0; i < 4; ++i)
        placeKnob (macroKnobs[(size_t) i],
                   R (1028 + (i % 2) * 120, 90 + (i / 2) * 108, 118, 104).reduced (S (4.0f)));

    outPanel = R (1020, 318, 252, 218);
    placeKnob (masterKnob, R (1028, 346, 120, 182));
    meterArea = R (1158, 352, 24, 168);
    ledsArea  = R (1196, 352, 66, 120);

    // Séquenceur
    seqPanel = R (8, 544, 1264, 168);
    seqButton.setBounds (R (18, 572, 150, 24));
    barsBox.setBounds (R (18, 600, 150, 22));
    rateBox.setBounds (R (18, 626, 150, 22));
    placeKnob (gateKnob, R (18, 652, 150, 20));
    placeKnob (tempoKnob, R (18, 676, 150, 20));
    grid.setBounds (R (180, 572, 1080, 132));
    pianoButton.setBounds (R (960, 547, 90, 20));
    recButton.setBounds (R (1056, 547, 90, 20));
    if (keyboard != nullptr)
    {
        const auto kb = R (180, 572, 1080, 132);
        keyboard->setBounds (kb);
        keyboard->setKeyWidth ((float) kb.getWidth() / 22.0f);     // 22 touches blanches de C2 a C5
        keyboard->setLowestVisibleKey (36);
    }
    beatLedArea = R (1160, 548, 96, 16);

    updateMenuVisibility();
}

//==============================================================================
void MonSyntheEditor::timerCallback()
{
    grid.refresh();
    headerSpectrum.update();
    if (screenSpectrum.isVisible())
        screenSpectrum.update();

    float buf[256];
    proc.copyRecentSamples (buf, 256);
    float peak = 0.0f;
    for (float v : buf)
        peak = juce::jmax (peak, std::abs (v));
    const float level = juce::jlimit (0.0f, 1.0f, (juce::Decibels::gainToDecibels (peak, -60.0f) + 60.0f) / 60.0f);
    outLevel = level > outLevel ? level : outLevel * 0.9f;

    if (const int learned = proc.takeLearnedCc(); learned >= 0)
    {
        statusMessage = "CC " + juce::String (learned) + " -> " + proc.getLastLearnedParamName();
        statusFrames = 200;
    }

    if (resampleSlot >= 0)
    {
        const int st = proc.getResampleState();
        if (st == 2)
        {
            juce::String error;
            const int slot = resampleSlot;
            resampleSlot = -1;
            if (proc.finishResample (slot, error))
            {
                auto& box = slot == 0 ? tableA.box : tableB.box;
                box.setSelectedItemIndex (WavetableBank::numTables + slot, juce::sendNotificationSync);
                statusMessage = "RESAMPLE -> USER " + juce::String (slot + 1) + " : 32 formes d'onde";
            }
            else
                statusMessage = error;
            statusFrames = 150;
        }
        else if (st == 1)
        {
            statusMessage = "RESAMPLE : JOUE ET TIENS UNE NOTE...  " + juce::String (juce::roundToInt (proc.getResampleProgress() * 100.0f)) + "%";
            statusFrames = 5;
        }
    }

    updateLeds();
    if (currentMenu == mRack)
        refreshRack();

    // Survol d'un potard : valeur réelle + rappel des raccourcis
    if (hoveredKnob != nullptr && statusFrames <= 1 && hoveredKnob->slider.isShowing() && hoveredKnob->label.getText().isNotEmpty())
    {
        const auto msg = hoveredKnob->label.getText() + " = " + hoveredKnob->slider.getTextFromValue (hoveredKnob->slider.getValue())
                         + "     [haut/bas] 1 unite   [gauche/droite] fin 0,25 %   [Maj] x10   [clic droit] valeur, MIDI learn, modulation";
        if (msg != statusMessage)
        {
            statusMessage = msg;
            repaint (seqPanel);
        }
        statusFrames = 2;
    }

    if (statusFrames > 0)
        --statusFrames;
    if (deleteArmed > 0 && --deleteArmed == 0)
        deleteButton.setButtonText ("SUPPR");

    repaint (gfxA);
    repaint (gfxB);
    if (currentMenu == mEq)
        repaint (eqGraphArea);
    if (currentMenu == mDraw)
        drawView.repaint();
    repaint (meterArea.expanded (S (4.0f)));
    repaint (ledsArea.expanded (S (4.0f)));
    repaint (seqPanel.withHeight (S (26.0f)));
    repaint (juce::Rectangle<int> (S (10.0f), S (4.0f), S (160.0f), S (44.0f)));
}

//==============================================================================
// Presets et import
//==============================================================================
void MonSyntheEditor::refreshPresetList (const juce::String& select)
{
    if (browser != nullptr && browser->isVisible())
        browser->rebuild();

    presetBox.clear (juce::dontSendNotification);

    juce::StringArray cats;
    const int nf = MonSyntheProcessor::getNumFactoryPresets();
    for (int i = 0; i < nf; ++i)
        cats.addIfNotAlreadyThere (MonSyntheProcessor::getFactoryPresetCategory (i));

    for (const auto& cat : cats)
    {
        presetBox.addSectionHeading (cat);
        for (int i = 0; i < nf; ++i)
            if (MonSyntheProcessor::getFactoryPresetCategory (i) == cat)
                presetBox.addItem (MonSyntheProcessor::getFactoryPresetName (i), 1000 + i);
    }

    const auto users = proc.getPresetNames();
    if (users.size() > 0)
    {
        presetBox.addSectionHeading ("UTILISATEUR");
        for (int j = 0; j < users.size(); ++j)
            presetBox.addItem (users[j], 2000 + j);
    }

    if (select.isNotEmpty())
        for (int j = 0; j < users.size(); ++j)
            if (users[j] == select)
                presetBox.setSelectedId (2000 + j, juce::dontSendNotification);
}

void MonSyntheEditor::stepPreset (int delta)
{
    std::vector<int> ids;
    for (int i = 0; i < presetBox.getNumItems(); ++i)
        if (presetBox.getItemId (i) != 0)
            ids.push_back (presetBox.getItemId (i));

    if (ids.empty())
        return;

    const int cur = presetBox.getSelectedId();
    int idx = -1;
    for (size_t i = 0; i < ids.size(); ++i)
        if (ids[i] == cur)
            idx = (int) i;

    idx = idx < 0 ? (delta > 0 ? 0 : (int) ids.size() - 1)
                  : (idx + delta + (int) ids.size()) % (int) ids.size();
    presetBox.setSelectedId (ids[(size_t) idx], juce::sendNotificationSync);
}

void MonSyntheEditor::importWavetable (int slot)
{
    chooser = std::make_unique<juce::FileChooser> ("Choisir un fichier audio (wavetable)", juce::File(),
                                                   "*.wav;*.aif;*.aiff;*.flac");
    juce::Component::SafePointer<MonSyntheEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [safe, slot] (const juce::FileChooser& fc)
                          {
                              const auto f = fc.getResult();
                              if (safe != nullptr && f.existsAsFile())
                                  safe->loadWavetableFile (slot, f);
                          });
}

void MonSyntheEditor::loadWavetableFile (int slot, const juce::File& file)
{
    juce::String error;
    if (proc.loadUserWavetable (slot, file, error))
    {
        auto& box = slot == 0 ? tableA.box : tableB.box;
        box.setSelectedItemIndex (WavetableBank::numTables + slot, juce::sendNotificationSync);
        statusMessage = "USER " + juce::String (slot + 1) + " : " + proc.getUserTableName (slot);
    }
    else
        statusMessage = error.isNotEmpty() ? error : "IMPORT IMPOSSIBLE";

    statusFrames = 120;
    repaint();
}

bool MonSyntheEditor::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& f : files)
    {
        const auto ext = juce::File (f).getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".flac")
            return true;
    }
    return false;
}

void MonSyntheEditor::filesDropped (const juce::StringArray& files, int, int)
{
    if (files.isEmpty())
        return;
    loadWavetableFile (currentMenu == mOscB ? 1 : 0, juce::File (files[0]));
}

//==============================================================================
// Dessin
//==============================================================================
void MonSyntheEditor::drawPanel (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title,
                                 juce::Colour colour, const juce::String& rightText)
{
    const auto r = area.toFloat();
    const float c = 10.0f * scale;

    juce::Path p;
    p.startNewSubPath (r.getX() + c, r.getY());
    p.lineTo (r.getRight(), r.getY());
    p.lineTo (r.getRight(), r.getBottom());
    p.lineTo (r.getX(), r.getBottom());
    p.lineTo (r.getX(), r.getY() + c);
    p.closeSubPath();

    g.setColour (Pal::panel);
    g.fillPath (p);
    g.setColour (Pal::line);
    g.strokePath (p, juce::PathStrokeType (1.0f));

    g.setColour (colour);
    g.fillRect (r.getX() + c, r.getY(), 46.0f * scale, 2.5f * scale);

    const float b = 9.0f * scale;
    g.drawLine (r.getRight() - b, r.getBottom() - 1.0f, r.getRight() - 1.0f, r.getBottom() - 1.0f, 1.5f);
    g.drawLine (r.getRight() - 1.0f, r.getBottom() - b, r.getRight() - 1.0f, r.getBottom() - 1.0f, 1.5f);

    g.setFont (Pal::mono (11.0f * scale, true));
    g.setColour (colour);
    g.drawText (title, area.getX() + S (14.0f), area.getY() + S (5.0f), area.getWidth() - S (28.0f), S (18.0f),
                juce::Justification::centredLeft, false);

    if (rightText.isNotEmpty())
    {
        g.setColour (Pal::dim);
        g.drawText (rightText, area.getX() + S (14.0f), area.getY() + S (5.0f), area.getWidth() - S (28.0f), S (18.0f),
                    juce::Justification::centredRight, false);
    }
}

void MonSyntheEditor::drawLogo (juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto r = area.toFloat();
    const float cx = r.getX() + r.getHeight() * 0.5f;
    const float cy = r.getCentreY();
    const float rad = r.getHeight() * 0.42f;

    juce::Path hex;
    for (int k = 0; k < 6; ++k)
    {
        const float a = juce::MathConstants<float>::pi / 6.0f + (float) k * juce::MathConstants<float>::pi / 3.0f;
        const float x = cx + std::cos (a) * rad, y = cy + std::sin (a) * rad;
        if (k == 0) hex.startNewSubPath (x, y); else hex.lineTo (x, y);
    }
    hex.closeSubPath();

    g.setColour (Pal::cyan.withAlpha (0.12f));
    g.fillPath (hex);
    glowStroke (g, hex, Pal::cyan, 1.6f * scale);

    juce::Path chev;
    chev.startNewSubPath (cx - rad * 0.32f, cy - rad * 0.38f);
    chev.lineTo (cx + rad * 0.28f, cy);
    chev.lineTo (cx - rad * 0.32f, cy + rad * 0.38f);
    g.setColour (Pal::cyan.brighter (0.4f));
    g.strokePath (chev, juce::PathStrokeType (2.2f * scale));

    const float pulse = 0.5f + 0.5f * std::sin ((float) (nowSeconds() * 3.0));
    g.setColour (Pal::violet.withAlpha (0.35f + 0.65f * pulse));
    g.fillEllipse (cx + rad * 0.45f - 2.5f * scale, cy - 2.5f * scale, 5.0f * scale, 5.0f * scale);

    const float tx = cx + rad + 10.0f * scale;
    g.setColour (Pal::text);
    g.setFont (Pal::mono (24.0f * scale, true));
    g.drawText ("MS1", juce::Rectangle<float> (tx, r.getY(), 90.0f * scale, r.getHeight() * 0.62f),
                juce::Justification::centredLeft, false);
    g.setColour (Pal::dim);
    g.setFont (Pal::mono (8.5f * scale));
   #ifdef MS1_VERSION_TEXT
    g.drawText (MS1_VERSION_TEXT, juce::Rectangle<float> (tx + 52.0f * scale, r.getY() + r.getHeight() * 0.14f, 80.0f * scale, r.getHeight() * 0.3f),
                juce::Justification::centredLeft, false);
   #endif
    g.drawText ("WAVETABLE SYNTH", juce::Rectangle<float> (tx, r.getY() + r.getHeight() * 0.58f, 130.0f * scale, r.getHeight() * 0.36f),
                juce::Justification::centredLeft, false);
}

void MonSyntheEditor::paint (juce::Graphics& g)
{
    g.fillAll (Pal::bg);

    // Léger quadrillage anthracite sur tout le fond
    {
        g.setColour (Pal::grid.withAlpha (0.16f));
        const float step = 24.0f * scale;
        for (float x = 0.0f; x < (float) getWidth(); x += step)
            g.drawVerticalLine ((int) x, 0.0f, (float) getHeight());
        for (float y = 0.0f; y < (float) getHeight(); y += step)
            g.drawHorizontalLine ((int) y, 0.0f, (float) getWidth());
    }

    // En-tête
    g.setColour (Pal::panel);
    g.fillRect (0, 0, getWidth(), S (54.0f));
    g.setColour (Pal::cyan.withAlpha (0.5f));
    g.fillRect (0, S (54.0f), getWidth(), juce::jmax (1, S (1.5f)));
    drawLogo (g, juce::Rectangle<int> (S (10.0f), S (4.0f), S (160.0f), S (44.0f)));

    // Menus
    drawPanel (g, menuPanel, "MENUS", Pal::cyan);

    // Écrans
    if (currentMenu == matrixMenu)
    {
        const auto both = screenAPanel.getUnion (screenBPanel);
        drawPanel (g, both, "MATRICE 12 ROUTES   //   SOURCE  x  QUANTITE  ->  CIBLE   (quantite negative = inverse)", Pal::role (menuAt (currentMenu).role));

        g.setFont (Pal::mono (12.0f * scale, true));
        g.setColour (Pal::dim);
        for (int i = 0; i < 12; ++i)
        {
            const int y = matrixArea.getY() + S (26.0f) + i * S (34.0f) + S (2.0f);
            g.drawText ("->", matrixArea.getX() + S (222.0f), y, S (32.0f), S (28.0f), juce::Justification::centred, false);
            g.setColour (Pal::line);
            g.drawHorizontalLine (matrixArea.getY() + S (26.0f) + (i + 1) * S (34.0f) - S (2.0f),
                                  (float) matrixArea.getX(), (float) matrixArea.getRight());
            g.setColour (Pal::dim);
        }
    }
    else if (currentMenu == mDraw)
    {
        drawPanel (g, screenAPanel.getUnion (screenBPanel),
                   "LFO DESSINE   //   dessine la courbe a la souris (clic droit : remet a zero)   -   source DESSIN dans la matrice",
                   Pal::role (menuAt (currentMenu).role));
    }
    else if (currentMenu == mRack)
    {
        drawPanel (g, screenAPanel.getUnion (screenBPanel),
                   "RACK FX   //   4 effets en serie, juste avant le master   -   < > : echanger avec le voisin",
                   Pal::role (menuAt (currentMenu).role));
        for (int s = 0; s < 4; ++s)
        {
            const auto& r = rack[(size_t) s];
            const auto col = Pal::role ((s + 1) % 6);
            const bool active = (int) pv (("fx" + juce::String (s + 1) + "Type").toRawUTF8()) != 0
                                && pv (("fx" + juce::String (s + 1) + "On").toRawUTF8()) > 0.5f;
            g.setColour (Pal::bg.withAlpha (0.55f));
            g.fillRect (r.area);
            g.setColour (active ? col.withAlpha (0.8f) : Pal::line);
            g.drawRect (r.area, juce::jmax (1, S (1.0f)));
            g.setColour (col);
            g.fillRect (r.area.getX(), r.area.getY(), r.area.getWidth(), S (3.0f));
            g.setFont (Pal::mono (12.0f * scale, true));
            g.drawText ("FX " + juce::String (s + 1), r.area.getX() + S (8.0f), r.area.getY() + S (6.0f), S (80.0f), S (18.0f),
                        juce::Justification::centredLeft, false);
            g.setColour (Pal::dim);
            g.setFont (Pal::mono (9.0f * scale));
            g.drawText (active ? "ACTIF" : "BYPASS", r.area.getRight() - S (88.0f), r.area.getY() + S (6.0f), S (80.0f), S (18.0f),
                        juce::Justification::centredRight, false);
            if (s < 3)
                g.drawText (">", r.area.getRight() - S (2.0f), r.area.getCentreY() - S (8.0f), S (12.0f), S (16.0f),
                            juce::Justification::centred, false);
        }
    }
    else if (currentMenu == mEq)
    {
        drawPanel (g, screenAPanel.getUnion (screenBPanel), "EQ 8 BANDES   //   glisser un point : frequence + gain   -   molette : Q   -   double-clic : on/off",
                   Pal::role (menuAt (currentMenu).role));
        drawEq (g);
    }
    else
    {
        drawScreen (g, screenAPanel, gfxA, true);
        drawScreen (g, screenBPanel, gfxB, false);
    }

    // Colonne de droite
    drawPanel (g, macrosPanel, "MACROS", Pal::violet, "sources de la matrice");
    drawPanel (g, outPanel, "SORTIE", Pal::cyan);

    // Vumètre
    {
        const auto m = meterArea.toFloat();
        const int segs = 16;
        const float sh = m.getHeight() / (float) segs;
        const int lit = (int) std::ceil (outLevel * (float) segs - 0.2f);
        for (int i = 0; i < segs; ++i)
        {
            const float t = (float) i / (float) (segs - 1);
            const auto c = t < 0.75f ? Pal::azure.interpolatedWith (Pal::cyan, t / 0.75f)
                                     : Pal::cyan.interpolatedWith (Pal::violet, (t - 0.75f) / 0.25f);
            g.setColour (c.withAlpha (i < lit ? 1.0f : 0.09f));
            g.fillRect (m.getX(), m.getBottom() - (float) (i + 1) * sh + 1.0f, m.getWidth(), sh - 2.0f);
        }
    }

    // Voyants de voix
    {
        const int mask = proc.getActiveVoiceMask();
        g.setFont (Pal::mono (9.0f * scale));
        g.setColour (Pal::dim);
        g.drawText ("VOIX", ledsArea.getX(), ledsArea.getY() - S (14.0f), ledsArea.getWidth(), S (12.0f),
                    juce::Justification::centred, false);

        const float cell = (float) ledsArea.getWidth() * 0.5f;
        const float d = 11.0f * scale;
        for (int i = 0; i < 8; ++i)
        {
            const float x = (float) ledsArea.getX() + (float) (i % 2) * cell + (cell - d) * 0.5f;
            const float y = (float) ledsArea.getY() + (float) (i / 2) * (float) ledsArea.getHeight() * 0.25f + 4.0f * scale;
            const bool on = (mask & (1 << i)) != 0;
            if (on)
            {
                g.setColour (Pal::cyan.withAlpha (0.28f));
                g.fillRect (juce::Rectangle<float> (x, y, d, d).expanded (2.5f * scale));
            }
            g.setColour (Pal::cyan.withAlpha (on ? 1.0f : 0.12f));
            g.fillRect (x, y, d, d);
        }
    }

    // Séquenceur
    drawPanel (g, seqPanel, "SEQUENCEUR", Pal::teal);
    {
        g.setFont (Pal::mono (10.0f * scale, true));
        g.setColour (Pal::cyan);
        if (statusFrames > 0)
            g.drawText (statusMessage, seqPanel.getX() + S (160.0f), seqPanel.getY() + S (5.0f), S (900.0f), S (18.0f),
                        juce::Justification::centredLeft, false);

        int beat = -1;
        if (proc.isHostPlaying())
            beat = (((int) std::floor (proc.getHostPpq())) % 4 + 4) % 4;
        else if (proc.currentStep.load() >= 0)
            beat = (proc.currentStep.load() / 4) % 4;

        for (int k = 0; k < 4; ++k)
        {
            const float d = 11.0f * scale;
            const juce::Rectangle<float> led ((float) beatLedArea.getX() + (float) k * 22.0f * scale,
                                              (float) beatLedArea.getY() + 2.0f * scale, d, d);
            if (k == beat)
            {
                g.setColour (Pal::teal.withAlpha (0.3f));
                g.fillRect (led.expanded (2.5f * scale));
            }
            g.setColour (Pal::teal.withAlpha (k == beat ? 1.0f : 0.14f));
            g.fillRect (led);
        }
    }
}

void MonSyntheEditor::drawScreen (juce::Graphics& g, juce::Rectangle<int> panelArea, juce::Rectangle<int> gfxArea, bool isA)
{
    const auto& def = menuAt (currentMenu);
    const auto colour = Pal::role ((def.role + (isA ? 0 : 1)) % 6);

    drawPanel (g, panelArea, juce::String (def.name) + (isA ? "   // A" : "   // B"), colour);

    const auto gr = gfxArea.toFloat();
    g.setColour (Pal::bg.darker (0.35f));
    g.fillRect (gr);

    g.setColour (Pal::grid.withAlpha (0.38f));
    const float step = 20.0f * scale;
    for (float x = gr.getX() + step; x < gr.getRight(); x += step)
        g.drawVerticalLine ((int) x, gr.getY(), gr.getBottom());
    for (float y = gr.getY() + step; y < gr.getBottom(); y += step)
        g.drawHorizontalLine ((int) y, gr.getX(), gr.getRight());

    g.setColour (colour.withAlpha (0.4f));
    g.drawRect (gr, 1.0f);

    drawViz (g, isA ? def.vizA : def.vizB, gr.reduced (6.0f * scale), colour);
}

void MonSyntheEditor::drawViz (juce::Graphics& g, int viz, juce::Rectangle<float> r, juce::Colour colour)
{
    switch (viz)
    {
        case VizScopeA:    drawScopeViz (g, r, colour, false); break;
        case VizScopeB:    drawScopeViz (g, r, colour, true);  break;
        case VizUnison:    drawUnisonViz (g, r, colour);       break;
        case VizFmRing:    drawFmRingViz (g, r, colour);       break;
        case VizFilter:    drawFilterViz (g, r, colour);       break;
        case VizFilter2:   drawFilter2Viz (g, r, colour);      break;
        case VizEnvFilter: drawEnvelopeViz (g, r, colour, "fltAttack", "fltDecay", "fltSustain", "fltRelease"); break;
        case VizEnvAmp:    drawEnvelopeViz (g, r, colour, "attack", "decay", "sustain", "release");            break;
        case VizEnvMod:    drawEnvelopeViz (g, r, colour, "modAttack", "modDecay", "modSustain", "modRelease"); break;
        case VizLfo1:      drawLfoViz (g, r, colour, 0);       break;
        case VizLfo2:      drawLfoViz (g, r, colour, 1);       break;
        case VizLfo3:      drawLfoViz (g, r, colour, 2);       break;
        case VizLfo4:      drawLfoViz (g, r, colour, 3);       break;
        case VizGlide:     drawGlideViz (g, r, colour);        break;
        case VizChorus:    drawChorusViz (g, r, colour);       break;
        case VizDelay:     drawDelayViz (g, r, colour);        break;
        case VizReverb:    drawReverbViz (g, r, colour);       break;
        case VizNoise:     drawNoiseViz (g, r, colour);        break;
        case VizString:    drawStringViz (g, r, colour);       break;
        case VizPitch:     drawPitchViz (g, r, colour);        break;
        case VizDisto:     drawDistoViz (g, r, colour);        break;
        case VizComp:      drawCompViz (g, r, colour);         break;
        default:           break;                               // spectre : composant à part ; matrice : composants
    }
}

//------------------------------------------------------------------------------
void MonSyntheEditor::drawScopeViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour, bool forB)
{
    const int table = juce::jlimit (0, WavetableBank::totalTables - 1, (int) pv (forB ? "tableB" : "tableA"));
    const float pos = pv (forB ? "oscBPos" : "position");
    const int wmode = (int) pv (forB ? "warpModeB" : "warpModeA");
    const float wamt = pv (forB ? "warpAmtB" : "warpAmtA");
    const auto& bank = proc.getBank();

    const auto area = r.reduced (4.0f * scale, 16.0f * scale);
    const int n = juce::jmax (48, (int) (area.getWidth() / 2.0f));

    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.6f));
    g.drawHorizontalLine ((int) area.getCentreY(), area.getX(), area.getRight());

    auto makePath = [&] (float p)
    {
        juce::Path path;
        for (int i = 0; i <= n; ++i)
        {
            const double ph = juce::jlimit (0.0, 0.99999, warpPhase (wmode, wamt, (double) i / (double) n * 0.99999));
            const float v = bank.getSample (table, juce::jlimit (0.0f, 1.0f, p), 0, ph);
            const float x = area.getX() + area.getWidth() * (float) i / (float) n;
            const float y = area.getCentreY() - juce::jlimit (-1.2f, 1.2f, v) * area.getHeight() * 0.45f;
            if (i == 0) path.startNewSubPath (x, y); else path.lineTo (x, y);
        }
        return path;
    };

    g.setColour (colour.withAlpha (0.22f));
    g.strokePath (makePath (pos - 0.12f), juce::PathStrokeType (1.0f));
    g.strokePath (makePath (pos + 0.12f), juce::PathStrokeType (1.0f));
    glowStroke (g, makePath (pos), colour, 2.0f * scale);

    if (forB && pv ("oscBLevel") < 0.01f && pv ("fmAmount") < 0.01f && pv ("ringAmount") < 0.01f)
    {
        g.setColour (Pal::bg.withAlpha (0.55f));
        g.fillRect (r);
        g.setColour (colour);
        g.setFont (Pal::mono (12.5f * scale, true));
        g.drawText ("OSC B SILENCIEUX", r, juce::Justification::centred, false);
        g.setFont (Pal::mono (10.0f * scale));
        g.setColour (Pal::dim);
        g.drawText ("monte NIVEAU (ou FM / RING) pour l'entendre",
                    r.withTrimmedTop (r.getHeight() * 0.58f), juce::Justification::centredTop, false);
    }

    const auto names = WavetableBank::tableNames();
    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText (names[table].toUpperCase() + "   POS " + juce::String (juce::roundToInt (pos * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawUnisonViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const int n = juce::jlimit (1, 7, (int) pv ("unison"));
    const float det = pv ("detune");
    const float spr = pv ("spread");

    const auto area = r.reduced (8.0f * scale, 20.0f * scale);
    const float cx = area.getCentreX();

    for (int i = 0; i < n; ++i)
    {
        const float off = n == 1 ? 0.0f : ((float) i / (float) (n - 1) - 0.5f) * 2.0f;
        const float x = cx + off * (det / 100.0f) * area.getWidth() * 0.45f;
        const float h = area.getHeight() * 0.62f * (1.0f - 0.35f * std::abs (off));
        const float top = area.getCentreY() - h * 0.5f;

        g.setColour (colour.withAlpha (0.25f));
        g.fillRect (x - 3.0f * scale, top, 6.0f * scale, h);
        g.setColour (colour);
        g.fillRect (x - 1.0f * scale, top, 2.0f * scale, h);
        g.fillEllipse (x - 4.0f * scale, top - 4.0f * scale, 8.0f * scale, 8.0f * scale);

        const float pan = (i % 2 == 0 ? -1.0f : 1.0f) * spr * (n == 1 ? 0.0f : 0.25f + 0.75f * std::abs (off));
        g.setColour (Pal::role (4).withAlpha (0.9f));
        juce::Path d;
        const float px = cx + pan * area.getWidth() * 0.45f, py = area.getBottom() - 2.0f * scale, s = 4.0f * scale;
        d.addRectangle (-s, -s, s * 2.0f, s * 2.0f);
        g.fillPath (d, juce::AffineTransform::rotation (juce::MathConstants<float>::pi * 0.25f).translated (px, py));
    }

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("x" + juce::String (n) + "   DESACCORD " + juce::String (juce::roundToInt (det)) + " cents   STEREO "
                    + juce::String (juce::roundToInt (spr * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawFmRingViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float fm = pv ("fmAmount");
    const float ring = pv ("ringAmount");
    const auto area = r.reduced (4.0f * scale, 18.0f * scale);
    const int n = juce::jmax (64, (int) (area.getWidth() / 2.0f));
    const float twoPi = juce::MathConstants<float>::twoPi;

    juce::Path plain, result;
    for (int i = 0; i <= n; ++i)
    {
        const float t = (float) i / (float) n;
        const float x = area.getX() + area.getWidth() * t;
        const float base = std::sin (twoPi * 3.0f * t);
        const float mod = std::sin (twoPi * (3.0f * t + fm * 4.0f * std::sin (twoPi * 5.0f * t)));
        const float rm = mod * (1.0f - ring + ring * std::sin (twoPi * 7.0f * t));

        const float y0 = area.getCentreY() - base * area.getHeight() * 0.42f;
        const float y1 = area.getCentreY() - rm * area.getHeight() * 0.42f;
        if (i == 0) { plain.startNewSubPath (x, y0); result.startNewSubPath (x, y1); }
        else        { plain.lineTo (x, y0);          result.lineTo (x, y1); }
    }

    g.setColour (colour.withAlpha (0.22f));
    g.strokePath (plain, juce::PathStrokeType (1.0f));
    glowStroke (g, result, colour, 2.0f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("FM " + juce::String (juce::roundToInt (fm * 100.0f)) + "%   RING "
                    + juce::String (juce::roundToInt (ring * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawFilterViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float fc = pv ("cutoff");
    const float q = juce::jmax (0.3f, pv ("resonance"));
    const int type = (int) pv ("filterType");
    static const char* typeNames[] = { "LP 12", "LP 24", "HP", "BP", "NOTCH", "COMB" };

    const auto area = r.reduced (4.0f * scale, 18.0f * scale);
    auto fx = [&] (float hz) { return area.getX() + area.getWidth() * std::log (hz / 20.0f) / std::log (1000.0f); };

    g.setColour (Pal::line.brighter (0.4f).withAlpha (0.7f));
    for (float hz : { 100.0f, 1000.0f, 10000.0f })
        g.drawVerticalLine ((int) fx (hz), area.getY(), area.getBottom());

    auto mag = [&] (float f)
    {
        const float x = f / fc, x2 = x * x;
        const float d = std::sqrt ((1.0f - x2) * (1.0f - x2) + x2 / (q * q));
        switch (type)
        {
            case 0:  return 1.0f / d;
            case 1:  return 1.0f / (d * d);
            case 2:  return x2 / d;
            case 4:  return std::abs (1.0f - x2) / d;
            case 5:  return 0.15f + 1.2f * std::abs (std::cos (juce::MathConstants<float>::pi * x)) * (0.4f + 0.12f * q);
            default: return (x / q) / d;
        }
    };

    juce::Path line;
    const int n = 140;
    for (int i = 0; i <= n; ++i)
    {
        const float f = 20.0f * std::pow (1000.0f, (float) i / (float) n);
        const float db = juce::jlimit (-48.0f, 18.0f, juce::Decibels::gainToDecibels (mag (f), -60.0f));
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        const float y = area.getBottom() - area.getHeight() * (db + 48.0f) / 66.0f;
        if (i == 0) line.startNewSubPath (x, y); else line.lineTo (x, y);
    }

    juce::Path fill (line);
    fill.lineTo (area.getRight(), area.getBottom());
    fill.lineTo (area.getX(), area.getBottom());
    fill.closeSubPath();
    g.setColour (colour.withAlpha (0.16f));
    g.fillPath (fill);
    glowStroke (g, line, colour, 2.0f * scale);

    g.setColour (colour.withAlpha (0.6f));
    g.drawVerticalLine ((int) fx (fc), area.getY(), area.getBottom());

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText (juce::String (typeNames[juce::jlimit (0, 3, type)]) + "   CUTOFF " + fmtHz (fc) + " Hz   Q "
                    + juce::String (q, 1),
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawEnvelopeViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour,
                                       const char* a, const char* d, const char* s, const char* rel)
{
    const float ta = pv (a), td = pv (d), sus = pv (s), tr = pv (rel);
    auto w = [] (float t) { return std::sqrt (t) + 0.06f; };
    const float wa = w (ta), wd = w (td), wh = 0.5f, wr = w (tr);
    const float total = wa + wd + wh + wr;

    const auto area = r.withTrimmedBottom (18.0f * scale).reduced (6.0f * scale, 12.0f * scale);
    auto X = [&] (float u) { return area.getX() + area.getWidth() * u / total; };
    auto Y = [&] (float v) { return area.getBottom() - area.getHeight() * v; };

    juce::Path p;
    p.startNewSubPath (X (0.0f), Y (0.0f));
    p.lineTo (X (wa), Y (1.0f));
    p.lineTo (X (wa + wd), Y (sus));
    p.lineTo (X (wa + wd + wh), Y (sus));
    p.lineTo (X (total), Y (0.0f));

    juce::Path fill (p);
    fill.closeSubPath();
    g.setColour (colour.withAlpha (0.16f));
    g.fillPath (fill);

    g.setColour (colour.withAlpha (0.25f));
    for (float u : { wa, wa + wd, wa + wd + wh })
        g.drawVerticalLine ((int) X (u), area.getY(), area.getBottom());

    glowStroke (g, p, colour, 2.0f * scale);

    g.setColour (colour.brighter (0.5f));
    for (auto pt : { juce::Point<float> (X (wa), Y (1.0f)), juce::Point<float> (X (wa + wd), Y (sus)),
                     juce::Point<float> (X (wa + wd + wh), Y (sus)) })
        g.fillEllipse (pt.x - 3.0f * scale, pt.y - 3.0f * scale, 6.0f * scale, 6.0f * scale);

    g.setFont (Pal::mono (10.0f * scale, true));
    g.setColour (Pal::text);
    const float ly = r.getBottom() - 15.0f * scale;
    auto label = [&] (const juce::String& t, float u0, float u1)
    {
        g.drawText (t, juce::Rectangle<float> (X (u0), ly, X (u1) - X (u0), 14.0f * scale),
                    juce::Justification::centred, false);
    };
    label ("A " + fmtTime (ta), 0.0f, wa);
    label ("D " + fmtTime (td), wa, wa + wd);
    label ("S " + juce::String (juce::roundToInt (sus * 100.0f)) + "%", wa + wd, wa + wd + wh);
    label ("R " + fmtTime (tr), wa + wd + wh, total);
}

void MonSyntheEditor::drawLfoViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour, int which)
{
    const juce::String pre = which == 0 ? juce::String ("lfo") : "lfo" + juce::String (which + 1);
    const int shape = (int) pv ((pre + "Shape").toRawUTF8());
    const float rate = pv ((pre + "Rate").toRawUTF8());
    const int sync = (int) pv ((pre + "Sync").toRawUTF8());
    const float depth = which != 0 ? 1.0f : juce::jmax (0.12f, pv ("lfoDepth"));
    const double bpm = proc.getHostBpm() > 1.0 ? proc.getHostBpm() : 120.0;

    const auto area = r.reduced (6.0f * scale, 20.0f * scale);
    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.6f));
    g.drawHorizontalLine ((int) area.getCentreY(), area.getX(), area.getRight());

    auto yAt = [&] (float ph) { return area.getCentreY() - lfoShapeValue (shape, ph) * depth * area.getHeight() * 0.45f; };

    juce::Path p;
    const int n = 200;
    for (int i = 0; i <= n; ++i)
    {
        const float u = (float) i / (float) n * 2.0f;
        const float ph = u - std::floor (u);
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        const float y = yAt (i == n ? 0.9999f : ph);
        if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
    }
    glowStroke (g, p, colour, 2.0f * scale);

    const double hz = sync > 0 ? bpm / 60.0 : (double) rate;
    const float f = (float) std::fmod (nowSeconds() * juce::jmin (hz, 8.0), 1.0);
    g.setColour (colour.brighter (0.6f));
    for (int k = 0; k < 2; ++k)
    {
        const float x = area.getX() + area.getWidth() * (f + (float) k) * 0.5f;
        g.fillEllipse (x - 4.0f * scale, yAt (f) - 4.0f * scale, 8.0f * scale, 8.0f * scale);
    }

    const auto syncNames = MonSyntheProcessor::lfoSyncNames();
    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("LFO " + juce::String (which + 1) + "   "
                    + (sync > 0 ? syncNames[juce::jlimit (0, syncNames.size() - 1, sync)] : juce::String (rate, 2) + " Hz"),
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawGlideViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const bool mono = pv ("mono") > 0.5f;
    const float glide = pv ("glide");
    const int bend = (int) pv ("bendRange");

    const auto area = r.reduced (8.0f * scale, 22.0f * scale);
    const float lowY = area.getBottom() - area.getHeight() * 0.2f;
    const float highY = area.getY() + area.getHeight() * 0.2f;
    const float stepX = area.getX() + area.getWidth() * 0.3f;

    juce::Path target;
    target.startNewSubPath (area.getX(), lowY);
    target.lineTo (stepX, lowY);
    target.lineTo (stepX, highY);
    target.lineTo (area.getRight(), highY);
    g.setColour (colour.withAlpha (0.3f));
    g.strokePath (target, juce::PathStrokeType (1.0f));

    juce::Path actual;
    actual.startNewSubPath (area.getX(), lowY);
    actual.lineTo (stepX, lowY);
    const float tau = juce::jmax (0.004f, glide / 3.0f);
    const int n = 120;
    for (int i = 0; i <= n; ++i)
    {
        const float x = stepX + (area.getRight() - stepX) * (float) i / (float) n;
        const float tSec = 2.0f * (float) i / (float) n;
        const float v = 1.0f - std::exp (-tSec / tau);
        actual.lineTo (x, lowY + (highY - lowY) * v);
    }
    glowStroke (g, actual, colour, 2.0f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText (juce::String (mono ? "MONO / LEGATO" : "POLY") + "   GLIDE " + juce::String (glide, 2) + " s   BEND +/-"
                    + juce::String (bend),
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawChorusViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float mix = pv ("chorusMix"), rate = pv ("chorusRate"), depth = pv ("chorusDepth");
    const auto area = r.reduced (6.0f * scale, 20.0f * scale);
    const float t = (float) nowSeconds();
    const float twoPi = juce::MathConstants<float>::twoPi;

    juce::Path p1, p2;
    const int n = 160;
    for (int i = 0; i <= n; ++i)
    {
        const float u = (float) i / (float) n;
        const float x = area.getX() + area.getWidth() * u;
        const float y1 = area.getCentreY() - std::sin (twoPi * (u * 2.0f)) * area.getHeight() * 0.4f;
        const float y2 = area.getCentreY() - std::sin (twoPi * (u * 2.0f + depth * 0.3f * std::sin (t * rate * twoPi)))
                                                  * area.getHeight() * 0.4f;
        if (i == 0) { p1.startNewSubPath (x, y1); p2.startNewSubPath (x, y2); }
        else        { p1.lineTo (x, y1);          p2.lineTo (x, y2); }
    }

    g.setColour (colour.withAlpha (0.35f));
    g.strokePath (p1, juce::PathStrokeType (1.0f));
    g.setColour (colour.withAlpha (0.25f + 0.75f * mix));
    g.strokePath (p2, juce::PathStrokeType (2.0f * scale));

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("CHORUS   MIX " + juce::String (juce::roundToInt (mix * 100.0f)) + "%   " + juce::String (rate, 1) + " Hz",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawDelayViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float time = pv ("delayTime"), fb = pv ("delayFeedback"), mix = pv ("delayMix");
    const auto area = r.reduced (8.0f * scale, 22.0f * scale);

    for (int k = 0; k < 40; ++k)
    {
        const float ms = (float) k * time;
        if (ms > 4000.0f)
            break;

        const float x = area.getX() + area.getWidth() * ms / 4000.0f;
        const float amp = k == 0 ? 1.0f : std::pow (fb, (float) (k - 1)) * (0.2f + 0.8f * mix);
        const float h = area.getHeight() * juce::jlimit (0.03f, 1.0f, amp);

        const auto c = k == 0 ? Pal::text : colour;
        g.setColour (c.withAlpha (0.25f));
        g.fillRect (x - 3.0f * scale, area.getBottom() - h, 6.0f * scale, h);
        g.setColour (c);
        g.fillRect (x - 1.0f * scale, area.getBottom() - h, 2.0f * scale, h);
    }

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("DELAY   " + juce::String (juce::roundToInt (time)) + " ms   FB " + juce::String (juce::roundToInt (fb * 100.0f))
                    + "%   MIX " + juce::String (juce::roundToInt (mix * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawReverbViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float size = pv ("reverbSize"), damp = pv ("reverbDamp"), mix = pv ("reverbMix");
    const auto area = r.reduced (8.0f * scale, 22.0f * scale);
    const int n = 72;
    const float decayLen = 0.25f + 0.75f * size;

    for (int i = 0; i < n; ++i)
    {
        const float u = (float) i / (float) n;
        const float env = std::exp (-3.5f * u / decayLen);
        const float s = std::sin ((float) i * 12.9898f) * 43758.5453f;
        const float hash = s - std::floor (s);
        const float h = area.getHeight() * env * (0.25f + 0.75f * hash);
        const float x = area.getX() + area.getWidth() * u;
        const float fade = 1.0f - damp * 0.6f * u;

        g.setColour (colour.withAlpha ((0.25f + 0.75f * mix) * juce::jlimit (0.1f, 1.0f, fade)));
        g.fillRect (x, area.getBottom() - h, juce::jmax (1.5f, area.getWidth() / (float) n * 0.6f), h);
    }

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("REVERB   TAILLE " + juce::String (juce::roundToInt (size * 100.0f)) + "%   AMORTI "
                    + juce::String (juce::roundToInt (damp * 100.0f)) + "%   MIX " + juce::String (juce::roundToInt (mix * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

//------------------------------------------------------------------------------
void MonSyntheEditor::drawNoiseViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float level = pv ("noiseLevel"), tone = pv ("noiseTone"), decay = pv ("noiseDecay");
    const int type = (int) pv ("noiseType");
    static const char* names[] = { "BLANC", "ROSE", "SALE" };

    const auto area = r.reduced (6.0f * scale, 20.0f * scale);
    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.6f));
    g.drawHorizontalLine ((int) area.getCentreY(), area.getX(), area.getRight());

    // enveloppe de decay (fenêtre de 2 secondes)
    juce::Path env;
    const int n = 160;
    for (int i = 0; i <= n; ++i)
    {
        const float t = 2.0f * (float) i / (float) n;
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        const float y = area.getCentreY() - std::exp (-t / juce::jmax (0.005f, decay)) * level * area.getHeight() * 0.48f;
        if (i == 0) env.startNewSubPath (x, y); else env.lineTo (x, y);
    }
    g.setColour (colour.withAlpha (0.5f));
    g.strokePath (env, juce::PathStrokeType (1.2f * scale));

    // bruit animé, lissé selon le ton
    juce::Random rnd ((juce::int64) (nowSeconds() * 30.0));
    const float k = 0.02f + 0.98f * tone * tone;
    const float smooth = type == 1 ? 0.35f : 1.0f;
    float lp = 0.0f, held = 0.0f;
    int hold = 0;
    juce::Path noise;
    const int m = juce::jmax (64, (int) area.getWidth() / 2);
    for (int i = 0; i <= m; ++i)
    {
        float w = rnd.nextFloat() * 2.0f - 1.0f;
        if (type == 2) { if (hold <= 0) { held = w; hold = 1 + rnd.nextInt (10); } --hold; w = held; }
        lp += (w - lp) * k * smooth;
        const float t = 2.0f * (float) i / (float) m;
        const float amp = juce::jmax (0.04f, level) * std::exp (-t / juce::jmax (0.005f, decay));
        const float x = area.getX() + area.getWidth() * (float) i / (float) m;
        const float y = area.getCentreY() - lp * amp * area.getHeight() * 0.48f;
        if (i == 0) noise.startNewSubPath (x, y); else noise.lineTo (x, y);
    }
    glowStroke (g, noise, colour, 1.3f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("BRUIT " + juce::String (names[juce::jlimit (0, 2, type)]) + "   NIVEAU " + juce::String (juce::roundToInt (level * 100.0f))
                    + "%   DECAY " + (decay > 4.9f ? juce::String ("CONTINU") : fmtTime (decay)),
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawPitchViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float amt = pv ("pitchEnvAmt"), decay = pv ("pitchEnvDecay");
    const int mode = (int) pv ("phaseMode");
    static const char* modes[] = { "PHASE ALEATOIRE", "PHASE FIXE", "PHASE LIBRE" };

    const auto area = r.reduced (8.0f * scale, 22.0f * scale);
    const float zeroY = area.getCentreY();
    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.7f));
    g.drawHorizontalLine ((int) zeroY, area.getX(), area.getRight());

    const float window = juce::jmax (0.3f, decay * 6.0f);
    juce::Path p;
    const int n = 160;
    for (int i = 0; i <= n; ++i)
    {
        const float t = window * (float) i / (float) n;
        const float v = amt * std::exp (-t / decay);
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        const float y = zeroY - juce::jlimit (-60.0f, 60.0f, v) / 60.0f * area.getHeight() * 0.5f;
        if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
    }
    juce::Path fill (p);
    fill.lineTo (area.getRight(), zeroY);
    fill.lineTo (area.getX(), zeroY);
    fill.closeSubPath();
    g.setColour (colour.withAlpha (0.16f));
    g.fillPath (fill);
    glowStroke (g, p, colour, 2.0f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("PITCH " + juce::String (amt >= 0.0f ? "+" : "") + juce::String (juce::roundToInt (amt)) + " st   DECAY "
                    + fmtTime (decay) + "   " + modes[juce::jlimit (0, 2, mode)],
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawDistoViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const int type = (int) pv ("distType");
    const float amt = pv ("distAmt"), mix = pv ("distMix");
    static const char* names[] = { "SOFT", "HARD", "FOLD", "CRUSH", "ASYM" };

    const auto area = r.reduced (10.0f * scale, 22.0f * scale);
    const float cx = area.getCentreX(), cy = area.getCentreY();
    const float hw = area.getWidth() * 0.5f, hh = area.getHeight() * 0.5f;

    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.7f));
    g.drawHorizontalLine ((int) cy, area.getX(), area.getRight());
    g.drawVerticalLine ((int) cx, area.getY(), area.getBottom());

    juce::Path diag;
    diag.startNewSubPath (cx - hw, cy + hh);
    diag.lineTo (cx + hw, cy - hh);
    g.setColour (colour.withAlpha (0.25f));
    g.strokePath (diag, juce::PathStrokeType (1.0f));

    juce::Path curve;
    const int n = 240;
    for (int i = 0; i <= n; ++i)
    {
        const float x = -1.0f + 2.0f * (float) i / (float) n;
        const float wet = amt > 0.001f ? distShape (type, amt, x) : x;
        const float y = x + mix * (wet - x);
        const float px = cx + x * hw;
        const float py = cy - juce::jlimit (-1.2f, 1.2f, y) * hh;
        if (i == 0) curve.startNewSubPath (px, py); else curve.lineTo (px, py);
    }
    glowStroke (g, curve, colour, 2.0f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("DISTO " + juce::String (names[juce::jlimit (0, 4, type)]) + "   QTE " + juce::String (juce::roundToInt (amt * 100.0f))
                    + "%   MIX " + juce::String (juce::roundToInt (mix * 100.0f)) + "%",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawCompViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float thresh = pv ("compThresh"), ratio = pv ("compRatio"), makeup = pv ("compMakeup"), mix = pv ("compMix");
    const float ceil = pv ("limCeil");
    const bool on = thresh < -0.1f;

    const auto area = r.reduced (10.0f * scale, 22.0f * scale);
    // courbe entrée -> sortie, de -48 dB à 0 dB
    auto X = [&] (float db) { return area.getX() + area.getWidth() * (db + 48.0f) / 48.0f; };
    auto Y = [&] (float db) { return area.getBottom() - area.getHeight() * (juce::jlimit (-48.0f, 6.0f, db) + 48.0f) / 54.0f; };

    g.setColour (Pal::line.brighter (0.4f).withAlpha (0.7f));
    for (float db : { -36.0f, -24.0f, -12.0f, 0.0f })
    {
        g.drawVerticalLine ((int) X (db), area.getY(), area.getBottom());
        g.drawHorizontalLine ((int) Y (db), area.getX(), area.getRight());
    }

    juce::Path diag;
    diag.startNewSubPath (X (-48.0f), Y (-48.0f));
    diag.lineTo (X (0.0f), Y (0.0f));
    g.setColour (colour.withAlpha (0.25f));
    g.strokePath (diag, juce::PathStrokeType (1.0f));

    juce::Path curve;
    for (int i = 0; i <= 96; ++i)
    {
        const float in = -48.0f + 48.0f * (float) i / 96.0f;
        float out = in;
        if (on && in > thresh)
            out = thresh + (in - thresh) / juce::jmax (1.0f, ratio);
        if (on)
            out = in + mix * (out + makeup - in);
        out = juce::jmin (out, ceil);                         // le limiteur coupe au plafond
        if (i == 0) curve.startNewSubPath (X (in), Y (out)); else curve.lineTo (X (in), Y (out));
    }
    glowStroke (g, curve, colour, 2.0f * scale);

    if (on)
    {
        g.setColour (colour.withAlpha (0.6f));
        g.drawVerticalLine ((int) X (thresh), area.getY(), area.getBottom());
    }

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText (on ? "COMP " + juce::String (juce::roundToInt (thresh)) + " dB   " + juce::String (ratio, 1) + ":1   LIM "
                         + juce::String (ceil, 1) + " dB"
                   : juce::String ("COMP OFF (seuil a 0)   LIM ") + juce::String (ceil, 1) + " dB",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

//==============================================================================
// EQ 8 bandes
//==============================================================================
float MonSyntheEditor::paramPlain (const juce::String& id) const
{
    return proc.apvts.getRawParameterValue (id)->load();
}

void MonSyntheEditor::setParamPlain (const juce::String& id, float value)
{
    if (auto* p = proc.apvts.getParameter (id))
        p->setValueNotifyingHost (p->convertTo0to1 (value));
}

void MonSyntheEditor::selectEqBand (int band)
{
    eqBand = juce::jlimit (0, 7, band);
    const juce::String n = "eq" + juce::String (eqBand + 1);

    for (size_t i = 0; i < 4; ++i)
    {
        fixedKnobs[i]->attachment.reset();
        fixedKnobs[i]->paramId = {};
    }

    const char* labels[] = { "FREQ", "GAIN", "Q", "SORTIE" };
    const juce::String ids[] = { n + "Freq", n + "Gain", n + "Q", "eqOut" };
    for (size_t i = 0; i < 4; ++i)
    {
        fixedKnobs[i]->label.setText (labels[i], juce::dontSendNotification);
        fixedKnobs[i]->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, ids[i], fixedKnobs[i]->slider);
    }

    eqOnAtt.reset();
    eqTypeAtt.reset();
    eqOnAtt = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, n + "On", eqOnButton);
    eqTypeAtt = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, n + "Type", eqTypeBox);

    for (int i = 0; i < 8; ++i)
        eqBandButtons[(size_t) i].setToggleState (i == eqBand, juce::dontSendNotification);

    repaint();
}

juce::Point<float> MonSyntheEditor::eqNodePos (int band) const
{
    const juce::String n = "eq" + juce::String (band + 1);
    const auto area = eqGraphArea.toFloat().reduced (6.0f * scale);
    const float freq = paramPlain (n + "Freq");
    const int type = (int) paramPlain (n + "Type");
    const float gain = (type == 0 || type == 4) ? 0.0f : paramPlain (n + "Gain");

    const float x = area.getX() + area.getWidth() * std::log (freq / 20.0f) / std::log (1000.0f);
    const float y = area.getCentreY() - gain / 15.0f * area.getHeight() * 0.5f;
    return { x, y };
}

int MonSyntheEditor::eqHitTest (juce::Point<int> p) const
{
    int best = -1;
    float bestDist = 18.0f * scale;
    for (int b = 0; b < 8; ++b)
    {
        const float d = eqNodePos (b).getDistanceFrom (p.toFloat());
        if (d < bestDist)
        {
            bestDist = d;
            best = b;
        }
    }
    return best;
}

void MonSyntheEditor::mouseDown (const juce::MouseEvent& e)
{
    for (auto& k : knobs)
        if (e.eventComponent == &k->slider)
        {
            lastKnob = k.get();
            if (e.mods.isPopupMenu() && k->paramId.isNotEmpty())
            {
                showKnobMenu (*k);
                return;
            }
        }

    eqDragBand = -1;
    if (currentMenu != mEq || ! eqGraphArea.contains (e.getPosition()))
        return;

    const int hit = eqHitTest (e.getPosition());
    if (hit < 0)
        return;

    eqDragBand = hit;
    selectEqBand (hit);
    const juce::String n = "eq" + juce::String (hit + 1);
    if (paramPlain (n + "On") < 0.5f)
        setParamPlain (n + "On", 1.0f);          // un point attrapé s'active tout seul

    for (const char* s : { "Freq", "Gain", "Q" })
        if (auto* p = proc.apvts.getParameter (n + s))
            p->beginChangeGesture();
}

void MonSyntheEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (eqDragBand < 0)
        return;

    const juce::String n = "eq" + juce::String (eqDragBand + 1);
    const auto area = eqGraphArea.toFloat().reduced (6.0f * scale);
    const float fx = juce::jlimit (0.0f, 1.0f, ((float) e.x - area.getX()) / area.getWidth());
    setParamPlain (n + "Freq", 20.0f * std::pow (1000.0f, fx));

    const int type = (int) paramPlain (n + "Type");
    const float ny = ((float) e.y - area.getCentreY()) / (area.getHeight() * 0.5f);      // -1 (haut) .. +1 (bas)
    if (type == 0 || type == 4)
        setParamPlain (n + "Q", juce::jlimit (0.1f, 10.0f, 0.71f * std::pow (10.0f, -ny)));   // coupe-bas/haut : la hauteur règle le Q
    else
        setParamPlain (n + "Gain", juce::jlimit (-15.0f, 15.0f, -ny * 15.0f));
}

void MonSyntheEditor::mouseUp (const juce::MouseEvent&)
{
    if (eqDragBand >= 0)
    {
        const juce::String n = "eq" + juce::String (eqDragBand + 1);
        for (const char* s : { "Freq", "Gain", "Q" })
            if (auto* p = proc.apvts.getParameter (n + s))
                p->endChangeGesture();
    }
    eqDragBand = -1;
}

void MonSyntheEditor::mouseDoubleClick (const juce::MouseEvent& e)
{
    if (currentMenu != mEq || ! eqGraphArea.contains (e.getPosition()))
        return;

    const int hit = eqHitTest (e.getPosition());
    if (hit >= 0)
    {
        const juce::String id = "eq" + juce::String (hit + 1) + "On";
        setParamPlain (id, paramPlain (id) > 0.5f ? 0.0f : 1.0f);
    }
}

void MonSyntheEditor::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (currentMenu != mEq || ! eqGraphArea.contains (e.getPosition()))
        return;

    int hit = eqHitTest (e.getPosition());
    if (hit < 0)
        hit = eqBand;

    const juce::String id = "eq" + juce::String (hit + 1) + "Q";
    setParamPlain (id, juce::jlimit (0.1f, 10.0f, paramPlain (id) * std::exp (wheel.deltaY * 1.2f)));
    selectEqBand (hit);
}

void MonSyntheEditor::drawEq (juce::Graphics& g)
{
    const auto gr = eqGraphArea.toFloat();
    const auto area = gr.reduced (6.0f * scale);
    const double sr = proc.getSampleRate() > 0.0 ? proc.getSampleRate() : 44100.0;

    g.setColour (Pal::bg.darker (0.35f));
    g.fillRect (gr);

    auto fx = [&] (float hz) { return area.getX() + area.getWidth() * std::log (hz / 20.0f) / std::log (1000.0f); };
    auto gy = [&] (float db) { return area.getCentreY() - db / 15.0f * area.getHeight() * 0.5f; };

    // quadrillage : fréquences (20 Hz -> 20 kHz) et dB
    g.setFont (Pal::mono (9.0f * scale));
    for (float hz : { 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f,
                      800.0f, 900.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f, 20000.0f })
    {
        const bool major = hz == 100.0f || hz == 1000.0f || hz == 10000.0f;
        g.setColour (Pal::grid.withAlpha (major ? 0.75f : 0.35f));
        g.drawVerticalLine ((int) fx (hz), area.getY(), area.getBottom());
        if (major)
        {
            g.setColour (Pal::dim);
            g.drawText (hz >= 1000.0f ? juce::String ((int) (hz / 1000.0f)) + "k" : juce::String ((int) hz),
                        (int) fx (hz) + 2, (int) area.getBottom() - (int) (13.0f * scale), (int) (30.0f * scale), (int) (12.0f * scale),
                        juce::Justification::centredLeft, false);
        }
    }
    for (float db : { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f })
    {
        g.setColour (Pal::grid.withAlpha (db == 0.0f ? 0.9f : 0.45f));
        g.drawHorizontalLine ((int) gy (db), area.getX(), area.getRight());
        g.setColour (Pal::dim);
        g.drawText ((db > 0.0f ? "+" : "") + juce::String ((int) db), (int) area.getX() + 2, (int) gy (db) - (int) (12.0f * scale),
                    (int) (28.0f * scale), (int) (12.0f * scale), juce::Justification::centredLeft, false);
    }

    g.setColour (Pal::cyan.withAlpha (0.4f));
    g.drawRect (gr, 1.0f);

    // courbe de chaque bande active (discrète) et courbe totale
    std::array<juce::dsp::IIR::Coefficients<float>::Ptr, 8> coeffs;
    std::array<bool, 8> on {};
    for (int b = 0; b < 8; ++b)
    {
        const juce::String n = "eq" + juce::String (b + 1);
        on[(size_t) b] = paramPlain (n + "On") > 0.5f;
        coeffs[(size_t) b] = makeEqCoefficients ((int) paramPlain (n + "Type"), sr, paramPlain (n + "Freq"),
                                                 paramPlain (n + "Gain"), paramPlain (n + "Q"));
    }

    const int n = 200;
    juce::Path total;
    for (int b = 0; b < 8; ++b)
    {
        if (! on[(size_t) b])
            continue;
        juce::Path one;
        for (int i = 0; i <= n; ++i)
        {
            const float f = 20.0f * std::pow (1000.0f, (float) i / (float) n);
            const float db = juce::jlimit (-24.0f, 24.0f, juce::Decibels::gainToDecibels ((float) coeffs[(size_t) b]->getMagnitudeForFrequency (f, sr), -60.0f));
            const float x = area.getX() + area.getWidth() * (float) i / (float) n;
            if (i == 0) one.startNewSubPath (x, gy (db)); else one.lineTo (x, gy (db));
        }
        g.setColour (Pal::role (b % 6).withAlpha (0.35f));
        g.strokePath (one, juce::PathStrokeType (1.0f));
    }

    for (int i = 0; i <= n; ++i)
    {
        const float f = 20.0f * std::pow (1000.0f, (float) i / (float) n);
        float mag = 1.0f;
        for (int b = 0; b < 8; ++b)
            if (on[(size_t) b])
                mag *= (float) coeffs[(size_t) b]->getMagnitudeForFrequency (f, sr);
        const float db = juce::jlimit (-24.0f, 24.0f, juce::Decibels::gainToDecibels (mag, -60.0f));
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        if (i == 0) total.startNewSubPath (x, gy (db)); else total.lineTo (x, gy (db));
    }

    juce::Path fill (total);
    fill.lineTo (area.getRight(), gy (0.0f));
    fill.lineTo (area.getX(), gy (0.0f));
    fill.closeSubPath();
    g.setColour (Pal::cyan.withAlpha (0.12f));
    g.fillPath (fill);
    glowStroke (g, total, Pal::cyan, 2.0f * scale);

    // points de contrôle
    for (int b = 0; b < 8; ++b)
    {
        const auto p = eqNodePos (b);
        const auto c = Pal::role (b % 6);
        const float rad = 9.0f * scale;
        if (b == eqBand)
        {
            g.setColour (c.withAlpha (0.3f));
            g.fillEllipse (p.x - rad * 1.5f, p.y - rad * 1.5f, rad * 3.0f, rad * 3.0f);
        }
        g.setColour (on[(size_t) b] ? c : c.withAlpha (0.25f));
        g.fillEllipse (p.x - rad, p.y - rad, rad * 2.0f, rad * 2.0f);
        g.setColour (on[(size_t) b] ? Pal::bg : Pal::dim);
        g.setFont (Pal::mono (10.0f * scale, true));
        g.drawText (juce::String (b + 1), juce::Rectangle<float> (p.x - rad, p.y - rad, rad * 2.0f, rad * 2.0f),
                    juce::Justification::centred, false);
    }

    // Aide pour la bande choisie
    const juce::String nm = "eq" + juce::String (eqBand + 1);
    const auto typeNames = juce::StringArray { "COUPE-BAS", "SHELF GRAVE", "CLOCHE", "SHELF AIGU", "COUPE-HAUT" };
    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText ("BANDE " + juce::String (eqBand + 1) + "   " + typeNames[juce::jlimit (0, 4, (int) paramPlain (nm + "Type"))] + "   "
                    + fmtHz (paramPlain (nm + "Freq")) + " Hz   " + juce::String (paramPlain (nm + "Gain"), 1) + " dB   Q "
                    + juce::String (paramPlain (nm + "Q"), 2),
                juce::Rectangle<float> (gr.getX() + 8.0f * scale, gr.getY() + 3.0f * scale, gr.getWidth() - 16.0f * scale, 14.0f * scale),
                juce::Justification::centredLeft, false);
}

//==============================================================================
// Modulation façon Serum : clic droit sur un potard -> choisir la source
//==============================================================================
int MonSyntheEditor::modTargetForParam (const juce::String& id)
{
    static const std::pair<const char*, int> map[] = {
        { "position", 1 }, { "oscBPos", 2 }, { "cutoff", 3 }, { "resonance", 4 }, { "f2Cutoff", 21 }, { "f2Vowel", 22 }, { "detune", 6 },
        { "oscBLevel", 7 }, { "drive", 8 }, { "modAmount", 11 }, { "warpAmtA", 14 }, { "warpAmtB", 15 },
        { "fmAmount", 16 }, { "ringAmount", 17 }, { "distAmt", 18 }, { "noiseLevel", 19 }, { "fltEnvAmount", 10 }
    };
    for (const auto& m : map)
        if (id == m.first)
            return m.second;
    return 0;
}

void MonSyntheEditor::showModMenu (Knob& knob)
{
    const int target = modTargetForParam (knob.paramId);
    juce::PopupMenu menu;

    if (target == 0)
    {
        menu.addItem (1, "Ce reglage n'est pas modulable (utilise la MATRICE)", false, false);
        menu.showMenuAsync (juce::PopupMenu::Options());
        return;
    }

    const auto sources = MonSyntheProcessor::modSourceNames();
    const auto targets = MonSyntheProcessor::modTargetNames();
    menu.addSectionHeader ("MODULER  " + targets[target]);

    // routes déjà présentes vers cette cible
    std::array<int, 17> slotOf {};
    slotOf.fill (-1);
    for (int s = 1; s <= SynthVoice::numMixSlots; ++s)
    {
        const juce::String n = "mx" + juce::String (s);
        const int src = (int) paramPlain (n + "Src");
        if (src > 0 && src < 17 && (int) paramPlain (n + "Dst") == target && std::abs (paramPlain (n + "Amt")) > 0.001f)
            slotOf[(size_t) src] = s;
    }

    for (int src = 1; src < sources.size(); ++src)
        menu.addItem (100 + src, sources[src] + (slotOf[(size_t) src] > 0 ? "   (retirer)" : ""), true, slotOf[(size_t) src] > 0);

    menu.addSeparator();
    menu.addItem (1, "Retirer toutes les modulations de ce reglage");

    juce::Component::SafePointer<MonSyntheEditor> safe (this);
    menu.showMenuAsync (juce::PopupMenu::Options(), [safe, target, slotOf] (int result)
    {
        if (safe == nullptr || result == 0)
            return;

        auto* ed = safe.getComponent();
        auto clear = [ed] (int s)
        {
            const juce::String n = "mx" + juce::String (s);
            ed->setParamPlain (n + "Amt", 0.0f);
            ed->setParamPlain (n + "Src", 0.0f);
        };

        if (result == 1)
        {
            for (int src = 1; src < 17; ++src)
                if (slotOf[(size_t) src] > 0)
                    clear (slotOf[(size_t) src]);
        }
        else
        {
            const int src = result - 100;
            if (slotOf[(size_t) src] > 0)
                clear (slotOf[(size_t) src]);
            else
                for (int s = 1; s <= SynthVoice::numMixSlots; ++s)           // première route libre
                {
                    const juce::String n = "mx" + juce::String (s);
                    if ((int) ed->paramPlain (n + "Src") == 0 || std::abs (ed->paramPlain (n + "Amt")) < 0.001f)
                    {
                        ed->setParamPlain (n + "Src", (float) src);
                        ed->setParamPlain (n + "Dst", (float) target);
                        ed->setParamPlain (n + "Amt", 0.5f);
                        break;
                    }
                }
        }
        ed->repaint();
    });
}

// Anneaux de modulation : la zone colorée autour du potard montre l'amplitude de chaque route
void MonSyntheEditor::paintOverChildren (juce::Graphics& g)
{
    if (currentMenu == matrixMenu || currentMenu == mEq || currentMenu == mRack)
        return;

    for (auto* k : fixedKnobs)
    {
        if (! k->slider.isVisible() || k->paramId.isEmpty())
            continue;

        const int target = modTargetForParam (k->paramId);
        if (target == 0)
            continue;

        float total = 0.0f;
        for (int s = 1; s <= SynthVoice::numMixSlots; ++s)
        {
            const juce::String n = "mx" + juce::String (s);
            if ((int) paramPlain (n + "Src") > 0 && (int) paramPlain (n + "Dst") == target)
                total += paramPlain (n + "Amt");
        }

        if (std::abs (total) < 0.001f)
            continue;

        auto& sl = k->slider;
        auto sb = sl.getLookAndFeel().getSliderLayout (sl).sliderBounds.toFloat() + sl.getPosition().toFloat();
        const float rad = juce::jmin (sb.getWidth(), sb.getHeight()) * 0.5f + 2.0f * scale;
        const float a0 = juce::MathConstants<float>::pi * 1.2f, a1 = juce::MathConstants<float>::pi * 2.8f;
        const float pos = (float) sl.valueToProportionOfLength (sl.getValue());
        const float from = a0 + (a1 - a0) * pos;
        const float to = a0 + (a1 - a0) * juce::jlimit (0.0f, 1.0f, pos + total * 0.5f);

        juce::Path arc;
        arc.addCentredArc (sb.getCentreX(), sb.getCentreY(), rad, rad, 0.0f, juce::jmin (from, to), juce::jmax (from, to), true);
        g.setColour (Pal::role ((k->role + 2) % 6).withAlpha (0.9f));
        g.strokePath (arc, juce::PathStrokeType (3.0f * scale));
    }
}

void MonSyntheEditor::applyKeyboardColours()
{
    if (keyboard == nullptr)
        return;

    keyboard->setColour (juce::MidiKeyboardComponent::whiteNoteColourId, Pal::text.withMultipliedBrightness (0.85f));
    keyboard->setColour (juce::MidiKeyboardComponent::blackNoteColourId, Pal::bg.darker (0.4f));
    keyboard->setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId, Pal::line);
    keyboard->setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Pal::cyan.withAlpha (0.25f));
    keyboard->setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId, Pal::cyan.withAlpha (0.85f));
    keyboard->setColour (juce::MidiKeyboardComponent::textLabelColourId, Pal::bg);
    keyboard->setColour (juce::MidiKeyboardComponent::upDownButtonBackgroundColourId, Pal::panel);
    keyboard->setColour (juce::MidiKeyboardComponent::upDownButtonArrowColourId, Pal::dim);
    keyboard->setColour (juce::MidiKeyboardComponent::shadowColourId, Pal::bg);
}

//==============================================================================
// Navigateur de presets
//==============================================================================
PresetBrowser::PresetBrowser (MonSyntheProcessor& p, std::function<void (int)> onPickPreset, std::function<void()> onClose)
    : proc (p), onPick (std::move (onPickPreset))
{
    search.setMultiLine (false);
    search.setTextToShowWhenEmpty ("rechercher un preset...", Pal::dim);
    search.onTextChange = [this] { applyFilter(); };
    addAndMakeVisible (search);

    closeButton.onClick = std::move (onClose);
    addAndMakeVisible (closeButton);

    list.setRowHeight (28);
    list.setColour (juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (list);

    loadFavourites();
    rebuild();
}

juce::File PresetBrowser::favouritesFile() const
{
    return MonSyntheProcessor::getPresetDirectory().getParentDirectory().getChildFile ("favourites.txt");
}

void PresetBrowser::loadFavourites()
{
    favs.clear();
    const auto f = favouritesFile();
    if (f.existsAsFile())
        favs.addLines (f.loadFileAsString());
    favs.removeEmptyStrings();
}

void PresetBrowser::saveFavourites() const
{
    const auto f = favouritesFile();
    f.getParentDirectory().createDirectory();
    f.replaceWithText (favs.joinIntoString ("\n"));
}

void PresetBrowser::rebuild()
{
    all.clear();
    cats.clear();
    cats.add ("TOUS");
    cats.add ("FAVORIS");

    const int nf = MonSyntheProcessor::getNumFactoryPresets();
    for (int i = 0; i < nf; ++i)
    {
        const auto cat = MonSyntheProcessor::getFactoryPresetCategory (i);
        all.push_back ({ MonSyntheProcessor::getFactoryPresetName (i), cat, 1000 + i });
        cats.addIfNotAlreadyThere (cat);
    }

    const auto users = proc.getPresetNames();
    for (int j = 0; j < users.size(); ++j)
        all.push_back ({ users[j], "UTILISATEUR", 2000 + j });
    if (users.size() > 0)
        cats.add ("UTILISATEUR");

    catButtons.clear();
    for (const auto& c : cats)
    {
        auto b = std::make_unique<juce::TextButton> (c);
        b->setClickingTogglesState (false);
        b->setColour (juce::TextButton::buttonOnColourId, Pal::cyan);
        b->setColour (juce::TextButton::textColourOffId, Pal::dim);
        b->setColour (juce::TextButton::textColourOnId, Pal::text);
        b->onClick = [this, c]
        {
            catFilter = c;
            for (auto& other : catButtons)
                other->setToggleState (other->getButtonText() == catFilter, juce::dontSendNotification);
            applyFilter();
        };
        addAndMakeVisible (*b);
        catButtons.push_back (std::move (b));
    }

    if (! cats.contains (catFilter))
        catFilter = "TOUS";
    for (auto& b : catButtons)
        b->setToggleState (b->getButtonText() == catFilter, juce::dontSendNotification);

    resized();
    applyFilter();
}

void PresetBrowser::applyFilter()
{
    const auto needle = search.getText().trim().toLowerCase();
    shown.clear();

    for (const auto& e : all)
    {
        const bool catOk = catFilter == "TOUS" || (catFilter == "FAVORIS" ? favs.contains (e.name) : e.category == catFilter);
        const bool textOk = needle.isEmpty() || e.name.toLowerCase().contains (needle) || e.category.toLowerCase().contains (needle);
        if (catOk && textOk)
            shown.push_back (e);
    }

    list.updateContent();
    list.repaint();
}

void PresetBrowser::resized()
{
    auto r = getLocalBounds().reduced (10);
    auto top = r.removeFromTop (28);
    closeButton.setBounds (top.removeFromRight (90));
    top.removeFromRight (8);
    search.setBounds (top);
    r.removeFromTop (8);

    // boutons de categories sur plusieurs lignes
    int x = r.getX(), y = r.getY();
    const int h = 26, gap = 6;
    for (auto& b : catButtons)
    {
        const int w = juce::jmax (70, b->getButtonText().length() * 9 + 20);
        if (x + w > r.getRight())
        {
            x = r.getX();
            y += h + gap;
        }
        b->setBounds (x, y, w, h);
        x += w + gap;
    }
    r.removeFromTop (y + h + 10 - r.getY());
    list.setBounds (r);
}

void PresetBrowser::paint (juce::Graphics& g)
{
    g.fillAll (Pal::panel);
    g.setColour (Pal::cyan.withAlpha (0.5f));
    g.drawRect (getLocalBounds().toFloat(), 1.0f);
}

void PresetBrowser::paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (! juce::isPositiveAndBelow (row, (int) shown.size()))
        return;

    const auto& e = shown[(size_t) row];
    if (selected || e.id == currentId)
        g.fillAll (Pal::panelHi);
    else if (row % 2 == 1)
        g.fillAll (Pal::bg.withAlpha (0.25f));

    g.setFont (Pal::mono (13.0f, e.id == currentId));
    g.setColour (e.id == currentId ? Pal::cyan : Pal::text);
    g.drawText (e.name, 12, 0, width / 2, height, juce::Justification::centredLeft, true);
    g.setColour (Pal::dim);
    g.drawText (e.category, width / 2, 0, width / 2 - 56, height, juce::Justification::centredLeft, true);

    juce::Path star;
    star.addStar ({ (float) width - 26.0f, (float) height * 0.5f }, 5, 4.5f, 9.0f);
    if (favs.contains (e.name))
    {
        g.setColour (Pal::cyan);
        g.fillPath (star);
    }
    else
    {
        g.setColour (Pal::dim.withAlpha (0.6f));
        g.strokePath (star, juce::PathStrokeType (1.0f));
    }
}

void PresetBrowser::listBoxItemClicked (int row, const juce::MouseEvent& e)
{
    if (! juce::isPositiveAndBelow (row, (int) shown.size()))
        return;

    const auto entry = shown[(size_t) row];
    if (e.x > list.getWidth() - 50)
    {
        if (favs.contains (entry.name))
            favs.removeString (entry.name);
        else
            favs.add (entry.name);
        saveFavourites();
        applyFilter();
        return;
    }

    currentId = entry.id;
    list.repaint();
    if (onPick)
        onPick (entry.id);
}

void MonSyntheEditor::drawStringViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const float level = pv ("strLevel"), damp = pv ("strDamp"), pos = pv ("strPos"), decay = pv ("strDecay");
    const auto area = r.reduced (10.0f * scale, 24.0f * scale);
    const float cy = area.getCentreY();
    const float amp = area.getHeight() * 0.38f * (0.25f + 0.75f * level);

    g.setColour (Pal::line.brighter (0.3f).withAlpha (0.5f));
    g.drawHorizontalLine ((int) cy, area.getX(), area.getRight());

    // la corde au moment du pincement, puis ses vibrations qui s'estompent (plus de brillance = plus de pointes)
    const float peakX = area.getX() + area.getWidth() * juce::jlimit (0.08f, 0.5f, 0.08f + pos * 0.42f);
    for (int k = 0; k < 4; ++k)
    {
        const float a = amp * std::pow (0.45f + 0.4f * damp, (float) k) * (k % 2 == 0 ? 1.0f : -1.0f);
        juce::Path p;
        p.startNewSubPath (area.getX(), cy);
        p.lineTo (peakX, cy - a);
        p.lineTo (area.getRight(), cy);
        g.setColour (colour.withAlpha (k == 0 ? 0.95f : 0.35f / (float) k));
        g.strokePath (p, juce::PathStrokeType ((k == 0 ? 2.2f : 1.2f) * scale));
    }

    g.setColour (Pal::dim);
    g.setFont (Pal::mono (10.0f * scale));
    g.drawText ("CORDE PINCEE   duree " + juce::String (decay, 1) + " s", (int) area.getX(), (int) (r.getY() + 4.0f * scale),
                (int) area.getWidth(), (int) (14.0f * scale), juce::Justification::centredLeft, false);
}


//==============================================================================
// Rack d'effets
//==============================================================================
void MonSyntheEditor::setupRack()
{
    for (int s = 0; s < 4; ++s)
    {
        auto& r = rack[(size_t) s];
        const juce::String n = "fx" + juce::String (s + 1);
        const int role = (s + 1) % 6;

        styleButton (r.onBtn, role);
        r.onBtn.setClickingTogglesState (true);
        r.onAtt = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, n + "On", r.onBtn);

        styleButton (r.leftBtn, role);
        styleButton (r.rightBtn, role);
        r.leftBtn.setEnabled (s > 0);
        r.rightBtn.setEnabled (s < 3);
        r.leftBtn.onClick = [this, s] { swapRackSlots (s - 1); };
        r.rightBtn.onClick = [this, s] { swapRackSlots (s); };

        // Le type est géré à la main : un changement fait par l'utilisateur remet les réglages par défaut du type,
        // alors qu'un changement venant d'un preset ou de l'hôte n'y touche pas.
        r.typeBox.addItemList (FxRack::typeNames(), 1);
        r.typeBox.setColour (juce::ComboBox::arrowColourId, Pal::role (role));
        r.typeBox.setColour (juce::ComboBox::textColourId, Pal::text);
        comboRefs.push_back ({ &r.typeBox, role });
        addAndMakeVisible (r.typeBox);
        r.typeBox.onChange = [this, s]
        {
            auto& rs = rack[(size_t) s];
            const int type = rs.typeBox.getSelectedItemIndex();
            const juce::String id = "fx" + juce::String (s + 1);
            if (type < 0 || type == (int) paramPlain (id + "Type"))
                return;
            setParamPlain (id + "Type", (float) type);
            const char* ks[4] = { "A", "B", "C", "D" };
            for (int k = 0; k < 4; ++k)
                setParamPlain (id + ks[k], FxRack::defaultValue (type, k));
            setParamPlain (id + "Mix", FxRack::defaultMix (type));
            setParamPlain (id + "On", 1.0f);
            refreshRack();
        };

        const char* ks[4] = { "A", "B", "C", "D" };
        for (int k = 0; k < 4; ++k)
        {
            r.k[(size_t) k] = makeKnob (n + ks[k], ks[k], role);
            r.k[(size_t) k]->slider.textFromValueFunction = [this, s, k] (double v)
            {
                return FxRack::valueText ((int) paramPlain ("fx" + juce::String (s + 1) + "Type"), k, (float) v);
            };
        }
        r.k[4] = makeKnob (n + "Mix", "MIX", role, true);
        r.k[4]->slider.textFromValueFunction = [] (double v) { return juce::String (juce::roundToInt (v * 100.0)) + "%"; };
    }
    refreshRack();
}

void MonSyntheEditor::refreshRack()
{
    for (int s = 0; s < 4; ++s)
    {
        auto& r = rack[(size_t) s];
        const int type = (int) paramPlain ("fx" + juce::String (s + 1) + "Type");

        if (r.typeBox.getSelectedItemIndex() != type)
            r.typeBox.setSelectedItemIndex (type, juce::dontSendNotification);

        if (type == r.shownType)
            continue;
        r.shownType = type;

        for (int k = 0; k < 4; ++k)
        {
            const auto text = FxRack::label (type, k);
            const bool used = text != "-";
            r.k[(size_t) k]->label.setText (used ? text : juce::String(), juce::dontSendNotification);
            r.k[(size_t) k]->slider.setEnabled (used);
            r.k[(size_t) k]->slider.setAlpha (used ? 1.0f : 0.25f);
            r.k[(size_t) k]->slider.updateText();
        }
        r.k[4]->slider.setEnabled (type != 0);
        r.k[4]->slider.setAlpha (type != 0 ? 1.0f : 0.25f);
        r.onBtn.setEnabled (type != 0);
    }
}

void MonSyntheEditor::swapRackSlots (int s)
{
    if (s < 0 || s > 2)
        return;
    const char* ids[7] = { "Type", "On", "Mix", "A", "B", "C", "D" };
    const juce::String a = "fx" + juce::String (s + 1), b = "fx" + juce::String (s + 2);
    for (auto* id : ids)
    {
        const float va = paramPlain (a + id), vb = paramPlain (b + id);
        setParamPlain (a + id, vb);
        setParamPlain (b + id, va);
    }
    refreshRack();
    repaint();
}

void MonSyntheEditor::startResample (int slot)
{
    resampleSlot = slot;
    proc.startResample();
    statusMessage = "RESAMPLE : JOUE ET TIENS UNE NOTE...";
    statusFrames = 30;
}

void MonSyntheEditor::drawFilter2Viz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour)
{
    const int type = (int) pv ("f2Type");
    const float fc = pv ("f2Cutoff");
    const float res = pv ("f2Res");
    const float mix = pv ("f2Mix");
    const bool par = pv ("f2Mode") > 0.5f;
    static const char* names[] = { "OFF", "LP 12", "HP", "BP", "NOTCH", "FORMANTS" };
    static const float vw[5][3] = { { 800.0f, 1150.0f, 2900.0f }, { 400.0f, 1600.0f, 2700.0f }, { 350.0f, 1700.0f, 2700.0f },
                                    { 450.0f, 800.0f, 2830.0f }, { 325.0f, 700.0f, 2700.0f } };

    // filtre 1 (réponse simplifiée)
    const float fc1 = pv ("cutoff"), q1 = juce::jmax (0.3f, pv ("resonance"));
    const int t1 = (int) pv ("filterType");
    auto mag1 = [&] (float f)
    {
        const float x = f / fc1, x2 = x * x;
        const float d = std::sqrt ((1.0f - x2) * (1.0f - x2) + x2 / (q1 * q1));
        switch (t1)
        {
            case 0:  return 1.0f / d;
            case 1:  return 1.0f / (d * d);
            case 2:  return x2 / d;
            case 4:  return std::abs (1.0f - x2) / d;
            case 5:  return 0.5f;
            default: return (x / q1) / d;
        }
    };

    const float pos = juce::jlimit (0.0f, 1.0f, pv ("f2Vowel")) * 4.0f;
    const int i0 = juce::jlimit (0, 3, (int) pos);
    const float fr = pos - (float) i0;
    const float shift = juce::jlimit (0.25f, 4.0f, fc / 1000.0f);
    const float q2 = type == 5 ? 2.0f + res * 2.5f : juce::jmax (0.3f, res);

    auto mag2 = [&] (float f)
    {
        if (type == 5)
        {
            float s = 0.0f;
            const float gains[3] = { 1.6f, 1.2f, 0.8f };
            for (int k = 0; k < 3; ++k)
            {
                const float c = (vw[i0][k] * (1.0f - fr) + vw[i0 + 1][k] * fr) * shift;
                const float x = f / c, x2 = x * x;
                const float d = std::sqrt ((1.0f - x2) * (1.0f - x2) + x2 / (q2 * q2));
                s += gains[k] * (x / q2) / d;
            }
            return s;
        }
        const float x = f / fc, x2 = x * x;
        const float d = std::sqrt ((1.0f - x2) * (1.0f - x2) + x2 / (q2 * q2));
        switch (type)
        {
            case 1:  return 1.0f / d;
            case 2:  return x2 / d;
            case 4:  return std::abs (1.0f - x2) / d;
            default: return (x / q2) / d;
        }
    };

    const auto area = r.reduced (4.0f * scale, 18.0f * scale);
    auto fx = [&] (float hz) { return area.getX() + area.getWidth() * std::log (hz / 20.0f) / std::log (1000.0f); };
    g.setColour (Pal::line.brighter (0.4f).withAlpha (0.7f));
    for (float hz : { 100.0f, 1000.0f, 10000.0f })
        g.drawVerticalLine ((int) fx (hz), area.getY(), area.getBottom());

    juce::Path line;
    const int n = 140;
    for (int i = 0; i <= n; ++i)
    {
        const float f = 20.0f * std::pow (1000.0f, (float) i / (float) n);
        float m;
        if (type == 0)
            m = mag1 (f);
        else if (par)
            m = (1.0f - mix) * mag1 (f) + mix * mag2 (f);
        else
            m = mag1 (f) * (1.0f - mix + mix * mag2 (f));
        const float db = juce::jlimit (-48.0f, 18.0f, juce::Decibels::gainToDecibels (m, -60.0f));
        const float x = area.getX() + area.getWidth() * (float) i / (float) n;
        const float y = area.getBottom() - area.getHeight() * (db + 48.0f) / 66.0f;
        if (i == 0) line.startNewSubPath (x, y); else line.lineTo (x, y);
    }

    juce::Path fill (line);
    fill.lineTo (area.getRight(), area.getBottom());
    fill.lineTo (area.getX(), area.getBottom());
    fill.closeSubPath();
    g.setColour (colour.withAlpha (0.16f));
    g.fillPath (fill);
    glowStroke (g, line, colour, 2.0f * scale);

    g.setFont (Pal::mono (10.5f * scale, true));
    g.setColour (Pal::text);
    g.drawText (juce::String (names[juce::jlimit (0, 5, type)]) + (par ? "  //  PARALLELE" : "  //  SERIE")
                    + "   (courbe = filtre 1 + filtre 2)",
                r.removeFromTop (14.0f * scale), juce::Justification::centredLeft, false);
}

//==============================================================================
// LFO dessiné
//==============================================================================
void DrawLfoView::setPoint (int index, float value)
{
    if (auto* p = proc.apvts.getParameter ("draw" + juce::String (index + 1)))
        p->setValueNotifyingHost (p->convertTo0to1 (juce::jlimit (-1.0f, 1.0f, value)));
}

void DrawLfoView::setShape (int kind)
{
    juce::Random rng;
    for (int i = 0; i < 32; ++i)
    {
        const float t = (float) i / 31.0f;
        float v = 0.0f;
        switch (kind)
        {
            case 0:  v = 1.0f - 2.0f * t; break;
            case 1:  v = std::sin (juce::MathConstants<float>::twoPi * t); break;
            case 2:  v = rng.nextFloat() * 2.0f - 1.0f; break;
            case 3:  v = -1.0f + 2.0f * (float) (i / 4) / 7.0f; break;
            default: v = 0.0f; break;
        }
        setPoint (i, v);
    }
}

void DrawLfoView::paintAt (const juce::MouseEvent& e)
{
    const int n = juce::jlimit (2, 32, (int) std::round (proc.apvts.getRawParameterValue ("dwSteps")->load()));
    const float w = (float) getWidth(), h = (float) getHeight();
    const int idx = juce::jlimit (0, n - 1, (int) (e.position.x / w * (float) n));
    const float val = e.mods.isPopupMenu() ? 0.0f : 1.0f - 2.0f * juce::jlimit (0.0f, 1.0f, e.position.y / h);

    if (lastIdx >= 0 && std::abs (idx - lastIdx) > 1)         // trait continu si la souris saute des pas
    {
        const int a = juce::jmin (idx, lastIdx), b = juce::jmax (idx, lastIdx);
        for (int i = a; i <= b; ++i)
            setPoint (i, lastVal + (val - lastVal) * (float) (i - lastIdx) / (float) (idx - lastIdx));
    }
    else
        setPoint (idx, val);

    lastIdx = idx;
    lastVal = val;
    repaint();
}

void DrawLfoView::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat();
    const auto col = Pal::role (3);
    const int n = juce::jlimit (2, 32, (int) std::round (proc.apvts.getRawParameterValue ("dwSteps")->load()));
    const float smooth = proc.apvts.getRawParameterValue ("dwSmooth")->load();

    g.setColour (Pal::bg.withAlpha (0.6f));
    g.fillRect (r);
    g.setColour (Pal::line);
    g.drawHorizontalLine ((int) r.getCentreY(), r.getX(), r.getRight());
    for (int i = 0; i <= n; ++i)
    {
        const float x = r.getX() + r.getWidth() * (float) i / (float) n;
        g.setColour (Pal::line.withAlpha (i % 4 == 0 ? 0.9f : 0.4f));
        g.drawVerticalLine ((int) x, r.getY(), r.getBottom());
    }

    auto yOf = [&] (float v) { return r.getY() + r.getHeight() * (1.0f - v) * 0.5f; };
    float val[33];
    for (int i = 0; i < n; ++i)
        val[i] = proc.apvts.getRawParameterValue ("draw" + juce::String (i + 1))->load();
    val[n] = val[0];

    // barres des pas
    for (int i = 0; i < n; ++i)
    {
        const float x0 = r.getX() + r.getWidth() * (float) i / (float) n;
        const float x1 = r.getX() + r.getWidth() * (float) (i + 1) / (float) n;
        const float y = yOf (val[i]), y0 = yOf (0.0f);
        g.setColour (col.withAlpha (0.22f));
        g.fillRect (juce::Rectangle<float> (x0 + 1.0f, juce::jmin (y, y0), x1 - x0 - 2.0f, std::abs (y - y0)));
    }

    // courbe réellement produite (même formule que dans la voix)
    juce::Path line;
    const int pts = 240;
    for (int k = 0; k <= pts; ++k)
    {
        const float ph = (float) k / (float) pts;
        const float x = ph * (float) n * 0.99999f;
        const int i0 = juce::jmin (n - 1, (int) x);
        const float fr = x - (float) i0;
        const float a = val[i0], b = val[i0 + 1];
        const float t = smooth <= 0.001f ? 0.0f : juce::jlimit (0.0f, 1.0f, (fr - (1.0f - smooth)) / smooth);
        const float v = a + (b - a) * (t * t * (3.0f - 2.0f * t));
        const float px = r.getX() + r.getWidth() * ph;
        if (k == 0) line.startNewSubPath (px, yOf (v)); else line.lineTo (px, yOf (v));
    }
    g.setColour (col);
    g.strokePath (line, juce::PathStrokeType (2.0f));
}

//==============================================================================
// Ergonomie : menu clic droit, réglage fin au clavier, raccourcis
//==============================================================================
void MonSyntheEditor::showKnobMenu (Knob& knob)
{
    const int cc = proc.getCcForParam (knob.paramId);
    juce::PopupMenu menu;
    menu.addItem (10, "Entrer une valeur...");
    menu.addItem (11, "Valeur par defaut");
    menu.addSeparator();
    menu.addItem (20, proc.isMidiLearning() ? "MIDI LEARN en cours : bouge un potard MIDI..." : "MIDI LEARN : assigner un potard MIDI");
    if (cc >= 0)
        menu.addItem (21, "Oublier le CC " + juce::String (cc));
    menu.addSeparator();
    menu.addItem (30, "Moduler ce reglage (macro, LFO, enveloppe)...", modTargetForParam (knob.paramId) != 0);

    juce::Component::SafePointer<MonSyntheEditor> safe (this);
    const auto id = knob.paramId;
    auto* kp = &knob;
    menu.showMenuAsync (juce::PopupMenu::Options(), [safe, id, kp] (int r)
    {
        if (safe == nullptr || r == 0)
            return;
        auto* ed = safe.getComponent();
        switch (r)
        {
            case 10: kp->slider.showTextBox(); break;
            case 11: kp->slider.setValue (kp->slider.getDoubleClickReturnValue(), juce::sendNotificationSync); break;
            case 20:
                ed->proc.startMidiLearn (id);
                ed->statusMessage = "MIDI LEARN : bouge un potard de ton clavier MIDI (Echap pour annuler)";
                ed->statusFrames = 600;
                break;
            case 21:
                ed->proc.forgetCcForParam (id);
                ed->statusMessage = "CC oublie";
                ed->statusFrames = 90;
                break;
            case 30: ed->showModMenu (*kp); break;
            default: break;
        }
    });
}

void MonSyntheEditor::mouseEnter (const juce::MouseEvent& e)
{
    for (auto& k : knobs)
        if (e.eventComponent == &k->slider)
        {
            hoveredKnob = k.get();
            lastKnob = k.get();
            grabKeyboardFocus();
            return;
        }
}

void MonSyntheEditor::mouseExit (const juce::MouseEvent& e)
{
    if (hoveredKnob != nullptr && e.eventComponent == &hoveredKnob->slider)
        hoveredKnob = nullptr;
}

void MonSyntheEditor::autoEnable (const juce::String& id)
{
    // Un potard qui ne fait rien parce que son module est sur OFF déroute : on l'allume au premier geste.
    auto enable = [this] (const char* choiceId, int value, const char* msg)
    {
        if (proc.apvts.getRawParameterValue (choiceId)->load() < 0.5f)
            if (auto* p = proc.apvts.getParameter (choiceId))
            {
                p->setValueNotifyingHost (p->convertTo0to1 ((float) value));
                statusMessage = msg;
                statusFrames = 90;
            }
    };

    if (id == "warpAmtA")        enable ("warpModeA", 1, "WARP A ACTIVE (BEND+) : change le mode dans la liste");
    else if (id == "warpAmtB")   enable ("warpModeB", 1, "WARP B ACTIVE (BEND+) : change le mode dans la liste");
    else if (id == "f2Vowel")    enable ("f2Type", 5, "FILTRE 2 ACTIVE (FORMANTS)");
    else if (id == "f2Cutoff" || id == "f2Res" || id == "f2Mix")
        enable ("f2Type", 1, "FILTRE 2 ACTIVE (LP 12) : change le type dans la liste");
}

void MonSyntheEditor::nudgeKnob (Knob& k, int direction, bool fine, bool big)
{
    autoEnable (k.paramId);
    auto& s = k.slider;
    const double lo = s.getMinimum(), hi = s.getMaximum();
    const double range = hi - lo;
    const double interval = s.getInterval();
    double v = s.getValue();

    if (fine)
    {
        // 0,25 % de la course du potard (dans l'échelle réelle du potard : précis même pour les Hz)
        const double p = juce::jlimit (0.0, 1.0, s.valueToProportionOfLength (v) + direction * (big ? 0.025 : 0.0025));
        v = s.proportionOfLengthToValue (p);
    }
    else
    {
        // une unité affichée : 1 si la plage est grande, sinon 0,1 ou 0,01
        double step = range >= 20.0 ? 1.0 : (range >= 2.0 ? 0.1 : 0.01);
        if (interval > 0.0)
            step = juce::jmax (step, interval);
        v += direction * step * (big ? 10.0 : 1.0);
    }

    s.setValue (juce::jlimit (lo, hi, v), juce::sendNotificationSync);
    statusMessage = k.label.getText() + " = " + s.getTextFromValue (s.getValue());
    statusFrames = 60;
}

bool MonSyntheEditor::keyPressed (const juce::KeyPress& key)
{
    const bool shift = key.getModifiers().isShiftDown();

    if (key == juce::KeyPress::escapeKey)
    {
        if (proc.isMidiLearning())
        {
            proc.cancelMidiLearn();
            statusMessage = "MIDI LEARN annule";
            statusFrames = 60;
        }
        return true;
    }

    if (key == juce::KeyPress::pageUpKey)   { stepPreset (-1); return true; }
    if (key == juce::KeyPress::pageDownKey) { stepPreset (1);  return true; }

    if (key == juce::KeyPress::tabKey)
    {
        selectMenu ((currentMenu + (shift ? numMenus - 1 : 1)) % numMenus);
        return true;
    }

    auto* k = hoveredKnob != nullptr ? hoveredKnob : lastKnob;
    if (k == nullptr || ! k->slider.isShowing() || ! k->slider.isEnabled())
        return false;

    if (key.isKeyCode (juce::KeyPress::upKey))    { nudgeKnob (*k, 1, false, shift);  return true; }
    if (key.isKeyCode (juce::KeyPress::downKey))  { nudgeKnob (*k, -1, false, shift); return true; }
    if (key.isKeyCode (juce::KeyPress::rightKey)) { nudgeKnob (*k, 1, true, shift);   return true; }
    if (key.isKeyCode (juce::KeyPress::leftKey))  { nudgeKnob (*k, -1, true, shift);  return true; }
    return false;
}
