/*******************************************************************************
 * @file layer.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef LAYER_H
#    define LAYER_H 1

#    include <SDL/SDL.h>

typedef struct layer
{
    Uint8 **v;
    long size;
    double persistence;
} layer;

struct layer *init_layer (long);
void free_layer (struct layer *);

layer *generate_random_layer (struct layer *c, unsigned long seed);

void generate_work_layer (long frequency,
                          long octaves,
                          double persistence,
                          layer * current_layer, layer * random_layer);

layer *smooth_layer (long factor, layer * current_layer);

// TODO: remove these lines.
/* int interpolate (int, int, int, int); */
/* int valeur_interpolee (int, int, int, struct layer *); */


#endif // LAYER_H
