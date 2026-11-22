#include "PluginEditor.h"

namespace
{
    // Découpe un rectangle en colonnes proportionnelles aux poids (avec un espace fixe entre elles)
    std::vector<juce::Rectangle<int>> splitRow (juce::Rectangle<int> area, std::initializer_list<float> weights, int gap)
    {
        std::vector<juce::Rectangle<int>> out;
        float total = 0.0f;
        for (auto w : weights)
            total += w;

        const float usable = (float) (area.getWidth() - gap * ((int) weights.size() - 1));
        float x = (float) area.getX();
        size_t i = 0;

        for (auto w : weights)
        {
            const float width = usable * w / total;
            const int x0 = juce::roundToInt (x);
            const int x1 = (++i == weights.size()) ? area.getRight() : juce::roundToInt (x + width);
            out.push_back ({ x0, area.getY(), x1 - x0, area.getHeight() });
            x += width + (float) gap;
        }
        return out;
    }

    // Idem, en lignes
    std::vector<juce::Rectangle<int>> splitCol (juce::Rectangle<int> area, std::initializer_list<float> weights, int gap)
    {
        std::vector<juce::Rectangle<int>> out;
        float total = 0.0f;
        for (auto w : weights)
            total += w;

        const float usable = (float) (area.getHeight() - gap * ((int) weights.size() - 1));
        float y = (float) area.getY();
        size_t i = 0;

        for (auto w : weights)
        {
            const float height = usable * w / total;
            const int y0 = juce::roundToInt (y);
            const int y1 = (++i == weights.size()) ? area.getBottom() : juce::roundToInt (y + height);
            out.push_back ({ area.getX(), y0, area.getWidth(), y1 - y0 });
            y += height + (float) gap;
        }
        return out;
    }
}

//==============================================================================
// Look & feel
//==============================================================================
CyberLookAndFeel::CyberLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId, Pal::panel);
    setColour (juce::PopupMenu::textColourId, Pal::dim);
    setColour (juce::PopupMenu::headerTextColourId, Pal::indigo);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Pal::cyan.withAlpha (0.22f));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff070a12));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);

    setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff070a12));
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
    const float r = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f - 4.0f * scale;
    if (r < 6.0f)
        return;

    const float cx = b.getCentreX();
    const float cy = b.getCentreY();
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Graduations
    g.setColour (accent.withAlpha (0.30f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (float) i / 10.0f;
        const float s = std::sin (a), c = std::cos (a);
        const float r0 = r + 3.0f * scale;
        const float r1 = r + (i % 5 == 0 ? 6.5f : 4.8f) * scale;
        g.drawLine (cx + s * r0, cy - c * r0, cx + s * r1, cy - c * r1, 1.0f);
    }

    // Rail et arc de valeur
    juce::Path track;
    track.addCentredArc (cx, cy, r, r, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (Pal::line.brighter (0.35f));
    g.strokePath (track, juce::PathStrokeType (2.0f * scale));

    juce::Path value;
    value.addCentredArc (cx, cy, r, r, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent);
    g.strokePath (value, juce::PathStrokeType (2.6f * scale));

    // Corps
    const float rb = r - 6.0f * scale;
    g.setColour (Pal::panelHi);
    g.fillEllipse (cx - rb, cy - rb, rb * 2.0f, rb * 2.0f);
    g.setColour (Pal::line.brighter (0.5f));
    g.drawEllipse (cx - rb, cy - rb, rb * 2.0f, rb * 2.0f, 1.0f);

    // Repère
    const float s = std::sin (angle), c = std::cos (angle);
    g.setColour (accent.brighter (0.3f));
    g.drawLine (cx + s * rb * 0.35f, cy - c * rb * 0.35f,
                cx + s * (rb - 2.0f * scale), cy - c * (rb - 2.0f * scale), 2.0f * scale);

    // Pastille au bout de l'arc
    const float dot = 2.6f * scale;
    g.setColour (accent.brighter (0.5f));
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

    g.setColour (Pal::line.brighter (0.35f));
    g.fillRect (left, cy - 1.0f * scale, (float) width, 2.0f * scale);
    g.setColour (accent);
    g.fillRect (left, cy - 1.25f * scale, fillW, 2.5f * scale);

    // Curseur en losange
    const float d = 5.0f * scale;
    juce::Path diamond;
    diamond.addRectangle (-d, -d, d * 2.0f, d * 2.0f);
    g.setColour (accent.brighter (0.4f));
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
    g.setColour (Pal::line.brighter (0.5f));
    g.drawRect (r, 1.0f);
    g.setColour (accent);
    g.fillRect (r.getX(), r.getY(), 2.5f * scale, r.getHeight());           // barre d'accent à gauche

    // Chevron
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

    float a = button.getToggleState() ? 0.22f : 0.0f;
    if (down)             a = 0.38f;
    else if (highlighted) a = juce::jmax (a, 0.12f);

    g.setColour (accent.withAlpha (a));
    g.fillRect (r);
    g.setColour (accent.withAlpha (button.getToggleState() ? 0.9f : 0.5f));
    g.drawRect (r, 1.0f);

    if (button.getToggleState())                                            // trait d'état en bas
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
        level = v > level ? v : level * 0.86f + v * 0.14f;                  // attaque immédiate, descente douce
    }

    repaint();
}

