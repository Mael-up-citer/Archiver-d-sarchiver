Programme de Décodage Binaire et UUEncode en C

Cette partie contient deux programmes en C qui permettent de traiter des fichiers encodés de manière binaire. L'un décode des fichiers encodés avec une méthode de décalage binaire personnalisée et une possible sur couche avec uuencode, et l'autre traite ces fichiers encodés. Les deux programmes offrent des options variées pour interagir avec des fichiers ou des flux d'entrée/sortie.

---------

Fonctionnalités principales:

encodage binaire personnalisé: Le programme encode un fichier ou un flux binaire en utilisant des manipulations de bits pour convertir les blocs de données en caractères ASCII.

Décodage binaire personnalisé: Le programme décode un fichier ou un flux binaire en utilisant des manipulations de bits pour convertir les blocs de données encodés.

Support du mode -uu pour uuencode et uudecode : Le programme peut interagir avec un processus uuencode et uudecode via un pipe pour effectuer des décryptages UUEncode.

Compatible tout type de fichier.

Utilisation comme filtre possible.

--------

Les deux fichiers principaux:

bonus_encode.c : Permet de d'encoder des fichiers ou flux avec une méthode de décalage des bits et une possible surcouche uuencode.

bonus_decode.c : Permet de décoder des fichiers ou flux encodés de manière binaire personnalisée en utilisant des décalages de bits.

decode.c et encode.c s'utilisent avec 2 args, un fihier input et un output

filtre_encode et filtre_decode fonctionne comme bonus_encode sauf que l'option -uu n'est pas suporté


--------

Comment utiliser ce programme

Prérequis:
Le programme est écrit en C et nécessite un compilateur C pour être exécuté (par exemple, gcc).
Si vous souhaitez utiliser le mode -uu, un environnement compatible avec la commande uudecode est nécessaire.

Compilation:
Pour compiler le programme, exécutez la commande suivante dans le terminal :

gcc -o encoder_program bonus_encode.c
gcc -o decoder_program bonus_decode.c

Les programmes accepte entre 1 et 4 arguments pour déterminer le mode d'entrée et de sortie. Voici les différentes façons d'exécuter les programmes:

Utiliser le mode -uu pour traiter avec un pipe, les mêmes règles de parametres sont appliqué
  ./decode_program -uu input_file.txt output_file.txt


Exécution de encoder_program:

1. Encoder un fichier d'entrée et produire un fichier de sortie
  ./encoder_program input_file output_file

2. Encoder le flux d'entrée et envoyer le resultat dans un fichier de sortie (si saisie manuelle faite ctrl d pour finir la chaine)
  ./encoder_program output_file

3. Encoder depuis l'entrée standard et afficher le résultat sur la sortie standard (si saisie manuelle faite ctrl d pour finir la chaine)
  ./encoder_program


Exécution de decoder_program:

1. Décoder un fichier d'entrée et produire un fichier de sortie
  ./decoder_program input_file output_file

2. Décoder depuis l'entrée standard et produire un fichier de sortie
  ./decoder_program output_file

3. Décoder depuis l'entrée standard et afficher le résultat sur la sortie standard
  ./decoder_program


utilisation combiné:
  echo salut | ./encoder_program | ./decoder_program out

Utilisation avec -uu pareil sauf qu'on mets -uu avant les fichiers

  ex: echo salut | ./encoder_program -uu | ./decoder_program -uu  out