#pragma once
#include "PluginProcessor.h"

//==============================================================================
// Palette cyberpunk
namespace Cyber
{
    inline const juce::Colour cyan    { 0xff00f0ff };
    inline const juce::Colour magenta { 0xffff2bd6 };
    inline const juce::Colour violet  { 0xffa070ff };
    inline const juce::Colour blue    { 0xff3d7bff };
    inline const juce::Colour lime    { 0xffb6ff00 };
    inline const juce::Colour orange  { 0xffff8a1f };
    inline const juce::Colour yellow  { 0xffffe600 };
    inline const juce::Colour panel   { 0xff0e0e1a };
    inline const juce::Colour grey    { 0xff8d97ad };

    inline juce::Font mono (float size, bool bold = false)
    {
        return juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), size,
                                              bold ? juce::Font::bold : juce::Font::plain));
    }
}

//==============================================================================
class CyberLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CyberLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override              { return Cyber::mono (13.0f, true); }
    juce::Font getPopupMenuFont() override                             { return Cyber::mono (13.0f); }
    juce::Font getLabelFont (juce::Label&) override                    { return Cyber::mono (11.0f); }
    juce::Font getTextButtonFont (juce::TextButton&, int) override     { return Cyber::mono (12.0f, true); }
};

//==============================================================================
// Grille du séquenceur : 4 lignes (mesures) x 16 pas.
//   clic            : active / désactive un pas
//   glisser haut/bas: change la note   (molette aussi)
//   clic droit      : efface le pas
class StepGrid : public juce::Component
{
public:
    StepGrid (MonSyntheProcessor& p, juce::Colour accentColour) : proc (p), accent (accentColour) {}

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

    void refresh();     // appelée par le timer de l'éditeur

private:
    int stepAt (juce::Point<int>) const;
    juce::Rectangle<float> cellBounds (int step) const;

    MonSyntheProcessor& proc;
    juce::Colour accent;

    int dragStep = -1;
    int dragStartNote = 48;
    bool wasOn = false;
    bool dragMoved = false;

    int lastStep = -2, lastRevision = -1, lastBars = -1;
    bool lastOn = false;
};

//==============================================================================
class MonSyntheEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    explicit MonSyntheEditor (MonSyntheProcessor&);
    ~MonSyntheEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<APVTS::SliderAttachment> attachment;   // détruit en premier
    };

    struct PageItem { juce::Component* comp; int page; };
    struct Panel    { int page; juce::Rectangle<int> area; juce::String title; juce::Colour colour; };

    static constexpr int pageOsc = 0, pageFilter = 1, pageFx = 2, pageSeq = 3;

    Knob* makeKnob (int page, const juce::String& paramId, const juce::String& name, juce::Colour colour,
                    bool linear = false);
    void setupCombo (int page, juce::ComboBox&, juce::Label&, const juce::String& title, juce::Colour colour,
                     const juce::StringArray& items);
    void updatePageVisibility();

    static void placeKnob (Knob* k, juce::Rectangle<int> bounds);
    static void layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content);
    static void placeCombo (juce::Label& label, juce::ComboBox& box, juce::Rectangle<int> bounds);

    void refreshPresetList (const juce::String& select = {});
    void stepPreset (int delta);

    void drawPanel (juce::Graphics&, const Panel&);
    void drawScope (juce::Graphics&, juce::Rectangle<float> area);
    void timerCallback() override;

    MonSyntheProcessor& proc;
    CyberLookAndFeel laf;

    int page = 0;
    std::vector<PageItem> pageItems;
    std::vector<Panel> panels;

    // Boutons rotatifs / curseurs
    std::vector<std::unique_ptr<Knob>> knobs;
    Knob* waveKnob = nullptr;
    Knob* gateKnob = nullptr;
    Knob* tempoKnob = nullptr;
    std::vector<Knob*> oscBKnobs, unisonKnobs, lfoKnobs, modKnobs,
                       filterKnobs, fltEnvKnobs, ampKnobs, jeuKnobs,
                       chorusKnobs, delayKnobs, reverbKnobs;

    // Listes déroulantes
    juce::ComboBox lfoShapeBox, filterTypeBox, barsBox, rateBox;
    juce::Label lfoShapeLabel, filterTypeLabel, barsLabel, rateLabel;
    std::unique_ptr<APVTS::ComboBoxAttachment> lfoShapeAttachment, filterTypeAttachment,
                                               barsAttachment, rateAttachment;

    // Boutons on/off
    juce::TextButton monoButton, seqButton;
    std::unique_ptr<APVTS::ButtonAttachment> monoAttachment, seqButtonAttachment;

    StepGrid grid;

    // Onglets
    std::array<juce::TextButton, 4> tabButtons;

    // Presets
    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" }, saveButton { "SAVE" }, deleteButton { "SUPPR" };
    juce::TextEditor nameEditor;
    int deleteArmed = 0;

    juce::Rectangle<int> scopeArea;
    float lastPos = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonSyntheEditor)
};
