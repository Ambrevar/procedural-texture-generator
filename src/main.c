/**
 * Copyright © 2013 Pierre Neidhardt
 * See LICENSE file for copyright and license details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include <strings.h>

#include "config.h"

/* TODO: replace persistence double with a Uint8/Uint8 fraction. */
/* TODO: Could seed be one one byte only? */
/**
 * Texture file spec:
 *
 * Uint8 seed;
 * Uint16 octaves;
 * Uint16 frequency;
 * Uint8 persistence_num;
 * Uint8 persistence_denom;
 * tsize_t width;
 * tsize_t height;
 * Uint8  threshold_red;
 * Uint8  threshold_green;
 * Uint8  threshold_blue;
 * Uint8  color1.red;
 * Uint8  color1.green;
 * Uint8  color1.blue;
 * Uint8  color2.red;
 * Uint8  color2.green;
 * Uint8  color2.blue;
 * Uint8  color3.red;
 * Uint8  color3.green;
 * Uint8  color3.blue;
 * Uint8 smoothing;
 */

#define TEXTURE_FILE_SIZE 34

/* Type for texture pixel size. */
typedef Uint32 tsize_t;

typedef struct color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
} color;

/* Texture parameters. */
typedef struct texture_parameter {
    Uint32 seed;
    Uint16 octaves;
    Uint16 frequency;
    double persistence;
    tsize_t width;
    Uint8 threshold_red;
    Uint8 threshold_green;
    Uint8 threshold_blue;
    color color1;
    color color2;
    color color3;
    Uint16 smoothing;
} texture_parameter;


typedef struct layer
{
    Uint8 *v;
    tsize_t size;
} layer;


/******************************************************************************/

void
usage (void)
{
    puts ("procedural-textures [OPTIONS] FILE\n\n"
          "Option list:\n" "  none\n");
}

void
trace (const char *s)
{
    fprintf (stderr, "==> %s\n", s);
}

/******************************************************************************/

/**
 * Returns a value between 0 and max inclusive.
 */
unsigned long
randomgen (unsigned long max)
{
    return (unsigned long) ((((double) rand ()) / RAND_MAX) * max);
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

int
init_layer (layer * current_layer, tsize_t size)
{
    if (!current_layer)
    {
        trace ("Wrong layer, could not initialize.");
        return EXIT_FAILURE;
    }

    /* WARNING: there is no overflow check here! */
    tsize_t memsize = size * size;
    current_layer->v = malloc (memsize * sizeof (Uint8));

    if (!current_layer->v)
    {
        trace ("Allocation error.");
        return EXIT_FAILURE;
    }

    memset (current_layer->v, 0, memsize);
    current_layer->size = size;

    return EXIT_SUCCESS;
}

void
free_layer (struct layer *l)
{
    free (l->v);
}

Uint8 *
at_layer (struct layer *l, tsize_t i, tsize_t j)
{
    return &(l->v[i * l->size + j]);
}


/* SDL function to color a specific pixel on "screen". */
void
color_pixel (SDL_Surface * screen, tsize_t x, tsize_t y, tsize_t red,
             Uint8 green, Uint8 blue)
{
    Uint32 map = SDL_MapRGB (screen->format, red, green, blue);
    *((Uint32 *) (screen->pixels) + x + y * screen->w) = map;
}

/* Grayscale bitmap. */
int
save_bmp (struct layer *current_layer, const char *filename)
{
    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size,
                              current_layer->size, 32, 0, 0, 0,
                              0);
    if (!screen)
    {
        trace ("SDL error on SDL_CreateRGBSurface");
        return EXIT_FAILURE;
    }

    /* The last three args of color_pixel are red, green and blue values. For
     * grayscale, we provide 3 times the same value. */
    tsize_t i, j;
    for (i = 0; i < current_layer->size; i++)
        for (j = 0; j < current_layer->size; j++)
            color_pixel (screen, i, j, *at_layer (current_layer, i, j),
                         *at_layer (current_layer, i, j),
                         *at_layer (current_layer, i, j));

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);

    return EXIT_SUCCESS;
}

