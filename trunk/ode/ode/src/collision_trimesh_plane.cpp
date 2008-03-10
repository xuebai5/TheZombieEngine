/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001-2003 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

// TriMesh plane collision code by David Reyes i Forniés.

#include <ode/collision.h>
#include <ode/matrix.h>
#include <ode/rotation.h>
#include <ode/odemath.h>
#include "collision_util.h"

#include "collision_std_internal.h"

#define TRIMESH_INTERNAL
#include "collision_trimesh_internal.h"

bool const _IntersectPlaneSegment( const Point& pointA, const Point& pointB, const dxPlane* plane, Point& intersectionPoint ) 
{
    Point ab = pointB - pointA;

    dReal coeficient = ( plane->p[3] - dDOT(plane->p, pointA) ) / dDOT(plane->p, ab ); 

    // coeficient [0..1]

    if( coeficient >= 0 && coeficient <= dReal(1) )
    {
        intersectionPoint = pointA + coeficient * ab;

        return true;
    }
    
    return false;
}

bool const _CollidePlaneTriangle( const VertexPointers& vertexes, const dxPlane* plane, Point& intersectionPoint )
{
    // collide the plane against the three segments

    if( _IntersectPlaneSegment( *vertexes.Vertex[0],*vertexes.Vertex[1],plane, intersectionPoint ) )
    {
        return true;
    }

    if( _IntersectPlaneSegment( *vertexes.Vertex[0],*vertexes.Vertex[2],plane, intersectionPoint ) )
    {
        return true;
    }

    if( _IntersectPlaneSegment( *vertexes.Vertex[1],*vertexes.Vertex[2],plane, intersectionPoint ) )
    {
        return true;
    }

    return false;
}

