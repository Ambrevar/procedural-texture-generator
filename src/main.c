#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "layer.h"
#include "misc.h"

void
usage ()
{
    puts ("procedural-textures [OPTIONS] FILE\n\n"
          "Option list:\n" "  -h: show this help.\n");
}

/* TODO: replace persistence double with a Uint8/Uint8 fraction. */
/**
 * File spec:
 *
 * Uint32 seed;
 * Uint16 octaves;
 * Uint16 frequency;
 * double persistence;
 * Uint32 width;
 * Uint32 height (unused);
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
 * Uint16 smoothing;
 */

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
        trace("Could not open file:");
        trace(argv[1]);
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size != TEXTURE_FILE_SIZE)
    {
        trace("Wrongly formatted texture file.");
        return EXIT_FAILURE;
    }
    
    char * file_buf = malloc (file_size);
    if (file_buf == NULL)
    {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    fread( file_buf, 1, file_size, file);
    fclose(file);

    // Texture parameters.
    Uint32 seed;
    Uint16 octaves;
    Uint16 frequency;
    double persistence;
    Uint32 width;
    Uint8  threshold_red;
    Uint8  threshold_green;
    Uint8  threshold_blue;
    color  color1;
    color  color2;
    color  color3;
    Uint16 smoothing;

    char * option_ptr = file_buf;
#define READ_OPT(opt,size) memcpy(&(opt), option_ptr, size); option_ptr += size;

    READ_OPT(seed, 4);
    READ_OPT(octaves, 2);
    READ_OPT(frequency, 2);
    READ_OPT(persistence, 8);
    READ_OPT(width, 4);
    READ_OPT(threshold_red, 1);
    READ_OPT(threshold_green, 1);
    READ_OPT(threshold_blue, 1);
    READ_OPT(color1.red, 1);
    READ_OPT(color1.green, 1);
    READ_OPT(color1.blue, 1);
    READ_OPT(color2.red, 1);
    READ_OPT(color2.green, 1);
    READ_OPT(color2.blue, 1);
    READ_OPT(color3.red, 1);
    READ_OPT(color3.green, 1);
    READ_OPT(color3.blue, 1);
    READ_OPT(smoothing, 2);

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


    // Génération aléatoire : utilisation de la graine.
    srand (seed);

    // Création de layer
    struct layer *base;

    trace("Init.");

    /* The base layer is empty at the beginning. It will be generated upon a */
    /* random layer. */
    base = init_layer (width);
    if (!base)
    {
        trace("Init layer failed.");
        return 1;
    }


    trace("Random layer.");

    // Transformation via l'algorithme de Perlin.
    layer *random_layer = generate_random_layer (base);
    save_bmp (random_layer, OUTPUT_RANDOM);
    generate_work_layer (frequency, octaves, persistence, base,
                         random_layer);

    trace("GS.");

    save_bmp (base, OUTPUT_GS);
    trace("RGB.");
    save_bmp_rgb (base, OUTPUT_RGB, threshold_red, threshold_green,
                  threshold_blue, color1, color2, color3);
    trace("Alt.");
    save_bmp_alt (base, OUTPUT_ALT, threshold_red, color1, color2);



    /* Smoothed version if option is non-zero. */
    if (smoothing != 0)
    {
        layer *layer_smoothed = smooth_layer (smoothing, base);

        save_bmp (layer_smoothed, OUTPUT_GS_SMOOTH);
        save_bmp_rgb (layer_smoothed, OUTPUT_RGB_SMOOTH, threshold_red,
                      threshold_green, threshold_blue, color1, color2,
                      color3);
        save_bmp_alt (layer_smoothed, OUTPUT_ALT_SMOOTH, threshold_red,
                      color1, color2);

        free_layer (layer_smoothed);
    }


    free(file_buf);
    return EXIT_SUCCESS;
}
