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
//   pitchEnvAmt : demi-tons de chute (kicks) ; pour les KICKS, jouer des notes graves (C1 a F1)
//
// Rappel : "lfoDepth" et "modAmount" bougent la position de la wavetable par défaut ; on les met à 0
// quand le preset ne doit pas bouger tout seul.
// Les presets sont rangés par catégorie : l'ordre des catégories dans cette liste est celui du menu.
//==============================================================================
const std::vector<FactoryPreset>& factoryPresets()
{
    static const std::vector<FactoryPreset> presets =
    {
        { "Init", "INIT", {} },

        //======================================================================== KICKS (jouer des notes graves : C1 a F1)
        { "Techno Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 14000.0f },
            { "phaseMode", 1.0f }, { "oscPhase", 0.0f }, { "pitchEnvAmt", 36.0f }, { "pitchEnvDecay", 0.045f },
            { "attack", 0.001f }, { "decay", 0.45f }, { "sustain", 0.0f }, { "release", 0.08f },
            { "distType", 0.0f }, { "distAmt", 0.25f }, { "noiseLevel", 0.12f }, { "noiseTone", 0.7f }, { "noiseDecay", 0.012f } } },

        { "Hard Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 1.0f }, { "cutoff", 8000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 48.0f }, { "pitchEnvDecay", 0.035f },
            { "attack", 0.001f }, { "decay", 0.38f }, { "sustain", 0.0f }, { "release", 0.06f },
            { "distType", 1.0f }, { "distAmt", 0.55f }, { "noiseLevel", 0.2f }, { "noiseTone", 0.8f }, { "noiseDecay", 0.015f } } },

        { "Rumble Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 700.0f }, { "resonance", 1.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 30.0f }, { "pitchEnvDecay", 0.06f },
            { "attack", 0.001f }, { "decay", 0.9f }, { "sustain", 0.0f }, { "release", 0.2f },
            { "distType", 0.0f }, { "distAmt", 0.35f }, { "reverbMix", 0.35f }, { "reverbSize", 0.8f }, { "reverbDamp", 0.7f } } },

        { "Industrial Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 1.0f }, { "cutoff", 9000.0f }, { "drive", 0.3f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 40.0f }, { "pitchEnvDecay", 0.05f },
            { "attack", 0.001f }, { "decay", 0.3f }, { "sustain", 0.0f }, { "release", 0.06f },
            { "distType", 2.0f }, { "distAmt", 0.5f }, { "distMix", 0.8f },
            { "noiseLevel", 0.25f }, { "noiseType", 2.0f }, { "noiseTone", 0.8f }, { "noiseDecay", 0.03f } } },

        { "Punch Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 12000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 30.0f }, { "pitchEnvDecay", 0.03f },
            { "attack", 0.001f }, { "decay", 0.22f }, { "sustain", 0.0f }, { "release", 0.05f },
            { "distType", 0.0f }, { "distAmt", 0.4f }, { "noiseLevel", 0.2f }, { "noiseDecay", 0.01f } } },

        { "Distorted Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 1.0f }, { "cutoff", 5000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 42.0f }, { "pitchEnvDecay", 0.07f },
            { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f }, { "release", 0.08f },
            { "distType", 4.0f }, { "distAmt", 0.8f }, { "drive", 0.2f } } },

        { "808 Boom", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 6000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 14.0f }, { "pitchEnvDecay", 0.12f },
            { "attack", 0.002f }, { "decay", 1.4f }, { "sustain", 0.0f }, { "release", 0.3f },
            { "distType", 0.0f }, { "distAmt", 0.15f } } },

        { "Crushed Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 12000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 36.0f }, { "pitchEnvDecay", 0.05f },
            { "attack", 0.001f }, { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.06f },
            { "distType", 3.0f }, { "distAmt", 0.45f }, { "distMix", 0.7f } } },

        { "Click Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 0.0f }, { "cutoff", 16000.0f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 40.0f }, { "pitchEnvDecay", 0.03f },
            { "attack", 0.001f }, { "decay", 0.25f }, { "sustain", 0.0f }, { "release", 0.05f },
            { "noiseLevel", 0.45f }, { "noiseTone", 1.0f }, { "noiseDecay", 0.006f }, { "distAmt", 0.15f } } },

        { "Gabber Kick", "KICKS", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "filterType", 1.0f }, { "cutoff", 6000.0f }, { "drive", 0.6f },
            { "phaseMode", 1.0f }, { "pitchEnvAmt", 48.0f }, { "pitchEnvDecay", 0.1f },
            { "attack", 0.001f }, { "decay", 0.55f }, { "sustain", 0.0f }, { "release", 0.1f },
            { "distType", 1.0f }, { "distAmt", 0.9f }, { "reverbMix", 0.1f } } },

        //======================================================================== BASSES
        { "Sub Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 0.9f }, { "drive", 0.15f },
            { "fltEnvAmount", 0.3f }, { "fltAttack", 0.003f }, { "fltDecay", 0.25f }, { "fltSustain", 0.4f },
            { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },

        { "Wobble Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 10.0f },
            { "spread", 0.4f }, { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 450.0f },
            { "resonance", 4.5f }, { "drive", 0.35f }, { "lfoShape", 1.0f }, { "lfoRate", 2.2f }, { "lfoDepth", 0.0f },
            { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.55f },
            { "attack", 0.005f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f } } },

        { "Reese Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "unison", 5.0f }, { "detune", 28.0f },
            { "spread", 0.5f }, { "subLevel", 0.3f }, { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.2f },
            { "drive", 0.3f }, { "lfoShape", 0.0f }, { "lfoRate", 0.3f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f },
            { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.2f }, { "chorusMix", 0.2f } } },

        { "Acid Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 300.0f }, { "resonance", 6.0f }, { "drive", 0.5f },
            { "fltEnvAmount", 0.9f }, { "fltAttack", 0.002f }, { "fltDecay", 0.18f }, { "fltSustain", 0.0f },
            { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 0.9f }, { "release", 0.08f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velFilter", 0.5f } } },

        { "Growl Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 0.0f },
            { "fmAmount", 0.35f }, { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 1200.0f }, { "resonance", 2.0f },
            { "drive", 0.5f }, { "lfo2Shape", 1.0f }, { "lfo2Rate", 3.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 8.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.35f }, { "mx2Src", 8.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.3f },
            { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.1f } } },

        { "Pluck Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.02f }, { "position", 0.43f }, { "unison", 1.0f }, { "subLevel", 0.3f },
            { "filterType", 1.0f }, { "cutoff", 250.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.8f },
            { "fltAttack", 0.002f }, { "fltDecay", 0.2f }, { "fltSustain", 0.0f },
            { "attack", 0.002f }, { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.12f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velFilter", 0.5f } } },

        { "Dub Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.09f }, { "position", 0.0f }, { "subLevel", 0.3f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 400.0f }, { "resonance", 1.0f }, { "drive", 0.2f },
            { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.25f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "delayMix", 0.2f }, { "delayTime", 375.0f }, { "delayFeedback", 0.5f }, { "reverbMix", 0.12f } } },

        { "Rubber Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "warpModeA", 1.0f }, { "warpAmtA", 0.5f },
            { "unison", 1.0f }, { "subLevel", 0.25f }, { "filterType", 0.0f }, { "cutoff", 500.0f }, { "resonance", 2.5f },
            { "fltEnvAmount", 0.6f }, { "fltDecay", 0.2f }, { "fltSustain", 0.1f },
            { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.1f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },

        { "Distorted Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 8.0f },
            { "spread", 0.3f }, { "subLevel", 0.35f }, { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 1.5f },
            { "drive", 0.8f }, { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.1f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },

        { "Tempo Wobble", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 12.0f },
            { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 400.0f }, { "resonance", 4.0f }, { "drive", 0.3f },
            { "lfoShape", 2.0f }, { "lfoSync", 6.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.65f },
            { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f } } },

        { "Wheel Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 10.0f },
            { "subLevel", 0.4f }, { "filterType", 1.0f }, { "cutoff", 300.0f }, { "resonance", 2.0f },
            { "mx1Src", 6.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f }, { "mx2Src", 6.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.3f },
            { "attack", 0.004f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.12f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },


        { "Industrial Reese", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "unison", 5.0f }, { "detune", 40.0f }, { "spread", 0.6f },
            { "subLevel", 0.3f }, { "filterType", 1.0f }, { "cutoff", 600.0f }, { "resonance", 2.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "lfo2Shape", 0.0f }, { "lfo2Rate", 0.2f },
            { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f },
            { "distType", 1.0f }, { "distAmt", 0.45f }, { "distMix", 0.8f },
            { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.2f } } },

        { "Metal Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 0.0f }, { "oscBTune", 7.0f },
            { "fmAmount", 0.6f }, { "ringAmount", 0.3f }, { "unison", 1.0f }, { "filterType", 1.0f }, { "cutoff", 2500.0f }, { "resonance", 1.5f },
            { "distType", 2.0f }, { "distAmt", 0.4f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.12f } } },

        { "Rolling Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.0f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "spread", 0.3f },
            { "filterType", 1.0f }, { "cutoff", 200.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.8f },
            { "fltAttack", 0.002f }, { "fltDecay", 0.12f }, { "fltSustain", 0.1f },
            { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 0.3f }, { "release", 0.06f },
            { "distType", 0.0f }, { "distAmt", 0.3f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "subLevel", 0.3f } } },

        { "Acid Distorted", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.07f }, { "position", 0.43f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 280.0f }, { "resonance", 8.0f }, { "fltEnvAmount", 0.9f },
            { "fltAttack", 0.002f }, { "fltDecay", 0.18f }, { "fltSustain", 0.0f },
            { "attack", 0.002f }, { "decay", 0.2f }, { "sustain", 0.8f }, { "release", 0.08f },
            { "distType", 1.0f }, { "distAmt", 0.4f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f } } },

        { "Sub Crush", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.0f }, { "subLevel", 0.5f }, { "unison", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 1500.0f }, { "distType", 3.0f }, { "distAmt", 0.3f }, { "distMix", 0.4f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f } } },

        { "Dark Wobble", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 15.0f }, { "subLevel", 0.35f },
            { "filterType", 1.0f }, { "cutoff", 350.0f }, { "resonance", 5.0f }, { "lfoShape", 2.0f }, { "lfoSync", 5.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f },
            { "distType", 2.0f }, { "distAmt", 0.3f }, { "attack", 0.004f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.12f } } },

        { "Warp Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.43f }, { "warpModeA", 5.0f }, { "warpAmtA", 0.6f },
            { "unison", 2.0f }, { "detune", 10.0f }, { "subLevel", 0.3f }, { "filterType", 1.0f }, { "cutoff", 700.0f }, { "resonance", 1.5f },
            { "lfoShape", 0.0f }, { "lfoRate", 1.5f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.4f }, { "distType", 0.0f }, { "distAmt", 0.25f },
            { "attack", 0.004f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.1f } } },

        { "Quant Bass", "BASSES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "position", 0.43f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.5f },
            { "unison", 1.0f }, { "subLevel", 0.3f }, { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 2.0f },
            { "modAttack", 0.002f }, { "modDecay", 0.4f }, { "modSustain", 0.2f }, { "modAmount", 0.0f }, { "lfoDepth", 0.0f },
            { "mx1Src", 2.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.4f }, { "distType", 1.0f }, { "distAmt", 0.3f },
            { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 1.0f }, { "release", 0.1f } } },

        //======================================================================== LEADS
        { "Supersaw Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.07f }, { "position", 0.43f }, { "unison", 7.0f }, { "detune", 32.0f },
            { "spread", 0.9f }, { "oscBLevel", 0.5f }, { "oscBPos", 0.43f }, { "oscBOct", -1.0f }, { "oscBTune", 7.0f },
            { "cutoff", 6000.0f }, { "resonance", 1.0f }, { "filterType", 0.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.3f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "chorusMix", 0.2f }, { "delayMix", 0.22f }, { "delayTime", 380.0f }, { "delayFeedback", 0.4f },
            { "reverbMix", 0.2f }, { "reverbSize", 0.6f } } },

        { "Acid Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "unison", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 350.0f }, { "resonance", 7.5f }, { "drive", 0.4f },
            { "fltEnvAmount", 0.85f }, { "fltAttack", 0.002f }, { "fltDecay", 0.22f }, { "fltSustain", 0.05f },
            { "attack", 0.003f }, { "decay", 0.25f }, { "sustain", 0.8f }, { "release", 0.1f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "delayMix", 0.25f }, { "delayTime", 375.0f }, { "delayFeedback", 0.45f } } },

        { "Sync Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "warpModeA", 3.0f }, { "warpAmtA", 0.25f },
            { "unison", 3.0f }, { "detune", 12.0f }, { "spread", 0.6f }, { "cutoff", 7000.0f },
            { "lfoShape", 1.0f }, { "lfoRate", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.35f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f }, { "delayMix", 0.2f } } },

        { "Hoover Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.12f }, { "position", 0.43f }, { "warpModeA", 2.0f }, { "warpAmtA", 0.4f },
            { "unison", 7.0f }, { "detune", 45.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 3500.0f },
            { "resonance", 1.5f }, { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.3f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "chorusMix", 0.4f }, { "reverbMix", 0.25f } } },

        { "Soft Sine Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.1f }, { "position", 0.0f }, { "unison", 2.0f }, { "detune", 4.0f },
            { "spread", 0.3f }, { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "attack", 0.02f }, { "decay", 0.3f },
            { "sustain", 0.9f }, { "release", 0.35f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "delayMix", 0.3f }, { "delayTime", 400.0f }, { "delayFeedback", 0.4f }, { "reverbMix", 0.3f } } },

        { "PWM Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "tableA", 2.0f }, { "position", 0.4f }, { "unison", 2.0f }, { "detune", 9.0f },
            { "filterType", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.3f }, { "fltDecay", 0.3f },
            { "fltSustain", 0.4f }, { "lfoShape", 0.0f }, { "lfoRate", 1.5f }, { "lfoDepth", 0.25f }, { "modAmount", 0.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.25f }, { "chorusMix", 0.25f } } },

        { "Morph Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "tableA", 5.0f }, { "position", 0.1f }, { "unison", 3.0f }, { "detune", 10.0f },
            { "cutoff", 6000.0f }, { "modAttack", 0.01f }, { "modDecay", 0.7f }, { "modSustain", 0.3f }, { "modAmount", 0.0f },
            { "lfoDepth", 0.0f }, { "mx1Src", 2.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.6f },
            { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.3f }, { "delayMix", 0.2f }, { "reverbMix", 0.2f } } },

        { "Vocal Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.08f }, { "tableA", 1.0f }, { "position", 0.2f }, { "unison", 2.0f }, { "detune", 7.0f },
            { "cutoff", 8000.0f }, { "lfoShape", 0.0f }, { "lfoRate", 0.5f }, { "lfoDepth", 0.3f }, { "modAmount", 0.0f },
            { "attack", 0.03f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.3f }, { "reverbMix", 0.25f } } },

        { "Ring Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 8.0f },
            { "ringAmount", 0.35f }, { "unison", 2.0f }, { "detune", 8.0f }, { "cutoff", 5000.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f },
            { "delayMix", 0.2f } } },

        { "FM Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.35f },
            { "unison", 2.0f }, { "detune", 6.0f }, { "cutoff", 9000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "modAttack", 0.002f }, { "modDecay", 0.5f }, { "modSustain", 0.2f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f },
            { "attack", 0.003f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.25f }, { "delayMix", 0.18f } } },

        { "Mirror Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "warpModeA", 4.0f }, { "warpAmtA", 0.5f },
            { "unison", 3.0f }, { "detune", 14.0f }, { "cutoff", 6500.0f }, { "lfoShape", 0.0f }, { "lfoRate", 0.6f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 14.0f }, { "mx1Amt", 0.3f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.3f }, { "chorusMix", 0.2f }, { "reverbMix", 0.2f } } },

        { "Trance Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.43f }, { "unison", 7.0f }, { "detune", 25.0f }, { "spread", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 4500.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.3f }, { "fltDecay", 0.25f },
            { "fltSustain", 0.6f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.3f },
            { "delayMix", 0.3f }, { "delayTime", 280.0f }, { "delayFeedback", 0.45f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f } } },


        { "Screech Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 14.0f },
            { "filterType", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 4.0f }, { "distType", 2.0f }, { "distAmt", 0.5f }, { "distMix", 0.6f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.2f },
            { "delayMix", 0.2f } } },

        { "Industrial Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "warpModeA", 3.0f }, { "warpAmtA", 0.5f },
            { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.2f }, { "unison", 3.0f }, { "detune", 18.0f },
            { "filterType", 1.0f }, { "cutoff", 4500.0f }, { "resonance", 2.0f }, { "distType", 1.0f }, { "distAmt", 0.35f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.2f },
            { "delayMix", 0.2f }, { "reverbMix", 0.15f } } },

        { "Rave Lead", "LEADS", {
            { "mono", 1.0f }, { "glide", 0.04f }, { "position", 0.43f }, { "unison", 7.0f }, { "detune", 38.0f }, { "spread", 1.0f },
            { "filterType", 0.0f }, { "cutoff", 5500.0f }, { "distType", 0.0f }, { "distAmt", 0.35f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.25f },
            { "delayMix", 0.25f }, { "delayTime", 330.0f }, { "reverbMix", 0.25f } } },

        //======================================================================== PADS
        { "Warm Pad", "PADS", {
            { "position", 0.25f }, { "unison", 5.0f }, { "detune", 18.0f }, { "spread", 0.85f },
            { "oscBLevel", 0.45f }, { "oscBPos", 0.15f }, { "oscBTune", 6.0f },
            { "cutoff", 2200.0f }, { "resonance", 0.9f },
            { "attack", 1.2f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 1.8f },
            { "modAttack", 2.5f }, { "modDecay", 2.5f }, { "modSustain", 0.5f }, { "modAmount", 0.25f },
            { "lfoRate", 0.25f }, { "lfoDepth", 0.08f },
            { "chorusMix", 0.4f }, { "chorusRate", 0.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.85f }, { "reverbDamp", 0.55f } } },

        { "Vowel Choir", "PADS", {
            { "tableA", 1.0f }, { "position", 0.1f }, { "lfoShape", 1.0f }, { "lfoRate", 0.35f }, { "lfoDepth", 0.4f },
            { "modAmount", 0.0f }, { "unison", 4.0f }, { "detune", 12.0f }, { "spread", 0.7f }, { "cutoff", 7000.0f },
            { "attack", 0.35f }, { "decay", 0.5f }, { "sustain", 0.85f }, { "release", 0.9f },
            { "reverbMix", 0.35f }, { "reverbSize", 0.75f }, { "chorusMix", 0.25f } } },

        { "Dark Pad", "PADS", {
            { "position", 0.25f }, { "unison", 5.0f }, { "detune", 16.0f }, { "spread", 0.8f }, { "subLevel", 0.25f },
            { "filterType", 0.0f }, { "cutoff", 900.0f }, { "resonance", 1.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.8f }, { "decay", 1.0f }, { "sustain", 0.9f }, { "release", 2.5f },
            { "chorusMix", 0.35f }, { "reverbMix", 0.6f }, { "reverbSize", 0.9f }, { "reverbDamp", 0.7f } } },

        { "Glass Pad", "PADS", {
            { "tableA", 6.0f }, { "position", 0.3f }, { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.9f },
            { "cutoff", 9000.0f }, { "lfoShape", 0.0f }, { "lfoRate", 0.2f }, { "lfoDepth", 0.25f }, { "modAmount", 0.0f },
            { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 2.2f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.6f }, { "reverbSize", 0.9f } } },

        { "Evolving Pad", "PADS", {
            { "tableA", 3.0f }, { "position", 0.2f }, { "unison", 5.0f }, { "detune", 15.0f }, { "spread", 0.9f },
            { "lfoShape", 1.0f }, { "lfoRate", 0.12f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "lfo2Shape", 0.0f }, { "lfo2Rate", 0.08f }, { "mx1Src", 1.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 8.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.3f }, { "cutoff", 2500.0f }, { "resonance", 1.2f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 3.0f },
            { "chorusMix", 0.35f }, { "reverbMix", 0.55f }, { "reverbSize", 0.9f } } },

        { "Shimmer Pad", "PADS", {
            { "position", 0.15f }, { "unison", 4.0f }, { "detune", 14.0f }, { "spread", 0.9f },
            { "oscBPos", 0.15f }, { "oscBOct", 1.0f }, { "oscBLevel", 0.4f }, { "oscBTune", 5.0f },
            { "cutoff", 5000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 2.5f },
            { "chorusMix", 0.5f }, { "reverbMix", 0.7f }, { "reverbSize", 0.95f }, { "reverbDamp", 0.35f } } },

        { "Analog Pad", "PADS", {
            { "position", 0.43f }, { "unison", 6.0f }, { "detune", 20.0f }, { "spread", 0.85f },
            { "filterType", 0.0f }, { "cutoff", 1800.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.25f },
            { "fltAttack", 1.2f }, { "fltDecay", 1.5f }, { "fltSustain", 0.5f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.9f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 1.6f },
            { "chorusMix", 0.45f }, { "reverbMix", 0.35f } } },

        { "Sweep Pad", "PADS", {
            { "position", 0.43f }, { "unison", 5.0f }, { "detune", 16.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 500.0f }, { "resonance", 2.0f }, { "fltEnvAmount", 0.6f },
            { "fltAttack", 2.5f }, { "fltDecay", 2.5f }, { "fltSustain", 0.4f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.2f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 2.0f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.45f }, { "reverbSize", 0.85f } } },

        { "Breath Pad", "PADS", {
            { "tableA", 7.0f }, { "position", 0.1f }, { "unison", 3.0f }, { "detune", 25.0f }, { "spread", 1.0f },
            { "filterType", 3.0f }, { "cutoff", 2500.0f }, { "resonance", 1.5f },
            { "lfoShape", 0.0f }, { "lfoRate", 0.2f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 3.0f }, { "reverbMix", 0.55f }, { "reverbSize", 0.9f } } },

        { "Macro Pad", "PADS", {
            { "position", 0.25f }, { "unison", 5.0f }, { "detune", 16.0f }, { "spread", 0.85f },
            { "cutoff", 700.0f }, { "resonance", 1.2f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.8f },      // MACRO 1 : ouvre le filtre
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.6f },     // MACRO 2 : balaie la wavetable
            { "mx3Src", 11.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.7f },    // MACRO 3 : warp (mode SYNC)
            { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.8f },     // MACRO 4 : desaccord
            { "warpModeA", 3.0f }, { "attack", 1.0f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 1.8f },
            { "chorusMix", 0.4f }, { "reverbMix", 0.45f }, { "reverbSize", 0.85f } } },


        { "Cold Pad", "PADS", {
            { "tableA", 6.0f }, { "position", 0.3f }, { "unison", 5.0f }, { "detune", 20.0f }, { "spread", 0.9f },
            { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "resonance", 1.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 2.5f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.7f }, { "reverbSize", 0.9f }, { "reverbDamp", 0.3f } } },

        { "Drone Pad", "PADS", {
            { "tableA", 7.0f }, { "position", 0.1f }, { "unison", 7.0f }, { "detune", 50.0f }, { "spread", 1.0f }, { "subLevel", 0.3f },
            { "filterType", 0.0f }, { "cutoff", 800.0f }, { "resonance", 1.5f }, { "distType", 3.0f }, { "distAmt", 0.15f }, { "distMix", 0.3f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 2.5f }, { "decay", 1.0f }, { "sustain", 0.9f }, { "release", 3.5f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        { "Frozen Pad", "PADS", {
            { "tableA", 6.0f }, { "position", 0.5f }, { "warpModeA", 7.0f }, { "warpAmtA", 0.4f }, { "unison", 5.0f }, { "detune", 14.0f }, { "spread", 1.0f },
            { "cutoff", 5000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 3.0f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.8f }, { "reverbSize", 0.95f }, { "reverbDamp", 0.25f } } },

        { "Dark Ambient", "PADS", {
            { "position", 0.1f }, { "subLevel", 0.4f }, { "unison", 5.0f }, { "detune", 25.0f }, { "spread", 0.9f },
            { "filterType", 0.0f }, { "cutoff", 600.0f }, { "resonance", 1.2f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "lfo2Shape", 0.0f }, { "lfo2Rate", 0.06f }, { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.35f },
            { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "reverbMix", 0.8f }, { "reverbSize", 0.95f }, { "reverbDamp", 0.8f } } },

        { "Metal Pad", "PADS", {
            { "position", 0.43f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 23.0f }, { "ringAmount", 0.4f },
            { "unison", 4.0f }, { "detune", 16.0f }, { "spread", 0.9f }, { "cutoff", 3000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 2.5f }, { "reverbMix", 0.55f }, { "reverbSize", 0.9f } } },

        { "Distorted Pad", "PADS", {
            { "position", 0.43f }, { "unison", 6.0f }, { "detune", 22.0f }, { "spread", 0.9f }, { "filterType", 0.0f }, { "cutoff", 1500.0f },
            { "distType", 2.0f }, { "distAmt", 0.3f }, { "distMix", 0.4f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.2f }, { "decay", 1.0f }, { "sustain", 0.85f }, { "release", 2.0f }, { "chorusMix", 0.35f }, { "reverbMix", 0.5f } } },

        //======================================================================== CORDES
        { "PWM Strings", "CORDES", {
            { "tableA", 2.0f }, { "position", 0.3f }, { "lfoShape", 0.0f }, { "lfoRate", 0.6f }, { "lfoDepth", 0.35f },
            { "modAmount", 0.0f }, { "unison", 4.0f }, { "detune", 14.0f }, { "spread", 0.85f },
            { "cutoff", 3500.0f }, { "resonance", 0.8f },
            { "attack", 0.5f }, { "decay", 0.6f }, { "sustain", 0.8f }, { "release", 1.0f },
            { "chorusMix", 0.5f }, { "chorusRate", 0.7f }, { "reverbMix", 0.3f } } },

        { "String Ensemble", "CORDES", {
            { "position", 0.43f }, { "unison", 6.0f }, { "detune", 16.0f }, { "spread", 0.9f },
            { "filterType", 0.0f }, { "cutoff", 3000.0f }, { "resonance", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.4f }, { "decay", 0.8f }, { "sustain", 0.85f }, { "release", 0.8f }, { "velFilter", 0.3f },
            { "chorusMix", 0.6f }, { "chorusRate", 0.6f }, { "reverbMix", 0.4f }, { "reverbSize", 0.75f } } },

        { "Slow Strings", "CORDES", {
            { "position", 0.43f }, { "unison", 6.0f }, { "detune", 14.0f }, { "spread", 0.9f },
            { "filterType", 0.0f }, { "cutoff", 2200.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.3f }, { "decay", 1.0f }, { "sustain", 0.9f }, { "release", 1.8f },
            { "chorusMix", 0.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.85f } } },

        { "Pizzicato", "CORDES", {
            { "position", 0.43f }, { "unison", 3.0f }, { "detune", 6.0f }, { "spread", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 900.0f }, { "fltEnvAmount", 0.6f }, { "fltAttack", 0.002f },
            { "fltDecay", 0.18f }, { "fltSustain", 0.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.002f }, { "decay", 0.22f }, { "sustain", 0.0f }, { "release", 0.2f }, { "velFilter", 0.4f },
            { "reverbMix", 0.3f }, { "reverbSize", 0.6f } } },

        { "Solo Cello", "CORDES", {
            { "mono", 1.0f }, { "glide", 0.06f }, { "position", 0.43f }, { "unison", 3.0f }, { "detune", 7.0f }, { "spread", 0.5f },
            { "filterType", 0.0f }, { "cutoff", 1200.0f }, { "resonance", 1.0f }, { "fltEnvAmount", 0.3f }, { "fltAttack", 0.12f },
            { "fltDecay", 0.4f }, { "fltSustain", 0.6f }, { "lfoShape", 0.0f }, { "lfoRate", 5.0f }, { "lfoDepth", 0.0f },
            { "modAmount", 0.0f }, { "attack", 0.15f }, { "decay", 0.4f }, { "sustain", 0.85f }, { "release", 0.3f },
            { "reverbMix", 0.3f } } },

        { "Tremolo Strings", "CORDES", {
            { "position", 0.43f }, { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.9f },
            { "filterType", 0.0f }, { "cutoff", 4000.0f }, { "lfoShape", 0.0f }, { "lfoRate", 6.0f }, { "lfoDepth", 0.0f },
            { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.4f },
            { "attack", 0.3f }, { "decay", 0.5f }, { "sustain", 0.85f }, { "release", 0.8f },
            { "chorusMix", 0.35f }, { "reverbMix", 0.45f }, { "reverbSize", 0.8f } } },

        //======================================================================== CLOCHES
        { "Glass Bells", "CLOCHES", {
            { "tableA", 6.0f }, { "position", 0.35f }, { "unison", 2.0f }, { "detune", 6.0f }, { "cutoff", 12000.0f },
            { "resonance", 0.7f }, { "attack", 0.002f }, { "decay", 2.0f }, { "sustain", 0.0f }, { "release", 1.5f },
            { "modAttack", 0.002f }, { "modDecay", 1.2f }, { "modSustain", 0.0f }, { "modAmount", 0.5f },
            { "lfoDepth", 0.0f }, { "reverbMix", 0.45f }, { "reverbSize", 0.8f }, { "delayMix", 0.2f } } },

        { "FM Bell", "CLOCHES", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.3f },
            { "unison", 1.0f }, { "spread", 0.0f }, { "cutoff", 12000.0f },
            { "attack", 0.002f }, { "decay", 1.8f }, { "sustain", 0.0f }, { "release", 1.2f },
            { "modAttack", 0.002f }, { "modDecay", 0.9f }, { "modSustain", 0.0f }, { "modAmount", 0.0f }, { "lfoDepth", 0.0f },
            { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.6f },
            { "reverbMix", 0.35f }, { "reverbSize", 0.8f } } },

        { "Tubular Bell", "CLOCHES", {
            { "tableA", 6.0f }, { "position", 0.2f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 4.0f },
            { "fmAmount", 0.4f }, { "unison", 1.0f }, { "cutoff", 10000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "modAttack", 0.002f }, { "modDecay", 1.5f }, { "modSustain", 0.0f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.4f },
            { "attack", 0.002f }, { "decay", 3.0f }, { "sustain", 0.0f }, { "release", 2.5f },
            { "reverbMix", 0.4f }, { "reverbSize", 0.85f } } },

        { "Music Box", "CLOCHES", {
            { "position", 0.0f }, { "unison", 1.0f }, { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "oscBLevel", 0.25f },
            { "filterType", 0.0f }, { "cutoff", 9000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.9f }, { "sustain", 0.0f }, { "release", 0.7f }, { "velAmp", 0.7f },
            { "reverbMix", 0.3f }, { "reverbSize", 0.7f }, { "delayMix", 0.12f } } },

        { "Church Bell", "CLOCHES", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 38.0f }, { "ringAmount", 0.5f },
            { "oscBLevel", 0.3f }, { "unison", 1.0f }, { "cutoff", 8000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 3.5f }, { "sustain", 0.0f }, { "release", 3.0f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.9f } } },

        //======================================================================== PLUCKS
        { "Classic Pluck", "PLUCKS", {
            { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "spread", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 500.0f }, { "resonance", 2.2f }, { "fltEnvAmount", 0.75f },
            { "fltAttack", 0.002f }, { "fltDecay", 0.28f }, { "fltSustain", 0.0f },
            { "attack", 0.002f }, { "decay", 0.4f }, { "sustain", 0.0f }, { "release", 0.25f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velFilter", 0.4f },
            { "chorusMix", 0.25f }, { "delayMix", 0.15f }, { "delayTime", 320.0f }, { "reverbMix", 0.15f } } },

        { "Harp Pluck", "PLUCKS", {
            { "position", 0.0f }, { "unison", 2.0f }, { "detune", 5.0f }, { "spread", 0.7f }, { "oscBPos", 0.0f },
            { "oscBOct", 1.0f }, { "oscBLevel", 0.2f }, { "filterType", 0.0f }, { "cutoff", 3500.0f }, { "fltEnvAmount", 0.3f },
            { "fltDecay", 0.4f }, { "fltSustain", 0.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 1.2f }, { "sustain", 0.0f }, { "release", 0.8f },
            { "reverbMix", 0.4f }, { "reverbSize", 0.8f }, { "chorusMix", 0.15f } } },

        { "Marimba", "PLUCKS", {
            { "position", 0.0f }, { "unison", 1.0f }, { "oscBPos", 0.0f }, { "oscBOct", 2.0f }, { "oscBLevel", 0.18f }, { "oscBTune", 12.0f },
            { "filterType", 0.0f }, { "cutoff", 2500.0f }, { "fltEnvAmount", 0.2f }, { "fltDecay", 0.15f }, { "fltSustain", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f }, { "release", 0.35f },
            { "velFilter", 0.5f }, { "reverbMix", 0.22f } } },

        { "Bright Pluck", "PLUCKS", {
            { "position", 0.43f }, { "unison", 3.0f }, { "detune", 12.0f }, { "spread", 0.8f },
            { "filterType", 1.0f }, { "cutoff", 800.0f }, { "resonance", 1.5f }, { "fltEnvAmount", 0.85f }, { "fltAttack", 0.001f },
            { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.3f }, { "sustain", 0.0f }, { "release", 0.2f },
            { "delayMix", 0.25f }, { "delayTime", 280.0f }, { "delayFeedback", 0.4f }, { "reverbMix", 0.2f } } },

        { "Echo Pluck", "PLUCKS", {
            { "position", 0.3f }, { "warpModeA", 1.0f }, { "warpAmtA", 0.3f }, { "unison", 2.0f }, { "detune", 7.0f },
            { "filterType", 0.0f }, { "cutoff", 1200.0f }, { "fltEnvAmount", 0.6f }, { "fltDecay", 0.22f }, { "fltSustain", 0.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.002f }, { "decay", 0.3f }, { "sustain", 0.0f }, { "release", 0.2f },
            { "delayMix", 0.45f }, { "delayTime", 330.0f }, { "delayFeedback", 0.6f }, { "reverbMix", 0.3f } } },

        //======================================================================== CLAVIERS
        { "Drawbar Organ", "CLAVIERS", {
            { "tableA", 4.0f }, { "position", 0.55f }, { "unison", 1.0f }, { "spread", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.004f }, { "decay", 0.1f }, { "sustain", 1.0f }, { "release", 0.08f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "velAmp", 0.2f },
            { "chorusMix", 0.35f }, { "chorusRate", 4.5f }, { "chorusDepth", 0.3f }, { "reverbMix", 0.12f } } },

        { "Electric Keys", "CLAVIERS", {
            { "position", 0.1f }, { "oscBLevel", 0.35f }, { "oscBPos", 0.43f }, { "oscBOct", 1.0f },
            { "unison", 2.0f }, { "detune", 5.0f }, { "cutoff", 3500.0f },
            { "fltEnvAmount", 0.4f }, { "fltDecay", 0.5f }, { "fltSustain", 0.1f },
            { "attack", 0.003f }, { "decay", 1.4f }, { "sustain", 0.15f }, { "release", 0.5f },
            { "velAmp", 1.0f }, { "velFilter", 0.6f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "chorusMix", 0.3f }, { "reverbMix", 0.2f } } },

        { "Pulse Keys", "CLAVIERS", {
            { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "cutoff", 600.0f }, { "resonance", 2.0f },
            { "fltEnvAmount", 0.5f }, { "fltDecay", 0.35f }, { "fltSustain", 0.2f },
            { "attack", 0.003f }, { "decay", 0.8f }, { "sustain", 0.5f }, { "release", 0.4f },
            { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "mx1Src", 1.0f }, { "mx1Dst", 10.0f }, { "mx1Amt", 0.6f } } },

        { "Soft Rhodes", "CLAVIERS", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "fmAmount", 0.15f }, { "unison", 2.0f }, { "detune", 4.0f },
            { "spread", 0.5f }, { "filterType", 0.0f }, { "cutoff", 4500.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "modAttack", 0.002f }, { "modDecay", 0.6f }, { "modSustain", 0.0f }, { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.3f },
            { "attack", 0.003f }, { "decay", 1.6f }, { "sustain", 0.2f }, { "release", 0.6f }, { "velFilter", 0.5f },
            { "chorusMix", 0.4f }, { "chorusRate", 0.6f }, { "reverbMix", 0.25f } } },

        { "Clav Funk", "CLAVIERS", {
            { "position", 0.43f }, { "unison", 1.0f }, { "filterType", 2.0f }, { "cutoff", 700.0f }, { "resonance", 2.5f },
            { "fltEnvAmount", 0.5f }, { "fltAttack", 0.001f }, { "fltDecay", 0.12f }, { "fltSustain", 0.1f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.001f }, { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.12f },
            { "velFilter", 0.6f }, { "drive", 0.2f } } },

        { "80s Keys", "CLAVIERS", {
            { "position", 0.1f }, { "oscBPos", 0.43f }, { "oscBLevel", 0.4f }, { "oscBOct", 0.0f }, { "oscBTune", 8.0f },
            { "unison", 2.0f }, { "detune", 10.0f }, { "spread", 0.8f }, { "cutoff", 4000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.004f }, { "decay", 1.0f }, { "sustain", 0.4f }, { "release", 0.5f }, { "velFilter", 0.4f },
            { "chorusMix", 0.5f }, { "delayMix", 0.2f }, { "delayTime", 300.0f }, { "reverbMix", 0.3f } } },

        { "Church Organ", "CLAVIERS", {
            { "tableA", 4.0f }, { "position", 0.8f }, { "unison", 3.0f }, { "detune", 3.0f }, { "spread", 0.6f },
            { "oscBPos", 0.3f }, { "oscBOct", -1.0f }, { "oscBLevel", 0.5f }, { "cutoff", 7000.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.02f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.3f },
            { "velAmp", 0.0f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        //======================================================================== CUIVRES
        { "Velocity Brass", "CUIVRES", {
            { "position", 0.43f }, { "unison", 3.0f }, { "detune", 9.0f }, { "filterType", 0.0f }, { "cutoff", 700.0f },
            { "fltEnvAmount", 0.5f }, { "fltAttack", 0.12f }, { "fltDecay", 0.5f }, { "fltSustain", 0.5f },
            { "attack", 0.06f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.2f },
            { "velAmp", 0.6f }, { "velFilter", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "reverbMix", 0.15f } } },

        { "Brass Stab", "CUIVRES", {
            { "position", 0.43f }, { "unison", 4.0f }, { "detune", 12.0f }, { "spread", 0.7f }, { "filterType", 1.0f },
            { "cutoff", 600.0f }, { "resonance", 1.2f }, { "fltEnvAmount", 0.7f }, { "fltAttack", 0.03f }, { "fltDecay", 0.3f },
            { "fltSustain", 0.2f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.02f }, { "decay", 0.35f }, { "sustain", 0.4f }, { "release", 0.2f }, { "velFilter", 0.5f },
            { "chorusMix", 0.2f }, { "reverbMix", 0.2f } } },

        { "Soft Horn", "CUIVRES", {
            { "mono", 1.0f }, { "glide", 0.05f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 5.0f },
            { "filterType", 0.0f }, { "cutoff", 900.0f }, { "fltEnvAmount", 0.4f }, { "fltAttack", 0.15f }, { "fltDecay", 0.4f },
            { "fltSustain", 0.6f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.12f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.3f }, { "velFilter", 0.4f }, { "reverbMix", 0.3f } } },


        //======================================================================== INDUS (percussions, coups, stabs)
        { "Metal Hit", "INDUS", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 23.0f }, { "ringAmount", 0.7f }, { "unison", 1.0f },
            { "noiseLevel", 0.3f }, { "noiseDecay", 0.15f }, { "filterType", 3.0f }, { "cutoff", 3000.0f }, { "resonance", 3.0f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.001f }, { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.25f },
            { "reverbMix", 0.3f }, { "reverbSize", 0.7f } } },

        { "Industrial Stab", "INDUS", {
            { "position", 0.43f }, { "unison", 5.0f }, { "detune", 30.0f }, { "spread", 0.8f }, { "filterType", 1.0f }, { "cutoff", 1200.0f },
            { "resonance", 1.5f }, { "fltEnvAmount", 0.6f }, { "fltAttack", 0.002f }, { "fltDecay", 0.15f }, { "fltSustain", 0.0f },
            { "distType", 1.0f }, { "distAmt", 0.5f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.002f }, { "decay", 0.25f }, { "sustain", 0.0f }, { "release", 0.15f },
            { "delayMix", 0.25f }, { "delayTime", 280.0f }, { "delayFeedback", 0.4f }, { "reverbMix", 0.25f } } },

        { "Noise Snare", "INDUS", {
            { "mono", 1.0f }, { "position", 0.0f }, { "unison", 1.0f }, { "oscALevel", 0.6f }, { "phaseMode", 1.0f },
            { "pitchEnvAmt", 12.0f }, { "pitchEnvDecay", 0.04f }, { "noiseLevel", 0.9f }, { "noiseTone", 0.6f }, { "noiseDecay", 0.12f },
            { "filterType", 2.0f }, { "cutoff", 200.0f }, { "resonance", 0.8f }, { "distType", 3.0f }, { "distAmt", 0.2f }, { "distMix", 0.5f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.001f }, { "decay", 0.2f }, { "sustain", 0.0f }, { "release", 0.1f },
            { "reverbMix", 0.2f } } },

        { "Hat Closed", "INDUS", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.8f }, { "noiseTone", 1.0f }, { "noiseDecay", 0.03f },
            { "filterType", 2.0f }, { "cutoff", 7000.0f }, { "resonance", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.05f }, { "sustain", 0.0f }, { "release", 0.02f } } },

        { "Hat Open", "INDUS", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.8f }, { "noiseTone", 1.0f }, { "noiseDecay", 0.25f },
            { "filterType", 2.0f }, { "cutoff", 6500.0f }, { "resonance", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.3f }, { "sustain", 0.0f }, { "release", 0.15f }, { "reverbMix", 0.15f } } },

        { "Hat Industrial", "INDUS", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.85f }, { "noiseType", 2.0f }, { "noiseTone", 0.9f }, { "noiseDecay", 0.05f },
            { "filterType", 2.0f }, { "cutoff", 5000.0f }, { "resonance", 1.5f }, { "distType", 3.0f }, { "distAmt", 0.3f }, { "distMix", 0.6f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 0.001f }, { "decay", 0.08f }, { "sustain", 0.0f }, { "release", 0.03f } } },

        { "Clap Noise", "INDUS", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.9f }, { "noiseTone", 0.8f }, { "noiseDecay", 0.09f },
            { "filterType", 3.0f }, { "cutoff", 1500.0f }, { "resonance", 1.5f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.14f }, { "sustain", 0.0f }, { "release", 0.1f }, { "reverbMix", 0.3f }, { "reverbSize", 0.6f } } },

        { "Laser Zap", "INDUS", {
            { "mono", 1.0f }, { "position", 0.43f }, { "unison", 1.0f }, { "phaseMode", 1.0f }, { "pitchEnvAmt", 48.0f }, { "pitchEnvDecay", 0.12f },
            { "filterType", 1.0f }, { "cutoff", 8000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.25f }, { "sustain", 0.0f }, { "release", 0.1f }, { "delayMix", 0.2f } } },

        { "Anvil", "INDUS", {
            { "position", 0.0f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 31.0f }, { "fmAmount", 0.5f }, { "ringAmount", 0.5f },
            { "unison", 1.0f }, { "noiseLevel", 0.2f }, { "noiseDecay", 0.02f }, { "cutoff", 9000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.001f }, { "decay", 0.5f }, { "sustain", 0.0f }, { "release", 0.4f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f } } },

        { "Siren", "INDUS", {
            { "mono", 1.0f }, { "glide", 0.1f }, { "position", 0.43f }, { "unison", 2.0f }, { "detune", 8.0f }, { "filterType", 0.0f }, { "cutoff", 4000.0f },
            { "lfoShape", 1.0f }, { "lfoRate", 0.8f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 5.0f }, { "mx1Amt", 0.5f },
            { "attack", 0.1f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.4f }, { "delayMix", 0.2f }, { "reverbMix", 0.3f } } },

        //======================================================================== TEXTURES
        { "Chaos Texture", "TEXTURES", {
            { "tableA", 7.0f }, { "position", 0.2f }, { "lfoShape", 0.0f }, { "lfoRate", 0.15f }, { "lfoDepth", 0.6f },
            { "unison", 5.0f }, { "detune", 30.0f }, { "spread", 1.0f }, { "cutoff", 3000.0f }, { "resonance", 2.0f },
            { "filterType", 3.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 2.5f },
            { "mx1Src", 7.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.3f }, { "modAmount", 0.0f },
            { "chorusMix", 0.4f }, { "delayMix", 0.3f }, { "delayTime", 520.0f }, { "delayFeedback", 0.5f },
            { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        { "Resonance Sweep", "TEXTURES", {
            { "tableA", 5.0f }, { "position", 0.2f }, { "modAttack", 1.5f }, { "modDecay", 1.5f }, { "modSustain", 0.6f },
            { "modAmount", 0.5f }, { "lfoDepth", 0.0f }, { "unison", 3.0f }, { "detune", 14.0f }, { "cutoff", 5000.0f },
            { "attack", 0.3f }, { "decay", 0.8f }, { "sustain", 0.8f }, { "release", 0.8f }, { "reverbMix", 0.3f } } },

        { "Ring Metal", "TEXTURES", {
            { "position", 0.43f }, { "oscBPos", 0.0f }, { "oscBOct", 1.0f }, { "oscBTune", 15.0f }, { "ringAmount", 0.6f },
            { "unison", 2.0f }, { "detune", 6.0f }, { "cutoff", 5000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.003f }, { "decay", 0.9f }, { "sustain", 0.2f }, { "release", 0.6f },
            { "lfo2Rate", 0.3f }, { "mx1Src", 8.0f }, { "mx1Dst", 17.0f }, { "mx1Amt", 0.3f } } },

        { "Wind Sweep", "TEXTURES", {
            { "tableA", 7.0f }, { "position", 0.6f }, { "unison", 7.0f }, { "detune", 60.0f }, { "spread", 1.0f },
            { "filterType", 3.0f }, { "cutoff", 1500.0f }, { "resonance", 3.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "lfoShape", 0.0f }, { "lfoRate", 0.1f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "attack", 2.5f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 3.5f }, { "reverbMix", 0.55f }, { "reverbSize", 0.95f } } },

        { "Robot Voice", "TEXTURES", {
            { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 1.0f }, { "position", 0.45f }, { "oscBPos", 0.0f }, { "oscBOct", 0.0f },
            { "oscBTune", 20.0f }, { "ringAmount", 0.5f }, { "unison", 1.0f }, { "cutoff", 6000.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.85f }, { "release", 0.15f }, { "delayMix", 0.15f } } },

        { "Dark Drone", "TEXTURES", {
            { "position", 0.1f }, { "subLevel", 0.5f }, { "unison", 7.0f }, { "detune", 40.0f }, { "spread", 1.0f },
            { "filterType", 1.0f }, { "cutoff", 400.0f }, { "resonance", 2.5f }, { "lfoShape", 0.0f }, { "lfoRate", 0.07f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.4f },
            { "attack", 3.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 4.0f }, { "reverbMix", 0.6f }, { "reverbSize", 0.95f } } },

        { "Alien Choir", "TEXTURES", {
            { "tableA", 1.0f }, { "position", 0.7f }, { "oscBPos", 0.3f }, { "oscBLevel", 0.4f }, { "oscBTune", 23.0f }, { "oscBOct", 1.0f },
            { "fmAmount", 0.15f }, { "unison", 5.0f }, { "detune", 35.0f }, { "spread", 1.0f }, { "cutoff", 6000.0f },
            { "lfoShape", 1.0f }, { "lfoRate", 0.3f }, { "lfoDepth", 0.5f }, { "modAmount", 0.0f },
            { "attack", 1.2f }, { "decay", 1.0f }, { "sustain", 0.8f }, { "release", 2.0f },
            { "chorusMix", 0.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        { "Noise Bed", "TEXTURES", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.6f }, { "noiseType", 1.0f }, { "noiseTone", 0.5f },
            { "filterType", 3.0f }, { "cutoff", 1200.0f }, { "resonance", 2.5f }, { "lfo2Shape", 0.0f }, { "lfo2Rate", 0.1f },
            { "mx1Src", 8.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 2.0f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f } } },

        { "Machine Hum", "TEXTURES", {
            { "position", 0.43f }, { "subLevel", 0.4f }, { "unison", 5.0f }, { "detune", 12.0f }, { "spread", 0.6f },
            { "filterType", 1.0f }, { "cutoff", 500.0f }, { "resonance", 1.5f }, { "distType", 1.0f }, { "distAmt", 0.5f }, { "distMix", 0.6f },
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.3f } } },

        { "Static Field", "TEXTURES", {
            { "oscALevel", 0.0f }, { "unison", 1.0f }, { "noiseLevel", 0.6f }, { "noiseType", 2.0f }, { "noiseTone", 0.8f },
            { "filterType", 2.0f }, { "cutoff", 1500.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f },
            { "attack", 1.5f }, { "decay", 1.0f }, { "sustain", 1.0f }, { "release", 2.5f },
            { "delayMix", 0.35f }, { "delayTime", 410.0f }, { "delayFeedback", 0.55f }, { "reverbMix", 0.6f }, { "reverbSize", 0.9f } } },

        { "Pulse Machine", "TEXTURES", {
            { "position", 0.43f }, { "unison", 3.0f }, { "detune", 10.0f }, { "filterType", 1.0f }, { "cutoff", 1500.0f }, { "resonance", 2.0f },
            { "lfoShape", 3.0f }, { "lfoSync", 7.0f }, { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.9f },
            { "distType", 0.0f }, { "distAmt", 0.3f }, { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.2f },
            { "delayMix", 0.25f }, { "reverbMix", 0.2f } } }
    };

    return presets;
}
