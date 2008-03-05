#ifndef NC_SPATIALINDOOR_H
#define NC_SPATIALINDOOR_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialIndoor
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial indoor component for indoor entities.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nspatial/ncspatialspace.h"

class nSpatialModel;

//------------------------------------------------------------------------------
class ncSpatialIndoor : public ncSpatialSpace
{

    NCOMPONENT_DECLARE(ncSpatialIndoor, ncSpatialSpace);

public:

    /// constructor
    ncSpatialIndoor();
    /// destructor
    virtual ~ncSpatialIndoor();

    /// Initialize the entity instance
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// destroy all the space
    virtual void DestroySpace();

#ifndef NGAME
    /// remove the entity from the spaces without result check
    void DoEnterLimbo();
    /// register the space to the spatial server
    void DoLeaveLimbo();
#endif // !NGAME

    /// add an entity to the space, and put it in the corresponding cell
    virtual bool AddEntity(nEntityObject *entity);

    /// searches the cell that contains the given point
    virtual ncSpatialCell *SearchCellContaining(const vector3 &point, const int flags) const;
    /// searches the cell that contains the given bounding box
    virtual ncSpatialCell *SearchCellContaining(const bbox3 &box) const;
    /// search the inner cell in the tree that completely contains the given sphere
    virtual ncSpatialCell *SearchCellContaining(const sphere &sph) const;
    /// searches the cell that contains the given model (by now it only works for sphere models)
    virtual ncSpatialCell *SearchCellContaining(const nSpatialModel *model) const;

    /// searches all the cells intersecting with the given bounding box
    virtual void SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches all the cells intersecting with the given sphere
    virtual void SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells) const;

    /// searches all the entities intersecting with the given bounding box
    virtual void SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities) const;
    /// searches all the entities intersecting with the given sphere
    virtual void SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities) const;

    /// searches the cells whose wrapper meshes contain the given point 
    virtual void SearchCellsContaining(const vector3 &point, nArray<ncSpatialCell*> *cells, 
        const int flags) const;
    /// searches the cells whose bounding boxes completely contain the given bounding box
    virtual void SearchCellsContaining(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches the cells whose bounding boxes completely contain the given sphere
    virtual void SearchCellsContaining(const sphere &sph, nArray<ncSpatialCell*> *cells) const;
    /// searches all the entities whose position is contained into the given sphere
    virtual void SearchEntitiesContainedByPos(const sphere &sph, nArray<nEntityObject*> *entities) const;

    /// get the all the indoor brushes into this indoor
    void GetIndoorBrushes(nArray<nEntityObject*> &indoorBrushes) const;
    /// get the all the portals into this indoor
    void GetPortals(nArray<ncSpatialPortal*> &portals) const;
    /// get the entities that are not indoor brushes nor portals
    void GetDynamicEntities(nArray<nEntityObject*> &entitiesArray) const;
    /// get the portals that point to other indoors
    void GetOtherIndoorsPortals(nArray<ncSpatialPortal*> &otherIndoorsPortals) const;
    /// get the portals that point to the outdoor
    void GetOutdoorPortals(nArray<ncSpatialPortal*> &outdoorPortals) const;
    /// get the portals that point to the global space
    void GetGlobalPortals(nArray<ncSpatialPortal*> &globalPortals) const;
    /// get the portals that are disconnected
    void GetDisconnectedPortals(nArray<ncSpatialPortal*> &disconnectedPortals) const;
    /// update space
    virtual void Update(const matrix44 &matrix);
    /// get facade entity
    nEntityObject *GetFacade();
    /// set the transform facade flag
    void SetTransformFacade(bool flag);
    /// get the transform facade flag
    bool GetTransformFacade() const;
    
    /// get the occluders array (if any)
    const nArray<nEntityObject*> *GetOccluders();

#ifndef NGAME
    /// set the layer id to all the entities into this space
    void SetLayerId(int layerId);
    /// enable/disable the indoor
    void SetEnable(bool value);
    /// is this indoor enabled?
    bool IsEnabled() const;
#endif

    /// disconnect the indoor space completely, this is, of the outdoors and 
    /// the other indoors
    void Disconnect();
    /// disconnect the indoor space temporary, this is, of the outdoors and 
    /// the other indoors and mantein the other indoors connected
    void DisconnectTemporary();
    /// disconnect the indoor space of the other indoors
    void DisconnectFromIndoors();
    /// connect the indoor space to other spaces (indoors and outdoor)
    void Connect();
    /// connect the indoor space to the outdoors
    void ConnectToOutdoor();
#ifndef NGAME
    /// remove temporary all the indoor
    void RemoveTemporary();
    /// restore this indoor's objects from the temporary array
    void RestoreFromTempArray();
#endif // !NGAME

#ifndef NGAME
#endif // !NGAME

protected:

    friend class nVisibleFrustumVisitor;

    /// indoor's facade
    nEntityObject *m_facade;
    /// tells if when this indoor is moved it has to also move its facade
    bool m_transformFacade;

    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByBBox(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByPos(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByModel(nEntityObject *entity, const int flags);

    /// read the information contained in a TiXmlElement and fill the space with it
    virtual bool ReadCellsStaticInfo(const TiXmlElement *spaceElem);

    /// transform indoor's facade
    void TransformFacade(const matrix44 &matrix);

    /// occluders associated with this indoor (it can be NULL, because the indoor has no occluders)
    nArray<nEntityObject*> *m_occluders;

#ifndef NGAME
    bool enabled;
#endif // !NGAME

};

//------------------------------------------------------------------------------
/**
    get facade entity
*/
inline
nEntityObject *
ncSpatialIndoor::GetFacade()
{
    return this->m_facade;
}

//------------------------------------------------------------------------------
/**
    set the transform facade flag
*/
inline
void 
ncSpatialIndoor::SetTransformFacade(bool flag)
{
    this->m_transformFacade = flag;
}

//------------------------------------------------------------------------------
/**
    get the transform facade flag
*/
inline
bool 
ncSpatialIndoor::GetTransformFacade() const
{
    return this->m_transformFacade;
}

//------------------------------------------------------------------------------
/**
    get the occluders array (if any)
*/
inline
const nArray<nEntityObject*> *
ncSpatialIndoor::GetOccluders()
{
    return this->m_occluders;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    enable/disable the indoor
*/
inline
void 
ncSpatialIndoor::SetEnable(bool value)
{
    this->enabled = value;
}

//------------------------------------------------------------------------------
/**
    is this indoor enabled?
*/
inline
bool 
ncSpatialIndoor::IsEnabled() const
{
    return this->enabled;
}
#endif // !NGAME

#endif // NC_SPATIALINDOOR_H

