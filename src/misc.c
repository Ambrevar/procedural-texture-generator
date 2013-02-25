#include "layer.h"
#include "misc.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void
trace (const char *s)
{
    fprintf (stderr, "==> %s\n", s);
}

/* SDL function to color a specific pixel on "screen". */
void
color_pixel (SDL_Surface * screen, int x, int y, Uint32 target_color)
{
    *((Uint32 *) (screen->pixels) + x + y * screen->w) = target_color;
}

/**
 * Returns a value between 0 and max inclusive.
 */
unsigned long
randomgen (unsigned long max)
{
    return (unsigned long) ( ( ((double)rand()) / RAND_MAX) * max);
}

/**
 * Returns a value between 0 and max inclusive. Must be run the first time with
 * a seed, then without a seed.
 *
 * In this home-made random, we use the following recursive sequence to generate
 * random values:
 *
 *  random_number = ( factor * random_number + offset ) % max
 *
 * We need to match the following properties to have a decent RNG.
 *
 *  - Offset and max must be coprime
 *  - If 4 divides max , then factor % 4 == 1
 *  - For all p dividing max, factor % p == 1
 *
 * Besides,
 *
 *  - offset must be "small" compared to max;
 *  - factor must be close to the square root of max.
 */
unsigned long 
custom_randomgen (unsigned long max, unsigned long seed)
{
    static unsigned long random_number = 0;
    if (seed != 0)
    {
        random_number = seed;
        return random_number;
    }

    unsigned long factor = RANDOMGEN_FACTOR, offset = RANDOMGEN_OFFSET;

    random_number = (factor * random_number + offset) % max;
    return random_number;
}

/* Grayscale bitmap. */
void
save_bmp (struct layer *current_layer, const char *filename)
{

    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size, current_layer->size, 32, 0, 0, 0,
                              0);
    if (!screen)
        trace ("SDL error on SDL_CreateRGBSurface");

    long i, j;
    Uint32 map;
    SDL_PixelFormat *fmt = screen->format;
    for (i = 0; i < current_layer->size; i++)
    {
        for (j = 0; j < current_layer->size; j++)
        {
            map = SDL_MapRGB (fmt, current_layer->v[i][j], current_layer->v[i][j], current_layer->v[i][j]);
            color_pixel (screen, i, j, map);
        }
    }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);
}

void
save_bmp_rgb (layer * current_layer,
              const char *filename,
              Uint8 threshold_red,
              Uint8 threshold_green,
              Uint8 threshold_blue, 
              color color1,
              color color2, 
              color color3)
{

    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size, current_layer->size, 32, 0, 0, 0,
                              0);
    if (!screen)
        trace ("SDL error on SDL_CreateRGBSurface");

    long i, j;
    Uint8 color_init = 0;
    Uint32 map;
    SDL_PixelFormat *fmt = screen->format;

    for (i = 0; i < current_layer->size; i++)
        for (j = 0; j < current_layer->size; j++)
        {
            color_init = current_layer->v[i][j];

            Uint8 red, green, blue;
            double f;

            if (color_init < threshold_red)
            {
                /* f = (double) (color_init - 0) / (threshold_red - 0); */
                red = color1.red;
                green = color1.green;
                blue = color1.blue;
            }
            else if (color_init < threshold_green)
            {
                f = (double) (color_init - threshold_red) / (threshold_green -
                                                             threshold_red);
                red = (Uint8) (color1.red * (1 - f) + color2.red * (f));
                green = (Uint8) (color1.green * (1 - f) + color2.green * (f));
                blue = (Uint8) (color1.blue * (1 - f) + color2.blue * (f));
            }
            else if (color_init < threshold_blue)
            {
                f = (double) (color_init -
                              threshold_green) / (threshold_blue -
                                                  threshold_green);
                red = (Uint8) (color2.red * (1 - f) + color3.red * (f));
                green = (Uint8) (color2.green * (1 - f) + color3.green * (f));
                blue = (Uint8) (color2.blue * (1 - f) + color3.blue * (f));
            }
            else
            {
                f = (double) (color_init - threshold_blue) / (255 -
                                                              threshold_blue);
                red = color3.red;
                green = color3.green;
                blue = color3.blue;
            }

            map = SDL_MapRGB (fmt, red, green, blue);
            color_pixel (screen, i, j, map);
        }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);
}


void
save_bmp_alt (layer * current_layer,
              const char *filename, Uint8 threshold, color color1, color color2)
{

    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size, current_layer->size, 32, 0, 0, 0,
                              0);
    if (!screen)
        trace ("SDL error on SDL_CreateRGBSurface");

    long i, j;
    Uint8 color_init = 0;
    Uint32 map;
    SDL_PixelFormat *fmt = screen->format;
    for (i = 0; i < current_layer->size; i++)
        for (j = 0; j < current_layer->size; j++)
        {

            color_init = current_layer->v[i][j];

            Uint8 red, green, blue;
            double f;

            double value = fmod (color_init, threshold);
            if (value > threshold / 2)
                value = threshold - value;

            f = (1 - cos (M_PI * value / (threshold / 2))) / 2;
            red = color1.red * (1 - f) + color2.red * f;
            green = color1.green * (1 - f) + color2.green * f;
            blue = color1.blue * (1 - f) + color2.blue * f;

            map = SDL_MapRGB (fmt, red, green, blue);
            color_pixel (screen, i, j, map);
        }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);
}
