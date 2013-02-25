/*******************************************************************************
 * @file proctext.h
 * @date 
 * @brief 
 *
 ******************************************************************************/

#ifndef PROCTEXT_H
#    define PROCTEXT_H 1

#    include <SDL/SDL.h>

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


#    define TEXTURE_FILE_SIZE 34

typedef struct color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
} color;

/* Texture parameters. */
typedef struct texture_param
{
    Uint32 seed;
    Uint16 octaves;
    Uint16 frequency;
    double persistence;
    Uint32 width;
    Uint8 threshold_red;
    Uint8 threshold_green;
    Uint8 threshold_blue;
    color color1;
    color color2;
    color color3;
    Uint16 smoothing;
} texture_param;


#endif // PROCTEXT_H
