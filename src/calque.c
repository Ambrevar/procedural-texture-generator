#include "calque.h"
#include "misc.h"
#include "main.h"
#include "interpol.h"

#include <stdio.h>
#include <stdlib.h>

struct calque* init_calque(int t, float p){

    struct calque *s = malloc(sizeof(struct calque));

    if(!s){
        printf("Erreur d'allocation");
        return NULL;
    }

    s->v = malloc(t*sizeof(int*));

    if(!s->v){
        printf("Erreur d'allocation");
        return NULL;
    }

    int i,j;
    for (i=0; i<t ; i++){
        s->v[i]= malloc(t*sizeof(int));
        if(!s->v[i]) {
            printf("Erreur d'allocation");
            return NULL;
        }
        for (j=0; j<t; j++)
            s->v[i][j]=0;
    }

    s->taille = t;
    s->persistance = p;

    return s;
}

void free_calque(struct calque* s){
    int j;
    for (j=0; j<s->taille; j++)
        free(s->v[j]);
    free(s->v);
}

// Calque aléatoire GS
calque* generer_calque_alea(// int seed,
                    struct calque *c){

    int taille = c->taille;
    int i,j;


    calque *random;
    random = init_calque(taille,1);
    if (!random)
        return c;

    for (i=0; i<taille; i++)
        for (j=0; j<taille; j++)
            random->v[i][j]= aleatoire(256);
            // random->v[i][j]=randomgen(seed,255);

    return random;
}




void generer_calque_travail(int frequence,
                    int octaves,
                    float persistance,
                    calque* c,
                    calque* random){

    int taille = c->taille;
    int i,j,n,f_courante;
    int a;
    float pas, sum_persistances;

    pas = (float)(taille)/frequence;
    float persistance_courante = persistance;

    // calques de travail
    calque **mes_calques = malloc(octaves*sizeof(struct calque*));
    for (i=0; i<octaves; i++){
        mes_calques[i] = init_calque(taille, persistance_courante);
        if (!mes_calques[i])
            return;
        persistance_courante*=persistance;
    }

    f_courante = frequence;

    // remplissage de calque
    for (n=0; n<octaves; n++){
        for(i=0; i<taille; i++)
            for(j=0; j<taille; j++) {
                a = interpol_val(i, j, f_courante, random);
                mes_calques[n]->v[i][j]=a;
            }
        f_courante*=frequence;
    }

    sum_persistances = 0;
    for (i=0; i<octaves; i++)
        sum_persistances+=mes_calques[i]->persistance;

    // ajout des calques successifs
    for (i=0; i<taille; i++) {
        for (j=0; j<taille; j++) {
            for (n=0; n<octaves; n++) {
                c->v[i][j]+=mes_calques[n]->v[i][j]*mes_calques[n]->persistance;
            }
            // normalisation
            c->v[i][j] =  c->v[i][j] / sum_persistances;
        }
    }

//    enregistrer_bmp(mes_calques[0], "bitmap/octave0.bmp");
//    enregistrer_bmp(mes_calques[1], "bitmap/octave1.bmp");
//    enregistrer_bmp(mes_calques[2], "bitmap/octave2.bmp");
//    enregistrer_bmp(mes_calques[3], "bitmap/octave3.bmp");

// Libération mémoire
    free_calque(random);

    for (i=0; i<octaves; i++) {
        free_calque(mes_calques[i]);
    }
    free(mes_calques);
}






// Lissage
calque* lissage_calque(int liss, calque *c){

    int taille = c->taille;
    int n;
    int x,y,k,l;
    int a;

    calque *lissage;
    lissage = init_calque(taille, 0);

    if(!lissage){
        return c;
    }

    for (x=0; x<taille; x++)
        for (y=0; y<taille; y++){
            a=0;
            n=0;
            for (k=x-liss; k<=x+liss; k++)
                for (l=y-liss; l<=y+liss; l++)
                    if ((k>=0) && (k<taille) && (l>=0) && (l<taille)) {
                        n++;
                        a+=c->v[k][l];
                    }
            lissage->v[x][y] = (float)a/n;
        }
    return lissage;
}