/**
 * In the whole program layers are encoded in grayscale. To add colors, we use
 * the three colors and thresholds provided as argument.
 */
int
save_bmp_rgb (layer * current_layer, const char *filename,
              Uint8 threshold_red, Uint8 threshold_green,
              Uint8 threshold_blue, color color1, color color2, color color3)
{
    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size,
                              current_layer->size, 32, 0, 0, 0, 0);
    if (!screen)
    {
        trace ("SDL error on SDL_CreateRGBSurface");
        return EXIT_FAILURE;
    }

    tsize_t i, j;

    for (i = 0; i < current_layer->size; i++)
        for (j = 0; j < current_layer->size; j++)
        {
            Uint8 red, green, blue;
            double f;

            if (*at_layer (current_layer, i, j) < threshold_red)
            {
                red = color1.red;
                green = color1.green;
                blue = color1.blue;
            }
            else if (*at_layer (current_layer, i, j) < threshold_green)
            {
                f = (double) (*at_layer (current_layer, i, j) -
                              threshold_red) / (threshold_green -
                                                threshold_red);
                red = (color1.red * (1 - f) + color2.red * (f));
                green = (color1.green * (1 - f) + color2.green * (f));
                blue = (color1.blue * (1 - f) + color2.blue * (f));
            }
            else if (*at_layer (current_layer, i, j) < threshold_blue)
            {
                f = (double) (*at_layer (current_layer, i, j) -
                              threshold_green) / (threshold_blue -
                                                  threshold_green);
                red = (color2.red * (1 - f) + color3.red * (f));
                green = (color2.green * (1 - f) + color3.green * (f));
                blue = (color2.blue * (1 - f) + color3.blue * (f));
            }
            else
            {
                f = (double) (*at_layer (current_layer, i, j) -
                              threshold_blue) / (255 - threshold_blue);
                red = color3.red;
                green = color3.green;
                blue = color3.blue;
            }

            color_pixel (screen, i, j, red, green, blue);
        }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);

    return EXIT_SUCCESS;
}

/**
 * Same as save_bmp_rgb but with cosine interpolation for colors. Result is more
 * "liquid".
 */
int
save_bmp_alt (layer * current_layer,
              const char *filename, Uint8 threshold, color color1,
              color color2)
{
    SDL_Surface *screen =
        SDL_CreateRGBSurface (SDL_SWSURFACE, current_layer->size,
                              current_layer->size, 32, 0, 0, 0,
                              0);
    if (!screen)
    {
        trace ("SDL error on SDL_CreateRGBSurface");
        return EXIT_FAILURE;
    }

    tsize_t i, j;
    for (i = 0; i < current_layer->size; i++)
        for (j = 0; j < current_layer->size; j++)
        {
            Uint8 red, green, blue;
            double f;

            /* TODO: test against "threshold", not "threshold / 2". */
            double value = fmod (*at_layer (current_layer, i, j), threshold);
            if (value > threshold / 2)
                value = threshold - value;

            f = (1 - cos (M_PI * value / (threshold / 2))) / 2;
            red = color1.red * (1 - f) + color2.red * f;
            green = color1.green * (1 - f) + color2.green * f;
            blue = color1.blue * (1 - f) + color2.blue * f;

            color_pixel (screen, i, j, red, green, blue);
        }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);
    return EXIT_SUCCESS;
}

/* Using splines. We use long arguments as y1 and y2 on purpose, so that it can
 * be used in any context. */
