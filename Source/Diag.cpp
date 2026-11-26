// Outil de diagnostic de MS1 : mesure hors ligne ce que l'oreille reproche au moteur.
// Ne fait pas partie du plugin. Lancé par le workflow « Diagnostic ».
#include "PluginProcessor.h"
#include <cstdio>
#include <chrono>
#include <map>
#include <tuple>
#include <algorithm>

namespace
{
constexpr double SR = 44100.0;
constexpr int BLOCK = 512;

struct Render
{
    std::vector<float> m;      // mono
    double cpu = 0.0;          // temps de calcul / durée du son (1 = temps réel)
};

float db (double x) { return (float) (20.0 * std::log10 (juce::jmax (1.0e-9, x))); }

void setP (MonSyntheProcessor& p, const char* id, float real)
{
    if (auto* rp = p.apvts.getParameter (id))
        rp->setValueNotifyingHost (rp->convertTo0to1 (real));
}

Render render (MonSyntheProcessor& p, const std::vector<int>& notes, double hold, double tail, float vel = 0.8f)
{
    p.reset();
    Render out;
    const int hb = (int) (hold * SR / BLOCK), tb = (int) (tail * SR / BLOCK);
    juce::AudioBuffer<float> buf (2, BLOCK);
    const auto t0 = std::chrono::high_resolution_clock::now();
    for (int b = 0; b < hb + tb; ++b)
    {
        juce::MidiBuffer midi;
        if (b == 0)       for (int n : notes) midi.addEvent (juce::MidiMessage::noteOn (1, n, vel), 0);
        if (b == hb)      for (int n : notes) midi.addEvent (juce::MidiMessage::noteOff (1, n), 0);
        buf.clear();
        p.processBlock (buf, midi);
        for (int i = 0; i < BLOCK; ++i)
            out.m.push_back (0.5f * (buf.getSample (0, i) + buf.getSample (1, i)));
    }
    const double el = std::chrono::duration<double> (std::chrono::high_resolution_clock::now() - t0).count();
    out.cpu = el / ((hold + tail));
    return out;
}

double rmsRange (const std::vector<float>& x, double t0, double t1)
{
    const size_t a = (size_t) (t0 * SR), b = juce::jmin (x.size(), (size_t) (t1 * SR));
    double s = 0; size_t n = 0;
    for (size_t i = a; i < b; ++i) { s += (double) x[i] * x[i]; ++n; }
    return n ? std::sqrt (s / (double) n) : 0.0;
}

// Spectre moyen (dB) en 24 bandes log de 80 Hz à 16 kHz, sur [t0,t1]
std::vector<float> bands (const std::vector<float>& x, double t0, double t1)
{
    constexpr int order = 13, N = 1 << order;
    juce::dsp::FFT fft (order);
    std::vector<float> acc (24, 0.0f);
    std::vector<float> fd (2 * N);
    int frames = 0;
    for (size_t s = (size_t) (t0 * SR); s + N <= juce::jmin (x.size(), (size_t) (t1 * SR)); s += N / 2)
    {
        for (int i = 0; i < N; ++i)
            fd[(size_t) i] = x[s + (size_t) i] * (0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * (float) i / (float) N));
        std::fill (fd.begin() + N, fd.end(), 0.0f);
        fft.performFrequencyOnlyForwardTransform (fd.data());
        for (int k = 0; k < 24; ++k)
        {
            const double f0 = 80.0 * std::pow (200.0, k / 24.0), f1 = 80.0 * std::pow (200.0, (k + 1) / 24.0);
            const int i0 = (int) (f0 / SR * N), i1 = juce::jmax (i0 + 1, (int) (f1 / SR * N));
            double e = 0; for (int i = i0; i < i1 && i < N / 2; ++i) e += (double) fd[(size_t) i] * fd[(size_t) i];
            acc[(size_t) k] += (float) e;
        }
        ++frames;
    }
    for (auto& v : acc) v = db (std::sqrt (v / (float) juce::jmax (1, frames)) / N);
    return acc;
}

float bandDist (const std::vector<float>& a, const std::vector<float>& b, bool normalise)
{
    float ma = 0, mb = 0;
    if (normalise) { for (auto v : a) ma += v; for (auto v : b) mb += v; ma /= 24; mb /= 24; }
    float d = 0; for (size_t i = 0; i < 24; ++i) d += std::abs ((a[i] - ma) - (b[i] - mb));
    return d / 24.0f;
}

