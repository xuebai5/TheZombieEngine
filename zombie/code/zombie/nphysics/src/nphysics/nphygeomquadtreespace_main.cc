//-----------------------------------------------------------------------------
//  nphygeomquadtreespace_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomQuadTreeSpace, "nphyspace");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 11-Oct-2004   Zombie         created
*/
nPhyGeomQuadTreeSpace::nPhyGeomQuadTreeSpace() :
    containerGeometries(NumInitialGeometries,NumGrowthGeometries)
{
    this->type = QuadTreeBasedSpace;
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 11-Oct-2004   Zombie         created
*/
nPhyGeomQuadTreeSpace::~nPhyGeomQuadTreeSpace()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 11-Oct-2004   Zombie         created
*/
void nPhyGeomQuadTreeSpace::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempting to create an object already created." );

    this->geomID = phyCreateQuadTreeSpace(this->centerSpace, this->extensSpace, this->depthSpace);    

    n_assert2( this->Id() != NoValidID , "Hasn't been possible to create the space" );    

    nPhysicsGeom::Create();
}

//-----------------------------------------------------------------------------
/**
    Sets up the space before to be created.

    @param center   where this space has the center.
    @param extens   size of the space
    @param depth    how many divisions 4^depth

    history:
        - 11-Oct-2004   Zombie         created
*/
void nPhyGeomQuadTreeSpace::SetUp( const vector3& center, const vector3& extens, int depth )
{
    n_assert2( !this->Id() , "It cannot be call after creating the space" );

    n_assert2( depth , "Depth cannot be zero" );

    this->centerSpace = center;

    this->extensSpace = extens;

    this->depthSpace = depth;
}

//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param boundingbox  data structure to be filled

    history:
        - 11-Oct-2004   Zombie         created
*/
void nPhyGeomQuadTreeSpace::GetAABB( nPhysicsAABB& boundingbox )
{
    // NOTE: pre-calculate due it's not gonna change of size or place
    boundingbox.minx = this->centerSpace.x + -(this->extensSpace.x / phyreal(2.0));
    boundingbox.maxx = this->centerSpace.x +  (this->extensSpace.x / phyreal(2.0));
    boundingbox.miny = this->centerSpace.y + -(this->extensSpace.y / phyreal(2.0));
    boundingbox.maxy = this->centerSpace.y +  (this->extensSpace.y / phyreal(2.0));
    boundingbox.minz = this->centerSpace.z + -(this->extensSpace.z / phyreal(2.0));
    boundingbox.maxz = this->centerSpace.z +  (this->extensSpace.z / phyreal(2.0));
}

//-----------------------------------------------------------------------------
/**
    Returns a geometry by index.

    @param geometry index index of the geometry to be retrieved
    
    @return collision geometry

    history:
        - 23-Sep-2005   Zombie         created
*/
nPhysicsGeom* nPhyGeomQuadTreeSpace::GetGeometry( int index )
{
    return this->containerGeometries[ index ];
}

//-----------------------------------------------------------------------------
/**
    Adds a geometry to the space.

    @param geometry geometry collision geometry

    history:
        - 23-Sep-2005   Zombie         created
*/
void nPhyGeomQuadTreeSpace::Add( nPhysicsGeom* geometry )
{
    this->containerGeometries.Add( phyGeomIDToInt( geometry->Id() ), geometry );

    nPhySpace::Add( geometry );
}

//-----------------------------------------------------------------------------
/**
    Removes a geometry to the space.

    @param geometry geometry collision geometry

    history:
        - 23-Sep-2005   Zombie         created
*/
void nPhyGeomQuadTreeSpace::Remove( nPhysicsGeom* geometry )
{
    this->containerGeometries.Rem( phyGeomIDToInt( geometry->Id() ) );

    nPhySpace::Remove( geometry );
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries contained in the space.

    @return number of geometries

    history:
        - 23-Sep-2005   Zombie         created
*/
int nPhyGeomQuadTreeSpace::GetNumGeometries() const
{
    return this->containerGeometries.Size();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
