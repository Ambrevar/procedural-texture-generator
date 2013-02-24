#include <stdio.h>
#include <stdlib.h>
/* #include <time.h> */
#include <SDL/SDL.h>

#include "layer.h"
#include "misc.h"

void
usage ()
{
    puts ("procedural-textures [OPTIONS] FILE\n\n"
          "Option list:\n" "  -h: show this help.\n");
}

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

    /* printf ("%p\n",option_ptr); */
    /* printf ("%p\n",file_buf); */
    /* memcpy(&(seed), option_ptr, 4); option_ptr += 4; */
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

    printf ("%ld,%ld\n",option_ptr-file_buf, file_size);
    
    
// TODO: persistence translation.

    fprintf (stderr, "%ld\n", seed);
    fprintf (stderr, "%ld\n", octaves);
    fprintf (stderr, "%ld\n", frequency);
    fprintf (stderr, "%lf\n", persistence);
    fprintf (stderr, "%ld\n", width);
    fprintf (stderr, "%ld\n", threshold_red);
    fprintf (stderr, "%ld\n", threshold_green);
    fprintf (stderr, "%ld\n", threshold_blue);
    fprintf (stderr, "%ld\n", color1.red);
    fprintf (stderr, "%ld\n", color1.green);
    fprintf (stderr, "%ld\n", color1.blue);
    fprintf (stderr, "%ld\n", color2.red);
    fprintf (stderr, "%ld\n", color2.green);
    fprintf (stderr, "%ld\n", color2.blue);
    fprintf (stderr, "%ld\n", color3.red);
    fprintf (stderr, "%ld\n", color3.green);
    fprintf (stderr, "%ld\n", color3.blue);
    fprintf (stderr, "%ld\n", smoothing);

    return 0;






    // Génération aléatoire : utilisation de la graine.
    srand (seed);

    // Création de layer
    struct layer *base;

    trace("Init.");

    // Initialisation du layer
    // TODO: second arg should be persistance.
    base = init_layer (width, 1);
    if (!base)
    {
        return 1;
    }


    trace("Random layer.");

    // Transformation via l'algorithme de Perlin.
    layer *random_layer = generate_random_layer (base);
    save_bmp (random_layer, "random.bmp");
    generate_work_layer (frequency, octaves, persistence, base,
                         random_layer);

    trace("GS.");

    save_bmp (base, "resultat_GS.bmp");
    trace("RGB.");
    save_bmp_rgb (base, "resultat_RGB.bmp", threshold_red, threshold_green,
                  threshold_blue, color1, color2, color3);
    trace("Alt.");
    save_bmp_alt (base, "resultat_alt.bmp", threshold_red, color1, color2);



    // Version lissée si lissage activé en option.
    if (smoothing != 0)
    {
        // lissage_layer(degre_lissage, base_lisse, threshold_red, threshold_green, threshold_blue);

        layer *layer_smoothed = smooth_layer (smoothing, base);

        save_bmp (layer_smoothed, "resultat_GS_lisse.bmp");
        save_bmp_rgb (layer_smoothed, "resultat_RGB_lisse.bmp", threshold_red,
                      threshold_green, threshold_blue, color1, color2,
                      color3);
        save_bmp_alt (layer_smoothed, "resultat_alt_lisse.bmp", threshold_red,
                      color1, color2);

        free_layer (layer_smoothed);
    }


    free(file_buf);
    return EXIT_SUCCESS;
}
