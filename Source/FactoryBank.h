#pragma once
#include <juce_audio_utils/juce_audio_utils.h>

// Banque de presets d'usine : un preset = un nom, une catégorie et une liste de réglages (id du paramètre, valeur).
// Les paramètres non cités reprennent leur valeur par défaut. Pour ajouter un son, il suffit d'ajouter une
// entrée dans FactoryBank.cpp, dans le bloc de sa catégorie.
struct FactoryPreset
{
    const char* name;
    const char* category;
    std::vector<std::pair<const char*, float>> values;
};

const std::vector<FactoryPreset>& factoryPresets();
