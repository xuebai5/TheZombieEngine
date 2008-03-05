//------------------------------------------------------------------------------
//  ndebugcapsule_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugcapsule.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugCapsule::nDebugCapsule():
    center( 0, 0, 0 ),
    radius( 1 ),
    length( 1 ),
    orientation( 0, 0, 0, 1 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugCapsule::~nDebugCapsule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugCapsule::Draw( nGfxServer2 * const gfxServer )
{
    matrix33 mrot; 
    
    mrot = this->orientation;

    matrix44 transform;

    transform.set( mrot.x_component(), mrot.y_component(), mrot.z_component(), vector3(0,0,0) ); 

    transform.M14 = 0;
    transform.M24 = 0;
    transform.M34 = 0;
    transform.M44 = 1;

    matrix44 model1;
    matrix44 model2;
    matrix44 model3;

    model1.scale( vector3( this->radius, this->radius, this->length ) );
    model2.scale( vector3( this->radius, this->radius, this->radius ) );

    model3 = model2;

    model1 = model1 * transform;

    model2.set_translation( vector3( 0, 0, this->length / 4.0f ) );
    model2 = model2 * transform;
    
    model3.set_translation( vector3( 0, 0, this->length / -4.0f ) );
    model3 = model3 * transform;

    model1.translate( this->center );    
    model2.translate( model2.pos_component() + this->center );    
    model3.translate( model3.pos_component() + this->center );    

    vector4 color;
    this->GetColor( color );

    gfxServer->BeginShapes();
    gfxServer->DrawShape( nGfxServer2::Cylinder, model1, color );
    gfxServer->DrawShape( nGfxServer2::Sphere, model2, color );
    gfxServer->DrawShape( nGfxServer2::Sphere, model3, color );
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    @param point new center
*/
void
nDebugCapsule::SetCenter( const vector3 & point )
{
    this->center = point;
}

//------------------------------------------------------------------------------
/**
    @param value new radius
*/
void
nDebugCapsule::SetRadius( const float value )
{
    this->radius = value;
}

//------------------------------------------------------------------------------
/**
    @param value new radius
*/
void
nDebugCapsule::SetLength( const float value )
{
    this->length = value;
}

//------------------------------------------------------------------------------
/**
    @param value new orientation
*/
void
nDebugCapsule::SetOrientation( const quaternion & value )
{
    this->orientation = value;
}

//------------------------------------------------------------------------------
