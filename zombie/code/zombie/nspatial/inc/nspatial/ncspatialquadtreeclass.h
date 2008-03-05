#ifndef NC_SPATIALQUADTREECLASS_H
#define NC_SPATIALQUADTREECLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialQuadtreeClass
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial quadtree component class for entities.

    (C) 2006 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
class ncSpatialQuadtreeClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSpatialQuadtreeClass, nComponentClass);

public:
    
    /// constructor
    ncSpatialQuadtreeClass();
    /// destructor
    ~ncSpatialQuadtreeClass();

    /// set the space's original bbox, center and the extents
    void SetOriginalBBox(float, float, float, float, float, float);
    /// get original bbox, center and the extents
    void GetOriginalBBox(vector3&, vector3&);

    /// component class persistence
    bool SaveCmds(nPersistServer* ps);
    
    /// set original bbox
    void SetOriginalBBox(const bbox3 &box);
    /// get original bbox
    bbox3 &GetOriginalBBox();

private:

    /// original bounding box
    bbox3 m_originalBBox;
};

//------------------------------------------------------------------------------
/**
    constructor
*/
inline
ncSpatialQuadtreeClass::ncSpatialQuadtreeClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
inline
ncSpatialQuadtreeClass::~ncSpatialQuadtreeClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    set cell's original bbox, center and the extents
*/
inline
void 
ncSpatialQuadtreeClass::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
}

//------------------------------------------------------------------------------
/**
    set original bbox, center and the extents
*/
inline
void 
ncSpatialQuadtreeClass::SetOriginalBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
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
ncSpatialQuadtreeClass::GetOriginalBBox(vector3& v0, vector3& v1)
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
ncSpatialQuadtreeClass::GetOriginalBBox()
{
    return this->m_originalBBox;
}

#endif // NC_SPATIALQUADTREECLASS_H

