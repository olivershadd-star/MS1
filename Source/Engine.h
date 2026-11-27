#pragma once
// Briques DSP du moteur MS1 (v1.0), sans dépendance à JUCE : testables seules.
//   - cubic4      : interpolation cubique (Catmull-Rom) pour la lecture des tables d'ondes
//   - ModalBank   : banc de résonateurs modaux (cordes, barres, cloches, tubes, plaques, bols)
//   - FdnReverb   : réverbération à réseau de lignes à retard (8 lignes, matrice de Householder)
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace ms
{
constexpr double pi    = 3.14159265358979323846;
constexpr double twoPi = 6.28318530717958647692;

inline float cubic4 (float p0, float p1, float p2, float p3, float fr) noexcept
{
    const float c1 = 0.5f * (p2 - p0);
    const float c2 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    const float c3 = 0.5f * (p3 - p0) + 1.5f * (p1 - p2);
    return ((c3 * fr + c2) * fr + c1) * fr + p1;
}

// log(cosh(x)) stable : primitive de tanh (antialiasing par primitive, ADAA)
inline float logCosh (float x) noexcept
{
    x = std::fabs (x);
    return x + std::log1p (std::exp (-2.0f * x)) - 0.69314718f;
}

// Nombre d'harmoniques d'une version limitée en bande : niveau L couvre [20 * 2^(L/2), 20 * 2^((L+1)/2)[ Hz.
inline int maxHarmonicsForLevel (int level, int cap = 512) noexcept
{
    const double top = 20.0 * std::pow (2.0, (level + 1) * 0.5);
    return std::max (1, std::min (cap, (int) (21000.0 / top)));
}

inline int levelForFrequencyHalfOctave (double hz, int numLevels) noexcept
{
    const int l = (int) std::floor (2.0 * std::log2 (std::max (1.0, hz) / 20.0));
    return std::max (0, std::min (numLevels - 1, l));
}

//==============================================================================
// Banc de résonateurs modaux. Chaque mode est un résonateur à deux pôles :
//   y[n] = b * x[n] + 2 r cos(w) y[n-1] - r^2 y[n-2]
// excité par une courte salve de bruit (marteau, plectre, archet, souffle) et/ou par un signal continu (corps).
struct ModalBank
{
    static constexpr int maxModes = 12;

    struct Mode { float c1 = 0, c2 = 0, bExc = 0, bFeed = 0, y1 = 0, y2 = 0; };
    std::array<Mode, maxModes> m {};
    int n = 0;
    float outNorm = 1.0f;

    static constexpr int numMaterials = 7;   // CORDE BARRE MARIMBA CLOCHE TUBE PLAQUE BOL

    void reset() noexcept { for (auto& k : m) k.y1 = k.y2 = 0.0f; }

    // Rapports de fréquence et amplitudes de base de chaque matériau.
    static int ratios (int material, float stretch, float* r, float* a) noexcept
    {
        static const float bar[8]   = { 1.0f, 2.756f, 5.404f, 8.933f, 13.344f, 18.638f, 24.81f, 31.87f };
        static const float mar[8]   = { 1.0f, 3.97f, 9.90f, 17.7f, 27.2f, 38.5f, 51.5f, 66.3f };
        static const float bell[12] = { 0.5f, 1.0f, 1.183f, 1.506f, 2.0f, 2.514f, 2.662f, 3.011f, 4.166f, 5.433f, 6.796f, 8.215f };
        static const float bellA[12]= { 0.55f, 1.0f, 0.7f, 0.55f, 0.8f, 0.4f, 0.35f, 0.3f, 0.25f, 0.2f, 0.15f, 0.1f };
        static const float plate[12]= { 1.0f, 1.58f, 2.24f, 2.9f, 3.51f, 4.17f, 4.9f, 5.72f, 6.5f, 7.4f, 8.3f, 9.6f };
        static const float bowl[8]  = { 1.0f, 2.71f, 5.15f, 8.46f, 12.55f, 17.4f, 22.9f, 29.2f };
        static const float bowlA[8] = { 1.0f, 0.7f, 0.5f, 0.35f, 0.25f, 0.18f, 0.12f, 0.08f };

        const float s = std::max (0.0f, std::min (1.0f, stretch));
        int cnt = 0;

        auto stretchOthers = [&] (int count, const float* base, const float* amp, float slope)
        {
            for (int i = 0; i < count; ++i)
            {
                r[i] = base[0] * std::pow (base[i] / base[0], 1.0f + 0.2f * s);
                a[i] = amp != nullptr ? amp[i] : std::pow (base[i], -slope);
            }
            cnt = count;
        };

        switch (material)
        {
            case 1:  stretchOthers (8, bar, nullptr, 0.8f); break;
            case 2:  stretchOthers (8, mar, nullptr, 1.1f); break;
            case 3:  stretchOthers (12, bell, bellA, 0.0f); break;
            case 5:  stretchOthers (12, plate, nullptr, 0.6f); break;
            case 6:  stretchOthers (8, bowl, bowlA, 0.0f); break;
            case 4:                                                  // TUBE : harmoniques impairs
                for (int i = 0; i < 12; ++i)
                {
                    const float k = (float) (2 * i + 1);
                    r[i] = k * std::sqrt (1.0f + s * s * 0.004f * k * k);
                    a[i] = std::pow (k, -0.9f);
                }
                cnt = 12;
                break;
            default:                                                 // CORDE : harmoniques raidis (piano)
                for (int i = 0; i < 12; ++i)
                {
                    const float k = (float) (i + 1);
                    r[i] = k * std::sqrt (1.0f + s * s * 0.01f * k * k);
                    a[i] = 1.0f / k;
                }
                cnt = 12;
                break;
        }
        return cnt;
    }

    // f0 : fréquence du premier mode ; T60 : durée (s) à -60 dB du premier mode ;
    // damp : les modes aigus s'éteignent plus vite (0 = tous pareil) ; pos : point d'excitation (0..0,5)
    void setup (double sr, double f0, int material, float stretch, float T60, float damp, float pos) noexcept
    {
        float r[maxModes] {}, a[maxModes] {};
        const int cnt = ratios (material, stretch, r, a);
        n = 0;
        double sumsq = 0.0;

        for (int i = 0; i < cnt; ++i)
        {
            const double f = f0 * (double) r[i];
            if (f > 0.45 * sr || f < 12.0)
                continue;

            const double w = twoPi * f / sr;
            float amp = a[i];
            const float comb = std::fabs (std::sin ((float) pi * r[i] * pos));
            amp *= (material == 0 || material == 4) ? 0.12f + 0.88f * comb : 0.45f + 0.55f * comb;

            const float tk = std::max (0.01f, T60 * std::pow (std::max (r[i], 0.25f), -damp * 1.2f));
            const double rr = std::exp (-6.907755 / ((double) tk * sr));

            Mode& md = m[(size_t) n++];
            md.c1 = (float) (2.0 * rr * std::cos (w));
            md.c2 = (float) (rr * rr);
            md.bExc = amp * (float) std::sin (w);
            md.bFeed = (float) (2.0 * (1.0 - rr) * std::sin (w));
            sumsq += (double) amp * amp;
        }
        outNorm = sumsq > 1.0e-9 ? (float) (1.0 / std::sqrt (sumsq)) : 0.0f;
    }

    // exc : excitation (salve de bruit), feed : signal continu envoyé dans le corps
    float process (float exc, float feed) noexcept
    {
        float out = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            Mode& k = m[(size_t) i];
            const float y = k.bExc * exc + k.bFeed * feed + k.c1 * k.y1 - k.c2 * k.y2;
            k.y2 = k.y1;
            k.y1 = y;
            out += y;
        }
        return out * outNorm;
    }
};

