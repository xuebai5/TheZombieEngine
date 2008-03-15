#ifndef N_DELAUNAY_H
#define N_DELAUNAY_H
/**
    Source: http://astronomy.swin.edu.au/~pbourke/modelling/triangulate/

   (C) 2005 Conjurer Services, S.A.
*/

#include "mathlib/vector.h"

class nDelaunay
{
public:
    struct ITRIANGLE 
    {
        int p1, p2, p3;
    };

    struct IEDGE 
    {
        int p1, p2;
    };

    int Triangulate(int nv, vector3 *pxyz, ITRIANGLE *v, int *ntri);

private:
    int CircumCircle (float xp,float yp,
                      float x1, float y1, float x2, float y2, float x3, float y3,
                      float *xc, float *yc, float *r);
};

#endif