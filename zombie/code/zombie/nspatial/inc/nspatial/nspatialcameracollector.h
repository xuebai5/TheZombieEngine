#ifndef N_SPATIALCAMERACOLLECTOR_H
#define N_SPATIALCAMERACOLLECTOR_H
//------------------------------------------------------------------------------
/**
    @class nSpatialCameraCollector
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialCameraCollector collects visible entities from a spatial camera
    
    (C) 2005  Conjurer Services, S.A.
*/

#include "nspatial/nspatialcollector.h"
#include "util/narray.h"

class ncSpatialLight;

#ifndef NGAME
class ncNavMeshNode;
#endif

class nSpatialCameraCollector: public nSpatialCollector
{
public:

    /// constructor
    nSpatialCameraCollector();
    /// destructor
    virtual ~nSpatialCameraCollector();

    /// check an entity and store it
    virtual bool Accept(nEntityObject *entity);

    /// reset all
    virtual void Reset();
    /// reset the elements container
    void ResetContainer();
    /// reset the lights container
    void ResetLightsContainer();
#ifndef NGAME
    /// reset the nav mesh nodes container
    void ResetNavMeshNodesContainer();
#endif

    /// get visible entities array
    const nArray<nRef<nEntityObject> >& GetVisibleEntities() const;
    /// get number of visible entities (elements only)
    int GetNumVisibleObjects();
    /// get number of visible terrain cells
    int GetNumVisibleTerrainCells();

    /// get visible lights array
    const nArray<ncSpatialLight*>& GetVisibleLights();

#ifndef NGAME
    /// get visible nav mesh nodes for outdoor
    const nArray<ncNavMeshNode*>& GetVisibleNavMeshNodesOutdoor() const;
    /// get visible nav mesh nodes for indoor
    const nArray<ncNavMeshNode*>& GetVisibleNavMeshNodesIndoor() const;
#endif

private:

    /// array of visible entities
    nArray<nRef<nEntityObject> > visibleEntities;

    /// number of visible entities
    int numVisibleEntities;
    /// number of visible terrain cells
    int numVisibleTerrainCells;

    /// array of visible ligths
    nArray<ncSpatialLight*> visibleLights;

#ifndef NGAME
    /// array of visible nav mesh nodes for the outdoor (excluding walkable brushes)
    nArray<ncNavMeshNode*> navMeshNodesOutdoor;
    /// array of visible nav mesh nodes for the indoors (including walkable brushes)
    nArray<ncNavMeshNode*> navMeshNodesIndoor;
#endif
};

//-----------------------------------------------------------------------------
/**
    Get visible entities array
    
    history:
        - 25-Apr-2005   Miquel Angel Rujula created
        - 26-Oct-2005   MA Garcias          use nVisibleEntityArray
        - 21-Feb-2006   Miquel Angel Rujula removed use of nVisibleEntityArray
*/
inline
const nArray<nRef<nEntityObject> >&
nSpatialCameraCollector::GetVisibleEntities() const
{
    return this->visibleEntities;
}

//-----------------------------------------------------------------------------
/**
*/
inline
int
nSpatialCameraCollector::GetNumVisibleObjects()
{
    return this->numVisibleEntities;
}

//-----------------------------------------------------------------------------
/**
*/
inline
int
nSpatialCameraCollector::GetNumVisibleTerrainCells()
{
    return this->numVisibleTerrainCells;
}

//-----------------------------------------------------------------------------
/**
    Get visible lights array    
    
    history:
        - 14-Feb-2005   Miquel Angel Rujula. Created
        - 05-May-2005   Miquel Angel Rujula. Modified to return entities
        - 09-Nov-2005   MA.Garcias. Modified to return custom struct.
*/
inline
const nArray<ncSpatialLight*>&
nSpatialCameraCollector::GetVisibleLights()
{
    return this->visibleLights;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
*/
inline
const nArray<ncNavMeshNode*>&
nSpatialCameraCollector::GetVisibleNavMeshNodesOutdoor() const
{
    return this->navMeshNodesOutdoor;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const nArray<ncNavMeshNode*>&
nSpatialCameraCollector::GetVisibleNavMeshNodesIndoor() const
{
    return this->navMeshNodesIndoor;
}
#endif // !NGAME

#endif

