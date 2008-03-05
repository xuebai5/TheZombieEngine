#ifndef NC_SPATIALSPACE_H
#define NC_SPATIALSPACE_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialSpace
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial space component for space entities.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nspatial/ncspatialcell.h"
#include "kernel/nroot.h"
#include "kernel/nfileserver2.h"
#include "mathlib/matrix.h"
#include "mathlib/bbox.h"
#include "mathlib/obbox.h"
#include "mathlib/sphere.h"
#include "util/narray.h"
#include "tinyxml/tinyxml.h"

class ncSpatialCell;
class nSpatialModel;
class sphere;
class nEntityObject;

//------------------------------------------------------------------------------
class ncSpatialSpace : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpatialSpace,nComponentObject);

    friend class nIndoorBuilder;

public:

    enum {
        // extend with your own space types in a subclass
        N_SPATIAL_SPACE,  /// base type (this one)
        N_INDOOR_SPACE,   /// indoor space
        N_QUADTREE_SPACE, /// quadtree space
        N_OCTREE_SPACE,   /// octree space
        N_GLOBAL_SPACE    /// global space
    };

    typedef int nSpatialSpaceType;

    /// constructor
    ncSpatialSpace();
    /// destructor
    virtual ~ncSpatialSpace();

    /// flush all the lights in the space
    virtual void FlushAllLights();

    /// Set current cell
    void SetCellId(int);
    /// Add an entity id to the cell
    void AddEntityId(nEntityObjectId);
    /// Set wizard key id
    void SetWizard(int);

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// save all the entities in the space
    virtual bool Save();
    /// save the cells structure
    virtual bool SaveCellsStructure(const char *file);
    /// load the entities contained in the space
    virtual bool Load();
    /// load the cells structure
    virtual bool LoadCellsStructure(const char *file);

    /// destroy all the space
    virtual void DestroySpace();
    /// add a cell to the space
    virtual void AddSpatialCell(ncSpatialCell *cell);
    /// remove a cell from the space
    virtual bool RemoveSpatialCell(ncSpatialCell *cell);
    /// remove a cell from the space and destroys it
    virtual bool DestroySpatialCell(ncSpatialCell *cell);
    /// get the cells array
    nArray<ncSpatialCell*>& GetCellsArray();
    /// add an entity to the space, and put it in the corresponding cell
    /// Rewrite it in subclasses
    virtual bool AddEntity(nEntityObject *entity);
    /// add an entity to the space in the corresponding cell and category, depending
    /// on the flags
    virtual bool AddEntity(nEntityObject *entity, int flags);
    /// remove an entity from the space
    virtual bool RemoveEntity(nEntityObject *entity);
    /// move an entity from a cell to another one. The spatial component of the entity 
    /// contains the origin cell
    virtual bool MoveEntity(nEntityObject *entity, ncSpatialCell *destCell);

    /// set space's bounding box
    void SetBBox(const bbox3 &box);
    /// get space's bounding box
    bbox3 &GetBBox();
    /// set axis-aligned bounding box
    void SetBBox(float, float, float, float, float, float);
    /// get axis-aligned bounding box
    void GetBBox(vector3&, vector3&);
    /// set space's original bounding box
    void SetOriginalBBox(const bbox3 &box);
    /// get space's original bounding box
    bbox3 &GetOriginalBBox();
    /// set the transformation matrix
    void SetTransformMatrix(const matrix44 &matrix);
    /// get the transformation matrix
    matrix44 &GetTransformMatrix();
    /// get the number of entities in the space
    int GetNumEntities() const;
    /// add a portal that needs to solve its other side cell pointer
    void AddUnsolvedPortal(ncSpatialPortal* portal);
    /// search a cell with a given identifier
    virtual ncSpatialCell* SearchCellById(int cellId) const;
    /// searches the cell that contains the given point
    virtual ncSpatialCell *SearchCellContaining(const vector3 &point, const int flags) const;
    /// search the cell in the space that completely contains the given box
    virtual ncSpatialCell *SearchCellContaining(const bbox3 &box) const;
    /// search the inner cell in the tree that completely contains the given sphere
    virtual ncSpatialCell *SearchCellContaining(const sphere &sph) const;
    /// searches the cell that contains the given model (by now it only works for sphere models)
    virtual ncSpatialCell *SearchCellContaining(const nSpatialModel *model) const;

    /// searches all the cells intersecting with the given bounding box
    virtual void SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches all the cells intersecting with the given sphere
    virtual void SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells) const;

    /// searches all the elements intersecting with the given bounding box
    virtual void SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities) const;
    /// searches all the entities intersecting with the given sphere
    virtual void SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities) const;
    /// searches all the entities whose position is contained into the given sphere
    virtual void SearchEntitiesContainedByPos(const sphere &sph, nArray<nEntityObject*> *entities) const;

    /// searches the cells that contain the given point 
    virtual void SearchCellsContaining(const vector3 &point, nArray<ncSpatialCell*> *cells, 
        const int flags) const;
    /// searches the cells that contain the given bounding box
    virtual void SearchCellsContaining(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches the cells that contain the given sphere
    virtual void SearchCellsContaining(const sphere &sph, nArray<ncSpatialCell*> *cells) const;


    /// get all the entities of a given category, using the box and the flags
    virtual bool GetEntitiesCategory(const bbox3 &box,
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities) const;
    /// get all the entities of a given category, using the sphere and the flags
    virtual bool GetEntitiesCategory(const sphere &sph,
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities) const;
    /// get all the entities of a given category, using the point and the flags
    virtual bool GetEntitiesCategory(const vector3 &point,
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities) const;

    /// get the entities of various categories using the given flags
    virtual bool GetEntitiesCategories(const bbox3 &box, 
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities) const;
    /// get the entities of various categories using the given flags
    virtual bool GetEntitiesCategories(const sphere &sph, 
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities) const;
    /// get the entities of various categories using the given flags
    virtual bool GetEntitiesCategories(const vector3 &point, 
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities) const;



    /// get all the entities of a given category, using the box and the flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategory(const bbox3 &box,
                                          int category, 
                                          const int flags, 
                                          nArray<nEntityObject*> &entities,
                                          nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the sphere and the flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategory(const sphere &sph,
                                          int category, 
                                          const int flags, 
                                          nArray<nEntityObject*> &entities,
                                          nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the point and the flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategory(const vector3 &point,
                                          int category, 
                                          const int flags, 
                                          nArray<nEntityObject*> &entities,
                                          nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories using the given flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategories(const bbox3 &box, 
                                            const nArray<int> &categories,
                                            const int flags, 
                                            nArray<nEntityObject*> &entities,
                                            nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategories(const sphere &sph, 
                                            const nArray<int> &categories,
                                            const int flags, 
                                            nArray<nEntityObject*> &entities,
                                            nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags. It returns the intersecting cells
    virtual bool GetEntitiesCellsCategories(const vector3 &point, 
                                            const nArray<int> &categories,
                                            const int flags, 
                                            nArray<nEntityObject*> &entities,
                                            nArray<ncSpatialCell*> &cells) const;

    
    /// get all the entities of a given category, using the box and the flags and the given cells
    virtual bool GetEntitiesUsingCellsCategory(const bbox3 &box,
                                               int category, 
                                               const int flags, 
                                               nArray<nEntityObject*> &entities,
                                               const nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the sphere and the flags and the given cells
    virtual bool GetEntitiesUsingCellsCategory(const sphere &sph,
                                               int category, 
                                               const int flags, 
                                               nArray<nEntityObject*> &entities,
                                               const nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the point and the flags and the given cells
    virtual bool GetEntitiesUsingCellsCategory(const vector3 &point,
                                               int category, 
                                               const int flags, 
                                               nArray<nEntityObject*> &entities,
                                               const nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories using the given flags and the given cells
    virtual bool GetEntitiesUsingCellsCategories(const bbox3 &box, 
                                                 const nArray<int> &categories,
                                                 const int flags, 
                                                 nArray<nEntityObject*> &entities,
                                                 const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags and the given cells
    virtual bool GetEntitiesUsingCellsCategories(const sphere &sph, 
                                                 const nArray<int> &categories,
                                                 const int flags, 
                                                 nArray<nEntityObject*> &entities,
                                                 const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags and the given cells
    virtual bool GetEntitiesUsingCellsCategories(const vector3 &point, 
                                                 const nArray<int> &categories,
                                                 const int flags, 
                                                 nArray<nEntityObject*> &entities,
                                                 const nArray<ncSpatialCell*> &cells) const;

    /// returns the type of cell
    virtual nSpatialSpaceType GetType() const;
#ifndef NGAME
    /// set the layer id to all the entities into this space
    virtual void SetLayerId(int layerId);
#endif
    /// update space
    virtual void Update(const matrix44 &matrix);
    /// assign the pointer to the m_otherSide variable (cell the portal is pointing to) 
    /// of the portals in m_portalsArray, using cell's id contained in each portal
    void SolvePortalsPointers();
    /// get the spatial path
    const char *GetSpatialPath() const;
    
protected:

    /// array of entities ids for the space
    nArray<nEntityObjectId> m_entitiesIdArray;
    /// array of entity ids by cell
    ncSpatialCell* currentCell;

    /// space's bounding box
    bbox3 m_bbox;
    /// space's original bounding box
    bbox3 m_originalBBox;
    /// cells contained by this space
    nArray<ncSpatialCell*> m_cellsArray;
    /// temporal array of portals, used to solve the other side cell pointer
    /// in the portals during the load of a space
    nArray<ncSpatialPortal*> m_portalsArray;
    /// transformation matrix
    matrix44 m_transformationMatrix;
    /// space's type
    nSpatialSpaceType m_spaceType;
    /// spatial path
    const char *m_spatialPath;
    /// list of entity ids has already been loaded
    bool isLoaded;

    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByBBox(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByPos(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    virtual bool AddEntityByModel(nEntityObject *entity, const int flags);

    /// methods used to load

    /// load the entities contained in the space
    virtual bool LoadOld();

    /// read the information contained in a TiXmlElement and fill the space with it
    virtual bool ReadStaticInfo(const TiXmlElement *spaceElem);
    /// catch the information corresponding to the cells contained in spaceElem
    virtual bool ReadCellsStaticInfo(const TiXmlElement *spaceElem);
    
    /// methods used to save

    /// save all the entities in the space
    virtual bool SaveEntities(const char *fileName);

    /// qsort() hook to compare two entity ids
    static int __cdecl EntitiesSorter(const void* elm0, const void* elm1);

    /// get the static information of this space
    virtual bool WriteStaticInfo(TiXmlElement *spaceElem);

    /// fill TiXmlElements corresponding to space's cells and attach them to spaceElem
    virtual bool AddXmlElementsForCells(TiXmlElement *spaceElem);

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialSpace::SetWizard( int /*number*/ )
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
    get the cells array
*/
inline
nArray<ncSpatialCell*>&
ncSpatialSpace::GetCellsArray()
{
    return this->m_cellsArray;
}

//------------------------------------------------------------------------------
/**
    returns the type of cell
*/
inline
ncSpatialSpace::nSpatialSpaceType 
ncSpatialSpace::GetType() const
{
    return this->m_spaceType;
}

//------------------------------------------------------------------------------
/**
    set the transformation matrix
*/
inline
void 
ncSpatialSpace::SetTransformMatrix(const matrix44 &matrix)
{
    this->m_transformationMatrix = matrix;
}

//------------------------------------------------------------------------------
/**
    get the transformation matrix
*/
inline
matrix44&
ncSpatialSpace::GetTransformMatrix()
{
    return this->m_transformationMatrix;
}

//------------------------------------------------------------------------------
/**
    set space's bounding box
*/
inline
void 
ncSpatialSpace::SetBBox(const bbox3 &box)
{
    this->m_bbox = box;
}

//------------------------------------------------------------------------------
/**
    get space's bounding box
*/
inline
bbox3 &
ncSpatialSpace::GetBBox()
{
    return this->m_bbox;
}

//------------------------------------------------------------------------------
/**
    set the bounding box given the center and extents
*/
inline
void
ncSpatialSpace::SetBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
{
    bbox3 bbox(vector3(vcx, vcy, vcz), vector3(vex, vey, vez));
    
    this->m_bbox = bbox;
}

//------------------------------------------------------------------------------
/**
    returns the center and the extents of the bounding box
*/
inline
void
ncSpatialSpace::GetBBox(vector3& v0, vector3& v1)
{
    v0 = this->m_bbox.center();
    v1 = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    set space's original bounding box
*/
inline
void 
ncSpatialSpace::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
}

//------------------------------------------------------------------------------
/**
    get space's original bounding boxx
*/
inline
bbox3 &
ncSpatialSpace::GetOriginalBBox()
{
    return this->m_originalBBox;
}

//------------------------------------------------------------------------------
/**
    get the spatial path
*/
inline
const char *
ncSpatialSpace::GetSpatialPath() const
{
    return this->m_spatialPath;
}

//------------------------------------------------------------------------------
#endif // NC_SPATIALSPACE_H
