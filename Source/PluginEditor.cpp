#include "PluginEditor.h"

//==============================================================================
// Look & feel
//==============================================================================
CyberLookAndFeel::CyberLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId, Cyber::panel);
    setColour (juce::PopupMenu::textColourId, Cyber::grey);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Cyber::magenta.withAlpha (0.35f));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff05050a));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);

    setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff05050a));
    setColour (juce::TextEditor::textColourId, Cyber::cyan);
    setColour (juce::TextEditor::outlineColourId, Cyber::cyan.withAlpha (0.6f));
    setColour (juce::TextEditor::focusedOutlineColourId, Cyber::magenta);
    setColour (juce::TextEditor::highlightColourId, Cyber::magenta.withAlpha (0.4f));
    setColour (juce::CaretComponent::caretColourId, Cyber::magenta);
}

void CyberLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float cx = bounds.getCentreX();
    const float cy = bounds.getCentreY();
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float arcR = radius - 2.0f;

    juce::Path track;
    track.addCentredArc (cx, cy, arcR, arcR, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (accent.withAlpha (0.15f));
    g.strokePath (track, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path value;
    value.addCentredArc (cx, cy, arcR, arcR, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent.withAlpha (0.22f));
    g.strokePath (value, juce::PathStrokeType (8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent);
    g.strokePath (value, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float bodyR = radius - 11.0f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2a2a44), cx, cy - bodyR,
                                             juce::Colour (0xff0b0b16), cx, cy + bodyR, false));
    g.fillEllipse (cx - bodyR, cy - bodyR, bodyR * 2.0f, bodyR * 2.0f);
    g.setColour (accent.withAlpha (0.55f));
    g.drawEllipse (cx - bodyR, cy - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.2f);

    juce::Path pointer;
    pointer.addRectangle (-1.5f, -bodyR + 2.0f, 3.0f, bodyR * 0.5f);
    g.setColour (accent.brighter (0.4f));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (cx, cy));
}

void CyberLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float, float, juce::Slider::SliderStyle,
                                         juce::Slider& slider)
{
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const float left = (float) x;
    const float cy = (float) y + (float) height * 0.5f;
    const float fillW = juce::jmax (0.0f, sliderPos - left);

    g.setColour (accent.withAlpha (0.15f));
    g.fillRoundedRectangle (left, cy - 2.0f, (float) width, 4.0f, 2.0f);
    g.setColour (accent.withAlpha (0.25f));
    g.fillRoundedRectangle (left, cy - 4.0f, fillW, 8.0f, 4.0f);
    g.setColour (accent);
    g.fillRoundedRectangle (left, cy - 2.0f, fillW, 4.0f, 2.0f);

    g.setColour (accent.brighter (0.4f));
    g.fillEllipse (sliderPos - 6.0f, cy - 6.0f, 12.0f, 12.0f);
}

void CyberLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int,
                                     juce::ComboBox& box)
{
    const auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) w, (float) h).reduced (1.0f);
    const auto accent = box.findColour (juce::ComboBox::arrowColourId);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (r, 3.0f);
    g.setColour (accent.withAlpha (0.8f));
    g.drawRoundedRectangle (r, 3.0f, 1.2f);

    const float ax = r.getRight() - 12.0f;
    const float ay = r.getCentreY();
    juce::Path arrow;
    arrow.addTriangle (ax - 4.0f, ay - 2.0f, ax + 4.0f, ay - 2.0f, ax, ay + 3.0f);
    g.setColour (accent);
    g.fillPath (arrow);
}

void CyberLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                             bool highlighted, bool down)
{
    const auto r = button.getLocalBounds().toFloat().reduced (1.0f);
    const auto accent = button.findColour (juce::TextButton::buttonOnColourId);

    float a = button.getToggleState() ? 0.35f : 0.0f;
    if (down)             a = 0.55f;
    else if (highlighted) a = juce::jmax (a, 0.2f);

    g.setColour (accent.withAlpha (a));
    g.fillRoundedRectangle (r, 3.0f);
    g.setColour (accent.withAlpha (0.85f));
    g.drawRoundedRectangle (r, 3.0f, 1.2f);
}

