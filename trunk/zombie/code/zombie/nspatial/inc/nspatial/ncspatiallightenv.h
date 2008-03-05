#ifndef NC_SPATIALLIGHTENV_H
#define NC_SPATIALLIGHTENV_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialLightEnv
    @ingroup NebulaSpatialSystem
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Spatial component for a space-linked light environment.

    A spatial light environment links a light source and space instances
    to index a subset of a light scene resource when determining that
    some entity should be affected by a precomputed light environment.
    Specifically, it is intended to hold the set of lightmaps for an
    outdoor space.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatiallight.h"

//------------------------------------------------------------------------------
class ncSpatialLightEnv : public ncSpatialLight 
{

    NCOMPONENT_DECLARE(ncSpatialLightEnv, ncSpatialLight);

public:
    /// constructor
    ncSpatialLightEnv();
    /// destructor
    virtual ~ncSpatialLightEnv();
    /// Initialize the component
    virtual void InitInstance(nObject::InitInstanceMsg initType);
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// load the component
    virtual bool Load();

    /// load the component
    virtual void Unload();

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);

    /// set space entity id (for persistence)
    void SetSpaceEntityId(nEntityObjectId);
    /// set space entity
    void SetSpaceEntity(nEntityObject *);
    /// get space entity
    nEntityObject * GetSpaceEntity() const;

    /// set light entity id (for persistence)
    void SetLightEntityId(nEntityObjectId);
    /// set light entity
    void SetLightEntity(nEntityObject *);
    /// get light entity
    nEntityObject * GetLightEntity() const;

private:
    nEntityObjectId spaceEntityId;
    nEntityObjectId lightEntityId;
    nRef<nEntityObject> refSpaceEntity;
    nRef<nEntityObject> refLightEntity;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialLightEnv::SetSpaceEntityId(nEntityObjectId eoid)
{
    this->spaceEntityId = eoid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialLightEnv::SetSpaceEntity(nEntityObject *entity)
{
    this->refSpaceEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject *
ncSpatialLightEnv::GetSpaceEntity() const
{
    return this->refSpaceEntity.isvalid() ? this->refSpaceEntity.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialLightEnv::SetLightEntityId(nEntityObjectId eoid)
{
    this->lightEntityId = eoid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialLightEnv::SetLightEntity(nEntityObject *entity)
{
    this->refLightEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject *
ncSpatialLightEnv::GetLightEntity() const
{
    return this->refLightEntity.isvalid() ? this->refLightEntity.get() : 0;
}

//------------------------------------------------------------------------------
#endif  // NC_SPATIALLIGHTENV_H
