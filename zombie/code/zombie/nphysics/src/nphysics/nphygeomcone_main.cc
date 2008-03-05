//-----------------------------------------------------------------------------
//  nphygeomcone_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomcone.h"

//-----------------------------------------------------------------------------

const phyreal nPhyGeomCone::DefaultLength(1);
const phyreal nPhyGeomCone::DefaultRadius(1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomCone, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 15-Apr-2005   David Reyes    created
*/
nPhyGeomCone::nPhyGeomCone()
{
    this->type = Cone;

    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 15-Apr-2005   David Reyes    created
*/
nPhyGeomCone::~nPhyGeomCone()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 15-Apr-2005   David Reyes    created
*/
void nPhyGeomCone::Create()
{
    n_assert2( this->Id() == NoValidID, "Attempting to create an already created geometry" );

    geomID = phyCreateCone( DefaultLength, DefaultRadius );

    n_assert2( this->Id() != NoValidID, "Failing to create the geometry" );

    nPhysicsGeom::Create();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the geometry

    @param server     graphic server

    history:
        - 15-Apr-2005   David Reyes    created
*/
void nPhyGeomCone::Draw( nGfxServer2 *server )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    if( this->DrawShape() )
    {
        /// TODO: talk to MA
    }

    nPhysicsGeom::Draw( server );

}
#endif
//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhyGeomCone::Scale( const phyreal factor )
{
    this->SetRadius( this->GetRadius() * factor );

    this->SetLength( this->GetLength() * factor );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
