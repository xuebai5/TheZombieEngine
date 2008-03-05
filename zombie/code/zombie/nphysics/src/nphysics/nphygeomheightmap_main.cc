//-----------------------------------------------------------------------------
//  nphygeomheightmap_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/nphygeomheightmap.h"

#ifndef NGAME
    #include "gfx2/nlineserver.h"
    #include "nspatial/ncspatialquadtreecell.h"
#endif

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomHeightMap, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 11-Apr-2005   David Reyes    created
*/
nPhyGeomHeightMap::nPhyGeomHeightMap() :
    heights(0),
    step(0)
{
    this->type = HeightMap;
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 11-Apr-2005   David Reyes    created
*/
void nPhyGeomHeightMap::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempting to create an already created geometry" );

    n_assert2( this->heights, "No data available." );

    geomID = phyCreateHeightMap( this->heights, this->length, this->numNodes, this->step  );

    n_assert2( this->Id() != NoValidID , "Failing to create the geometry" );

    nPhysicsGeom::Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 11-Apr-2005   David Reyes    created
*/
nPhyGeomHeightMap::~nPhyGeomHeightMap()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Sets the buffer of heights

    @param buffer heights buffer

    history:
        - 11-Apr-2005   David Reyes    created
*/
void nPhyGeomHeightMap::SetHeightsBuffer( phyreal* buffer )
{
    n_assert2( buffer, "Null pointer." );

    this->heights = buffer;
}

//-----------------------------------------------------------------------------
/**
    Sets the length of the height map.

    @param length heightmap length

    history:
        - 11-Apr-2005   David Reyes    created
*/
void nPhyGeomHeightMap::SetLength( phyreal length )
{
    n_assert2( length > 0, "Error, length of a height map can't be less or equal to zero." );

    this->length = length;
}

//-----------------------------------------------------------------------------
/**
    Sets the number of nodes of the height map.

    @param numnodes heightmap nodes

    history:
        - 11-Apr-2005   David Reyes    created
*/
void nPhyGeomHeightMap::SetNumNodes( int numnodes )
{
    n_assert2( numnodes > 0, "Incorrect number of nodes." );

    this->numNodes = numnodes;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the geometry for debug pourposes.

    history:
        - 11-Apr-2005   David Reyes    created
*/
void nPhyGeomHeightMap::Draw( nGfxServer2* server )
{
    nPhysicsGeom::Draw( server );
    
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyHeightMap) )
        return;

    ncSpatialQuadtreeCell* spatial(this->GetOwner()->GetComponent<ncSpatialQuadtreeCell>());

    if( spatial )
    {
        if( !spatial->IsVisible() )
            return;
    }

    n_assert2( this->heights, "Missing data." );

    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 position;

    this->GetPosition( position );

    phyreal lengthNode(this->length/this->numNodes);

    vector3 triangle[6];

    static nLineHandler line( nGfxServer2::LineList, nMesh2::Coord | nMesh2::Color);
    
    static bool once(false);

    if( !once )
    {
        line.SetShader( "shaders:line_solid.fx" );
        once = true;
    }

    matrix44 modeltransform;

    for( int x(0); x < 3; ++x )
    {
        for( int y(0); y < 3; ++y )
        {
            modeltransform.m[y][x] = orientation.m[y][x];
        }
    }

    modeltransform.translate( position );
    
    for( int x(0); x < this->numNodes; ++x )
    {
        phyreal lengthx = x * lengthNode;
        phyreal lengthxplus = (x + 1) * lengthNode;

        vector4 color( phy_color_heightmap );

        if( !this->IsEnabled() )
            color = phy_color_heightmap_disabled;

        line.BeginLines(modeltransform);
        for( int y(0); y < this->numNodes; ++y )
        {
            if( !this->IsEnabled() )
                color = phy_color_heightmap_disabled;
            else
                color = phy_color_heightmap;

            phyreal lengthy = y * lengthNode;
            phyreal lengthyplus = (y + 1) * lengthNode;

            triangle[0].x = lengthx;
            triangle[0].z = lengthy;
            triangle[0].y = phyGetHeightInHeightMap( this->Id(), x, y );

            triangle[1].x = lengthxplus;
            triangle[1].z = lengthy;
            triangle[1].y = phyGetHeightInHeightMap( this->Id(), x+1, y );

            triangle[2] = triangle[1];

            triangle[3].x = lengthx;
            triangle[3].z = lengthyplus;
            triangle[3].y = phyGetHeightInHeightMap( this->Id(), x, y+1 );

            triangle[4] = triangle[3];

            triangle[5] = triangle[0];

            line.DrawLines3d(triangle, 0, 6, color );

        }
        line.EndLines();
    }
}


//-----------------------------------------------------------------------------
/**
    Updates the bounding box.

    history:
        - 15-Jun-2005   David Reyes    created
*/
void nPhyGeomHeightMap::UpdateAABB()
{
    phyUpdateBoundingBoxesHeightMap( this->Id() );
}

#endif


//-----------------------------------------------------------------------------
/**
    Sets a hole in the given coordinates.

    @param x x node coordinates
    @param z z node coordinates

    history:
        - 28-Jun-2005   David Reyes    created
*/
void nPhyGeomHeightMap::SetHole( int x, int z )
{
    phySetTerrainYHoleIn( this->Id(), x, z );
}

//-----------------------------------------------------------------------------
/**
    Gets a hole in the given coordinates.

    @param x x node coordinates
    @param z z node coordinates

    @return if hole present

    history:
        - 13-Jan-2006   David Reyes    created
*/
const bool nPhyGeomHeightMap::GetHole( const int x, const int z ) const
{
    return phyGetTerrainYHoleIn( this->Id(), x, z );
}


//-----------------------------------------------------------------------------
/**
    Removes all the holes.

    history:
        - 28-Jun-2005   David Reyes    created
*/
void nPhyGeomHeightMap::RemoveHoles()
{
    phyRemoveTerrainYHoles( this->Id() );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