void SpectrumView::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat();

    g.setColour (juce::Colour (0xff070a12));
    g.fillRect (r);
    g.setColour (Pal::line.brighter (0.5f));
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
            const float t = (float) seg / (float) (numSegments - 1);        // 0 en bas, 1 en haut
            const auto colour = Pal::azure.interpolatedWith (Pal::cyan, t);
            const bool on = seg < lit;

            g.setColour (colour.withAlpha (on ? 0.95f : 0.07f));
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

        g.setColour (((col / 4) % 2 == 0 ? juce::Colour (0xff111827) : juce::Colour (0xff0b101b)).withMultipliedAlpha (dim));
        g.fillRect (r);

        if (on)
        {
            g.setColour (accent.withAlpha (0.9f * dim));
            g.fillRect (r);

            g.setColour (juce::Colour (0xff05070c));
            g.setFont (Pal::mono (fontSize, true));
            g.drawText (juce::MidiMessage::getMidiNoteName (proc.sequencer.note[(size_t) step], true, true, 3),
                        r, juce::Justification::centred, false);
        }
        else
        {
            g.setColour (accent.withAlpha (0.14f * dim));
            g.drawRect (r, 1.0f);
        }

        if (seqOn && step == playing && row < bars)
        {
            g.setColour (juce::Colours::white.withAlpha (0.15f));
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

    if (e.mods.isPopupMenu())                 // clic droit : on efface
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

    if (! wasOn)                              // un pas éteint s'allume tout de suite
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
    if (dragStep >= 0 && wasOn && ! dragMoved)   // simple clic sur un pas allumé : on l'éteint
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
MonSyntheEditor::Knob* MonSyntheEditor::makeKnob (int pg, const juce::String& paramId, const juce::String& name,
                                                  juce::Colour colour, bool linear)
{
    auto k = std::make_unique<Knob>();

    k->slider.setSliderStyle (linear ? juce::Slider::LinearHorizontal : juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (linear ? juce::Slider::TextBoxRight : juce::Slider::TextBoxBelow, false, 64, 16);
    k->slider.setColour (juce::Slider::rotarySliderFillColourId, colour);
    k->slider.setColour (juce::Slider::textBoxTextColourId, Pal::text);

    k->label.setText (name, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setColour (juce::Label::textColourId, Pal::dim);

    addAndMakeVisible (k->slider);
    addAndMakeVisible (k->label);
    k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, paramId, k->slider);

    pageItems.push_back ({ &k->slider, pg });
    pageItems.push_back ({ &k->label, pg });

    knobs.push_back (std::move (k));
    return knobs.back().get();
}

void MonSyntheEditor::styleCombo (int pg, juce::ComboBox& box, juce::Colour colour, const juce::StringArray& items)
{
    box.addItemList (items, 1);
    box.setJustificationType (juce::Justification::centredLeft);
    box.setColour (juce::ComboBox::textColourId, Pal::text);
    box.setColour (juce::ComboBox::arrowColourId, colour);
    addAndMakeVisible (box);
    pageItems.push_back ({ &box, pg });
}

void MonSyntheEditor::setupCombo (int pg, juce::ComboBox& box, juce::Label& label, const juce::String& title,
                                  juce::Colour colour, const juce::StringArray& items)
{
    styleCombo (pg, box, colour, items);

    label.setText (title, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, Pal::dim);
    addAndMakeVisible (label);
    pageItems.push_back ({ &label, pg });
}

void MonSyntheEditor::updatePageVisibility()
{
    for (auto& item : pageItems)
        item.comp->setVisible (item.page == page);
}

MonSyntheEditor::MonSyntheEditor (MonSyntheProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p), grid (p, Pal::cyan), spectrum (p)
{
    setLookAndFeel (&laf);

    using namespace Pal;

    // ------------------------------------------------------------ Oscillateurs
    waveKnob = makeKnob (pageSynth, "position", "POSITION", cyan);
    setupCombo (pageSynth, tableABox, tableALabel, "TABLE", cyan, WavetableBank::tableNames());
    tableAAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "tableA", tableABox);

    oscBKnobs = { makeKnob (pageSynth, "oscBPos",   "POSITION", azure),
                  makeKnob (pageSynth, "oscBOct",   "OCTAVE",   azure),
                  makeKnob (pageSynth, "oscBTune",  "ACCORD",   azure),
                  makeKnob (pageSynth, "oscBLevel", "NIVEAU",   azure),
                  makeKnob (pageSynth, "subLevel",  "SUB",      azure) };
    setupCombo (pageSynth, tableBBox, tableBLabel, "TABLE", azure, WavetableBank::tableNames());
    tableBAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "tableB", tableBBox);

    importAButton.setColour (juce::TextButton::buttonOnColourId, cyan);
    importAButton.setColour (juce::TextButton::textColourOffId, cyan);
    importAButton.onClick = [this] { importWavetable (0); };
    addAndMakeVisible (importAButton);
    pageItems.push_back ({ &importAButton, pageSynth });

    importBButton.setColour (juce::TextButton::buttonOnColourId, azure);
    importBButton.setColour (juce::TextButton::textColourOffId, azure);
    importBButton.onClick = [this] { importWavetable (1); };
    addAndMakeVisible (importBButton);
    pageItems.push_back ({ &importBButton, pageSynth });

    unisonKnobs = { makeKnob (pageSynth, "unison", "VOIX",      blue),
                    makeKnob (pageSynth, "detune", "DESACCORD", blue),
                    makeKnob (pageSynth, "spread", "STEREO",    blue) };

    // ------------------------------------------------------------ Filtre et enveloppes
    setupCombo (pageSynth, filterTypeBox, filterTypeLabel, "TYPE", teal,
                juce::StringArray { "LP 12", "LP 24", "HP", "BP" });
    filterTypeAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "filterType", filterTypeBox);

    filterKnobs = { makeKnob (pageSynth, "cutoff",       "CUTOFF",  teal),
                    makeKnob (pageSynth, "resonance",    "RESO",    teal),
                    makeKnob (pageSynth, "drive",        "DRIVE",   teal),
                    makeKnob (pageSynth, "fltEnvAmount", "ENV AMT", teal) };

    fltEnvKnobs = { makeKnob (pageSynth, "fltAttack",  "ATTACK",  teal),
                    makeKnob (pageSynth, "fltDecay",   "DECAY",   teal),
                    makeKnob (pageSynth, "fltSustain", "SUSTAIN", teal),
                    makeKnob (pageSynth, "fltRelease", "RELEASE", teal) };

    ampKnobs = { makeKnob (pageSynth, "attack",  "ATTACK",  indigo),
                 makeKnob (pageSynth, "decay",   "DECAY",   indigo),
                 makeKnob (pageSynth, "sustain", "SUSTAIN", indigo),
                 makeKnob (pageSynth, "release", "RELEASE", indigo) };

    modKnobs = { makeKnob (pageSynth, "modAttack",  "ATTACK",   violet),
                 makeKnob (pageSynth, "modDecay",   "DECAY",    violet),
                 makeKnob (pageSynth, "modSustain", "SUSTAIN",  violet),
                 makeKnob (pageSynth, "modRelease", "RELEASE",  violet),
                 makeKnob (pageSynth, "modAmount",  "QUANTITE", violet) };

    // ------------------------------------------------------------ LFO, jeu, effets
    setupCombo (pageSynth, lfoShapeBox, lfoShapeLabel, "FORME", violet,
                juce::StringArray { "Sinus", "Triangle", "Saw", "Carre" });
    lfoShapeAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "lfoShape", lfoShapeBox);
    setupCombo (pageSynth, lfoSyncBox, lfoSyncLabel, "SYNC", violet, MonSyntheProcessor::lfoSyncNames());
    lfoSyncAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "lfoSync", lfoSyncBox);

    lfoKnobs = { makeKnob (pageSynth, "lfoRate",  "VITESSE",   violet),
                 makeKnob (pageSynth, "lfoDepth", "INTENSITE", violet) };

    monoButton.setClickingTogglesState (true);
    monoButton.setColour (juce::TextButton::buttonOnColourId, cyan);
    monoButton.setColour (juce::TextButton::textColourOffId, cyan);
    monoButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    monoButton.onStateChange = [this] { monoButton.setButtonText (monoButton.getToggleState() ? "MONO" : "POLY"); };
    addAndMakeVisible (monoButton);
    monoAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "mono", monoButton);
    monoButton.setButtonText (monoButton.getToggleState() ? "MONO" : "POLY");
    pageItems.push_back ({ &monoButton, pageSynth });

    jeuKnobs = { makeKnob (pageSynth, "glide",     "GLIDE",      cyan),
                 makeKnob (pageSynth, "bendRange", "PITCH BEND", cyan),
                 makeKnob (pageSynth, "master",    "VOLUME",     cyan) };

    chorusKnobs = { makeKnob (pageSynth, "chorusMix",   "MIX",     azure),
                    makeKnob (pageSynth, "chorusRate",  "VITESSE", azure),
                    makeKnob (pageSynth, "chorusDepth", "PROFOND.", azure) };

    delayKnobs = { makeKnob (pageSynth, "delayTime",     "TEMPS",    blue),
                   makeKnob (pageSynth, "delayFeedback", "FEEDBACK", blue),
                   makeKnob (pageSynth, "delayMix",      "MIX",      blue) };

    reverbKnobs = { makeKnob (pageSynth, "reverbSize", "TAILLE", indigo),
                    makeKnob (pageSynth, "reverbDamp", "AMORTI", indigo),
                    makeKnob (pageSynth, "reverbMix",  "MIX",    indigo) };

    // ------------------------------------------------------------ Page MOD
    for (size_t i = 0; i < 6; ++i)
    {
        const juce::String n = "mx" + juce::String ((int) i + 1);
        styleCombo (pageMod, mixSrcBox[i], cyan, MonSyntheProcessor::modSourceNames());
        styleCombo (pageMod, mixDstBox[i], indigo, MonSyntheProcessor::modTargetNames());
        mixSrcAtt[i] = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, n + "Src", mixSrcBox[i]);
        mixDstAtt[i] = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, n + "Dst", mixDstBox[i]);
        mixAmt[i] = makeKnob (pageMod, n + "Amt", "", azure, true);
    }

    const std::pair<juce::Label*, const char*> headers[] = { { &mixHdrSrc, "SOURCE" },
                                                             { &mixHdrDst, "CIBLE" },
                                                             { &mixHdrAmt, "QUANTITE" } };
    for (const auto& h : headers)
    {
        h.first->setText (h.second, juce::dontSendNotification);
        h.first->setJustificationType (juce::Justification::centredLeft);
        h.first->setColour (juce::Label::textColourId, dim);
        addAndMakeVisible (*h.first);
        pageItems.push_back ({ h.first, pageMod });
    }

    velKnobs = { makeKnob (pageMod, "velAmp",    "VOLUME", teal, true),
                 makeKnob (pageMod, "velFilter", "FILTRE", teal, true) };

    setupCombo (pageMod, lfo2ShapeBox, lfo2ShapeLabel, "FORME", violet,
                juce::StringArray { "Sinus", "Triangle", "Saw", "Carre" });
    lfo2ShapeAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "lfo2Shape", lfo2ShapeBox);
    setupCombo (pageMod, lfo2SyncBox, lfo2SyncLabel, "SYNC", violet, MonSyntheProcessor::lfoSyncNames());
    lfo2SyncAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "lfo2Sync", lfo2SyncBox);
    lfo2Knobs = { makeKnob (pageMod, "lfo2Rate", "VITESSE", violet) };

    macroKnobs = { makeKnob (pageMod, "macro1", "MACRO 1", cyan),
                   makeKnob (pageMod, "macro2", "MACRO 2", cyan),
                   makeKnob (pageMod, "macro3", "MACRO 3", cyan),
                   makeKnob (pageMod, "macro4", "MACRO 4", cyan) };

    // ------------------------------------------------------------ Page SEQ
    seqButton.setClickingTogglesState (true);
    seqButton.setColour (juce::TextButton::buttonOnColourId, teal);
    seqButton.setColour (juce::TextButton::textColourOffId, teal);
    seqButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    seqButton.onStateChange = [this] { seqButton.setButtonText (seqButton.getToggleState() ? "SEQ ON" : "SEQ OFF"); };
    addAndMakeVisible (seqButton);
    seqButtonAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "seqOn", seqButton);
    seqButton.setButtonText (seqButton.getToggleState() ? "SEQ ON" : "SEQ OFF");
    pageItems.push_back ({ &seqButton, pageSeq });

    setupCombo (pageSeq, barsBox, barsLabel, "LONGUEUR", teal,
                juce::StringArray { "1 mesure", "2 mesures", "3 mesures", "4 mesures" });
    barsAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqBars", barsBox);

    setupCombo (pageSeq, rateBox, rateLabel, "VITESSE", teal, juce::StringArray { "1/8", "1/16", "1/32" });
    rateAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqRate", rateBox);

    gateKnob = makeKnob (pageSeq, "seqGate", "GATE", teal, true);
    tempoKnob = makeKnob (pageSeq, "seqTempo", "TEMPO (hors lecture)", teal, true);

    addAndMakeVisible (grid);
    pageItems.push_back ({ &grid, pageSeq });

    // ------------------------------------------------------------ En-tête : onglets, analyseur, presets
    const char* tabNames[] = { "SYNTH", "MOD", "SEQ" };
    const juce::Colour tabColours[] = { cyan, violet, teal };

    for (size_t i = 0; i < tabButtons.size(); ++i)
    {
        auto& b = tabButtons[i];
        b.setButtonText (tabNames[i]);
        b.setClickingTogglesState (true);
        b.setRadioGroupId (1001);
        b.setColour (juce::TextButton::buttonOnColourId, tabColours[i]);
        b.setColour (juce::TextButton::textColourOffId, tabColours[i]);
        b.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        b.onClick = [this, i]
        {
            page = (int) i;
            updatePageVisibility();
            resized();
            repaint();
        };
        addAndMakeVisible (b);
    }
    tabButtons[0].setToggleState (true, juce::dontSendNotification);

    addAndMakeVisible (spectrum);

    presetBox.setTextWhenNothingSelected ("PRESETS");
    presetBox.setTextWhenNoChoicesAvailable ("(aucun preset)");
    presetBox.setColour (juce::ComboBox::textColourId, text);
    presetBox.setColour (juce::ComboBox::arrowColourId, cyan);
    addAndMakeVisible (presetBox);

    nameEditor.setFont (mono (12.5f));
    nameEditor.setTextToShowWhenEmpty ("nom du preset", dim);
    nameEditor.setMultiLine (false);
    nameEditor.setReturnKeyStartsNewLine (false);
    addAndMakeVisible (nameEditor);

    for (auto* b : { &prevButton, &nextButton })
    {
        b->setColour (juce::TextButton::buttonOnColourId, cyan);
        b->setColour (juce::TextButton::textColourOffId, cyan);
        addAndMakeVisible (b);
    }
    saveButton.setColour (juce::TextButton::buttonOnColourId, teal);
    saveButton.setColour (juce::TextButton::textColourOffId, teal);
    deleteButton.setColour (juce::TextButton::buttonOnColourId, violet);
    deleteButton.setColour (juce::TextButton::textColourOffId, violet);
    addAndMakeVisible (saveButton);
    addAndMakeVisible (deleteButton);

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        const int numFactory = MonSyntheProcessor::getNumFactoryPresets();

        if (id >= 1 && id <= numFactory)                    // preset d'usine
        {
            proc.loadFactoryPreset (id - 1);
            nameEditor.setText (MonSyntheProcessor::getFactoryPresetName (id - 1), false);
        }
        else if (id > numFactory)                           // preset de l'utilisateur
        {
            const auto name = presetBox.getText();
            if (name.isNotEmpty() && proc.loadPreset (name))
                nameEditor.setText (name, false);
        }
    };

    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (1); };

    saveButton.onClick = [this]
    {
        const auto name = nameEditor.getText().trim();
        if (name.isEmpty())
        {
            nameEditor.grabKeyboardFocus();
            return;
        }
        if (proc.savePreset (name))
            refreshPresetList (juce::File::createLegalFileName (name));
    };

    deleteButton.onClick = [this]
    {
        if (presetBox.getSelectedId() <= MonSyntheProcessor::getNumFactoryPresets())   // pas de suppression des presets d'usine
            return;

        if (deleteArmed <= 0)                 // 1er clic : confirmation demandée
        {
            deleteArmed = 90;
            deleteButton.setButtonText ("SUR ?");
        }
        else                                  // 2e clic : suppression
        {
            proc.deletePreset (presetBox.getText());
            deleteArmed = 0;
            deleteButton.setButtonText ("SUPPR");
            refreshPresetList();
        }
    };

    refreshPresetList();
    updatePageVisibility();

    // Fenêtre redimensionnable en gardant le format 16:9
    setResizable (true, true);
    setResizeLimits (1024, 576, 1920, 1080);
    getConstrainer()->setFixedAspectRatio ((double) baseWidth / (double) baseHeight);
    setSize (baseWidth, baseHeight);          // en dernier : resized() a besoin que tout soit créé

    startTimerHz (30);
}

