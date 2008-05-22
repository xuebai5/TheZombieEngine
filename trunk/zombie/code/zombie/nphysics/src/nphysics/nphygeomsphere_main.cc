//-----------------------------------------------------------------------------
//  nphygeomsphere_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/nphygeomsphere.h"

//-----------------------------------------------------------------------------

const phyreal nPhyGeomSphere::DefaultRadius(1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomSphere, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 23-Sep-2004   Zombie         created
*/
nPhyGeomSphere::nPhyGeomSphere()
{
    this->type = Sphere;

    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 23-Sep-2004   Zombie         created
*/
nPhyGeomSphere::~nPhyGeomSphere()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 23-Sep-2004   Zombie         created
        - 01-Oct-2004   Zombie         supressed name
*/
void nPhyGeomSphere::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempted to create again." );

    this->geomID = phyCreateSphere( DefaultRadius );

    n_assert2( this->Id() != NoValidID , "Hasn't been possible to create the sphere." );

    nPhysicsGeom::Create();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the geometry

    @param server     graphic server

    history:
        - 29-Sep-2004   Zombie         created
*/
void nPhyGeomSphere::Draw( nGfxServer2 *server )
{
    nPhysicsGeom::Draw( server );

    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyShapes) )
        return;

    if( this->DrawShape() )
    {
        matrix44 model;

        vector3 position;

        this->GetAbsolutePosition( position );

        phyreal radius(this->GetRadius());

        model.scale( vector3( radius, radius, radius ) );

        model.translate( position );

        server->BeginShapes();

        server->DrawShape( nGfxServer2::Sphere, model, phy_color_shape );

        server->EndShapes();
    }

}
#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
