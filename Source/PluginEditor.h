#pragma once
#include "PluginProcessor.h"

//==============================================================================
// Palette dynamique : tout se déduit d'une teinte centrale (comme la règle RGB d'Ableton).
// Les 6 accents sont des couleurs voisines de la teinte choisie, les fonds sont des gris anthracite teintés.
namespace Pal
{
    inline float hue = 0.52f;

    inline juce::Colour bg, panel, panelHi, line, grid, text, dim;
    inline juce::Colour cyan, azure, blue, indigo, violet, teal;      // accents 0..5

    inline void update (float h)
    {
        hue = h - std::floor (h);
        auto wrap = [] (float v) { return v - std::floor (v); };
        auto accent = [&] (float offset, float sat) { return juce::Colour::fromHSV (wrap (hue + offset), sat, 1.0f, 1.0f); };

        cyan   = accent ( 0.000f, 0.80f);
        azure  = accent ( 0.060f, 0.75f);
        blue   = accent ( 0.120f, 0.70f);
        indigo = accent ( 0.180f, 0.60f);
        violet = accent (-0.060f, 0.80f);
        teal   = accent (-0.120f, 0.80f);

        bg      = juce::Colour::fromHSV (hue, 0.30f, 0.050f, 1.0f);
        panel   = juce::Colour::fromHSV (hue, 0.16f, 0.095f, 1.0f);
        panelHi = juce::Colour::fromHSV (hue, 0.16f, 0.150f, 1.0f);
        line    = juce::Colour::fromHSV (hue, 0.18f, 0.230f, 1.0f);
        grid    = juce::Colour::fromHSV (hue, 0.10f, 0.220f, 1.0f);
        text    = juce::Colour::fromHSV (hue, 0.08f, 0.96f, 1.0f);
        dim     = juce::Colour::fromHSV (hue, 0.14f, 0.66f, 1.0f);
    }

    inline juce::Colour role (int i)
    {
        switch (i)
        {
            case 0:  return cyan;
            case 1:  return azure;
            case 2:  return blue;
            case 3:  return indigo;
            case 4:  return violet;
            default: return teal;
        }
    }

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
    CyberLookAndFeel() = default;

    void refreshColours();      // à appeler après Pal::update()
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
// Bouton de menu avec un voyant (LED) en haut à gauche
class LedButton : public juce::TextButton
{
public:
    void setLed (float level, juce::Colour colour)
    {
        if (std::abs (level - ledLevel) > 0.02f || colour != ledColour)
        {
            ledLevel = level;
            ledColour = colour;
            repaint();
        }
    }

    void paintButton (juce::Graphics& g, bool highlighted, bool down) override
    {
        juce::TextButton::paintButton (g, highlighted, down);

        const float s = juce::jmax (4.0f, (float) getHeight() * 0.17f);
        const juce::Rectangle<float> led (6.0f, 6.0f, s, s);

        if (ledLevel > 0.3f)
        {
            g.setColour (ledColour.withAlpha (0.28f * ledLevel));
            g.fillRect (led.expanded (2.5f));
        }
        g.setColour (ledColour.withAlpha (0.12f + 0.88f * ledLevel));
        g.fillRect (led);
    }

private:
    float ledLevel = 0.0f;
    juce::Colour ledColour { juce::Colours::white };
};

//==============================================================================
// Règle de couleur : on glisse pour changer la teinte centrale de toute l'interface
class HueBar : public juce::Component
{
public:
    std::function<void (float, bool)> onChange;       // (teinte, true quand on relâche la souris)

    void setValue (float v) { value = v; repaint(); }

    void paint (juce::Graphics& g) override
    {
        const auto r = getLocalBounds().toFloat().reduced (1.0f);
        const auto bar = r.withTrimmedTop (r.getHeight() * 0.30f).withTrimmedBottom (r.getHeight() * 0.30f);

        juce::ColourGradient grad (juce::Colour::fromHSV (0.0f, 0.8f, 1.0f, 1.0f), r.getX(), 0.0f,
                                   juce::Colour::fromHSV (1.0f, 0.8f, 1.0f, 1.0f), r.getRight(), 0.0f, false);
        for (int i = 1; i < 6; ++i)
            grad.addColour ((double) i / 6.0, juce::Colour::fromHSV ((float) i / 6.0f, 0.8f, 1.0f, 1.0f));

        g.setGradientFill (grad);
        g.fillRect (bar);

        const float x = r.getX() + value * r.getWidth();
        g.setColour (juce::Colours::white);
        g.fillRect (x - 1.5f, r.getY(), 3.0f, r.getHeight());
    }