MonSyntheEditor::~MonSyntheEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

//==============================================================================
// Import de wavetables (.wav) : bouton ou glisser-déposer
//==============================================================================
void MonSyntheEditor::importWavetable (int slot)
{
    chooser = std::make_unique<juce::FileChooser> ("Choisir une wavetable (.wav)", juce::File(), "*.wav;*.aif;*.aiff;*.flac");

    juce::Component::SafePointer<MonSyntheEditor> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [safe, slot] (const juce::FileChooser& fc)
                          {
                              const auto file = fc.getResult();
                              if (safe != nullptr && file != juce::File())
                                  safe->loadWavetableFile (slot, file);
                          });
}

void MonSyntheEditor::loadWavetableFile (int slot, const juce::File& file)
{
    juce::MouseCursor::showWaitCursor();
    juce::String error;
    const bool ok = proc.loadUserWavetable (slot, file, error);
    juce::MouseCursor::hideWaitCursor();

    if (ok)
    {
        // L'oscillateur concerné passe sur la table importée (USER 1 pour A, USER 2 pour B)
        if (auto* prm = proc.apvts.getParameter (slot == 0 ? "tableA" : "tableB"))
            prm->setValueNotifyingHost (prm->convertTo0to1 ((float) (WavetableBank::numTables + slot)));

        statusMessage = "WAV charge : " + file.getFileName();
    }
    else
    {
        statusMessage = error;
    }

    statusFrames = 150;          // affiché environ 5 secondes
    lastTable = -1;
    repaint (scopeArea);
}