/* TODO: what are the two spline conditions? */
long
interpol (long y1, long y2, long step, long delta)
{
    /* step == 0 should never happen. */
    if (step == 0)
        return y1;
    if (step == 1)
        return y2;

    double a = (double) 1 - (double) delta / step;
    double b = (double) delta / step;

    double fac1 = 3 * (a * a) - 2 * (a * a * a);
    double fac2 = 3 * (b * b) - 2 * (b * b * b);

    return y1 * fac1 + y2 * fac2;

    /* Linear interpolation. Unused. */
    /*
       if (n!=0)
       return y1+delta*((double)y2-(double)y1)/(double)n;
       else
       return y1;
     */
}

Uint8
interpol_val (tsize_t i, tsize_t j, Uint16 frequency, layer * current_layer)
{
    /* Bound values are the four corners of the square in which the point (i,j)
     * is. The square is actually the grid computed upon the frequency and the
     * size of the layout. */
    tsize_t bound1i, bound1j, bound2i, bound2j;

    tsize_t step = current_layer->size / frequency;
    if (step == 0)
        return *at_layer (current_layer, i, j);

    bound1i = i / step * step;
    bound2i = bound1i + step;

    if (bound2i >= current_layer->size)
        bound2i = current_layer->size - 1;

    bound1j = j / step * step;
    bound2j = bound1j + step;

    if (bound2j >= current_layer->size)
        bound2j = current_layer->size - 1;

    Uint8 b11, b12, b21, b22;
    b11 = *at_layer (current_layer, bound1i, bound1j);
    b12 = *at_layer (current_layer, bound1i, bound2j);
    b21 = *at_layer (current_layer, bound2i, bound1j);
    b22 = *at_layer (current_layer, bound2i, bound2j);

    Uint8 v1 = interpol (b11, b12, step, j - bound1j);
    Uint8 v2 = interpol (b21, b22, step, j - bound1j);
    Uint8 result = interpol (v1, v2, step, i - bound1i);

    return result;
}

int
generate_random_layer (layer * random_layer, layer * c, Uint32 seed)
{
    /* Values are only on 0..255, so it's gray scale. We add colors only when we
     * save/render the picture. */
    tsize_t size = c->size;
    tsize_t i, j;

    if (init_layer (random_layer, size) == EXIT_FAILURE)
    {
        trace ("Could not init random layer.");
        return EXIT_FAILURE;
    }

    /* Init seeds for both std and home-made RNG. We do not use the home-made
     * RNG here. */
    srand (seed);
    /* custom_randomgen (0, seed); */
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            /* random_layer->v[i][j] = custom_randomgen (255, 0); */
            random_layer->v[i * random_layer->size + j] = randomgen (255);

    return EXIT_SUCCESS;
}

int
generate_work_layer (Uint16 frequency,
                     Uint16 octaves,
                     double persistence,
                     layer * current_layer, layer * random_layer)
{
    tsize_t size = current_layer->size;
    tsize_t i, j;
    Uint16 n;                   /* Current octave. */
    Uint16 f = frequency;       /* Current frequency. Changes with octaves. */
    double sum_persistences = 0;

    layer *work_layers = malloc (octaves * sizeof (struct layer));
    double *work_persistence = malloc (octaves * sizeof (double));
    for (n = 0; n < octaves; n++)
    {
        if (init_layer (&(work_layers[n]), size) == EXIT_FAILURE)
            return EXIT_FAILURE;

        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
                *at_layer (&(work_layers[n]), i, j) =
                    interpol_val (i, j, f, random_layer);
        }

        f *= frequency;
        if (n == 0)
            work_persistence[n] = persistence;
        else
            work_persistence[n] = work_persistence[n - 1] * persistence;
        sum_persistences += work_persistence[n];
    }

    /* Sum of the consecutive layers for every pixel. */
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            for (n = 0; n < octaves; n++)
            {
                *at_layer (current_layer, i, j) +=
                    *at_layer (&(work_layers[n]), i, j) * work_persistence[n];
            }
            /* Normalizing. */
            *at_layer (current_layer, i, j) =
                *at_layer (current_layer, i, j) / sum_persistences;
        }
    }

    /* Clean. */
    free (work_persistence);
    for (n = 0; n < octaves; n++)
        free_layer (&(work_layers[n]));
    free (work_layers);

    return EXIT_SUCCESS;
}

