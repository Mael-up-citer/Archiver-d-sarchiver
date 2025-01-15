#include <stdio.h>
#include <stdlib.h>

//encode les char 
void char_to_bin(char ligne[], char oct[8]){
    int c=4;    //position de décalage
    int umask = 0;
    int courant = 0;
    int k = 0;
    int comp=0;

    //init le tab
    oct[courant] &= umask;
    oct[courant+1] &= umask;
    
    //parcours les 5 chars
    while(k != 5){
        //parcour le char courant et le met dans le bonne endroit
        for(int i=(sizeof(char)*8)-1; i>=0; i--){
            // change de case du tab et reinit les var 
            if(comp == 10){
                //printf(" SWITCH \n");
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
    if(argc < 1 || argc > 3){
        printf(" Ce prog prends 1 ou 2 arg");
        return 1;
    }

    FILE* input;
    FILE* out;

    if(argc == 3 ){
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
    else if(argc == 2){
        input = stdin;  
        //ouvre le fichier resultat
        out = fopen(argv[1], "w");
        //test si ca a marcher
        if (out == NULL) {
            printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
            return 3;
        }
    }
    else{
        input = stdin;
        out = stdout;
    }

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
            //printf("GO BOURER \n");
            ligne[nb] = 1;
            ligne[nb] <<= 7;
            for(int i = nb + 1; i < 5; i++)
                ligne[i] &= mask ;      //remplie les car manquant par 0000 0000
            bourage = nb;
        }
        ligne[5] = '\0'; // Ajout du caractère nul pour former une chaîne de caractères valide
        char_to_bin(ligne, oct);
        associer(out, oct); 
        secu ++;
    }
    if(secu)
        fprintf(out, "%d", bourage);

    fclose(input);
    fclose(out);
    return 0;
}