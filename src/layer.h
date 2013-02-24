/*******************************************************************************
 * @file layer.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef LAYER_H
#    define LAYER_H 1

#    include <stdint.h>

typedef struct layer
{
    uint8_t **v;
    long size;
    double persistance;
} layer;

typedef struct color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color;


struct layer *init_layer (int, double);
void free_layer (struct layer *);

layer *generate_random_layer (struct layer *c);

void generate_work_layer (long frequency,
                          int octaves,
                          double persistance,
                          layer * current_layer, layer * random_layer);

layer *smooth_layer (long factor, layer * current_layer);

int interpolate (int, int, int, int);
int valeur_interpolee (int, int, int, struct layer *);


#endif // LAYER_H
