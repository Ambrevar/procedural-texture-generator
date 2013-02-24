#include "layer.h"
#include "misc.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Fonction SDL pour colorer "screen".
void colorerPixel(SDL_Surface* screen, int x, int y, Uint32 target_color){
    *((Uint32*)(screen->pixels) + x + y * screen->w) = target_color;
}


// Fourni une valeur entre 0 et a.
unsigned char custom_random(double a){
    return (double)rand() / RAND_MAX * a;
}

// Fourni une valeur entre 0 et a.
//    * b et m sont premiers entre eux ;
//    * si m est un multiple de 4, alors a%4 = 1 ;
//    * pour tous les nombres premiers p diviseurs de m, on a a%p =1 .

// b doit être "petit" par rapport à a et m ;
// a doit être proche de la racine carrée de m.
unsigned char randomgen(int max, int seed){
    int i;
    static int random_number= 12453;
    int a = 22695477, b = 1;
    for (i=0; i<seed; i++){
        random_number = (a*random_number+b) % (int)pow(2,30);
    }
    return random_number % max;
}


void save_bmp(struct layer *c,
                     const char *filename){

    SDL_Surface *screen = SDL_CreateRGBSurface(SDL_SWSURFACE,c->size, c->size, 32,0, 0, 0, 0);
    if (!screen)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    unsigned char color_init;
    Uint32 u;
    SDL_PixelFormat *fmt = screen->format;
    for (i=0; i<c->size; i++)
        for (j=0; j < c->size; j++){

            color_init = c->v[i][j] % 255;

            u = SDL_MapRGB  (fmt, (char)c->v[i][j], (char)c->v[i][j], (char)c->v[i][j]);
            colorerPixel(screen, i, j, u);
        }

    SDL_SaveBMP(screen, filename);
    SDL_FreeSurface(screen);
}

void save_bmp_rgb(layer *c,
                         const char *filename,
                         int threshold_red,
                         int threshold_green,
                         int threshold_blue,
                         color color1,
                         color color2,
                         color color3){

    SDL_Surface *screen = SDL_CreateRGBSurface(SDL_SWSURFACE,c->size, c->size, 32,0, 0, 0, 0);
    if (!screen)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    int color_init = 0;
    Uint32 u;
    SDL_PixelFormat *fmt = screen->format;
    for (i=0; i<c->size; i++)
        for (j=0; j < c->size; j++){

            color_init = c->v[i][j] % 255;

            int red, green, blue;

            double f;

            if (color_init < threshold_red) {
                f = (double) (color_init - 0) / (threshold_red - 0);
                red = color1.red;
                green = color1.green;
                blue = color1.blue;
            }
            else if (color_init < threshold_green) {
                f = (double)(color_init - threshold_red) / (threshold_green - threshold_red);
                red = (int)color1.red*(1-f) + color2.red*(f);
                green = (int)color1.green*(1-f) + color2.green*(f);
                blue = (int)color1.blue*(1-f) + color2.blue*(f);
            }
            else if (color_init < threshold_blue) {
                f = (double)(color_init - threshold_green) / (threshold_blue - threshold_green);
                red = (int)color2.red*(1-f) + color3.red*(f);
                green = (int)color2.green*(1-f) + color3.green*(f);
                blue = (int)color2.blue*(1-f) + color3.blue*(f);
            }
            else {
                f = (double)(color_init - threshold_blue) / (255 - threshold_blue);
                red = color3.red;
                green = color3.green;
                blue = color3.blue;
            }


            u = SDL_MapRGB  (fmt, red, green, blue);
            colorerPixel(screen, i, j, u);
        }

    SDL_SaveBMP(screen, filename);
    SDL_FreeSurface(screen);
}


void save_bmp_alt(layer *c,
                         const char *filename,
                         int threshold,
                         color color1,
                         color color2){

    SDL_Surface *screen = SDL_CreateRGBSurface(SDL_SWSURFACE,c->size, c->size, 32,0, 0, 0, 0);
    if (!screen)
        printf("erreur SDL sur SDL_CreateRGBSurface");

    int i,j;
    int color_init = 0;
    Uint32 u;
    SDL_PixelFormat *fmt = screen->format;
    for (i=0; i<c->size; i++)
        for (j=0; j < c->size; j++){

            color_init = c->v[i][j] % 255;

            int red, green, blue;

            double f;

            double valeur = fmod(color_init, threshold);
            if(valeur > threshold / 2){
                valeur = threshold - valeur;
            }

            f = (1 - cos(3.1415 * valeur / (threshold / 2))) / 2;
            red = color1.red * (1 - f) + color2.red * f;
            green = color1.green * (1 - f) + color2.green * f;
            blue = color1.blue * (1 - f) + color2.blue * f;

            u = SDL_MapRGB  (fmt, red, green, blue);
            colorerPixel(screen, i, j, u);
        }

    SDL_SaveBMP(screen, filename);
    SDL_FreeSurface(screen);
}