bool MonSyntheEditor::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& f : files)
        if (f.endsWithIgnoreCase (".wav") || f.endsWithIgnoreCase (".aif")
            || f.endsWithIgnoreCase (".aiff") || f.endsWithIgnoreCase (".flac"))
            return true;

    return false;
}

void MonSyntheEditor::filesDropped (const juce::StringArray& files, int x, int y)
{
    if (files.isEmpty())
        return;

    loadWavetableFile (oscBArea.contains (x, y) ? 1 : 0, juce::File (files[0]));
}

void MonSyntheEditor::refreshPresetList (const juce::String& select)
{
    presetBox.clear (juce::dontSendNotification);

    // Presets d'usine, classés par catégorie
    const int numFactory = MonSyntheProcessor::getNumFactoryPresets();
    for (const char* category : { "INIT", "BASSES", "LEADS", "PLUCKS", "CLAVIERS", "PADS", "TEXTURES" })
    {
        bool headingAdded = false;
        for (int i = 0; i < numFactory; ++i)
        {
            if (MonSyntheProcessor::getFactoryPresetCategory (i) != category)
                continue;

            if (! headingAdded)
            {
                presetBox.addSectionHeading (category);
                headingAdded = true;
            }
            presetBox.addItem (MonSyntheProcessor::getFactoryPresetName (i), i + 1);
        }
    }

    // Presets de l'utilisateur
    const auto names = proc.getPresetNames();
    if (names.size() > 0)
    {
        presetBox.addSeparator();
        presetBox.addSectionHeading ("MES PRESETS");
        for (int i = 0; i < names.size(); ++i)
            presetBox.addItem (names[i], numFactory + 1 + i);
    }

    const int index = names.indexOf (select);
    if (index >= 0)
        presetBox.setSelectedId (numFactory + 1 + index, juce::dontSendNotification);
}