/**
 * We set the x,y pixel to be the mean of all pixels in the k,l square around
 * it. The new pixel value type needs to be higher than traditionnal pixel
 * because we sum pixels and thus it may overflow. The damping factor is the
 * number of pixels in the square. We need to compute it every time when we are
 * close to a border and k,l is no longer a square.
 */
int
smooth_layer (layer * smoothed_layer, tsize_t factor, layer * current_layer)
{
    tsize_t size = current_layer->size;
    long damping;
    tsize_t x, y;               /* Point coordinates */
    tsize_t k, l;               /* Coordinates of the points in the square around (x,y). */
    double pixel_val;

    if (init_layer (smoothed_layer, size) == EXIT_FAILURE)
    {
        trace ("Could not init smoothed layer.");
        return EXIT_FAILURE;
    }

    for (x = 0; x < size; x++)
    {
        for (y = 0; y < size; y++)
        {
            pixel_val = 0;
            damping = 0;

            size_t kbegin, kend, lbegin, lend;  /* Ranges. */
            kbegin = factor > x ? 0 : x - factor;
            lbegin = factor > y ? 0 : y - factor;
            kend = factor >= size - x ? size - 1 : x + factor;
            lend = factor >= size - y ? size - 1 : y + factor;

            for (k = kbegin; k <= kend; k++)
            {
                for (l = lbegin; l <= lend; l++)
                {
                    damping++;
                    pixel_val += *at_layer (current_layer, k, l);
                }
            }
            *at_layer (smoothed_layer, x, y) = (double) pixel_val / damping;
        }
    }

    return EXIT_SUCCESS;
}

/******************************************************************************/

int
read_opt (const char *buf, texture_parameter *tparam)
{
    unsigned long remmem = TEXTURE_FILE_SIZE;

    /* This macro comes in very handy to read argument one after another. */
#define READ_OPT(opt) \
    if (sizeof(opt) > remmem) return EXIT_FAILURE; \
    memcpy(&(opt), buf, sizeof(opt)); \
    remmem -= sizeof(opt) ; \
    buf += sizeof(opt);

    READ_OPT (tparam->seed);
    READ_OPT (tparam->octaves);
    READ_OPT (tparam->frequency);
    READ_OPT (tparam->persistence);
    READ_OPT (tparam->width);
    READ_OPT (tparam->threshold_red);
    READ_OPT (tparam->threshold_green);
    READ_OPT (tparam->threshold_blue);
    READ_OPT (tparam->color1.red);
    READ_OPT (tparam->color1.green);
    READ_OPT (tparam->color1.blue);
    READ_OPT (tparam->color2.red);
    READ_OPT (tparam->color2.green);
    READ_OPT (tparam->color2.blue);
    READ_OPT (tparam->color3.red);
    READ_OPT (tparam->color3.green);
    READ_OPT (tparam->color3.blue);
    READ_OPT (tparam->smoothing);    

    return EXIT_SUCCESS;
}