dReal AngleTwoVectors( dReal* a, Point* b ) {

    dReal result(dDOT(a,*b));

    dReal lena = dLENGTH(a);

    result = result / lena * b->Magnitude();

    return acos( result );
}
#if 0
int dCollidePTL(dxGeom *o1, dxGeom *o2, int flags, dContactGeom *contact, int skip)
{
    static dMatrix3 mTriMeshRot;
    static dVector3 mTriMeshPos;

    dContactGeom* contacts = contact;

    int numContacts = 0;

    dxTriMesh* TriMesh = (dxTriMesh*)o1;
    dxPlane* Plane = (dxPlane*)o2;

	const dMatrix3* pTriRot = (const dMatrix3*)dGeomGetRotation(TriMesh);
	memcpy(mTriMeshRot,pTriRot,sizeof(dMatrix3));

	const dVector3* pTriPos = (const dVector3*)dGeomGetPosition(TriMesh);
	memcpy(mTriMeshPos,pTriPos,sizeof(dVector3));	

	Matrix4x4 MeshMatrix;
	MakeMatrix(mTriMeshPos, mTriMeshRot, MeshMatrix);

    // loop through all intersecting triangles
	for (int i = 0;i < TriMesh->Data->Mesh.GetNbTriangles(); ++i )
	{
		if( numContacts >= unsigned int(flags & NUMC_MASK) ) 
		{
			break;
		}

        VertexPointers vertexes;

        TriMesh->Data->Mesh.GetTriangle( vertexes, i );

        Point intersection;

        Point a,b,c;
        
        a = MeshMatrix * (*vertexes.Vertex[0]);
        b = MeshMatrix * (*vertexes.Vertex[1]);
        c = MeshMatrix * (*vertexes.Vertex[2]);

        a += mTriMeshPos;
        b += mTriMeshPos;
        c += mTriMeshPos;

        vertexes.Vertex[0] = &a;
        vertexes.Vertex[1] = &b;
        vertexes.Vertex[2] = &c;

        if( _CollidePlaneTriangle( vertexes, Plane, intersection ) ) 
        {
            contacts = SAFECONTACT( flags, contacts, numContacts, skip );
            contacts->pos[0] = intersection[0];
            contacts->pos[1] = intersection[1];
            contacts->pos[2] = intersection[2];
            
            /*contacts->normal[0] = *(static_cast<const dReal*>(TriMesh->Data->Normals) + i*3);
            contacts->normal[1] = *(static_cast<const dReal*>(TriMesh->Data->Normals) + i*3 + 1);
            contacts->normal[2] = *(static_cast<const dReal*>(TriMesh->Data->Normals) + i*3 + 2);*/
            contacts->normal[0] = Plane->p[0];
            contacts->normal[1] = Plane->p[1];
            contacts->normal[2] = Plane->p[2];
            
            Point AB = (*vertexes.Vertex[1]) - (*vertexes.Vertex[0]);
            Point CB = (*vertexes.Vertex[2]) - (*vertexes.Vertex[0]);

            Point Normal;// = CB * AB;
            dCROSS( contacts->normal, =, CB, AB );

            contacts->normal[0] *= -1.f;
            contacts->normal[1] *= -1.f;
            contacts->normal[2] *= -1.f;
            //contacts->normal[0] = Normal[0];// + Plane->p[0];
            //contacts->normal[1] = Normal[1];// + Plane->p[1];
            //contacts->normal[2] = Normal[2];// + Plane->p[2];

            dNormalize3(contacts->normal);

            contacts->depth = 0.0f;

/*            contacts->normal[0] = Plane->p[0];
            contacts->normal[1] = Plane->p[1];
            contacts->normal[2] = Plane->p[2];*/

            Point A = (*vertexes.Vertex[0]) - intersection;        

            if( dDOT( Plane->p, A ) < 0 )
            //if( AngleTwoVectors( Plane->p, &A ) < 0 )
            {
                contacts->depth = A.Magnitude();
                contacts->normal[0] = -A[0];
                contacts->normal[1] = -A[1];
                contacts->normal[2] = -A[2];
            }

            Point B = (*vertexes.Vertex[1]) - intersection;

//            if( AngleTwoVectors( Plane->p, &B ) < 0 )
            if( dDOT( Plane->p, B ) < 0 )
            {
                if( contacts->depth < B.Magnitude() )
                {
                    contacts->depth = B.Magnitude();
                    contacts->normal[0] = -B[0];
                    contacts->normal[1] = -B[1];
                    contacts->normal[2] = -B[2];
                }
            }

            Point C = (*vertexes.Vertex[2]) - intersection ;

            //if( AngleTwoVectors( Plane->p, &C ) < 0 )
            if( dDOT( Plane->p, C ) < 0 )
            {
                if( contacts->depth < C.Magnitude() )
                {
                    contacts->depth = C.Magnitude();
                    contacts->normal[0] = -C[0];
                    contacts->normal[1] = -C[1];
                    contacts->normal[2] = -C[2];
                }
            }

            contacts->depth *= 0.5f;

            if( contacts->depth < 0 )
                __asm int 3;

            contacts->g1 = o1;
            contacts->g2 = o2;


            ++numContacts;
        }
	}

    return numContacts;
}
#else
int dCollidePTL(dxGeom *o1, dxGeom *o2, int flags, dContactGeom *contact, int skip) {


    dContactGeom* aContact = contact;

    dxPlane* plane(static_cast<dxPlane*>(o2));

    dReal *NormalPlane(plane->p);
   
    dVector3 position = {
        NormalPlane[0] * NormalPlane[3]-.0005,
        NormalPlane[1] * NormalPlane[3]-.0005,
        NormalPlane[2] * NormalPlane[3]-.0005 };

    dMatrix3 orientation;

    dRFromAxisAndAngle( orientation, plane->p[2], 0, -plane->p[0], acos( plane->p[1]) );

    dGeomID idgeom(dCreateBox( 0, 1000000, dReal(.001), 1000000 ));

    dGeomSetRotation( idgeom, orientation );

    dGeomSetPosition( idgeom, position[0], position[1], position[2] );

    int contacts = dCollideBTL( o1, idgeom, flags, contact, skip );

    for( int index(0); index < contacts; ++index )
    {
        aContact = SAFECONTACT( flags, contact, index, skip );

        aContact->g2 = o2;
    }

    dGeomDestroy( idgeom );

    return contacts;
}

#endif