//==============================================================================
// Grille du séquenceur
//==============================================================================
juce::Rectangle<float> StepGrid::cellBounds (int step) const
{
    const float labelW = 22.0f;
    const float cw = ((float) getWidth() - labelW) / (float) StepSequencerData::stepsPerBar;
    const float ch = (float) getHeight() / (float) StepSequencerData::maxBars;
    const int row = step / StepSequencerData::stepsPerBar;
    const int col = step % StepSequencerData::stepsPerBar;
    return { labelW + (float) col * cw + 1.5f, (float) row * ch + 1.5f, cw - 3.0f, ch - 3.0f };
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

    // Numéros de ligne
    g.setFont (Cyber::mono (12.0f, true));
    for (int row = 0; row < StepSequencerData::maxBars; ++row)
    {
        g.setColour (accent.withAlpha (row < bars ? 0.9f : 0.3f));
        const auto first = cellBounds (row * StepSequencerData::stepsPerBar);
        g.drawText (juce::String (row + 1), juce::Rectangle<float> (0.0f, first.getY(), 20.0f, first.getHeight()),
                    juce::Justification::centred, false);
    }

    for (int step = 0; step < StepSequencerData::maxSteps; ++step)
    {
        const auto r = cellBounds (step);
        const int row = step / StepSequencerData::stepsPerBar;
        const int col = step % StepSequencerData::stepsPerBar;
        const float dim = row < bars ? 1.0f : 0.3f;
        const bool on = proc.sequencer.on[(size_t) step];

        g.setColour (((col / 4) % 2 == 0 ? juce::Colour (0xff17172c) : juce::Colour (0xff101020)).withMultipliedAlpha (dim));
        g.fillRoundedRectangle (r, 3.0f);

        if (on)
        {
            g.setColour (accent.withAlpha (0.25f * dim));
            g.drawRoundedRectangle (r.expanded (1.5f), 4.0f, 2.5f);
            g.setColour (accent.withAlpha (0.9f * dim));
            g.fillRoundedRectangle (r, 3.0f);

            g.setColour (juce::Colour (0xff05050a));
            g.setFont (Cyber::mono (10.0f, true));
            g.drawText (juce::MidiMessage::getMidiNoteName (proc.sequencer.note[(size_t) step], true, true, 3),
                        r, juce::Justification::centred, false);
        }
        else
        {
            g.setColour (accent.withAlpha (0.12f * dim));
            g.drawRoundedRectangle (r, 3.0f, 1.0f);
        }

        if (seqOn && step == playing && row < bars)
        {
            g.setColour (juce::Colours::white.withAlpha (0.18f));
            g.fillRoundedRectangle (r, 3.0f);
            g.setColour (juce::Colours::white.withAlpha (0.9f));
            g.drawRoundedRectangle (r, 3.0f, 1.5f);
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
MonSyntheEditor::Knob* MonSyntheEditor::makeKnob (const juce::String& paramId, const juce::String& name,
                                                  juce::Colour colour, bool linear)
{
    auto k = std::make_unique<Knob>();

    if (linear)
    {
        k->slider.setSliderStyle (juce::Slider::LinearHorizontal);
        k->slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 16);
    }
    else
    {
        k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 16);
    }

    k->slider.setColour (juce::Slider::rotarySliderFillColourId, colour);
    k->slider.setColour (juce::Slider::textBoxTextColourId, colour.brighter (0.4f));

    k->label.setText (name, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setColour (juce::Label::textColourId, Cyber::grey);

    addAndMakeVisible (k->slider);
    addAndMakeVisible (k->label);
    k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, paramId, k->slider);

    knobs.push_back (std::move (k));
    return knobs.back().get();
}

void MonSyntheEditor::setupCombo (juce::ComboBox& box, juce::Label& label, const juce::String& title,
                                  juce::Colour colour, const juce::StringArray& items)
{
    box.addItemList (items, 1);
    box.setJustificationType (juce::Justification::centred);
    box.setColour (juce::ComboBox::textColourId, colour);
    box.setColour (juce::ComboBox::arrowColourId, colour);
    addAndMakeVisible (box);

    label.setText (title, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, Cyber::grey);
    addAndMakeVisible (label);
}

MonSyntheEditor::MonSyntheEditor (MonSyntheProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p), grid (p, Cyber::yellow)
{
    setLookAndFeel (&laf);

    waveKnob = makeKnob ("position", "POSITION", Cyber::cyan);

    unisonKnobs = { makeKnob ("unison", "VOIX", Cyber::blue),
                    makeKnob ("detune", "DESACCORD", Cyber::blue),
                    makeKnob ("spread", "STEREO", Cyber::blue) };

    filterKnobs = { makeKnob ("cutoff", "CUTOFF", Cyber::orange),
                    makeKnob ("resonance", "RESO", Cyber::orange) };

    lfoKnobs = { makeKnob ("lfoRate", "VITESSE", Cyber::magenta),
                 makeKnob ("lfoDepth", "INTENSITE", Cyber::magenta) };

    modKnobs = { makeKnob ("modAttack", "ATTACK", Cyber::violet),
                 makeKnob ("modDecay", "DECAY", Cyber::violet),
                 makeKnob ("modSustain", "SUSTAIN", Cyber::violet),
                 makeKnob ("modRelease", "RELEASE", Cyber::violet),
                 makeKnob ("modAmount", "QUANTITE", Cyber::violet) };

    ampKnobs = { makeKnob ("attack", "ATTACK", Cyber::lime),
                 makeKnob ("decay", "DECAY", Cyber::lime),
                 makeKnob ("sustain", "SUSTAIN", Cyber::lime),
                 makeKnob ("release", "RELEASE", Cyber::lime) };

    // LFO : forme
    setupCombo (lfoShapeBox, lfoShapeLabel, "FORME", Cyber::magenta,
                juce::StringArray { "Sinus", "Triangle", "Saw", "Carre" });
    lfoShapeAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "lfoShape", lfoShapeBox);

    // Séquenceur
    seqButton.setClickingTogglesState (true);
    seqButton.setColour (juce::TextButton::buttonOnColourId, Cyber::yellow);
    seqButton.setColour (juce::TextButton::textColourOffId, Cyber::yellow);
    seqButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    seqButton.onStateChange = [this] { seqButton.setButtonText (seqButton.getToggleState() ? "SEQ ON" : "SEQ OFF"); };
    addAndMakeVisible (seqButton);
    seqButtonAttachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, "seqOn", seqButton);
    seqButton.setButtonText (seqButton.getToggleState() ? "SEQ ON" : "SEQ OFF");

    setupCombo (barsBox, barsLabel, "LONGUEUR", Cyber::yellow,
                juce::StringArray { "1 mesure", "2 mesures", "3 mesures", "4 mesures" });
    barsAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqBars", barsBox);

    setupCombo (rateBox, rateLabel, "VITESSE", Cyber::yellow, juce::StringArray { "1/8", "1/16", "1/32" });
    rateAttachment = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, "seqRate", rateBox);

    gateKnob = makeKnob ("seqGate", "GATE", Cyber::yellow, true);
    tempoKnob = makeKnob ("seqTempo", "TEMPO (hors lecture)", Cyber::yellow, true);

    addAndMakeVisible (grid);

    // Presets
    presetBox.setTextWhenNothingSelected ("PRESETS");
    presetBox.setTextWhenNoChoicesAvailable ("(aucun preset)");
    presetBox.setColour (juce::ComboBox::textColourId, Cyber::cyan);
    presetBox.setColour (juce::ComboBox::arrowColourId, Cyber::cyan);
    addAndMakeVisible (presetBox);

    nameEditor.setFont (Cyber::mono (13.0f));
    nameEditor.setTextToShowWhenEmpty ("nom du preset", Cyber::grey);
    nameEditor.setMultiLine (false);
    nameEditor.setReturnKeyStartsNewLine (false);
    addAndMakeVisible (nameEditor);

    for (auto* b : { &prevButton, &nextButton })
    {
        b->setColour (juce::TextButton::buttonOnColourId, Cyber::cyan);
        b->setColour (juce::TextButton::textColourOffId, Cyber::cyan);
        addAndMakeVisible (b);
    }
    saveButton.setColour (juce::TextButton::buttonOnColourId, Cyber::lime);
    saveButton.setColour (juce::TextButton::textColourOffId, Cyber::lime);
    deleteButton.setColour (juce::TextButton::buttonOnColourId, Cyber::magenta);
    deleteButton.setColour (juce::TextButton::textColourOffId, Cyber::magenta);
    addAndMakeVisible (saveButton);
    addAndMakeVisible (deleteButton);

    presetBox.onChange = [this]
    {
        const auto name = presetBox.getText();
        if (name.isNotEmpty() && proc.loadPreset (name))
            nameEditor.setText (name, false);
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
        if (presetBox.getSelectedId() == 0)
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
    setSize (900, 652);      // en dernier : resized() a besoin que tout soit créé
    startTimerHz (30);
}