int
main (int argc, char **argv)
{
    if (argc < 2)
    {
        usage ();
        return 0;
    }

    FILE *file = NULL;
    file = fopen (argv[1], "rb");
    if (file == NULL)
    {
        trace ("Could not open file:");
        trace (argv[1]);
        return EXIT_FAILURE;
    }

    fseek (file, 0, SEEK_END);
    unsigned long file_size = ftell (file);
    fseek (file, 0, SEEK_SET);

    /* WARNING: it is important to check the input file size. */
    if (file_size != TEXTURE_FILE_SIZE)
    {
        trace ("Wrongly formatted texture file.");
        return EXIT_FAILURE;
    }

    char *file_buf = malloc (file_size);
    if (file_buf == NULL)
    {
        perror (argv[1]);
        return EXIT_FAILURE;
    }

    fread (file_buf, 1, file_size, file);
    fclose (file);

    /* Texture parameters. */
    texture_parameter tparam;
    if (read_opt(file_buf, &tparam) == EXIT_FAILURE)
    {
        trace("Texture file is corrupted.");
        return EXIT_FAILURE;
    }

    /* fprintf (stderr, "%ld\n", seed); */
    /* fprintf (stderr, "%ld\n", octaves); */
    /* fprintf (stderr, "%ld\n", frequency); */
    /* fprintf (stderr, "%lf\n", persistence); */
    /* fprintf (stderr, "%ld\n", width); */
    /* fprintf (stderr, "%ld\n", threshold_red); */
    /* fprintf (stderr, "%ld\n", threshold_green); */
    /* fprintf (stderr, "%ld\n", threshold_blue); */
    /* fprintf (stderr, "%ld\n", color1.red); */
    /* fprintf (stderr, "%ld\n", color1.green); */
    /* fprintf (stderr, "%ld\n", color1.blue); */
    /* fprintf (stderr, "%ld\n", color2.red); */
    /* fprintf (stderr, "%ld\n", color2.green); */
    /* fprintf (stderr, "%ld\n", color2.blue); */
    /* fprintf (stderr, "%ld\n", color3.red); */
    /* fprintf (stderr, "%ld\n", color3.green); */
    /* fprintf (stderr, "%ld\n", color3.blue); */
    /* fprintf (stderr, "%ld\n", smoothing); */

    /* The base layer will contain our final result. */
    trace ("Init.");
    struct layer base;

    /* The base layer is empty at the beginning. It will be generated upon a
     * random layer. */
    if (init_layer (&base, tparam.width) == EXIT_FAILURE)
    {
        trace ("Init layer failed.");
        return EXIT_FAILURE;
    }

    /* Transform base using Perlin algorithm upon a randomly generated layer. */
    trace ("Random layer.");
    layer random_layer;
    if (generate_random_layer (&random_layer, &base, tparam.seed) == EXIT_FAILURE)
        return EXIT_FAILURE;
    save_bmp (&random_layer, OUTPUT_RANDOM);
    if (generate_work_layer
        (tparam.frequency, tparam.octaves, tparam.persistence, &base,
         &random_layer) == EXIT_FAILURE)
        return EXIT_FAILURE;
    free_layer (&random_layer);

    trace ("GS.");
    save_bmp (&base, OUTPUT_GS);
    trace ("RGB.");

    save_bmp_rgb (&base, OUTPUT_RGB, tparam.threshold_red, tparam.threshold_green,
                  tparam.threshold_blue, tparam.color1, tparam.color2, tparam.color3);

    trace ("Alt.");
    save_bmp_alt (&base, OUTPUT_ALT, tparam.threshold_red, tparam.color1, tparam.color2);

    /* Smoothed version if option is non-zero. */
    if (tparam.smoothing != 0)
    {
        layer layer_smoothed;
        if (smooth_layer (&layer_smoothed, tparam.smoothing, &base) == EXIT_FAILURE)
            return EXIT_FAILURE;

        save_bmp (&layer_smoothed, OUTPUT_GS_SMOOTH);
        save_bmp_rgb (&layer_smoothed, OUTPUT_RGB_SMOOTH, tparam.threshold_red,
                      tparam.threshold_green, tparam.threshold_blue, tparam.color1, tparam.color2,
                      tparam.color3);
        save_bmp_alt (&layer_smoothed, OUTPUT_ALT_SMOOTH, tparam.threshold_red,
                      tparam.color1, tparam.color2);

        free_layer (&layer_smoothed);
    }

    free_layer (&base);
    free (file_buf);
    return EXIT_SUCCESS;
}
