/*******************************************************************************
 * @file misc.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef MISC_H
#    define MISC_H 1
#    include <SDL/SDL.h>

void
trace (const char *s);

void color_pixel (SDL_Surface * screen, int x, int y, Uint32 target_color);

unsigned long custom_random (unsigned long max);
unsigned long randomgen (unsigned long max, unsigned long seed);

void save_bmp (struct layer *c, const char *filename);

void save_bmp_rgb (struct layer *c,
                   const char *filename,
                   Uint8 threshold_red,
                   Uint8 threshold_green,
                   Uint8 threshold_blue,
                   color color1,
                   color color2,
                   color color3);

void save_bmp_alt (layer * c,
                   const char *filename,
                   Uint8 threshold, color color1, color color2);

#endif // MISC_H
