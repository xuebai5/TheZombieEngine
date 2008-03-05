#include "precompiled/pchnnavmesh.h"

#include "ndelaunay/ndelaunay.h"
#include <stdlib.h>
#include "mathlib/nmath.h"
#include "kernel/ntypes.h" // for n_realloc
#include "kernel/nlogclass.h"

/*  
   qsort(p,nv,sizeof(dXYZ),XYZCompare);
      :
   int XYZCompare(void *v1,void *v2)
   {
      dXYZ *p1,*p2;
      p1 = v1;
      p2 = v2;
      if (p1->x < p2->x)
         return(-1);
      else if (p1->x > p2->x)
         return(1);
      else
         return(0);
   }
*/

#define EPSILON 0.005f

//------------------------------------------------------------------------------
/**
    Triangulate

    @brief  Triangulation subroutine
            Takes as input NV vertices in array pxyz
            Returned is a list of ntri triangular faces in the array v
            These triangles are arranged in a consistent clockwise order.
            The triangle array 'v' should be malloced to 3 * nv
            The vertex array pxyz must be big enough to hold 3 more points
            The vertex array must be sorted in increasing x values say
*/
int 
nDelaunay::Triangulate (int nv, vector3 *pxyz, ITRIANGLE *v, int *ntri)
{
   int *complete = 0;   
   IEDGE *edges = 0;   
   int nedge = 0;
   int trimax,emax = 200;
   int status = 0;

   int inside;
   int i,j,k;
   float xp,yp,x1,y1,x2,y2,x3,y3,xc,yc,r;
   float xmin,xmax,ymin,ymax,xmid,ymid;
   float dx,dy,dmax;

   /* Allocate memory for the completeness list, flag for each triangle */
   trimax = 4 * nv;   
   if ((complete = (int*)n_new_array (int, trimax)) == 0)
   {
      status = 1;
      goto skip;
   }

   /* Allocate memory for the edge list */   
   if ((edges = (IEDGE*)n_new_array (IEDGE, emax)) == 0) 
   {   
      status = 2;
      goto skip;
   }

   /*
      Find the maximum and minimum vertex bounds.
      This is to allow calculation of the bounding triangle
   */
   xmin = pxyz[0].x;
   ymin = pxyz[0].z;
   xmax = xmin;
   ymax = ymin;
   
   for (i=1;i<nv;i++)
   {
      if (pxyz[i].x < xmin) xmin = pxyz[i].x;
      if (pxyz[i].x > xmax) xmax = pxyz[i].x;
      if (pxyz[i].z < ymin) ymin = pxyz[i].z;
      if (pxyz[i].z > ymax) ymax = pxyz[i].z;
   }

   dx = xmax - xmin;
   dy = ymax - ymin;
   dmax = (dx > dy) ? dx : dy;
   xmid = (xmax + xmin) / 2.0f;
   ymid = (ymax + ymin) / 2.0f;

   /*
      Set up the supertriangle
      This is a triangle which encompasses all the sample points.
      The supertriangle coordinates are added to the end of the
      vertex list. The supertriangle is the first triangle in
      the triangle list.
   */
   pxyz[nv+0].x = xmid - 20 * dmax;
   pxyz[nv+0].y = 0.0;
   pxyz[nv+0].z = ymid - dmax;   
   pxyz[nv+1].x = xmid;
   pxyz[nv+1].y = 0.0;
   pxyz[nv+1].z = ymid + 20 * dmax;   
   pxyz[nv+2].x = xmid + 20 * dmax;
   pxyz[nv+2].y = 0.0;
   pxyz[nv+2].z = ymid - dmax;
   
   v[0].p1 = nv;
   v[0].p2 = nv+1;
   v[0].p3 = nv+2;
   complete[0] = false;
   *ntri = 1;

   /*
      Include each point one at a time into the existing mesh
   */
   nTime startTime( nTimeServer::Instance()->GetTime() );
   for (i=0;i<nv;i++)
   {
       if ( nTimeServer::Instance()->GetTime() - startTime > 10 )
       {
           startTime = nTimeServer::Instance()->GetTime();
           NLOG_REL( navbuild, (NLOGUSER | 1, "[2/4] Delaunay triangulation...%d/%d", i+1, nv) );
       }

      xp = pxyz[i].x;
      yp = pxyz[i].z;
      nedge = 0;

      /*
         Set up the edge buffer.
         If the point (xp,yp) lies inside the circumcircle then the
         three edges of that triangle are added to the edge buffer
         and that triangle is removed.
      */
      for (j=0;j<(*ntri);j++) 
      {
         if (complete[j])
         {
            continue;
         }

         x1 = pxyz[v[j].p1].x;
         y1 = pxyz[v[j].p1].z;
         x2 = pxyz[v[j].p2].x;
         y2 = pxyz[v[j].p2].z;
         x3 = pxyz[v[j].p3].x;
         y3 = pxyz[v[j].p3].z;
         inside = this->CircumCircle(xp,yp,x1,y1,x2,y2,x3,y3,&xc,&yc,&r);

         if (xc + r < xp)
         {
            complete[j] = true;
         }

         if (inside) 
         {
            /* Check that we haven't exceeded the edge list size */
            if (nedge+3 >= emax) 
            {
               emax += 100;
               
               if ((edges = (IEDGE*)n_realloc(edges,emax*(long)sizeof(IEDGE))) == 0) 
               {
                  status = 3;
                  goto skip;
               }
            }
            edges[nedge+0].p1 = v[j].p1;
            edges[nedge+0].p2 = v[j].p2;
            edges[nedge+1].p1 = v[j].p2;
            edges[nedge+1].p2 = v[j].p3;
            edges[nedge+2].p1 = v[j].p3;
            edges[nedge+2].p2 = v[j].p1;
            nedge += 3;
            v[j] = v[(*ntri)-1];
            complete[j] = complete[(*ntri)-1];
            (*ntri)--;
            j--;
         }
      }

      /*
         Tag multiple edges
         Note: if all triangles are specified anticlockwise then all
               interior edges are opposite pointing in direction.
      */
      for ( j=0; j<nedge-1; j++ )
      {
         for ( k=j+1; k<nedge; k++ )
         {
            if ( (edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1) ) 
            {
               edges[j].p1 = -1;
               edges[j].p2 = -1;
               edges[k].p1 = -1;
               edges[k].p2 = -1;
            }
            /* Shouldn't need the following, see note above */
            if ( (edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2) )
            {
               edges[j].p1 = -1;
               edges[j].p2 = -1;
               edges[k].p1 = -1;
               edges[k].p2 = -1;
            }
         }
      }

      /*
         Form new triangles for the current point
         Skipping over any tagged edges.
         All edges are arranged in clockwise order.
      */
      for (j=0;j<nedge;j++) 
      {
         if (edges[j].p1 < 0 || edges[j].p2 < 0)
         {
            continue;
         }

         if ((*ntri) >= trimax) 
         {
            status = 4;
            goto skip;
         }

         v[*ntri].p1 = edges[j].p1;
         v[*ntri].p2 = edges[j].p2;
         v[*ntri].p3 = i;
         complete[*ntri] = false;
         (*ntri)++;
      }
   }

   /*
      Remove triangles with supertriangle vertices
      These are triangles which have a vertex number greater than nv
   */
  /* for (i=0;i<(*ntri);i++) {
      if (v[i].p1 >= nv || v[i].p2 >= nv || v[i].p3 >= nv) {
         v[i] = v[(*ntri)-1];
         (*ntri)--;
         i--;
      }
   }*/

skip:
   n_delete_array (edges);
   n_delete_array (complete);
   return(status);
}

