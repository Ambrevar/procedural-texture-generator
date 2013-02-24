#ifndef __misc_h__
#define __misc_h__

#include <SDL/SDL.h>

void colorerPixel(SDL_Surface* ecran, int x, int y, Uint32 couleur);

unsigned char aleatoire(float a);
unsigned char randomgen(int max, int seed);

void enregistrer_bmp(struct calque *c,
                        const char *filename);

void enregistrer_bmp_rgb(struct calque *c,
                        const char *filename,
                        int seuil_rouge,
                        int seuil_vert,
                        int seuil_bleu,
                        couleur couleur1,
                        couleur couleur2,
                        couleur couleur3);

void enregistrer_bmp_alt(calque *c,
                        const char *filename,
                        int seuil,
                        couleur couleur1,
                        couleur couleur2);

#endif
