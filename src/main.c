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

int
main (int argc, char **argv)
{
    if (argc < 2)
    {
        usage ();
        return 0;
    }

    // Texture parameter.
    long seed;
    long octaves;
    long frequency;
    double persistance;
    long width;

    Uint8 threshold_red;
    Uint8 threshold_green;
    Uint8 threshold_blue;

    color color1;
    color color2;
    color color3;

    long option_lissage;
    long degre_lissage;

    char attribute[100] = "";
    long value;
    double fvalue;

    FILE *file = NULL;

    file = fopen (argv[1], "r");

    if (file != NULL)
    {
        fscanf (file, "%s %ld\n", attribute, &value); seed = value;
        fscanf (file, "%s %ld\n", attribute, &value); octaves = value;
        fscanf (file, "%s %ld\n", attribute, &value ); frequency = value;
        fscanf (file, "%s %lf\n", attribute, &fvalue); persistance = fvalue;
        fscanf (file, "%s %ld\n", attribute, &value); width = value;
        fscanf (file, "%s %ld\n", attribute, &value); threshold_red = value;
        fscanf (file, "%s %ld\n", attribute, &value); threshold_green = value;
        fscanf (file, "%s %ld\n", attribute, &value); threshold_blue = value;
        fscanf (file, "%s %ld\n", attribute, &value); color1.red = value;
        fscanf (file, "%s %ld\n", attribute, &value); color1.green = value;
        fscanf (file, "%s %ld\n", attribute, &value); color1.blue = value;
        fscanf (file, "%s %ld\n", attribute, &value); color2.red = value;
        fscanf (file, "%s %ld\n", attribute, &value); color2.green = value;
        fscanf (file, "%s %ld\n", attribute, &value); color2.blue = value;
        fscanf (file, "%s %ld\n", attribute, &value); color3.red = value;
        fscanf (file, "%s %ld\n", attribute, &value); color3.green = value;
        fscanf (file, "%s %ld\n", attribute, &value); color3.blue = value;
        fscanf (file, "%s %ld\n", attribute, &option_lissage);
        fscanf (file, "%s %ld\n", attribute, &degre_lissage);

        fclose (file);
    }
    else
    {
        trace("Could not open file:");
        trace(argv[1]);
        return 1;
    }


    /* fprintf (stderr, "%d\n", seed); */
    /* fprintf (stderr, "%ld\n", octaves); */
    /* fprintf (stderr, "%ld\n", frequency); */
    /* fprintf (stderr, "%lf\n",persistance); */
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
    /* fprintf (stderr, "%d\n", option_lissage); */
    /* fprintf (stderr, "%d\n", degre_lissage); */


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
    trace("Work layer.");
    generate_work_layer (frequency, octaves, persistance, base,
                         random_layer);

    trace("GS.");

    save_bmp (base, "resultat_GS.bmp");
    trace("RGB.");
    save_bmp_rgb (base, "resultat_RGB.bmp", threshold_red, threshold_green,
                  threshold_blue, color1, color2, color3);
    trace("Alt.");
    save_bmp_alt (base, "resultat_alt.bmp", threshold_red, color1, color2);



    // Version lissée si lissage activé en option.
    if (option_lissage != 0)
    {
        // lissage_layer(degre_lissage, base_lisse, threshold_red, threshold_green, threshold_blue);
        layer *layer_lisse = smooth_layer (degre_lissage, base);

        save_bmp (layer_lisse, "resultat_GS_lisse.bmp");
        save_bmp_rgb (layer_lisse, "resultat_RGB_lisse.bmp", threshold_red,
                      threshold_green, threshold_blue, color1, color2,
                      color3);
        save_bmp_alt (layer_lisse, "resultat_alt_lisse.bmp", threshold_red,
                      color1, color2);

        free_layer (layer_lisse);
    }

    return 0;
}
