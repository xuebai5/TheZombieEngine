//------------------------------------------------------------------------------
//  ndebugvector_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugvector.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugVector::nDebugVector():
    center(0.f,0.f,0.f),
    direction(1.f,1.f,1.f),
    arrowSize( 0.2f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugVector::~nDebugVector()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugVector::Draw( nGfxServer2 * const gfxServer )
{
    const float arrowSz = this->arrowSize;
    vector3 lista[8];

    lista[0] = this->center;
    lista[1] = this->center + this->direction;

    lista[2].x = lista[1].x; lista[2].y = lista[1].y + arrowSz; lista[2].z = lista[1].z;
    lista[3].x = lista[1].x; lista[3].y = lista[1].y - arrowSz; lista[3].z = lista[1].z;
    lista[4].x = lista[1].x + arrowSz; lista[4].y = lista[1].y; lista[4].z = lista[1].z;
    lista[5].x = lista[1].x - arrowSz; lista[5].y = lista[1].y; lista[5].z = lista[1].z;
    lista[6].x = lista[1].x; lista[6].y = lista[1].y; lista[6].z = lista[1].z + arrowSz;
    lista[7].x = lista[1].x; lista[7].y = lista[1].y; lista[7].z = lista[1].z - arrowSz;

    vector4 color;
    this->GetColor( color );

    gfxServer->BeginLines();
    gfxServer->DrawLines3d( lista,   2, color );
    gfxServer->DrawLines3d( lista+2, 2, color );
    gfxServer->DrawLines3d( lista+4, 2, color );
    gfxServer->DrawLines3d( lista+6, 2, color );
    gfxServer->EndLines();
}

//------------------------------------------------------------------------------
/**
    @param point new center
*/
void
nDebugVector::SetCenter( const vector3 & point )
{
    this->center = point;
}

//------------------------------------------------------------------------------
/**
    @param dir new direction
*/
void
nDebugVector::SetDirection( const vector3 & dir )
{
    this->direction = dir;
}

//------------------------------------------------------------------------------
/**
    @param set arrow size
*/
void
nDebugVector::SetSize( float size )
{
    this->arrowSize = size;
}

//------------------------------------------------------------------------------
