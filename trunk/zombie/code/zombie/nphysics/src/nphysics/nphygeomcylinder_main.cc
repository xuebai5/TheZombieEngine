//-----------------------------------------------------------------------------
//  nphygeomcylinder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomcylinder.h"

//-----------------------------------------------------------------------------

const phyreal nPhyGeomCylinder::DefaultLength(1);
const phyreal nPhyGeomCylinder::DefaultRadius(1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomCylinder, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 28-Sep-2004   David Reyes    created
*/
nPhyGeomCylinder::nPhyGeomCylinder()
{
    this->type = RegularFlatEndedCylinder;

    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 28-Sep-2004   David Reyes    created
*/
nPhyGeomCylinder::~nPhyGeomCylinder()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 28-Sep-2004   David Reyes    created
        - 01-Oct-2004   David Reyes    supressed name
*/
void nPhyGeomCylinder::Create()
{
    n_assert2( this->Id() == NoValidID, "Attempting to create an already created geometry" );

    geomID = phyCreateCylinder( DefaultLength, DefaultRadius );

    n_assert2( this->Id() != NoValidID, "Failing to create the geometry" );

    nPhysicsGeom::Create();
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Draws the geometry

    @param server     graphic server

    history:
        - 29-Sep-2004   David Reyes    created
        - 05-Oct-2004   David Reyes    added caps visualization
*/
void nPhyGeomCylinder::Draw( nGfxServer2 *server )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    nPhysicsGeom::Draw( server );

    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyShapes) )
        return;

    if( this->DrawShape() )
    {
        vector3 position;

        this->GetAbsolutePosition( position );

        matrix33 orientation; 
        
        quaternion q;

        this->GetAbsoluteOrientation( q );

        orientation = q;

        matrix44 transform;

        transform.set( orientation.x_component(), orientation.y_component(), orientation.z_component(), vector3(0,0,0) ); 

        transform.M14 = 0;
        transform.M24 = 0;
        transform.M34 = 0;
        transform.M44 = 1;

        matrix44 model;

        matrix44 model1;

        matrix44 model2;

        model.scale( vector3(GetRadius(), GetRadius(), GetLength()) );

        model1.scale( vector3(GetRadius(), GetRadius(), GetRadius()));

        model2 = model1;

        model = model * transform;

        model1.set_translation( vector3( 0, 0, GetLength() / phyreal(4.0) ) );

        model1 = model1 * transform;
        
        model2.set_translation( vector3( 0, 0, GetLength() / phyreal(-4.0) ) );

        model2 = model2 * transform;

        model.translate( position );    

        model1.translate( model1.pos_component() + position );    
        
        model2.translate( model2.pos_component() + position );    

        server->BeginShapes();

        server->DrawShape( nGfxServer2::Cylinder, model, phy_color_shape );
        
        server->DrawShape( nGfxServer2::Sphere, model1, phy_color_shape );
        
        server->DrawShape( nGfxServer2::Sphere, model2, phy_color_shape );

        server->EndShapes();
    }


}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
