#include "layer.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// n is the 
int interpol(int y1, int y2, int n, int delta){

    // interpolation non linéaire
    if (n==0)
        return y1;
    if (n==1)
        return y2;

    double a = (double) delta/n;

    double fac1 = 3*pow(1-a, 2) - 2*pow(1-a,3);
    double fac2 = 3*pow(a, 2) - 2*pow(a, 3);

    return y1*fac1 + y2*fac2;

	//////////////////////////////////////////////

// Interpolation linéaire. Alternative inutilisée.
    /*if (n!=0)
        return y1+delta*((double)y2-(double)y1)/(double)n;
    else
        return y1;*/
}


int interpol_val(int i, int j, int frenquency, layer *r){
    // Bound values.
    int bound1x, bound1y, bound2x, bound2y, q;
    double step;
    step = (double) r->size/frenquency;

    q = (double)i/step;
    bound1x = q*step;
    bound2x = (q+1)*step;

    if(bound2x >= r->size)
        bound2x = r->size-1;

    q = (double) j/step;
    bound1y = q*step;
    bound2y = (q+1)*step;

    if(bound2y >= r->size)
        bound2y = r->size-1;

    int b00,b01,b10,b11;
    b00 = r->v[bound1x][bound1y];
    b01 = r->v[bound1x][bound2y];
    b10 = r->v[bound2x][bound1y];
    b11 = r->v[bound2x][bound2y];

    int v1 = interpol(b00, b01, bound2y-bound1y, j-bound1y);
    int v2 = interpol(b10, b11, bound2y-bound1y, j-bound1y);
    int fin = interpol(v1, v2, bound2x-bound1x , i-bound1x);

    return fin;
}
