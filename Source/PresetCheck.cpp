// Outil de mesure : rend chaque preset d'usine hors ligne et signale niveau, silence, saturation et valeurs invalides.
#include "PluginProcessor.h"
#include <cstdio>

int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    MonSyntheProcessor proc;
    const double sr = 44100.0;
    const int block = 512;
    proc.setRateAndBufferSizeDetails (sr, block);
    proc.prepareToPlay (sr, block);

    const int n = MonSyntheProcessor::getNumFactoryPresets();
    int bad = 0;
    std::printf ("IDX|NOM|CAT|PIC dB|RMS dB|QUEUE dB|ETAT\n");

    for (int p = 0; p < n; ++p)
    {
        proc.loadFactoryPreset (p);
        proc.reset();

        juce::AudioBuffer<float> buf (2, block);
        double sumSq = 0.0, tailSq = 0.0;
        float peak = 0.0f;
        long count = 0, tailCount = 0;
        bool nan = false;

        const int holdBlocks = (int) (2.0 * sr / block);
        const int tailBlocks = (int) (1.5 * sr / block);
        const int note = 48;      // C3 (les basses sont jouées une octave plus bas pour les catégories BASSES)
        const int usedNote = MonSyntheProcessor::getFactoryPresetCategory (p) == "BASSES" ? note - 12 : note;

        for (int b = 0; b < holdBlocks + tailBlocks; ++b)
        {
            juce::MidiBuffer midi;
            if (b == 0)
                midi.addEvent (juce::MidiMessage::noteOn (1, usedNote, 0.8f), 0);
            if (b == holdBlocks)
                midi.addEvent (juce::MidiMessage::noteOff (1, usedNote), 0);

            buf.clear();
            proc.processBlock (buf, midi);

            for (int i = 0; i < block; ++i)
            {
                const float l = buf.getSample (0, i), r = buf.getSample (1, i);
                if (! std::isfinite (l) || ! std::isfinite (r)) { nan = true; continue; }
                const float m = juce::jmax (std::abs (l), std::abs (r));
                if (b < holdBlocks)
                {
                    peak = juce::jmax (peak, m);
                    sumSq += (double) l * l + (double) r * r;
                    count += 2;
                }
                else
                {
                    tailSq += (double) l * l + (double) r * r;
                    tailCount += 2;
                }
            }
        }

        const double rms = count > 0 ? std::sqrt (sumSq / (double) count) : 0.0;
        const double tail = tailCount > 0 ? std::sqrt (tailSq / (double) tailCount) : 0.0;
        auto db = [] (double x) { return 20.0 * std::log10 (juce::jmax (1.0e-9, x)); };

        juce::String state = "ok";
        if (nan) state = "NAN";
        else if (peak < 0.01f) state = "SILENCE";
        else if (peak > 1.0f) state = "SATURE";
        else if (db (rms) > -12.0) state = "FORT";
        else if (db (rms) < -30.0) state = "FAIBLE";
        if (state != "ok") ++bad;

        std::printf ("%d|%s|%s|%.1f|%.1f|%.1f|%s\n", p, MonSyntheProcessor::getFactoryPresetName (p).toRawUTF8(),
                     MonSyntheProcessor::getFactoryPresetCategory (p).toRawUTF8(), db (peak), db (rms), db (tail), state.toRawUTF8());
    }

    std::printf ("TOTAL %d presets, %d a revoir\n", n, bad);
    return 0;
}