//==============================================================================
// Réverbération à réseau de retards (FDN) : 8 lignes, matrice de Householder, amortissement des aigus
// dans la boucle, lignes légèrement modulées (pas de résonance métallique), diffusion à l'entrée.
class FdnReverb
{
public:
    static constexpr int N = 8;

    void prepare (double sampleRate)
    {
        sr = sampleRate;
        const int cap = 1 << 16;
        for (auto& b : buf) b.assign ((size_t) cap, 0.0f);
        mask = cap - 1;
        for (auto& a : ap) { a.assign (1024, 0.0f); }
        reset();
        setParams (0.5f, 0.5f);
    }

    void reset()
    {
        for (auto& b : buf) std::fill (b.begin(), b.end(), 0.0f);
        for (auto& a : ap) std::fill (a.begin(), a.end(), 0.0f);
        lp.fill (0.0f);
        wp = 0;
        for (auto& p : apPos) p = 0;
        for (int i = 0; i < N; ++i) phase[(size_t) i] = (double) i / (double) N;
    }

    void setParams (float size, float damp)
    {
        static const double base[N] = { 1117, 1327, 1559, 1783, 2003, 2221, 2437, 2663 };
        const double sc = (0.45 + 1.35 * (double) size) * sr / 44100.0;
        const double rt = 0.25 + 9.0 * (double) size * (double) size;
        for (int i = 0; i < N; ++i)
        {
            len[(size_t) i] = base[i] * sc;
            gain[(size_t) i] = (float) std::pow (10.0, -3.0 * len[(size_t) i] / (sr * rt));
        }
        const double fc = 18000.0 * std::pow (0.04, (double) damp);
        kd = (float) (1.0 - std::exp (-twoPi * fc / sr));
        modDepth = (float) (2.0 + 2.0 * (double) size) * (float) (sr / 44100.0);
    }

