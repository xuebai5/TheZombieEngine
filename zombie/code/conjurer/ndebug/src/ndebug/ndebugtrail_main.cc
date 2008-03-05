//------------------------------------------------------------------------------
//  ndebugtrail_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugtrail.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugTrail::nDebugTrail()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugTrail::~nDebugTrail()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugTrail::Draw( nGfxServer2 * const gfxServer )
{
    vector4 color;
    this->GetColor( color );

    gfxServer->BeginLines();
    gfxServer->DrawLines3d( this->points.Begin(), this->points.Size() , color );
    gfxServer->EndLines();
}

//------------------------------------------------------------------------------
/**
    @param point new point
*/
void
nDebugTrail::InsertPoint( const vector3 & point )
{
    this->points.Append( point );
}

//------------------------------------------------------------------------------
