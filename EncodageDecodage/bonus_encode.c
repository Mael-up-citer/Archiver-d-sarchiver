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
        //execl("/usr/bin/uuencode","uuencode","-m","/dev/stdin",NULL);
        //perror("erreur du execl !\n");
        //PAS BESOIN *********************
        //cree un autre processus fils
        FILE* pipe_out = popen("uuencode -m -", "r");
        //test si ca a marcher
        if (pipe_out == NULL) {
            printf("Erreur lors de l'exécution de popen");
            exit(EXIT_FAILURE);
        }

        // Lit les données depuis le processus fils (via le pipe) et écrit dans le fichier ou la sortie standard
        char buffer[1024];
        int bytesRead;
        //lit depuis le fils et ecrit dans le pipe
        while((bytesRead = fread(buffer, 1, sizeof(buffer), pipe_out)) > 0)
                fwrite(buffer, 1, bytesRead, res);
        
        //JUSQUE LA **************

        fclose(res); // Ferme le descripteur de fichier associé à la sortie du fils
        pclose(pipe_out);   // Ferme le descripteur de fichier associé à popen
        close(0);   
        exit(0);    //permet au pere de plus attendre
    }
}