void MonSyntheEditor::stepPreset (int delta)
{
    const int n = presetBox.getNumItems();
    if (n == 0)
        return;

    int index = presetBox.getSelectedItemIndex();
    index = index < 0 ? (delta > 0 ? 0 : n - 1) : (index + delta + n) % n;
    presetBox.setSelectedItemIndex (index, juce::sendNotification);
}

void MonSyntheEditor::timerCallback()
{
    spectrum.update();

    if (page == pageSynth)
    {
        const float pos = *proc.apvts.getRawParameterValue ("position");
        const int table = (int) *proc.apvts.getRawParameterValue ("tableA");
        if (! juce::approximatelyEqual (pos, lastPos) || table != lastTable)
        {
            lastPos = pos;
            lastTable = table;
            repaint (scopeArea);
        }
    }

    if (page == pageSeq)
        grid.refresh();

    if (statusFrames > 0 && --statusFrames == 0)
    {
        statusMessage = {};
        repaint (scopeArea);
    }

    if (deleteArmed > 0 && --deleteArmed == 0)
        deleteButton.setButtonText ("SUPPR");
}

void MonSyntheEditor::placeKnob (Knob* k, juce::Rectangle<int> b)
{
    k->label.setBounds (b.removeFromTop (S (16)));
    k->slider.setBounds (b);
}

void MonSyntheEditor::placeCombo (juce::Label& label, juce::ComboBox& box, juce::Rectangle<int> b)
{
    label.setBounds (b.removeFromTop (S (16)));
    box.setBounds (b.withHeight (S (26)).translated (0, S (4)));
}

void MonSyntheEditor::layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content)
{
    if (items.empty())
        return;

    const int w = content.getWidth() / (int) items.size();
    for (auto* k : items)
        placeKnob (k, content.removeFromLeft (w));
}

