#!/bin/bash
#Test des arguments
test $# -lt 1 && echo 'Ce prog prend au moins 1 argument' && exit 1
#Initialisation de my-ball-encode.sh
if test "$1" != "-r@$"
then
cat << TAG_ENCODE > filtre_encode.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

//encode les char 
void encode(char ligne[], char oct[8]){
    int c=4;    //position de décalage
    int umask = 0;
    int courant = 0;
    int k = 0;
    int comp = 0;
    //init le tab
    oct[courant] &= umask;
    oct[courant+1] &= umask;
    
    //parcours les 5 chars
    while(k != 5){
        //parcour le char courant et le met dans le bonne endroit
        for(int i=(sizeof(char)*8)-1; i>=0; i--){
            // change de case du tab et reinit les var 
            if(comp == 10){
                c = 4 ;
                comp = 0;
                courant = courant + 2;  //traite les cases 2 par 2 donc decale 2 par 2
                oct[courant] &= umask;
                oct[courant+1] &= umask;
            }
            //les b impaires vont dans le tab courant
            if(i % 2 == 1){
                //si le b vaut 1 on fait le traitement sinon "on fait rien" 
                if((ligne[k]>>i & 1) == 1){
                    umask++;    // = 0000 0001 apres on decal le 1
                    umask <<= c;
                    oct[courant] |= umask;    //copie la val dans le tab
                    //printf(">>>bin a= %d \n", umask);
                    //printf(">>>decalage a= %d \n", c);
                    umask = 0;
                }
            }
            //les b paires vont dans le tab courant +1
            else{
                if((ligne[k]>>i & 1) == 1){
                    umask++; 
                    umask <<= c;
                    oct[courant+1] |= umask;
                    //printf(">>>bin b= %d \n", umask);
                    //printf(">>>decalage b= %d \n", c);
                    umask = 0;
                }
                c--;    //ajuste la positions de decalage
            }
        comp++; //comp compt quand faut changer de col
        }
    k++;    //change de char
    }

}

//prend des val entre 0 et 31 passe de a->z || 0->5
void associer(FILE* out, char oct[]){
    for(int j = 0; j < 8; j++ ){
        //si char entre O 25 -> a z
        if(oct[j] < 26){
            fprintf(out, "%c", oct[j] + 97 );  //add 97 pour atteindre 'a'
            }
        //sinon de 26 31 -> 0 5
        else 
        fprintf(out, "%c", oct[j] + (48-26));   //add 48-26 pour atteindre '0'
        }
}


