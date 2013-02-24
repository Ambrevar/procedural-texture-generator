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

void
trace (const char *s)
{
    fprintf (stderr, "==> %s\n", s);
}

int
main (int argc, char **argv)
{
    if (argc < 2)
    {
        usage ();
        return 0;
    }

    // Paramètres de la texture (valeurs d'entrée)
    int seed;
    int octaves;
    int frenquency;
    double persistance;
    int width;

    uint8_t threshold_red;
    uint8_t threshold_green;
    uint8_t threshold_blue;

    color color1;
    color color2;
    color color3;


    int option_lissage;
    int degre_lissage;

    char chaine[100] = "";

    FILE *fichier = NULL;
    FILE *errorlog = NULL;

    // fichier = fopen("test.ptx", "r");
    fichier = fopen (argv[1], "r");

    if (fichier != NULL)
    {
        fscanf (fichier, "%s %d\n", chaine, &seed);
        fscanf (fichier, "%s %d\n", chaine, &octaves);
        fscanf (fichier, "%s %d\n", chaine, &frenquency);
        fscanf (fichier, "%s %lf\n", chaine, &persistance);
        fscanf (fichier, "%s %d\n", chaine, &width);
        fscanf (fichier, "%s %d\n", chaine, &threshold_red);
        fscanf (fichier, "%s %d\n", chaine, &threshold_green);
        fscanf (fichier, "%s %d\n", chaine, &threshold_blue);
        fscanf (fichier, "%s %d\n", chaine, &color1.red);
        fscanf (fichier, "%s %d\n", chaine, &color1.green);
        fscanf (fichier, "%s %d\n", chaine, &color1.blue);
        fscanf (fichier, "%s %d\n", chaine, &color2.red);
        fscanf (fichier, "%s %d\n", chaine, &color2.green);
        fscanf (fichier, "%s %d\n", chaine, &color2.blue);
        fscanf (fichier, "%s %d\n", chaine, &color3.red);
        fscanf (fichier, "%s %d\n", chaine, &color3.green);
        fscanf (fichier, "%s %d\n", chaine, &color3.blue);
        fscanf (fichier, "%s %d\n", chaine, &option_lissage);
        fscanf (fichier, "%s %d\n", chaine, &degre_lissage);

        fclose (fichier);
    }
    else
    {
        errorlog = fopen ("error.txt", "a");
        if (errorlog != NULL)
            fprintf (errorlog, "\nImpossible d'ouvrir le fichier.\n");
        fclose (errorlog);
        return 1;
    }

    // Génération aléatoire : utilisation de la graine.
    srand (seed);

    // Création de layer
    struct layer *base;

    // Initialisation du layer
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
