#ifndef N_PHYGEOMQUADTREESPACE_H
#define N_PHYGEOMQUADTREESPACE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomQuadTreeSpace
    @ingroup NebulaPhysicsSystem
    @brief A quadtree space physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Quad Tree Plane
    
    @cppclass nPhyGeomQuadTreeSpace
    
    @superclass nPhySpace

    @classinfo A quadtree space physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphyspace.h"

//-----------------------------------------------------------------------------
class nPhyGeomQuadTreeSpace : public nPhySpace 
{
public:
    /// constructor
    nPhyGeomQuadTreeSpace();
            
    /// destructor
    ~nPhyGeomQuadTreeSpace();

    /// sets up the space before to be created
    void SetUp( const vector3& center, const vector3& extens, int depth );

    /// creates the geometry
    void Create();

    /// returns the bounding box of this geometry
    void GetAABB( nPhysicsAABB& boundingbox );

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// adds a geometry to the space
    void Add( nPhysicsGeom* geometry );

    /// removes a geometry to the space
    void Remove( nPhysicsGeom* geometry );

    /// returns a geometry by index
    nPhysicsGeom* GetGeometry( int index );

    /// returns the number of geometries contained in the space
    int GetNumGeometries() const;

private:
    /// stores the center for this space
    vector3 centerSpace;

    /// stores the extens for this space
    vector3 extensSpace;

    /// stores how deep is the space
    int depthSpace;

    /// type of container
    typedef nKeyArray< nPhysicsGeom* > tContainerGeometries;

    /// geometries container
    tContainerGeometries containerGeometries;

    /// number of initial geometries space
    static const int NumInitialGeometries = 10;

    /// growth pace
    static const int NumGrowthGeometries = 10;

};

#endif 
