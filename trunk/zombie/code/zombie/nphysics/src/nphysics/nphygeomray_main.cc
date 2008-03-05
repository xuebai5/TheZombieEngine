//-----------------------------------------------------------------------------
//  nphygeomray_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomray.h"

//-----------------------------------------------------------------------------

const phyreal nPhyGeomRay::DefaultLength(1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomRay, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 24-Sep-2004   David Reyes    created
*/
nPhyGeomRay::nPhyGeomRay() : 
    lengthRay( DefaultLength )
{
    this->type = Ray;
    
    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 24-Sep-2004   David Reyes    created
*/
nPhyGeomRay::~nPhyGeomRay()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 24-Sep-2004   David Reyes    created
        - 01-Oct-2004   David Reyes    supress name
*/
void nPhyGeomRay::Create()
{
    n_assert2( this->Id() == NoValidID, "Attempting to create an already created geometry" );

    geomID = phyCreateRay( DefaultLength );

    n_assert2( this->Id() != NoValidID, "Error creating a geometry" );

    nPhysicsGeom::Create();
}


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the ray

    @param server  graphic server

    history:
        - 24-Sep-2004   David Reyes    created
*/
void nPhyGeomRay::Draw( nGfxServer2* server )
{
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyRays) )
        return;

    if( this->DrawShape() )
    {
        n_assert2( this->Id() != NoValidID, "No valid geometry" );

        vector3 positions[2];

        // Gets start position
        this->GetPosition( positions[0] );
        // Gets end position
        this->GetEndPosition( positions[1] );

        server->BeginLines();

        server->DrawLines3d( positions, 2, phy_color_ray );

        server->EndLines();    
    }
}
#endif
//-----------------------------------------------------------------------------
/**
    Returns the end position of the ray

    @param position  end position

    history:
        - 24-Sep-2004   David Reyes    created
*/
void nPhyGeomRay::GetEndPosition( vector3& position ) const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    vector3 direction;

    this->GetPosition( position );

    phyreal length( this->GetLength() );

    this->GetDirection( direction );

    direction = direction * length;

    position += direction;
}

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhyGeomRay::Scale( const phyreal factor )
{
    n_assert2( factor != 0, "Can't scale with a factor of 0." );

    phyreal len(this->GetLength());

    len *= factor;

    this->SetLength( len );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