MonSyntheEditor::~MonSyntheEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void MonSyntheEditor::refreshPresetList (const juce::String& select)
{
    presetBox.clear (juce::dontSendNotification);

    const auto names = proc.getPresetNames();
    for (int i = 0; i < names.size(); ++i)
        presetBox.addItem (names[i], i + 1);

    const int index = names.indexOf (select);
    if (index >= 0)
        presetBox.setSelectedId (index + 1, juce::dontSendNotification);
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
    const float pos = *proc.apvts.getRawParameterValue ("position");
    if (! juce::approximatelyEqual (pos, lastPos))
    {
        lastPos = pos;
        repaint (scopeArea);
    }

    grid.refresh();

    if (deleteArmed > 0 && --deleteArmed == 0)
        deleteButton.setButtonText ("SUPPR");
}

void MonSyntheEditor::placeKnob (Knob* k, juce::Rectangle<int> b)
{
    k->label.setBounds (b.removeFromTop (16));
    k->slider.setBounds (b);
}

void MonSyntheEditor::placeCombo (juce::Label& label, juce::ComboBox& box, juce::Rectangle<int> b)
{
    label.setBounds (b.removeFromTop (16));
    box.setBounds (b.withHeight (26));
}

void MonSyntheEditor::layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content)
{
    const int w = content.getWidth() / (int) items.size();
    for (auto* k : items)
        placeKnob (k, content.removeFromLeft (w));
}

