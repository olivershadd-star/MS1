#pragma once
// Briques « caractère » de MS1 (v1.1), sans dépendance à JUCE :
//   - NlFilter   : filtres non linéaires (échelle, diode, SVF sale, cascade) : chacun réagit autrement au signal
//   - ModChorus  : chorus stéréo à 2 taps par canal, lecture cubique (pas de bruit de lecture linéaire)
//   - Phaser6    : phaser à 6 étages, balayage exponentiel, réaction saturée
//   - unisonPattern : répartition du désaccord et intervalles de l'unison selon le mode
#include "Engine.h"
#include <cstring>

namespace ms
{
// tanh rationnel : précis à mieux que 2 % sur [-3, 3], borné au-delà. Bien moins cher que std::tanh.
inline float fastTanh (float x) noexcept
{
    if (x > 3.0f)  return 1.0f;
    if (x < -3.0f) return -1.0f;
    const float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

//==============================================================================
// Filtres à caractère. Chaque canal a ses états. Tout tourne à 2x la fréquence d'échantillonnage (2 sous-pas),
// la sortie est la moyenne des deux : les harmoniques de la saturation se replient moins.
//   type 0 ECHELLE : 4 pôles, tanh à chaque étage (rondeur, réaction à la dynamique, résonance qui s'écrase)
//   type 1 DIODE   : 3 pôles, retour passe-haut (la résonance mange moins le grave : son « acide »)
//   type 2 SALE LP / 3 SALE BP / 4 SALE HP : SVF dont la bande passante sature (résonance rugueuse)
//   type 5 CASCADE : deux SVF saturants en série (pente raide et grain)
struct NlFilter
{
    enum Type { Ladder = 0, Diode = 1, SvfLp = 2, SvfBp = 3, SvfHp = 4, Cascade = 5 };

    float s[2][4] {};
    float hp[2] {};
    float xPrev[2] {};
    int   type = 0;
    float g1 = 0.1f;      // coefficient d'un pôle (échelle, diode)
    float gT = 0.1f;      // tan(pi fc / fs2) pour les SVF
    float k = 0.0f;       // retour de l'échelle ou amortissement du SVF
    float drv = 1.6f;
    float hpA = 0.01f;
    float outGain = 1.0f;

    void reset() noexcept { std::memset (s, 0, sizeof (s)); std::memset (hp, 0, sizeof (hp)); std::memset (xPrev, 0, sizeof (xPrev)); }

    // fc en Hz, q de 0,5 à 10, drive de 0 à 1
    void set (int t, double sr, float fc, float q, float drive) noexcept
    {
        type = t;
        const double fs2 = sr * 2.0;
        fc = std::min (fc, (float) (0.46 * sr));
        g1 = (float) (1.0 - std::exp (-twoPi * (double) fc / fs2));
        gT = (float) std::tan (pi * (double) fc / fs2);
        drv = 1.6f + 3.0f * drive;
        const float r = std::min (1.0f, std::max (0.0f, (q - 0.5f) / 9.5f));
        hpA = (float) (1.0 - std::exp (-twoPi * 140.0 / fs2));
        switch (t)
        {
            case Ladder:
                k = 4.0f * r * (1.0f - 0.55f * g1);        // marge de stabilité vers les fréquences hautes
                outGain = (1.0f + 0.4f * k) / drv;
                break;
            case Diode:
                k = 4.6f * r * (1.0f - 0.55f * g1);
                outGain = (1.0f + 0.3f * k) / drv * 1.6f;
                break;
            default:
                k = 1.0f / std::max (0.5f, q);
                outGain = 1.0f;
                break;
        }
    }

    inline float ladderStep (int ch, float x, bool diode) noexcept
    {
        float* st = s[ch];
        const float fbSrc = diode ? (st[3] - hp[ch]) : st[3];
        if (diode) hp[ch] += hpA * (st[3] - hp[ch]);
        const float u = fastTanh (drv * x - k * fbSrc);
        st[0] += g1 * (u - fastTanh (st[0]));
        st[1] += g1 * (fastTanh (st[0]) - fastTanh (st[1]));
        st[2] += g1 * (fastTanh (st[1]) - fastTanh (st[2]));
        st[3] += g1 * (fastTanh (st[2]) - fastTanh (st[3]));
        return diode ? st[2] : st[3];
    }

    // SVF de Simper ; la bande passante est écrêtée avant d'être réinjectée
    inline void svfStep (float* ic, float x, float kk, float satD, float gg, float& lp, float& bp, float& hpOut) noexcept
    {
        const float a1 = 1.0f / (1.0f + gg * (gg + kk));
        const float a2 = gg * a1;
        const float a3 = gg * a2;
        const float v3 = x - ic[1];
        float v1 = a1 * ic[0] + a2 * v3;
        const float v2 = ic[1] + a2 * ic[0] + a3 * v3;
        v1 = fastTanh (v1 * satD) / satD;
        ic[0] = 2.0f * v1 - ic[0];
        ic[1] = 2.0f * v2 - ic[1];
        ic[0] = std::max (-8.0f, std::min (8.0f, ic[0]));
        ic[1] = std::max (-8.0f, std::min (8.0f, ic[1]));
        lp = v2; bp = v1; hpOut = x - kk * v1 - v2;
    }

    inline float sub (int ch, float x) noexcept
    {
        switch (type)
        {
            case Ladder: return ladderStep (ch, x, false) * outGain;
            case Diode:  return ladderStep (ch, x, true) * outGain;
            case SvfLp: case SvfBp: case SvfHp:
            {
                float lp, bp, h;
                svfStep (s[ch], x, k, drv * 1.2f, gT, lp, bp, h);
                return type == SvfLp ? lp : (type == SvfBp ? bp * 2.0f : h);
            }
            default:     // CASCADE
            {
                float lp, bp, h;
                svfStep (s[ch], x, k, drv * 0.9f, gT, lp, bp, h);
                float lp2, bp2, h2;
                svfStep (s[ch] + 2, lp, 0.9f, drv * 1.6f, gT * 1.6f, lp2, bp2, h2);
                return lp2;
            }
        }
    }

    inline float process (int ch, float x) noexcept
    {
        const float xm = 0.5f * (x + xPrev[ch]);
        xPrev[ch] = x;
        const float y1 = sub (ch, xm);
        const float y2 = sub (ch, x);
        return 0.5f * (y1 + y2);
    }
};

//==============================================================================
// Chorus : 2 taps par canal en opposition de phase, canaux en quadrature, interpolation cubique
struct ModChorus
{
    std::vector<float> buf[2];
    int size = 4096, mask = 4095, wp = 0;
    double sr = 44100.0, ph = 0.0;
    float dSm[2][2] { { 300.0f, 300.0f }, { 300.0f, 300.0f } };
    float fbState[2] {};

    void prepare (double sampleRate)
    {
        sr = sampleRate;
        size = 1; while ((double) size < sr * 0.08 + 8.0) size <<= 1;
        mask = size - 1;
        for (auto& b : buf) b.assign ((size_t) size, 0.0f);
        wp = 0; ph = 0.0;
        fbState[0] = fbState[1] = 0.0f;
    }

    inline float readCubic (const std::vector<float>& b, float delay) const noexcept
    {
        const float rp = (float) wp - delay;
        const int i = (int) std::floor (rp);
        const float fr = rp - (float) i;
        return cubic4 (b[(size_t) ((i - 1) & mask)], b[(size_t) (i & mask)], b[(size_t) ((i + 1) & mask)], b[(size_t) ((i + 2) & mask)], fr);
    }

    void process (float* l, float* r, int n, float rate, float depth, float centreMs, float feedback, float mix) noexcept
    {
        float* io[2] = { l, r };
        const double inc = (double) rate / sr;
        const float cSamp = centreMs * 0.001f * (float) sr;
        const float smooth = 0.002f;
        for (int i = 0; i < n; ++i)
        {
            ph += inc; if (ph >= 1.0) ph -= 1.0;
            for (int ch = 0; ch < 2; ++ch)
            {
                const double p1 = ph + 0.25 * ch, p2 = p1 + 0.5;
                const float m1 = (float) std::sin (twoPi * p1), m2 = (float) std::sin (twoPi * p2);
                const float want1 = std::max (4.0f, cSamp * (1.0f + 0.9f * depth * m1));
                const float want2 = std::max (4.0f, cSamp * (1.0f + 0.9f * depth * m2));
                dSm[ch][0] += smooth * (want1 - dSm[ch][0]);
                dSm[ch][1] += smooth * (want2 - dSm[ch][1]);
                const float in = io[ch][i];
                const float wet = 0.5f * (readCubic (buf[ch], dSm[ch][0]) + readCubic (buf[ch], dSm[ch][1]));
                buf[ch][(size_t) wp] = in + feedback * fbState[ch];
                fbState[ch] = wet;
                io[ch][i] = in * (1.0f - 0.5f * mix) + wet * (0.5f * mix);
            }
            wp = (wp + 1) & mask;
        }
    }
};

//==============================================================================
// Phaser : 6 allpass du premier ordre, centre balayé en octaves, retour écrêté
struct Phaser6
{
    float z[2][6] {};
    float fb[2] {};
    double ph = 0.0, sr = 44100.0;
    float aSm[2] { 0.0f, 0.0f };

    void prepare (double sampleRate) { sr = sampleRate; std::memset (z, 0, sizeof (z)); fb[0] = fb[1] = 0.0f; ph = 0.0; }

    void process (float* l, float* r, int n, float rate, float depth, float centre, float feedback, float mix) noexcept
    {
        float* io[2] = { l, r };
        const double inc = (double) rate / sr;
        for (int i = 0; i < n; ++i)
        {
            ph += inc; if (ph >= 1.0) ph -= 1.0;
            for (int ch = 0; ch < 2; ++ch)
            {
                const float lfo = (float) std::sin (twoPi * (ph + 0.125 * ch));
                const float fc = std::min ((float) (0.45 * sr), std::max (20.0f, centre * std::exp2 (depth * 2.0f * lfo)));
                const float t = std::tan ((float) pi * fc / (float) sr);
                const float a = (t - 1.0f) / (t + 1.0f);
                aSm[ch] += 0.02f * (a - aSm[ch]);
                const float in = io[ch][i];
                float y = in + feedback * fb[ch];
                for (int s = 0; s < 6; ++s)
                {
                    const float o = aSm[ch] * y + z[ch][s];
                    z[ch][s] = y - aSm[ch] * o;
                    y = o;
                }
                fb[ch] = fastTanh (y);
                io[ch][i] = in * (1.0f - 0.5f * mix) + y * (0.5f * mix);
            }
        }
    }
};

//==============================================================================
// Unison : écart de désaccord (multiplie le désaccord en cents) et intervalle d'accord (en demi-tons) de la voix u parmi n.
//   0 LINEAIRE  1 EXPO (centre serré, bords larges)  2 SUPER (centre large, bords proches)
//   3 OCTAVES  4 QUINTES  5 MINEUR  6 MAJEUR  7 CLUSTER (demi-tons serrés)
inline void unisonPattern (int mode, int u, int n, float& spreadOffset, float& semis) noexcept
{
    const float o = n > 1 ? -1.0f + 2.0f * (float) u / (float) (n - 1) : 0.0f;
    const float a = std::fabs (o);
    const float sgn = o < 0.0f ? -1.0f : 1.0f;
    semis = 0.0f;
    switch (mode)
    {
        case 1:  spreadOffset = sgn * a * a * 1.0f; break;
        case 2:  spreadOffset = sgn * std::pow (a, 0.55f); break;
        case 3:  { static const float t[7] = { 0, 12, -12, 24, -24, 12, -12 };  semis = t[u % 7]; spreadOffset = o * 0.25f; break; }
        case 4:  { static const float t[7] = { 0, 7, -5, 12, -12, 19, -17 };    semis = t[u % 7]; spreadOffset = o * 0.25f; break; }
        case 5:  { static const float t[7] = { 0, 3, 7, 12, -12, 15, -9 };      semis = t[u % 7]; spreadOffset = o * 0.25f; break; }
        case 6:  { static const float t[7] = { 0, 4, 7, 12, -12, 16, -8 };      semis = t[u % 7]; spreadOffset = o * 0.25f; break; }
        case 7:  { static const float t[7] = { 0, 1, -1, 2, -2, 3, -3 };        semis = t[u % 7] * 0.5f; spreadOffset = o * 0.25f; break; }
        default: spreadOffset = o; break;
    }
}
}