    // Traitement en place, signal 100 % réverbéré (le mélange sec/humide est géré à l'extérieur)
    void process (float* l, float* r, int numSamples)
    {
        static const double rate[N] = { 0.13, 0.17, 0.21, 0.25, 0.29, 0.31, 0.19, 0.23 };
        const float in0 = 0.5f;
        for (int s = 0; s < numSamples; ++s)
        {
            float inL = l[s], inR = r[s];
            inL = allpass (0, inL); inL = allpass (1, inL);
            inR = allpass (2, inR); inR = allpass (3, inR);

            float v[N];
            float sum = 0.0f;
            for (int i = 0; i < N; ++i)
            {
                const double md = modDepth * std::sin (twoPi * phase[(size_t) i]);
                phase[(size_t) i] += rate[i] / sr;
                if (phase[(size_t) i] >= 1.0) phase[(size_t) i] -= 1.0;

                const double rp = (double) wp - len[(size_t) i] - md;
                const double fl = std::floor (rp);
                const int i0 = (int) fl;
                const float fr = (float) (rp - fl);
                const auto& b = buf[(size_t) i];
                float d = b[(size_t) (i0 & mask)] * (1.0f - fr) + b[(size_t) ((i0 + 1) & mask)] * fr;

                lp[(size_t) i] += kd * (d - lp[(size_t) i]);
                v[i] = lp[(size_t) i] * gain[(size_t) i];
                sum += v[i];
            }

            const float h = sum * (2.0f / (float) N);
            float outL = 0.0f, outR = 0.0f;
            for (int i = 0; i < N; ++i)
            {
                const float fb = v[i] - h;
                const float x = (i & 1) ? inR : inL;
                const float sgn = (i & 2) ? -1.0f : 1.0f;
                buf[(size_t) i][(size_t) (wp & mask)] = fb + x * in0 * sgn;
                if (i & 1) outR += v[i] * ((i & 2) ? -1.0f : 1.0f);
                else       outL += v[i] * ((i & 2) ? -1.0f : 1.0f);
            }
            ++wp;
            l[s] = outL * outGain;
            r[s] = outR * outGain;
        }
    }

    float outGain = 1.2f;

private:
    float allpass (int k, float x)
    {
        static const int lens[4] = { 142, 379, 107, 277 };
        auto& b = ap[(size_t) k];
        int& p = apPos[(size_t) k];
        const int L = lens[k];
        const float d = b[(size_t) p];
        const float y = d - 0.6f * x;
        b[(size_t) p] = x + 0.6f * y;
        p = p + 1 >= L ? 0 : p + 1;
        return y;
    }

    double sr = 44100.0;
    std::array<std::vector<float>, N> buf;
    std::array<std::vector<float>, 4> ap;
    std::array<int, 4> apPos {};
    std::array<double, N> len {};
    std::array<float, N> gain {}, lp {};
    std::array<double, N> phase {};
    int wp = 0, mask = 0;
    float kd = 0.5f, modDepth = 2.0f;
};
}
