#include "layer.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// TODO: what are the two spline conditions?
long
interpol (long y1, long y2, long step, long delta)
{
    /* Spline. */
    if (step == 0)
        return y1;
    if (step == 1)
        return y2;

    double a = (double) delta / step;

    double fac1 = 3 * (1-a)*(1-a) - 2 * (1-a)*(1-a)*(1-a);
    double fac2 = 3 * a*a - 2 * a*a*a;

    return y1 * fac1 + y2 * fac2;

    /* Linear interpolation. Unused. */
    /*
      if (n!=0)
       return y1+delta*((double)y2-(double)y1)/(double)n;
       else
       return y1;
    */
}


// TODO: refactor.
long
interpol_val (long i, long j, long frequency, layer * current_layer)
{
    // Bound values.
    long bound1i, bound1j, bound2i, bound2j;
    long buf;

    // TODO: no need for doubles in all this function.
    double step = (double) current_layer->size / frequency;

    buf = (double) i / step;
    bound1i = buf * step;
    bound2i = bound1i + step;

    if (bound2i >= current_layer->size)
        bound2i = current_layer->size - 1;

    buf = (double) j / step;
    bound1j = buf * step;
    bound2j = bound1j + step;

    if (bound2j >= current_layer->size)
        bound2j = current_layer->size - 1;

    int b11, b12, b21, b22;
    b11 = current_layer->v[bound1i][bound1j];
    b12 = current_layer->v[bound1i][bound2j];
    b21 = current_layer->v[bound2i][bound1j];
    b22 = current_layer->v[bound2i][bound2j];

    int v1 = interpol (b11, b12, step, j - bound1j);
    int v2 = interpol (b21, b22, step, j - bound1j);
    int result = interpol (v1, v2, step, i - bound1i);

    return result;
}