void MonSyntheEditor::resized()
{
    if (waveKnob == nullptr)     // composants pas encore créés
        return;

    auto contentOf = [] (juce::Rectangle<int> section) { return section.withTrimmedTop (32).reduced (10, 6); };

    // En-tête : presets
    {
        juce::Rectangle<int> bar (270, 16, getWidth() - 270 - 14, 28);
        prevButton.setBounds (bar.removeFromLeft (28));
        bar.removeFromLeft (4);
        presetBox.setBounds (bar.removeFromLeft (210));
        bar.removeFromLeft (4);
        nextButton.setBounds (bar.removeFromLeft (28));
        bar.removeFromLeft (12);
        nameEditor.setBounds (bar.removeFromLeft (140));
        bar.removeFromLeft (6);
        saveButton.setBounds (bar.removeFromLeft (64));
        bar.removeFromLeft (6);
        deleteButton.setBounds (bar.removeFromLeft (64));
    }

    auto area = getLocalBounds().reduced (14);
    area.removeFromTop (60);                         // titre
    auto row1 = area.removeFromTop (176);
    area.removeFromTop (14);
    auto row2 = area.removeFromTop (148);
    area.removeFromTop (14);
    seqArea = area;

    // Ligne 1 : wavetable + unison + filtre
    waveArea = row1.removeFromLeft (400);
    row1.removeFromLeft (16);
    unisonArea = row1.removeFromLeft (250);
    row1.removeFromLeft (16);
    filterArea = row1;

    auto c = contentOf (waveArea);
    scopeArea = c.removeFromLeft (240);
    c.removeFromLeft (12);
    placeKnob (waveKnob, c);

    layoutRow (unisonKnobs, contentOf (unisonArea));
    layoutRow (filterKnobs, contentOf (filterArea));

    // Ligne 2 : LFO + enveloppe de modulation + enveloppe de volume
    lfoArea = row2.removeFromLeft (210);
    row2.removeFromLeft (16);
    modArea = row2.removeFromLeft (350);
    row2.removeFromLeft (16);
    ampArea = row2;

    auto lc = contentOf (lfoArea);
    const int w = lc.getWidth() / 3;
    placeCombo (lfoShapeLabel, lfoShapeBox, lc.removeFromLeft (w).reduced (3, 0).translated (0, 8));
    layoutRow (lfoKnobs, lc);

    layoutRow (modKnobs, contentOf (modArea));
    layoutRow (ampKnobs, contentOf (ampArea));

    // Séquenceur
    auto sc = contentOf (seqArea);
    auto strip = sc.removeFromTop (44);
    sc.removeFromTop (6);
    grid.setBounds (sc);

    seqButton.setBounds (strip.removeFromLeft (100).withTrimmedTop (16).withHeight (26));
    strip.removeFromLeft (12);
    placeCombo (barsLabel, barsBox, strip.removeFromLeft (130));
    strip.removeFromLeft (12);
    placeCombo (rateLabel, rateBox, strip.removeFromLeft (110));
    strip.removeFromLeft (20);
    placeKnob (gateKnob, strip.removeFromLeft (200));
    strip.removeFromLeft (16);
    placeKnob (tempoKnob, strip.removeFromLeft (220));
}

