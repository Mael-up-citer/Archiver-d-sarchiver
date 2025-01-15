# Archiver/Désarchiveur

## Description
Ce projet, réalisé lors du **premier semestre de L2**, est un script en **Bash** qui permet d'archiver et de désarchiver des fichiers ou répertoires en suivant un principe inspiré des **poupées russes**. Il utilise les outils `uuencode` et `uudecode` pour l'encodage et le décodage couplé avec un autre algorithme basée sur le décalage des bits, et repose sur un système générant dynamiquement des fichiers C et leurs binaires associés.

Le script supporte des chemins ou noms de fichiers contenant des espaces, à condition de les entourer de guillemets (`""`).

De plus dans le dossier EncodageDecodage vous trouverez une implémentation des algorithmes utilisé par l'archiveur

---

## Fonctionnalités
1. **Archiver :** 
   - L'archivage est initié avec la commande :
     ```bash
     ./create-ball-myencode.sh chemin/vers/fichier_ou_dossier
     ```
   - Cela génère :
     - Un script Bash nommé `my-ball-encode.sh`.
     - Un fichier C dédié à l'encodage.
     - Le binaire associé pour effectuer l'encodage.

2. **Désarchiver :** 
   - Le désarchivage est initié avec la commande :
     ```bash
     ./my-ball-encode.sh
     ```
   - Cela génère :
     - Un fichier C pour décoder l'archive.
     - Le binaire associé pour reconstituer l'arborescence d'origine.
     - La réstitution de l'archive

---

## Prérequis
Pour exécuter ce projet, vous devez disposer de :
1. Un système Linux ou macOS (ou tout système compatible Bash).
2. Les outils suivants installés :
   - `bash`
   - `encode` et `uudecode`.
   - Un compilateur C (`gcc` ou similaire) pour compiler les fichiers générés.