#include <stdio.h>
#include <string.h>

//decode les char 
void decode(FILE* out, char ligne[], char caractere[], int bour, int end){
    int c = 7;    //position de décalage
    int umask = 0;
    int courant = 0;    //case courante dans caractere(resultate de decode)
    int k = 0;  //position dans ligne (ce qu'on veut decoder)
    int comp = 0;   //nombre de bits de ligne qu'on a mit dans caractere / 2

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

            c--; //decla de c-1 apres chaque copie
            //meme chose que l'autre bloc
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
    //si on a du bourrage et que c'est la fin du fichier debour
    if(bour && end){
        //quand on a le 1er char du bourrage on cesse d'ecrire
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

//passer de a-z -> 0-25     et de 0-5 -> 26-31
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
    if(argc != 3){
        printf(" Ce prog prends 2 arg");
        return 1;
    }

    //ouverture du ficier a decoder
    FILE* input;
    input = fopen(argv[1], "r");
    //test si ca a marcher
    if (input == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 2;
    }
    //ouvre le fichier resultat
    FILE* out;
    out = fopen(argv[2], "w");
    //test si ca a marcher
    if (out == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
        return 3;
    }

    //init les var
    char ligne[9];  //1er groupe de 8 lu
    char ligne2[9]; //2nd
    char caractere[5];  //chaine resultat
    int mask = 0;
    int nb, end = 0;
    int bourage = 0;
/*
Pour cette partie on vas juste lire les 8 1er dans ligne puis les 8 suivants dans ligne2 selon ligne2 traite ligne1 ensuite
on met ligne2 dans ligne1 et on lit 8char dans ligne 2 et selon ligne2, on traite ligne et ainsi de suite
*/
    //initialisation
    fread(ligne, 1, 8, input);
    ligne[8] = '\0'; // Ajout du caractère nul pour former une chaîne de caractères valide
    nb = fread(ligne2, 1, 8, input);
    ligne2[8] = '\0';   //pour former une chaine correctement pour strcpy
    if (nb != 8 ) {
        bourage = (int) ligne2[0] - '0';    //recupere la valeur du bourrage
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
        decode(out, ligne, caractere, bourage,end);
    }

    //fermeture des fichiers
    fclose(input);
    fclose(out);
    return 0;
}