    void mouseDown (const juce::MouseEvent& e) override { setFromX (e.x, false); }
    void mouseDrag (const juce::MouseEvent& e) override { setFromX (e.x, false); }
    void mouseUp (const juce::MouseEvent& e) override   { setFromX (e.x, true); }

private:
    void setFromX (int x, bool released)
    {
        value = juce::jlimit (0.0f, 1.0f, (float) x / (float) juce::jmax (1, getWidth()));
        repaint();
        if (onChange)
            onChange (value, released);
    }

    float value = 0.5f;
};

//==============================================================================
// Navigateur de presets : categories, recherche, favoris
class PresetBrowser : public juce::Component, private juce::ListBoxModel
{
public:
    PresetBrowser (MonSyntheProcessor& p, std::function<void (int)> onPickPreset, std::function<void()> onClose);

    void rebuild();                               // relit la banque d'usine et les presets utilisateur
    void setSelectedId (int id) { currentId = id; list.repaint(); }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Entry { juce::String name, category; int id; };

    int getNumRows() override { return (int) shown.size(); }
    void paintListBoxItem (int row, juce::Graphics&, int width, int height, bool selected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;

    void applyFilter();
    juce::File favouritesFile() const;
    void loadFavourites();
    void saveFavourites() const;

    MonSyntheProcessor& proc;
    std::function<void (int)> onPick;
    juce::TextEditor search;
    juce::ListBox list { "presets", this };
    juce::TextButton closeButton { "FERMER" };
    std::vector<std::unique_ptr<juce::TextButton>> catButtons;
    std::vector<Entry> all, shown;
    juce::StringArray cats, favs;
    juce::String catFilter { "TOUS" };
    int currentId = -1;
};

//==============================================================================
// Analyseur de spectre de la sortie, en segments
// Zone où l'on dessine la courbe du LFO dessiné (32 points au plus) à la souris
class DrawLfoView : public juce::Component
{
public:
    explicit DrawLfoView (MonSyntheProcessor& p) : proc (p) {}

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent& e) override { lastIdx = -1; paintAt (e); }
    void mouseDrag (const juce::MouseEvent& e) override { paintAt (e); }
    void setShape (int kind);      // 0 rampe, 1 sinus, 2 aléatoire, 3 escalier, 4 plat

private:
    void paintAt (const juce::MouseEvent&);
    void setPoint (int index, float value);

    MonSyntheProcessor& proc;
    int lastIdx = -1;
    float lastVal = 0.0f;
};

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

    void setAccent (juce::Colour c) { accent = c; repaint(); }

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
// Éditeur : menus à gauche, deux écrans (A et B) avec chacun 4 potards fixes, macros à droite,
// séquenceur en bas. Les 8 potards fixes se réassignent automatiquement selon le menu choisi.
class MonSyntheEditor : public juce::AudioProcessorEditor,
                        public juce::FileDragAndDropTarget,
                        private juce::Timer
{
public:
    explicit MonSyntheEditor (MonSyntheProcessor&);
    ~MonSyntheEditor() override;

    void showMenuForSnapshot (int index) { selectMenu (index); }
    void paint (juce::Graphics&) override;
    void resized() override;
    void paintOverChildren (juce::Graphics&) override;
    static int modTargetForParam (const juce::String& id);

    // EQ : on attrape les points du graphique à la souris
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

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
        int role = 0;
        bool linear = false;
        juce::String paramId;      // paramètre actuellement relié (pour la modulation par clic droit)
    };
    void showModMenu (Knob& knob);
    void autoEnable (const juce::String& paramId);      // active le module quand on touche un de ses potards
    juce::TextButton mutateButton { "MUTER" };

    enum Viz { VizNone = 0, VizScopeA, VizUnison, VizScopeB, VizFmRing, VizFilter,
               VizEnvFilter, VizEnvAmp, VizEnvMod, VizLfo1, VizLfo2, VizLfo3, VizLfo4, VizGlide,
               VizChorus, VizDelay, VizReverb, VizMatrix, VizSpectrum, VizNoise, VizPitch, VizDisto, VizComp, VizString, VizFilter2, VizDraw };

    struct ParamSlot { const char* id; const char* label; };
    struct MenuDef   { const char* name; int role; int vizA; int vizB; ParamSlot p[8]; };