void MonSyntheEditor::resized()
{
    if (waveKnob == nullptr)     // composants pas encore créés
        return;

    scale = (float) getWidth() / (float) baseWidth;
    laf.setScale (scale);
    nameEditor.setFont (Pal::mono (12.5f * scale));

    for (auto& k : knobs)
    {
        if (k->slider.getSliderStyle() == juce::Slider::LinearHorizontal)
            k->slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, S (56), S (16));
        else
            k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, S (64), S (16));
    }

    const int margin = S (14), gap = S (12);
    auto area = getLocalBounds().reduced (margin);
    auto header = area.removeFromTop (S (44));
    area.removeFromTop (S (14));

    // ------------------------------------------------------------ En-tête
    {
        auto h = header;
        h.removeFromLeft (S (130));                                         // logo (dessiné dans paint)

        const int tabsW = S (3 * 84 + 2 * 6);
        auto tabs = h.removeFromLeft (tabsW).withSizeKeepingCentre (tabsW, S (30));
        for (auto& b : tabButtons)
        {
            b.setBounds (tabs.removeFromLeft (S (84)));
            tabs.removeFromLeft (S (6));
        }

        h.removeFromLeft (S (16));
        spectrum.setBounds (h.removeFromLeft (S (170)).withSizeKeepingCentre (S (170), S (36)));

        const int barW = S (598);
        auto bar = h.removeFromRight (barW).withSizeKeepingCentre (barW, S (30));
        prevButton.setBounds (bar.removeFromLeft (S (28)));
        bar.removeFromLeft (S (4));
        presetBox.setBounds (bar.removeFromLeft (S (250)));
        bar.removeFromLeft (S (4));
        nextButton.setBounds (bar.removeFromLeft (S (28)));
        bar.removeFromLeft (S (10));
        nameEditor.setBounds (bar.removeFromLeft (S (150)));
        bar.removeFromLeft (S (6));
        saveButton.setBounds (bar.removeFromLeft (S (56)));
        bar.removeFromLeft (S (6));
        deleteButton.setBounds (bar.removeFromLeft (S (56)));
    }

    panels.clear();
    auto addPanel = [this] (int pg, juce::Rectangle<int> r, const char* title, juce::Colour c)
    {
        panels.push_back ({ pg, r, title, c });
    };
    auto contentOf = [this] (juce::Rectangle<int> r) { return r.withTrimmedTop (S (32)).reduced (S (10), S (6)); };

    // ------------------------------------------------------------ Page SYNTH : tout sur un seul écran
    {
        const auto rows = splitCol (area, { 240.0f, 184.0f, 184.0f }, gap);
        const auto a = splitRow (rows[0], { 6.0f, 6.0f, 3.0f }, gap);
        const auto b = splitRow (rows[1], { 5.0f, 4.0f, 4.0f, 5.0f }, gap);
        const auto c = splitRow (rows[2], { 4.0f, 4.0f, 3.0f, 3.0f, 3.0f }, gap);

        addPanel (pageSynth, a[0], "OSC A / WAVETABLE", Pal::cyan);
        addPanel (pageSynth, a[1], "OSC B + SUB",       Pal::azure);
        addPanel (pageSynth, a[2], "UNISON",            Pal::blue);
        addPanel (pageSynth, b[0], "FILTRE",            Pal::teal);
        addPanel (pageSynth, b[1], "FILTRE ENV",        Pal::teal);
        addPanel (pageSynth, b[2], "AMP ENV",           Pal::indigo);
        addPanel (pageSynth, b[3], "MOD ENV",           Pal::violet);
        addPanel (pageSynth, c[0], "LFO 1",             Pal::violet);
        addPanel (pageSynth, c[1], "JEU",               Pal::cyan);
        addPanel (pageSynth, c[2], "CHORUS",            Pal::azure);
        addPanel (pageSynth, c[3], "DELAY",             Pal::blue);
        addPanel (pageSynth, c[4], "REVERB",            Pal::indigo);

        oscAArea = a[0];
        oscBArea = a[1];

        // OSC A : écran de forme d'onde + table + import + position
        auto ca = contentOf (a[0]);
        scopeArea = ca.removeFromLeft (juce::roundToInt ((float) ca.getWidth() * 0.64f));
        ca.removeFromLeft (S (12));
        placeCombo (tableALabel, tableABox, ca.removeFromTop (S (50)));
        importAButton.setBounds (ca.removeFromTop (S (30)).reduced (0, S (2)));
        placeKnob (waveKnob, ca);

        // OSC B : table + import + 5 réglages
        auto cb = contentOf (a[1]);
        const auto comboSlot = cb.removeFromLeft (cb.getWidth() / 6).reduced (S (3), 0);
        placeCombo (tableBLabel, tableBBox, comboSlot.translated (0, S (4)));
        importBButton.setBounds (comboSlot.getX(), comboSlot.getY() + S (58), comboSlot.getWidth(), S (26));
        layoutRow (oscBKnobs, cb);

        layoutRow (unisonKnobs, contentOf (a[2]));

        // Filtre : type + 4 réglages
        auto cf = contentOf (b[0]);
        placeCombo (filterTypeLabel, filterTypeBox, cf.removeFromLeft (cf.getWidth() / 5).reduced (S (3), 0).translated (0, S (12)));
        layoutRow (filterKnobs, cf);

        layoutRow (fltEnvKnobs, contentOf (b[1]));
        layoutRow (ampKnobs, contentOf (b[2]));
        layoutRow (modKnobs, contentOf (b[3]));

        // LFO : forme + 2 réglages
        auto cl = contentOf (c[0]);
        const int lfoSlot = cl.getWidth() / 4;
        placeCombo (lfoShapeLabel, lfoShapeBox, cl.removeFromLeft (lfoSlot).reduced (S (3), 0).translated (0, S (12)));
        placeCombo (lfoSyncLabel, lfoSyncBox, cl.removeFromLeft (lfoSlot).reduced (S (3), 0).translated (0, S (12)));
        layoutRow (lfoKnobs, cl);

        // Jeu : mono/poly + 3 réglages
        auto cj = contentOf (c[1]);
        const int slotW = cj.getWidth() / 4;
        auto first = cj.removeFromLeft (slotW);
        monoButton.setBounds (first.withSizeKeepingCentre (slotW - S (10), S (28)).translated (0, S (8)));
        layoutRow (jeuKnobs, cj);

        layoutRow (chorusKnobs, contentOf (c[2]));
        layoutRow (delayKnobs, contentOf (c[3]));
        layoutRow (reverbKnobs, contentOf (c[4]));
    }

    // ------------------------------------------------------------ Page MOD
    {
        const auto rows = splitCol (area, { 54.0f, 46.0f }, gap);
        const auto low = splitRow (rows[1], { 4.0f, 4.0f, 4.0f }, gap);
        addPanel (pageMod, rows[0], "MATRICE DE MODULATION", Pal::indigo);
        addPanel (pageMod, low[0],  "LFO 2",                 Pal::violet);
        addPanel (pageMod, low[1],  "MACROS",                Pal::cyan);
        addPanel (pageMod, low[2],  "VELOCITE",              Pal::teal);

        auto mc = contentOf (rows[0]);
        auto hdr = mc.removeFromTop (S (18));
        const int comboW = juce::roundToInt ((float) mc.getWidth() * 0.30f);
        const int arrowW = S (44);
        const int amtGap = S (14);
        mixHdrSrc.setBounds (hdr.removeFromLeft (comboW));
        hdr.removeFromLeft (arrowW);
        mixHdrDst.setBounds (hdr.removeFromLeft (comboW));
        hdr.removeFromLeft (amtGap);
        mixHdrAmt.setBounds (hdr);

        const int rowH = mc.getHeight() / 6;
        for (size_t i = 0; i < 6; ++i)
        {
            auto row = mc.removeFromTop (rowH).withSizeKeepingCentre (mc.getWidth(), S (30));
            mixSrcBox[i].setBounds (row.removeFromLeft (comboW));
            row.removeFromLeft (arrowW);
            mixDstBox[i].setBounds (row.removeFromLeft (comboW));
            row.removeFromLeft (amtGap);
            mixAmt[i]->label.setBounds (juce::Rectangle<int>());
            mixAmt[i]->slider.setBounds (row);
        }

        auto cl2 = contentOf (low[0]);
        const int lfo2Slot = cl2.getWidth() / 3;
        placeCombo (lfo2ShapeLabel, lfo2ShapeBox, cl2.removeFromLeft (lfo2Slot).reduced (S (3), 0).translated (0, S (12)));
        placeCombo (lfo2SyncLabel, lfo2SyncBox, cl2.removeFromLeft (lfo2Slot).reduced (S (3), 0).translated (0, S (12)));
        layoutRow (lfo2Knobs, cl2);

        layoutRow (macroKnobs, contentOf (low[1]));
        layoutRow (velKnobs, contentOf (low[2]));
    }

    // ------------------------------------------------------------ Page SEQ
    {
        const juce::Rectangle<int> seqArea (area.getX(), area.getY(), area.getWidth(),
                                            juce::jmin (area.getHeight(), S (400)));
        addPanel (pageSeq, seqArea, "SEQUENCEUR", Pal::teal);

        auto sc = contentOf (seqArea);
        auto strip = sc.removeFromTop (S (50));
        sc.removeFromTop (S (8));
        grid.setBounds (sc);

        seqButton.setBounds (strip.removeFromLeft (S (110)).withTrimmedTop (S (20)).withHeight (S (28)));
        strip.removeFromLeft (S (14));
        placeCombo (barsLabel, barsBox, strip.removeFromLeft (S (150)));
        strip.removeFromLeft (S (14));
        placeCombo (rateLabel, rateBox, strip.removeFromLeft (S (120)));
        strip.removeFromLeft (S (24));
        placeKnob (gateKnob, strip.removeFromLeft (S (280)));
        strip.removeFromLeft (S (16));
        placeKnob (tempoKnob, strip.removeFromLeft (S (300)));
    }
}

