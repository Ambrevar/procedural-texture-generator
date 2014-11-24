/*
Copyright © 2013-2014 Pierre Neidhardt
See LICENSE file for copyright and license details.
*/

#ifndef CONFIG_H
#define CONFIG_H 1

#define RANDOMGEN_FACTOR 16
#define RANDOMGEN_OFFSET 1
/* #define RANDOMGEN_FACTOR 22695477 */
/* #define RANDOMGEN_OFFSET 1 */

#define OUTPUT_RANDOM "result_random.bmp"
#define OUTPUT_RGB "result_RGB.bmp"
#define OUTPUT_GS "result_GS.bmp"
#define OUTPUT_ALT "result_alt.bmp"
#define OUTPUT_RGB_SMOOTH "result_RGB_smooth.bmp"
#define OUTPUT_GS_SMOOTH "result_GS_smooth.bmp"
#define OUTPUT_ALT_SMOOTH "result_alt_smooth.bmp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#endif /* CONFIG_H */