//==============================================================================
void MonSyntheEditor::drawPanel (juce::Graphics& g, juce::Rectangle<int> area,
                                 const juce::String& title, juce::Colour colour)
{
    const auto r = area.toFloat();
    const float c = 10.0f;     // coins coupés

    juce::Path p;
    p.startNewSubPath (r.getX() + c, r.getY());
    p.lineTo (r.getRight(), r.getY());
    p.lineTo (r.getRight(), r.getBottom() - c);
    p.lineTo (r.getRight() - c, r.getBottom());
    p.lineTo (r.getX(), r.getBottom());
    p.lineTo (r.getX(), r.getY() + c);
    p.closeSubPath();

    g.setColour (Cyber::panel);
    g.fillPath (p);
    g.setColour (colour.withAlpha (0.25f));
    g.strokePath (p, juce::PathStrokeType (4.0f));        // halo
    g.setColour (colour.withAlpha (0.75f));
    g.strokePath (p, juce::PathStrokeType (1.2f));

    g.setColour (colour);
    g.fillRect (r.getX() + 14.0f, r.getY() + 10.0f, 4.0f, 12.0f);
    g.setFont (Cyber::mono (13.0f, true));
    g.drawText ("// " + title, juce::Rectangle<float> (r.getX() + 24.0f, r.getY() + 6.0f, r.getWidth() - 40.0f, 20.0f),
                juce::Justification::centredLeft, false);

    g.setColour (colour.withAlpha (0.3f));
    g.drawLine (r.getX() + 14.0f, r.getY() + 30.0f, r.getRight() - 14.0f, r.getY() + 30.0f, 1.0f);
}

