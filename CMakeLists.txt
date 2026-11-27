cmake_minimum_required(VERSION 3.22)
project(MS1 VERSION 0.9.0)

# Numéro de build (fourni par GitHub Actions) affiché dans le plugin
set(MS1_BUILD 0 CACHE STRING "Numéro de build")
add_compile_definitions(MS1_VERSION_TEXT="v${PROJECT_VERSION} b${MS1_BUILD}")

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MS1
    COMPANY_NAME "ANKS Labs"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    PLUGIN_MANUFACTURER_CODE Anks
    PLUGIN_CODE Ms01
    FORMATS VST3 Standalone
    PRODUCT_NAME "MS1")

target_sources(MS1 PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/FactoryBank.h
    Source/FactoryBank.cpp
    Source/FxRack.h
    Source/FxRack.cpp
    Source/WavetableData.cpp)

target_compile_definitions(MS1 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MS1
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

# Outil de mesure des presets (n'est pas construit par défaut : cible PresetCheck)
juce_add_console_app(PresetCheck PRODUCT_NAME "PresetCheck")
target_sources(PresetCheck PRIVATE
    Source/PresetCheck.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/FactoryBank.cpp
    Source/FxRack.cpp
    Source/WavetableData.cpp)
set_target_properties(PresetCheck PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_definitions(PresetCheck PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(PresetCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

# Captures d'écran de l'interface (documentation)
juce_add_console_app(Snapshot PRODUCT_NAME "Snapshot")
target_sources(Snapshot PRIVATE
    Source/Snapshot.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/FactoryBank.cpp
    Source/FxRack.cpp
    Source/WavetableData.cpp)
set_target_properties(Snapshot PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_definitions(Snapshot PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_MODAL_LOOPS_PERMITTED=1)
target_link_libraries(Snapshot PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

# Outil de diagnostic du moteur (mesures hors ligne)
juce_add_console_app(Diag PRODUCT_NAME "Diag")
target_sources(Diag PRIVATE
    Source/Diag.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/FactoryBank.cpp
    Source/FxRack.cpp
    Source/WavetableData.cpp)
set_target_properties(Diag PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_definitions(Diag PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(Diag PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
