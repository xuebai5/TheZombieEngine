//-----------------------------------------------------------------------------
//  nphysicsaabb.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsaabb.h"
#include "gfx2/ngfxserver2.h"

//-----------------------------------------------------------------------------

float nPhysicsAABB::* nPhysicsAABB::arrayPointers[8][3] = {
    {&nPhysicsAABB::minx,&nPhysicsAABB::maxy,&nPhysicsAABB::minz},
    {&nPhysicsAABB::maxx,&nPhysicsAABB::maxy,&nPhysicsAABB::minz},
    {&nPhysicsAABB::maxx,&nPhysicsAABB::miny,&nPhysicsAABB::minz},
    {&nPhysicsAABB::minx,&nPhysicsAABB::miny,&nPhysicsAABB::minz},
    {&nPhysicsAABB::minx,&nPhysicsAABB::maxy,&nPhysicsAABB::maxz},
    {&nPhysicsAABB::maxx,&nPhysicsAABB::maxy,&nPhysicsAABB::maxz},
    {&nPhysicsAABB::maxx,&nPhysicsAABB::miny,&nPhysicsAABB::maxz},
    {&nPhysicsAABB::minx,&nPhysicsAABB::miny,&nPhysicsAABB::maxz},
};

//-----------------------------------------------------------------------------
/**
    Returns the vertexs of the box.

    @param index        index to the vertex (0..7)
    @param vertex       selected vertex

    history:
        - 30-Nov-2004   David Reyes   created
*/
void nPhysicsAABB::GetVertex( int index, vector3& vertex ) const
{
    n_assert2( index < 8, "Index out of bounds" );
    n_assert2( index >= 0, "Index out of bounds" );

    vertex.x = this->*arrayPointers[index][0];
    vertex.y = this->*arrayPointers[index][1];
    vertex.z = this->*arrayPointers[index][2];
}

//-----------------------------------------------------------------------------
/**
    Allows adding two bounding boxes.

    @param box      box to be added
    @return result of adding two bounding boxes

    history:
        - 30-Nov-2004   David Reyes   created
*/
nPhysicsAABB nPhysicsAABB::operator + ( const nPhysicsAABB& box )
{
    nPhysicsAABB result;

    result.minx = box.minx < this->minx ? box.minx : this->minx;
    result.miny = box.miny < this->miny ? box.miny : this->miny;
    result.minz = box.minz < this->minz ? box.minz : this->minz;

    result.maxx = box.maxx > this->maxx ? box.maxx : this->maxx;
    result.maxy = box.maxy > this->maxy ? box.maxy : this->maxy;
    result.maxz = box.maxz > this->maxz ? box.maxz : this->maxz;

    return result;
}

//-----------------------------------------------------------------------------
/**
    Adds a bounding box to this one.

    @param box      box to be added
    @return itself

    history:
        - 30-Nov-2004   David Reyes   created
*/
nPhysicsAABB& nPhysicsAABB::operator += ( const nPhysicsAABB& box )
{
    this->minx = box.minx < this->minx ? box.minx : this->minx;
    this->miny = box.miny < this->miny ? box.miny : this->miny;
    this->minz = box.minz < this->minz ? box.minz : this->minz;

    this->maxx = box.maxx > this->maxx ? box.maxx : this->maxx;
    this->maxy = box.maxy > this->maxy ? box.maxy : this->maxy;
    this->maxz = box.maxz > this->maxz ? box.maxz : this->maxz;

    return *this;
}

//-----------------------------------------------------------------------------
/**
    Scales the bounding box.

    @param scale scale factor

    history:
        - 14-Dec-2004   David Reyes   created
*/
void nPhysicsAABB::Scale( const float scale )
{
    this->minx *= scale;
    this->miny *= scale;
    this->minz *= scale;

    this->maxx *= scale;
    this->maxy *= scale;
    this->maxz *= scale;
}

//-----------------------------------------------------------------------------
/**
    Compares two bounding boxes.

    @param box axis aligned bounding box

    @return true/false

    history:
        - 15-Dec-2004   David Reyes   created
*/
const bool nPhysicsAABB::operator == ( const nPhysicsAABB& box ) const
{
    if( this->minx != box.minx )
        return false;
    if( this->miny != box.miny )
        return false;
    if( this->minz != box.minz )
        return false;

    if( this->maxx != box.maxx )
        return false;
    if( this->maxy != box.maxy )
        return false;
    if( this->maxz != box.maxz )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
/**
    Compares two bounding boxes.

    @param box axis aligned bounding box

    @return true/false

    history:
        - 15-Dec-2004   David Reyes   created
*/
const bool nPhysicsAABB::operator != ( const nPhysicsAABB& box ) const
{
    return !(*this == box);
}


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the bounding box.

    history:
        - 15-Dec-2004   David Reyes   created
*/
void nPhysicsAABB::Draw( nGfxServer2* server, const vector4& color )
{
    vector3 vertexes[10];

    //  3---2
    //  |   |
    //  0---1

    // upper top
    vertexes[0].x = minx;
    vertexes[0].y = maxy;
    vertexes[0].z = minz;

    vertexes[1].x = maxx;
    vertexes[1].y = maxy;
    vertexes[1].z = minz;

    vertexes[2].x = maxx;
    vertexes[2].y = maxy;
    vertexes[2].z = maxz;

    vertexes[3].x = minx;
    vertexes[3].y = maxy;
    vertexes[3].z = maxz;

    /// To Close
    vertexes[4].x = vertexes[0].x;
    vertexes[4].y = vertexes[0].y;
    vertexes[4].z = vertexes[0].z;

    //  3---2
    //  |   |
    //  0---1

    // botton top
    vertexes[5].x = minx;
    vertexes[5].y = miny;
    vertexes[5].z = minz;

    vertexes[6].x = maxx;
    vertexes[6].y = miny;
    vertexes[6].z = minz;

    vertexes[7].x = maxx;
    vertexes[7].y = miny;
    vertexes[7].z = maxz;

    vertexes[8].x = minx;
    vertexes[8].y = miny;
    vertexes[8].z = maxz;

    /// To Close
    vertexes[9].x = vertexes[5].x;
    vertexes[9].y = vertexes[5].y;
    vertexes[9].z = vertexes[5].z;

    server->BeginLines();

    server->DrawLines3d( vertexes, 10, color );

    /// Vertical lines
    vertexes[0].x = maxx;
    vertexes[0].y = maxy;
    vertexes[0].z = minz;

    vertexes[1].x = maxx;
    vertexes[1].y = miny;
    vertexes[1].z = minz;

    server->DrawLines3d( vertexes, 2, color );

    vertexes[0].x = maxx;
    vertexes[0].y = maxy;
    vertexes[0].z = maxz;

    vertexes[1].x = maxx;
    vertexes[1].y = miny;
    vertexes[1].z = maxz;

    server->DrawLines3d( vertexes, 2, color );

    vertexes[0].x = minx;
    vertexes[0].y = maxy;
    vertexes[0].z = maxz;

    vertexes[1].x = minx;
    vertexes[1].y = miny;
    vertexes[1].z = maxz;

    server->DrawLines3d( vertexes, 2, color );

    server->EndLines();
}
#endif
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
