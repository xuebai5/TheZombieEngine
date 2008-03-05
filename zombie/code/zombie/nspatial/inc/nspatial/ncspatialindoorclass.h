#ifndef NC_SPATIALINDOORCLASS_H
#define NC_SPATIALINDOORCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialIndoorClass
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial indoor component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
class ncSpatialIndoorClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSpatialIndoorClass, nComponentClass);

public:
    
    /// constructor
    ncSpatialIndoorClass();
    /// destructor
    ~ncSpatialIndoorClass();

    /// set the space's original bbox
    void SetOriginalBBox(float, float, float, float, float, float);
    /// get original bbox (for an instance)
    void GetOriginalBBox(vector3&, vector3&);
    
    /// set the indoor's shell name
    void SetShellName(const char*);

    /// set the has occluders flag
    void SetHasOccluders(bool);
    /// says if this class has occluders
    bool GetHasOccluders() const;

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// component class persistence
    bool SaveCmds(nPersistServer* ps);
    
    /// set original bbox
    void SetOriginalBBox(const bbox3 &box);
    /// get original bbox
    bbox3 &GetOriginalBBox();

    /// get the shell name
    const nString &GetShellName() const;

private:

    /// original bounding box
    bbox3 m_originalBBox;
    
    /// indoor shell's class name
    nString m_shellName;

    /// true if this class has occluders
    bool m_hasOccluders;

};

//------------------------------------------------------------------------------
/**
    constructor
*/
inline
ncSpatialIndoorClass::ncSpatialIndoorClass():
    m_hasOccluders(false)
{
    this->m_originalBBox.begin_extend();
}

//------------------------------------------------------------------------------
/**
    destructor
*/
inline
ncSpatialIndoorClass::~ncSpatialIndoorClass()
{
}

//------------------------------------------------------------------------------
/**
    set cell's original bbox
*/
inline
void 
ncSpatialIndoorClass::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
}

//------------------------------------------------------------------------------
/**
    set original bbox
*/
inline
void 
ncSpatialIndoorClass::SetOriginalBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
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
ncSpatialIndoorClass::GetOriginalBBox(vector3& v0, vector3& v1)
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
ncSpatialIndoorClass::GetOriginalBBox()
{
    return this->m_originalBBox;
}

//------------------------------------------------------------------------------
/**
    set the indoor's shell name
*/
inline
void 
ncSpatialIndoorClass::SetShellName(const char *name)
{
    this->m_shellName = name;
}

//------------------------------------------------------------------------------
/**
    get the shell name
*/
inline
const nString &
ncSpatialIndoorClass::GetShellName() const
{
    return this->m_shellName;
}

//------------------------------------------------------------------------------
/**
    set the has occluders flag
*/
inline
void 
ncSpatialIndoorClass::SetHasOccluders(bool flag)
{
    this->m_hasOccluders = flag;
}

//------------------------------------------------------------------------------
/**
    says if this class has occluders
*/
inline
bool 
ncSpatialIndoorClass::GetHasOccluders() const
{
    return this->m_hasOccluders;
}

//------------------------------------------------------------------------------
#endif