    struct ComboRef  { juce::ComboBox* box; int role; };
    struct ButtonRef { juce::TextButton* button; int role; };
    struct NamedCombo { juce::ComboBox box; std::unique_ptr<APVTS::ComboBoxAttachment> att; };

    // L'interface est dessinée pour 1280 x 720 puis mise à l'échelle (fenêtre redimensionnable, 16:9)
    static constexpr int baseWidth = 1280, baseHeight = 660;
    static constexpr int numMenus = 28, matrixMenu = 23;
    enum MenuId { mOscA = 0, mOscB, mFmRing, mUnison, mNoise, mString, mPitch, mFilter, mFilter2, mDisto, mEnvAmp, mEnvMod,
                  mLfo, mLfo34, mDraw, mJeu, mChorus, mPhaser, mDelay, mReverb, mEq, mMaster, mRack, mMatrix,
                  mFmRingUni, mNoiseStr, mPitchJeu, mEnvFlt };      // pages fusionnées (ajoutées à la fin)

    static const MenuDef& menuAt (int index);
    struct FamilyDef { const char* name; int role; int numTabs; int tabs[8]; };
    static const FamilyDef& familyAt (int index);

    int S (float v) const noexcept { return juce::roundToInt (v * scale); }
    float pv (const char* id) const;

    Knob* makeKnob (const juce::String& paramId, const juce::String& name, int role, bool linear = false);
    void styleButton (juce::TextButton&, int role);

    void selectMenu (int index);
    void updateMenuVisibility();
    void applyTheme();
    void updateLeds();

    void placeKnob (Knob* k, juce::Rectangle<int> bounds);
    void layoutRow (const std::vector<Knob*>& items, juce::Rectangle<int> content);

    void importWavetable (int slot);
    void loadWavetableFile (int slot, const juce::File& file);
    void refreshPresetList (const juce::String& select = {});
    std::unique_ptr<PresetBrowser> browser;
    juce::TextButton browserButton { "PRESETS" };
    void stepPreset (int delta);

    // Réglage fin au clavier, MIDI learn, raccourcis
    bool keyPressed (const juce::KeyPress&) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;
    void showKnobMenu (Knob&);
    void nudgeKnob (Knob&, int direction, bool fine, bool big);
    Knob* hoveredKnob = nullptr;
    Knob* lastKnob = nullptr;

