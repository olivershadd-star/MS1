#pragma once
#include "PluginProcessor.h"

//==============================================================================
// Palette : fonds sombres bleutés, accents froids (cyan, azur, bleu, indigo, violet, turquoise)
namespace Pal
{
    inline const juce::Colour bg      { 0xff06080d };
    inline const juce::Colour panel   { 0xff0b0f18 };
    inline const juce::Colour panelHi { 0xff131a29 };
    inline const juce::Colour line    { 0xff1d2638 };
    inline const juce::Colour text    { 0xffd6e2f5 };
    inline const juce::Colour dim     { 0xff6f7f99 };

    inline const juce::Colour cyan    { 0xff2fe0ff };
    inline const juce::Colour azure   { 0xff3f9bff };
    inline const juce::Colour blue    { 0xff5b7cff };
    inline const juce::Colour indigo  { 0xff8a7bff };
    inline const juce::Colour violet  { 0xffb27bff };
    inline const juce::Colour teal    { 0xff2ee6c6 };

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

    void setScale (float s) noexcept { scale = s; }
    float getScale() const noexcept  { return scale; }

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;

    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override              { return Pal::mono (12.5f * scale, true); }
    juce::Font getPopupMenuFont() override                             { return Pal::mono (12.5f * scale); }
    juce::Font getLabelFont (juce::Label&) override                    { return Pal::mono (10.5f * scale); }
    juce::Font getTextButtonFont (juce::TextButton&, int) override     { return Pal::mono (11.5f * scale, true); }

private:
    float scale = 1.0f;
};

//==============================================================================
// Analyseur de spectre de la sortie, en segments (affiché dans l'en-tête)
class SpectrumView : public juce::Component
{
public:
    explicit SpectrumView (MonSyntheProcessor& p);

    void paint (juce::Graphics&) override;
    void update();      // appelée 30 fois par seconde

private:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int numBars = 32;
    static constexpr int numSegments = 8;

    MonSyntheProcessor& proc;
    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { (size_t) fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, fftSize * 2> fftData {};
    std::array<float, numBars> levels {};
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
                        public juce::FileDragAndDropTarget,
                        private juce::Timer
{
public:
    explicit MonSyntheEditor (MonSyntheProcessor&);
    ~MonSyntheEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Glisser un fichier .wav sur la fenêtre : OSC A (USER 1) ou OSC B (USER 2)
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

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

    // L'interface est dessinée pour 1280 x 720 puis mise à l'échelle (fenêtre redimensionnable, 16:9)
    static constexpr int baseWidth = 1280, baseHeight = 720;
    static constexpr int pageSynth = 0, pageMod = 1, pageSeq = 2;

    int S (float v) const noexcept { return juce::roundToInt (v * scale); }

    Knob* makeKnob (int page, const juce::String& paramId, const juce::String& name, juce::Colour colour,
                    bool linear = false);
    void styleCombo (int page, juce::ComboBox&, juce::Colour colour, const juce::StringArray& items);
    void setupCombo (int page, juce::ComboBox&, juce::Label&, const juce::String& title, juce::Colour colour,
                     const juce::StringArray& items);
    void updatePageVisibility();

    void placeKnob (Knob* k, juce::Rectangle<int> bounds);
    void layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content);
    void placeCombo (juce::Label& label, juce::ComboBox& box, juce::Rectangle<int> bounds);

    void importWavetable (int slot);
    void loadWavetableFile (int slot, const juce::File& file);

    void refreshPresetList (const juce::String& select = {});
    void stepPreset (int delta);

    void drawPanel (juce::Graphics&, const Panel&);
    void drawLogo (juce::Graphics&, juce::Rectangle<int> area);
    void drawScope (juce::Graphics&, juce::Rectangle<float> area);
    void timerCallback() override;

    MonSyntheProcessor& proc;
    CyberLookAndFeel laf;
    float scale = 1.0f;

    int page = 0;
    std::vector<PageItem> pageItems;
    std::vector<Panel> panels;

    // Boutons rotatifs / curseurs
    std::vector<std::unique_ptr<Knob>> knobs;
    Knob* waveKnob = nullptr;
    Knob* gateKnob = nullptr;
    Knob* tempoKnob = nullptr;
    std::array<Knob*, 6> mixAmt {};
    std::vector<Knob*> oscBKnobs, unisonKnobs, lfoKnobs, modKnobs,
                       filterKnobs, fltEnvKnobs, ampKnobs, jeuKnobs,
                       chorusKnobs, delayKnobs, reverbKnobs, velKnobs, lfo2Knobs, macroKnobs;

    // Listes déroulantes
    juce::ComboBox lfoShapeBox, lfoSyncBox, lfo2ShapeBox, lfo2SyncBox, filterTypeBox, barsBox, rateBox, tableABox, tableBBox;
    juce::Label lfoShapeLabel, lfoSyncLabel, lfo2ShapeLabel, lfo2SyncLabel, filterTypeLabel, barsLabel, rateLabel,
                tableALabel, tableBLabel;
    std::unique_ptr<APVTS::ComboBoxAttachment> lfoShapeAttachment, lfoSyncAttachment,
                                               lfo2ShapeAttachment, lfo2SyncAttachment,
                                               filterTypeAttachment, barsAttachment, rateAttachment,
                                               tableAAttachment, tableBAttachment;

    // Matrice de modulation (6 routes)
    std::array<juce::ComboBox, 6> mixSrcBox, mixDstBox;
    std::array<std::unique_ptr<APVTS::ComboBoxAttachment>, 6> mixSrcAtt, mixDstAtt;
    juce::Label mixHdrSrc, mixHdrDst, mixHdrAmt;

    // Boutons on/off
    juce::TextButton monoButton, seqButton;
    std::unique_ptr<APVTS::ButtonAttachment> monoAttachment, seqButtonAttachment;

    StepGrid grid;
    SpectrumView spectrum;

    // Onglets
    std::array<juce::TextButton, 3> tabButtons;

    // Presets
    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" }, saveButton { "SAVE" }, deleteButton { "SUPPR" };
    juce::TextEditor nameEditor;
    int deleteArmed = 0;

    // Import de wavetables
    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton importAButton { "IMPORT WAV" }, importBButton { "WAV" };
    juce::String statusMessage;
    int statusFrames = 0;
    juce::Rectangle<int> oscAArea, oscBArea;

    juce::Rectangle<int> scopeArea;
    float lastPos = -1.0f;
    int lastTable = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonSyntheEditor)
};
