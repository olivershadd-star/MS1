#include "FactoryBank.h"

//==============================================================================
// Banque d'usine de MS1 (v1.0) : presets recréés pour le nouveau moteur.
// Principe : chaque preset part d'une SOURCE propre et reconnaissable (table d'ondes, FM, corde, barre, cloche, tube, souffle) ;
// la saleté (disto, drive) n'est ajoutée que là où elle fait partie du son, et reste dosée par la macro GRAIN.
//
//   Macros (même sens dans presque tous les presets) : 1 BRILLANCE, 2 MOUVEMENT, 3 GRAIN, 4 LARGEUR
//   tableA / tableB : 0 BASIQUE 1 VOCAL 2 PWM 3 SPECTRE 4 ORGUE 5 RESONANCE 6 PEIGNE 7 CHAOS
//                     8 CLASSIQUE 9 CHOEUR 10 FM METAL 11 ORGUE ELEC 12 PIANO EP 13 CORDES 14 VENTS 15 GRANULAIRE
//                     16 DISTO 17 HARMONIQUES 18 CHIP 19 BASSES 20 GUITARES 21 DESSIN 22 SYNTH A 23 SYNTH B
//   rsMat (résonateur) : 0 CORDE 1 BARRE 2 MARIMBA 3 CLOCHE 4 TUBE 5 PLAQUE 6 BOL
//   filterType : 0 LP12  1 LP24  2 HP  3 BP  4 NOTCH  5 PEIGNE      lfoShape : 0 sinus 1 triangle 2 saw 3 carre 4 saw bas 5 hasard pas 6 hasard lisse
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
            { "glide", 0.04f }, { "tableA", 0.0f }, { "position", 0.0f }, { "subLevel", 0.55f },
            { "filterType", 1.0f }, { "cutoff", 520.0f }, { "resonance", 0.7f }, { "attack", 0.004f },
            { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.15f }, { "distType", 0.0f }, { "distAmt", 0.0f },
            { "fltKey", 0.25f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 11.0f },
            { "mx3Dst", 18.0f }, { "mx3Amt", 0.8f }, { "mx3Crv", 5.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 8.0f },
            { "mx4Amt", 0.6f }, { "mx4Crv", 6.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 4.0f }, { "mx5Amt", 0.5f },
            { "mx5Crv", 7.0f }, { "voiceGain", -3.8f } } },

        { "Reese Propre", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 19.0f },
            { "position", 0.3f }, { "tableB", 19.0f }, { "oscBPos", 0.35f }, { "oscBLevel", 0.9f },
            { "oscBTune", 14.0f }, { "unison", 1.0f }, { "detune", 12.0f }, { "spread", 0.6f }, { "subLevel", 0.4f },
            { "filterType", 1.0f }, { "cutoff", 420.0f }, { "resonance", 1.0f }, { "fltKey", 0.3f },
            { "phaserMix", 0.0f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 1.0f }, { "release", 0.2f },
            { "lfo3Rate", 0.12f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.55f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f },
            { "mx3Dst", 2.0f }, { "mx3Amt", 0.6f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.8f }, { "mx4Crv", 5.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 8.0f }, { "mx5Amt", 0.6f },
            { "mx5Crv", 6.0f }, { "mx6Src", 11.0f }, { "mx6Dst", 4.0f }, { "mx6Amt", 0.5f }, { "mx6Crv", 7.0f },
            { "mx7Src", 12.0f }, { "mx7Dst", 6.0f }, { "mx7Amt", 0.7f }, { "mx7Crv", 0.0f }, { "voiceGain", -3.4f } } },

        { "Acide Rond", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.06f }, { "tableA", 0.0f }, { "position", 0.55f }, { "filterType", 1.0f },
            { "cutoff", 420.0f }, { "resonance", 4.5f }, { "drive", 0.12f }, { "fltEnvAmount", 0.65f },
            { "fltAttack", 0.003f }, { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "attack", 0.003f },
            { "decay", 0.3f }, { "sustain", 0.6f }, { "release", 0.08f }, { "distType", 0.0f }, { "distAmt", 0.12f },
            { "velFilter", 0.5f }, { "fltKey", 0.3f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx1Crv", 0.0f }, { "mx2Src", 10.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.4f }, { "mx2Crv", 0.0f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.8f }, { "mx3Crv", 5.0f }, { "mx4Src", 11.0f },
            { "mx4Dst", 8.0f }, { "mx4Amt", 0.6f }, { "mx4Crv", 6.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 4.0f },
            { "mx5Amt", 0.5f }, { "mx5Crv", 7.0f }, { "voiceGain", 3.9f } } },

        { "Corde Basse", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.0f }, { "oscALevel", 0.0f }, { "subLevel", 0.3f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f },
            { "rsDecay", 1.6f }, { "rsDamp", 0.8f }, { "rsPos", 0.12f }, { "rsBurst", 1.6f }, { "rsBright", 0.55f },
            { "rsStretch", 0.03f }, { "filterType", 0.0f }, { "cutoff", 1500.0f }, { "resonance", 0.8f },
            { "fltKey", 0.5f }, { "attack", 0.001f }, { "decay", 1.8f }, { "sustain", 0.0f }, { "release", 0.4f },
            { "velFilter", 0.3f }, { "noiseLevel", 0.05f }, { "noiseType", 1.0f }, { "noiseTone", 0.4f },
            { "noiseDecay", 0.02f }, { "reverbMix", 0.1f }, { "reverbSize", 0.3f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 11.0f }, { "mx2Dst", 18.0f },
            { "mx2Amt", 0.35f }, { "mx2Crv", 1.0f }, { "voiceGain", 4.1f } } },

        { "Wobble Large", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.03f }, { "tableA", 22.0f },
            { "position", 0.5f }, { "unison", 3.0f }, { "detune", 12.0f }, { "spread", 0.4f }, { "subLevel", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 360.0f }, { "resonance", 2.2f }, { "distType", 0.0f },
            { "distAmt", 0.22f }, { "lfoShape", 1.0f }, { "lfoSync", 6.0f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.12f }, { "fltKey", 0.2f }, { "mx1Src", 1.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.5f }, { "mx2Src", 1.0f }, { "mx2Dst", 4.0f }, { "mx2Amt", 0.18f }, { "mx3Src", 9.0f },
            { "mx3Dst", 3.0f }, { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f }, { "mx4Src", 10.0f }, { "mx4Dst", 1.0f },
            { "mx4Amt", 0.5f }, { "mx4Crv", 0.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f }, { "mx5Amt", 0.8f },
            { "mx5Crv", 5.0f }, { "mx6Src", 11.0f }, { "mx6Dst", 8.0f }, { "mx6Amt", 0.6f }, { "mx6Crv", 6.0f },
            { "mx7Src", 11.0f }, { "mx7Dst", 4.0f }, { "mx7Amt", 0.5f }, { "mx7Crv", 7.0f }, { "mx8Src", 12.0f },
            { "mx8Dst", 6.0f }, { "mx8Amt", 0.5f }, { "mx8Crv", 0.0f }, { "voiceGain", -5.0f } } },

        { "Gate Machine", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.02f }, { "tableA", 18.0f }, { "position", 0.4f }, { "subLevel", 0.4f },
            { "filterType", 1.0f }, { "cutoff", 900.0f }, { "resonance", 1.2f }, { "distType", 0.0f },
            { "distAmt", 0.2f }, { "lfoShape", 1.0f }, { "lfoSync", 7.0f }, { "attack", 0.002f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.04f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f },
            { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.8f }, { "mx4Crv", 5.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 8.0f }, { "mx5Amt", 0.6f },
            { "mx5Crv", 6.0f }, { "mx6Src", 11.0f }, { "mx6Dst", 4.0f }, { "mx6Amt", 0.5f }, { "mx6Crv", 7.0f },
            { "voiceGain", -2.8f } } },

        { "FM Boisee", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.04f }, { "tableA", 0.0f }, { "position", 0.0f }, { "tableB", 0.0f }, { "oscBPos", 0.0f },
            { "oscBLevel", 0.0f }, { "fmAmount", 0.22f }, { "subLevel", 0.45f }, { "filterType", 1.0f },
            { "cutoff", 1400.0f }, { "resonance", 0.9f }, { "fltKey", 0.3f }, { "attack", 0.003f }, { "decay", 0.2f },
            { "sustain", 1.0f }, { "release", 0.15f }, { "modDecay", 0.25f }, { "modSustain", 0.0f },
            { "mx1Src", 2.0f }, { "mx1Dst", 16.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.45f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.35f },
            { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.8f }, { "mx4Crv", 5.0f },
            { "mx5Src", 11.0f }, { "mx5Dst", 8.0f }, { "mx5Amt", 0.6f }, { "mx5Crv", 6.0f }, { "mx6Src", 11.0f },
            { "mx6Dst", 4.0f }, { "mx6Amt", 0.5f }, { "mx6Crv", 7.0f }, { "voiceGain", -4.2f } } },

        { "Pompe Sidechain", "BASSES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "glide", 0.03f }, { "tableA", 8.0f }, { "position", 0.8f }, { "subLevel", 0.5f }, { "filterType", 1.0f },
            { "cutoff", 800.0f }, { "resonance", 1.0f }, { "lfoShape", 4.0f }, { "lfoSync", 5.0f },
            { "attack", 0.003f }, { "decay", 0.2f }, { "sustain", 1.0f }, { "release", 0.1f }, { "fltKey", 0.3f },
            { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.5f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f },
            { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f },
            { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.8f }, { "mx4Crv", 5.0f },
            { "mx5Src", 11.0f }, { "mx5Dst", 8.0f }, { "mx5Amt", 0.6f }, { "mx5Crv", 6.0f }, { "mx6Src", 11.0f },
            { "mx6Dst", 4.0f }, { "mx6Amt", 0.5f }, { "mx6Crv", 7.0f }, { "voiceGain", -2.9f } } },

        //======================================================================== NAPPES
        { "Brume Choeur", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 9.0f }, { "position", 0.35f }, { "tableB", 9.0f },
            { "oscBPos", 0.45f }, { "oscBLevel", 0.6f }, { "oscBTune", -9.0f }, { "unison", 5.0f },
            { "detune", 11.0f }, { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 6500.0f }, { "fltKey", 0.5f },
            { "attack", 0.8f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.4f },
            { "reverbSize", 0.85f }, { "chorusMix", 0.2f }, { "lfo3Rate", 0.11f }, { "lfo3Shape", 1.0f },
            { "lfoFade", 1.5f }, { "mx1Src", 14.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.22f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f },
            { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.35f },
            { "mx4Crv", 1.0f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f }, { "mx5Crv", 0.0f },
            { "voiceGain", -0.6f } } },

        { "Cathedrale Orgue", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 4.0f }, { "position", 0.45f }, { "unison", 3.0f },
            { "detune", 7.0f }, { "spread", 0.8f }, { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "fltKey", 0.4f },
            { "attack", 0.35f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 1.8f }, { "chorusMix", 0.3f },
            { "chorusRate", 0.5f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.3f },
            { "mx3Crv", 1.0f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f }, { "mx4Crv", 0.0f },
            { "voiceGain", -1.0f } } },

        { "Cordes Larges", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 13.0f }, { "position", 0.4f },
            { "tableB", 13.0f }, { "oscBPos", 0.6f }, { "oscBLevel", 0.7f }, { "oscBTune", 7.0f }, { "unison", 5.0f },
            { "detune", 13.0f }, { "spread", 1.0f }, { "filterType", 1.0f }, { "cutoff", 4200.0f },
            { "resonance", 0.8f }, { "fltKey", 0.5f }, { "attack", 0.9f }, { "decay", 0.5f }, { "sustain", 1.0f },
            { "release", 2.2f }, { "chorusMix", 0.3f }, { "reverbMix", 0.4f }, { "reverbSize", 0.85f },
            { "lfo3Rate", 0.1f }, { "lfo3Shape", 1.0f }, { "tbAmt", 0.35f }, { "tbDecay", 1.5f }, { "tbEnd", 3.5f },
            { "tbVel", 0.4f }, { "mx1Src", 14.0f }, { "mx1Dst", 1.0f }, { "mx1Amt", 0.28f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 2.0f },
            { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.3f },
            { "mx4Crv", 1.0f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f }, { "mx5Crv", 0.0f },
            { "voiceGain", -1.0f } } },

        { "Verre Chante", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "rsLevel", 0.9f }, { "rsMat", 6.0f }, { "rsDecay", 5.0f }, { "rsDamp", 0.15f }, { "rsPos", 0.2f },
            { "rsBurst", 30.0f }, { "rsBright", 0.45f }, { "rsFeed", 1.0f }, { "rsDry", 0.0f },
            { "noiseLevel", 0.45f }, { "noiseType", 1.0f }, { "noiseTone", 0.75f }, { "noiseDecay", 5.0f },
            { "attack", 1.2f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "filterType", 0.0f },
            { "cutoff", 9000.0f }, { "fltKey", 0.4f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f },
            { "lfo3Rate", 0.09f }, { "lfo3Shape", 0.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 21.0f }, { "mx1Amt", 0.0f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f },
            { "mx3Dst", 19.0f }, { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.25f }, { "mx4Crv", 1.0f }, { "voiceGain", -2.1f } } },

        { "Souffle Nord", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "rsLevel", 0.9f }, { "rsMat", 4.0f }, { "rsDecay", 0.35f }, { "rsDamp", 0.3f }, { "rsPos", 0.45f },
            { "rsBurst", 20.0f }, { "rsBright", 0.5f }, { "rsFeed", 1.0f }, { "rsDry", 0.5f }, { "noiseLevel", 1.0f },
            { "noiseType", 0.0f }, { "noiseTone", 0.6f }, { "noiseDecay", 5.0f }, { "attack", 0.7f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.2f }, { "filterType", 0.0f }, { "cutoff", 7000.0f },
            { "fltKey", 0.6f }, { "reverbMix", 0.5f }, { "reverbSize", 0.9f }, { "lfo3Rate", 0.08f },
            { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 19.0f }, { "mx1Amt", 0.15f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 19.0f },
            { "mx3Amt", 0.3f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.25f },
            { "mx4Crv", 1.0f }, { "voiceGain", 9.1f } } },

        { "Nuit Planetaire", "NAPPES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 15.0f }, { "position", 0.4f },
            { "tableB", 15.0f }, { "oscBPos", 0.65f }, { "oscBLevel", 0.5f }, { "unison", 5.0f }, { "detune", 12.0f },
            { "spread", 1.0f }, { "filterType", 0.0f }, { "cutoff", 4200.0f }, { "fltKey", 0.4f }, { "attack", 1.5f },
            { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.5f }, { "delayMode", 1.0f }, { "delaySync", 2.0f },
            { "delayMix", 0.28f }, { "delayFeedback", 0.5f }, { "delayLP", 4500.0f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.95f }, { "lfo2Rate", 0.07f }, { "lfo2Shape", 1.0f }, { "mx1Src", 8.0f },
            { "mx1Dst", 2.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.3f }, { "mx4Crv", 1.0f }, { "mx5Src", 12.0f },
            { "mx5Dst", 6.0f }, { "mx5Amt", 0.6f }, { "mx5Crv", 0.0f }, { "voiceGain", -2.0f } } },

        //======================================================================== LEADS
        { "Lame Acier", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 8.0f },
            { "position", 0.85f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.6f }, { "filterType", 1.0f },
            { "cutoff", 3500.0f }, { "resonance", 1.4f }, { "fltEnvAmount", 0.45f }, { "fltAttack", 0.005f },
            { "fltDecay", 0.3f }, { "fltSustain", 0.4f }, { "attack", 0.005f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.25f }, { "velFilter", 0.4f }, { "fltKey", 0.4f }, { "delayMode", 1.0f },
            { "delaySync", 3.0f }, { "delayMix", 0.25f }, { "delayFeedback", 0.4f }, { "delayLP", 5000.0f },
            { "reverbMix", 0.22f }, { "reverbSize", 0.6f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx1Crv", 0.0f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f }, { "mx2Amt", 0.3f }, { "mx2Crv", 0.0f },
            { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.5f }, { "mx3Crv", 1.0f }, { "mx4Src", 12.0f },
            { "mx4Dst", 6.0f }, { "mx4Amt", 0.5f }, { "mx4Crv", 0.0f }, { "voiceGain", -3.5f } } },

        { "Sync Laser", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 8.0f },
            { "position", 0.5f }, { "unison", 2.0f }, { "detune", 8.0f }, { "spread", 0.5f }, { "warpModeA", 3.0f },
            { "warpAmtA", 0.25f }, { "modDecay", 0.35f }, { "modSustain", 0.0f }, { "filterType", 1.0f },
            { "cutoff", 4000.0f }, { "resonance", 1.2f }, { "fltKey", 0.4f }, { "attack", 0.002f }, { "decay", 0.25f },
            { "sustain", 0.85f }, { "release", 0.2f }, { "reverbMix", 0.2f }, { "mx1Src", 2.0f }, { "mx1Dst", 14.0f },
            { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f }, { "mx2Crv", 0.0f },
            { "mx3Src", 10.0f }, { "mx3Dst", 14.0f }, { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.5f }, { "mx4Crv", 1.0f }, { "mx5Src", 12.0f }, { "mx5Dst", 6.0f },
            { "mx5Amt", 0.5f }, { "mx5Crv", 0.0f }, { "voiceGain", 1.5f } } },

        { "Voix Robot", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.04f }, { "tableA", 9.0f },
            { "position", 0.4f }, { "unison", 1.0f }, { "filterType", 0.0f }, { "cutoff", 9000.0f },
            { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.9f }, { "release", 0.25f }, { "chorusMix", 0.25f },
            { "reverbMix", 0.22f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f }, { "f2Res", 0.8f }, { "f2Mix", 0.9f },
            { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "lfo3Rate", 0.9f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f },
            { "mx1Dst", 22.0f }, { "mx1Amt", 0.4f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.4f },
            { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 22.0f }, { "mx3Amt", 0.7f }, { "mx3Crv", 0.0f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.3f }, { "mx4Crv", 1.0f }, { "voiceGain", -1.1f } } },

        { "Cristal FM", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 10.0f }, { "position", 0.6f }, { "tableB", 8.0f },
            { "oscBPos", 0.05f }, { "oscBLevel", 0.0f }, { "fmAmount", 0.3f }, { "unison", 2.0f }, { "detune", 6.0f },
            { "filterType", 0.0f }, { "cutoff", 7000.0f }, { "fltKey", 0.4f }, { "attack", 0.004f }, { "decay", 0.3f },
            { "sustain", 0.6f }, { "release", 0.3f }, { "modDecay", 0.3f }, { "modSustain", 0.0f },
            { "delayMode", 1.0f }, { "delaySync", 3.0f }, { "delayMix", 0.28f }, { "delayFeedback", 0.45f },
            { "delayLP", 6000.0f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f }, { "mx1Src", 2.0f },
            { "mx1Dst", 16.0f }, { "mx1Amt", 0.35f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f },
            { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 16.0f }, { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.3f }, { "mx4Crv", 1.0f }, { "mx5Src", 12.0f },
            { "mx5Dst", 6.0f }, { "mx5Amt", 0.5f }, { "mx5Crv", 0.0f }, { "voiceGain", 0.3f } } },

        { "Pluck Verre", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 17.0f }, { "position", 0.5f },
            { "filterType", 1.0f }, { "cutoff", 4000.0f }, { "resonance", 1.0f }, { "fltEnvAmount", 0.45f },
            { "fltAttack", 0.001f }, { "fltDecay", 0.2f }, { "fltSustain", 0.0f }, { "attack", 0.001f },
            { "decay", 0.35f }, { "sustain", 0.0f }, { "release", 0.25f }, { "velFilter", 0.4f }, { "fltKey", 0.4f },
            { "delayMode", 1.0f }, { "delaySync", 3.0f }, { "delayMix", 0.3f }, { "delayFeedback", 0.5f },
            { "delayLP", 5500.0f }, { "reverbMix", 0.3f }, { "reverbSize", 0.7f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 10.0f }, { "mx2Dst", 1.0f },
            { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.35f },
            { "mx3Crv", 1.0f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.4f }, { "mx4Crv", 0.0f },
            { "voiceGain", 9.2f } } },

        { "Lead Dessine", "LEADS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 1.0f }, { "glide", 0.05f }, { "tableA", 8.0f },
            { "position", 0.8f }, { "unison", 3.0f }, { "detune", 9.0f }, { "spread", 0.6f }, { "filterType", 1.0f },
            { "cutoff", 1800.0f }, { "resonance", 1.6f }, { "attack", 0.004f }, { "decay", 0.3f }, { "sustain", 0.9f },
            { "release", 0.25f }, { "reverbMix", 0.22f }, { "fltKey", 0.4f }, { "f2Type", 1.0f },
            { "f2Cutoff", 3000.0f }, { "f2Res", 0.9f }, { "f2Mix", 0.5f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f },
            { "dwRate", 1.0f }, { "dwSync", 6.0f }, { "dwSmooth", 0.4f }, { "dwSteps", 8.0f }, { "draw1", -1.0f },
            { "draw2", -0.5f }, { "draw3", 0.3f }, { "draw4", 1.0f }, { "draw5", 0.6f }, { "draw6", -0.2f },
            { "draw7", -0.8f }, { "draw8", 0.5f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.7f },
            { "mx2Src", 16.0f }, { "mx2Dst", 21.0f }, { "mx2Amt", 0.4f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f },
            { "mx3Amt", 0.5f }, { "mx3Crv", 0.0f }, { "mx4Src", 10.0f }, { "mx4Dst", 1.0f }, { "mx4Amt", 0.4f },
            { "mx4Crv", 0.0f }, { "mx5Src", 11.0f }, { "mx5Dst", 18.0f }, { "mx5Amt", 0.5f }, { "mx5Crv", 1.0f },
            { "mx6Src", 12.0f }, { "mx6Dst", 6.0f }, { "mx6Amt", 0.5f }, { "mx6Crv", 0.0f }, { "voiceGain", 3.2f } } },

        //======================================================================== CORDES
        { "Guitare Nylon", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "spread", 0.0f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "attack", 0.001f }, { "sustain", 0.0f },
            { "filterType", 0.0f }, { "fltKey", 0.5f }, { "velFilter", 0.3f }, { "rsDecay", 2.4f },
            { "rsDamp", 0.72f }, { "rsPos", 0.17f }, { "rsBurst", 2.5f }, { "rsBright", 0.5f }, { "rsStretch", 0.02f },
            { "cutoff", 4200.0f }, { "decay", 2.6f }, { "release", 0.5f }, { "noiseLevel", 0.07f },
            { "noiseType", 1.0f }, { "noiseTone", 0.4f }, { "noiseDecay", 0.015f }, { "reverbMix", 0.16f },
            { "reverbSize", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "mx2Src", 11.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.3f }, { "mx2Crv", 1.0f }, { "voiceGain", -1.2f } } },

        { "Guitare Acier", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "spread", 0.0f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "attack", 0.001f }, { "sustain", 0.0f },
            { "filterType", 0.0f }, { "fltKey", 0.5f }, { "velFilter", 0.3f }, { "rsDecay", 2.0f }, { "rsDamp", 0.5f },
            { "rsPos", 0.1f }, { "rsBurst", 1.0f }, { "rsBright", 0.85f }, { "rsStretch", 0.04f },
            { "cutoff", 9000.0f }, { "decay", 2.2f }, { "release", 0.4f }, { "noiseLevel", 0.05f },
            { "noiseType", 0.0f }, { "noiseTone", 0.7f }, { "noiseDecay", 0.01f }, { "drive", 0.1f },
            { "distType", 0.0f }, { "distAmt", 0.1f }, { "chorusMix", 0.18f }, { "reverbMix", 0.18f },
            { "reverbSize", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "mx2Src", 11.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.4f }, { "mx2Crv", 1.0f }, { "voiceGain", 2.2f } } },

        { "Harpe", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "spread", 0.0f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "attack", 0.001f }, { "sustain", 0.0f },
            { "filterType", 0.0f }, { "fltKey", 0.5f }, { "velFilter", 0.3f }, { "rsDecay", 3.4f }, { "rsDamp", 0.5f },
            { "rsPos", 0.3f }, { "rsBurst", 1.2f }, { "rsBright", 0.7f }, { "rsStretch", 0.01f },
            { "cutoff", 11000.0f }, { "decay", 3.6f }, { "release", 1.2f }, { "reverbMix", 0.35f },
            { "reverbSize", 0.7f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "voiceGain", -1.2f } } },

        { "Piano Sourd", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "spread", 0.0f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "attack", 0.001f }, { "sustain", 0.0f },
            { "filterType", 0.0f }, { "fltKey", 0.5f }, { "velFilter", 0.5f }, { "rsDecay", 5.0f }, { "rsDamp", 0.6f },
            { "rsPos", 0.12f }, { "rsBurst", 6.0f }, { "rsBright", 0.35f }, { "rsStretch", 0.22f },
            { "cutoff", 7000.0f }, { "decay", 5.0f }, { "release", 0.45f }, { "tbAmt", 0.55f }, { "tbDecay", 1.3f },
            { "tbEnd", 2.8f }, { "tbVel", 0.6f }, { "ampKey", -1.0f }, { "reverbMix", 0.22f }, { "reverbSize", 0.55f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "voiceGain", -1.6f } } },

        { "Piano Electrique", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "tableA", 12.0f }, { "position", 0.4f }, { "unison", 2.0f },
            { "detune", 4.0f }, { "spread", 0.5f }, { "filterType", 0.0f }, { "cutoff", 8000.0f }, { "fltKey", 0.5f },
            { "attack", 0.002f }, { "decay", 2.2f }, { "sustain", 0.2f }, { "release", 0.35f }, { "tbAmt", 0.75f },
            { "tbDecay", 0.7f }, { "tbEnd", 2.2f }, { "tbVel", 0.6f }, { "velFilter", 0.4f }, { "chorusMix", 0.35f },
            { "chorusRate", 0.9f }, { "chorusDepth", 0.35f }, { "reverbMix", 0.2f }, { "reverbSize", 0.5f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 10.0f },
            { "mx2Dst", 1.0f }, { "mx2Amt", 0.3f }, { "mx2Crv", 0.0f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f },
            { "mx3Amt", 0.3f }, { "mx3Crv", 1.0f }, { "mx4Src", 12.0f }, { "mx4Dst", 6.0f }, { "mx4Amt", 0.4f },
            { "mx4Crv", 0.0f }, { "voiceGain", 1.7f } } },

        { "Clavecin", "CORDES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "mono", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "spread", 0.0f }, { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "attack", 0.001f }, { "sustain", 0.0f },
            { "filterType", 0.0f }, { "fltKey", 0.5f }, { "velFilter", 0.3f }, { "rsDecay", 1.4f },
            { "rsDamp", 0.35f }, { "rsPos", 0.4f }, { "rsBurst", 0.6f }, { "rsBright", 1.0f }, { "rsStretch", 0.05f },
            { "cutoff", 12000.0f }, { "decay", 1.5f }, { "release", 0.12f }, { "reverbMix", 0.2f },
            { "reverbSize", 0.45f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "voiceGain", 2.6f } } },

        //======================================================================== PERCUS
        { "Cloche Nuit", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 3.0f }, { "rsDecay", 8.0f }, { "rsDamp", 0.25f },
            { "rsPos", 0.3f }, { "rsBurst", 0.8f }, { "rsBright", 0.85f }, { "decay", 5.0f }, { "release", 3.0f },
            { "reverbMix", 0.4f }, { "reverbSize", 0.75f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx1Crv", 0.0f }, { "voiceGain", 1.3f } } },

        { "Marimba", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 2.0f }, { "rsDecay", 1.1f }, { "rsDamp", 0.45f },
            { "rsPos", 0.25f }, { "rsBurst", 1.5f }, { "rsBright", 0.6f }, { "decay", 1.2f }, { "release", 0.3f },
            { "ampKey", -1.5f }, { "reverbMix", 0.15f }, { "reverbSize", 0.35f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "voiceGain", -2.5f } } },

        { "Vibraphone", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 1.0f }, { "rsDecay", 5.0f }, { "rsDamp", 0.2f },
            { "rsPos", 0.3f }, { "rsBurst", 2.0f }, { "rsBright", 0.55f }, { "decay", 5.0f }, { "release", 1.2f },
            { "lfoShape", 0.0f }, { "lfoRate", 5.2f }, { "chorusMix", 0.18f }, { "reverbMix", 0.3f },
            { "reverbSize", 0.6f }, { "mx1Src", 1.0f }, { "mx1Dst", 9.0f }, { "mx1Amt", 0.22f }, { "mx2Src", 9.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "voiceGain", -1.7f } } },

        { "Gong Plaque", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 5.0f }, { "rsDecay", 7.0f }, { "rsDamp", 0.15f },
            { "rsPos", 0.2f }, { "rsBurst", 14.0f }, { "rsBright", 0.4f }, { "decay", 5.0f }, { "release", 3.0f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx1Crv", 0.0f }, { "mx2Src", 11.0f }, { "mx2Dst", 18.0f }, { "mx2Amt", 0.3f }, { "mx2Crv", 1.0f },
            { "voiceGain", -1.3f } } },

        { "Bol Tibetain", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 6.0f }, { "rsDecay", 9.0f }, { "rsDamp", 0.1f },
            { "rsPos", 0.15f }, { "rsBurst", 25.0f }, { "rsBright", 0.3f }, { "decay", 5.0f }, { "release", 3.5f },
            { "reverbMix", 0.45f }, { "reverbSize", 0.8f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx1Crv", 0.0f }, { "voiceGain", -3.6f } } },

        { "Kalimba", "PERCUS", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f }, { "spread", 0.0f },
            { "rsLevel", 0.9f }, { "attack", 0.001f }, { "sustain", 0.0f }, { "filterType", 0.0f }, { "fltKey", 0.3f },
            { "cutoff", 14000.0f }, { "velFilter", 0.2f }, { "rsMat", 2.0f }, { "rsStretch", 0.3f },
            { "rsDecay", 0.7f }, { "rsDamp", 0.5f }, { "rsPos", 0.2f }, { "rsBurst", 0.5f }, { "rsBright", 0.9f },
            { "rsTune", 1200.0f }, { "decay", 0.8f }, { "release", 0.2f }, { "reverbMix", 0.2f },
            { "reverbSize", 0.4f }, { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f },
            { "voiceGain", 3.6f } } },

        //======================================================================== TEXTURES
        { "Pluie Acide", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.2f }, { "tableA", 15.0f },
            { "position", 0.5f }, { "unison", 1.0f }, { "noiseLevel", 1.0f }, { "noiseType", 1.0f },
            { "noiseTone", 0.5f }, { "filterType", 3.0f }, { "cutoff", 2200.0f }, { "resonance", 3.2f },
            { "attack", 1.0f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.5f }, { "reverbMix", 0.5f },
            { "reverbSize", 0.9f }, { "dwRate", 0.3f }, { "dwSync", 0.0f }, { "dwSmooth", 0.6f }, { "dwSteps", 8.0f },
            { "draw1", -1.0f }, { "draw2", 0.8f }, { "draw3", -0.4f }, { "draw4", 1.0f }, { "draw5", -0.9f },
            { "draw6", 0.2f }, { "draw7", 0.6f }, { "draw8", -0.7f }, { "mx1Src", 16.0f }, { "mx1Dst", 3.0f },
            { "mx1Amt", 0.6f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f },
            { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f },
            { "mx4Dst", 18.0f }, { "mx4Amt", 0.25f }, { "mx4Crv", 1.0f }, { "voiceGain", 12.0f } } },

        { "Usine Lointaine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.3f }, { "tableA", 7.0f },
            { "position", 0.4f }, { "unison", 1.0f }, { "noiseLevel", 1.0f }, { "noiseType", 2.0f },
            { "noiseTone", 0.35f }, { "filterType", 5.0f }, { "cutoff", 180.0f }, { "resonance", 3.0f },
            { "attack", 1.2f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 3.0f }, { "reverbMix", 0.55f },
            { "reverbSize", 0.9f }, { "lfo3Rate", 0.06f }, { "lfo3Shape", 1.0f }, { "mx1Src", 14.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.25f }, { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.45f },
            { "mx2Crv", 0.0f }, { "mx3Src", 10.0f }, { "mx3Dst", 1.0f }, { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f },
            { "mx4Src", 11.0f }, { "mx4Dst", 18.0f }, { "mx4Amt", 0.25f }, { "mx4Crv", 1.0f }, { "voiceGain", 7.7f } } },

        { "Respiration", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "noiseLevel", 0.7f }, { "noiseType", 1.0f }, { "noiseTone", 0.55f }, { "filterType", 0.0f },
            { "cutoff", 3000.0f }, { "attack", 1.4f }, { "decay", 0.5f }, { "sustain", 1.0f }, { "release", 2.0f },
            { "reverbMix", 0.4f }, { "reverbSize", 0.7f }, { "f2Type", 5.0f }, { "f2Cutoff", 1000.0f },
            { "f2Res", 0.8f }, { "f2Mix", 1.0f }, { "f2Vowel", 0.0f }, { "f2Mode", 0.0f }, { "lfo3Rate", 0.2f },
            { "lfo3Shape", 0.0f }, { "mx1Src", 14.0f }, { "mx1Dst", 22.0f }, { "mx1Amt", 0.6f }, { "mx2Src", 14.0f },
            { "mx2Dst", 3.0f }, { "mx2Amt", 0.2f }, { "mx3Src", 9.0f }, { "mx3Dst", 3.0f }, { "mx3Amt", 0.45f },
            { "mx3Crv", 0.0f }, { "mx4Src", 10.0f }, { "mx4Dst", 22.0f }, { "mx4Amt", 0.5f }, { "mx4Crv", 0.0f },
            { "mx5Src", 11.0f }, { "mx5Dst", 18.0f }, { "mx5Amt", 0.25f }, { "mx5Crv", 1.0f }, { "voiceGain", 12.0f } } },

        { "Vent Solaire", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "noiseLevel", 1.0f }, { "noiseType", 0.0f }, { "noiseTone", 0.5f }, { "filterType", 3.0f },
            { "cutoff", 1500.0f }, { "resonance", 2.2f }, { "attack", 2.0f }, { "decay", 0.5f }, { "sustain", 1.0f },
            { "release", 3.0f }, { "reverbMix", 0.6f }, { "reverbSize", 0.95f }, { "lfo4Rate", 0.05f },
            { "lfo4Shape", 0.0f }, { "width", 1.4f }, { "mx1Src", 15.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f },
            { "mx2Src", 9.0f }, { "mx2Dst", 3.0f }, { "mx2Amt", 0.5f }, { "mx2Crv", 0.0f }, { "mx3Src", 10.0f },
            { "mx3Dst", 15.0f }, { "mx3Amt", 0.4f }, { "mx3Crv", 0.0f }, { "mx4Src", 11.0f }, { "mx4Dst", 18.0f },
            { "mx4Amt", 0.2f }, { "mx4Crv", 1.0f }, { "voiceGain", 12.0f } } },

        { "Cables Metal", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.5f }, { "tableA", 10.0f },
            { "position", 0.3f }, { "tableB", 8.0f }, { "oscBPos", 0.1f }, { "oscBOct", 1.0f }, { "oscBLevel", 0.0f },
            { "ringAmount", 0.5f }, { "unison", 1.0f }, { "rsLevel", 0.7f }, { "rsMat", 5.0f }, { "rsDecay", 2.5f },
            { "rsDamp", 0.2f }, { "rsFeed", 0.6f }, { "rsDry", 0.5f }, { "rsBurst", 3.0f }, { "rsBright", 0.6f },
            { "filterType", 0.0f }, { "cutoff", 6000.0f }, { "attack", 0.6f }, { "decay", 0.5f }, { "sustain", 1.0f },
            { "release", 1.8f }, { "reverbMix", 0.35f }, { "reverbSize", 0.7f }, { "mx1Src", 9.0f },
            { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 10.0f }, { "mx2Dst", 17.0f },
            { "mx2Amt", 0.3f }, { "mx2Crv", 0.0f }, { "mx3Src", 11.0f }, { "mx3Dst", 18.0f }, { "mx3Amt", 0.3f },
            { "mx3Crv", 1.0f }, { "voiceGain", 2.1f } } },

        { "Echo Machine", "TEXTURES", {
            { "lfoDepth", 0.0f }, { "modAmount", 0.0f }, { "oscALevel", 0.0f }, { "unison", 1.0f },
            { "rsLevel", 0.9f }, { "rsMat", 0.0f }, { "rsDecay", 1.8f }, { "rsDamp", 0.6f }, { "rsPos", 0.2f },
            { "rsBurst", 1.5f }, { "rsBright", 0.7f }, { "rsStretch", 0.05f }, { "attack", 0.001f }, { "decay", 1.9f },
            { "sustain", 0.0f }, { "release", 0.4f }, { "filterType", 1.0f }, { "cutoff", 2600.0f },
            { "resonance", 1.2f }, { "fltEnvAmount", 0.3f }, { "fltDecay", 0.4f }, { "fltSustain", 0.0f },
            { "fltKey", 0.5f }, { "delayMode", 1.0f }, { "delaySync", 2.0f }, { "delayMix", 0.4f },
            { "delayFeedback", 0.55f }, { "delayLP", 4200.0f }, { "reverbMix", 0.25f }, { "reverbSize", 0.6f },
            { "mx1Src", 9.0f }, { "mx1Dst", 3.0f }, { "mx1Amt", 0.5f }, { "mx1Crv", 0.0f }, { "mx2Src", 11.0f },
            { "mx2Dst", 18.0f }, { "mx2Amt", 0.3f }, { "mx2Crv", 1.0f }, { "voiceGain", 0.0f } } }
    };

    return presets;
}
