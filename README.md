<div align="center">

# MS1
### Synthé à table d'onde · VST3 & Standalone · Windows 64 bits

![Statut](https://img.shields.io/badge/STATUT-VERSION_DE_TEST-EF343E?style=for-the-badge&labelColor=10151E)
![JUCE](https://img.shields.io/badge/MOTEUR-C%2B%2B%20%2F%20JUCE%208-19B9FF?style=for-the-badge&labelColor=10151E)
![Format](https://img.shields.io/badge/FORMAT-VST3-F5C343?style=for-the-badge&labelColor=10151E)
![Gratuit](https://img.shields.io/badge/GRATUIT-OUI-42C77A?style=for-the-badge&labelColor=10151E)

**ANKS Labs**

[![Builds](https://img.shields.io/badge/GITHUB_ACTIONS-TELECHARGER_LE_BUILD-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star/MS1/actions)

<img src="docs/MS1_interface.png" alt="Interface et caractéristiques de MS1" width="800">

</div>

---

> **Statut : version de test.** MS1 est un synthétiseur complet mais encore en développement. Il est fait pour être joué, tordu et massacré (techno, indus, sound design), pas pour être « propre ». Des bugs et des niveaux à ajuster sont possibles : les retours sont les bienvenus.

## Présentation

MS1 est un synthé à table d'onde, **léger et gratuit**, pensé pour faire vite des sons puissants dans Ableton Live (ou n'importe quelle autre hôte VST3). Il s'inspire de l'esprit de Massive et de Serum (oscillateurs, modulation partout, effets) mais reste compact, avec des menus plutôt qu'un mur de potards.

Il est écrit en C++ avec JUCE par **ANKS Labs**, sans reprendre ni code ni interface d'un autre produit.

## Ce que fait MS1

### Sources
- **2 oscillateurs à table d'onde** (A et B), avec tables d'usine et **tables utilisateur** (fichiers WAV).
- **Sub**, **bruit**, et **corde** (Karplus-Strong).
- **FM** et **modulation en anneau** entre A et B.
- **Unison**, **drive** avant filtre.
- **Resample** : MS1 s'enregistre lui-même et réinjecte le résultat comme table d'onde.

### Filtres
- **Filtre 1** et **filtre 2**, en série ou en parallèle.
- Le filtre 2 a un mode **formants** (voyelles).
- **Distorsion par voix** après les filtres.

### Modulation
- **Enveloppe d'amplitude** et **enveloppe de modulation**.
- **4 LFO** (formes, vitesse libre ou synchro).
- **LFO dessiné** : tu traces la forme à la souris (32 points), avec formes prédéfinies et synchro.
- **4 macros**.
- **Matrice de modulation à 12 slots** : 17 sources vers 23 cibles.

### Effets
- Chaîne globale : **chorus / flanger → phaser → delay → reverb → EQ 8 bandes**.
- **Rack FX à 4 emplacements** (ordre modifiable) avec 20 effets :

| Famille | Effets |
|:--|:--|
| Dégradation et couleur | Bitcrusher, Lo-Fi, Waveshaper, Exciter |
| Modulation | Ring mod, Décaleur de fréquence, Rotary, Pan/Trémolo, Autowah |
| Filtres et résonances | Résonateur, Formants, Bande |
| Rythme et espace | Gate, Stutter, Shimmer, Imager |
| Dynamique | Limiteur, Compresseur, Glue, Maximizer |

- Fin de chaîne : compresseur, largeur stéréo, master, limiteur.

### Jeu
- **Séquenceur pas à pas** (jusqu'à 64 pas, 4 mesures).
- **213 presets d'usine**, niveaux calibrés.
- **MIDI learn** sur n'importe quel potard.

## Installation

1. Ouvre l'onglet **[Actions](https://github.com/olivershadd-star/MS1/actions)** du dépôt.
2. Choisis le dernier build vert (workflow **Build VST3**).
3. Télécharge l'artefact **MS1-VST3-Windows** (il faut être connecté à GitHub).
4. Décompresse et copie le dossier `MS1.vst3` dans `C:\Program Files\Common Files\VST3`.
5. Relance la détection des plugins dans ton hôte (Ableton : *Préférences → Plug-ins → Rescan*). Le plugin apparaît sous l'éditeur **ANKS Labs**.

Un artefact est un build de test : il expire au bout de quelques semaines. Prérequis : Windows 64 bits, hôte compatible VST3. Le mode Standalone est aussi compilé.

## Raccourcis et ergonomie

Survole un potard avec la souris : sa **valeur réelle** s'affiche dans la barre du bas.

| Action | Effet |
|:--|:--|
| `Haut` / `Bas` | Une unité (1, 0,1 ou 0,01 selon l'étendue). `Maj` : x10 |
| `Gauche` / `Droite` | Réglage fin : 0,25 % de la course. `Maj` : 2,5 % |
| Clic droit sur un potard | Entrer une valeur, valeur par défaut, **MIDI learn**, modulation rapide (macros, LFO, enveloppes) |
| `Tab` / `Maj+Tab` | Menu suivant / précédent |
| `Page haut` / `Page bas` | Preset précédent / suivant |
| `Echap` | Annule un MIDI learn |
| `<` `>` dans le rack FX | Changer l'ordre des 4 emplacements |

**MIDI learn** : clic droit sur un potard, puis bouge un contrôleur MIDI. Le CC est mémorisé dans `ANKS Labs/MS1/midimap.txt` et vaut pour tous les projets.

Dans certains hôtes, le clavier de l'ordinateur ne parvient pas toujours à la fenêtre du plugin. Clique d'abord dans l'interface.

## Pour les développeurs

- Code dans `Source/`, projet CMake (JUCE 8.0.4, récupéré à la compilation).
- Compilation locale : `cmake -B build -A x64` puis `cmake --build build --config Release`.
- Workflows GitHub Actions :
  - **Build VST3** : compile le plugin.
  - **Preset check** : mesure chaque preset (niveau, silence, saturation).
  - **Snapshot** : génère des captures de l'interface.
- Les presets d'usine sont générés dans `Source/FactoryBank.cpp`.

## Droits et licence

Projet **ANKS Labs**, distribué librement et gratuitement. Rendre un dépôt public n'accorde pas à lui seul de licence de réutilisation : vérifie la licence du dépôt et les conditions de **JUCE** avant toute redistribution. *Massive*, *Serum* et les noms associés appartiennent à leurs propriétaires ; MS1 n'a aucun lien avec ces produits.

---

<div align="center">

**BUILD. BREAK. REWIRE. LISTEN.**

[![ANKS LABS](https://img.shields.io/badge/ANKS_LABS-PROFIL-19B9FF?style=for-the-badge&labelColor=10151E)](https://github.com/olivershadd-star)

</div>