//==============================================================================
void MonSyntheEditor::drawPanel (juce::Graphics& g, const Panel& panel)
{
    const auto r = panel.area.toFloat();
    const auto colour = panel.colour;
    const float c = 9.0f * scale;       // coin coupé en haut à gauche

    juce::Path p;
    p.startNewSubPath (r.getX() + c, r.getY());
    p.lineTo (r.getRight(), r.getY());
    p.lineTo (r.getRight(), r.getBottom());
    p.lineTo (r.getX(), r.getBottom());
    p.lineTo (r.getX(), r.getY() + c);
    p.closeSubPath();

    g.setColour (Pal::panel);
    g.fillPath (p);
    g.setColour (Pal::line.brighter (0.4f));
    g.strokePath (p, juce::PathStrokeType (1.0f));

    // Trait d'accent sur le bord haut
    g.setColour (colour);
    g.fillRect (r.getX() + c, r.getY(), 34.0f * scale, 2.0f * scale);

    // Titre
    g.setColour (colour);
    g.fillRect (r.getX() + 14.0f * scale, r.getY() + 11.0f * scale, 6.0f * scale, 6.0f * scale);
    g.setColour (Pal::text);
    g.setFont (Pal::mono (11.0f * scale, true));
    g.drawText (panel.title,
                juce::Rectangle<float> (r.getX() + 26.0f * scale, r.getY() + 6.0f * scale,
                                        r.getWidth() - 40.0f * scale, 16.0f * scale),
                juce::Justification::centredLeft, false);

    // Filet sous le titre
    g.setColour (Pal::line.brighter (0.3f));
    g.drawLine (r.getX() + 14.0f * scale, r.getY() + 28.0f * scale,
                r.getRight() - 14.0f * scale, r.getY() + 28.0f * scale, 1.0f);

    // Équerre en bas à droite
    const float k = 8.0f * scale;
    g.setColour (colour.withAlpha (0.8f));
    g.drawLine (r.getRight() - k, r.getBottom() - 0.5f, r.getRight(), r.getBottom() - 0.5f, 1.5f);
    g.drawLine (r.getRight() - 0.5f, r.getBottom() - k, r.getRight() - 0.5f, r.getBottom(), 1.5f);
}

void MonSyntheEditor::drawLogo (juce::Graphics& g, juce::Rectangle<int> area)
{
    const juce::Point<float> c ((float) area.getX() + (float) S (20), (float) area.getCentreY());
    const float r = (float) S (17);

    juce::Path hex;
    for (int i = 0; i < 6; ++i)
    {
        const float a = juce::MathConstants<float>::pi / 3.0f * (float) i + juce::MathConstants<float>::pi / 6.0f;
        const juce::Point<float> pt (c.x + r * std::cos (a), c.y + r * std::sin (a));
        if (i == 0) hex.startNewSubPath (pt);
        else        hex.lineTo (pt);
    }
    hex.closeSubPath();
    g.setColour (Pal::cyan);
    g.strokePath (hex, juce::PathStrokeType (1.6f * scale));

    juce::Path chevron;
    chevron.startNewSubPath (c.x - r * 0.28f, c.y - r * 0.42f);
    chevron.lineTo (c.x + r * 0.28f, c.y);
    chevron.lineTo (c.x - r * 0.28f, c.y + r * 0.42f);
    g.setColour (Pal::azure);
    g.strokePath (chevron, juce::PathStrokeType (2.2f * scale));

    g.setColour (Pal::text);
    g.setFont (Pal::mono (24.0f * scale, true));
    g.drawText ("MS1", area.getX() + S (44), area.getY(), S (80), area.getHeight(),
                juce::Justification::centredLeft, false);
}

