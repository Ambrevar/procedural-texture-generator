#include "layer.h"
#include "misc.h"
#include "interpol.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

layer *
init_layer (long size)
{
    layer *current_layer = malloc (sizeof (layer));

    if (!current_layer)
    {
        trace ("Allocation error.");
        return NULL;
    }

    current_layer->v = malloc (size * sizeof (long *));

    if (!current_layer->v)
    {
        trace ("Allocation error.");
        return NULL;
    }

    // TODO: do only one size*size malloc here.
    long i, j;
    for (i = 0; i < size; i++)
    {
        current_layer->v[i] = malloc (size * sizeof (long));
        if (!current_layer->v[i])
        {
            trace ("Allocation error.");
            return NULL;
        }
        for (j = 0; j < size; j++)
            current_layer->v[i][j] = 0;
    }

    current_layer->size = size;

    return current_layer;
}

void
free_layer (struct layer *s)
{
    int j;
    for (j = 0; j < s->size; j++)
        free (s->v[j]);
    free (s->v);
}

// TODO: use constant seed?
/* Gray scale */
layer *
generate_random_layer (struct layer *c, unsigned long seed)
{
    long size = c->size;
    long i, j;

    layer *random_layer;
    random_layer = init_layer (size);
    if (!random_layer)
    {
        trace("Could not init random layer.");
        return c;
    }
    
    /* Init seeds for both std and home-made RNG. */
    srand (seed);
    custom_randomgen (0, seed);
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            /* random_layer->v[i][j] = custom_randomgen (255, 0); */
            random_layer->v[i][j] = randomgen (255);

    return random_layer;
}

// TODO: remove all int.
// TODO: check mem_leaks
// TODO: remove persistence in args and get it from current.
void
generate_work_layer (long frequency,
                     long octaves,
                     double persistence,
                     layer * current_layer, layer * random_layer)
{
    long size = current_layer->size;
    long i, j, n, f_courante = frequency;
    double sum_persistences = 0;

    layer **work_layers = malloc (octaves * sizeof (struct layer *));
    double *work_persistence = malloc (octaves * sizeof (double));
    for (n = 0; n < octaves; n++)
    {
        work_layers[n] = init_layer (size);
        if (!work_layers[n])
            return;

        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
                work_layers[n]->v[i][j] = 
                    interpol_val (i, j, f_courante, random_layer);
        }

        f_courante *= frequency;
        if (n==0)
            work_persistence[n] = persistence;
        else
            work_persistence[n] = work_persistence[n-1] * persistence;
        sum_persistences += work_persistence[n];
    }

    /* Sum of the consecutive layers for every pixel. */
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            for (n = 0; n < octaves; n++)
            {
                current_layer->v[i][j] +=
                    work_layers[n]->v[i][j] * work_persistence[n];
            }
            /* Normalizing. */
            current_layer->v[i][j] =
                current_layer->v[i][j] / sum_persistences;
        }
    }

    /* Clean. */
    free_layer (random_layer);
    for (n = 0; n < octaves; n++)
        free_layer (work_layers[n]);
    free (work_layers);
}


/**
 * We set the x,y pixel to be the mean of all pixels in the k,l square around
 * it. The new pixel value type needs to be higher than traditionnal pixel
 * because we sum pixels and thus it may overflow. The damping factor is the
 * number of pixels in the square. We need to compute it every time when we are
 * close to a border and k,l is no longer a square.
 */
layer *
smooth_layer (long factor, layer * current_layer)
{

    long size = current_layer->size;
    long damping;
    long x, y; 
    long k, l;
    double pixel_val;

    layer *smoothed_layer;
    smoothed_layer = init_layer (size);

    if (!smoothed_layer)
    {
        trace("Could not init smoothed layer.");
        return current_layer;
    }

    for (x = 0; x < size; x++)
        for (y = 0; y < size; y++)
        {
            pixel_val = 0;
            damping = 0;
            for (k = x - factor; k <= x + factor; k++)
            {
                for (l = y - factor; l <= y + factor; l++)
                    if ((k >= 0) && (k < size) && (l >= 0) && (l < size))
                    {
                        damping++;
                        pixel_val += current_layer->v[k][l];
                    }
            }
            smoothed_layer->v[x][y] = (double) pixel_val / damping;
        }

    return smoothed_layer;
}