void MonSyntheEditor::drawScope (juce::Graphics& g, juce::Rectangle<float> r)
{
    const auto& bank = proc.getBank();
    const float pos = *proc.apvts.getRawParameterValue ("position");

    g.setColour (juce::Colour (0xff05050a));
    g.fillRect (r);

    {
        juce::Graphics::ScopedSaveState save (g);
        g.reduceClipRegion (r.toNearestInt());

        g.setColour (Cyber::cyan.withAlpha (0.07f));
        for (int i = 1; i < 8; ++i)
            g.drawVerticalLine ((int) (r.getX() + r.getWidth() * (float) i / 8.0f), r.getY(), r.getBottom());
        for (int i = 1; i < 4; ++i)
            g.drawHorizontalLine ((int) (r.getY() + r.getHeight() * (float) i / 4.0f), r.getX(), r.getRight());
        g.setColour (Cyber::cyan.withAlpha (0.22f));
        g.drawHorizontalLine ((int) r.getCentreY(), r.getX(), r.getRight());

        const int n = juce::jmax (2, (int) r.getWidth());
        const float amp = r.getHeight() * 0.5f - 8.0f;

        auto makePath = [&] (float pp)
        {
            juce::Path path;
            for (int i = 0; i < n; ++i)
            {
                const double phase = (double) i / (double) (n - 1);
                const float v = bank.getSample (pp, 0, phase);
                const float px = r.getX() + r.getWidth() * (float) i / (float) (n - 1);
                const float py = r.getCentreY() - v * amp;
                if (i == 0) path.startNewSubPath (px, py);
                else        path.lineTo (px, py);
            }
            return path;
        };

        g.setColour (Cyber::violet.withAlpha (0.16f));
        for (int f = 0; f < WavetableBank::numFrames; ++f)
            g.strokePath (makePath ((float) f / (float) (WavetableBank::numFrames - 1)), juce::PathStrokeType (1.0f));

        const auto current = makePath (pos);
        g.setColour (Cyber::cyan.withAlpha (0.18f));
        g.strokePath (current, juce::PathStrokeType (7.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (Cyber::cyan);
        g.strokePath (current, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    g.setColour (Cyber::cyan.withAlpha (0.6f));
    g.drawRect (r, 1.0f);

    g.setColour (Cyber::cyan);
    g.setFont (Cyber::mono (11.0f, true));
    g.drawText ("WT POS " + juce::String (pos, 2),
                juce::Rectangle<float> (r.getX() + 6.0f, r.getY() + 4.0f, 120.0f, 14.0f),
                juce::Justification::centredLeft, false);
}

void MonSyntheEditor::paint (juce::Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff0b0b16), 0.0f, 0.0f,
                                             juce::Colour (0xff040408), 0.0f, h, false));
    g.fillAll();
    g.setColour (Cyber::cyan.withAlpha (0.035f));
    for (int x = 0; x < getWidth(); x += 30)
        g.drawVerticalLine (x, 0.0f, h);
    for (int y = 0; y < getHeight(); y += 30)
        g.drawHorizontalLine (y, 0.0f, w);

    // Titre (effet d'aberration chromatique)
    g.setFont (Cyber::mono (30.0f, true));
    g.setColour (Cyber::magenta.withAlpha (0.85f));
    g.drawText ("MS1", 17, 11, 250, 36, juce::Justification::centredLeft, false);
    g.setColour (Cyber::cyan);
    g.drawText ("MS1", 14, 10, 250, 36, juce::Justification::centredLeft, false);

    g.setGradientFill (juce::ColourGradient (Cyber::cyan, 14.0f, 0.0f, Cyber::magenta, w - 14.0f, 0.0f, false));
    g.fillRect (14.0f, 56.0f, w - 28.0f, 2.0f);

    drawPanel (g, waveArea,   "WAVETABLE",  Cyber::cyan);
    drawPanel (g, unisonArea, "UNISON",     Cyber::blue);
    drawPanel (g, filterArea, "FILTRE",     Cyber::orange);
    drawPanel (g, lfoArea,    "LFO",        Cyber::magenta);
    drawPanel (g, modArea,    "MOD ENV",    Cyber::violet);
    drawPanel (g, ampArea,    "AMP ENV",    Cyber::lime);
    drawPanel (g, seqArea,    "SEQUENCEUR", Cyber::yellow);

    drawScope (g, scopeArea.toFloat());
}
