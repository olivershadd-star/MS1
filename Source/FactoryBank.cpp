#include "FactoryBank.h"

//==============================================================================
// Banque d'usine de MS1
//
//   tableA / tableB : 0 BASIQUE 1 VOCAL 2 PWM 3 SPECTRE 4 ORGUE 5 RESONANCE 6 PEIGNE 7 CHAOS
//   filterType : 0 LP12  1 LP24  2 HP  3 BP        lfoShape : 0 sinus 1 triangle 2 saw 3 carre
//   warpMode   : 0 OFF 1 BEND+ 2 BEND- 3 SYNC 4 MIRROR
//   lfoSync    : 0 libre 1 4 mes 2 2 mes 3 1 mes 4 1/2 5 1/4 6 1/8 7 1/16 8 1/32
//   mxNSrc : 1 LFO 2 ENV MOD 3 ENV FILTRE 4 VELOCITE 5 NOTE 6 MOLETTE 7 ALEA 8 LFO 2 9..12 MACRO 1..4
//   mxNDst : 1 POS A 2 POS B 3 CUTOFF 4 RESO 5 HAUTEUR 6 DESACCORD 7 NIVEAU B 8 DRIVE 9 VOLUME
//            10 QTE ENV FILTRE 11 QTE ENV MOD 12 DECAY VOL 13 DECAY FILTRE 14 WARP A 15 WARP B 16 FM 17 RING
//            18 DISTO 19 NIVEAU BRUIT 20 PITCH ENV      (source 13 = ENV PITCH)
//   warpMode : 5 ASYM 6 FLIP 7 QUANT 8 PWM 9 WOBBLE     distType : 0 SOFT 1 HARD 2 FOLD 3 CRUSH 4 ASYM
//   noiseType : 0 BLANC 1 ROSE 2 SALE     phaseMode : 0 ALEATOIRE 1 FIXE 2 LIBRE
//   mxNSrc 14 LFO 3, 15 LFO 4 ; macros 1..4 pre-branchees dans la plupart des presets (cutoff, disto, position, desaccord...)
//
// Rappel : "lfoDepth" et "modAmount" bougent la position de la wavetable par défaut ; on les met à 0
// quand le preset ne doit pas bouger tout seul.
// Les presets sont rangés par catégorie : l'ordre des catégories dans cette liste est celui du menu.
//==============================================================================
const std::vector<FactoryPreset>& factoryPresets()
{
    static const std::vector<FactoryPreset> presets =
    {
        //======================================================================== INIT
        { "Init", "INIT", {
            { "voiceGain", -2.2f } } },

        { "Master Bus Doux", "INIT", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "unison", 3.0f },
            { "detune", 9.0f }, { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "attack", 0.01f }, { "decay", 0.4f },
            { "sustain", 0.8f }, { "release", 0.3f }, { "fx1Type", 17.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f },
            { "fx1A", 0.5f }, { "fx1B", 0.2f }, { "fx1C", 0.4f }, { "fx1D", 1.0f }, { "fx2Type", 19.0f },
            { "fx2On", 1.0f }, { "fx2Mix", 0.3f }, { "fx2A", 0.4f }, { "fx2B", 0.4f }, { "fx2C", 0.3f },
            { "fx2D", 0.2f }, { "fx3Type", 20.0f }, { "fx3On", 1.0f }, { "fx3Mix", 1.0f }, { "fx3A", 0.55f },
            { "fx3B", 0.3f }, { "fx3C", 0.5f }, { "fx3D", 0.5f }, { "fx4Type", 15.0f }, { "fx4On", 1.0f },
            { "fx4Mix", 1.0f }, { "fx4A", 0.9f }, { "fx4B", 0.1f }, { "fx4C", 0.4f }, { "fx4D", 0.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        //======================================================================== BASSES
        { "Sub Pur", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.0f }, { "subLevel", 0.55f }, { "filterType", 1.0f },
            { "cutoff", 700.0f }, { "resonance", 0.7f }, { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -5.6f } } },

        { "Reese Noir", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.5f },
            { "oscBLevel", 0.9f }, { "oscBPos", 0.5f }, { "oscBTune", 14.0f }, { "unison", 3.0f }, { "detune", 12.0f },
            { "spread", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 420.0f },
            { "resonance", 1.2f }, { "drive", 0.2f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f },
            { "release", 0.2f }, { "phaserMix", 0.25f }, { "phaserRate", 0.2f }, { "phaserFeedback", 0.4f },
            { "lfo3Rate", 0.15f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f }, { "voiceGain", -3.4f } } },

        { "Acid Squelch", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.06f }, { "position", 0.6f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 6.0f }, { "drive", 0.3f }, { "fltEnvAmount", 0.7f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.18f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.3f },
            { "sustain", 0.6f }, { "release", 0.08f }, { "distType", 4.0f }, { "distAmt", 0.2f },
            { "velFilter", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.6f },
            { "voiceGain", -1.2f } } },

        { "Growl FM", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "fmAmount", 0.45f }, { "subLevel", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 1.0f }, { "distType", 2.0f },
            { "distAmt", 0.35f }, { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.15f }, { "mx1Src", 1.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.35f },
            { "mx2Src", 9.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f }, { "voiceGain", -5.8f } } },

        { "Wobble 1/8", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.5f },
            { "unison", 3.0f }, { "detune", 14.0f }, { "spread", 0.4f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 380.0f }, { "resonance", 2.5f }, { "distType", 0.0f }, { "distAmt", 0.3f },
            { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.12f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.55f }, { "mx2Src", 1.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -6.3f } } },

        { "Gate Bass 1/16", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.02f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f },
            { "cutoff", 900.0f }, { "resonance", 1.4f }, { "distType", 1.0f }, { "distAmt", 0.25f },
            { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.04f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f },
            { "voiceGain", -5.7f } } },

        { "Ring Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.05f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 7.0f },
            { "ringAmount", 0.6f }, { "subLevel", 0.4f }, { "filterType", 0.0f }, { "cutoff", 1800.0f },
            { "resonance", 0.9f }, { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -3.6f } } },

        { "Comb Laser Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "position", 0.5f }, { "subLevel", 0.5f }, { "filterType", 5.0f },
            { "cutoff", 220.0f }, { "resonance", 5.0f }, { "fltEnvAmount", 0.6f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.15f }, { "attack", 0.002f }, { "decay", 0.35f },
            { "sustain", 0.7f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", -2.4f } } },

        { "Machine Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "tableA", 6.0f }, { "position", 0.3f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.4f },
            { "subLevel", 0.5f }, { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 2.0f },
            { "distType", 3.0f }, { "distAmt", 0.3f }, { "distMix", 0.5f }, { "attack", 0.002f }, { "decay", 0.25f },
            { "sustain", 0.9f }, { "release", 0.1f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 14.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", -0.3f } } },

        { "Fold Sub", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "distType", 2.0f }, { "distAmt", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 18.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -6.5f } } },

        { "Laser Dive", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "pitchEnvAmt", 24.0f },
            { "pitchEnvDecay", 0.12f }, { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.5f },
            { "attack", 0.001f }, { "decay", 0.4f }, { "sustain", 0.5f }, { "release", 0.15f }, { "mx1Src", 9.0f },
            { "mx1Dst", 20.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -1.1f } } },

        { "Pump Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.55f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 800.0f }, { "resonance", 1.2f }, { "lfoShape", 2.0f }, { "lfoSync", 5.0f },
            { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 1.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "voiceGain", -2.5f } } },

        { "Dirty Mono", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 2.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.55f }, { "detune", 8.0f }, { "noiseLevel", 0.15f },
            { "noiseType", 2.0f }, { "noiseTone", 0.3f }, { "noiseDecay", 5.0f }, { "distType", 1.0f },
            { "distAmt", 0.4f }, { "filterType", 1.0f }, { "cutoff", 600.0f }, { "resonance", 1.6f },
            { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -4.3f } } },

        { "Phase Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f },
            { "cutoff", 700.0f }, { "resonance", 1.0f }, { "phaserMix", 0.6f }, { "phaserRate", 0.3f },
            { "phaserDepth", 0.8f }, { "phaserFeedback", 0.6f }, { "phaserCentre", 900.0f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f }, { "voiceGain", -2.1f } } },

        { "Basse Electrique", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 6.8f }, { "mono", 1.0f }, { "glide", 0.0f }, { "strDecay", 1.8f },
            { "strDamp", 0.55f }, { "strPos", 0.12f }, { "strBright", 0.5f }, { "cutoff", 3000.0f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 35.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f } } },

        { "Basse Slap", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 5.7f }, { "mono", 1.0f }, { "glide", 0.0f }, { "strDecay", 0.6f },
            { "strDamp", 0.8f }, { "strPos", 0.06f }, { "strBright", 1.0f }, { "cutoff", 6000.0f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 60.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f } } },

        { "Contrebasse Pizz", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 4.9f }, { "mono", 1.0f }, { "glide", 0.0f }, { "strDecay", 1.2f },
            { "strDamp", 0.35f }, { "strPos", 0.3f }, { "strBright", 0.3f }, { "cutoff", 2000.0f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 35.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f } } },

        { "Basse Qui Parle", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.429f },
            { "subLevel", 0.35f }, { "unison", 2.0f }, { "detune", 8.0f }, { "filterType", 1.0f },
            { "cutoff", 1400.0f }, { "resonance", 0.9f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.12f }, { "f2Type", 5.0f }, { "f2Cutoff", 900.0f }, { "f2Res", 0.9f }, { "f2Mix", 0.85f },
            { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "dwRate", 1.0f }, { "dwSync", 6.0f }, { "dwSmooth", 0.5f },
            { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", -0.2f }, { "draw3", 0.6f }, { "draw4", 1.0f },
            { "draw5", 0.3f }, { "draw6", -0.6f }, { "draw7", -1.0f }, { "draw8", 0.0f }, { "mx1Src", 16.0f },
            { "mx1Dst", 22.0f }, { "mx1Amt", 0.85f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f },
            { "mx3Src", 10.0f }, { "mx3Dst", 21.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.5f } } },

        { "Fold Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 1800.0f }, { "resonance", 0.8f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.12f }, { "fx1Type", 13.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.45f },
            { "fx1B", 0.25f }, { "fx1C", 0.5f }, { "fx1D", 0.45f }, { "fx2Type", 17.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 1.0f }, { "fx2A", 0.5f }, { "fx2B", 0.2f }, { "fx2C", 0.3f }, { "fx2D", 1.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.4f } } },

        { "Asym Grit Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 2.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.429f }, { "subLevel", 0.45f }, { "detune", 8.0f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.5f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f }, { "fx1Type", 13.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 1.0f }, { "fx1A", 0.35f }, { "fx1B", 0.75f }, { "fx1C", 0.55f }, { "fx1D", 0.45f },
            { "fx2Type", 16.0f }, { "fx2On", 1.0f }, { "fx2Mix", 1.0f }, { "fx2A", 0.5f }, { "fx2B", 0.4f },
            { "fx2C", 0.3f }, { "fx2D", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Maximizer Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 3.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.286f }, { "subLevel", 0.5f }, { "detune", 10.0f },
            { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.2f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f }, { "fx1Type", 18.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 1.0f }, { "fx1A", 0.3f }, { "fx1B", 0.9f }, { "fx1C", 0.4f }, { "fx1D", 0.3f },
            { "fx2Type", 20.0f }, { "fx2On", 1.0f }, { "fx2Mix", 1.0f }, { "fx2A", 0.5f }, { "fx2B", 0.35f },
            { "fx2C", 0.5f }, { "fx2D", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Mono Sub Wide Top", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 5.0f }, { "spread", 1.0f },
            { "glide", 0.04f }, { "position", 0.429f }, { "oscBPos", 0.5f }, { "oscBLevel", 0.8f },
            { "oscBTune", 11.0f }, { "detune", 14.0f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 1200.0f }, { "resonance", 1.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f },
            { "release", 0.2f }, { "fx1Type", 20.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.9f },
            { "fx1B", 0.5f }, { "fx1C", 0.5f }, { "fx1D", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f } } },

        { "Wah Bass Auto", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 2.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.429f }, { "subLevel", 0.35f }, { "detune", 8.0f },
            { "filterType", 1.0f }, { "cutoff", 2500.0f }, { "resonance", 0.8f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f }, { "fx1Type", 14.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 1.0f }, { "fx1A", 0.55f }, { "fx1B", 0.35f }, { "fx1C", 0.6f }, { "fx1D", 0.55f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f } } },

        { "Formant Growl", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 3.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.429f }, { "subLevel", 0.4f }, { "detune", 10.0f },
            { "filterType", 1.0f }, { "cutoff", 1600.0f }, { "resonance", 1.0f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f },
            { "f2Res", 1.0f }, { "f2Mix", 0.9f }, { "f2Vowel", 0.0f }, { "f2Mode", 1.0f }, { "dwRate", 1.0f },
            { "dwSync", 6.0f }, { "dwSmooth", 0.6f }, { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", 1.0f },
            { "draw3", -1.0f }, { "draw4", 0.5f }, { "draw5", -0.3f }, { "draw6", 1.0f }, { "draw7", -0.6f },
            { "draw8", 0.2f }, { "fx1Type", 13.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.4f },
            { "fx1B", 0.0f }, { "fx1C", 0.5f }, { "fx1D", 0.5f }, { "mx1Src", 16.0f }, { "mx1Dst", 22.0f },
            { "mx1Amt", 0.8f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Bande Sub", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.05f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 900.0f }, { "attack", 0.005f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.2f },
            { "fx1Type", 9.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.8f }, { "fx1A", 0.35f }, { "fx1B", 0.3f },
            { "fx1C", 0.5f }, { "fx1D", 0.5f }, { "fx2Type", 17.0f }, { "fx2On", 1.0f }, { "fx2Mix", 1.0f },
            { "fx2A", 0.45f }, { "fx2B", 0.2f }, { "fx2C", 0.4f }, { "fx2D", 1.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Rotary Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 2.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.143f }, { "subLevel", 0.4f }, { "detune", 8.0f },
            { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 0.8f }, { "attack", 0.004f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f }, { "fx1Type", 11.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.7f }, { "fx1A", 0.45f }, { "fx1B", 0.6f }, { "fx1C", 0.4f }, { "fx1D", 0.4f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f } } },

        //======================================================================== ROBOTIQUE
        { "Voice Box", "ROBOTIQUE", {
            { "lfoDepth", 0.6f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.02f }, { "tableA", 1.0f },
            { "position", 0.3f }, { "fmAmount", 0.2f }, { "oscBPos", 0.2f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "lfoShape", 1.0f }, { "lfoRate", 1.2f },
            { "chorusMix", 0.3f }, { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f },
            { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 6.5f } } },

        { "Modem", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "oscBPos", 0.3f },
            { "fmAmount", 0.6f }, { "oscBOct", 1.0f }, { "oscBTune", 7.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "lfo3Shape", 3.0f }, { "lfo3Rate", 8.0f },
            { "attack", 0.001f }, { "decay", 0.12f }, { "sustain", 0.0f }, { "release", 0.1f }, { "delayMode", 1.0f },
            { "delaySync", 4.0f }, { "delayMix", 0.3f }, { "delayFeedback", 0.45f }, { "delayLP", 5000.0f },
            { "mx1Src", 7.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 1.0f }, { "mx2Src", 14.0f }, { "mx2Dst", 16.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", 6.0f } } },

        { "Sync Droid", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "unison", 2.0f },
            { "detune", 8.0f }, { "warpModeA", 3.0f }, { "warpAmtA", 0.5f }, { "modDecay", 0.3f },
            { "filterType", 1.0f }, { "cutoff", 3500.0f }, { "resonance", 1.4f }, { "distType", 1.0f },
            { "distAmt", 0.2f }, { "attack", 0.002f }, { "decay", 0.25f }, { "sustain", 0.8f }, { "release", 0.15f },
            { "mx1Src", 2.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 14.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -5.8f } } },

        { "Arp Droid", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "warpModeA", 9.0f },
            { "warpAmtA", 0.3f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 4500.0f },
            { "attack", 0.001f }, { "decay", 0.18f }, { "sustain", 0.0f }, { "release", 0.12f }, { "delayMode", 1.0f },
            { "delaySync", 3.0f }, { "delayMix", 0.35f }, { "delayFeedback", 0.5f }, { "delayLP", 5000.0f },
            { "chorusMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.4f },
            { "voiceGain", 1.9f } } },

        { "Alien Code", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.4f }, { "oscBPos", 0.5f },
            { "fmAmount", 0.3f }, { "oscBOct", 1.0f }, { "unison", 1.0f }, { "filterType", 3.0f },
            { "cutoff", 2500.0f }, { "resonance", 3.0f }, { "attack", 0.001f }, { "decay", 0.15f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "delayMix", 0.25f }, { "delaySync", 2.0f },
            { "delayFeedback", 0.4f }, { "mx1Src", 7.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.6f }, { "mx2Src", 7.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 7.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 9.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 10.0f }, { "mx5Dst", 3.0f },
            { "mx5Amt", 0.4f }, { "voiceGain", 3.1f } } },

        { "Servo Glide", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.3f }, { "tableA", 2.0f },
            { "position", 0.4f }, { "warpModeA", 8.0f }, { "warpAmtA", 0.5f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 2000.0f }, { "resonance", 2.0f }, { "lfoRate", 5.0f },
            { "attack", 0.01f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.2f }, { "mx1Src", 1.0f },
            { "mx1Dst", 14.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 5.0f },
            { "mx4Amt", 0.1f }, { "voiceGain", 2.5f } } },

        { "Circuit Bend", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.5f },
            { "warpModeA", 7.0f }, { "warpAmtA", 0.6f }, { "distType", 3.0f }, { "distAmt", 0.5f }, { "unison", 2.0f },
            { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 1.8f },
            { "lfoRate", 2.0f }, { "attack", 0.002f }, { "decay", 0.3f }, { "sustain", 0.5f }, { "release", 0.2f },
            { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f },
            { "mx4Dst", 14.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -4.1f } } },

        { "Talk Machine", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.8f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 1.0f },
            { "position", 0.1f }, { "modDecay", 0.35f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "delayMix", 0.2f }, { "delaySync", 2.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.4f }, { "voiceGain", -1.8f } } },

        { "Sequence Toy", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 1.0f },
            { "filterType", 5.0f }, { "cutoff", 600.0f }, { "resonance", 4.0f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.15f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.2f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "delayMix", 0.25f }, { "delaySync", 4.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "voiceGain", 0.1f } } },

        { "Ring Talker", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 1.0f },
            { "position", 0.5f }, { "ringAmount", 0.5f }, { "oscBTune", 12.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "lfoRate", 3.0f }, { "attack", 0.005f },
            { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.2f }, { "mx1Src", 1.0f }, { "mx1Dst", 17.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f },
            { "voiceGain", -1.5f } } },

        { "Data Burst", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.6f },
            { "warpModeA", 6.0f }, { "warpAmtA", 0.5f }, { "distType", 3.0f }, { "distAmt", 0.3f }, { "unison", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 800.0f }, { "attack", 0.001f }, { "decay", 0.1f }, { "sustain", 0.0f },
            { "release", 0.08f }, { "delayMode", 1.0f }, { "delaySync", 5.0f }, { "delayMix", 0.3f },
            { "delayFeedback", 0.55f }, { "delayHP", 300.0f }, { "mx1Src", 7.0f }, { "mx1Dst", 1.0f },
            { "mx1Amt", 0.8f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 3.7f } } },

        { "Androide Triste", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.12f }, { "position", 0.0f },
            { "oscBPos", 0.0f }, { "fmAmount", 0.2f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 6000.0f }, { "attack", 0.02f }, { "decay", 0.4f }, { "sustain", 0.8f }, { "release", 0.4f },
            { "lfoShape", 1.0f }, { "lfoRate", 5.0f }, { "f2Type", 5.0f }, { "f2Cutoff", 700.0f }, { "f2Res", 0.9f },
            { "f2Mix", 1.0f }, { "f2Vowel", 0.57f }, { "f2Mode", 0.0f }, { "fx1Type", 2.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.35f }, { "fx1A", 0.3f }, { "fx1B", 0.1f }, { "fx1C", 0.2f }, { "fx1D", 0.3f },
            { "fx2Type", 9.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.5f }, { "fx2A", 0.5f }, { "fx2B", 0.4f },
            { "fx2C", 0.3f }, { "fx2D", 0.6f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.1f },
            { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f }, { "mx3Dst", 17.0f },
            { "mx3Amt", 0.5f } } },

        { "Robot Parle", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "position", 0.286f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 10000.0f }, { "attack", 0.002f }, { "decay", 0.15f },
            { "sustain", 0.7f }, { "release", 0.1f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f },
            { "f2Mix", 1.0f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "dwRate", 1.0f }, { "dwSync", 7.0f },
            { "dwSmooth", 0.0f }, { "dwSteps", 16.0f }, { "draw1", -1.0f }, { "draw2", 0.5f }, { "draw3", 1.0f },
            { "draw4", -0.5f }, { "draw5", 0.0f }, { "draw6", 1.0f }, { "draw7", -1.0f }, { "draw8", 0.3f },
            { "draw9", 0.8f }, { "draw10", -0.2f }, { "draw11", -0.9f }, { "draw12", 0.4f }, { "draw13", 1.0f },
            { "draw14", 0.0f }, { "draw15", -0.7f }, { "draw16", 0.6f }, { "fx1Type", 1.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.4f }, { "fx1A", 0.6f }, { "fx1B", 0.45f }, { "fx1C", 0.3f }, { "fx1D", 0.3f },
            { "mx1Src", 16.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.8f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f },
            { "mx2Amt", 0.8f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        //======================================================================== MALSAIN
        { "Rotten Choir", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.6f }, { "unison", 7.0f },
            { "detune", 60.0f }, { "spread", 1.0f }, { "filterType", 4.0f }, { "cutoff", 900.0f },
            { "resonance", 3.0f }, { "distType", 4.0f }, { "distAmt", 0.3f }, { "attack", 1.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.45f }, { "reverbSize", 0.9f },
            { "lfo3Rate", 0.12f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.6f },
            { "mx4Src", 11.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -3.7f } } },

        { "Sick Detune", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBLevel", 0.8f },
            { "oscBPos", 0.5f }, { "oscBTune", 23.0f }, { "unison", 2.0f }, { "detune", 20.0f },
            { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "attack", 0.8f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 2.0f }, { "lfoRate", 0.4f }, { "chorusMix", 0.4f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.05f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.6f }, { "mx4Src", 11.0f }, { "mx4Dst", 7.0f }, { "mx4Amt", 0.4f },
            { "voiceGain", -2.4f } } },

        { "Bad Signal", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "noiseLevel", 0.35f }, { "noiseType", 2.0f },
            { "noiseDecay", 5.0f }, { "noiseTone", 0.7f }, { "position", 0.5f }, { "filterType", 3.0f },
            { "cutoff", 1500.0f }, { "resonance", 5.0f }, { "distType", 3.0f }, { "distAmt", 0.35f },
            { "attack", 0.4f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 1.5f }, { "lfo2Rate", 3.0f },
            { "lfo2Shape", 3.0f }, { "delayMix", 0.25f }, { "delayFeedback", 0.5f }, { "delayMode", 1.0f },
            { "delaySync", 2.0f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.35f }, { "mx2Src", 7.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "mx5Src", 11.0f }, { "mx5Dst", 19.0f },
            { "mx5Amt", 0.5f }, { "voiceGain", -3.5f } } },

        { "Bleed Pad", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.45f }, { "unison", 5.0f },
            { "detune", 25.0f }, { "spread", 0.9f }, { "subLevel", 0.3f }, { "filterType", 1.0f },
            { "cutoff", 500.0f }, { "resonance", 2.0f }, { "distType", 2.0f }, { "distAmt", 0.4f }, { "attack", 2.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.4f }, { "lfo3Rate", 0.1f },
            { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 14.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -2.3f } } },

        { "Hospital", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBTune", -17.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.15f }, { "ringAmount", 0.2f },
            { "unison", 2.0f }, { "detune", 15.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 1.2f },
            { "decay", 1.0f }, { "sustain", 0.9f }, { "release", 2.5f }, { "lfoRate", 0.3f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.85f }, { "mx1Src", 1.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.2f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f }, { "mx3Dst", 17.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 11.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.4f }, { "voiceGain", -2.8f } } },

        { "Corrosion", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.5f }, { "unison", 5.0f },
            { "detune", 30.0f }, { "spread", 0.8f }, { "distType", 2.0f }, { "distAmt", 0.7f }, { "filterType", 1.0f },
            { "cutoff", 1800.0f }, { "resonance", 1.5f }, { "attack", 2.5f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 3.5f }, { "reverbMix", 0.35f }, { "lfo4Rate", 0.08f }, { "mx1Src", 15.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 15.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.25f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f }, { "mx5Src", 11.0f }, { "mx5Dst", 1.0f }, { "mx5Amt", 0.5f }, { "voiceGain", -0.8f } } },

        { "Grind Pad", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 18.0f },
            { "distType", 3.0f }, { "distAmt", 0.5f }, { "distMix", 0.8f }, { "filterType", 0.0f },
            { "cutoff", 2500.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "reverbMix", 0.3f }, { "chorusMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", 2.1f } } },

        { "Insect Swarm", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "unison", 7.0f },
            { "detune", 90.0f }, { "spread", 1.0f }, { "filterType", 2.0f }, { "cutoff", 1800.0f },
            { "resonance", 2.0f }, { "lfoRate", 7.0f }, { "lfoShape", 1.0f }, { "attack", 0.8f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.5f }, { "reverbMix", 0.3f }, { "mx1Src", 1.0f }, { "mx1Dst", 1.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f },
            { "voiceGain", -0.8f } } },

        { "Sub Rot", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.1f }, { "position", 0.0f },
            { "subLevel", 0.5f }, { "oscBPos", 0.0f }, { "oscBTune", -9.0f }, { "oscBLevel", 0.5f },
            { "ringAmount", 0.25f }, { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 500.0f },
            { "resonance", 1.4f }, { "distType", 4.0f }, { "distAmt", 0.4f }, { "attack", 0.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.2f }, { "lfoRate", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -6.2f } } },

        { "Vein Pulse", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "unison", 5.0f }, { "detune", 22.0f },
            { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.8f }, { "lfoShape", 1.0f },
            { "lfoRate", 0.6f }, { "attack", 1.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "reverbMix", 0.4f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 1.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.25f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "voiceGain", -3.7f } } },

        { "Frequence Fausse", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.6f }, { "unison", 3.0f }, { "detune", 12.0f }, { "filterType", 0.0f },
            { "cutoff", 5000.0f }, { "attack", 0.1f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 0.8f },
            { "fx1Type", 3.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.7f }, { "fx1A", 0.62f }, { "fx1B", 0.55f },
            { "fx1C", 0.0f }, { "fx1D", 0.0f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f } } },

        { "Decaleur Descendant", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.7f }, { "unison", 5.0f }, { "detune", 15.0f }, { "spread", 0.8f }, { "filterType", 1.0f },
            { "cutoff", 3000.0f }, { "attack", 0.3f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.0f },
            { "fx1Type", 3.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.8f }, { "fx1A", 0.4f }, { "fx1B", 0.5f },
            { "fx1C", 1.0f }, { "fx1D", 0.0f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f } } },

        { "Cloche Inharmonique", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.143f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.5f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 1.6f }, { "sustain", 0.0f }, { "release", 1.5f },
            { "fx1Type", 3.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.8f }, { "fx1A", 0.62f }, { "fx1B", 0.5f },
            { "fx1C", 0.4f }, { "fx1D", 0.0f }, { "fx2Type", 4.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.5f },
            { "fx2A", 0.4f }, { "fx2B", 0.7f }, { "fx2C", 0.5f }, { "fx2D", 0.0f }, { "reverbMix", 0.3f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f },
            { "mx2Amt", 0.5f } } },

        { "Ring Lent Malade", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.5f }, { "unison", 3.0f }, { "detune", 10.0f }, { "filterType", 0.0f },
            { "cutoff", 4000.0f }, { "attack", 0.1f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 0.8f },
            { "fx1Type", 2.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.65f }, { "fx1A", 0.35f }, { "fx1B", 0.0f },
            { "fx1C", 0.8f }, { "fx1D", 0.12f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f } } },

        { "Radio Brisee", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.571f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.5f }, { "unison", 2.0f }, { "detune", 9.0f }, { "filterType", 0.0f },
            { "cutoff", 4500.0f }, { "attack", 0.05f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.4f },
            { "fx1Type", 10.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.8f }, { "fx1B", 0.55f },
            { "fx1C", 0.6f }, { "fx1D", 0.5f }, { "fx2Type", 1.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.4f },
            { "fx2A", 0.55f }, { "fx2B", 0.4f }, { "fx2C", 0.3f }, { "fx2D", 0.3f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Digital Rot", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "oscBPos", 0.7f },
            { "oscBLevel", 0.5f }, { "fmAmount", 0.3f }, { "unison", 3.0f }, { "detune", 12.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.02f }, { "decay", 0.5f }, { "sustain", 1.0f },
            { "release", 0.3f }, { "fx1Type", 1.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.6f }, { "fx1A", 0.7f },
            { "fx1B", 0.55f }, { "fx1C", 0.4f }, { "fx1D", 0.4f }, { "fx2Type", 3.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 0.4f }, { "fx2A", 0.55f }, { "fx2B", 0.5f }, { "fx2C", 0.2f }, { "fx2D", 0.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f } } },

        { "Chambre Tordue", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "oscBPos", 0.8f },
            { "oscBLevel", 0.5f }, { "unison", 5.0f }, { "detune", 17.0f }, { "spread", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 4500.0f }, { "attack", 0.5f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.5f },
            { "fx1Type", 3.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.5f }, { "fx1A", 0.58f }, { "fx1B", 0.5f },
            { "fx1C", 0.8f }, { "fx1D", 0.0f }, { "fx2Type", 8.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.4f },
            { "fx2A", 0.75f }, { "fx2B", 0.6f }, { "fx2C", 0.5f }, { "fx2D", 0.5f }, { "reverbMix", 0.45f },
            { "reverbSize", 0.95f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f } } },

        { "Auto Wah Sale", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.5f }, { "unison", 3.0f }, { "detune", 12.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.15f },
            { "fx1Type", 14.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.7f }, { "fx1B", 0.3f },
            { "fx1C", 0.8f }, { "fx1D", 0.8f }, { "fx2Type", 13.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.5f },
            { "fx2A", 0.35f }, { "fx2B", 0.6f }, { "fx2C", 0.5f }, { "fx2D", 0.45f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Notch Vivant", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "unison", 5.0f },
            { "detune", 14.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 12000.0f },
            { "attack", 0.4f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.8f }, { "f2Type", 4.0f },
            { "f2Cutoff", 800.0f }, { "f2Res", 3.0f }, { "f2Mix", 1.0f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f },
            { "dwRate", 0.15f }, { "dwSync", 0.0f }, { "dwSmooth", 0.9f }, { "dwSteps", 12.0f }, { "draw1", -1.0f },
            { "draw2", -0.7f }, { "draw3", -0.3f }, { "draw4", 0.1f }, { "draw5", 0.5f }, { "draw6", 0.8f },
            { "draw7", 1.0f }, { "draw8", 0.6f }, { "draw9", 0.2f }, { "draw10", -0.2f }, { "draw11", -0.6f },
            { "draw12", -0.9f }, { "reverbMix", 0.3f }, { "mx1Src", 16.0f }, { "mx1Dst", 21.0f }, { "mx1Amt", 0.9f },
            { "mx2Src", 9.0f }, { "mx2Dst", 21.0f }, { "mx2Amt", 0.7f }, { "mx3Src", 10.0f }, { "mx3Dst", 6.0f },
            { "mx3Amt", 0.5f } } },

        //======================================================================== NAPPES
        { "Black Glass", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.3f }, { "unison", 5.0f },
            { "detune", 18.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "attack", 1.2f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f }, { "chorusMix", 0.3f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "reverbHP", 200.0f }, { "lfo3Rate", 0.1f }, { "mx1Src", 14.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.2f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f },
            { "mx4Amt", 0.5f }, { "voiceGain", -1.6f } } },

        { "Cold Machine", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 6.0f }, { "detune", 14.0f },
            { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 1400.0f }, { "resonance", 1.2f },
            { "attack", 1.8f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "chorusMix", 0.3f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "lfo3Rate", 0.08f }, { "mx1Src", 14.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 1.5f } } },

        { "Iron Pad", "NAPPES", {
            { "lfoDepth", 0.3f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.35f }, { "unison", 5.0f },
            { "detune", 20.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.5f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.45f },
            { "reverbSize", 0.85f }, { "lfoRate", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", -1.2f } } },

        { "Dust Choir", "NAPPES", {
            { "lfoDepth", 0.4f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.5f }, { "unison", 6.0f },
            { "detune", 24.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 2.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.5f }, { "chorusMix", 0.4f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.9f }, { "lfoRate", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", 1.7f } } },

        { "Void Drift", "NAPPES", {
            { "lfoDepth", 0.5f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.4f },
            { "unison", 7.0f }, { "detune", 35.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 600.0f },
            { "resonance", 1.8f }, { "lfoRate", 0.06f }, { "attack", 4.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 4.5f }, { "reverbMix", 0.6f }, { "reverbSize", 0.95f }, { "lfo3Rate", 0.05f },
            { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -1.5f } } },

        { "Glacier", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 12.0f },
            { "filterType", 5.0f }, { "cutoff", 300.0f }, { "resonance", 3.0f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.5f }, { "reverbMix", 0.55f }, { "reverbSize", 0.95f },
            { "reverbHP", 300.0f }, { "lfo3Rate", 0.07f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", 4.3f } } },

        { "Industrial Air", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.5f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.6f }, { "subLevel", 0.0f },
            { "unison", 1.0f }, { "filterType", 3.0f }, { "cutoff", 1400.0f }, { "resonance", 3.5f },
            { "lfo2Rate", 0.1f }, { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f }, { "voiceGain", 11.4f } } },

        { "Dark Ether", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.4f },
            { "oscBLevel", 0.25f }, { "oscBOct", 2.0f }, { "oscBPos", 0.3f }, { "unison", 5.0f }, { "detune", 15.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 4.0f }, { "chorusMix", 0.3f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.92f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f },
            { "voiceGain", -1.2f } } },

        { "Phase Cloud", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.45f }, { "unison", 5.0f },
            { "detune", 16.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.5f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "phaserMix", 0.6f },
            { "phaserRate", 0.25f }, { "phaserFeedback", 0.55f }, { "reverbMix", 0.4f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", -0.6f } } },

        { "Slow Motion", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.6f }, { "tableA", 5.0f }, { "position", 0.2f }, { "unison", 4.0f },
            { "detune", 14.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "attack", 4.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "modAttack", 3.0f }, { "modDecay", 4.0f },
            { "modSustain", 0.4f }, { "reverbMix", 0.45f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", 3.2f } } },

        { "Night Shift", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "subLevel", 0.3f },
            { "unison", 5.0f }, { "detune", 18.0f }, { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 2.0f }, { "lfo4Rate", 0.1f }, { "attack", 1.8f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 3.0f }, { "reverbMix", 0.4f }, { "mx1Src", 15.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f }, { "voiceGain", -1.5f } } },

        { "Warm Machine", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 5.0f }, { "detune", 22.0f },
            { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 1800.0f }, { "drive", 0.15f }, { "attack", 1.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.0f }, { "chorusMix", 0.4f }, { "reverbMix", 0.3f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "voiceGain", 3.3f } } },

        { "Mouth Pad", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.7f }, { "oscBTune", -8.0f }, { "unison", 5.0f }, { "detune", 15.0f }, { "spread", 0.8f },
            { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.2f }, { "f2Type", 5.0f }, { "f2Cutoff", 1100.0f }, { "f2Res", 0.6f }, { "f2Mix", 0.7f },
            { "f2Vowel", 0.3f }, { "f2Mode", 1.0f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "lfo2Rate", 0.12f }, { "lfo2Shape", 1.0f }, { "mx1Src", 8.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.6f },
            { "mx2Src", 14.0f }, { "mx2Dst", 21.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 22.0f },
            { "mx3Amt", 0.8f }, { "mx4Src", 10.0f }, { "mx4Dst", 21.0f }, { "mx4Amt", 0.4f } } },

        { "Tremolo Pad", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.7f }, { "oscBTune", -7.0f }, { "unison", 5.0f }, { "detune", 14.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 3500.0f }, { "attack", 0.5f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.0f }, { "fx1Type", 12.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.45f },
            { "fx1B", 0.7f }, { "fx1C", 0.0f }, { "fx1D", 1.0f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.4f } } },

        { "Shimmer Pad", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "oscBPos", 0.3f },
            { "oscBLevel", 0.6f }, { "oscBTune", -7.0f }, { "unison", 5.0f }, { "detune", 14.0f }, { "spread", 0.8f },
            { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 0.8f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.5f }, { "fx1Type", 8.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.5f }, { "fx1A", 0.75f },
            { "fx1B", 0.55f }, { "fx1C", 0.5f }, { "fx1D", 0.6f }, { "reverbMix", 0.3f }, { "reverbSize", 0.9f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.4f } } },

        { "Shimmer Quinte", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.143f },
            { "oscBLevel", 0.5f }, { "oscBOct", 1.0f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.8f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.5f }, { "fx1Type", 8.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.45f }, { "fx1A", 0.79f },
            { "fx1B", 0.6f }, { "fx1C", 0.5f }, { "fx1D", 0.7f }, { "reverbMix", 0.35f }, { "reverbSize", 0.9f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.4f } } },

        { "Shimmer Octave Bas", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.6f }, { "unison", 5.0f }, { "detune", 16.0f }, { "spread", 0.9f }, { "filterType", 0.0f },
            { "cutoff", 6000.0f }, { "attack", 0.8f }, { "decay", 0.5f }, { "sustain", 0.9f }, { "release", 1.8f },
            { "fx1Type", 8.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.45f }, { "fx1A", 0.25f }, { "fx1B", 0.55f },
            { "fx1C", 0.5f }, { "fx1D", 0.4f }, { "reverbMix", 0.3f }, { "reverbSize", 0.9f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.4f } } },

        { "Cassette Pad", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "oscBTune", -9.0f }, { "unison", 5.0f }, { "detune", 13.0f }, { "spread", 0.8f },
            { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "attack", 0.7f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.2f }, { "fx1Type", 9.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.55f },
            { "fx1B", 0.4f }, { "fx1C", 0.45f }, { "fx1D", 0.55f }, { "fx2Type", 10.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 0.5f }, { "fx2A", 0.5f }, { "fx2B", 0.35f }, { "fx2C", 0.3f }, { "fx2D", 0.3f },
            { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.4f } } },

        { "Vieille Bande Cordes", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "oscBPos", 0.429f },
            { "oscBLevel", 0.8f }, { "oscBTune", -10.0f }, { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 3500.0f }, { "attack", 0.4f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 0.8f }, { "fx1Type", 9.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.6f },
            { "fx1B", 0.45f }, { "fx1C", 0.5f }, { "fx1D", 0.5f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Rotary Pad Lent", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.4f },
            { "oscBLevel", 0.6f }, { "oscBOct", 1.0f }, { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.7f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 1.0f }, { "fx1Type", 11.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.1f },
            { "fx1B", 0.8f }, { "fx1C", 0.6f }, { "fx1D", 0.2f }, { "reverbMix", 0.35f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        //======================================================================== TEXTURES
        { "Wind Tunnel", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.7f },
            { "noiseType", 0.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.8f }, { "filterType", 3.0f },
            { "cutoff", 900.0f }, { "resonance", 3.0f }, { "lfo2Rate", 0.15f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.4f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.55f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f },
            { "voiceGain", 8.9f } } },

        { "Steel Rain", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 11.0f }, { "ringAmount", 0.7f }, { "unison", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 900.0f }, { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f },
            { "release", 0.4f }, { "delayMode", 1.0f }, { "delaySync", 5.0f }, { "delayMix", 0.4f },
            { "delayFeedback", 0.6f }, { "delayHP", 400.0f }, { "delayLP", 6000.0f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "mx1Src", 7.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 7.0f },
            { "mx2Dst", 17.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 17.0f }, { "mx4Amt", 0.5f }, { "voiceGain", 12.0f } } },

        { "Rusted Drone", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.5f },
            { "unison", 7.0f }, { "detune", 45.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 380.0f },
            { "resonance", 2.5f }, { "lfo3Rate", 0.05f }, { "distType", 0.0f }, { "distAmt", 0.3f },
            { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "reverbMix", 0.6f },
            { "reverbSize", 0.95f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f },
            { "voiceGain", -2.2f } } },

        { "Radio Ghost", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.2f }, { "noiseLevel", 0.35f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.5f }, { "filterType", 5.0f },
            { "cutoff", 420.0f }, { "resonance", 4.0f }, { "lfo2Rate", 0.2f }, { "attack", 1.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.5f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.45f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f },
            { "voiceGain", 4.8f } } },

        { "Metal Breath", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 6.0f }, { "position", 0.5f }, { "oscBPos", 0.7f },
            { "oscBTune", 19.0f }, { "oscBLevel", 0.5f }, { "unison", 3.0f }, { "detune", 30.0f },
            { "filterType", 3.0f }, { "cutoff", 1800.0f }, { "resonance", 4.0f }, { "lfoRate", 0.3f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 1.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f },
            { "mx4Dst", 17.0f }, { "mx4Amt", 0.4f }, { "voiceGain", -1.7f } } },

        { "Static Field", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.6f },
            { "noiseType", 2.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.8f }, { "filterType", 2.0f },
            { "cutoff", 1500.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "delayMix", 0.35f }, { "delayTime", 410.0f }, { "delayFeedback", 0.55f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.4f }, { "voiceGain", 8.6f } } },

        { "Pulse Machine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 3.0f },
            { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 2.0f },
            { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "distAmt", 0.3f }, { "attack", 0.01f }, { "decay", 0.3f },
            { "sustain", 1.0f }, { "release", 0.2f }, { "delayMix", 0.25f }, { "reverbMix", 0.2f }, { "mx1Src", 1.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.9f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -6.0f } } },

        { "Chaos Garden", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "oscBPos", 0.4f },
            { "fmAmount", 0.25f }, { "oscBOct", 1.0f }, { "unison", 5.0f }, { "detune", 40.0f }, { "spread", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "lfo3Rate", 0.2f }, { "lfo4Rate", 0.13f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "delayMode", 1.0f }, { "mx1Src", 14.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 15.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.3f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 16.0f },
            { "mx4Amt", 0.5f }, { "voiceGain", 5.8f } } },

        { "Deep Sea", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "subLevel", 0.6f },
            { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 1.0f }, { "cutoff", 300.0f },
            { "resonance", 2.5f }, { "lfo3Rate", 0.07f }, { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 4.0f }, { "reverbMix", 0.65f }, { "reverbSize", 0.95f }, { "reverbLP", 3000.0f },
            { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f }, { "voiceGain", -2.4f } } },

        { "Granular Dust", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.6f },
            { "warpModeA", 7.0f }, { "warpAmtA", 0.5f }, { "unison", 6.0f }, { "detune", 50.0f }, { "spread", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 1200.0f }, { "lfoShape", 3.0f }, { "lfoRate", 9.0f },
            { "distType", 3.0f }, { "distAmt", 0.25f }, { "attack", 1.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 2.0f }, { "reverbMix", 0.45f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 7.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 14.0f }, { "mx4Amt", 0.5f }, { "voiceGain", 0.3f } } },

        { "Souffle Voyelle", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.2f }, { "position", 0.429f },
            { "noiseLevel", 0.6f }, { "noiseType", 1.0f }, { "noiseTone", 0.5f }, { "filterType", 0.0f },
            { "cutoff", 12000.0f }, { "attack", 0.5f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.9f },
            { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.9f }, { "f2Mix", 1.0f }, { "f2Vowel", 0.25f },
            { "f2Mode", 0.0f }, { "reverbMix", 0.4f }, { "reverbSize", 0.8f }, { "mx1Src", 8.0f }, { "mx1Dst", 22.0f },
            { "mx1Amt", 0.6f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.4f } } },

        { "Auto Pan Texture", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "oscBPos", 0.2f },
            { "oscBLevel", 0.6f }, { "unison", 7.0f }, { "detune", 18.0f }, { "spread", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 5000.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.2f },
            { "fx1Type", 12.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.3f }, { "fx1B", 0.9f },
            { "fx1C", 0.0f }, { "fx1D", 0.0f }, { "reverbMix", 0.35f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f } } },

        { "Tuyau Metallique", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.7f },
            { "noiseType", 0.0f }, { "noiseTone", 0.7f }, { "filterType", 2.0f }, { "cutoff", 300.0f },
            { "attack", 0.003f }, { "decay", 1.2f }, { "sustain", 0.0f }, { "release", 1.0f }, { "fx1Type", 4.0f },
            { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.4f }, { "fx1B", 0.9f }, { "fx1C", 0.4f },
            { "fx1D", 1.0f }, { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f } } },

        { "Disque Use", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "unison", 5.0f }, { "detune", 13.0f }, { "spread", 0.7f }, { "filterType", 0.0f },
            { "cutoff", 4500.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.0f },
            { "fx1Type", 10.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.6f }, { "fx1B", 0.85f },
            { "fx1C", 0.3f }, { "fx1D", 0.45f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.4f } } },

        { "Balayage Dessine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "unison", 5.0f }, { "detune", 15.0f }, { "spread", 0.8f }, { "filterType", 1.0f },
            { "cutoff", 1200.0f }, { "resonance", 2.0f }, { "attack", 0.4f }, { "decay", 0.5f }, { "sustain", 1.0f },
            { "release", 1.0f }, { "dwRate", 0.12f }, { "dwSync", 0.0f }, { "dwSmooth", 0.9f }, { "dwSteps", 12.0f },
            { "draw1", -1.0f }, { "draw2", -0.8f }, { "draw3", -0.5f }, { "draw4", -0.1f }, { "draw5", 0.4f },
            { "draw6", 0.9f }, { "draw7", 1.0f }, { "draw8", 0.5f }, { "draw9", 0.0f }, { "draw10", -0.3f },
            { "draw11", -0.7f }, { "draw12", -0.9f }, { "reverbMix", 0.3f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.75f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f } } },

        { "Bruit Resonant", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.8f },
            { "noiseType", 1.0f }, { "noiseTone", 0.5f }, { "filterType", 3.0f }, { "cutoff", 1500.0f },
            { "resonance", 6.0f }, { "attack", 0.5f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.0f },
            { "f2Type", 3.0f }, { "f2Cutoff", 3000.0f }, { "f2Res", 4.0f }, { "f2Mix", 0.6f }, { "f2Vowel", 0.0f },
            { "f2Mode", 1.0f }, { "dwRate", 0.3f }, { "dwSync", 0.0f }, { "dwSmooth", 0.7f }, { "dwSteps", 8.0f },
            { "draw1", -1.0f }, { "draw2", 0.8f }, { "draw3", -0.4f }, { "draw4", 1.0f }, { "draw5", -0.9f },
            { "draw6", 0.2f }, { "draw7", 0.6f }, { "draw8", -0.7f }, { "reverbMix", 0.35f }, { "mx1Src", 16.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.6f }, { "mx2Src", 16.0f }, { "mx2Dst", 21.0f }, { "mx2Amt", 0.7f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 21.0f },
            { "mx4Amt", 0.5f } } },

        //======================================================================== LEADS
        { "Acid Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.07f }, { "position", 0.6f },
            { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 1200.0f }, { "resonance", 4.5f },
            { "fltEnvAmount", 0.5f }, { "fltDecay", 0.25f }, { "fltSustain", 0.2f }, { "attack", 0.002f },
            { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.15f }, { "delayMix", 0.2f }, { "delaySync", 2.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f },
            { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 1.3f } } },

        { "Saw Stack", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.6f },
            { "unison", 7.0f }, { "detune", 24.0f }, { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 4500.0f },
            { "resonance", 1.0f }, { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f },
            { "delayMix", 0.2f }, { "reverbMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", 2.3f } } },

        { "Vocal Lead", "LEADS", {
            { "lfoDepth", 0.15f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 1.0f },
            { "position", 0.4f }, { "unison", 3.0f }, { "detune", 8.0f }, { "filterType", 0.0f },
            { "cutoff", 6000.0f }, { "lfoRate", 5.5f }, { "attack", 0.03f }, { "decay", 0.3f }, { "sustain", 0.8f },
            { "release", 0.3f }, { "chorusMix", 0.25f }, { "reverbMix", 0.25f }, { "mx1Src", 6.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 0.4f } } },

        { "FM Glass Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.0f },
            { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "fmAmount", 0.3f }, { "unison", 2.0f }, { "detune", 6.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.002f }, { "decay", 0.5f },
            { "sustain", 0.5f }, { "release", 0.4f }, { "reverbMix", 0.3f }, { "modDecay", 0.4f },
            { "modSustain", 0.0f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "voiceGain", -2.5f } } },

        { "Sync Scream", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.5f },
            { "warpModeA", 3.0f }, { "warpAmtA", 0.3f }, { "unison", 2.0f }, { "detune", 10.0f },
            { "filterType", 1.0f }, { "cutoff", 5000.0f }, { "resonance", 1.5f }, { "distType", 1.0f },
            { "distAmt", 0.25f }, { "lfoRate", 5.0f }, { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.2f }, { "mx1Src", 6.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 1.0f },
            { "mx2Dst", 14.0f }, { "mx2Amt", 0.1f }, { "mx3Src", 9.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f },
            { "mx5Amt", 0.5f }, { "voiceGain", -3.4f } } },

        { "Hollow Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 2.0f },
            { "position", 0.5f }, { "warpModeA", 8.0f }, { "warpAmtA", 0.2f }, { "unison", 3.0f }, { "detune", 12.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "lfoRate", 0.4f }, { "attack", 0.01f }, { "decay", 0.3f },
            { "sustain", 0.8f }, { "release", 0.25f }, { "chorusMix", 0.3f }, { "mx1Src", 1.0f }, { "mx1Dst", 14.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 3.6f } } },

        { "Pulse Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 2.0f },
            { "position", 0.3f }, { "unison", 2.0f }, { "detune", 10.0f }, { "filterType", 1.0f },
            { "cutoff", 3500.0f }, { "resonance", 1.2f }, { "attack", 0.003f }, { "decay", 0.25f },
            { "sustain", 0.7f }, { "release", 0.2f }, { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 2.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 3.0f } } },

        { "Ring Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.5f },
            { "oscBOct", 0.0f }, { "oscBTune", 15.0f }, { "ringAmount", 0.35f }, { "unison", 2.0f },
            { "detune", 8.0f }, { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "attack", 0.004f }, { "decay", 0.3f },
            { "sustain", 0.8f }, { "release", 0.25f }, { "delayMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 1.8f } } },

        //======================================================================== STABS
        { "Dub Chord Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 12.0f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.25f }, { "fltSustain", 0.0f }, { "attack", 0.002f }, { "decay", 0.35f },
            { "sustain", 0.0f }, { "release", 0.2f }, { "delayMode", 1.0f }, { "delaySync", 3.0f },
            { "delayMix", 0.4f }, { "delayFeedback", 0.55f }, { "delayLP", 3500.0f }, { "reverbMix", 0.3f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", 0.5f } } },

        { "Techno Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 3.0f }, { "detune", 14.0f },
            { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 2.0f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.12f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.18f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "distType", 0.0f }, { "distAmt", 0.25f },
            { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", 0.0f } } },

        { "Rave Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 7.0f }, { "detune", 28.0f },
            { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 2500.0f }, { "fltEnvAmount", 0.4f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.1f }, { "attack", 0.002f }, { "decay", 0.3f }, { "sustain", 0.0f },
            { "release", 0.2f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f }, { "voiceGain", 2.7f } } },

        { "Metal Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 9.0f }, { "ringAmount", 0.4f }, { "fmAmount", 0.2f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 0.001f }, { "decay", 0.4f },
            { "sustain", 0.0f }, { "release", 0.3f }, { "reverbMix", 0.3f }, { "delayMix", 0.2f },
            { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "voiceGain", -0.6f } } },

        { "FM Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "fmAmount", 0.4f }, { "modDecay", 0.15f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "attack", 0.001f }, { "decay", 0.35f },
            { "sustain", 0.0f }, { "release", 0.25f }, { "reverbMix", 0.2f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f },
            { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "voiceGain", -1.2f } } },

        { "Hollow Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 2.0f }, { "detune", 8.0f },
            { "filterType", 4.0f }, { "cutoff", 1200.0f }, { "resonance", 2.5f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.3f }, { "sustain", 0.0f },
            { "release", 0.2f }, { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 0.8f } } },

        { "Glass Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.5f }, { "unison", 2.0f },
            { "detune", 6.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 0.5f },
            { "sustain", 0.0f }, { "release", 0.4f }, { "chorusMix", 0.25f }, { "reverbMix", 0.35f },
            { "reverbSize", 0.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 2.2f } } },

        { "Bounce Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.5f },
            { "subLevel", 0.3f }, { "pitchEnvAmt", 7.0f }, { "pitchEnvDecay", 0.05f }, { "filterType", 1.0f },
            { "cutoff", 1200.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.15f },
            { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 0.0f }, { "release", 0.1f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 20.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -0.3f } } },

        { "Pad Gate 1/16", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.45f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "oscBTune", 12.0f }, { "unison", 5.0f }, { "detune", 14.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 2800.0f }, { "resonance", 0.9f }, { "attack", 0.01f },
            { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.3f }, { "fx1Type", 6.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 1.0f }, { "fx1A", 0.4f }, { "fx1B", 0.85f }, { "fx1C", 0.2f }, { "fx1D", 0.0f },
            { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Pad Gate Syncope", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 5.0f }, { "detune", 12.0f },
            { "spread", 0.8f }, { "filterType", 1.0f }, { "cutoff", 2500.0f }, { "resonance", 0.8f },
            { "attack", 0.02f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.3f }, { "fx1Type", 6.0f },
            { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.4f }, { "fx1B", 0.9f }, { "fx1C", 0.15f },
            { "fx1D", 0.4f }, { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f } } },

        { "Pulse Drone 1/8", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.571f }, { "subLevel", 0.4f },
            { "unison", 3.0f }, { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 1500.0f },
            { "resonance", 1.2f }, { "attack", 0.01f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.2f },
            { "fx1Type", 6.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.2f }, { "fx1B", 1.0f },
            { "fx1C", 0.3f }, { "fx1D", 0.2857f }, { "delayMix", 0.3f }, { "delaySync", 4.0f },
            { "delayFeedback", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Escalier Dessine", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "unison", 3.0f },
            { "detune", 9.0f }, { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 2.5f },
            { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.1f }, { "dwRate", 1.0f },
            { "dwSync", 7.0f }, { "dwSmooth", 0.0f }, { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", -0.6f },
            { "draw3", -0.2f }, { "draw4", 0.2f }, { "draw5", 0.6f }, { "draw6", 1.0f }, { "draw7", 0.4f },
            { "draw8", -0.4f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.8f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f } } },

        { "Pompe Dessinee", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "oscBTune", 7.0f }, { "unison", 5.0f }, { "detune", 13.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 2200.0f }, { "resonance", 0.8f }, { "attack", 0.005f },
            { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.15f }, { "dwRate", 1.0f }, { "dwSync", 7.0f },
            { "dwSmooth", 0.4f }, { "dwSteps", 16.0f }, { "draw1", 1.0f }, { "draw2", -1.0f }, { "draw3", -1.0f },
            { "draw4", -1.0f }, { "draw5", 0.5f }, { "draw6", -1.0f }, { "draw7", -1.0f }, { "draw8", -1.0f },
            { "draw9", 1.0f }, { "draw10", -1.0f }, { "draw11", -1.0f }, { "draw12", -1.0f }, { "draw13", 0.5f },
            { "draw14", -1.0f }, { "draw15", 0.0f }, { "draw16", -1.0f }, { "mx1Src", 16.0f }, { "mx1Dst", 9.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 16.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 9.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        { "Trap Hat Drone", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.7f },
            { "noiseType", 0.0f }, { "noiseTone", 0.9f }, { "filterType", 2.0f }, { "cutoff", 6500.0f },
            { "resonance", 1.0f }, { "attack", 0.002f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.05f },
            { "dwRate", 1.0f }, { "dwSync", 7.0f }, { "dwSmooth", 0.0f }, { "dwSteps", 16.0f }, { "draw1", 1.0f },
            { "draw2", -1.0f }, { "draw3", -0.2f }, { "draw4", -1.0f }, { "draw5", 0.7f }, { "draw6", -1.0f },
            { "draw7", -0.3f }, { "draw8", -1.0f }, { "draw9", 1.0f }, { "draw10", -1.0f }, { "draw11", -0.2f },
            { "draw12", -1.0f }, { "draw13", 0.7f }, { "draw14", -0.2f }, { "draw15", 0.4f }, { "draw16", -1.0f },
            { "mx1Src", 16.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 19.0f }, { "mx3Amt", 0.5f } } },

        { "Arpege Fantome", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "oscBPos", 0.286f },
            { "oscBLevel", 0.5f }, { "oscBOct", 1.0f }, { "unison", 3.0f }, { "detune", 8.0f }, { "filterType", 0.0f },
            { "cutoff", 3500.0f }, { "attack", 0.005f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.4f },
            { "dwRate", 1.0f }, { "dwSync", 7.0f }, { "dwSmooth", 0.0f }, { "dwSteps", 8.0f }, { "draw1", -1.0f },
            { "draw2", 0.1f }, { "draw3", 0.9f }, { "draw4", 0.1f }, { "draw5", -1.0f }, { "draw6", 0.3f },
            { "draw7", 1.0f }, { "draw8", 0.3f }, { "delayMix", 0.4f }, { "delaySync", 5.0f }, { "delayMode", 1.0f },
            { "delayFeedback", 0.5f }, { "reverbMix", 0.3f }, { "mx1Src", 16.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 16.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 9.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f } } },

        { "Stutter Nappe", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "oscBPos", 0.5f },
            { "oscBLevel", 0.6f }, { "unison", 5.0f }, { "detune", 15.0f }, { "spread", 0.9f }, { "filterType", 0.0f },
            { "cutoff", 5000.0f }, { "attack", 0.2f }, { "decay", 0.4f }, { "sustain", 1.0f }, { "release", 0.6f },
            { "fx1Type", 7.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.4f }, { "fx1B", 0.3f },
            { "fx1C", 0.6f }, { "fx1D", 0.0f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.4f } } },

        { "Stutter Inverse", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.3f }, { "oscBPos", 0.6f },
            { "oscBLevel", 0.5f }, { "unison", 3.0f }, { "detune", 10.0f }, { "filterType", 1.0f },
            { "cutoff", 3000.0f }, { "resonance", 1.0f }, { "attack", 0.002f }, { "decay", 0.4f }, { "sustain", 0.6f },
            { "release", 0.3f }, { "fx1Type", 7.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.3f },
            { "fx1B", 0.2f }, { "fx1C", 0.7f }, { "fx1D", 1.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f } } },

        { "Marche Industrielle", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "position", 0.5f }, { "subLevel", 0.5f },
            { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 1.0f }, { "cutoff", 600.0f },
            { "resonance", 2.0f }, { "distType", 1.0f }, { "distAmt", 0.25f }, { "attack", 0.002f }, { "decay", 0.3f },
            { "sustain", 1.0f }, { "release", 0.06f }, { "dwRate", 1.0f }, { "dwSync", 7.0f }, { "dwSmooth", 0.0f },
            { "dwSteps", 8.0f }, { "draw1", 1.0f }, { "draw2", -1.0f }, { "draw3", -1.0f }, { "draw4", 0.3f },
            { "draw5", 1.0f }, { "draw6", -1.0f }, { "draw7", 0.3f }, { "draw8", -1.0f }, { "mx1Src", 16.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 16.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.6f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.5f } } },

        //======================================================================== CLOCHES
        { "FM Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 6.0f }, { "fmAmount", 0.35f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 12000.0f }, { "attack", 0.001f }, { "decay", 1.8f },
            { "sustain", 0.0f }, { "release", 1.5f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "mx1Src", 9.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.4f }, { "voiceGain", -2.5f } } },

        { "Iron Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", -13.0f }, { "ringAmount", 0.6f }, { "fmAmount", 0.15f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 10000.0f }, { "attack", 0.001f },
            { "decay", 2.0f }, { "sustain", 0.0f }, { "release", 1.8f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "delayMix", 0.2f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", -2.2f } } },

        { "Dark Chime", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 6.0f }, { "position", 0.4f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.001f }, { "decay", 2.5f },
            { "sustain", 0.0f }, { "release", 2.0f }, { "reverbMix", 0.55f }, { "reverbSize", 0.9f },
            { "reverbHP", 250.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 8.2f } } },

        { "Tube Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 5.0f }, { "position", 0.3f }, { "oscBPos", 0.3f },
            { "oscBLevel", 0.3f }, { "oscBOct", 1.0f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 8000.0f }, { "attack", 0.001f }, { "decay", 1.5f }, { "sustain", 0.0f }, { "release", 1.5f },
            { "reverbMix", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -2.8f } } },

        { "Cold Gong", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBTune", -19.0f }, { "ringAmount", 0.5f }, { "noiseLevel", 0.12f }, { "noiseType", 1.0f },
            { "noiseDecay", 0.6f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5000.0f },
            { "attack", 0.003f }, { "decay", 3.0f }, { "sustain", 0.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -1.6f } } },

        { "Robot Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "fmAmount", 0.5f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.3f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 0.8f },
            { "sustain", 0.0f }, { "release", 0.6f }, { "delayMode", 1.0f }, { "delaySync", 3.0f },
            { "delayMix", 0.3f }, { "delayFeedback", 0.5f }, { "delayLP", 6000.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 16.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 14.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", -1.2f } } },

        { "Tubular Bells", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 30.0f }, { "ringAmount", 0.45f }, { "fmAmount", 0.15f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 11000.0f }, { "attack", 0.001f },
            { "decay", 3.0f }, { "sustain", 0.0f }, { "release", 2.5f }, { "voiceGain", 0.2f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 120.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f } } },

        { "Church Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 12.0f }, { "ringAmount", 0.5f }, { "fmAmount", 0.25f },
            { "modDecay", 1.5f }, { "modSustain", 0.0f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 4.5f }, { "sustain", 0.0f }, { "release", 4.0f },
            { "voiceGain", -0.9f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 100.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 17.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f } } },

        { "Gamelan", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", -20.0f }, { "oscBLevel", 0.3f }, { "ringAmount", 0.55f },
            { "unison", 2.0f }, { "detune", 25.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.001f }, { "decay", 1.8f }, { "sustain", 0.0f }, { "release", 1.2f }, { "voiceGain", 0.3f },
            { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 120.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f },
            { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f } } },

        { "Steel Drum", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.5f }, { "oscBTune", 6.0f }, { "fmAmount", 0.1f },
            { "pitchEnvAmt", 2.0f }, { "pitchEnvDecay", 0.02f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 5500.0f }, { "attack", 0.001f }, { "decay", 0.6f }, { "sustain", 0.0f }, { "release", 0.4f },
            { "voiceGain", -1.7f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 100.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 7.0f },
            { "mx2Amt", 0.5f } } },

        { "Cloche Resonateur", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.143f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.3f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 1.0f }, { "sustain", 0.0f }, { "release", 1.0f },
            { "fx1Type", 4.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.7f }, { "fx1A", 0.35f }, { "fx1B", 0.75f },
            { "fx1C", 0.3f }, { "fx1D", 1.0f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f } } },

        { "Bol Chante", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 19.0f }, { "oscBLevel", 0.4f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 0.002f }, { "decay", 2.0f },
            { "sustain", 0.0f }, { "release", 2.0f }, { "fx1Type", 4.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.6f },
            { "fx1A", 0.45f }, { "fx1B", 0.85f }, { "fx1C", 0.5f }, { "fx1D", 1.0f }, { "fx2Type", 8.0f },
            { "fx2On", 1.0f }, { "fx2Mix", 0.3f }, { "fx2A", 0.75f }, { "fx2B", 0.4f }, { "fx2C", 0.5f },
            { "fx2D", 0.6f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        //======================================================================== CORDES
        { "Dark Strings", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.55f }, { "unison", 6.0f },
            { "detune", 16.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "attack", 0.9f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 1.5f }, { "chorusMix", 0.4f }, { "reverbMix", 0.35f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f }, { "voiceGain", 4.3f } } },

        { "Analog Strings", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 5.0f }, { "detune", 20.0f },
            { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "attack", 0.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.2f }, { "chorusMix", 0.5f }, { "phaserMix", 0.25f },
            { "phaserRate", 0.3f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 7.9f } } },

        { "Ensemble Lent", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 6.0f },
            { "detune", 16.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.0f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -3.1f } } },

        { "Ensemble Pizzicato", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 4.0f },
            { "detune", 10.0f }, { "spread", 0.7f }, { "filterType", 1.0f }, { "cutoff", 2200.0f },
            { "fltEnvAmount", 0.4f }, { "fltDecay", 0.12f }, { "fltSustain", 0.0f }, { "attack", 0.001f },
            { "decay", 0.25f }, { "sustain", 0.0f }, { "release", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -2.4f } } },

        { "Cordes Cinema", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 7.0f }, { "detune", 22.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "attack", 1.5f }, { "decay", 0.5f },
            { "sustain", 1.0f }, { "release", 2.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 110.0f },
            { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f },
            { "voiceGain", -2.5f } } },

        { "Cello Synth", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 8.0f },
            { "spread", 0.5f }, { "filterType", 1.0f }, { "cutoff", 1200.0f }, { "resonance", 1.0f },
            { "fltEnvAmount", 0.3f }, { "fltAttack", 0.15f }, { "fltDecay", 0.4f }, { "fltSustain", 0.6f },
            { "lfoRate", 5.2f }, { "attack", 0.15f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.3f },
            { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.012f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -3.4f } } },

        { "Harpe Synth", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.2f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 6000.0f }, { "noiseLevel", 0.06f }, { "noiseTone", 0.5f }, { "noiseDecay", 0.01f },
            { "attack", 0.001f }, { "decay", 1.5f }, { "sustain", 0.0f }, { "release", 0.8f }, { "voiceGain", -0.1f },
            { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 80.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f } } },

        //======================================================================== CLAVIERS
        { "Digital EP", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 0.0f }, { "fmAmount", 0.2f }, { "modDecay", 0.3f }, { "modSustain", 0.0f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "velFilter", 0.4f },
            { "attack", 0.002f }, { "decay", 1.2f }, { "sustain", 0.0f }, { "release", 0.5f }, { "chorusMix", 0.3f },
            { "reverbMix", 0.25f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 4.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f }, { "voiceGain", -0.9f } } },

        { "Organ Drone", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.5f }, { "unison", 2.0f },
            { "detune", 6.0f }, { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.02f }, { "decay", 0.3f },
            { "sustain", 1.0f }, { "release", 0.2f }, { "chorusMix", 0.35f }, { "lfoRate", 6.0f },
            { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "voiceGain", -2.0f } } },

        { "Clav Funk", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 2.0f }, { "position", 0.3f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 1800.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.12f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.25f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "velFilter", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 0.6f } } },

        { "Dark Keys", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.25f }, { "fmAmount", 0.1f }, { "unison", 2.0f }, { "detune", 7.0f },
            { "filterType", 1.0f }, { "cutoff", 1800.0f }, { "attack", 0.003f }, { "decay", 0.8f },
            { "sustain", 0.2f }, { "release", 0.6f }, { "chorusMix", 0.3f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -1.6f } } },

        { "Rhodes", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "fmAmount", 0.2f }, { "modDecay", 0.25f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "velFilter", 0.5f }, { "attack", 0.002f },
            { "decay", 1.6f }, { "sustain", 0.15f }, { "release", 0.35f }, { "voiceGain", -1.7f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 70.0f }, { "eq1Q", 0.71f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f },
            { "mx1Amt", 0.35f }, { "mx2Src", 4.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.15f }, { "mx3Src", 9.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.5f } } },

        { "Wurli", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.286f }, { "fmAmount", 0.08f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 2600.0f }, { "resonance", 1.2f },
            { "distType", 0.0f }, { "distAmt", 0.1f }, { "velFilter", 0.6f }, { "attack", 0.002f }, { "decay", 0.9f },
            { "sustain", 0.25f }, { "release", 0.2f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 90.0f },
            { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f }, { "voiceGain", -6.0f } } },

        { "DX Piano", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "fmAmount", 0.4f }, { "modDecay", 0.6f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "velFilter", 0.5f }, { "attack", 0.001f },
            { "decay", 1.8f }, { "sustain", 0.0f }, { "release", 0.4f }, { "voiceGain", -1.3f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 60.0f }, { "eq1Q", 0.71f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 4.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.5f } } },

        { "Clavinet", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 2.0f }, { "position", 0.35f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 2000.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.1f },
            { "fltSustain", 0.2f }, { "velFilter", 0.6f }, { "attack", 0.001f }, { "decay", 0.3f },
            { "sustain", 0.0f }, { "release", 0.08f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 150.0f },
            { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f }, { "voiceGain", 7.3f } } },

        { "Hammond", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.83f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.003f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.05f }, { "noiseLevel", 0.03f }, { "noiseTone", 0.9f },
            { "noiseDecay", 0.01f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -2.1f } } },

        { "Church Organ", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.71f },
            { "oscBLevel", 0.3f }, { "tableB", 4.0f }, { "oscBPos", 0.4f }, { "oscBOct", 1.0f }, { "unison", 2.0f },
            { "detune", 5.0f }, { "subLevel", 0.3f }, { "filterType", 0.0f }, { "cutoff", 6000.0f },
            { "attack", 0.05f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.4f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", -3.9f } } },

        { "Harmonium", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.857f }, { "unison", 2.0f },
            { "detune", 8.0f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "lfoRate", 5.0f },
            { "attack", 0.04f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.2f }, { "mx1Src", 1.0f },
            { "mx1Dst", 5.0f }, { "mx1Amt", 0.01f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -6.0f } } },

        { "Synth Piano", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "unison", 2.0f },
            { "detune", 4.0f }, { "filterType", 1.0f }, { "cutoff", 3500.0f }, { "fltEnvAmount", 0.4f },
            { "fltDecay", 0.5f }, { "fltSustain", 0.1f }, { "velFilter", 0.6f }, { "noiseLevel", 0.08f },
            { "noiseTone", 0.6f }, { "noiseDecay", 0.01f }, { "attack", 0.001f }, { "decay", 1.2f },
            { "sustain", 0.05f }, { "release", 0.4f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 60.0f },
            { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -1.3f } } },

        { "Orgue Rotary", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.857f }, { "oscBPos", 0.857f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.5f }, { "unison", 2.0f }, { "detune", 4.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.08f },
            { "fx1Type", 11.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.45f }, { "fx1B", 0.65f },
            { "fx1C", 0.5f }, { "fx1D", 0.35f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Piano Cassette", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.3f }, { "fmAmount", 0.2f }, { "modDecay", 0.3f },
            { "modSustain", 0.0f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5500.0f },
            { "velFilter", 0.5f }, { "attack", 0.002f }, { "decay", 1.8f }, { "sustain", 0.1f }, { "release", 0.5f },
            { "voiceGain", 3.0f }, { "fx1Type", 9.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.45f },
            { "fx1B", 0.35f }, { "fx1C", 0.4f }, { "fx1D", 0.4f }, { "fx2Type", 10.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 0.4f }, { "fx2A", 0.5f }, { "fx2B", 0.3f }, { "fx2C", 0.2f }, { "fx2D", 0.25f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Cordes Rotary", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "oscBPos", 0.429f },
            { "oscBLevel", 0.8f }, { "oscBTune", -9.0f }, { "unison", 5.0f }, { "detune", 11.0f }, { "spread", 0.7f },
            { "filterType", 1.0f }, { "cutoff", 4500.0f }, { "attack", 0.35f }, { "decay", 0.5f }, { "sustain", 0.9f },
            { "release", 0.6f }, { "fx1Type", 11.0f }, { "fx1On", 1.0f }, { "fx1Mix", 0.6f }, { "fx1A", 0.2f },
            { "fx1B", 0.5f }, { "fx1C", 0.5f }, { "fx1D", 0.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f } } },

        { "Cuivres Rack", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "oscBPos", 0.429f },
            { "oscBLevel", 0.8f }, { "oscBTune", -8.0f }, { "unison", 3.0f }, { "detune", 8.0f },
            { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "fltEnvAmount", 0.5f }, { "fltAttack", 0.08f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.4f }, { "attack", 0.05f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.15f }, { "fx1Type", 17.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.5f },
            { "fx1B", 0.2f }, { "fx1C", 0.4f }, { "fx1D", 1.0f }, { "fx2Type", 19.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 0.4f }, { "fx2A", 0.4f }, { "fx2B", 0.4f }, { "fx2C", 0.4f }, { "fx2D", 0.2f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Piano Glue", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.3f }, { "fmAmount", 0.2f }, { "modDecay", 0.3f },
            { "modSustain", 0.0f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 6500.0f },
            { "velFilter", 0.5f }, { "attack", 0.002f }, { "decay", 1.8f }, { "sustain", 0.1f }, { "release", 0.5f },
            { "voiceGain", 3.0f }, { "fx1Type", 17.0f }, { "fx1On", 1.0f }, { "fx1Mix", 1.0f }, { "fx1A", 0.5f },
            { "fx1B", 0.15f }, { "fx1C", 0.4f }, { "fx1D", 1.0f }, { "fx2Type", 19.0f }, { "fx2On", 1.0f },
            { "fx2Mix", 0.3f }, { "fx2A", 0.4f }, { "fx2B", 0.4f }, { "fx2C", 0.4f }, { "fx2D", 0.2f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        //======================================================================== MALLETS
        { "Vibraphone", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.15f }, { "fmAmount", 0.05f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "lfoRate", 5.0f }, { "attack", 0.001f },
            { "decay", 2.5f }, { "sustain", 0.0f }, { "release", 1.5f }, { "voiceGain", -1.3f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 100.0f }, { "eq1Q", 0.71f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f },
            { "mx1Amt", 0.2f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 7.0f }, { "mx3Amt", 0.5f } } },

        { "Marimba", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.25f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 4500.0f }, { "velFilter", 0.5f }, { "noiseLevel", 0.1f }, { "noiseTone", 0.4f },
            { "noiseDecay", 0.01f }, { "attack", 0.001f }, { "decay", 0.6f }, { "sustain", 0.0f }, { "release", 0.3f },
            { "voiceGain", 0.5f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 80.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 7.0f },
            { "mx2Amt", 0.5f } } },

        { "Xylophone", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.4f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "noiseLevel", 0.15f }, { "noiseTone", 0.8f }, { "noiseDecay", 0.008f },
            { "attack", 0.001f }, { "decay", 0.25f }, { "sustain", 0.0f }, { "release", 0.15f },
            { "voiceGain", -0.6f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 150.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 7.0f },
            { "mx2Amt", 0.5f } } },

        { "Glockenspiel", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.2f }, { "fmAmount", 0.15f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 15000.0f }, { "attack", 0.001f }, { "decay", 1.2f },
            { "sustain", 0.0f }, { "release", 0.8f }, { "voiceGain", -1.6f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 300.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f } } },

        { "Music Box", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscAOct", 1.0f },
            { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "oscBLevel", 0.2f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 12000.0f }, { "attack", 0.001f }, { "decay", 0.9f },
            { "sustain", 0.0f }, { "release", 0.5f }, { "voiceGain", -0.3f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 250.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f } } },

        { "Kalimba", "MALLETS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "oscBLevel", 0.1f }, { "fmAmount", 0.12f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "noiseLevel", 0.1f }, { "noiseTone", 0.5f },
            { "noiseDecay", 0.01f }, { "attack", 0.001f }, { "decay", 0.7f }, { "sustain", 0.0f }, { "release", 0.4f },
            { "voiceGain", -2.5f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 120.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f },
            { "mx2Amt", 0.5f } } },

        //======================================================================== CUIVRES
        { "Trompette", "CUIVRES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 5.0f },
            { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.0f }, { "fltEnvAmount", 0.6f },
            { "fltAttack", 0.06f }, { "fltDecay", 0.3f }, { "fltSustain", 0.5f }, { "velFilter", 0.6f },
            { "noiseLevel", 0.05f }, { "noiseTone", 0.6f }, { "noiseDecay", 5.0f }, { "attack", 0.04f },
            { "decay", 0.2f }, { "sustain", 0.9f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -4.1f } } },

        { "Trombone", "CUIVRES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 5.0f },
            { "filterType", 1.0f }, { "cutoff", 450.0f }, { "resonance", 1.0f }, { "fltEnvAmount", 0.55f },
            { "fltAttack", 0.08f }, { "fltDecay", 0.35f }, { "fltSustain", 0.5f }, { "velFilter", 0.5f },
            { "attack", 0.07f }, { "decay", 0.2f }, { "sustain", 0.9f }, { "release", 0.2f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", -3.5f } } },

        { "Section Cuivres", "CUIVRES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 5.0f },
            { "detune", 10.0f }, { "spread", 0.7f }, { "filterType", 1.0f }, { "cutoff", 900.0f },
            { "fltEnvAmount", 0.55f }, { "fltAttack", 0.06f }, { "fltDecay", 0.3f }, { "fltSustain", 0.55f },
            { "velFilter", 0.6f }, { "attack", 0.06f }, { "decay", 0.2f }, { "sustain", 0.9f }, { "release", 0.25f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f }, { "voiceGain", -4.2f } } },

        { "Stab Cuivres", "CUIVRES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 5.0f },
            { "detune", 10.0f }, { "spread", 0.7f }, { "filterType", 1.0f }, { "cutoff", 1000.0f },
            { "fltEnvAmount", 0.6f }, { "fltAttack", 0.005f }, { "fltDecay", 0.25f }, { "fltSustain", 0.0f },
            { "velFilter", 0.6f }, { "attack", 0.005f }, { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.2f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f }, { "voiceGain", -3.0f } } },

        { "Cuivre Doux", "CUIVRES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.36f }, { "unison", 3.0f }, { "detune", 6.0f },
            { "filterType", 1.0f }, { "cutoff", 600.0f }, { "fltEnvAmount", 0.4f }, { "fltAttack", 0.15f },
            { "fltDecay", 0.4f }, { "fltSustain", 0.6f }, { "attack", 0.12f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -4.3f } } },

        //======================================================================== VENTS
        { "Flute", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.07f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "noiseLevel", 0.08f }, { "noiseType", 1.0f },
            { "noiseTone", 0.7f }, { "noiseDecay", 5.0f }, { "lfoRate", 5.2f }, { "attack", 0.06f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.15f }, { "voiceGain", -3.3f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.01f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.5f } } },

        { "Flute de Pan", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "noiseLevel", 0.18f }, { "noiseType", 1.0f },
            { "noiseTone", 0.6f }, { "noiseDecay", 5.0f }, { "lfoRate", 4.8f }, { "attack", 0.08f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.2f }, { "voiceGain", -3.9f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.008f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.5f } } },

        { "Shakuhachi", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "noiseLevel", 0.25f }, { "noiseType", 1.0f },
            { "noiseTone", 0.5f }, { "noiseDecay", 5.0f }, { "lfoRate", 4.5f }, { "attack", 0.1f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.25f }, { "voiceGain", -3.9f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.03f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.5f } } },

        { "Ocarina", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "noiseLevel", 0.02f }, { "noiseType", 1.0f },
            { "noiseDecay", 5.0f }, { "lfoRate", 5.0f }, { "attack", 0.05f }, { "decay", 0.1f }, { "sustain", 1.0f },
            { "release", 0.15f }, { "voiceGain", -3.9f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.01f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 19.0f },
            { "mx3Amt", 0.5f } } },

        { "Clarinette", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.286f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "resonance", 1.0f }, { "noiseLevel", 0.03f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "lfoRate", 5.0f }, { "attack", 0.05f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.006f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 19.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", -7.3f } } },

        { "Hautbois", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.714f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "resonance", 1.5f }, { "noiseLevel", 0.03f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "lfoRate", 5.0f }, { "attack", 0.04f }, { "decay", 0.1f },
            { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.008f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 19.0f },
            { "mx3Amt", 0.5f }, { "voiceGain", -3.6f } } },

        { "Harmonica", "VENTS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.857f }, { "unison", 2.0f },
            { "detune", 14.0f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "noiseLevel", 0.04f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "attack", 0.03f }, { "decay", 0.1f }, { "sustain", 1.0f },
            { "release", 0.1f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -6.0f } } },

        //======================================================================== VOIX
        { "Voix Aah", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.0f }, { "unison", 5.0f },
            { "detune", 10.0f }, { "spread", 0.7f }, { "filterType", 0.0f }, { "cutoff", 7000.0f },
            { "lfoRate", 5.0f }, { "attack", 0.3f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.5f },
            { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.025f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -3.2f } } },

        { "Voix Ooh", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.571f },
            { "unison", 5.0f }, { "detune", 10.0f }, { "spread", 0.7f }, { "filterType", 0.0f }, { "cutoff", 6000.0f },
            { "lfoRate", 5.0f }, { "attack", 0.3f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.5f },
            { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.025f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -2.7f } } },

        { "Choeur", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.43f }, { "unison", 7.0f },
            { "detune", 20.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.5f },
            { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -3.5f } } },

        { "Voix Lunaire", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.1f }, { "oscBPos", 0.1f },
            { "tableB", 1.0f }, { "oscBOct", 1.0f }, { "oscBLevel", 0.3f }, { "unison", 4.0f }, { "detune", 14.0f },
            { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "lfo3Rate", 0.15f },
            { "lfo3Shape", 1.0f }, { "attack", 0.8f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 1.2f },
            { "mx1Src", 14.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "voiceGain", -3.6f } } },

        { "Chorale A-E-I-O-U", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 0.0f }, { "position", 0.429f },
            { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.7f }, { "filterType", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.25f }, { "decay", 0.5f }, { "sustain", 0.9f }, { "release", 0.8f }, { "reverbMix", 0.35f },
            { "reverbSize", 0.8f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f }, { "f2Mix", 0.9f },
            { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "dwRate", 0.25f }, { "dwSync", 0.0f }, { "dwSmooth", 0.7f },
            { "dwSteps", 8.0f }, { "draw1", -1.0f }, { "draw2", -0.5f }, { "draw3", 0.0f }, { "draw4", 0.5f },
            { "draw5", 1.0f }, { "draw6", 0.5f }, { "draw7", 0.0f }, { "draw8", -0.5f }, { "mx1Src", 16.0f },
            { "mx1Dst", 22.0f }, { "mx1Amt", 0.9f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f },
            { "mx3Src", 10.0f }, { "mx3Dst", 21.0f }, { "mx3Amt", 0.4f } } },

        { "Voyelle Machine", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 0.0f }, { "position", 0.429f },
            { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 0.0f }, { "cutoff", 12000.0f },
            { "attack", 0.01f }, { "decay", 0.4f }, { "sustain", 0.8f }, { "release", 0.25f }, { "f2Type", 5.0f },
            { "f2Cutoff", 1000.0f }, { "f2Res", 0.7f }, { "f2Mix", 1.0f }, { "f2Vowel", 0.3f }, { "f2Mode", 0.0f },
            { "lfoShape", 0.0f }, { "lfoRate", 3.5f }, { "mx1Src", 1.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.35f },
            { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f }, { "mx3Dst", 21.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 7.0f }, { "mx4Amt", 0.4f } } },

        { "Wah Wah Vocal", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "unison", 2.0f },
            { "detune", 7.0f }, { "filterType", 0.0f }, { "cutoff", 12000.0f }, { "attack", 0.005f },
            { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.2f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f },
            { "f2Res", 0.7f }, { "f2Mix", 1.0f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "lfoShape", 0.0f },
            { "lfoRate", 2.2f }, { "velFilter", 0.2f }, { "mx1Src", 1.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f }, { "mx3Dst", 21.0f },
            { "mx3Amt", 0.5f } } },

        { "Vocodeur Fantome", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 7.0f }, { "detune", 10.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 14000.0f }, { "attack", 0.4f }, { "decay", 0.4f },
            { "sustain", 0.9f }, { "release", 1.0f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.5f },
            { "f2Mix", 0.8f }, { "f2Vowel", 0.5f }, { "f2Mode", 0.0f }, { "fx1Type", 8.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.4f }, { "fx1A", 0.75f }, { "fx1B", 0.4f }, { "fx1C", 0.5f }, { "fx1D", 0.6f },
            { "fx2Type", 20.0f }, { "fx2On", 1.0f }, { "fx2Mix", 1.0f }, { "fx2A", 0.8f }, { "fx2B", 0.1f },
            { "fx2C", 0.5f }, { "fx2D", 0.5f }, { "mx1Src", 8.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f }, { "mx3Dst", 21.0f },
            { "mx3Amt", 0.4f } } },

        { "Chant Gregorien", "VOIX", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.429f }, { "unison", 4.0f },
            { "detune", 6.0f }, { "spread", 0.6f }, { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.3f },
            { "decay", 0.5f }, { "sustain", 0.9f }, { "release", 0.9f }, { "f2Type", 5.0f }, { "f2Cutoff", 950.0f },
            { "f2Res", 0.7f }, { "f2Mix", 0.9f }, { "f2Vowel", 0.57f }, { "f2Mode", 0.0f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.95f }, { "chorusMix", 0.25f }, { "mx1Src", 8.0f }, { "mx1Dst", 22.0f },
            { "mx1Amt", 0.15f }, { "mx2Src", 9.0f }, { "mx2Dst", 22.0f }, { "mx2Amt", 0.8f }, { "mx3Src", 10.0f },
            { "mx3Dst", 21.0f }, { "mx3Amt", 0.4f } } },

        //======================================================================== PLUCKS
        { "Basse Pincee", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.143f },
            { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 600.0f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "velFilter", 0.5f }, { "noiseLevel", 0.04f },
            { "noiseTone", 0.4f }, { "noiseDecay", 0.01f }, { "attack", 0.002f }, { "decay", 0.6f },
            { "sustain", 0.1f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -1.6f } } },

        { "Contrebasse", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f },
            { "subLevel", 0.3f }, { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 500.0f },
            { "noiseLevel", 0.1f }, { "noiseTone", 0.3f }, { "noiseDecay", 0.01f }, { "attack", 0.002f },
            { "decay", 0.7f }, { "sustain", 0.05f }, { "release", 0.2f }, { "voiceGain", -0.9f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f } } },

        { "Clavecin", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 6.0f },
            { "filterType", 0.0f }, { "cutoff", 4500.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.1f },
            { "fltSustain", 0.0f }, { "noiseLevel", 0.12f }, { "noiseTone", 0.7f }, { "noiseDecay", 0.008f },
            { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f }, { "release", 0.1f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 120.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -6.0f } } },

        { "Luth Synth", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.286f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 1800.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.25f },
            { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.8f }, { "sustain", 0.0f }, { "release", 0.2f },
            { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 90.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f },
            { "voiceGain", -6.0f } } },

        { "Banjo Synth", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.571f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.08f },
            { "fltSustain", 0.0f }, { "pitchEnvAmt", 3.0f }, { "pitchEnvDecay", 0.02f }, { "attack", 0.001f },
            { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.1f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 150.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 20.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 0.6f } } },

        { "Nylon Synth", "PLUCKS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.2f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 3000.0f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.3f }, { "fltSustain", 0.0f },
            { "attack", 0.001f }, { "decay", 0.9f }, { "sustain", 0.0f }, { "release", 0.2f }, { "eq1On", 1.0f },
            { "eq1Type", 0.0f }, { "eq1Freq", 80.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f }, { "voiceGain", -1.2f } } },

        //======================================================================== PERCUSSIONS
        { "Hat Ferme", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.3f }, { "position", 0.286f },
            { "tableB", 0.0f }, { "oscBPos", 0.286f }, { "oscBOct", 1.0f }, { "oscBTune", 30.0f },
            { "ringAmount", 0.5f }, { "noiseLevel", 1.0f }, { "noiseType", 0.0f }, { "noiseTone", 1.0f },
            { "noiseDecay", 0.05f }, { "unison", 1.0f }, { "filterType", 2.0f }, { "cutoff", 6500.0f },
            { "attack", 0.001f }, { "decay", 0.05f }, { "sustain", 0.0f }, { "release", 0.03f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f },
            { "voiceGain", 1.4f } } },

        { "Hat Ouvert", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.3f }, { "position", 0.286f },
            { "oscBPos", 0.286f }, { "oscBOct", 1.0f }, { "oscBTune", 30.0f }, { "ringAmount", 0.5f },
            { "noiseLevel", 1.0f }, { "noiseType", 0.0f }, { "noiseTone", 1.0f }, { "noiseDecay", 0.35f },
            { "unison", 1.0f }, { "filterType", 2.0f }, { "cutoff", 6500.0f }, { "attack", 0.001f },
            { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 0.1f } } },

        { "Shaker", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 1.0f },
            { "noiseType", 0.0f }, { "noiseTone", 0.6f }, { "noiseDecay", 0.09f }, { "unison", 1.0f },
            { "filterType", 3.0f }, { "cutoff", 6500.0f }, { "resonance", 1.2f }, { "attack", 0.02f },
            { "decay", 0.09f }, { "sustain", 0.0f }, { "release", 0.05f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.3f }, { "voiceGain", 12.0f } } },

        { "Tambourin", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.3f }, { "position", 0.286f },
            { "oscBPos", 0.286f }, { "oscBOct", 2.0f }, { "oscBTune", 20.0f }, { "ringAmount", 0.35f },
            { "noiseLevel", 0.7f }, { "noiseType", 0.0f }, { "noiseTone", 0.9f }, { "noiseDecay", 0.2f },
            { "unison", 1.0f }, { "filterType", 2.0f }, { "cutoff", 4500.0f }, { "attack", 0.003f },
            { "decay", 0.22f }, { "sustain", 0.0f }, { "release", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f }, { "voiceGain", 0.5f } } },

        { "Claves", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscAOct", 2.0f }, { "unison", 1.0f },
            { "pitchEnvAmt", 3.0f }, { "pitchEnvDecay", 0.01f }, { "filterType", 0.0f }, { "cutoff", 8000.0f },
            { "attack", 0.001f }, { "decay", 0.08f }, { "sustain", 0.0f }, { "release", 0.05f },
            { "voiceGain", -1.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 20.0f }, { "mx2Amt", 0.5f } } },

        { "Woodblock", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.143f }, { "oscAOct", 1.0f },
            { "noiseLevel", 0.1f }, { "noiseTone", 0.7f }, { "noiseDecay", 0.01f }, { "unison", 1.0f },
            { "filterType", 3.0f }, { "cutoff", 1800.0f }, { "resonance", 4.0f }, { "attack", 0.001f },
            { "decay", 0.12f }, { "sustain", 0.0f }, { "release", 0.05f }, { "voiceGain", 10.7f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f } } },

        { "Triangle", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscAOct", 2.0f },
            { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "oscBTune", 40.0f }, { "ringAmount", 0.6f },
            { "unison", 1.0f }, { "filterType", 2.0f }, { "cutoff", 1500.0f }, { "attack", 0.001f }, { "decay", 2.0f },
            { "sustain", 0.0f }, { "release", 1.5f }, { "voiceGain", 10.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f } } },

        { "Rim Click", "PERCUSSIONS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscAOct", 2.0f },
            { "pitchEnvAmt", 6.0f }, { "pitchEnvDecay", 0.008f }, { "noiseLevel", 0.3f }, { "noiseTone", 0.8f },
            { "noiseDecay", 0.02f }, { "unison", 1.0f }, { "filterType", 3.0f }, { "cutoff", 2500.0f },
            { "resonance", 2.5f }, { "attack", 0.001f }, { "decay", 0.04f }, { "sustain", 0.0f }, { "release", 0.03f },
            { "voiceGain", 6.1f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f } } },

        //======================================================================== GUITARES
        { "Guitare Classique", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.0f }, { "strDecay", 2.2f }, { "strDamp", 0.35f },
            { "strPos", 0.22f }, { "strBright", 0.45f }, { "cutoff", 5000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 80.0f }, { "eq1Q", 0.71f }, { "eq2On", 1.0f }, { "eq2Type", 2.0f }, { "eq2Freq", 200.0f },
            { "eq2Gain", 3.0f }, { "eq2Q", 1.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 11.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.0f } } },

        { "Guitare Folk", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 4.1f }, { "strDecay", 2.8f }, { "strDamp", 0.5f },
            { "strPos", 0.18f }, { "strBright", 0.7f }, { "cutoff", 8000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 80.0f }, { "eq1Q", 0.71f }, { "eq2On", 1.0f }, { "eq2Type", 2.0f }, { "eq2Freq", 3500.0f },
            { "eq2Gain", 2.0f }, { "eq2Q", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Guitare Nylon", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.7f }, { "strDecay", 1.8f }, { "strDamp", 0.25f },
            { "strPos", 0.25f }, { "strBright", 0.3f }, { "cutoff", 4000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 80.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Guitare Electrique", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.2f }, { "strDecay", 3.5f }, { "strDamp", 0.6f },
            { "strPos", 0.12f }, { "strBright", 0.6f }, { "cutoff", 7000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 90.0f }, { "eq1Q", 0.71f }, { "eq2On", 1.0f }, { "eq2Type", 2.0f }, { "eq2Freq", 2500.0f },
            { "eq2Gain", 2.0f }, { "eq2Q", 1.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f } } },

        { "Guitare Etouffee", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 4.8f }, { "strDecay", 0.35f }, { "strDamp", 0.2f },
            { "strPos", 0.15f }, { "strBright", 0.5f }, { "cutoff", 4500.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 100.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Harpe", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.5f },
            { "filterType", 0.0f }, { "voiceGain", 2.1f }, { "strDecay", 4.0f }, { "strDamp", 0.75f },
            { "strPos", 0.35f }, { "strBright", 0.5f }, { "cutoff", 9000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 60.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Banjo", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.0f }, { "strDecay", 0.9f }, { "strDamp", 0.8f },
            { "strPos", 0.1f }, { "strBright", 0.95f }, { "cutoff", 9000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 200.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Mandoline", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.1f }, { "strDecay", 1.1f }, { "strDamp", 0.7f },
            { "strPos", 0.12f }, { "strBright", 0.8f }, { "cutoff", 9000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 250.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Koto", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.4f }, { "strDecay", 2.2f }, { "strDamp", 0.7f },
            { "strPos", 0.08f }, { "strBright", 0.85f }, { "cutoff", 9000.0f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f },
            { "eq1Freq", 150.0f }, { "eq1Q", 0.71f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Sitar Synth", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 1.6f }, { "strDecay", 2.5f }, { "strDamp", 1.0f },
            { "strPos", 0.05f }, { "strBright", 1.0f }, { "cutoff", 9000.0f }, { "distType", 0.0f },
            { "distAmt", 0.15f }, { "eq1On", 1.0f }, { "eq1Type", 0.0f }, { "eq1Freq", 120.0f }, { "eq1Q", 0.71f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f } } },

        { "Corde Infinie", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 1.5f },
            { "filterType", 0.0f }, { "voiceGain", 1.6f }, { "strDecay", 12.0f }, { "strDamp", 1.0f },
            { "strPos", 0.3f }, { "strBright", 0.35f }, { "cutoff", 6000.0f }, { "reverbMix", 0.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Corde Metallique", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.0f }, { "strDecay", 3.0f }, { "strDamp", 0.9f },
            { "strPos", 0.1f }, { "strBright", 0.8f }, { "cutoff", 9000.0f }, { "fx1Type", 4.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.5f }, { "fx1A", 0.4f }, { "fx1B", 0.8f }, { "fx1C", 0.4f }, { "fx1D", 1.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Corde Pincee Lo-Fi", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f },
            { "filterType", 0.0f }, { "voiceGain", 3.0f }, { "strDecay", 2.0f }, { "strDamp", 0.4f },
            { "strPos", 0.2f }, { "strBright", 0.5f }, { "cutoff", 5000.0f }, { "fx1Type", 9.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 1.0f }, { "fx1A", 0.5f }, { "fx1B", 0.4f }, { "fx1C", 0.5f }, { "fx1D", 0.5f },
            { "fx2Type", 10.0f }, { "fx2On", 1.0f }, { "fx2Mix", 0.5f }, { "fx2A", 0.55f }, { "fx2B", 0.4f },
            { "fx2C", 0.3f }, { "fx2D", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } },

        { "Harpe Shimmer", "GUITARES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "strLevel", 1.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.5f },
            { "filterType", 0.0f }, { "voiceGain", 3.0f }, { "strDecay", 4.0f }, { "strDamp", 0.75f },
            { "strPos", 0.35f }, { "strBright", 0.5f }, { "cutoff", 9000.0f }, { "fx1Type", 8.0f }, { "fx1On", 1.0f },
            { "fx1Mix", 0.4f }, { "fx1A", 0.75f }, { "fx1B", 0.55f }, { "fx1C", 0.5f }, { "fx1D", 0.6f },
            { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f } } }
    };

    return presets;
}
