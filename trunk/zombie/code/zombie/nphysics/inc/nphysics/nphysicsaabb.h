#ifndef N_PHYSICSAABB_H
#define N_PHYSICSAABB_H

//-----------------------------------------------------------------------------
/**
    @class nPhysicsAABB
    @ingroup NebulaPhysicsSystem
    @brief A representation of the bounding box data for geometries.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "mathlib/vector.h"
//-----------------------------------------------------------------------------
class nGfxServer2;
//-----------------------------------------------------------------------------

class nPhysicsAABB {
public:

    /// stores the values that represent the bounding box
    float minx;
    float maxx;
    float miny;
    float maxy;
    float minz;
    float maxz;

    /// returns the vertexs of the box
    void GetVertex( int index, vector3& vertex ) const;

    /// allows adding two bounding boxes
    nPhysicsAABB operator + ( const nPhysicsAABB& box );
    
    /// adds a bounding box to this one
    nPhysicsAABB& operator += ( const nPhysicsAABB& box );

    /// scales the bounding box
    void Scale( const float scale );

    /// compares two bounding boxes
    const bool operator == ( const nPhysicsAABB& box ) const;

    /// compares two bounding boxes
    const bool operator != ( const nPhysicsAABB& box ) const;

    /// returns if a point it's inside the AABB
    const bool IsInside( const float x, const float y, const float z ) const;

#ifndef NGAME
    /// draws the bounding box
    void Draw( nGfxServer2* server, const vector4& color );
#endif

private:
    /// accessor to the information
    static float nPhysicsAABB::* arrayPointers[8][3];
};

//-----------------------------------------------------------------------------
/**
    Returns if a point it's inside the AABB.

    @param x x world coordinate
    @param y y world coordinate
    @param z z world coordinate

    @return true/false

    history:
        - 15-Dec-2004   David Reyes   created
        - 08-Mar-2006   David Reyes   inlined
*/
inline
const bool nPhysicsAABB::IsInside( const float x, const float y, const float z ) const
{

    if( y > this->maxy )
        return false;
    if( y < this->miny )
        return false;

    if( x > this->maxx )
        return false;
    if( x < this->minx )
        return false;

    if( z > this->maxz )
        return false;
    if( z < this->minz )
        return false;

    return true;
}

#endif