int main(int argc, char *argv[]){
    //test le nb d'arg
    if(argc < 1 || argc > 4){
        printf(" Ce prog prends 1-4 arg");
        return 1;
    }

    FILE* input;    //entré du prog
    FILE* out;  //sortie du pipe ou du prog le cas echeant
   
    FILE *res;   // sorie du prog
    int t[2];   //pipe ou t[0] = lecture   et t[1] = ecriture
    int r = 1;
    int opt = 1;
    int pipmod = 0;

    if(argc > 1)
        opt = strcmp(argv[1], "-uu");   //test si on a "-uu" si oui opt = 0 sinon opt = 1 || -1

        //pour tous les test si on ouvre un fichier on le test et ainsi de suite
        if(argc == 3 && opt){
        input = fopen(argv[1], "rb");
        //test si ca a marcher
        if (input == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
            return 2;
        }
        //ouvre le fichier resultat
        out = fopen(argv[2], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
            return 3;
        }
    }
    else if(argc == 2 && opt){
        input = stdin;  
        //ouvre le fichier resultat
        out = fopen(argv[1], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
            return 3;
        }
    }
    else if(opt && argc == 1){
        input = stdin;
        out = stdout;
    }

    //test pour le pipe
    else if(!opt && argc == 4){
        input = fopen(argv[2], "rb");
        res = fopen(argv[3], "w");
    }
    else if(!opt && argc == 3){
        input = fopen(argv[2], "rb");
        if(input == NULL){
            printf("impossible d'ouvrir %s \n", argv[2]);
            return 2;
        }
        res = stdout;
    }
    else if(!opt && argc == 2){
        input = stdin;
        res = stdout;
    }
    else{
        printf("no matching case \n");
        return 5;
    }

    //cree le pipe et execute le code du pere
    if(! opt){
        out = stdout;   //pour ecrire dans le pipe
        //out = fdopen(t[1], "w");    //si on a -uu on veut ecrire le resultat dans le pipe ecrit donc dans le descripteur write
        pipmod = 1;
        pipe(t);    //cree le pipe
        r = fork();     //cree le processus fils
        if(r == -1){
            printf("echec de la creation du processus");
            return 4;
        }
        //met en place le pipe
        if(r != 0){
            // Ferme le descripteur de fichier en lecture du pipe (reading end)
            close(t[0]);
            // Ferme le descripteur de fichier standard de sortie (stdout)
            close(1);
            // Duplique le descripteur de fichier en écriture du pipe (writing end) sur le descripteur de fichier standard de sortie (stdout)
            dup(t[1]);
            // Ferme le descripteur de fichier en écriture du pipe (writing end) après la duplication
            close(t[1]);
        }
    }
    //code du pere a faire si c'est le pere ou si on a pas -uu
    if(r){
        //***************meme code que encode.c******************//
        //init les var
        char ligne[6];
        char oct[8];
        int mask = 0;
        int nb;
        int bourage = 0;
        int secu = 0;
    
        //boucle principale qui recupere 5 char et les traitent 
        while((nb = fread(ligne, 1, 5, input)) > 0){
            if(nb != 5){
                ligne[nb] = 1;
                ligne[nb] <<= 7;
                for(int i = nb + 1; i < 5; i++)
                    ligne[i] &= mask ;      //remplie les car manquant par 0000 0000
                bourage = 5-nb;
            }
            ligne[5] = '\0'; // Ajout du caractère nul pour former une chaîne de caractères valide
            encode(ligne, oct);
            associer(out, oct); 
            secu = 1;
        }
        if(secu)
            fprintf(out, "%d", bourage);

        //***************meme code que encode.c******************//
        if(pipmod) {
        fclose(out);
        wait(NULL); // Le père attend que le fils termine avant de continuer
        }
        fclose(input);

        return 0;
    }


    // instruction du fils
    if(r == 0){
        close(t[1]);
        // Ferme le descripteur de fichier en lecture standard (stdin)
        close(0);
        // Duplique le descripteur de fichier du pipe en lecture sur stdin
        dup2(t[0], 0);
        // Ferme le descripteur de fichier en lecture du pipe (il n'est plus nécessaire)
        close(t[0]);

        //option avec execl on a chemin du programme, 0er arg, 1nd, dest apres decodage, fichier d'entré, fin des args (variadique)
        execl("/usr/bin/uuencode","uuencode","-m","/dev/stdin",NULL);
        perror("erreur du execl !\n");
        
        fclose(res); // Ferme le descripteur de fichier associé à la sortie du fils
        close(0);   
        exit(0);    //permet au pere de plus attendre
    }

}
TAG_ENCODE
gcc filtre_encode.c -o fe



cat << TAG_CAT > my-ball-encode.sh 
#!/bin/bash

cat << TAG_DECODE > filtre_decode.c 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

//decode les char 
void decode(FILE* out, char ligne[], char caractere[], int bourage, int end){
    int c = 7;    //position de décalage
    int umask = 0;
    int courant = 0;
    int k = 0;
    int comp = 0;

    //init le tab
    caractere[courant] &= umask;
    
    //parcours les 8 chars
    while(k != 8){
        //parcour le char courant et le met dans le bonne endroit
        for(int i = 4; i >= 0; i--){
            // change de case du tab et reinit les var 
            if(comp == 4){
                c = 7 ;
                comp = 0;
                courant ++; 
                caractere[courant] &= umask;
            }
           
            if((ligne[k]>>i & 1) == 1){
                umask++;    // = 0000 0001 apres on decal le 1
                umask <<= c;
                caractere[courant] |= umask;    //copie la val dans le tab
                umask = 0;
            }

            c--; 

            if((ligne[k+1]>>i & 1) == 1){
            umask++; 
            umask <<= c;
            caractere[courant] |= umask;
            umask = 0;
            }
            c--;
            comp++;
        }
    k = k + 2;    //change de char 2 a 2
    } 

    //réécri dans le fichier 
   int flag = 1, j=0;
   

    while( j < 5 && flag ){
    if( bourage && end){
        if(caractere[j] == -128){    //signifie que ca plus ce qui suit = bourage
            flag = 0;
        }
        else
            fprintf(out, "%c", caractere[j]);
    }
    else
        fprintf(out, "%c", caractere[j]);
    
    j++;   
    }
}

