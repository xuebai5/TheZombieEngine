//------------------------------------------------------------------------------
//  ndebugcross_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugcross.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugCross::nDebugCross():
    center(0.f,0.f,0.f),
    size( 0.1f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugCross::~nDebugCross()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugCross::Draw( nGfxServer2 * const gfxServer )
{
    vector3 lista[6];

    lista[0].x = center.x; lista[0].y = center.y + this->size; lista[0].z = center.z;
    lista[1].x = center.x; lista[1].y = center.y - this->size; lista[1].z = center.z;
    lista[2].x = center.x + this->size; lista[2].y = center.y; lista[2].z = center.z;
    lista[3].x = center.x - this->size; lista[3].y = center.y; lista[3].z = center.z;
    lista[4].x = center.x; lista[4].y = center.y; lista[4].z = center.z + this->size;
    lista[5].x = center.x; lista[5].y = center.y; lista[5].z = center.z - this->size;

    vector4 color;
    this->GetColor( color );

    gfxServer->BeginLines();
    gfxServer->DrawLines3d( lista,   2, color );
    gfxServer->DrawLines3d( lista+2, 2, color );
    gfxServer->DrawLines3d( lista+4, 2, color );
    gfxServer->EndLines();
}

//------------------------------------------------------------------------------
/**
    @param point new center
*/
void
nDebugCross::SetCenter( const vector3 & point )
{
    this->center = point;
}

//------------------------------------------------------------------------------
