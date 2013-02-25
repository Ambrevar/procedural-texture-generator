/**
 * Copyright © 2013 Pierre Neidhardt
 * See LICENSE file for copyright and license details.
 *
 * This program takes a procural textures binary descriptor (ptx) file as
 * argument, and creates several graphic files showing different steps of the
 * process.
 *
 * It uses Perlin algorithm. See README for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include <strings.h>

#include "config.h"

/* Type for pixel lengths, like texture resolution. */
typedef Uint32 tsize_t;
typedef Uint64 tarea_t;

typedef struct color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
} color;

/* This is the sume of all parameter sizes. */
#define TEXTURE_FILE_SIZE 29

/**
 * Texture parameters. Note that the persistence is given as two positive
 * integers, the numerator and the denominator. The final persistence is
 *
 *   (double) persistence_num / (double) persistence_den
 *
 * For now only square textures are generated, so we do not use height. It would
 * not require much to implement rectangle support.
 */
/* TODO: decrease size of the seed? */
typedef struct texture_parameter
{
    tsize_t width;
    tsize_t height;
    Uint16 seed;
    Uint16 octaves;
    Uint16 frequency;
    Uint8 persistence_num;
    Uint8 persistence_den;
    Uint8 threshold_red;
    Uint8 threshold_green;
    Uint8 threshold_blue;
    color color1;
    color color2;
    color color3;
    Uint8 smoothing;
} texture_parameter;

typedef struct layer
{
    Uint8 *v;
    tsize_t size;
} layer;

/* Quick strings. Having length is time saving compared to strlen(). */
typedef struct qstring
{
    char *val;
    unsigned long length;
} qstring;

/******************************************************************************/

int
qstring_init (qstring * s, unsigned long size)
{
    s->val = malloc (size);
    if (s->val == NULL)
    {
        perror ("qstring_init");
        return EXIT_FAILURE;
    }
    s->length = size;

    return EXIT_SUCCESS;
}

void
qstring_free (qstring * s)
{
    if (s->val != NULL)
        free (s->val);
}

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

    tarea_t memsize = (tarea_t) size * (tarea_t) size;
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
    return &(l->v[(tarea_t) i * (tarea_t) l->size + (tarea_t) j]);
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
 * "liquid". Note the mirrored value around threshold/2. This is what gives the
 * wave effect.
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

            double value = fmod (*at_layer (current_layer, i, j), threshold);
            if (value > threshold / 2)
                value = threshold - value;

            double f = (1 - cos (M_PI * value / (threshold / 2))) / 2;

            red = color1.red * (1 - f) + color2.red * f;
            green = color1.green * (1 - f) + color2.green * f;
            blue = color1.blue * (1 - f) + color2.blue * f;

            color_pixel (screen, i, j, red, green, blue);
        }

    SDL_SaveBMP (screen, filename);
    SDL_FreeSurface (screen);
    return EXIT_SUCCESS;
}

/**
 * Using cubic splines. We use a cubic polinom p(x) = a + b*x + c*x^2 + d*x^3 so
 * that
 *
 *   p(0) = y1
 *   p(1) = y2
 *   p'(0) = 0
 *   p'(1) = 0
 *
 * We want the border to be smooth, hence the flat tangent. The uniq resulting
 * polynom is
 *
 *   p(x) = y1 [ 3 * (1-x)^2 - 2 (1-x)^3 ] + y2 [ 3*x^2 -2*x^3 ]
 *
 * Here x is delta / step. delta is the distance to y1, step is the distance
 * between y1 and y2. We normalize everything to [0,1] to get the previous
 * property.
 *
 * We use 'long' type for y1 and y2 arguments on purpose, so that it can be used
 * in any context. (This is debatable.)
 */
long
interpol (long y1, long y2, tsize_t step, tsize_t delta)
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
        {
            /* random_layer->v[i][j] = custom_randomgen (255, 0); */
            random_layer->v[i * random_layer->size + j] = randomgen (255);
        }

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

void
texture_details (texture_parameter * tparam)
{
    if (tparam == NULL)
        return;

    puts ("{");
#define TEXTURE_PRINT(opt,fmt)  fprintf (stderr, "  " #opt ": %" fmt  "\n", opt);

    // TODO: use macros to print using PRIu ## type_size.
    TEXTURE_PRINT (tparam->width, PRIu32);
    TEXTURE_PRINT (tparam->height, PRIu32);

    TEXTURE_PRINT (tparam->seed, PRIu16);
    TEXTURE_PRINT (tparam->octaves, PRIu16);
    TEXTURE_PRINT (tparam->frequency, PRIu16);
    TEXTURE_PRINT (tparam->persistence_num, PRIu8);
    TEXTURE_PRINT (tparam->persistence_den, PRIu8);

    TEXTURE_PRINT (tparam->threshold_red, PRIu8);
    TEXTURE_PRINT (tparam->threshold_green, PRIu8);
    TEXTURE_PRINT (tparam->threshold_blue, PRIu8);
    TEXTURE_PRINT (tparam->color1.red, PRIu8);
    TEXTURE_PRINT (tparam->color1.green, PRIu8);
    TEXTURE_PRINT (tparam->color1.blue, PRIu8);
    TEXTURE_PRINT (tparam->color2.red, PRIu8);
    TEXTURE_PRINT (tparam->color2.green, PRIu8);
    TEXTURE_PRINT (tparam->color2.blue, PRIu8);
    TEXTURE_PRINT (tparam->color3.red, PRIu8);
    TEXTURE_PRINT (tparam->color3.green, PRIu8);
    TEXTURE_PRINT (tparam->color3.blue, PRIu8);

    TEXTURE_PRINT (tparam->smoothing, PRIu8);

    puts ("}");
}

