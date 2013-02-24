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
    int seed;
    int octaves;
    int frenquency;
    double persistance;
    int width;

    Uint8 threshold_red;
    Uint8 threshold_green;
    Uint8 threshold_blue;

    color color1;
    color color2;
    color color3;


    int option_lissage;
    int degre_lissage;

    char attribute[100] = "";

    FILE *file = NULL;

    file = fopen (argv[1], "r");

    if (file != NULL)
    {
        fscanf (file, "%s %d\n", attribute, &seed);
        fscanf (file, "%s %d\n", attribute, &octaves);
        fscanf (file, "%s %d\n", attribute, &frenquency);
        fscanf (file, "%s %lf\n", attribute, &persistance);
        fscanf (file, "%s %d\n", attribute, &width);
        fscanf (file, "%s %d\n", attribute, &threshold_red);
        fscanf (file, "%s %d\n", attribute, &threshold_green);
        fscanf (file, "%s %d\n", attribute, &threshold_blue);
        fscanf (file, "%s %d\n", attribute, &color1.red);
        fscanf (file, "%s %d\n", attribute, &color1.green);
        fscanf (file, "%s %d\n", attribute, &color1.blue);
        fscanf (file, "%s %d\n", attribute, &color2.red);
        fscanf (file, "%s %d\n", attribute, &color2.green);
        fscanf (file, "%s %d\n", attribute, &color2.blue);
        fscanf (file, "%s %d\n", attribute, &color3.red);
        fscanf (file, "%s %d\n", attribute, &color3.green);
        fscanf (file, "%s %d\n", attribute, &color3.blue);
        fscanf (file, "%s %d\n", attribute, &option_lissage);
        fscanf (file, "%s %d\n", attribute, &degre_lissage);

        fclose (file);
    }
    else
    {
        trace("Could not open file:");
        trace(argv[1]);
        return 1;
    }

    // Génération aléatoire : utilisation de la graine.
    srand (seed);

    // Création de layer
    struct layer *base;

    // Initialisation du layer
    // TODO: second arg should be persistance.
    base = init_layer (width, 1);
    if (!base)
    {
        return 1;
    }


    // Transformation via l'algorithme de Perlin.
    layer *random_layer = generate_random_layer (base);
    save_bmp (random_layer, "random.bmp");
    generate_work_layer (frenquency, octaves, persistance, base,
                         random_layer);

    save_bmp (base, "resultat_GS.bmp");
    save_bmp_rgb (base, "resultat_RGB.bmp", threshold_red, threshold_green,
                  threshold_blue, color1, color2, color3);
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
