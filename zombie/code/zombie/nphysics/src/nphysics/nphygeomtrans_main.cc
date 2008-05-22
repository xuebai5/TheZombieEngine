//-----------------------------------------------------------------------------
//  nphygeombox_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomTrans, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 18-Oct-2004   Zombie         created
*/
nPhyGeomTrans::nPhyGeomTrans() :
    encapsulatedGeom(0)
{
    this->type = GeometryTransform;

    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 18-Oct-2004   Zombie         created
*/
nPhyGeomTrans::~nPhyGeomTrans()
{

}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 18-Oct-2004   Zombie         created
*/
void nPhyGeomTrans::Create()
{
    this->geomID = phyCreateGeomTrans();

    nPhysicsGeom::Create();
}

//-----------------------------------------------------------------------------
/**
    Adds a geometry to be encapsulated

    @param geom it's the geom to be encapsulated

    history:
        - 18-Oct-2004   Zombie         created
*/
void nPhyGeomTrans::AddGeometry( nPhysicsGeom* geom )
{

    if( this->encapsulatedGeom )
    {
        this->encapsulatedGeom->Release();
    }

    this->encapsulatedGeom = geom;


    if( !geom )
    {
        return;
    }

    /// setting this is the transform
    geom->SetTransform( this );

    /// It can't belong to a space in its own
    this->encapsulatedGeom->MoveToSpace(0);

    /// ataching the geometry
    phyGeomTransSetGeometry( this->Id(), geom->Id() );

    /// inheret the collision flags
    this->SetCategories( geom->GetCategories() );
    this->SetCollidesWith( geom->GetCollidesWith() );

    /// inheret material
    this->SetMaterial( geom->GetMaterial() );

    /// inheret attributes
    this->SetAttributes( geom->GetAttributes() );

    /// share the same owner
    geom->SetOwner( this->nPhysicsGeom::GetOwner() );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draw.

    @return encapsulated geometry

    history:
        - 05-Nov-2004   Zombie         created
*/
void nPhyGeomTrans::Draw( nGfxServer2 *server )
{
    if( this->encapsulatedGeom )
    {
        encapsulatedGeom->Draw( server );
        nPhysicsGeom::Draw( server );
    }
}
#endif
//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void nPhyGeomTrans::Scale( const phyreal factor )
{
    nPhysicsGeom* geometry(this->GetGeometry());

    if( geometry )
    {
        geometry->Scale( factor );

        vector3 position;

        geometry->GetPosition( position );

        position *= factor;

        geometry->SetPosition( position );
    }
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
