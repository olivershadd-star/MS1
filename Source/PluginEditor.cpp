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
    static const MenuDef defs[numMenus] =
    {
        { "OSC A",     0, VizScopeA, VizSpectrum, { { "position", "POSITION" }, { "warpAmtA", "WARP" },
                                                    { "unison", "UNISON" }, { "detune", "DESACCORD" },
                                                    { "spread", "STEREO" }, { "subLevel", "SUB" } } },
        { "OSC B",     1, VizScopeB, VizFmRing,   { { "oscBPos", "POSITION" }, { "warpAmtB", "WARP" },
                                                    { "oscBOct", "OCTAVE" }, { "oscBTune", "ACCORD" },
                                                    { "oscBLevel", "NIVEAU" }, { "fmAmount", "FM" },
                                                    { "ringAmount", "RING" } } },
        { "FM / RING", 2, VizFmRing, VizScopeB,   { { "fmAmount", "FM" }, { "ringAmount", "RING" },
                                                    { "oscBLevel", "NIVEAU B" }, { "subLevel", "SUB" } } },
        { "UNISON",    3, VizUnison, VizScopeA,   { { "unison", "VOIX" }, { "detune", "DESACCORD" },
                                                    { "spread", "STEREO" } } },
        { "FILTRE",    4, VizFilter, VizEnvFilter,{ { "cutoff", "CUTOFF" }, { "resonance", "RESO" },
                                                    { "drive", "DRIVE" }, { "fltEnvAmount", "ENV AMT" },
                                                    { "fltAttack", "ATTACK" }, { "fltDecay", "DECAY" },
                                                    { "fltSustain", "SUSTAIN" }, { "fltRelease", "RELEASE" } } },
        { "ENV AMP",   5, VizEnvAmp, VizSpectrum, { { "attack", "ATTACK" }, { "decay", "DECAY" },
                                                    { "sustain", "SUSTAIN" }, { "release", "RELEASE" } } },
        { "ENV MOD",   0, VizEnvMod, VizSpectrum, { { "modAttack", "ATTACK" }, { "modDecay", "DECAY" },
                                                    { "modSustain", "SUSTAIN" }, { "modRelease", "RELEASE" },
                                                    { "modAmount", "QUANTITE" } } },
        { "LFO",       1, VizLfo1, VizLfo2,       { { "lfoRate", "VITESSE" }, { "lfoDepth", "INTENSITE" },
                                                    { nullptr, nullptr }, { nullptr, nullptr },
                                                    { "lfo2Rate", "VITESSE" } } },
        { "JEU",       2, VizGlide, VizSpectrum,  { { "glide", "GLIDE" }, { "bendRange", "BEND" } } },
        { "CHORUS",    3, VizChorus, VizSpectrum, { { "chorusMix", "MIX" }, { "chorusRate", "VITESSE" },
                                                    { "chorusDepth", "PROFONDEUR" } } },
        { "DELAY",     4, VizDelay, VizSpectrum,  { { "delayTime", "TEMPS" }, { "delayFeedback", "FEEDBACK" },
                                                    { "delayMix", "MIX" } } },
        { "REVERB",    5, VizReverb, VizSpectrum, { { "reverbSize", "TAILLE" }, { "reverbDamp", "AMORTI" },
                                                    { "reverbMix", "MIX" } } },
        { "VELOCITE",  0, VizSpectrum, VizScopeA, { { "velAmp", "VEL > VOL" }, { "velFilter", "VEL > FILTRE" } } },
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

    auto* raw = k.get();
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
    setupCombo (lfoShape, "lfoShape", 1);    setupCombo (lfoSync, "lfoSync", 1);
    setupCombo (lfo2Shape, "lfo2Shape", 2);  setupCombo (lfo2Sync, "lfo2Sync", 2);

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

    // --- Matrice
    for (int i = 0; i < 6; ++i)
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

    // --- En-tête : presets
    for (auto* b : { &prevButton, &nextButton, &saveButton, &deleteButton })
        styleButton (*b, 0);

    presetBox.setColour (juce::ComboBox::arrowColourId, Pal::cyan);
    presetBox.setColour (juce::ComboBox::textColourId, Pal::text);
    comboRefs.push_back ({ &presetBox, 0 });
    addAndMakeVisible (presetBox);

    nameEditor.setMultiLine (false);
    nameEditor.setTextToShowWhenEmpty ("nom du preset", Pal::dim);
    addAndMakeVisible (nameEditor);

    refreshPresetList();

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
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

        if (def.p[i].id != nullptr)
        {
            k->role = i < 4 ? def.role : (def.role + 1) % 6;
            k->slider.setColour (juce::Slider::rotarySliderFillColourId, Pal::role (k->role));
            k->label.setText (def.p[i].label, juce::dontSendNotification);
            k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, def.p[i].id, k->slider);
        }
    }

    for (int i = 0; i < numMenus; ++i)
        menuButtons[(size_t) i].setToggleState (i == currentMenu, juce::dontSendNotification);

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

    tableA.box.setVisible (currentMenu == 0);
    warpA.box.setVisible (currentMenu == 0);
    importAButton.setVisible (currentMenu == 0);
    tableB.box.setVisible (currentMenu == 1);
    warpB.box.setVisible (currentMenu == 1);
    importBButton.setVisible (currentMenu == 1);
    filterType.box.setVisible (currentMenu == 4);
    lfoShape.box.setVisible (currentMenu == 7);
    lfoSync.box.setVisible (currentMenu == 7);
    lfo2Shape.box.setVisible (currentMenu == 7);
    lfo2Sync.box.setVisible (currentMenu == 7);
    monoButton.setVisible (currentMenu == 8);

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
            case 0: case 1: case 2: case 3:
                lvl = active ? 0.35f + 0.65f * pulse (1.3 + 0.4 * i, i) : 0.12f;  break;
            case 4:  lvl = active ? 0.35f + 0.65f * pulse (2.1, 0.5) : 0.12f;     break;
            case 5:  lvl = active ? 1.0f : 0.12f;                                  break;
            case 6:  lvl = active ? 0.35f + 0.65f * pulse (1.7, 1.0) : 0.12f;     break;
            case 7:
            {
                const double hz = pv ("lfoSync") > 0.5f ? bpm / 60.0 : (double) pv ("lfoRate");
                lvl = 0.12f + 0.88f * pulse (juce::jmin (hz, 8.0), 0.0);
                break;
            }
            case 8:  lvl = pv ("mono") > 0.5f ? 0.9f : 0.25f;                      break;
            case 9:  lvl = pv ("chorusMix") > 0.01f ? 0.45f + 0.55f * pulse (pv ("chorusRate"), 0.0) : 0.12f; break;
            case 10: lvl = pv ("delayMix") > 0.01f
                              ? 0.2f + 0.8f * pulse (juce::jmin (1000.0 / (double) juce::jmax (20.0f, pv ("delayTime")), 8.0), 0.0)
                              : 0.12f;
                     break;
            case 11: lvl = pv ("reverbMix") > 0.01f ? 0.8f : 0.12f;                break;
            case 12: lvl = active ? 0.7f : 0.12f;                                  break;
            default:
            {
                int used = 0;
                for (int r = 1; r <= 6; ++r)
                    if (std::abs (pv (("mx" + juce::String (r) + "Amt").toRawUTF8())) > 0.01f)
                        ++used;
                lvl = used > 0 ? 0.5f + 0.5f * (float) used / 6.0f : 0.12f;
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
    presetBox.setBounds (R (206, 12, 214, 28));
    nextButton.setBounds (R (424, 12, 26, 28));
    nameEditor.setBounds (R (458, 12, 120, 28));
    saveButton.setBounds (R (582, 12, 52, 28));
    deleteButton.setBounds (R (638, 12, 62, 28));
    headerSpectrum.setBounds (R (712, 8, 290, 38));
    hueBar.setBounds (R (1020, 12, 240, 28));

    // Menus
    menuPanel = R (8, 60, 132, 476);
    for (int i = 0; i < numMenus; ++i)
        menuButtons[(size_t) i].setBounds (R (14, 88 + i * 31, 120, 28));

    // Écrans
    screenAPanel = R (148, 60, 426, 476);
    screenBPanel = R (582, 60, 426, 476);
    gfxA   = R (158, 90, 406, 236);   gfxB   = R (592, 90, 406, 236);
    stripA = R (158, 332, 406, 28);   stripB = R (592, 332, 406, 28);
    knobsA = R (158, 366, 406, 162);  knobsB = R (592, 366, 406, 162);
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
            tableA.box.setBounds (first);       warpA.box.setBounds (second);       importAButton.setBounds (third);
            tableB.box.setBounds (first);       warpB.box.setBounds (second);       importBButton.setBounds (third);
            filterType.box.setBounds (first);
            lfoShape.box.setBounds (first);     lfoSync.box.setBounds (second);
            monoButton.setBounds (first);
        }
        else
        {
            lfo2Shape.box.setBounds (first);    lfo2Sync.box.setBounds (second);
        }
    }

    for (int s = 0; s < 2; ++s)
    {
        std::vector<Knob*> row;
        for (int i = 0; i < 4; ++i)
            row.push_back (fixedKnobs[(size_t) (s * 4 + i)]);
        layoutRow (row, s == 0 ? knobsA : knobsB);
    }

    // Matrice (occupe les deux écrans)
    matrixArea = R (158, 92, 840, 436);
    {
        const int x0 = matrixArea.getX();
        mixHdrSrc.setBounds (x0, matrixArea.getY(), S (230.0f), S (22.0f));
        mixHdrDst.setBounds (x0 + S (266.0f), matrixArea.getY(), S (230.0f), S (22.0f));
        mixHdrAmt.setBounds (x0 + S (510.0f), matrixArea.getY(), S (330.0f), S (22.0f));

        for (int i = 0; i < 6; ++i)
        {
            const int y = matrixArea.getY() + S (26.0f) + i * S (60.0f) + S (15.0f);
            mixSrcBox[(size_t) i].setBounds (x0, y, S (230.0f), S (30.0f));
            mixDstBox[(size_t) i].setBounds (x0 + S (266.0f), y, S (230.0f), S (30.0f));
            placeKnob (mixAmt[(size_t) i], juce::Rectangle<int> (x0 + S (510.0f), y, S (330.0f), S (30.0f)));
        }
    }

    // Colonne de droite
    macrosPanel = R (1016, 60, 256, 250);
    for (int i = 0; i < 4; ++i)
        placeKnob (macroKnobs[(size_t) i],
                   R (1024 + (i % 2) * 122, 90 + (i / 2) * 108, 120, 104).reduced (S (4.0f)));

    outPanel = R (1016, 318, 256, 218);
    placeKnob (masterKnob, R (1024, 346, 124, 182));
    meterArea = R (1160, 352, 24, 168);
    ledsArea  = R (1196, 352, 66, 120);

    // Séquenceur
    seqPanel = R (8, 544, 1264, 168);
    seqButton.setBounds (R (18, 572, 150, 24));
    barsBox.setBounds (R (18, 600, 150, 22));
    rateBox.setBounds (R (18, 626, 150, 22));
    placeKnob (gateKnob, R (18, 652, 150, 20));
    placeKnob (tempoKnob, R (18, 676, 150, 20));
    grid.setBounds (R (180, 572, 1080, 132));
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

    updateLeds();

    if (statusFrames > 0)
        --statusFrames;
    if (deleteArmed > 0 && --deleteArmed == 0)
        deleteButton.setButtonText ("SUPPR");

    repaint (gfxA);
    repaint (gfxB);
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
    loadWavetableFile (currentMenu == 1 ? 1 : 0, juce::File (files[0]));
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
        drawPanel (g, both, "MATRICE DE MODULATION   //   SOURCE  x  QUANTITE  ->  CIBLE", Pal::role (menuAt (currentMenu).role));

        g.setFont (Pal::mono (12.0f * scale, true));
        g.setColour (Pal::dim);
        for (int i = 0; i < 6; ++i)
        {
            const int y = matrixArea.getY() + S (26.0f) + i * S (60.0f) + S (15.0f);
            g.drawText ("->", matrixArea.getX() + S (232.0f), y, S (32.0f), S (30.0f), juce::Justification::centred, false);
            g.setColour (Pal::line);
            g.drawHorizontalLine (matrixArea.getY() + S (26.0f) + (i + 1) * S (60.0f) - S (2.0f),
                                  (float) matrixArea.getX(), (float) matrixArea.getRight());
            g.setColour (Pal::dim);
        }

        g.setFont (Pal::mono (10.5f * scale));
        g.setColour (Pal::dim);
        g.drawFittedText ("Chaque ligne relie une SOURCE (LFO, enveloppe, velocite, macro...) a une CIBLE (cutoff, position, warp...).\n"
                          "La quantite regle la force, et peut etre negative. Exemple : LFO -> CUTOFF = wobble ; MACRO 1 -> plusieurs cibles.",
                          juce::Rectangle<int> (matrixArea.getX(), matrixArea.getY() + S (392.0f), matrixArea.getWidth(), S (40.0f)),
                          juce::Justification::topLeft, 3);
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
        case VizEnvFilter: drawEnvelopeViz (g, r, colour, "fltAttack", "fltDecay", "fltSustain", "fltRelease"); break;
        case VizEnvAmp:    drawEnvelopeViz (g, r, colour, "attack", "decay", "sustain", "release");            break;
        case VizEnvMod:    drawEnvelopeViz (g, r, colour, "modAttack", "modDecay", "modSustain", "modRelease"); break;
        case VizLfo1:      drawLfoViz (g, r, colour, false);   break;
        case VizLfo2:      drawLfoViz (g, r, colour, true);    break;
        case VizGlide:     drawGlideViz (g, r, colour);        break;
        case VizChorus:    drawChorusViz (g, r, colour);       break;
        case VizDelay:     drawDelayViz (g, r, colour);        break;
        case VizReverb:    drawReverbViz (g, r, colour);       break;
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
    static const char* typeNames[] = { "LP 12", "LP 24", "HP", "BP" };

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

void MonSyntheEditor::drawLfoViz (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour colour, bool second)
{
    const int shape = (int) pv (second ? "lfo2Shape" : "lfoShape");
    const float rate = pv (second ? "lfo2Rate" : "lfoRate");
    const int sync = (int) pv (second ? "lfo2Sync" : "lfoSync");
    const float depth = second ? 1.0f : juce::jmax (0.12f, pv ("lfoDepth"));
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
    g.drawText (juce::String (second ? "LFO 2   " : "LFO 1   ")
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