    // Dessin
    void drawPanel (juce::Graphics&, juce::Rectangle<int> area, const juce::String& title, juce::Colour colour,
                    const juce::String& rightText = {});
    void drawLogo (juce::Graphics&, juce::Rectangle<int> area);
    void drawScreen (juce::Graphics&, juce::Rectangle<int> panelArea, juce::Rectangle<int> gfxArea, bool isA);
    void drawViz (juce::Graphics&, int viz, juce::Rectangle<float> r, juce::Colour colour);
    void drawScopeViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour, bool forB);
    void drawUnisonViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawFmRingViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawFilterViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawFilter2Viz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawEnvelopeViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour,
                          const char* a, const char* d, const char* s, const char* rel);
    void drawLfoViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour, int which);
    void drawGlideViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawChorusViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawDelayViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawReverbViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawNoiseViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawStringViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawPitchViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawDistoViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawCompViz (juce::Graphics&, juce::Rectangle<float> r, juce::Colour colour);
    void drawEq (juce::Graphics&);
    void selectEqBand (int band);
    juce::Point<float> eqNodePos (int band) const;
    int eqHitTest (juce::Point<int>) const;
    void setParamPlain (const juce::String& id, float value);
    float paramPlain (const juce::String& id) const;
    void timerCallback() override;

    MonSyntheProcessor& proc;
    CyberLookAndFeel laf;
    float scale = 1.0f;
    int currentMenu = 0;

    // Tous les boutons rotatifs / curseurs
    std::vector<std::unique_ptr<Knob>> knobs;
    std::array<Knob*, 8> fixedKnobs {};          // P1..P8 : réassignés selon le menu
    std::array<Knob*, 4> macroKnobs {};
    Knob* masterKnob = nullptr;
    Knob* gateKnob = nullptr;
    Knob* tempoKnob = nullptr;
    std::array<Knob*, 12> mixAmt {};

    NamedCombo tableA, tableB, warpA, warpB, filterType, lfoShape, lfoSync, lfo2Shape, lfo2Sync, lfo3Shape, lfo3Sync, lfo4Shape, lfo4Sync, delaySync, delayMode,
               noiseType, phaseMode, distType, f2Type, f2Mode, dwSync, quality;
    juce::TextButton monoButton;
    std::unique_ptr<APVTS::ButtonAttachment> monoAttachment;
    std::vector<std::pair<juce::Rectangle<int>, juce::String>> captions;

    // Menus : 28 pages (cachées) regroupées en 10 familles (boutons de gauche) et jusqu'à 8 onglets au-dessus des écrans
    std::array<LedButton, numMenus> menuButtons;
    std::array<LedButton, 10> famButtons;
    std::array<LedButton, 8> tabButtons;
    int currentFamily = 0, currentTab = 0;
    std::array<int, 10> lastTab {};
    int originX = 0;
    juce::TextButton maschineButton { "MASCHINE" };
    int lastFamilyLearn = -1;
    void selectFamily (int family);
    void selectTab (int tab);
    void updateNavButtons();
    void applyKnobText (Knob&);
    void pushPageToProcessor();
    void handleMidiNavigation();

    // Séquenceur
    juce::TextButton seqButton;
    juce::ComboBox barsBox, rateBox;
    juce::Label barsLabel, rateLabel;
    std::unique_ptr<APVTS::ButtonAttachment> seqButtonAttachment;
    std::unique_ptr<APVTS::ComboBoxAttachment> barsAttachment, rateAttachment;
    StepGrid grid;
    juce::TextButton pianoButton { "PIANO" }, recButton { "REC" };
    std::unique_ptr<APVTS::ButtonAttachment> recAttachment;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    bool pianoMode = false;
    void applyKeyboardColours();

    // Matrice de modulation (6 routes)
    std::array<juce::ComboBox, 12> mixSrcBox, mixDstBox, mixCrvBox;
    std::array<std::unique_ptr<APVTS::ComboBoxAttachment>, 12> mixSrcAtt, mixDstAtt, mixCrvAtt;
    juce::Label mixHdrSrc, mixHdrDst, mixHdrAmt, mixHdrCrv;
    std::vector<juce::Component*> matrixComponents;

    // En-tête
    SpectrumView headerSpectrum, screenSpectrum;
    HueBar hueBar;
    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" }, saveButton { "SAVE" }, deleteButton { "SUPPR" };
    juce::TextEditor nameEditor;
    int deleteArmed = 0;

    // Import de wavetables
    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton importAButton { "IMPORT" }, importBButton { "IMPORT" };
    juce::TextButton resampleAButton { "RESAMPLE" }, resampleBButton { "RESAMPLE" };
    int resampleSlot = -1;
    void startResample (int slot);
    juce::String statusMessage;
    int statusFrames = 0;
    juce::Rectangle<int> oscAArea, oscBArea;          // (zones de dépôt des fichiers)

    // Zones
    juce::Rectangle<int> menuPanel, screenAPanel, screenBPanel, gfxA, gfxB, macrosPanel, outPanel, seqPanel,
                         matrixArea, ledsArea, meterArea, beatLedArea, stripA, stripB, knobsA, knobsB;
    juce::Rectangle<int> scopeArea;                   // = zone graphique de l'écran A

    // EQ 8 bandes
    std::array<juce::TextButton, 8> eqBandButtons;
    DrawLfoView drawView { proc };
    std::array<juce::TextButton, 5> drawShapeButtons;
    juce::TextButton eqOnButton { "BANDE ON" };
    juce::ComboBox eqTypeBox;
    std::unique_ptr<APVTS::ButtonAttachment> eqOnAtt;
    std::unique_ptr<APVTS::ComboBoxAttachment> eqTypeAtt;
    juce::Rectangle<int> eqGraphArea;
    int eqBand = 2;
    int eqDragBand = -1;

    // Rack d'effets (4 emplacements)
    struct RackSlotUi
    {
        juce::TextButton onBtn { "ON" }, leftBtn { "<" }, rightBtn { ">" };
        juce::ComboBox typeBox;
        std::unique_ptr<APVTS::ButtonAttachment> onAtt;
        std::array<Knob*, 5> k {};            // A, B, C, D, MIX
        int shownType = -1;
        juce::Rectangle<int> area;
    };
    std::array<RackSlotUi, 4> rack;
    void setupRack();
    void refreshRack();
    void swapRackSlots (int s);

    // Mise à jour des couleurs
    std::vector<ComboRef> comboRefs;
    std::vector<ButtonRef> buttonRefs;
    std::vector<juce::Label*> dimLabels;

    float outLevel = 0.0f;
    float lastSig = -1.0f;
    int lastTable = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonSyntheEditor)
};
