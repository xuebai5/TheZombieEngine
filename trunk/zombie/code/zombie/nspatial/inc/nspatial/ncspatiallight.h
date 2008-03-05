#ifndef NC_SPATIALLIGHT_H
#define NC_SPATIALLIGHT_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialLight
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial light component for light entities.

    A spatial light has a Test Model, that represents its 
    ROI (Region Of Influence). The objects affected by the light will 
    be those that are partially or totally inside this ROI.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialcell.h"
#include "nscene/ncscene.h"

class nVisibleFrustumVisitor;
class nLightRegion;

//------------------------------------------------------------------------------
class ncSpatialLight : public ncSpatial
{
    
    NCOMPONENT_DECLARE(ncSpatialLight, ncSpatial);

public:
    /// constructor
    ncSpatialLight();
    /// destructor
    virtual ~ncSpatialLight();

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// destroy the spatial comp
    virtual void Destroy();
    /// set if the light affects only one cell
    void SetAffectsOnlyACell(bool flag);
    /// get if the light affects only one cell
    bool AffectsOnlyACell() const;
    /// set if the light affects other space types than the current one where is in
    void SetAffectsOtherSpaceTypes(bool flag);
    /// get if the light affects other space types than the current one where is in
    bool AffectsOtherSpaceTypes() const;
    /// set if the light cast shadows
    void SetCastShadows(bool flag);
    /// get if the light cast shadows
    bool GetCastShadows() const;

    /// makes the light to reset
    void Flush();

    /// turn on the light
    void TurnOn();
    /// turn off the light
    void TurnOff();
    /// is the light on?
    bool IsOn() const;

    #ifndef NGAME
    /// draw the light's roi
    void DrawROI( nShader2* shapeShader );

    /// set light on/off
    void SetLightOn(bool);
    bool GetLightOn();
   
    /// enter limbo
    void DoEnterLimbo();
    
    #endif // !NGAME

    /// light range (when the light has a sphere model)
    void SetLightRadius(float);
    float GetLightRadius();
    /// light frustum (when the light has a frustum model)
    void SetLightFrustum(float, float, float, float);
    void GetLightFrustum(float&, float&, float&, float&);

    /// adds a light region to this light
    void AddLightRegion(nLightRegion *lightRegion);
    /// remove a light region from this light
    void RemoveLightRegion(nLightRegion *lightRegion);
    /// get the light regions array
    nArray<nLightRegion*> &GetLightRegions();
    /// destroy all the regions of this light
    void DestroyLightRegions();

    /// adds a linked entity
    void AddLinkedEntity(nEntityObject *entity);
    /// removes a linked entity
    void RemoveLinkedEntity(nEntityObject *entity);
    /// unlink from the entities
    void UnlinkFromEntities();
    /// get the array of linked entities to this light
    const nArray<nEntityObject*> &GetLinkedEntities() const;

    /// save state of the component
    bool SaveCmds(nPersistServer *ps);
    /// updates element transform
    virtual void Update(const matrix44 &matrix);

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);
    /// accept a spatial visitor
    virtual void Accept(nSpatialVisitor &visitor);
    /// set the z offset
    void SetZOffset(float offset);
    /// get the z offset.
    float GetZOffset() const;

protected:

    /// true if the light affects only a cell, false if affects all the space
    bool affectsOnlyACell;
    
    /// true if the light affects only the space where is in and others of the same type
    bool affectsOtherSpaceTypes;

    /// true if the light cast shadows
    bool castShadows;

    /// flag to know if the light is on or not
    bool isOn;

    /// displace the frustrum in z direction.
    float zOffset;

    /// light regions
    nArray<nLightRegion*> lightRegions;

    /// linked entities to this light
    nArray<nEntityObject*> linkedEntities;
};

//------------------------------------------------------------------------------
/**
    get the range of the light: it only affects a cell?
*/
inline
bool
ncSpatialLight::AffectsOnlyACell() const
{
    return this->affectsOnlyACell;
}

//------------------------------------------------------------------------------
/**
    set if the light affects other space types than the current one where is in
*/
inline
void 
ncSpatialLight::SetAffectsOtherSpaceTypes(bool flag)
{
    this->affectsOtherSpaceTypes = flag;
    this->Flush();
}

//------------------------------------------------------------------------------
/**
    get if the light affects other space types than the current one where is in
*/
inline
bool 
ncSpatialLight::AffectsOtherSpaceTypes() const
{
    return this->affectsOtherSpaceTypes;
}

//------------------------------------------------------------------------------
/**
    set if the light cast shadows
*/
inline
void 
ncSpatialLight::SetCastShadows(bool flag)
{
    this->castShadows = flag;
    ncScene *sceneComp = this->GetComponentSafe<ncScene>();
    sceneComp->SetFlag(ncScene::CastShadows, this->castShadows);
    this->Flush();
}

//------------------------------------------------------------------------------
/**
    get if the light cast shadows
*/
inline
bool
ncSpatialLight::GetCastShadows() const
{
    return this->castShadows;
}

//------------------------------------------------------------------------------
/**
    turn off the light
*/
inline
void 
ncSpatialLight::TurnOff()
{
    this->isOn = false;
    this->RemoveFromSpaces();
}

//------------------------------------------------------------------------------
/**
    is the light on?
*/
inline
bool 
ncSpatialLight::IsOn() const
{
    return this->isOn;
}

//------------------------------------------------------------------------------
/**
    get the light regions array
*/
inline
nArray<nLightRegion*> &
ncSpatialLight::GetLightRegions()
{
    return this->lightRegions;
}

//------------------------------------------------------------------------------
/**
    adds a linked entity
*/
inline
void 
ncSpatialLight::AddLinkedEntity(nEntityObject *entity)
{
    n_assert(this->linkedEntities.FindIndex(entity) == -1);
    this->linkedEntities.Append(entity);
}

//------------------------------------------------------------------------------
/**
    removes a linked entity
*/
inline
void
ncSpatialLight::RemoveLinkedEntity(nEntityObject *entity)
{
    int index(this->linkedEntities.FindIndex(entity));
    if ( index != -1 )
    {
        this->linkedEntities.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    get the array of linked entities to this light
*/
inline
const nArray<nEntityObject*> &
ncSpatialLight::GetLinkedEntities() const
{
    return this->linkedEntities;
}

//------------------------------------------------------------------------------
#endif  // NC_SPATIALLIGHT_H