void MonSyntheEditor::drawScope (juce::Graphics& g, juce::Rectangle<float> r)
{
    const auto& bank = proc.getBank();
    const float pos = *proc.apvts.getRawParameterValue ("position");
    const int tab = (int) *proc.apvts.getRawParameterValue ("tableA");

    g.setColour (juce::Colour (0xff070a12));
    g.fillRect (r);

    {
        juce::Graphics::ScopedSaveState save (g);
        g.reduceClipRegion (r.toNearestInt());

        // Grille
        g.setColour (Pal::cyan.withAlpha (0.06f));
        for (int i = 1; i < 8; ++i)
            g.drawVerticalLine ((int) (r.getX() + r.getWidth() * (float) i / 8.0f), r.getY(), r.getBottom());
        for (int i = 1; i < 4; ++i)
            g.drawHorizontalLine ((int) (r.getY() + r.getHeight() * (float) i / 4.0f), r.getX(), r.getRight());
        g.setColour (Pal::cyan.withAlpha (0.20f));
        g.drawHorizontalLine ((int) r.getCentreY(), r.getX(), r.getRight());

        const int n = juce::jmax (2, (int) r.getWidth());
        const float amp = r.getHeight() * 0.5f - 12.0f * scale;

        auto makePath = [&] (float pp)
        {
            juce::Path path;
            for (int i = 0; i < n; ++i)
            {
                const double phase = (double) i / (double) (n - 1);
                const float v = bank.getSample (tab, pp, 0, phase);
                const float px = r.getX() + r.getWidth() * (float) i / (float) (n - 1);
                const float py = r.getCentreY() - v * amp;
                if (i == 0) path.startNewSubPath (px, py);
                else        path.lineTo (px, py);
            }
            return path;
        };

        // Les 8 formes de la table en filigrane
        g.setColour (Pal::indigo.withAlpha (0.20f));
        for (int f = 0; f < WavetableBank::numFrames; ++f)
            g.strokePath (makePath ((float) f / (float) (WavetableBank::numFrames - 1)), juce::PathStrokeType (1.0f));

        // Forme courante
        const auto current = makePath (pos);
        g.setColour (Pal::cyan.withAlpha (0.14f));
        g.strokePath (current, juce::PathStrokeType (5.0f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (Pal::cyan);
        g.strokePath (current, juce::PathStrokeType (1.8f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    g.setColour (Pal::line.brighter (0.5f));
    g.drawRect (r, 1.0f);

    juce::String tableLabel = WavetableBank::tableNames()[juce::jlimit (0, WavetableBank::totalTables - 1, tab)];
    if (tab >= WavetableBank::numTables)
        tableLabel += " : " + proc.getUserTableName (tab - WavetableBank::numTables);

    g.setColour (Pal::dim);
    g.setFont (Pal::mono (10.0f * scale, true));
    g.drawText (tableLabel + "   POS " + juce::String (pos, 2),
                juce::Rectangle<float> (r.getX() + 8.0f * scale, r.getY() + 5.0f * scale, r.getWidth() - 16.0f * scale, 14.0f * scale),
                juce::Justification::centredLeft, false);

    if (statusMessage.isNotEmpty())
    {
        g.setColour (Pal::teal);
        g.drawText (statusMessage,
                    juce::Rectangle<float> (r.getX() + 8.0f * scale, r.getBottom() - 20.0f * scale, r.getWidth() - 16.0f * scale, 14.0f * scale),
                    juce::Justification::centredLeft, false);
    }
}

void MonSyntheEditor::paint (juce::Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    g.setGradientFill (juce::ColourGradient (Pal::bg.brighter (0.07f), 0.0f, 0.0f, Pal::bg, 0.0f, h, false));
    g.fillAll();

    // Repères en croix, discrets
    const int step = juce::jmax (16, S (32));
    g.setColour (Pal::cyan.withAlpha (0.07f));
    for (int x = step; x < getWidth(); x += step)
        for (int y = step; y < getHeight(); y += step)
        {
            g.drawHorizontalLine (y, (float) (x - 2), (float) (x + 3));
            g.drawVerticalLine (x, (float) (y - 2), (float) (y + 3));
        }

    // En-tête
    drawLogo (g, { S (14), S (14), S (130), S (44) });

    g.setGradientFill (juce::ColourGradient (Pal::cyan, (float) S (14), 0.0f, Pal::indigo, w - (float) S (14), 0.0f, false));
    g.fillRect ((float) S (14), (float) (S (14) + S (44) + S (6)), w - 2.0f * (float) S (14), 1.5f * scale);

    for (const auto& panel : panels)
        if (panel.page == page)
            drawPanel (g, panel);

    if (page == pageSynth)
        drawScope (g, scopeArea.toFloat());

    if (page == pageMod)
    {
        g.setColour (Pal::indigo);
        g.setFont (Pal::mono (14.0f * scale, true));
        for (size_t i = 0; i < mixSrcBox.size(); ++i)
        {
            const int x0 = mixSrcBox[i].getRight();
            const int x1 = mixDstBox[i].getX();
            g.drawText ("->", x0, mixSrcBox[i].getY(), x1 - x0, mixSrcBox[i].getHeight(),
                        juce::Justification::centred, false);
        }
    }
}
