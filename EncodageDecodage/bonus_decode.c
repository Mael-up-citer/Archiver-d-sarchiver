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