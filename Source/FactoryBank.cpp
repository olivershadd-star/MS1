#include "FactoryBank.h"

//==============================================================================
// Banque d'usine de MS1 (v0.6) : presets recréés pour le nouveau moteur.
//
//   Macros (même sens dans presque tous les presets) : 1 BRILLANCE, 2 MOUVEMENT, 3 GRAIN, 4 LARGEUR
//   tableA / tableB : 0 BASIQUE 1 VOCAL 2 PWM 3 SPECTRE 4 ORGUE 5 RESONANCE 6 PEIGNE 7 CHAOS
//                     8 CLASSIQUE 9 CHOEUR 10 FM METAL 11 ORGUE ELEC 12 PIANO EP 13 CORDES 14 VENTS 15 GRANULAIRE
//                     16 DISTO 17 HARMONIQUES 18 CHIP 19 BASSES 20 GUITARES 21 DESSIN 22 SYNTH A 23 SYNTH B
//   filterType : 0 LP12  1 LP24  2 HP  3 BP  4 NOTCH  5 PEIGNE      lfoShape : 0 sinus 1 triangle 2 saw 3 carre
//   warpMode   : 0 OFF 1 BEND+ 2 BEND- 3 SYNC 4 MIRROR 5 ASYM 6 FLIP 7 QUANT 8 PWM 9 WOBBLE
//   lfoSync    : 0 libre 1 4 mes 2 2 mes 3 1 mes 4 1/2 5 1/4 6 1/8 7 1/16 8 1/32
//   mxNSrc : 1 LFO 2 ENV MOD 3 ENV FILTRE 4 VELOCITE 5 NOTE 6 MOLETTE 7 ALEA 8 LFO 2 9..12 MACRO 1..4 13 ENV PITCH
//            14 LFO 3 15 LFO 4 16 DESSIN
//   mxNDst : 1 POS A 2 POS B 3 CUTOFF 4 RESO 5 HAUTEUR 6 DESACCORD 7 NIVEAU B 8 DRIVE 9 VOLUME 10 QTE ENV FILTRE
//            11 QTE ENV MOD 12 DECAY VOL 13 DECAY FILTRE 14 WARP A 15 WARP B 16 FM 17 RING 18 DISTO 19 BRUIT
//            20 PITCH ENV 21 FILTRE 2 CUTOFF 22 FILTRE 2 VOYELLE
//==============================================================================