void dissocier(char oct[]){
    for(int j = 0; j < 8; j++ ){
        if(oct[j] >= 'a')        
        oct[j] = oct[j] - 97;
        
        else
        oct[j] = oct[j] - (48-26);
    }
}


int main(int argc, char *argv[]){
    //test le nb d'arg
    if(argc < 1 || argc > 4){
        printf(" Ce prog prends 1-4 arg");
        return 1;
    }

    FILE* input;    //entré de la partie de mon decodage
    FILE* out;      //sortie du prog

    FILE *res;  //sortie de uudecode
    int t[2];   //pipe ou t[0] = lecture   et t[1] = ecriture
    int r = 1;
    int fils = 1;   //diferencier le code qui doit etre fait par le fils ou le pere en plus de r
    int opt;
    int pipmod = 0;
    char command[32];

    if(argc > 1)
        opt = strcmp(argv[1], "-uu");   //test si on a "-uu" si oui opt = 0 sinon opt = 1 || -1

        //pour tous les test si on ouvre un fichier on le test et ainsi de suite
        if(argc == 3 && opt){
        input = fopen(argv[1], "rb");
        //test si ca a marcher
        if (input == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
            return 2;
        }
        //ouvre le fichier resultat
        out = fopen(argv[2], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
            return 3;
        }
    }
    else if(argc == 2 && opt){
        input = stdin;  
        //ouvre le fichier resultat
        out = fopen(argv[1], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
            return 3;
        }
    }
    else if(opt && argc == 1){
        input = stdin;
        out = stdout;
    }

    //test pour le pipe
    else if(!opt && argc == 4){
        out = fopen(argv[3], "w");
        if(out == NULL){
            printf("impossible d'ouvrir %s \n", argv[3]);
            return 2;
        }
        //constuit la command
        sprintf(command, "uudecode -p %s", argv[2]);
    }
    else if(!opt && argc == 3){
        out = fopen(argv[2], "w");
        if(out == NULL){
            printf("impossible d'ouvrir %s \n", argv[2]);
            return 2;
        }
        //constuit la command
        sprintf(command, "uudecode -p");
    }
    else if(!opt && argc == 2){
        out = stdout;
        //constuit la command
        sprintf(command, "uudecode -p");
    }
    else{
        printf("no matching case \n");
        return 5;
    }

    //cree le pipe et execute le code du pere
    if(! opt){
        input = stdin;
        res = stdout;   //pour ecrire dans le pipe
        //res = fdopen(t[1], "w");    //pour ecrire dans le pipe 
        pipmod = 1;
        pipe(t);
        r = fork(); //cree le processus fils
        if(r == -1){
            printf("echec de la creation du processus");
            return 4;
        }
        //code du pere
        if(r != 0){
            fils = 0;
            // Ferme le descripteur de fichier en lecture du pipe (reading end)
            close(t[0]);
            // Ferme le descripteur de fichier standard de sortie (stdout)
            close(1);
            // Duplique le descripteur de fichier en écriture du pipe (writing end) sur le descripteur de fichier standard de sortie (stdout)
            dup(t[1]);
            // Ferme le descripteur de fichier en écriture du pipe (writing end) après la duplication
            close(t[1]);
            //OPTION AVEC EXECL
            //execl("/usr/bin/uudecode","uudecode","-p",NULL);
            //perror("erreur du execl !\n");

            //REMPLCE CA***********$
            FILE* pipe_out = popen(command, "r");
            if(pipe_out == NULL) {
                printf("Erreur lors de l'exécution de popen");
                exit(EXIT_FAILURE);
            }

            // Lit les données depuis le processus fils (via le pipe) et écrit dans le fichier ou la sortie standard
            char buffer[1024];
            int bytesRead;

            while((bytesRead = fread(buffer, 1, sizeof(buffer), pipe_out)) > 0)
                fwrite(buffer, 1, bytesRead, res);

            //JUSQUE LA **********************

            //ferme les differents FILE*
            pclose(pipe_out);
            fclose(res);
            wait(NULL); // Le père attend que le fils termine avant de continuer
            return 0;
        }
    }

    //instruction du fils
    if(r == 0){
        close(t[1]);
        // Ferme le descripteur de fichier en lecture standard (stdin)
        close(0);
        // Duplique le descripteur de fichier du pipe en lecture sur stdin
        dup2(t[0], 0);
        // Ferme le descripteur de fichier en lecture du pipe (il n'est plus nécessaire)
        close(t[0]);
    }
    //si on est le fils ou on a pas -uu fait ce code
    if(r == 0 || fils == 1){
        //identique a decode.c
        //init les var
        char ligne[9];
        char ligne2[9];
        char caractere[5];
        int mask = 0;
        int nb, end = 0;
        int bourage = 0;
        //initialisation
        fread(ligne, 1, 8, input);
        ligne[8] = '\0'; // Ajout du caractère nul pour former une chaîne de caractères valide
        nb = fread(ligne2, 1, 8, input);
        ligne2[8] = '\0';
        if (nb != 8 ) {
            bourage = (int) ligne2[0] - '0';
            end = 1;
        }
        dissocier(ligne);
        decode(out, ligne, caractere, bourage, end);

        //boucle principale qui recupere 8 char et les traitent 
        while(! end){
            strcpy(ligne, ligne2);
            nb = fread(ligne2, 1, 8, input);
            ligne2[8] = '\0';
            if (nb != 8) {
                bourage = (int) ligne2[0] - '0';
                end = 1;
            }
            dissocier(ligne);
            decode(out, ligne, caractere, bourage, end);
        }

        fclose(input);
        fclose(out); // Ferme le descripteur de fichier associé à la sortie du fils
        if(!r){
        close(0);
        exit(0);    //signal au pere la fin du fils
        }
    }

}
TAG_DECODE
gcc filtre_decode.c -o fd
TAG_CAT
#On donne les droits d'execution
	chmod u+x my-ball-encode.sh

