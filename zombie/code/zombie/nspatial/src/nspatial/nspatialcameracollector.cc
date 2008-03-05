#include "precompiled/pchnspatial.h"
//-----------------------------------------------------------------------------
//  nspatialcameracollector.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nspatial/nspatialcameracollector.h"
#include "entity/nentity.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatiallight.h"

#ifndef NGAME
#include "ncnavmesh/ncnavmeshnode.h"
#endif

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 14-Feb-2005   Miquel Angel Rujula created
*/
nSpatialCameraCollector::nSpatialCameraCollector():
numVisibleEntities(0),
numVisibleTerrainCells(0)
{

}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 14-Feb-2005   Miquel Angel Rujula created
*/
nSpatialCameraCollector::~nSpatialCameraCollector()
{
    this->visibleLights.Reset();
}

//-----------------------------------------------------------------------------
/**
    Accepts a spatial element and decides to store it or not.
    
    If the 'm_checkLayers' flag is true, the collector uses the active layers 
    array to collect only the spatial elements that are on that layers. If the 
    flag is false the collector collects all the elements that receives without 
    checking the layer.

    Returns true if it stil wants to continue receiving elements, false in
    negative case. (Don't confuse! It doesn't mean that the element was 
    collected).

    history:
        - 14-Feb-2005   Miquel Angel Rujula. Created
        - 18-Feb-2005   Miquel Angel Rujula. Added checking of the active layers. 
        - 25-Apr-2005   Miquel Angel Rujula. Modified to work with entities.
        - 05-May-2005   Miquel Angel Rujula. Modified to use entity type.
        - 25-May-2005   Miquel Angel Rujula. Modified to catch the layer id from the editor component.
        - 15-Jun-2005   MA Garcias.          Modifed to accept global entities.
        - 26-Oct-2005   MA Garcias           Modified to handle LOD from spatial component.
        - 21-Feb-2006   Miquel Angel Rujula. Removed use of nVisibleEntitiesArray.
        - 05-Jul-2006   Carles Ros           Added gathering of nav mesh nodes.
*/
bool 
nSpatialCameraCollector::Accept(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    ncSpatialQuadtreeCell *spatialQuadCellComp = entity->GetComponent<ncSpatialQuadtreeCell>();

    if (spatialComponent)
    {
        // collect the element
        switch(spatialComponent->GetType())
        {
            case ncSpatial::N_SPATIAL_ELEMENT:
            case ncSpatial::N_SPATIAL_MC:
#ifndef NGAME
                {
                    ncNavMeshNode* meshNode = entity->GetComponent<ncNavMeshNode>();
                    if ( meshNode )
                    {
                        if ( meshNode->IsInOutdoor() )
                        {
                            this->navMeshNodesOutdoor.Append( meshNode );
                        }
                        else
                        {
                            this->navMeshNodesIndoor.Append( meshNode );
                        }
                        break;
                    }
                }
#endif
            case ncSpatial::N_SPATIAL_BATCH:
            case ncSpatial::N_SPATIAL_PORTAL:
                this->visibleEntities.Append(entity);
                ++this->numVisibleEntities;
                break;

            case ncSpatial::N_SPATIAL_LIGHT:
                {   
                    this->visibleLights.Append(entity->GetComponentSafe<ncSpatialLight>());
                }
                break;

            default:
                break;
        }
    }
    else if (spatialQuadCellComp)
    {
        this->visibleEntities.Append(entity);
        ++this->numVisibleTerrainCells;
    }
    else
    {
        // accept global entities
        this->visibleEntities.Append(entity);
        ++this->numVisibleEntities;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Reset all (elements container and flags)
    
    history:
        - 14-Feb-2005   Miquel Angel Rujula created
*/
void 
nSpatialCameraCollector::Reset()
{
    this->ResetContainer();
    this->ResetLightsContainer();
#ifndef NGAME
    this->ResetNavMeshNodesContainer();
#endif
}

//-----------------------------------------------------------------------------
/**
    Reset the elements container
    
    history:
        - 14-Feb-2005   Miquel Angel Rujula created
*/
void 
nSpatialCameraCollector::ResetContainer()
{
    this->visibleEntities.Reset();

    this->numVisibleEntities = 0;
    this->numVisibleTerrainCells = 0;
}

//-----------------------------------------------------------------------------
/**
    Reset the lights container
    
    history:
        - 14-Feb-2005   Miquel Angel Rujula created
*/
void 
nSpatialCameraCollector::ResetLightsContainer()
{
    this->visibleLights.Reset();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Reset the nav mesh nodes container
    
    history:
        - 05-Jul-2005   Carles Ros created
*/
void 
nSpatialCameraCollector::ResetNavMeshNodesContainer()
{
    this->navMeshNodesOutdoor.Reset();
    this->navMeshNodesIndoor.Reset();
}
#endif // !NGAME
