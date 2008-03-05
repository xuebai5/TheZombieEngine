//-----------------------------------------------------------------------------
//  nphygeomplane_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomplane.h"

#ifndef NGAME
    #include "nphysics/nphysicsserver.h"
#endif

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomPlane, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 12-Jan-2005   David Reyes    created
*/
nPhyGeomPlane::nPhyGeomPlane() :
    planeequation( 0, 0, 0, 0 )
{
    this->type = InfinitePlane;
    
    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 12-Jan-2005   David Reyes    created
*/
nPhyGeomPlane::~nPhyGeomPlane()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 12-Jan-2005   David Reyes    created
*/
void nPhyGeomPlane::Create()
{
    n_assert2( this->Id() == NoValidID, "Attempting to create an already created geometry" );

    geomID = phyCreatePlane( planeequation );

    n_assert2( this->Id() != NoValidID, "Error creating a geometry" );

    nPhysicsGeom::Create();    
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this geometry (doesn't affect).

    @param newposition new geometry position

    history:
        - 12-Jan-2005   David Reyes    created
*/
void nPhyGeomPlane::SetPosition( const vector3& /*newposition*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns current position.

    @param position world position

    history:
        - 12-Jan-2005   David Reyes    created
*/
void nPhyGeomPlane::GetPosition( vector3& position ) const
{
    position.x = this->GetEquation().x * this->GetEquation().w;
    position.y = this->GetEquation().y * this->GetEquation().w;
    position.z = this->GetEquation().z * this->GetEquation().w;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the plane.

    history:
        - 12-Jan-2005   David Reyes    created
*/
void nPhyGeomPlane::Draw( nGfxServer2* server )
{
    nPhysicsGeom::Draw( server );    
 
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyShapes) )
        return;

    if( nPhysicsServer::Instance()->isDrawingAlphaShapes() == false )
    {
        nPhysicsServer::Instance()->ListAlphaShapes().PushBack(this);
        return;
    }

    if( this->DrawShape() )
    {
        vector3 position(
            this->GetEquation().x*this->GetEquation().w-.0005f,
            this->GetEquation().y*this->GetEquation().w-.0005f,
            this->GetEquation().z*this->GetEquation().w-.0005f
            );

        matrix33 orientation; 

        orientation.rotate( vector3( -this->GetEquation().z, 0, this->GetEquation().x ), acos( this->GetEquation().y ) );

        matrix44 transform;

        transform.set( orientation.x_component(), orientation.y_component(), orientation.z_component(), vector3(0,0,0) ); 

        transform.M14 = 0;
        transform.M24 = 0;
        transform.M34 = 0;
        transform.M44 = 1;   

        matrix44 model;

        vector3 lengths( 100000.0f, 0.01f, 100000.0f );

        model.scale( vector3( lengths.x,lengths.y , lengths.z) );

        model = model * transform;

        model.translate( position );    

        server->BeginShapes();

        int col1(int(reinterpret_cast<size_t>(this)) & 0x0f);
        int col2((int(reinterpret_cast<size_t>(this)) & 0xf0) >> 4);
        server->DrawShape( nGfxServer2::Box, model, vector4(0.0f, col1/16.0f, col2/16.0f, 0.5f ) );

        server->EndShapes();
    }

}
#endif

//-----------------------------------------------------------------------------
/**
    Sets plane equation.

    @param equation plane equation coeficients

    history:
        - 12-Jan-2005   David Reyes    created
*/
void nPhyGeomPlane::SetEquation( const vector4& equation )
{
    n_assert2( this->Id() != NoValidID, "Error the plane hasn't been created" );

    // testing that the plane normal has lenght one.
    vector3 planeNormal( equation.x, equation.y, equation.z );

    planeNormal.norm();

    vector4 equationPlane( planeNormal.x, planeNormal.y, planeNormal.z, equation.w );

    this->planeequation = equationPlane;

    phyPlaneSetEquation( this->Id(), equationPlane );
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of this geometry (doesn't affect).

    @param neworientation matrix new orientation

    history:
        - 08-Aug-2005   David Reyes    created
*/
void nPhyGeomPlane::SetOrientation( const matrix33& /*neworientation*/)
{    
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns current orientation (doesn't affect).

    @param orientation matrix orientation

    history:
        - 08-Aug-2005   David Reyes    created
*/
void nPhyGeomPlane::GetOrientation( matrix33& /*orientation*/) const
{    
    // Empty
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
