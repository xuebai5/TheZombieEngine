#ifndef NC_SPATIALQUADTREECELL_H
#define NC_SPATIALQUADTREECELL_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialQuadtreeCell
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial quadtree cell component for outdoor cell entities.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nspatial/ncspatialcell.h"

class ncSpatialQuadtree;

//------------------------------------------------------------------------------
class ncSpatialQuadtreeCell : public ncSpatialCell
{

    NCOMPONENT_DECLARE(ncSpatialQuadtreeCell, ncSpatialCell)

public:

    /// constructor
    ncSpatialQuadtreeCell();
    /// destructor
    virtual ~ncSpatialQuadtreeCell();

    /// flush all the lights in the cell
    virtual void FlushAllLights();

    /// save state of the component
    bool SaveCmds(nPersistServer *);
    /// save the entities contained into this cell
    virtual void Save(WizardEntityArray &wizards);
    /// load the entities contained into this cell
    virtual bool Load(const char *spatialPath);
    /// destroy the cell and all its elements
    virtual void DestroyCell();

    /// set the bounding box of the cell
    void SetBBox(const bbox3 &box);

    /// add a subcell
    bool AddSubCell(ncSpatialQuadtreeCell *subCell);
    /// remove a quadtree cell
    virtual bool RemoveSubcell(ncSpatialQuadtreeCell *cell);
    /// get subcells array (pointer to the first cell in the array)
    ncSpatialQuadtreeCell** GetSubcells();
    /// get the number of subcells
    int GetNumSubcells();
    /// set the parent cell of this one
    void SetParentCell(ncSpatialQuadtreeCell *parentCell);
    /// get the parent cell of this one
    ncSpatialQuadtreeCell *GetParentCell();
    /// search a cell with the given identifier into its subtree
    ncSpatialCell* SearchCellById(int cellId);
    /// says if this cell is a leaf or not
    bool IsLeaf() const;
    /// set tree's depth
    void SetDepth(int depth);
    /// get tree's depth
    int GetDepth() const;
    /// set bx
    void SetBX(int x);
    /// get bx
    int GetBX() const;
    /// set bz
    void SetBZ(int z);
    /// get bz
    int GetBZ() const;
    /// get the bounding sphere
    const sphere &GetBoundingSphere() const;
    /// get the number of entities in the space
    int GetTotalNumEntities();

    /// set the active flag for this quadtree cell
    void SetActive(bool flag);
    /// get the active flag for this quadtree cell
    bool IsActive() const;

    /// set this cell as visible from the given camera id in this frames
    void SetVisibleSubtreeBy(nEntityObjectId cameraId);
    /// set the visible terrain cells array index
    void SetVisibleTerrainIndex(int index);
    /// get the visible terrain cells array index
    int GetVisibleTerrainIndex() const;

    /// set the terrain of this cell as visible from the given camera id in the current frame
    void SetTerrainVisibleBy(nEntityObjectId cameraId);
    /// says if the terrain fo this cell was visible for the current camera in this frame
    bool IsTerrainVisibleBy(nEntityObjectId currentCam) const;
    /// get the id of the camera the terrain of the cell was visible for the last time
    nEntityObjectId GetLastTerrainVisibleCam() const;
    /// set the id of the camera the terrain of the cell was visible for the last time
    void SetLastTerrainVisibleCam(nEntityObjectId cameraId);
    /// set terrain visibility frame id
    void SetTerrainVisibleFrameId(int id);
    /// get terrain visibility frame id
    int GetTerrainVisibleFrameId() const;

    /// set the terrain of this cell as linked to the given light in this frame
    void SetLinkedTo(nEntityObjectId currentLight);
    /// says if the terrain of this cell was linked to the given light in this frame
    bool IsLinkedTo(nEntityObjectId currentLight);

    /// clear all the scene links of this entity
    void ClearLinks();

    /// adjust cells' bounding box with the given offset and subcells' ones
    void AdjustBBox(float offset);

    /// catch the information from a TiXmlElement
    virtual bool ReadStaticInfo(const TiXmlElement *cellElem);

    /// get the static information of this cell
    virtual bool WriteStaticInfo(TiXmlElement *cellElem);

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);

private:

    friend class nVisibleFrustumVisitor;

    /// cell's depth in the quadtree
    int m_depth;

    /// block x index
    int bx;
    /// block z index
    int bz;

    /// bounding sphere
    sphere m_sph;

    /// subcells
    ncSpatialQuadtreeCell* m_cellsArray[4];

    /// number of subcells
    int m_numSubcells;

    /// parent cell
    ncSpatialQuadtreeCell *m_parentCell;

    /// says if the cell is active or not
    bool m_active;

    /// index in the visible terrain cells array of the visibility visitor
    int m_visTerrainIndex;

    /// id of the frame that the current light was linked to this terrain cell
    int m_linkLightFrameId;
    /// id of the last light that was linked to this terrain cell
    nEntityObjectId m_lastLinkLight;

    /// id of the camera the terrain of this cell was visible for the last time
    nEntityObjectId lastTerrainVisibleCam;
    /// id of the frame the terrain of this cell was visible for the last time
    int visibleTerrainFrameId;

};

//------------------------------------------------------------------------------
/**
    set tree's depth
*/
inline
void 
ncSpatialQuadtreeCell::SetDepth(int depth)
{
    this->m_depth = depth;
}

