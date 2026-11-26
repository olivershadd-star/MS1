#pragma once

// Données des wavetables dérivées d'AKWF (Adventure Kid Waveforms, licence CC0), stockées en blocs de texte (base64).
namespace WavetableData
{
    const char* const* chunks();
    int numChunks();
}
