//------------------------------------------------------------------------------
//  ndebugsphere_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugsphere.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugSphere::nDebugSphere():
    center( 0, 0,0 ),
    radius( 1 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugSphere::~nDebugSphere()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugSphere::Draw( nGfxServer2 * const gfxServer )
{
    matrix44 model;

    model.scale( vector3( this->radius, this->radius, this->radius ) );
    model.translate( this->center );

    vector4 color;
    this->GetColor( color );

    gfxServer->BeginShapes();
    gfxServer->DrawShape( nGfxServer2::Sphere, model, color );
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    @param point new center
*/
void
nDebugSphere::SetCenter( const vector3 & point )
{
    this->center = point;
}

//------------------------------------------------------------------------------
/**
    @param value new radius
*/
void
nDebugSphere::SetRadius( const float value )
{
    this->radius = value;
}

//------------------------------------------------------------------------------
