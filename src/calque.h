#ifndef __calque_h__
#define __calque_h__

typedef struct calque calque;
struct calque{
    int **v;
    int taille;
    float persistance;
};

typedef struct couleur couleur;
struct couleur{
    int rouge;
    int vert;
    int bleu;
};


struct calque* init_calque(int, float);
void free_calque(struct calque*);

calque* generer_calque_alea(// int seed,
                    struct calque *c);

void generer_calque_travail(int frequence,
                    int octaves,
                    float persistance,
                    calque* c,
                    calque* random);

calque* lissage_calque(int liss, calque *c);

int interpolate(int, int, int, int);
int valeur_interpolee(int, int, int, struct calque*);

#endif
