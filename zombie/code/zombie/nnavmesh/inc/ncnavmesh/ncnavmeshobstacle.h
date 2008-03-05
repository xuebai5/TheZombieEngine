#ifndef NC_NAVMESHOBSTACLE_H
#define NC_NAVMESHOBSTACLE_H

//------------------------------------------------------------------------------
/**
    @class ncNavMeshObstacle
    @ingroup NebulaNavmeshSystem

    Navigation mesh obstacle component object

    This components provides a mean to have a navmesh obstacle defined as an
    entity, which is usefull to insert it in the spatial server and speed up
    queries for obstacles.

    This component only kepts a reference to the obstacle's polygonal
    represenation, it doesn't delete the polygon when this component is
    destroyed.
*/

#include "entity/nentity.h"

class polygon;

//------------------------------------------------------------------------------
class ncNavMeshObstacle : public nComponentObject
{

    NCOMPONENT_DECLARE(ncNavMeshObstacle,nComponentObject);

public:
    /// Default constructor
    ncNavMeshObstacle();
    /// Destructor
    ~ncNavMeshObstacle();
    /// Set the obstacle's polygonal representation
    void SetPolygonalRepresentation( polygon* poly );
    /// Get the obstacle's polygonal representation
    polygon* GetPolygonalRepresentation() const;

private:
    /// The obstacle's polygonal representation
    polygon* obstacle;
};

//-----------------------------------------------------------------------------
/**
    Set the obstacle's polygonal representation
*/
inline
void ncNavMeshObstacle::SetPolygonalRepresentation( polygon* poly )
{
    this->obstacle = poly;
}

//-----------------------------------------------------------------------------
/**
    Get the obstacle's polygonal representation
*/
inline
polygon* ncNavMeshObstacle::GetPolygonalRepresentation() const
{
    return this->obstacle;
}

#endif
