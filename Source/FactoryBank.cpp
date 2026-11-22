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
 } },

        //======================================================================== BASSES
        { "Sub Pur", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.0f }, { "subLevel", 0.55f }, { "filterType", 1.0f },
            { "cutoff", 700.0f }, { "resonance", 0.7f }, { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f } } },

        { "Reese Noir", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.5f },
            { "oscBLevel", 0.9f }, { "oscBPos", 0.5f }, { "oscBTune", 14.0f }, { "unison", 3.0f }, { "detune", 12.0f },
            { "spread", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 420.0f },
            { "resonance", 1.2f }, { "drive", 0.2f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f },
            { "release", 0.2f }, { "phaserMix", 0.25f }, { "phaserRate", 0.2f }, { "phaserFeedback", 0.4f },
            { "lfo3Rate", 0.15f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.6f } } },

        { "Acid Squelch", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.06f }, { "position", 0.6f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 6.0f }, { "drive", 0.3f }, { "fltEnvAmount", 0.7f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.18f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.3f },
            { "sustain", 0.6f }, { "release", 0.08f }, { "distType", 4.0f }, { "distAmt", 0.2f },
            { "velFilter", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.6f } } },

        { "Growl FM", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "fmAmount", 0.45f }, { "subLevel", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 1.0f }, { "distType", 2.0f },
            { "distAmt", 0.35f }, { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.15f }, { "mx1Src", 1.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.35f },
            { "mx2Src", 9.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f } } },

        { "Wobble 1/8", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.5f },
            { "unison", 3.0f }, { "detune", 14.0f }, { "spread", 0.4f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 380.0f }, { "resonance", 2.5f }, { "distType", 0.0f }, { "distAmt", 0.3f },
            { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.12f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.55f }, { "mx2Src", 1.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        { "Gate Bass 1/16", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.02f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f },
            { "cutoff", 900.0f }, { "resonance", 1.4f }, { "distType", 1.0f }, { "distAmt", 0.25f },
            { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 1.0f },
            { "release", 0.04f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Ring Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.05f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 7.0f },
            { "ringAmount", 0.6f }, { "subLevel", 0.4f }, { "filterType", 0.0f }, { "cutoff", 1800.0f },
            { "resonance", 0.9f }, { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Comb Laser Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "position", 0.5f }, { "subLevel", 0.5f }, { "filterType", 5.0f },
            { "cutoff", 220.0f }, { "resonance", 5.0f }, { "fltEnvAmount", 0.6f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.15f }, { "attack", 0.002f }, { "decay", 0.35f },
            { "sustain", 0.7f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Machine Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "tableA", 6.0f }, { "position", 0.3f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.4f },
            { "subLevel", 0.5f }, { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 2.0f },
            { "distType", 3.0f }, { "distAmt", 0.3f }, { "distMix", 0.5f }, { "attack", 0.002f }, { "decay", 0.25f },
            { "sustain", 0.9f }, { "release", 0.1f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 14.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f } } },

        { "Fold Sub", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "distType", 2.0f }, { "distAmt", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 18.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f } } },

        { "Laser Dive", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "pitchEnvAmt", 24.0f },
            { "pitchEnvDecay", 0.12f }, { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.5f },
            { "attack", 0.001f }, { "decay", 0.4f }, { "sustain", 0.5f }, { "release", 0.15f }, { "mx1Src", 9.0f },
            { "mx1Dst", 20.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Pump Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "position", 0.55f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 800.0f }, { "resonance", 1.2f }, { "lfoShape", 2.0f }, { "lfoSync", 5.0f },
            { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 1.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f } } },

        { "Dirty Mono", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 2.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.55f }, { "detune", 8.0f }, { "noiseLevel", 0.15f },
            { "noiseType", 2.0f }, { "noiseTone", 0.3f }, { "noiseDecay", 5.0f }, { "distType", 1.0f },
            { "distAmt", 0.4f }, { "filterType", 1.0f }, { "cutoff", 600.0f }, { "resonance", 1.6f },
            { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Phase Bass", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "position", 0.5f }, { "subLevel", 0.4f }, { "filterType", 1.0f },
            { "cutoff", 700.0f }, { "resonance", 1.0f }, { "phaserMix", 0.6f }, { "phaserRate", 0.3f },
            { "phaserDepth", 0.8f }, { "phaserFeedback", 0.6f }, { "phaserCentre", 900.0f }, { "attack", 0.003f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f } } },

        //======================================================================== ROBOTIQUE
        { "Voice Box", "ROBOTIQUE", {
            { "lfoDepth", 0.6f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.02f }, { "tableA", 1.0f },
            { "position", 0.3f }, { "fmAmount", 0.2f }, { "oscBPos", 0.2f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "lfoShape", 1.0f }, { "lfoRate", 1.2f },
            { "chorusMix", 0.3f }, { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f },
            { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f } } },

        { "Modem", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "oscBPos", 0.3f },
            { "fmAmount", 0.6f }, { "oscBOct", 1.0f }, { "oscBTune", 7.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "lfo3Shape", 3.0f }, { "lfo3Rate", 8.0f },
            { "attack", 0.001f }, { "decay", 0.12f }, { "sustain", 0.0f }, { "release", 0.1f }, { "delayMode", 1.0f },
            { "delaySync", 4.0f }, { "delayMix", 0.3f }, { "delayFeedback", 0.45f }, { "delayLP", 5000.0f },
            { "mx1Src", 7.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 1.0f }, { "mx2Src", 14.0f }, { "mx2Dst", 16.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        { "Sync Droid", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "unison", 2.0f },
            { "detune", 8.0f }, { "warpModeA", 3.0f }, { "warpAmtA", 0.5f }, { "modDecay", 0.3f },
            { "filterType", 1.0f }, { "cutoff", 3500.0f }, { "resonance", 1.4f }, { "distType", 1.0f },
            { "distAmt", 0.2f }, { "attack", 0.002f }, { "decay", 0.25f }, { "sustain", 0.8f }, { "release", 0.15f },
            { "mx1Src", 2.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 14.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        { "Arp Droid", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "warpModeA", 9.0f },
            { "warpAmtA", 0.3f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 4500.0f },
            { "attack", 0.001f }, { "decay", 0.18f }, { "sustain", 0.0f }, { "release", 0.12f }, { "delayMode", 1.0f },
            { "delaySync", 3.0f }, { "delayMix", 0.35f }, { "delayFeedback", 0.5f }, { "delayLP", 5000.0f },
            { "chorusMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.4f } } },

        { "Alien Code", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.4f }, { "oscBPos", 0.5f },
            { "fmAmount", 0.3f }, { "oscBOct", 1.0f }, { "unison", 1.0f }, { "filterType", 3.0f },
            { "cutoff", 2500.0f }, { "resonance", 3.0f }, { "attack", 0.001f }, { "decay", 0.15f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "delayMix", 0.25f }, { "delaySync", 2.0f },
            { "delayFeedback", 0.4f }, { "mx1Src", 7.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.6f }, { "mx2Src", 7.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 7.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 9.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 10.0f }, { "mx5Dst", 3.0f },
            { "mx5Amt", 0.4f } } },

        { "Servo Glide", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.3f }, { "tableA", 2.0f },
            { "position", 0.4f }, { "warpModeA", 8.0f }, { "warpAmtA", 0.5f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 2000.0f }, { "resonance", 2.0f }, { "lfoRate", 5.0f },
            { "attack", 0.01f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.2f }, { "mx1Src", 1.0f },
            { "mx1Dst", 14.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 5.0f },
            { "mx4Amt", 0.1f } } },

        { "Circuit Bend", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.5f },
            { "warpModeA", 7.0f }, { "warpAmtA", 0.6f }, { "distType", 3.0f }, { "distAmt", 0.5f }, { "unison", 2.0f },
            { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 1.8f },
            { "lfoRate", 2.0f }, { "attack", 0.002f }, { "decay", 0.3f }, { "sustain", 0.5f }, { "release", 0.2f },
            { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f },
            { "mx4Dst", 14.0f }, { "mx4Amt", 0.5f } } },

        { "Talk Machine", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.8f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 1.0f },
            { "position", 0.1f }, { "modDecay", 0.35f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "delayMix", 0.2f }, { "delaySync", 2.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 11.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.4f } } },

        { "Sequence Toy", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 1.0f },
            { "filterType", 5.0f }, { "cutoff", 600.0f }, { "resonance", 4.0f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.15f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.2f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "delayMix", 0.25f }, { "delaySync", 4.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f } } },

        { "Ring Talker", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 1.0f },
            { "position", 0.5f }, { "ringAmount", 0.5f }, { "oscBTune", 12.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "lfoRate", 3.0f }, { "attack", 0.005f },
            { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.2f }, { "mx1Src", 1.0f }, { "mx1Dst", 17.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f } } },

        { "Data Burst", "ROBOTIQUE", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.6f },
            { "warpModeA", 6.0f }, { "warpAmtA", 0.5f }, { "distType", 3.0f }, { "distAmt", 0.3f }, { "unison", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 800.0f }, { "attack", 0.001f }, { "decay", 0.1f }, { "sustain", 0.0f },
            { "release", 0.08f }, { "delayMode", 1.0f }, { "delaySync", 5.0f }, { "delayMix", 0.3f },
            { "delayFeedback", 0.55f }, { "delayHP", 300.0f }, { "mx1Src", 7.0f }, { "mx1Dst", 1.0f },
            { "mx1Amt", 0.8f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        //======================================================================== MALSAIN
        { "Rotten Choir", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.6f }, { "unison", 7.0f },
            { "detune", 60.0f }, { "spread", 1.0f }, { "filterType", 4.0f }, { "cutoff", 900.0f },
            { "resonance", 3.0f }, { "distType", 4.0f }, { "distAmt", 0.3f }, { "attack", 1.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.45f }, { "reverbSize", 0.9f },
            { "lfo3Rate", 0.12f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.6f },
            { "mx4Src", 11.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f } } },

        { "Sick Detune", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "oscBLevel", 0.8f },
            { "oscBPos", 0.5f }, { "oscBTune", 23.0f }, { "unison", 2.0f }, { "detune", 20.0f },
            { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "attack", 0.8f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 2.0f }, { "lfoRate", 0.4f }, { "chorusMix", 0.4f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f },
            { "mx1Amt", 0.05f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.6f }, { "mx4Src", 11.0f }, { "mx4Dst", 7.0f }, { "mx4Amt", 0.4f } } },

        { "Bad Signal", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "noiseLevel", 0.35f }, { "noiseType", 2.0f },
            { "noiseDecay", 5.0f }, { "noiseTone", 0.7f }, { "position", 0.5f }, { "filterType", 3.0f },
            { "cutoff", 1500.0f }, { "resonance", 5.0f }, { "distType", 3.0f }, { "distAmt", 0.35f },
            { "attack", 0.4f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 1.5f }, { "lfo2Rate", 3.0f },
            { "lfo2Shape", 3.0f }, { "delayMix", 0.25f }, { "delayFeedback", 0.5f }, { "delayMode", 1.0f },
            { "delaySync", 2.0f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.35f }, { "mx2Src", 7.0f },
            { "mx2Dst", 4.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "mx5Src", 11.0f }, { "mx5Dst", 19.0f },
            { "mx5Amt", 0.5f } } },

        { "Bleed Pad", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.45f }, { "unison", 5.0f },
            { "detune", 25.0f }, { "spread", 0.9f }, { "subLevel", 0.3f }, { "filterType", 1.0f },
            { "cutoff", 500.0f }, { "resonance", 2.0f }, { "distType", 2.0f }, { "distAmt", 0.4f }, { "attack", 2.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.4f }, { "lfo3Rate", 0.1f },
            { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 14.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.3f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        { "Hospital", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBTune", -17.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.15f }, { "ringAmount", 0.2f },
            { "unison", 2.0f }, { "detune", 15.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 1.2f },
            { "decay", 1.0f }, { "sustain", 0.9f }, { "release", 2.5f }, { "lfoRate", 0.3f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.85f }, { "mx1Src", 1.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.2f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f }, { "mx3Dst", 17.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 11.0f }, { "mx4Dst", 16.0f }, { "mx4Amt", 0.4f } } },

        { "Corrosion", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.5f }, { "unison", 5.0f },
            { "detune", 30.0f }, { "spread", 0.8f }, { "distType", 2.0f }, { "distAmt", 0.7f }, { "filterType", 1.0f },
            { "cutoff", 1800.0f }, { "resonance", 1.5f }, { "attack", 2.5f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 3.5f }, { "reverbMix", 0.35f }, { "lfo4Rate", 0.08f }, { "mx1Src", 15.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 15.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.25f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 10.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.4f }, { "mx5Src", 11.0f }, { "mx5Dst", 1.0f }, { "mx5Amt", 0.5f } } },

        { "Grind Pad", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 18.0f },
            { "distType", 3.0f }, { "distAmt", 0.5f }, { "distMix", 0.8f }, { "filterType", 0.0f },
            { "cutoff", 2500.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "reverbMix", 0.3f }, { "chorusMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f },
            { "mx3Amt", 0.5f } } },

        { "Insect Swarm", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "unison", 7.0f },
            { "detune", 90.0f }, { "spread", 1.0f }, { "filterType", 2.0f }, { "cutoff", 1800.0f },
            { "resonance", 2.0f }, { "lfoRate", 7.0f }, { "lfoShape", 1.0f }, { "attack", 0.8f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.5f }, { "reverbMix", 0.3f }, { "mx1Src", 1.0f }, { "mx1Dst", 1.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 10.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.5f } } },

        { "Sub Rot", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.1f }, { "position", 0.0f },
            { "subLevel", 0.5f }, { "oscBPos", 0.0f }, { "oscBTune", -9.0f }, { "oscBLevel", 0.5f },
            { "ringAmount", 0.25f }, { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 500.0f },
            { "resonance", 1.4f }, { "distType", 4.0f }, { "distAmt", 0.4f }, { "attack", 0.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.2f }, { "lfoRate", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Vein Pulse", "MALSAIN", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "unison", 5.0f }, { "detune", 22.0f },
            { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.8f }, { "lfoShape", 1.0f },
            { "lfoRate", 0.6f }, { "attack", 1.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "reverbMix", 0.4f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 1.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.25f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f },
            { "mx4Src", 10.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f } } },

        //======================================================================== NAPPES
        { "Black Glass", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.3f }, { "unison", 5.0f },
            { "detune", 18.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "attack", 1.2f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f }, { "chorusMix", 0.3f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "reverbHP", 200.0f }, { "lfo3Rate", 0.1f }, { "mx1Src", 14.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.2f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 1.0f },
            { "mx4Amt", 0.5f } } },

        { "Cold Machine", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 6.0f }, { "detune", 14.0f },
            { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 1400.0f }, { "resonance", 1.2f },
            { "attack", 1.8f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "chorusMix", 0.3f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "lfo3Rate", 0.08f }, { "mx1Src", 14.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f } } },

        { "Iron Pad", "NAPPES", {
            { "lfoDepth", 0.3f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.35f }, { "unison", 5.0f },
            { "detune", 20.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.5f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.45f },
            { "reverbSize", 0.85f }, { "lfoRate", 0.15f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Dust Choir", "NAPPES", {
            { "lfoDepth", 0.4f }, { "modAmount", 0.0f }, { "tableA", 1.0f }, { "position", 0.5f }, { "unison", 6.0f },
            { "detune", 24.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5000.0f }, { "attack", 2.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.5f }, { "chorusMix", 0.4f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.9f }, { "lfoRate", 0.12f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f },
            { "mx3Amt", 0.5f } } },

        { "Void Drift", "NAPPES", {
            { "lfoDepth", 0.5f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.4f },
            { "unison", 7.0f }, { "detune", 35.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 600.0f },
            { "resonance", 1.8f }, { "lfoRate", 0.06f }, { "attack", 4.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 4.5f }, { "reverbMix", 0.6f }, { "reverbSize", 0.95f }, { "lfo3Rate", 0.05f },
            { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f } } },

        { "Glacier", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 12.0f },
            { "filterType", 5.0f }, { "cutoff", 300.0f }, { "resonance", 3.0f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.5f }, { "reverbMix", 0.55f }, { "reverbSize", 0.95f },
            { "reverbHP", 300.0f }, { "lfo3Rate", 0.07f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f } } },

        { "Industrial Air", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.5f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.6f }, { "subLevel", 0.0f },
            { "unison", 1.0f }, { "filterType", 3.0f }, { "cutoff", 1400.0f }, { "resonance", 3.5f },
            { "lfo2Rate", 0.1f }, { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f } } },

        { "Dark Ether", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.4f },
            { "oscBLevel", 0.25f }, { "oscBOct", 2.0f }, { "oscBPos", 0.3f }, { "unison", 5.0f }, { "detune", 15.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 4.0f }, { "chorusMix", 0.3f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.92f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f } } },

        { "Phase Cloud", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.45f }, { "unison", 5.0f },
            { "detune", 16.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "attack", 1.5f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "phaserMix", 0.6f },
            { "phaserRate", 0.25f }, { "phaserFeedback", 0.55f }, { "reverbMix", 0.4f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f } } },

        { "Slow Motion", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.6f }, { "tableA", 5.0f }, { "position", 0.2f }, { "unison", 4.0f },
            { "detune", 14.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "attack", 4.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "modAttack", 3.0f }, { "modDecay", 4.0f },
            { "modSustain", 0.4f }, { "reverbMix", 0.45f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Night Shift", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.4f }, { "subLevel", 0.3f },
            { "unison", 5.0f }, { "detune", 18.0f }, { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 2.0f }, { "lfo4Rate", 0.1f }, { "attack", 1.8f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 3.0f }, { "reverbMix", 0.4f }, { "mx1Src", 15.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.45f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f },
            { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.4f } } },

        { "Warm Machine", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 5.0f }, { "detune", 22.0f },
            { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 1800.0f }, { "drive", 0.15f }, { "attack", 1.0f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.0f }, { "chorusMix", 0.4f }, { "reverbMix", 0.3f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.4f } } },

        //======================================================================== TEXTURES
        { "Wind Tunnel", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.7f },
            { "noiseType", 0.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.8f }, { "filterType", 3.0f },
            { "cutoff", 900.0f }, { "resonance", 3.0f }, { "lfo2Rate", 0.15f }, { "attack", 2.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.4f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.55f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f } } },

        { "Steel Rain", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 11.0f }, { "ringAmount", 0.7f }, { "unison", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 900.0f }, { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f },
            { "release", 0.4f }, { "delayMode", 1.0f }, { "delaySync", 5.0f }, { "delayMix", 0.4f },
            { "delayFeedback", 0.6f }, { "delayHP", 400.0f }, { "delayLP", 6000.0f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "mx1Src", 7.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 7.0f },
            { "mx2Dst", 17.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 17.0f }, { "mx4Amt", 0.5f } } },

        { "Rusted Drone", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "subLevel", 0.5f },
            { "unison", 7.0f }, { "detune", 45.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 380.0f },
            { "resonance", 2.5f }, { "lfo3Rate", 0.05f }, { "distType", 0.0f }, { "distAmt", 0.3f },
            { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "reverbMix", 0.6f },
            { "reverbSize", 0.95f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Radio Ghost", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.2f }, { "noiseLevel", 0.35f },
            { "noiseType", 1.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.5f }, { "filterType", 5.0f },
            { "cutoff", 420.0f }, { "resonance", 4.0f }, { "lfo2Rate", 0.2f }, { "attack", 1.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.5f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.45f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f }, { "mx4Src", 11.0f }, { "mx4Dst", 19.0f }, { "mx4Amt", 0.4f } } },

        { "Metal Breath", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 6.0f }, { "position", 0.5f }, { "oscBPos", 0.7f },
            { "oscBTune", 19.0f }, { "oscBLevel", 0.5f }, { "unison", 3.0f }, { "detune", 30.0f },
            { "filterType", 3.0f }, { "cutoff", 1800.0f }, { "resonance", 4.0f }, { "lfoRate", 0.3f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 1.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f },
            { "mx4Dst", 17.0f }, { "mx4Amt", 0.4f } } },

        { "Static Field", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "noiseLevel", 0.6f },
            { "noiseType", 2.0f }, { "noiseDecay", 5.0f }, { "noiseTone", 0.8f }, { "filterType", 2.0f },
            { "cutoff", 1500.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "delayMix", 0.35f }, { "delayTime", 410.0f }, { "delayFeedback", 0.55f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.4f } } },

        { "Pulse Machine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.43f }, { "unison", 3.0f },
            { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 2.0f },
            { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "distAmt", 0.3f }, { "attack", 0.01f }, { "decay", 0.3f },
            { "sustain", 1.0f }, { "release", 0.2f }, { "delayMix", 0.25f }, { "reverbMix", 0.2f }, { "mx1Src", 1.0f },
            { "mx1Dst", 9.0f }, { "mx1Amt", 0.9f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Chaos Garden", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 7.0f }, { "position", 0.5f }, { "oscBPos", 0.4f },
            { "fmAmount", 0.25f }, { "oscBOct", 1.0f }, { "unison", 5.0f }, { "detune", 40.0f }, { "spread", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "lfo3Rate", 0.2f }, { "lfo4Rate", 0.13f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "delayMode", 1.0f }, { "mx1Src", 14.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 15.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.3f },
            { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 16.0f },
            { "mx4Amt", 0.5f } } },

        { "Deep Sea", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "subLevel", 0.6f },
            { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 1.0f }, { "cutoff", 300.0f },
            { "resonance", 2.5f }, { "lfo3Rate", 0.07f }, { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 4.0f }, { "reverbMix", 0.65f }, { "reverbSize", 0.95f }, { "reverbLP", 3000.0f },
            { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 4.0f }, { "mx3Amt", 0.4f } } },

        { "Granular Dust", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 3.0f }, { "position", 0.6f },
            { "warpModeA", 7.0f }, { "warpAmtA", 0.5f }, { "unison", 6.0f }, { "detune", 50.0f }, { "spread", 1.0f },
            { "filterType", 2.0f }, { "cutoff", 1200.0f }, { "lfoShape", 3.0f }, { "lfoRate", 9.0f },
            { "distType", 3.0f }, { "distAmt", 0.25f }, { "attack", 1.0f }, { "decay", 1.0f }, { "sustain", 1.0f },
            { "release", 2.0f }, { "reverbMix", 0.45f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.4f },
            { "mx2Src", 7.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f },
            { "mx3Amt", 0.5f }, { "mx4Src", 10.0f }, { "mx4Dst", 14.0f }, { "mx4Amt", 0.5f } } },

        //======================================================================== LEADS
        { "Acid Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.07f }, { "position", 0.6f },
            { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 1200.0f }, { "resonance", 4.5f },
            { "fltEnvAmount", 0.5f }, { "fltDecay", 0.25f }, { "fltSustain", 0.2f }, { "attack", 0.002f },
            { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.15f }, { "delayMix", 0.2f }, { "delaySync", 2.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f },
            { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        { "Saw Stack", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.6f },
            { "unison", 7.0f }, { "detune", 24.0f }, { "spread", 0.9f }, { "filterType", 1.0f }, { "cutoff", 4500.0f },
            { "resonance", 1.0f }, { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f },
            { "delayMix", 0.2f }, { "reverbMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Vocal Lead", "LEADS", {
            { "lfoDepth", 0.15f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 1.0f },
            { "position", 0.4f }, { "unison", 3.0f }, { "detune", 8.0f }, { "filterType", 0.0f },
            { "cutoff", 6000.0f }, { "lfoRate", 5.5f }, { "attack", 0.03f }, { "decay", 0.3f }, { "sustain", 0.8f },
            { "release", 0.3f }, { "chorusMix", 0.25f }, { "reverbMix", 0.25f }, { "mx1Src", 6.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f } } },

        { "FM Glass Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.0f },
            { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "fmAmount", 0.3f }, { "unison", 2.0f }, { "detune", 6.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.002f }, { "decay", 0.5f },
            { "sustain", 0.5f }, { "release", 0.4f }, { "reverbMix", 0.3f }, { "modDecay", 0.4f },
            { "modSustain", 0.0f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f } } },

        { "Sync Scream", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.5f },
            { "warpModeA", 3.0f }, { "warpAmtA", 0.3f }, { "unison", 2.0f }, { "detune", 10.0f },
            { "filterType", 1.0f }, { "cutoff", 5000.0f }, { "resonance", 1.5f }, { "distType", 1.0f },
            { "distAmt", 0.25f }, { "lfoRate", 5.0f }, { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.2f }, { "mx1Src", 6.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 1.0f },
            { "mx2Dst", 14.0f }, { "mx2Amt", 0.1f }, { "mx3Src", 9.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f },
            { "mx5Amt", 0.5f } } },

        { "Hollow Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 2.0f },
            { "position", 0.5f }, { "warpModeA", 8.0f }, { "warpAmtA", 0.2f }, { "unison", 3.0f }, { "detune", 12.0f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "lfoRate", 0.4f }, { "attack", 0.01f }, { "decay", 0.3f },
            { "sustain", 0.8f }, { "release", 0.25f }, { "chorusMix", 0.3f }, { "mx1Src", 1.0f }, { "mx1Dst", 14.0f },
            { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f } } },

        { "Pulse Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 2.0f },
            { "position", 0.3f }, { "unison", 2.0f }, { "detune", 10.0f }, { "filterType", 1.0f },
            { "cutoff", 3500.0f }, { "resonance", 1.2f }, { "attack", 0.003f }, { "decay", 0.25f },
            { "sustain", 0.7f }, { "release", 0.2f }, { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 2.0f },
            { "mx1Dst", 1.0f }, { "mx1Amt", 0.3f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f } } },

        { "Ring Lead", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.5f },
            { "oscBOct", 0.0f }, { "oscBTune", 15.0f }, { "ringAmount", 0.35f }, { "unison", 2.0f },
            { "detune", 8.0f }, { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "attack", 0.004f }, { "decay", 0.3f },
            { "sustain", 0.8f }, { "release", 0.25f }, { "delayMix", 0.2f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f },
            { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f } } },

        //======================================================================== STABS
        { "Dub Chord Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 4.0f }, { "detune", 12.0f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.25f }, { "fltSustain", 0.0f }, { "attack", 0.002f }, { "decay", 0.35f },
            { "sustain", 0.0f }, { "release", 0.2f }, { "delayMode", 1.0f }, { "delaySync", 3.0f },
            { "delayMix", 0.4f }, { "delayFeedback", 0.55f }, { "delayLP", 3500.0f }, { "reverbMix", 0.3f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 6.0f }, { "mx3Amt", 0.5f } } },

        { "Techno Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 3.0f }, { "detune", 14.0f },
            { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 2.0f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.12f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.18f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "distType", 0.0f }, { "distAmt", 0.25f },
            { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.5f } } },

        { "Rave Stab", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 7.0f }, { "detune", 28.0f },
            { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 2500.0f }, { "fltEnvAmount", 0.4f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.1f }, { "attack", 0.002f }, { "decay", 0.3f }, { "sustain", 0.0f },
            { "release", 0.2f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.4f } } },

        { "Metal Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 9.0f }, { "ringAmount", 0.4f }, { "fmAmount", 0.2f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "attack", 0.001f }, { "decay", 0.4f },
            { "sustain", 0.0f }, { "release", 0.3f }, { "reverbMix", 0.3f }, { "delayMix", 0.2f },
            { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.5f } } },

        { "FM Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "fmAmount", 0.4f }, { "modDecay", 0.15f }, { "modSustain", 0.0f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "attack", 0.001f }, { "decay", 0.35f },
            { "sustain", 0.0f }, { "release", 0.25f }, { "reverbMix", 0.2f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f },
            { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f }, { "mx3Src", 10.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f } } },

        { "Hollow Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.5f }, { "unison", 2.0f }, { "detune", 8.0f },
            { "filterType", 4.0f }, { "cutoff", 1200.0f }, { "resonance", 2.5f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.3f }, { "sustain", 0.0f },
            { "release", 0.2f }, { "delayMix", 0.25f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.5f } } },

        { "Glass Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.5f }, { "unison", 2.0f },
            { "detune", 6.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 0.5f },
            { "sustain", 0.0f }, { "release", 0.4f }, { "chorusMix", 0.25f }, { "reverbMix", 0.35f },
            { "reverbSize", 0.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f } } },

        { "Bounce Pluck", "STABS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.5f },
            { "subLevel", 0.3f }, { "pitchEnvAmt", 7.0f }, { "pitchEnvDecay", 0.05f }, { "filterType", 1.0f },
            { "cutoff", 1200.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.5f }, { "fltDecay", 0.15f },
            { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 0.0f }, { "release", 0.1f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 20.0f },
            { "mx2Amt", 0.5f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f } } },

        //======================================================================== CLOCHES
        { "FM Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", 6.0f }, { "fmAmount", 0.35f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 12000.0f }, { "attack", 0.001f }, { "decay", 1.8f },
            { "sustain", 0.0f }, { "release", 1.5f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "mx1Src", 9.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.4f } } },

        { "Iron Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBTune", -13.0f }, { "ringAmount", 0.6f }, { "fmAmount", 0.15f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 10000.0f }, { "attack", 0.001f },
            { "decay", 2.0f }, { "sustain", 0.0f }, { "release", 1.8f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "delayMix", 0.2f }, { "delaySync", 2.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f } } },

        { "Dark Chime", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 6.0f }, { "position", 0.4f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.001f }, { "decay", 2.5f },
            { "sustain", 0.0f }, { "release", 2.0f }, { "reverbMix", 0.55f }, { "reverbSize", 0.9f },
            { "reverbHP", 250.0f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f } } },

        { "Tube Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 5.0f }, { "position", 0.3f }, { "oscBPos", 0.3f },
            { "oscBLevel", 0.3f }, { "oscBOct", 1.0f }, { "unison", 1.0f }, { "filterType", 0.0f },
            { "cutoff", 8000.0f }, { "attack", 0.001f }, { "decay", 1.5f }, { "sustain", 0.0f }, { "release", 1.5f },
            { "reverbMix", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 7.0f }, { "mx2Amt", 0.5f } } },

        { "Cold Gong", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBTune", -19.0f }, { "ringAmount", 0.5f }, { "noiseLevel", 0.12f }, { "noiseType", 1.0f },
            { "noiseDecay", 0.6f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 5000.0f },
            { "attack", 0.003f }, { "decay", 3.0f }, { "sustain", 0.0f }, { "release", 3.0f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "mx1Src", 9.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 19.0f }, { "mx2Amt", 0.5f } } },

        { "Robot Bell", "CLOCHES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.2f }, { "oscBPos", 0.0f },
            { "oscBOct", 2.0f }, { "fmAmount", 0.5f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.3f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "attack", 0.001f }, { "decay", 0.8f },
            { "sustain", 0.0f }, { "release", 0.6f }, { "delayMode", 1.0f }, { "delaySync", 3.0f },
            { "delayMix", 0.3f }, { "delayFeedback", 0.5f }, { "delayLP", 6000.0f }, { "mx1Src", 9.0f },
            { "mx1Dst", 16.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 14.0f }, { "mx2Amt", 0.5f } } },

        //======================================================================== CORDES
        { "Dark Strings", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.55f }, { "unison", 6.0f },
            { "detune", 16.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "attack", 0.9f },
            { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 1.5f }, { "chorusMix", 0.4f }, { "reverbMix", 0.35f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 6.0f },
            { "mx2Amt", 0.5f } } },

        { "Analog Strings", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.6f }, { "unison", 5.0f }, { "detune", 20.0f },
            { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "attack", 0.5f }, { "decay", 1.0f },
            { "sustain", 1.0f }, { "release", 1.2f }, { "chorusMix", 0.5f }, { "phaserMix", 0.25f },
            { "phaserRate", 0.3f }, { "reverbMix", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 10.0f }, { "mx2Dst", 6.0f }, { "mx2Amt", 0.5f } } },

        //======================================================================== CLAVIERS
        { "Digital EP", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 0.0f }, { "fmAmount", 0.2f }, { "modDecay", 0.3f }, { "modSustain", 0.0f },
            { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "velFilter", 0.4f },
            { "attack", 0.002f }, { "decay", 1.2f }, { "sustain", 0.0f }, { "release", 0.5f }, { "chorusMix", 0.3f },
            { "reverbMix", 0.25f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 4.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.5f },
            { "mx4Src", 10.0f }, { "mx4Dst", 3.0f }, { "mx4Amt", 0.4f } } },

        { "Organ Drone", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.5f }, { "unison", 2.0f },
            { "detune", 6.0f }, { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.02f }, { "decay", 0.3f },
            { "sustain", 1.0f }, { "release", 0.2f }, { "chorusMix", 0.35f }, { "lfoRate", 6.0f },
            { "reverbMix", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f } } },

        { "Clav Funk", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 2.0f }, { "position", 0.3f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 1800.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.5f },
            { "fltDecay", 0.12f }, { "fltSustain", 0.0f }, { "attack", 0.001f }, { "decay", 0.25f },
            { "sustain", 0.0f }, { "release", 0.1f }, { "velFilter", 0.5f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f } } },

        { "Dark Keys", "CLAVIERS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "position", 0.0f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.25f }, { "fmAmount", 0.1f }, { "unison", 2.0f }, { "detune", 7.0f },
            { "filterType", 1.0f }, { "cutoff", 1800.0f }, { "attack", 0.003f }, { "decay", 0.8f },
            { "sustain", 0.2f }, { "release", 0.6f }, { "chorusMix", 0.3f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 10.0f },
            { "mx2Dst", 16.0f }, { "mx2Amt", 0.5f } } }
    };

    return presets;
}
