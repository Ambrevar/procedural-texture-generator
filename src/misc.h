/*******************************************************************************
 * @file misc.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef MISC_H
#define MISC_H 1


#include <SDL/SDL.h>

void colorerPixel(SDL_Surface* screen, int x, int y, Uint32 color);

unsigned char custom_random(double a);
unsigned char randomgen(int max, int seed);

void save_bmp(struct layer *c,
                        const char *filename);

void save_bmp_rgb(struct layer *c,
                        const char *filename,
                        int threshold_red,
                        int threshold_green,
                        int threshold_blue,
                        color color1,
                        color color2,
                        color color3);

void save_bmp_alt(layer *c,
                        const char *filename,
                        int threshold,
                        color color1,
                        color color2);

#endif // MISC_H
