#ifndef N_SPATIALINDOORCELL_H
#define N_SPATIALINDOORCELL_H
//------------------------------------------------------------------------------
/**
    @class nSpatialIndoorCell
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialIndoorCell represents a division of an indoor space. 
    It contains entities with spatial component.
    
    (C) 2004  Conjurer Services, S.A.
*/

#include "nspatial/ncspatialcell.h"
#include "nspatial/nwrappermesh.h"
#include "tools/nmeshbuilder.h"
#include "mathlib/vector.h"
#include "mathlib/plane.h"
#include "mathlib/sphere.h"

//------------------------------------------------------------------------------
class nSpatialIndoorCell : public ncSpatialCell
{

    NCOMPONENT_DECLARE(nSpatialIndoorCell, ncSpatialCell);

public:

    /// constructor
    nSpatialIndoorCell();
    /// destructor
    virtual ~nSpatialIndoorCell();

    /// set original bbox
    void SetOriginalBBox(float, float, float, float, float, float);
    /// get original bbox
    void GetOriginalBBox(vector3&, vector3&);

    #ifndef NGAME
    /// remove temporary all the cell
    void RemoveTemporary();
    /// restore this cell's objects from the temporary array
    void RestoreTempArray();
    #endif // !NGAME

    /// set instance's original bbox
    void SetOriginalBBox(const bbox3 &box);
    /// get instance's original bbox
    bbox3 &GetOriginalBBox();
    /// removes an entity from the cell without destroying it unless it's an indoor brush or a portal
    virtual bool RemoveEntity(nEntityObject *entity);

    /// get the wrapper mesh
    nWrapperMesh& GetWrapperMesh();
    /// determine if a given point belongs to this cell using the wrapper mesh
    bool Contains(const vector3& point) const;
    /// determine if a given bbox belongs to this cell using the wrapper mesh
    bool Contains(const bbox3 &box) const;
    /// determine if a given sphere belongs to this cell using the wrapper mesh
    bool Contains(const sphere &sph) const;

#ifndef NGAME
    /// set the layer id to all the entities into this cell
    virtual void SetLayerId(int layerId);
#endif
    
    /// get the all the indoor brushes into this indoor cell
    void GetIndoorBrushes(nArray<nEntityObject*> &indoorBrushes);
    /// get the all the portals into this indoor cell
    void GetPortals(nArray<ncSpatialPortal*> &portals);
    /// get the entities that are not indoor brushes nor portals
    void GetDynamicEntities(nArray<nEntityObject*> &entitiesArray);
    /// update the transformation of all the entities into the cell
    virtual void Update(vector3 &incPos, quaternion &incQuat, const matrix44 &matrix);
    /// accept a frustum visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);

    /// catch the information from a TiXmlElement
    virtual bool ReadStaticInfo(const TiXmlElement *cellElem);
    /// fill a TiXmlElement with the cell's information
    virtual bool WriteStaticInfo(TiXmlElement *cellElem);
    
private:
    friend class nVisibleFrustumVisitor;

    /// cell's wrapper mesh
    nWrapperMesh m_wrapperMesh;

};

//------------------------------------------------------------------------------
/**
    set cell's original bbox
*/
inline
void 
nSpatialIndoorCell::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
}

//------------------------------------------------------------------------------
/**
    set cell's original bbox
*/
inline
void 
nSpatialIndoorCell::SetOriginalBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
{
    bbox3 bbox(vector3(vcx, vcy, vcz), vector3(vex, vey, vez));
    
    this->m_originalBBox = bbox;
}

//------------------------------------------------------------------------------
/**
    returns the center and the extents of the original bounding box
*/
inline
void
nSpatialIndoorCell::GetOriginalBBox(vector3& v0, vector3& v1)
{
    v0 = this->m_originalBBox.center();
    v1 = this->m_originalBBox.extents();
}

//------------------------------------------------------------------------------
/**
    get cell's original bbox
*/
inline
bbox3 &
nSpatialIndoorCell::GetOriginalBBox()
{
    return this->m_originalBBox;
}

//------------------------------------------------------------------------------
/**
    get the wrapper mesh
*/
inline
nWrapperMesh&
nSpatialIndoorCell::GetWrapperMesh()
{
    return this->m_wrapperMesh;
}

//------------------------------------------------------------------------------
/**
    determine if a given point belongs to this cell using the wrapper mesh
*/
inline
bool 
nSpatialIndoorCell::Contains(const vector3& point) const
{
    return this->m_wrapperMesh.Contains(point);
}

//------------------------------------------------------------------------------
/**
    Determine if a given sphere belongs to this cell using the wrapper mesh.

    NOTE: This method uses a conservative approximation. It uses the bounding 
    box of the sphere and determines if this one is completely inside the cell.
*/
inline
bool 
nSpatialIndoorCell::Contains(const sphere &sph) const
{
    bbox3 box(sph.p, vector3(sph.r, sph.r, sph.r));
    return this->Contains(box);
}

//------------------------------------------------------------------------------
#endif
