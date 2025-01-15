#include <stdio.h>

//encode les char 5 -> 8
void encode(char ligne[], char oct[8]){
    int c=4;    //position de décalage
    int umask = 0;  //mask pour copier les valeurs
    int courant = 0;    //case du tab oct
    int k = 0;  //parcour de ligne
    int comp=0; //savoir quand changer de case de oct

    //init le tab a 0 pour eviter les valeurs residuelles
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
            //les bits impaires vont dans le tab courant
            if(i % 2 == 1){
                //si le b vaut 1 on fait le traitement sinon "on fait rien" 
                if((ligne[k]>>i & 1) == 1){
                    umask++;    // = 0000 0001 apres on decal le 1
                    umask <<= c;
                    oct[courant] |= umask;    //copie la val dans le tab en preservant les anciennes
                    umask = 0;  //RAZ du la var pour eviter les problemes de decalage
                }
            }
            //les b paires vont dans le tab courant +1
            else{
                //meme traitement qu'au dessus
                if((ligne[k]>>i & 1) == 1){
                    umask++; 
                    umask <<= c;
                    oct[courant+1] |= umask;
                    umask = 0;
                }
                c--;   //ajuste la positions de decalage
            }
        comp++; 
        }
    k++;    //change de char
    }

}

//prend des val entre 0 et 31 passe de a->z || 0->5
void associer(FILE* n, char oct[]){
    for(int j = 0; j < 8; j++ ){
        //si char entre O 25 -> a z
        if(oct[j] < 26){
            fprintf(n, "%c", oct[j] + 97 );  //add 97 pour atteindre 'a'
            }
        //sinon de 26 31 -> 0 5
        else 
            fprintf(n, "%c", oct[j] + (48-26));   //add 48-26 pour atteindre '0'
        }
}


int main(int argc, char *argv[]){

    //test le nb d'arg
    if(argc != 3){
        printf(" Ce prog prends 2 arg");
        return 1;
    }

    //ouverture du ficier a encoder
    FILE* input;
    input = fopen(argv[1], "rb");
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
    char ligne[6];  //ligne lu dans input
    char oct[8];    //ce dans quoi on encode
    int mask = 0;   
    int nb;     //nombre de char lu dans le fichier
    int bourage = 0;    
    int secu = 0;   //pour eviter d'ecrire le bourrage dans les fichier vides
    
    //boucle principale qui recupere 5 char et les traitent 
    while((nb = fread(ligne, 1, 5, input)) > 0){
        //si on a pas un multiple de 5 bourrage
        if(nb != 5){
            //ajoute -128 au dernier char plus 1
            ligne[nb] = 1;
            ligne[nb] <<= 7;
            // et jusqu'a la fin met des 0
            for(int i = nb + 1; i < 5; i++)
                ligne[i] &= mask ;      //remplie les car manquant par 0000 0000
            bourage = 5-nb;   //savaegarde pour savoir de combien on a fait le bourrage(rq 1 aurait sufit)
        }
        ligne[5] = '\0'; // Ajout du caractère nul pour former une chaîne de caractères valide
        encode(ligne, oct);
        associer(out, oct);
        secu = 1; //indique que le fichier n'est pas vide
    }
    if(secu)
        fprintf(out, "%d", bourage);
    
    //ferme les fichiers
    fclose(input);
    fclose(out);
    return 0;
}