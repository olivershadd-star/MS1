// Outil : génère des captures de l'interface (sans fenêtre) pour la documentation
#include "PluginProcessor.h"
#include "PluginEditor.h"

int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    MonSyntheProcessor proc;
    proc.setRateAndBufferSizeDetails (44100.0, 512);
    proc.prepareToPlay (44100.0, 512);

    struct Shot { const char* file; int preset; int menu; };
    // preset choisis par nom
    auto find = [] (const juce::String& name)
    {
        for (int i = 0; i < MonSyntheProcessor::getNumFactoryPresets(); ++i)
            if (MonSyntheProcessor::getFactoryPresetName (i) == name)
                return i;
        return 0;
    };

    const Shot shots[] = { { "osc.png", find ("Reese Noir"), 0 },
                           { "filtre2.png", find ("Chorale A-E-I-O-U"), 8 },
                           { "rack.png", find ("Fold Bass"), 22 },
                           { "matrice.png", find ("Reese Noir"), 23 },
                           { "eq.png", find ("Shimmer Pad"), 20 },
                           { "dessin.png", find ("Pompe Dessinee"), 14 } };

    std::unique_ptr<juce::AudioProcessorEditor> base (proc.createEditor());
    auto* ed = dynamic_cast<MonSyntheEditor*> (base.get());
    if (ed == nullptr)
        return 1;

    for (const auto& s : shots)
    {
        proc.loadFactoryPreset (s.preset);
        ed->showMenuForSnapshot (s.menu);
        ed->resized();
        const auto img = ed->createComponentSnapshot (ed->getLocalBounds(), true, 2.0f);
        juce::File f = juce::File::getCurrentWorkingDirectory().getChildFile (s.file);
        f.deleteFile();
        juce::FileOutputStream out (f);
        juce::PNGImageFormat png;
        png.writeImageToStream (img, out);
        std::printf ("%s %dx%d\n", s.file, img.getWidth(), img.getHeight());
    }
    return 0;
}