//------------------------------------------------------------------------------
/**
    CircumCircle

   @return  TRUE if a point (xp,yp) is inside the circumcircle made up
            of the points (x1,y1), (x2,y2), (x3,y3)
            The circumcircle centre is returned in (xc,yc) and the radius r
            NOTE: A point on the edge is inside the circumcircle
*/
int 
nDelaunay::CircumCircle (float xp,float yp,
                         float x1, float y1, float x2, float y2, float x3, float y3,
                         float *xc, float *yc, float *r)
{
   double m1,m2,mx1,mx2,my1,my2;
   double dx,dy,rsqr,drsqr;
   double xxc, yyc;

   /* Check for coincident points */
   if (n_abs(y1-y2) < EPSILON && n_abs(y2-y3) < EPSILON)
   {
       return(false);
   }

   if (n_abs(y2-y1) < EPSILON) 
   {
      m2 = - (x3-x2) / (y3-y2);
      mx2 = (x2 + x3) / 2.0f;
      my2 = (y2 + y3) / 2.0f;
      xxc = (x2 + x1) / 2.0f;
      yyc = m2 * (xxc - mx2) + my2;
   }
   else if (n_abs(y3-y2) < EPSILON) 
   {
      m1 = - (x2-x1) / (y2-y1);
      mx1 = (x1 + x2) / 2.0f;
      my1 = (y1 + y2) / 2.0f;
      xxc = (x3 + x2) / 2.0f;
      yyc = m1 * (xxc - mx1) + my1;
   } 
   else 
   {
      m1 = - (x2-x1) / (y2-y1);
      m2 = - (x3-x2) / (y3-y2);
      mx1 = (x1 + x2) / 2.0f;
      mx2 = (x2 + x3) / 2.0f;
      my1 = (y1 + y2) / 2.0f;
      my2 = (y2 + y3) / 2.0f;
      xxc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
      yyc = m1 * (xxc - mx1) + my1;
   }

   dx = x1 - xxc;
   dy = y1 - yyc;
   rsqr = dx*dx + dy*dy;
   *xc = float(xxc);
   *yc = float(yyc);
   *r = n_sqrt(float(rsqr));

   dx = xp - xxc;
   dy = yp - yyc;
   drsqr = dx*dx + dy*dy;

   return drsqr < rsqr;
}

