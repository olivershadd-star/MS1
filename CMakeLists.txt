cmake_minimum_required(VERSION 3.22)
project(MS1 VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MS1
    COMPANY_NAME "ANKS Lab"
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
    Source/FxRack.cpp)

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
    Source/FxRack.cpp)
set_target_properties(PresetCheck PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_definitions(PresetCheck PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(PresetCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