//------------------------------------------------------------------------------
/**
    get tree's depth
*/
inline
int 
ncSpatialQuadtreeCell::GetDepth() const
{
    return this->m_depth;
}

//------------------------------------------------------------------------------
/**
    set bx
*/
inline
void 
ncSpatialQuadtreeCell::SetBX(int x)
{
    n_assert( (0 <= x) && (x < (1 << this->GetDepth())) );
    this->bx = x;
}

//------------------------------------------------------------------------------
/**
    get bx
*/
inline
int 
ncSpatialQuadtreeCell::GetBX() const
{
    return this->bx;
}

//------------------------------------------------------------------------------
/**
    set bz
*/
inline
void 
ncSpatialQuadtreeCell::SetBZ(int z)
{
    n_assert( (0 <= z) && (z < (1 << this->GetDepth())) );
    this->bz = z;
}

//------------------------------------------------------------------------------
/**
    get bz
*/
inline
int 
ncSpatialQuadtreeCell::GetBZ() const
{
    return this->bz;
}

//------------------------------------------------------------------------------
/**
    get the bounding sphere
*/
inline
const sphere &
ncSpatialQuadtreeCell::GetBoundingSphere() const
{
    return this->m_sph;
}

//------------------------------------------------------------------------------
/**
    get subcells array (pointer to the first cell in the array)
*/
inline
ncSpatialQuadtreeCell**
ncSpatialQuadtreeCell::GetSubcells()
{
    return this->m_cellsArray;
}

//------------------------------------------------------------------------------
/**
    get the number of subcells
*/
inline
int 
ncSpatialQuadtreeCell::GetNumSubcells()
{
    return this->m_numSubcells;
}

//------------------------------------------------------------------------------
/**
    set the parent cell of this one
*/
inline
void 
ncSpatialQuadtreeCell::SetParentCell(ncSpatialQuadtreeCell *parentCell)
{
    this->m_parentCell = parentCell;
}

//------------------------------------------------------------------------------
/**
    get the parent cell of this one
*/
inline
ncSpatialQuadtreeCell *
ncSpatialQuadtreeCell::GetParentCell()
{
    return this->m_parentCell;
}

//------------------------------------------------------------------------------
/**
    returns true if this cell is a leaf
*/
inline
bool 
ncSpatialQuadtreeCell::IsLeaf() const
{
    return (this->m_numSubcells == 0);
}

//------------------------------------------------------------------------------
/**
    set the active flag for this quadtree cell
*/
inline
void 
ncSpatialQuadtreeCell::SetActive(bool flag)
{
    this->m_active = flag;
}

//------------------------------------------------------------------------------
/**
    get the active flag for this quadtree cell
*/
inline
bool 
ncSpatialQuadtreeCell::IsActive() const
{
    return this->m_active;
}

//------------------------------------------------------------------------------
/**
    set the visible terrain cells array index
*/
inline
void 
ncSpatialQuadtreeCell::SetVisibleTerrainIndex(int index)
{
    this->m_visTerrainIndex = index;
}

//------------------------------------------------------------------------------
/**
    get the visible terrain cells array index
*/
inline
int 
ncSpatialQuadtreeCell::GetVisibleTerrainIndex() const
{
    return this->m_visTerrainIndex;
}

//------------------------------------------------------------------------------
/**
    set the id of the camera the terrain of the cell was visible for the last time
*/
inline
void
ncSpatialQuadtreeCell::SetLastTerrainVisibleCam(nEntityObjectId cameraId)
{
    this->lastTerrainVisibleCam = cameraId;
}

//------------------------------------------------------------------------------
/**
    get the id of the camera the terrain of the cell was visible for the last time
*/
inline
nEntityObjectId 
ncSpatialQuadtreeCell::GetLastTerrainVisibleCam() const
{
    return this->lastTerrainVisibleCam;
}

//------------------------------------------------------------------------------
/**
    set the terrain of this cell as visible from the given camera id in the 
    current frame
*/
inline
void 
ncSpatialQuadtreeCell::SetTerrainVisibleBy(nEntityObjectId cameraId)
{
    if ( !this->IsTerrainVisibleBy(cameraId) )
    {
        this->SetLastTerrainVisibleCam(cameraId);
        this->SetTerrainVisibleFrameId(nSpatialServer::Instance()->GetFrameId());
    }
}

//------------------------------------------------------------------------------
/**
    says if the terrain of the cell was visible for the current camera in this frame
*/
inline
bool 
ncSpatialQuadtreeCell::IsTerrainVisibleBy(nEntityObjectId currentCam) const
{
    return (this->GetTerrainVisibleFrameId()  == nSpatialServer::Instance()->GetFrameId() &&
            this->GetLastTerrainVisibleCam() == currentCam);
}

//------------------------------------------------------------------------------
/**
    set terrain visibility frame id
*/
inline
void 
ncSpatialQuadtreeCell::SetTerrainVisibleFrameId(int id)
{
    this->visibleTerrainFrameId = id;
}

//------------------------------------------------------------------------------
/**
    get terrain visibility frame id
*/
inline
int 
ncSpatialQuadtreeCell::GetTerrainVisibleFrameId() const
{
    return this->visibleTerrainFrameId;
}

#endif // NC_SPATIALQUADTREECELL_H

