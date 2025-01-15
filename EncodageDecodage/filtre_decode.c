#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//decode les char 
void char_to_bin(FILE* out, char ligne[], char caractere[], int bourage, int end){
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
    if(argc < 1 || argc > 3){
        printf(" Ce prog prends 1 ou 2 arg");
        return 1;
    }

    FILE* input;
    FILE* out;

    if(argc == 3 ){
        input = fopen(argv[1], "r");
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
    else if(argc == 2){
        input = stdin;
        //ouvre le fichier resultat
        out = fopen(argv[1], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
            return 3;
        }
    }
    else{
        input = stdin;
        out = stdout;
    }

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
    char_to_bin(out, ligne, caractere, bourage, end);

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
        char_to_bin(out, ligne, caractere, bourage,end);
    }


    fclose(input);
    fclose(out);
    return 0;
}