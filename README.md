<div align="center">

# MS1
### WAVETABLE SYNTHESIS · SOUND DESIGN EXPERIMENT · JUCE / VST3

![Status](https://img.shields.io/badge/STATUS-EXPERIMENTAL_TEST_BUILD-EF343E?style=for-the-badge&labelColor=10151E)
![JUCE](https://img.shields.io/badge/ENGINE-C%2B%2B%20%2F%20JUCE-19B9FF?style=for-the-badge&labelColor=10151E)
![Format](https://img.shields.io/badge/FORMAT-VST3-F5C343?style=for-the-badge&labelColor=10151E)

**ANKS LABS // EXPERIMENTAL AUDIO DEVELOPMENT**

[![Build artifacts](https://img.shields.io/badge/GITHUB_ACTIONS-BUILD_ARTIFACTS-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star/MS1/actions)

</div>

---

> **Current status: experimental / test build.** MS1 is a development and sound-design prototype, not a finished or stable product. Compilation artifacts depend on successful GitHub Actions runs and their retention period.

## 01 // THE IDEA

**MS1** started with a sound-design question: what would an original experimental synthesizer feel like if it explored some of the *musical approaches* associated with both **Massive** and **Serum** — modulation, oscillator layering, wavetable movement and quick experimentation — within a new, compact instrument?

The goal is **not** to reproduce those synthesizers or copy their proprietary code or interfaces. MS1 is an independent **C++ / JUCE** learning and experimentation project developed within **ANKS Labs**.

## 02 // FEATURES PRESENTED IN THE PROJECT

| Area | Function |
|:--|:--|
| **Sound source** | Wavetable synthesis |
| **Oscillator voicing** | Unison |
| **Filtering** | Filter module |
| **Modulation** | LFO and modulation envelope |
| **Rhythm** | Step sequencer |
| **Sound management** | Preset bank |
| **Plugin format** | VST3 via JUCE |

The precise synthesis behavior, usability and compatibility remain subject to further development and testing.

## 03 // TEST BUILDS

Builds are generated through **[GitHub Actions](https://github.com/olivershadd-star/MS1/actions)**. When a build succeeds and the artifact is still available, the workflow can provide an `MS1.vst3` file in its **Artifacts** section.

An Actions artifact is a **test result**, not necessarily a vetted release.

## 04 // DEVELOPMENT DIRECTION

- [x] Establish an initial JUCE VST3 synth architecture
- [x] Integrate wavetable, unison, filter and modulation concepts in the project
- [x] Add step sequencer and preset-bank functionality
- [ ] Continue sound-design and usability experiments
- [ ] Validate the plugin in a real DAW on the target Windows setup
- [ ] Publish a release only after the intended testing criteria are met

## 05 // CODE & RIGHTS

**ANKS Labs** experimental project. Making a repository public does not by itself grant a licence to reuse its code; check the repository's licence and the applicable **JUCE** terms before redistribution.

**Massive**, **Serum** and related brand names belong to their respective owners. MS1 is not affiliated with those products.

---

<div align="center">

**BUILD. BREAK. REWIRE. LISTEN.**

[![ANKS LABS](https://img.shields.io/badge/ANKS_LABS-PROFILE-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star)

</div>