// Part de l'énergie qui n'est pas sur un harmonique de f0 (repliement / bruit), en dB
float inharmonicDb (const std::vector<float>& x, double f0)
{
    constexpr int order = 15, N = 1 << order;
    juce::dsp::FFT fft (order);
    std::vector<float> fd (2 * N, 0.0f);
    const size_t s0 = (size_t) (0.4 * SR);
    if (x.size() < s0 + N) return 0.0f;
    for (int i = 0; i < N; ++i)
        fd[(size_t) i] = x[s0 + (size_t) i] * (0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * (float) i / (float) N));
    fft.performFrequencyOnlyForwardTransform (fd.data());
    double harm = 0, other = 0;
    for (int k = 4; k < N / 2; ++k)
    {
        const double f = (double) k * SR / N;
        if (f > 18000.0) break;
        const double h = f / f0, dist = std::abs (h - std::round (h)) * f0;     // écart en Hz au harmonique le plus proche
        const double e = (double) fd[(size_t) k] * fd[(size_t) k];
        (dist < 12.0 ? harm : other) += e;
    }
    return db (std::sqrt (other / juce::jmax (1.0e-20, harm + other)));
}

double hz (int note) { return 440.0 * std::pow (2.0, (note - 69) / 12.0); }
}

int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    const auto tBuild = std::chrono::high_resolution_clock::now();
    MonSyntheProcessor proc;
    std::printf ("Temps de creation du synthe (tables comprises) : %.0f ms\n", std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - tBuild).count());
    { const auto t1 = std::chrono::high_resolution_clock::now(); MonSyntheProcessor p2; std::printf ("Deuxieme instance : %.0f ms\n", std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - t1).count()); }
    proc.setRateAndBufferSizeDetails (SR, BLOCK);
    proc.prepareToPlay (SR, BLOCK);
    const int nPresets = MonSyntheProcessor::getNumFactoryPresets();

    // ------------------------------------------------------------------ A. Courbe du volume
    std::printf ("== A. VOLUME : niveau de sortie selon la position du potard (Init, note C4)\n");
    proc.loadFactoryPreset (0);
    for (float m : { 0.0f, 0.05f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f, 1.5f })
    {
        setP (proc, "master", m);
        auto r = render (proc, { 60 }, 1.0, 0.3);
        std::printf ("master=%.2f -> %.1f dB\n", m, db (rmsRange (r.m, 0.3, 1.0)));
    }
    setP (proc, "master", 1.0f);

    // ------------------------------------------------------------------ B. Attaque
    std::printf ("\n== B. ATTAQUE : temps pour atteindre 50%% du niveau final (ms)\n");
    for (float a : { 0.001f, 0.2f, 1.0f, 5.0f })
    {
        proc.loadFactoryPreset (0);
        setP (proc, "attack", a);
        setP (proc, "sustain", 1.0f);
        auto r = render (proc, { 60 }, 7.0, 0.5);
        const double fin = rmsRange (r.m, 6.0, 7.0);
        double t50 = -1;
        for (size_t i = 0; i + 441 < r.m.size(); i += 441)
        {
            double s = 0; for (size_t k = 0; k < 441; ++k) s += (double) r.m[i + k] * r.m[i + k];
            if (std::sqrt (s / 441.0) >= 0.5 * fin) { t50 = (double) i / SR * 1000.0; break; }
        }
        std::printf ("attack=%.3fs -> 50%% atteint a %.0f ms (niveau final %.1f dB)\n", a, t50, db (fin));
    }

    // ------------------------------------------------------------------ C. Repliement (aliasing)
    std::printf ("\n== C. REPLIEMENT : energie hors harmoniques (dB, plus bas = plus propre), note C6, une voix\n");
    for (int t = 0; t < 8; ++t)
    {
        proc.loadFactoryPreset (0);
        setP (proc, "tableA", (float) t); setP (proc, "position", 0.5f);
        setP (proc, "cutoff", 20000.0f); setP (proc, "unison", 1.0f);
        auto r = render (proc, { 84 }, 1.2, 0.1);
        std::printf ("table %d : %.1f dB\n", t, inharmonicDb (r.m, hz (84)));
    }
    for (auto [name, id, val] : std::vector<std::tuple<const char*, const char*, float>> {
             { "FM 0.5", "fmAmount", 0.5f }, { "RING 0.6", "ringAmount", 0.6f }, { "WARP A 0.6 (mode 3 sync)", "warpAmtA", 0.6f }, { "DISTO 0.7", "distAmt", 0.7f } })
    {
        proc.loadFactoryPreset (0);
        setP (proc, "cutoff", 20000.0f); setP (proc, "unison", 1.0f); setP (proc, "oscBLevel", 0.6f);
        if (std::string (id) == "warpAmtA") setP (proc, "warpModeA", 3.0f);
        if (std::string (id) == "distAmt") { setP (proc, "distType", 1.0f); setP (proc, "distMix", 1.0f); }
        setP (proc, id, val);
        auto r = render (proc, { 72 }, 1.2, 0.1);
        std::printf ("%s : %.1f dB\n", name, inharmonicDb (r.m, hz (72)));
    }

    std::printf ("\n== C2. QUALITE : repliement et CPU selon le reglage Qualite (0 ECO, 1 NORMAL, 2 HAUTE)\n");
    for (int q = 0; q < 3; ++q)
    {
        for (auto [name, id, val] : std::vector<std::tuple<const char*, const char*, float>> {
                 { "FM 0.5", "fmAmount", 0.5f }, { "WARP A 0.6", "warpAmtA", 0.6f }, { "DISTO 0.7", "distAmt", 0.7f } })
        {
            proc.loadFactoryPreset (0);
            setP (proc, "quality", (float) q);
            setP (proc, "cutoff", 20000.0f); setP (proc, "unison", 1.0f); setP (proc, "oscBLevel", 0.6f);
            if (std::string (id) == "warpAmtA") setP (proc, "warpModeA", 3.0f);
            if (std::string (id) == "distAmt") { setP (proc, "distType", 1.0f); setP (proc, "distMix", 1.0f); }
            setP (proc, id, val);
            auto r = render (proc, { 72 }, 1.2, 0.1);
            std::printf ("qualite %d  %s : %.1f dB\n", q, name, inharmonicDb (r.m, hz (72)));
        }
        proc.loadFactoryPreset (0);
        setP (proc, "quality", (float) q); setP (proc, "unison", 7.0f); setP (proc, "oscBLevel", 0.6f); setP (proc, "fmAmount", 0.3f);
        auto rc = render (proc, { 48, 52, 55, 59, 60, 64, 67, 71 }, 2.0, 0.5);
        std::printf ("qualite %d  CPU 8 notes, unison 7, osc B + FM : %.1f%%\n", q, rc.cpu * 100.0);
    }

    std::printf ("\n== C3. TABLES : niveau (dB) et variation du spectre en balayant la position 0 -> 1 (note C4, une voix)\n");
    for (int t = 0; t < WavetableBank::numTables; ++t)
    {
        std::vector<std::vector<float>> sp; float lo = 99, hi = -99;
        for (float pos : { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f })
        {
            proc.loadFactoryPreset (0);
            setP (proc, "tableA", (float) t); setP (proc, "position", pos); setP (proc, "cutoff", 20000.0f); setP (proc, "unison", 1.0f);
            auto r = render (proc, { 60 }, 0.9, 0.1);
            sp.push_back (bands (r.m, 0.3, 0.8));
            const float lv = db (rmsRange (r.m, 0.3, 0.8)); lo = juce::jmin (lo, lv); hi = juce::jmax (hi, lv);
        }
        float step = 0, tot = bandDist (sp.front(), sp.back(), true);
        for (size_t i = 0; i + 1 < sp.size(); ++i) step += bandDist (sp[i], sp[i + 1], true);
        std::printf ("table %2d %-12s : niveau %.1f..%.1f dB, variation totale %.1f dB, par pas %.1f dB\n", t,
                     WavetableBank::tableNames()[t].toRawUTF8(), lo, hi, tot, step / 5.0f);
    }

    // ------------------------------------------------------------------ D. Sensibilite des potards
    std::printf ("\n== D. POTARDS : changement audible entre minimum et maximum (dB moyen par bande ; <1,5 = presque rien)\n");
    struct K { const char* id; const char* label; float lo, hi; };
    const std::vector<K> knobs = {
        { "position", "Position table A", 0.0f, 1.0f }, { "oscBPos", "Position table B", 0.0f, 1.0f },
        { "warpAmtA", "Warp A (mode BEND+)", 0.0f, 1.0f }, { "warpAmtB", "Warp B (mode BEND+)", 0.0f, 1.0f },
        { "unison", "Unison (voix)", 1.0f, 7.0f }, { "detune", "Desaccord (unison 5)", 0.0f, 100.0f },
        { "cutoff", "Cutoff", 200.0f, 20000.0f }, { "resonance", "Resonance (cutoff 1500)", 0.5f, 10.0f },
        { "drive", "Drive", 0.0f, 1.0f }, { "fmAmount", "FM", 0.0f, 1.0f }, { "ringAmount", "Ring", 0.0f, 1.0f },
        { "subLevel", "Sub", 0.0f, 1.0f }, { "noiseLevel", "Bruit", 0.0f, 1.0f }, { "oscBLevel", "Niveau B", 0.0f, 1.0f },
        { "f2Mix", "Filtre 2 mix (cutoff 1200)", 0.0f, 1.0f }, { "distAmt", "Disto amount (type HARD, mix 1)", 0.0f, 1.0f },
        { "chorusMix", "Chorus mix", 0.0f, 1.0f }, { "reverbMix", "Reverb mix", 0.0f, 1.0f } };
    for (const auto& k : knobs)
    {
        std::vector<float> spec[2]; double lev[2];
        for (int e = 0; e < 2; ++e)
        {
            proc.loadFactoryPreset (0);
            setP (proc, "oscBLevel", 0.6f); setP (proc, "oscBPos", 0.4f); setP (proc, "position", 0.4f);
            setP (proc, "cutoff", 6000.0f); setP (proc, "unison", 1.0f);
            if (std::string (k.id) == "detune") setP (proc, "unison", 5.0f);
            if (std::string (k.id) == "resonance") setP (proc, "cutoff", 1500.0f);
            if (std::string (k.id) == "f2Mix") { setP (proc, "cutoff", 1200.0f); setP (proc, "f2Cutoff", 600.0f); }
            if (std::string (k.id) == "warpAmtA" || std::string (k.id) == "warpAmtB") { setP (proc, "warpModeA", 1.0f); setP (proc, "warpModeB", 1.0f); }
            if (std::string (k.id) == "distAmt") { setP (proc, "distType", 1.0f); setP (proc, "distMix", 1.0f); }
            if (std::string (k.id) == "fmAmount" || std::string (k.id) == "ringAmount") setP (proc, "oscBLevel", 0.3f);
            setP (proc, k.id, e ? k.hi : k.lo);
            auto r = render (proc, { 55 }, 1.6, 0.2);
            spec[e] = bands (r.m, 0.4, 1.5);
            lev[e] = rmsRange (r.m, 0.4, 1.5);
        }
        std::printf ("%-34s : spectre %.1f dB, niveau %+.1f dB\n", k.label, bandDist (spec[0], spec[1], false), db (lev[1]) - db (lev[0]));
    }

    // ------------------------------------------------------------------ E. Presets : attaque de note, craquements, disparition, CPU, variete
    std::printf ("\n== E. PRESETS (note C3 tenue 2 s, vel 0.8)\n");
    std::printf ("IDX|NOM|CAT|TAC dB|CRAQ/s|FIN-MAX dB|CPU %%\n");
    std::vector<std::vector<float>> fp;
    std::vector<juce::String> names;
    std::vector<double> cpus;
    int nTac = 0, nCrack = 0, nGone = 0;
    for (int p = 0; p < nPresets; ++p)
    {
        proc.loadFactoryPreset (p);
        const bool bass = MonSyntheProcessor::getFactoryPresetCategory (p) == "BASSES";
        const int note = bass ? 36 : 48;
        auto r = render (proc, { note }, 2.0, 0.6);
        const auto& x = r.m;

        // TAC : rapport haute frequence / basse frequence dans les 60 premieres ms, compare au regime etabli
        auto hfRatio = [&] (double t0, double t1)
        {
            double hf = 0, lf = 0; const size_t a = (size_t) (t0 * SR), b = (size_t) (t1 * SR);
            for (size_t i = a + 1; i < b && i < x.size(); ++i) { const double d = x[i] - x[i - 1]; hf += d * d; lf += (double) x[i] * x[i]; }
            return std::pair<double, double> (hf, lf);
        };
        float worst = -99.0f;
        double steadyR = 0;
        { auto [h, l] = hfRatio (1.0, 1.9); steadyR = h / juce::jmax (1.0e-20, l); }
        for (double t = 0.0; t < 0.08; t += 0.005)
        {
            auto [h, l] = hfRatio (t, t + 0.005);
            if (l / (0.005 * SR) < 1.0e-7) continue;                       // trop faible pour compter (< -70 dB)
            const double r2 = h / juce::jmax (1.0e-20, l);
            worst = juce::jmax (worst, db (std::sqrt (r2 / juce::jmax (1.0e-20, steadyR))));
        }

        // CRAQUEMENTS : impulsions isolees (derivee seconde) pendant le regime etabli
        int spikes = 0;
        {
            const size_t a = (size_t) (0.8 * SR), b = (size_t) (1.8 * SR);
            double s = 0; for (size_t i = a + 2; i < b; ++i) { const double d2 = x[i] - 2 * x[i - 1] + x[i - 2]; s += d2 * d2; }
            const double sd = std::sqrt (s / (double) (b - a));
            for (size_t i = a + 2; i < b; ++i)
            {
                const double d2 = x[i] - 2 * x[i - 1] + x[i - 2];
                if (sd > 1.0e-6 && std::abs (d2) > 12.0 * sd) ++spikes;
            }
        }

        // DISPARITION : niveau en fin de tenue par rapport au maximum
        float maxW = -200.0f;
        for (double t = 0.0; t < 1.8; t += 0.2) maxW = juce::jmax (maxW, db (rmsRange (x, t, t + 0.2)));
        const float endW = db (rmsRange (x, 1.7, 2.0));

        cpus.push_back (r.cpu);
        const char* flag = "";
        if (worst > 14.0f) { ++nTac; flag = " TAC"; }
        if (spikes > 20)   { ++nCrack; flag = " CRAQ"; }
        if (endW - maxW < -25.0f) { ++nGone; flag = " DISPARAIT"; }
        std::printf ("%d|%s|%s|%.1f|%d|%.1f|%.1f%s\n", p, MonSyntheProcessor::getFactoryPresetName (p).toRawUTF8(),
                     MonSyntheProcessor::getFactoryPresetCategory (p).toRawUTF8(), worst, spikes, endW - maxW, r.cpu * 100.0, flag);

        fp.push_back (bands (x, 0.3, 1.9));
        names.push_back (MonSyntheProcessor::getFactoryPresetName (p));
    }
    std::printf ("TAC (debut de note) : %d presets, CRAQUEMENTS : %d, DISPARITION : %d sur %d\n", nTac, nCrack, nGone, nPresets);

    double cpuAvg = 0, cpuMax = 0; for (double c : cpus) { cpuAvg += c; cpuMax = juce::jmax (cpuMax, c); }
    std::printf ("CPU (1 voix, machine du CI) : moyenne %.1f%%, max %.1f%%\n", 100.0 * cpuAvg / nPresets, 100.0 * cpuMax);

    // ------------------------------------------------------------------ F. Variete
    std::printf ("\n== F. VARIETE : distance spectrale entre presets (dB moyen par bande, forme seulement)\n");
    std::vector<float> nn (fp.size(), 1.0e9f);
    int close = 0; double sumNn = 0, sumAll = 0; long cnt = 0;
    std::vector<std::tuple<float, int, int>> pairs;
    for (size_t i = 0; i < fp.size(); ++i)
        for (size_t j = i + 1; j < fp.size(); ++j)
        {
            const float d = bandDist (fp[i], fp[j], true);
            nn[i] = juce::jmin (nn[i], d); nn[j] = juce::jmin (nn[j], d);
            sumAll += d; ++cnt; pairs.emplace_back (d, (int) i, (int) j);
        }
    for (float v : nn) { sumNn += v; if (v < 1.5f) ++close; }
    std::sort (pairs.begin(), pairs.end());
    std::printf ("distance moyenne entre deux presets : %.1f dB ; plus proche voisin moyen : %.1f dB\n", sumAll / (double) cnt, sumNn / (double) nn.size());
    std::printf ("presets ayant un quasi-jumeau (< 1.5 dB) : %d sur %d\n", close, (int) fp.size());
    std::printf ("10 paires les plus proches :\n");
    for (int k = 0; k < 10 && k < (int) pairs.size(); ++k)
        std::printf ("  %.2f dB : %s / %s\n", std::get<0> (pairs[(size_t) k]), names[(size_t) std::get<1> (pairs[(size_t) k])].toRawUTF8(), names[(size_t) std::get<2> (pairs[(size_t) k])].toRawUTF8());

    // ------------------------------------------------------------------ G. CPU en accord
    std::printf ("\n== G. CPU : accord de 8 notes (Init), machine du CI\n");
    proc.loadFactoryPreset (0);
    auto ch = render (proc, { 48, 52, 55, 59, 60, 64, 67, 71 }, 2.0, 0.5);
    std::printf ("Init, 8 notes : %.1f%% d'un coeur\n", ch.cpu * 100.0);
    proc.loadFactoryPreset (0); setP (proc, "unison", 7.0f); setP (proc, "oscBLevel", 0.6f);
    auto ch2 = render (proc, { 48, 52, 55, 59, 60, 64, 67, 71 }, 2.0, 0.5);
    std::printf ("Init, 8 notes, unison 7 + osc B : %.1f%% d'un coeur\n", ch2.cpu * 100.0);

    // ------------------------------------------------------------------ H. Accords enchaines (voix volees, queues longues)
    std::printf ("\n== H. ACCORDS ENCHAINES : 40 notes rapides sur nappe a longue queue (coupures et clics)\n");
    for (int pi : { 0, 67, 64, 77 })
    {
        proc.loadFactoryPreset (pi);
        setP (proc, "release", 4.0f);
        proc.reset();
        const int total = (int) (16.0 * SR / BLOCK);
        juce::AudioBuffer<float> buf (2, BLOCK);
        std::vector<float> x;
        const int scale[] = { 0, 3, 5, 7, 10, 12, 15, 17 };
        std::vector<std::pair<int, int>> offs;      // (bloc, note)
        for (int b = 0; b < total; ++b)
        {
            juce::MidiBuffer midi;
            const double t = (double) b * BLOCK / SR;
            const int step = (int) (t / 0.35), prevStep = (int) (((double) b * BLOCK - BLOCK) / SR / 0.35);
            if (b == 0 || step != prevStep)
            {
                const int n = 48 + scale[step % 8] + (step / 8) % 2 * 12;
                midi.addEvent (juce::MidiMessage::noteOn (1, n, 0.8f), 0);
                midi.addEvent (juce::MidiMessage::noteOn (1, n + 7, 0.8f), 0);
                offs.emplace_back (b + 26, n);          // tenue d'environ 0,3 s
                offs.emplace_back (b + 26, n + 7);
            }
            for (auto& o : offs)
                if (o.first == b)
                    midi.addEvent (juce::MidiMessage::noteOff (1, o.second), 0);
            buf.clear();
            proc.processBlock (buf, midi);
            for (int i = 0; i < BLOCK; ++i) x.push_back (0.5f * (buf.getSample (0, i) + buf.getSample (1, i)));
        }
        // clics : sauts de la derivee seconde, par rapport au niveau local
        int spikes = 0; float worst = 0.0f;
        for (size_t i = 4096; i + 2 < x.size(); i += 1)
        {
            if (i % 2048 == 0 || i == 4096) {}
            const double d2 = (double) x[i + 2] - 2.0 * x[i + 1] + x[i];
            double e = 0; for (size_t k = i - 2048; k < i; k += 16) { const double dd = (double) x[k + 2] - 2.0 * x[k + 1] + x[k]; e += dd * dd; }
            const double sd = std::sqrt (e / 128.0);
            if (sd > 1.0e-6 && std::abs (d2) > 12.0 * sd) { ++spikes; worst = juce::jmax (worst, (float) (std::abs (d2) / sd)); }
        }
        std::printf ("%s : %d clics detectes, pire %.0f sigma, niveau max %.1f dB\n", MonSyntheProcessor::getFactoryPresetName (pi).toRawUTF8(), spikes, worst, db (rmsRange (x, 2.0, 15.0)));
    }

    std::printf ("\nFIN\n");
    return 0;
}
