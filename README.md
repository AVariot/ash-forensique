# ash-forensique

Outil forensique en C pour la recuperation de fichiers supprimes depuis un peripherique de stockage brut. Le programme scanne les secteurs d'un disque a la recherche de signatures binaires (magic bytes) et reconstruit les fichiers effaces.

## Formats supportes

| Format | Detection | Fin de fichier |
|--------|-----------|----------------|
| JPEG   | `FF D8 FF` | `FF D9` |
| PNG    | `89 50 4E 47` | `IEND` chunk |
| PDF    | `%PDF` | `%%EOF` |
| WEBP   | `RIFF...WEBP` | Taille lue dans le header |

## Fonctionnement

Le programme lit le peripherique secteur par secteur (512 octets) en une seule passe **O(N)** :

1. Chaque secteur est analyse a la recherche de magic bytes
2. Quand un header est detecte, un fichier de sortie est cree
3. Les donnees sont ecrites jusqu'a la detection du footer (ou atteinte de la taille pour WEBP)
4. Le fichier est ferme et le scan continue

> **Note** : L'algorithme ne gere pas la fragmentation. Seuls les fichiers stockes de maniere contigue sur le disque peuvent etre recuperes.

## Compilation

```bash
# Construire l'executable
make

# Construire en bibliotheque partagee
make lib

# Nettoyer
make clean    # supprime les .o
make fclean   # supprime tout (objets + binaire + lib)
make re       # reconstruction complete
```

## Utilisation

```bash
./getLostData <peripherique> <dossier_sortie>
```

```bash
# Exemple : recuperer les fichiers d'une cle USB
sudo ./getLostData /dev/sdb ./recovered
```

Les fichiers recuperes sont nommes `fichier_0.jpeg`, `fichier_1.png`, etc.

## Structure du projet

```
.
├── main.c                   # Point d'entree
├── main.h
├── Makefile
├── include/
│   ├── elements/
│   │   └── element.h        # Signatures binaires des formats
│   └── sys/
│       └── linux/
│           └── file.h       # Interface I/O Linux
└── src/
    ├── elements/
    │   └── element.c        # Detection de type et fin de fichier
    └── sys/
        └── linux/
            └── file.c       # Algorithme de carving (read_metal)
```

## Details techniques

- **Lecture par secteurs** : blocs de 512 octets (taille standard d'un secteur disque)
- **Validation JPEG** : taille minimum de 50 Ko pour filtrer les faux positifs
- **WEBP** : la taille est extraite du header RIFF au lieu d'un footer
- **Zero dependance externe** : uniquement la libc standard
- **Compilation stricte** : `-Wall -Wextra -Werror`
