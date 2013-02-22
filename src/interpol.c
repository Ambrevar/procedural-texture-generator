#include "calque.h"
#include "misc.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int interpol(int y1, int y2, int n, int delta){

    // interpolation non linéaire
    if (n==0)
        return y1;
    if (n==1)
        return y2;

    float a = (float)delta/n;

    float fac1 = 3*pow(1-a, 2) - 2*pow(1-a,3);
    float fac2 = 3*pow(a, 2) - 2*pow(a, 3);

    return y1*fac1 + y2*fac2;

	//////////////////////////////////////////////

// Interpolation linéaire. Alternative inutilisée.
    /*if (n!=0)
        return y1+delta*((float)y2-(float)y1)/(float)n;
    else
        return y1;*/
}


int interpol_val(int i, int j, int frequence, calque *r){
    // valeurs des bornes
    int borne1x, borne1y, borne2x, borne2y, q;
    float pas;
    pas = (float)r->taille/frequence;

    q = (float)i/pas;
    borne1x = q*pas;
    borne2x = (q+1)*pas;

    if(borne2x >= r->taille)
        borne2x = r->taille-1;

    q = (float)j/pas;
    borne1y = q*pas;
    borne2y = (q+1)*pas;

    if(borne2y >= r->taille)
        borne2y = r->taille-1;

    int b00,b01,b10,b11;
    b00 = r->v[borne1x][borne1y];
    b01 = r->v[borne1x][borne2y];
    b10 = r->v[borne2x][borne1y];
    b11 = r->v[borne2x][borne2y];

    int v1 = interpol(b00, b01, borne2y-borne1y, j-borne1y);
    int v2 = interpol(b10, b11, borne2y-borne1y, j-borne1y);
    int fin = interpol(v1, v2, borne2x-borne1x , i-borne1x);

    return fin;
}
