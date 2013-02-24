/*******************************************************************************
 * @file config.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H 1


#define RANDOMGEN_FACTOR 22695477
#define RANDOMGEN_OFFSET 1
#define RANDOMGEN_INIT 12453

typedef struct color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
} color;

/* Texture parameters. */
typedef struct texture_param {
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
} texture_param;

#endif // CONFIG_H
