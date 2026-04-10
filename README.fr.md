# VOID (Platform Abstraction Layer)

*Lire dans d'autres langues : [English](README.md)*

**VOID** est une bibliothèque statique écrite en C pur (C11/C17) agissant comme une couche d'abstraction matérielle (Platform Abstraction Layer - PAL). Elle a été conçue pour servir de fondation "Back-end" au moteur **SOUL**, mais peut être utilisée dans n'importe quel projet nécessitant un accès bas niveau performant.

Sa philosophie principale est l'**Isolation Binaire Strict** : l'utilisateur de la bibliothèque ne voit jamais les dépendances sous-jacentes et n'interagit qu'avec un contrat d'interface minimal et prédictible (`void.h`).

---

## Principes d'Architecture

- **Zéro-Allocation Cachée** : Remplace les appels erratiques à `malloc/free` par un système d'Arènes pour des performances d'allocation instantanées et une cohérence de cache maximale.
- **Opaque par Défaut** : Tous les concepts matériels (Fenêtres, Contextes de rendu, Textures) sont masqués derrière des pointeurs opaques (`VoidWindow`, `VoidTexture`).
- **Data-Oriented Ready** : Les types primitifs sont strictement définis (`uint32`, `uint64`), garantissant la portabilité des structures de données (SoA) sur n'importe quel CPU.

---

## Contenu du SDK

Le projet compile en une bibliothèque statique autonome. L'API complète est exposée via un unique fichier d'en-tête :

- `include/void.h` : L'interface publique (Le Contrat).
- `lib/<os>-<arch>/libvoid.a` : Le binaire compilé à lier statiquement.

---

## Compilation

VOID utilise un `Makefile` robuste supportant la cross-compilation et générant à la fois la bibliothèque et une suite de tests.

### Prérequis
- `gcc` ou `mingw-w64` (pour Windows)
- `ar` (Archiver)
- `libsdl2-dev`, `libsdl2-image-dev`

### Commandes de Build

```bash
#Compiler la bibliothèque (libvoid.a) et l'exécutable de test pour l'OS actuel :
make

#Compiler en mode Debug :
make BUILD=debug

#Afficher les informations de la toolchain (très utile pour le diagnostic) :
make info

#Nettoyer l'espace de travail :
make fclean
```

### Cross-Compilation (ex: Windows depuis Linux)
```bash
make OS=windows ARCH=x86_64
```

---

## Utilisation du Banc de Test

Un programme de test indépendant est généré dans le dossier `bin/`. Il permet de stresser les différents modules isolément sans avoir besoin du moteur Front-end.

```bash
./bin/linux-x86_64/void [test_name]
```

Arguments disponibles :
- `memory` : Teste l'allocation de blocs persistants et le rollback des arènes de frames.
- `thread` : Teste les opérations atomiques en saturant les cœurs logiques détectés.
- `render` : Lance une boucle de rendu visuelle, validant le delta time et l'indépendance de l'affichage.
- `all` : Lance la suite complète.

---

## Intégration dans un projet externe (C/C++)

Pour utiliser **VOID** dans un projet supérieur, il suffit d'inclure le header et de lier la bibliothèque statique :

```makefile
# Exemple d'intégration Makefile
CFLAGS  += -I/chemin/vers/void/include
LDFLAGS += -L/chemin/vers/void/lib/linux-x86_64
LDLIBS  += -lvoid -lSDL2 -lSDL2_image -lm -pthread
```

*Note: Bien que le header `void.h` cache la SDL2, l'éditeur de liens du projet final aura besoin des dépendances système de la SDL pour résoudre les symboles de `libvoid.a`.*