else
	shift
fi

#Boucle pour tous les arguments
for i in "$@"
do  
	#Test dossier
	if test -d "$i"
	then
		#Test dans sous dossier
		for j in "$i"/*
		do	
			if test -f "$j" #On test si c'est un fichier
			then #Et on l'archive si c'est le cas en l'encodant
						
cat << TAG_CAT >> my-ball-encode.sh
mkdir -p "$(dirname "$j")"
./fd -uu "$j" << TAG
$(./fe -uu < "$j")
TAG
TAG_CAT
			elif test `ls -a "$j" | sed -e '/\.$/d' | wc -l` -ne 0 #On relance le programme avec le dossier dans le dossier s'il n'est pas vide
			then
				$0 -r@$ "$j"
			else #S'il est vide alors on rajoute juste la ligne pour recréer le dossier vide
cat << TAG_CAT >> my-ball-encode.sh
mkdir -p "$j"
TAG_CAT
			fi
			done
	#Test Fichier
	elif test -f "$i" #Si ce n'est pas un dossier, on teste si c'est un fichier
	then #Et on l'archive si c'est le cas en l'encodant
cat << TAG_CAT >> my-ball-encode.sh
mkdir -p "$(dirname "$i")"
./fd -uu "$i" << TAG
$(./fe -uu < "$i")
TAG
TAG_CAT
    else
        echo "$i" "n'exite pas sur cette machine"
	fi

done