/**
 * First we make sure the file length is correct, then we make sure the sequence
 * of READ_OPT does not go beyond the file length by testing against remmem.
 */
int
read_opt (qstring * s, texture_parameter * tparam)
{
    if (s->length != TEXTURE_FILE_SIZE)
        return EXIT_FAILURE;

    unsigned long remmem = TEXTURE_FILE_SIZE;
    const char *buf = s->val;

    /* This macro comes in very handy to read argument one after another. */
#define READ_OPT(opt) \
    if (sizeof(opt) > remmem) return EXIT_FAILURE; \
    memcpy(&(opt), buf, sizeof(opt)); \
    remmem -= sizeof(opt) ; \
    buf += sizeof(opt);

    READ_OPT (tparam->width);
    READ_OPT (tparam->height);
    READ_OPT (tparam->seed);
    READ_OPT (tparam->octaves);
    READ_OPT (tparam->frequency);
    READ_OPT (tparam->persistence_num);
    READ_OPT (tparam->persistence_den);
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

    /* buf[0] != '\0' should never happen. */
    if (buf[0] != '\0' || remmem != 0)
        return EXIT_FAILURE;

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

    qstring file_buf;
    if (qstring_init (&file_buf, file_size) == EXIT_FAILURE)
    {
        perror (argv[1]);
        fclose (file);
        return EXIT_FAILURE;
    }

    fread (file_buf.val, 1, file_size, file);
    fclose (file);

    /* Texture parameters. */
    texture_parameter tparam;
    if (read_opt (&file_buf, &tparam) == EXIT_FAILURE)
    {
        trace ("Texture file is corrupted.");
        qstring_free (&file_buf);
        return EXIT_FAILURE;
    }
    qstring_free (&file_buf);

    /* Print the details to output. */
    texture_details (&tparam);

    /**************************************/

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
    if (generate_random_layer (&random_layer, &base, tparam.seed) ==
        EXIT_FAILURE)
    {
        trace ("Random layer failed.");
        return EXIT_FAILURE;
    }
    save_bmp (&random_layer, OUTPUT_RANDOM);

    trace ("Work layer.");
    if (tparam.persistence_den == 0)
    {
        free_layer (&random_layer);
        trace ("Persistence denominator cannot be zero.");
        return EXIT_FAILURE;
    }
    if (generate_work_layer
        (tparam.frequency, tparam.octaves,
         (double) tparam.persistence_num / tparam.persistence_den, &base,
         &random_layer) == EXIT_FAILURE)
    {
        free_layer (&random_layer);
        trace ("Work layer failed.");
        return EXIT_FAILURE;
    }
    free_layer (&random_layer);

    trace ("GS.");
    save_bmp (&base, OUTPUT_GS);
    trace ("RGB.");

    save_bmp_rgb (&base, OUTPUT_RGB, tparam.threshold_red,
                  tparam.threshold_green, tparam.threshold_blue,
                  tparam.color1, tparam.color2, tparam.color3);

    trace ("Alt.");
    save_bmp_alt (&base, OUTPUT_ALT, tparam.threshold_red, tparam.color1,
                  tparam.color2);

    /* Smoothed version if option is non-zero. */
    if (tparam.smoothing != 0)
    {
        layer layer_smoothed;
        if (smooth_layer (&layer_smoothed, tparam.smoothing, &base) ==
            EXIT_FAILURE)
        {
            trace ("Smoothed layer failed.");
            return EXIT_FAILURE;
        }

        save_bmp (&layer_smoothed, OUTPUT_GS_SMOOTH);
        save_bmp_rgb (&layer_smoothed, OUTPUT_RGB_SMOOTH,
                      tparam.threshold_red, tparam.threshold_green,
                      tparam.threshold_blue, tparam.color1, tparam.color2,
                      tparam.color3);
        save_bmp_alt (&layer_smoothed, OUTPUT_ALT_SMOOTH,
                      tparam.threshold_red, tparam.color1, tparam.color2);

        free_layer (&layer_smoothed);
    }

    free_layer (&base);
    return EXIT_SUCCESS;
}
