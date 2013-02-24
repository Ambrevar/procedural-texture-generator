#include "calque.h"
#include "misc.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Fonction SDL pour colorer "ecran".
void colorerPixel(SDL_Surface* ecran, int x, int y, Uint32 couleur){
    *((Uint32*)(ecran->pixels) + x + y * ecran->w) = couleur;
}


// Fourni une valeur entre 0 et a.
unsigned char aleatoire(float a){
    return (float)rand() / RAND_MAX * a;
}

// Fourni une valeur entre 0 et a.
//    * b et m sont premiers entre eux ;
//    * si m est un multiple de 4, alors a%4 = 1 ;
//    * pour tous les nombres premiers p diviseurs de m, on a a%p =1 .

// b doit être "petit" par rapport à a et m ;
// a doit être proche de la racine carrée de m.
unsigned char randomgen(int max, int seed){
    int i;
    static int random= 12453;
    int a = 22695477, b = 1;
    for (i=0; i<seed; i++){
        random = (a*random+b) % (int)pow(2,30);
    }
    return random % max;
}


void enregistrer_bmp(struct calque *c,
                        const char *filename){

    SDL_Surface *ecran = SDL_CreateRGBSurface(SDL_SWSURFACE,c->taille, c->taille, 32,0, 0, 0, 0);
    if (!ecran)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    int couleur_init;
    Uint32 u;
    SDL_PixelFormat *fmt = ecran->format;
    for (i=0; i<c->taille; i++)
        for (j=0; j < c->taille; j++){

            couleur_init = c->v[i][j] % 255;

            u = SDL_MapRGB  (fmt, (char)c->v[i][j], (char)c->v[i][j], (char)c->v[i][j]);
            colorerPixel(ecran, i, j, u);
        }

    SDL_SaveBMP(ecran, filename);
    SDL_FreeSurface(ecran);
}

void enregistrer_bmp_rgb(calque *c,
                        const char *filename,
                        int seuil_rouge,
                        int seuil_vert,
                        int seuil_bleu,
                        couleur couleur1,
                        couleur couleur2,
                        couleur couleur3){

    SDL_Surface *ecran = SDL_CreateRGBSurface(SDL_SWSURFACE,c->taille, c->taille, 32,0, 0, 0, 0);
    if (!ecran)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    int couleur_init = 0;
    Uint32 u;
    SDL_PixelFormat *fmt = ecran->format;
    for (i=0; i<c->taille; i++)
        for (j=0; j < c->taille; j++){

            couleur_init = c->v[i][j] % 255;

            int rouge, vert, bleu;

            double f;

            if (couleur_init < seuil_rouge) {
                f = (double)(couleur_init - 0) / (seuil_rouge - 0);
                rouge = couleur1.rouge;
                vert = couleur1.vert;
                bleu = couleur1.bleu;
            }
            else if (couleur_init < seuil_vert) {
                f = (double)(couleur_init - seuil_rouge) / (seuil_vert - seuil_rouge);
                rouge = (int)couleur1.rouge*(1-f) + couleur2.rouge*(f);
                vert = (int)couleur1.vert*(1-f) + couleur2.vert*(f);
                bleu = (int)couleur1.bleu*(1-f) + couleur2.bleu*(f);
            }
            else if (couleur_init < seuil_bleu) {
                f = (double)(couleur_init - seuil_vert) / (seuil_bleu - seuil_vert);
                rouge = (int)couleur2.rouge*(1-f) + couleur3.rouge*(f);
                vert = (int)couleur2.vert*(1-f) + couleur3.vert*(f);
                bleu = (int)couleur2.bleu*(1-f) + couleur3.bleu*(f);
            }
            else {
                f = (double)(couleur_init - seuil_bleu) / (255 - seuil_bleu);
                rouge = couleur3.rouge;
                vert = couleur3.vert;
                bleu = couleur3.bleu;
            }


            u = SDL_MapRGB  (fmt, rouge, vert, bleu);
            colorerPixel(ecran, i, j, u);
        }

    SDL_SaveBMP(ecran, filename);
    SDL_FreeSurface(ecran);
}


void enregistrer_bmp_alt(calque *c,
                        const char *filename,
                        int seuil,
                        couleur couleur1,
                        couleur couleur2){

    SDL_Surface *ecran = SDL_CreateRGBSurface(SDL_SWSURFACE,c->taille, c->taille, 32,0, 0, 0, 0);
    if (!ecran)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    int couleur_init = 0;
    Uint32 u;
    SDL_PixelFormat *fmt = ecran->format;
    for (i=0; i<c->taille; i++)
        for (j=0; j < c->taille; j++){

            couleur_init = c->v[i][j] % 255;

            int rouge, vert, bleu;

            double f;

            double valeur = fmod(couleur_init, seuil);
            if(valeur > seuil / 2){
                valeur = seuil - valeur;
            }

            f = (1 - cos(3.1415 * valeur / (seuil / 2))) / 2;
            rouge = couleur1.rouge * (1 - f) + couleur2.rouge * f;
            vert = couleur1.vert * (1 - f) + couleur2.vert * f;
            bleu = couleur1.bleu * (1 - f) + couleur2.bleu * f;

            u = SDL_MapRGB  (fmt, rouge, vert, bleu);
            colorerPixel(ecran, i, j, u);
        }

    SDL_SaveBMP(ecran, filename);
    SDL_FreeSurface(ecran);
}