const std::vector<FactoryPreset>& factoryPresets()
{
    static const std::vector<FactoryPreset> presets =
    {
        //======================================================================== INIT
        { "Init", "INIT", {
 } },

        //======================================================================== BASSES
        { "Sub Fondation", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "tableA", 8.0f }, { "position", 0.02f }, { "subLevel", 0.55f },
            { "filterType", 1.0f }, { "cutoff", 520.0f }, { "resonance", 0.7f }, { "attack", 0.004f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f }, { "distType", 2.0f }, { "distAmt", 0.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.7f }, { "voiceGain", -2.5f } } },

        { "Reese Cathedrale", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 19.0f },
            { "position", 0.3f }, { "tableB", 19.0f }, { "oscBPos", 0.35f }, { "oscBLevel", 0.9f },
            { "oscBTune", 14.0f }, { "unison", 3.0f }, { "detune", 14.0f }, { "spread", 0.6f }, { "subLevel", 0.4f },
            { "filterType", 1.0f }, { "cutoff", 380.0f }, { "resonance", 1.2f }, { "drive", 0.15f },
            { "distType", 0.0f }, { "distAmt", 0.15f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f },
            { "release", 0.2f }, { "phaserMix", 0.22f }, { "phaserRate", 0.15f }, { "phaserFeedback", 0.4f },
            { "lfo3Rate", 0.12f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.28f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.55f }, { "mx3Src", 10.0f }, { "mx3Dst", 2.0f },
            { "mx3Amt", 0.6f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "mx5Src", 12.0f },
            { "mx5Dst", 6.0f }, { "mx5Amt", 0.7f }, { "voiceGain", -1.8f } } },

        { "Acide Zinc", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.06f }, { "tableA", 8.0f }, { "position", 0.85f }, { "filterType", 1.0f },
            { "cutoff", 450.0f }, { "resonance", 6.0f }, { "drive", 0.3f }, { "fltEnvAmount", 0.7f },
            { "fltAttack", 0.001f }, { "fltDecay", 0.18f }, { "fltSustain", 0.0f }, { "attack", 0.001f },
            { "decay", 0.3f }, { "sustain", 0.6f }, { "release", 0.08f }, { "distType", 4.0f }, { "distAmt", 0.2f },
            { "velFilter", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.6f } } },

        { "FM Griffe", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "tableA", 10.0f }, { "position", 0.2f }, { "tableB", 8.0f }, { "oscBPos", 0.05f },
            { "oscBLevel", 0.0f }, { "fmAmount", 0.4f }, { "subLevel", 0.45f }, { "filterType", 1.0f },
            { "cutoff", 1500.0f }, { "resonance", 1.0f }, { "distType", 2.0f }, { "distAmt", 0.3f },
            { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.15f }, { "mx1Src", 1.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx3Src", 10.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -2.5f } } },

        { "Wobble Sale", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 22.0f },
            { "position", 0.5f }, { "unison", 3.0f }, { "detune", 14.0f }, { "spread", 0.4f }, { "subLevel", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 380.0f }, { "resonance", 2.5f }, { "distType", 0.0f },
            { "distAmt", 0.3f }, { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.12f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.55f },
            { "mx2Src", 1.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 10.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f }, { "mx5Src", 11.0f },
            { "mx5Dst", 18.0f }, { "mx5Amt", 0.5f }, { "mx6Src", 12.0f }, { "mx6Dst", 6.0f }, { "mx6Amt", 0.5f },
            { "voiceGain", -3.6f } } },

        { "Gate Machine", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.02f }, { "tableA", 18.0f }, { "position", 0.4f }, { "subLevel", 0.4f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.4f }, { "distType", 1.0f },
            { "distAmt", 0.25f }, { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "attack", 0.002f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.04f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -1.8f } } },

        { "Ring Metal Bas", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.05f }, { "tableA", 8.0f }, { "position", 0.1f }, { "tableB", 10.0f }, { "oscBPos", 0.1f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.0f }, { "ringAmount", 0.6f }, { "subLevel", 0.4f },
            { "filterType", 0.0f }, { "cutoff", 1800.0f }, { "resonance", 0.9f }, { "attack", 0.003f },
            { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 2.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 17.0f }, { "mx3Amt", 0.4f } } },

        { "Comb Laser", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "tableA", 8.0f }, { "position", 0.8f }, { "subLevel", 0.5f }, { "filterType", 5.0f },
            { "cutoff", 220.0f }, { "resonance", 5.0f }, { "fltEnvAmount", 0.6f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.15f }, { "attack", 0.002f }, { "decay", 0.35f },
            { "sustain", 0.7f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Beton Brut", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "tableA", 16.0f }, { "position", 0.5f }, { "subLevel", 0.5f }, { "drive", 0.3f },
            { "distType", 1.0f }, { "distAmt", 0.5f }, { "filterType", 1.0f }, { "cutoff", 700.0f },
            { "resonance", 1.5f }, { "noiseLevel", 0.08f }, { "noiseType", 2.0f }, { "noiseTone", 0.3f },
            { "noiseDecay", 4.0f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "voiceGain", -4.9f } } },

        { "Pompe Sidechain", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "tableA", 8.0f }, { "position", 0.8f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 800.0f }, { "resonance", 1.2f }, { "lfoShape", 2.0f }, { "lfoSync", 5.0f },
            { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 1.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f } } },

        //======================================================================== NAPPES
        { "Brume Choeur", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 9.0f }, { "position", 0.35f }, { "tableB", 9.0f },
            { "oscBPos", 0.45f }, { "oscBLevel", 0.6f }, { "oscBTune", -9.0f }, { "unison", 5.0f },
            { "detune", 11.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 0.8f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.45f },
            { "reverbSize", 0.9f }, { "lfoShape", 0.0f }, { "lfoSync", 3.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 1.0f },
            { "mx1Amt", 0.25f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f },
            { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f } } },

        { "Cathedrale Orgue", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 11.0f }, { "position", 0.3f }, { "unison", 3.0f },
            { "detune", 8.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.5f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.0f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "fx1Type", 11.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.5f }, { "fx1A", 0.2f }, { "fx1B", 0.5f },
            { "fx1C", 0.5f }, { "fx1D", 0.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Cordes Fantomes", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 13.0f }, { "position", 0.4f },
            { "tableB", 13.0f }, { "oscBPos", 0.6f }, { "oscBLevel", 0.7f }, { "oscBTune", 7.0f }, { "unison", 5.0f },
            { "detune", 13.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 3500.0f },
            { "resonance", 0.8f }, { "attack", 0.9f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.2f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f }, { "lfo3Rate", 0.1f },
            { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 2.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f },
            { "mx5Amt", 0.6f } } },

        { "Verre Depoli", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 17.0f }, { "position", 0.5f }, { "unison", 5.0f },
            { "detune", 9.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "attack", 1.2f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "fx1Type", 8.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.3f }, { "fx1A", 0.75f }, { "fx1B", 0.4f },
            { "fx1C", 0.5f }, { "fx1D", 0.6f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f }, { "voiceGain", 4.0f } } },

        { "Souffle Nord", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 14.0f }, { "position", 0.3f },
            { "tableB", 14.0f }, { "oscBPos", 0.5f }, { "oscBLevel", 0.6f }, { "unison", 5.0f }, { "detune", 10.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.9f }, { "decay", 0.5f },
            { "sustain", 1.0f }, { "release", 2.4f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f }, { "f2Mix", 0.5f }, { "f2Vowel", 0.0f },
            { "f2Mode", 0.0f }, { "lfo3Rate", 0.08f }, { "lfo3Shape", 0.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 22.0f },
            { "mx1Amt", 0.8f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx3Src", 10.0f },
            { "mx3Dst", 22.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f },
            { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f }, { "voiceGain", -2.0f } } },

        { "Rouille Lente", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 22.0f }, { "position", 0.3f }, { "unison", 7.0f },
            { "detune", 18.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 1500.0f },
            { "resonance", 1.5f }, { "distType", 0.0f }, { "distAmt", 0.15f }, { "attack", 1.0f }, { "decay", 0.5f },
            { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "dwRate", 0.1f }, { "dwSync", 0.0f }, { "dwSmooth", 0.9f }, { "dwSteps", 10.0f }, { "draw1", -1.0f },
            { "draw2", -0.6f }, { "draw3", -0.1f }, { "draw4", 0.4f }, { "draw5", 0.9f }, { "draw6", 1.0f },
            { "draw7", 0.6f }, { "draw8", 0.1f }, { "draw9", -0.4f }, { "draw10", -0.8f }, { "mx1Src", 16.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.6f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.5f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.7f } } },

        { "Nuit Planetaire", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 15.0f }, { "position", 0.4f },
            { "tableB", 15.0f }, { "oscBPos", 0.65f }, { "oscBLevel", 0.5f }, { "unison", 5.0f }, { "detune", 12.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.5f }, { "decay", 0.5f },
            { "sustain", 1.0f }, { "release", 3.5f }, { "delayMode", 1.0f }, { "delaySync", 4.0f },
            { "delayMix", 0.3f }, { "delayFeedback", 0.5f }, { "delayLP", 4500.0f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.95f }, { "lfo2Rate", 0.07f }, { "lfo2Shape", 1.0f }, { "mx1Src", 8.0f },
            { "mx1Dst", 2.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f } } },

        { "Metal Doux", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 10.0f }, { "position", 0.5f },
            { "fmAmount", 0.2f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.9f }, { "filterType", 0.0f },
            { "cutoff", 4500.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.0f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.85f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f } } },

        { "Piano Fantome", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 12.0f }, { "position", 0.5f }, { "unison", 3.0f },
            { "detune", 9.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 0.35f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.5f }, { "chorusMix", 0.4f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "fx1Type", 17.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.45f },
            { "fx1B", 0.2f }, { "fx1C", 0.4f }, { "fx1D", 1.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Cire Chaude", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 8.0f }, { "position", 0.5f }, { "tableB", 8.0f },
            { "oscBPos", 0.6f }, { "oscBLevel", 0.7f }, { "oscBTune", 7.0f }, { "unison", 5.0f }, { "detune", 12.0f },
            { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 2200.0f }, { "resonance", 1.0f },
            { "attack", 0.7f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.0f }, { "phaserMix", 0.3f },
            { "phaserRate", 0.12f }, { "phaserFeedback", 0.4f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 2.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 12.0f },
            { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f } } },

        //======================================================================== LEADS
        { "Lame Acier", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 8.0f },
            { "position", 0.85f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.6f }, { "filterType", 1.0f },
            { "cutoff", 3500.0f }, { "resonance", 1.8f }, { "fltEnvAmount", 0.5f }, { "fltAttack", 0.005f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.4f }, { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.25f }, { "velFilter", 0.4f }, { "delayMode", 1.0f }, { "delaySync", 3.0f },
            { "delayMix", 0.3f }, { "delayFeedback", 0.4f }, { "delayLP", 5000.0f }, { "reverbMix", 0.25f },
            { "reverbSize", 0.6f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Sync Laser", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 8.0f },
            { "position", 0.5f }, { "unison", 2.0f }, { "detune", 8.0f }, { "spread", 0.5f }, { "warpModeA", 3.0f },
            { "warpAmtA", 0.3f }, { "modDecay", 0.35f }, { "modSustain", 0.0f }, { "filterType", 1.0f },
            { "cutoff", 3500.0f }, { "resonance", 1.4f }, { "attack", 0.002f }, { "decay", 0.25f },
            { "sustain", 0.85f }, { "release", 0.2f }, { "reverbMix", 0.2f }, { "mx1Src", 2.0f }, { "mx1Dst", 14.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx3Src", 10.0f },
            { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f },
            { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.5f } } },

        { "Voix Robot", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 9.0f },
            { "position", 0.4f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.25f }, { "chorusMix", 0.3f },
            { "reverbMix", 0.25f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f }, { "f2Mix", 0.9f },
            { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "lfo3Rate", 0.9f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f },
            { "mx1Dst", 22.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f },
            { "mx3Src", 10.0f }, { "mx3Dst", 22.0f }, { "mx3Amt", 0.7f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f }, { "voiceGain", -2.0f } } },

        { "Fer Chaud", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 16.0f },
            { "position", 0.3f }, { "unison", 3.0f }, { "detune", 8.0f }, { "spread", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 3000.0f }, { "resonance", 1.5f }, { "distType", 2.0f }, { "distAmt", 0.3f },
            { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.2f }, { "reverbMix", 0.15f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 12.0f },
            { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -2.5f } } },

        { "Cristal FM", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 10.0f }, { "position", 0.6f }, { "tableB", 8.0f },
            { "oscBPos", 0.05f }, { "oscBLevel", 0.0f }, { "fmAmount", 0.35f }, { "unison", 2.0f }, { "detune", 6.0f },
            { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 0.004f }, { "decay", 0.3f },
            { "sustain", 0.6f }, { "release", 0.3f }, { "modDecay", 0.3f }, { "modSustain", 0.0f },
            { "delayMode", 1.0f }, { "delaySync", 3.0f }, { "delayMix", 0.3f }, { "delayFeedback", 0.45f },
            { "delayLP", 6000.0f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f }, { "mx1Src", 2.0f },
            { "mx1Dst", 16.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.6f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.5f } } },

        { "Chip Acide", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 18.0f },
            { "position", 0.5f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.3f }, { "filterType", 1.0f },
            { "cutoff", 2500.0f }, { "resonance", 2.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.2f },
            { "fltSustain", 0.2f }, { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 0.7f }, { "release", 0.1f },
            { "delayMode", 1.0f }, { "delaySync", 3.0f }, { "delayMix", 0.25f }, { "delayFeedback", 0.4f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 14.0f },
            { "mx2Amt", 0.6f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Orgue Sale", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 11.0f }, { "position", 0.55f },
            { "unison", 2.0f }, { "detune", 6.0f }, { "spread", 0.6f }, { "distType", 4.0f }, { "distAmt", 0.2f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 1.0f },
            { "release", 0.25f }, { "reverbMix", 0.25f }, { "fx1Type", 11.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.5f },
            { "fx1A", 0.2f }, { "fx1B", 0.5f }, { "fx1C", 0.5f }, { "fx1D", 0.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f },
            { "mx4Amt", 0.5f } } },

        { "Pluck Verre", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 17.0f }, { "position", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 4000.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.5f },
            { "fltAttack", 0.001f }, { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "attack", 0.001f },
            { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.25f }, { "velFilter", 0.4f },
            { "delayMode", 1.0f }, { "delaySync", 3.0f }, { "delayMix", 0.35f }, { "delayFeedback", 0.5f },
            { "delayLP", 5500.0f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f },
            { "mx4Amt", 0.4f }, { "voiceGain", 8.0f } } },

        { "Corde Indus", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 20.0f }, { "position", 0.5f }, { "unison", 1.0f },
            { "distType", 4.0f }, { "distAmt", 0.3f }, { "filterType", 0.0f }, { "cutoff", 4500.0f },
            { "attack", 0.001f }, { "decay", 0.45f }, { "sustain", 0.0f }, { "release", 0.3f }, { "delayMode", 1.0f },
            { "delaySync", 4.0f }, { "delayMix", 0.3f }, { "delayFeedback", 0.45f }, { "delayLP", 5000.0f },
            { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Lead Dessine", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 8.0f },
            { "position", 0.8f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.6f }, { "filterType", 1.0f },
            { "cutoff", 1800.0f }, { "resonance", 2.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.25f }, { "reverbMix", 0.25f }, { "f2Type", 1.0f }, { "f2Cutoff", 3000.0f },
            { "f2Res", 0.9f }, { "f2Mix", 0.5f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "dwRate", 1.0f },
            { "dwSync", 6.0f }, { "dwSmooth", 0.4f }, { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", -0.5f },
            { "draw3", 0.3f }, { "draw4", 1.0f }, { "draw5", 0.6f }, { "draw6", -0.2f }, { "draw7", -0.8f },
            { "draw8", 0.5f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f }, { "mx2Src", 16.0f },
            { "mx2Dst", 21.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f },
            { "mx5Amt", 0.5f }, { "mx6Src", 12.0f }, { "mx6Dst", 6.0f }, { "mx6Amt", 0.5f } } },

        //======================================================================== TEXTURES
        { "Pluie Acide", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.2f }, { "tableA", 15.0f },
            { "position", 0.5f }, { "noiseLevel", 0.6f }, { "noiseType", 1.0f }, { "noiseTone", 0.5f },
            { "filterType", 3.0f }, { "cutoff", 2200.0f }, { "resonance", 5.0f }, { "attack", 1.0f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "dwRate", 0.3f }, { "dwSync", 0.0f }, { "dwSmooth", 0.6f }, { "dwSteps", 8.0f }, { "draw1", -1.0f },
            { "draw2", 0.8f }, { "draw3", -0.4f }, { "draw4", 1.0f }, { "draw5", -0.9f }, { "draw6", 0.2f },
            { "draw7", 0.6f }, { "draw8", -0.7f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f }, { "voiceGain", 9.0f } } },

        { "Usine Lointaine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 16.0f }, { "position", 0.6f },
            { "noiseLevel", 0.3f }, { "noiseType", 2.0f }, { "noiseTone", 0.4f }, { "unison", 3.0f },
            { "detune", 10.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 900.0f }, { "attack", 1.5f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.6f },
            { "reverbSize", 0.95f }, { "fx1Type", 1.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.3f }, { "fx1A", 0.55f },
            { "fx1B", 0.4f }, { "fx1C", 0.3f }, { "fx1D", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.6f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Respiration", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.3f }, { "tableA", 14.0f },
            { "position", 0.4f }, { "noiseLevel", 0.5f }, { "noiseType", 1.0f }, { "noiseTone", 0.6f },
            { "filterType", 3.0f }, { "cutoff", 1200.0f }, { "resonance", 3.0f }, { "attack", 1.5f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "lfoShape", 0.0f }, { "lfoSync", 2.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f }, { "voiceGain", 3.0f } } },

        { "Grain Eclat", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 15.0f }, { "position", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 4500.0f }, { "resonance", 1.0f }, { "attack", 0.001f },
            { "decay", 0.12f }, { "sustain", 0.0f }, { "release", 0.15f }, { "delayMode", 1.0f },
            { "delaySync", 5.0f }, { "delayMix", 0.35f }, { "delayFeedback", 0.5f }, { "delayLP", 5000.0f },
            { "reverbMix", 0.4f }, { "mx1Src", 7.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.9f }, { "mx2Src", 7.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.6f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f },
            { "mx5Amt", 0.4f }, { "voiceGain", 2.0f } } },

        { "Bourdon Metallique", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 10.0f }, { "position", 0.3f },
            { "tableB", 10.0f }, { "oscBPos", 0.7f }, { "oscBLevel", 0.8f }, { "oscBTune", 7.0f }, { "unison", 7.0f },
            { "detune", 25.0f }, { "spread", 1.0f }, { "ringAmount", 0.3f }, { "filterType", 0.0f },
            { "cutoff", 2500.0f }, { "attack", 1.2f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.5f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 2.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 17.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f } } },

        { "Echo Machine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 21.0f }, { "position", 0.5f }, { "unison", 3.0f },
            { "detune", 8.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 0.3f },
            { "decay", 0.4f }, { "sustain", 0.8f }, { "release", 1.5f }, { "delayMode", 1.0f }, { "delaySync", 4.0f },
            { "delayMix", 0.45f }, { "delayFeedback", 0.62f }, { "delayLP", 5000.0f }, { "reverbMix", 0.35f },
            { "reverbSize", 0.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Glace Noire", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 17.0f }, { "position", 0.7f }, { "unison", 5.0f },
            { "detune", 12.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 10000.0f },
            { "attack", 2.0f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 4.0f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.95f }, { "fx1Type", 8.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.4f }, { "fx1A", 0.75f },
            { "fx1B", 0.6f }, { "fx1C", 0.5f }, { "fx1D", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.3f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f },
            { "voiceGain", 3.0f } } },

        { "Sirene Lente", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 8.0f }, { "position", 0.3f }, { "unison", 3.0f },
            { "detune", 10.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "attack", 1.2f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "lfoShape", 0.0f }, { "lfoRate", 0.12f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.9f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.12f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f },
            { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.5f }, { "voiceGain", -2.5f } } },

        { "Cables", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 20.0f }, { "position", 0.8f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.002f }, { "decay", 1.2f },
            { "sustain", 0.0f }, { "release", 1.5f }, { "delayMode", 1.0f }, { "delaySync", 4.0f },
            { "delayMix", 0.4f }, { "delayFeedback", 0.55f }, { "delayLP", 5000.0f }, { "reverbMix", 0.4f },
            { "fx1Type", 4.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.5f }, { "fx1A", 0.4f }, { "fx1B", 0.7f },
            { "fx1C", 0.5f }, { "fx1D", 0.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Vent Solaire", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 22.0f }, { "position", 0.6f }, { "unison", 5.0f },
            { "detune", 14.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 2.0f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.5f }, { "reverbMix", 0.6f },
            { "reverbSize", 0.95f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f }, { "f2Mix", 0.6f },
            { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "dwRate", 0.08f }, { "dwSync", 0.0f }, { "dwSmooth", 0.9f },
            { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", -0.5f }, { "draw3", 0.0f }, { "draw4", 0.5f },
            { "draw5", 1.0f }, { "draw6", 0.5f }, { "draw7", 0.0f }, { "draw8", -0.5f }, { "mx1Src", 16.0f },
            { "mx1Dst", 22.0f }, { "mx1Amt", 0.9f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 22.0f }, { "mx3Amt", 0.6f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.3f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f } } }
    };

    return presets;
}
