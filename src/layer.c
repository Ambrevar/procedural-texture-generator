#include "layer.h"
#include "misc.h"
#include "interpol.h"

#include <stdio.h>
#include <stdlib.h>

struct layer *
init_layer (int t, double p)
{

    struct layer *s = malloc (sizeof (struct layer));

    if (!s)
    {
        printf ("Erreur d'allocation");
        return NULL;
    }

    s->v = malloc (t * sizeof (int *));

    if (!s->v)
    {
        printf ("Erreur d'allocation");
        return NULL;
    }

    int i, j;
    for (i = 0; i < t; i++)
    {
        s->v[i] = malloc (t * sizeof (int));
        if (!s->v[i])
        {
            printf ("Erreur d'allocation");
            return NULL;
        }
        for (j = 0; j < t; j++)
            s->v[i][j] = 0;
    }

    s->size = t;
    s->persistance = p;

    return s;
}

void
free_layer (struct layer *s)
{
    int j;
    for (j = 0; j < s->size; j++)
        free (s->v[j]);
    free (s->v);
}

/* Gray scale */
layer *
generate_random_layer (struct layer *c)
{

    int size = c->size;
    int i, j;


    layer *random_layer;
    random_layer = init_layer (size, 1);
    if (!random_layer)
        return c;

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
        {
            random_layer->v[i][j] = custom_random (256);
            /* random_layer->v[i][j] = randomgen(seed, 255); */
        }

    return random_layer;
}

void
generate_work_layer (long frequency,
                     int octaves,
                     double persistance,
                     layer * current_layer, layer * random_layer)
{
    long size = current_layer->size;
    int i, j, n, f_courante;
    int a;
    double sum_persistances;

    // TODO: what is it supposed to be used for?
    double pas;
    pas = (double) (size) / frequency;

    double persistance_courante = persistance;

    // layers de travail
    layer **mes_layers = malloc (octaves * sizeof (struct layer *));
    for (i = 0; i < octaves; i++)
    {
        mes_layers[i] = init_layer (size, persistance_courante);
        if (!mes_layers[i])
            return;
        persistance_courante *= persistance;
    }

    f_courante = frequency;

    // remplissage de layer
    for (n = 0; n < octaves; n++)
    {
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
            {
                a = interpol_val (i, j, f_courante, random_layer);
                mes_layers[n]->v[i][j] = a;
            }
        f_courante *= frequency;
    }

    sum_persistances = 0;
    for (i = 0; i < octaves; i++)
        sum_persistances += mes_layers[i]->persistance;

    // ajout des layers successifs
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            for (n = 0; n < octaves; n++)
            {
                current_layer->v[i][j] +=
                    mes_layers[n]->v[i][j] * mes_layers[n]->persistance;
            }
            // normalisation
            current_layer->v[i][j] =
                current_layer->v[i][j] / sum_persistances;
        }
    }

//    save_bmp(mes_layers[0], "bitmap/octave0.bmp");
//    save_bmp(mes_layers[1], "bitmap/octave1.bmp");
//    save_bmp(mes_layers[2], "bitmap/octave2.bmp");
//    save_bmp(mes_layers[3], "bitmap/octave3.bmp");

// Libération mémoire
    free_layer (random_layer);

    for (i = 0; i < octaves; i++)
    {
        free_layer (mes_layers[i]);
    }
    free (mes_layers);
}


// Lissage
layer *
smooth_layer (long factor, layer * current_layer)
{

    int size = current_layer->size;
    int n;
    int x, y, k, l;
    int a;

    layer *smoothed_layer;
    smoothed_layer = init_layer (size, 0);

    if (!smoothed_layer)
    {
        return current_layer;
    }

    for (x = 0; x < size; x++)
        for (y = 0; y < size; y++)
        {
            a = 0;
            n = 0;
            for (k = x - factor; k <= x + factor; k++)
                for (l = y - factor; l <= y + factor; l++)
                    if ((k >= 0) && (k < size) && (l >= 0) && (l < size))
                    {
                        n++;
                        a += current_layer->v[k][l];
                    }
            smoothed_layer->v[x][y] = (double) a / n;
        }
    return smoothed_layer;
}
