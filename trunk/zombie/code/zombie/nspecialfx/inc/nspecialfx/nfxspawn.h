#ifndef N_FXSPAWN_H
#define N_FXSPAWN_H
//------------------------------------------------------------------------------
/**
    @class nFXSpawn
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    (C) 2006 Conjurer Services, S.A.
*/
#include "nspecialfx/nfxobject.h"

//------------------------------------------------------------------------------
class nFXSpawn : public nFXObject
{
public:
    /// constructor
    nFXSpawn();
    /// destructor
    virtual ~nFXSpawn();

    /// load resources
    virtual bool Load();

    /// update the effect for current frame
    virtual void Trigger(nTime curTime);

    /// restart the effect
    virtual void Restart();

    /// stop
    virtual void Stop();

    /// set global
    void SetFXGlobal(bool global);
    /// get global
    bool GetFXGlobal() const;

    /// set parent entity
    void SetParentEntity(nEntityObject* entity);
    /// get parent entity
    nEntityObject* GetParentEntity() const;

private:

    /// create the spawned entity
    nEntityObject* CreateEntity();

    nRef<nEntityClass> refSpawnClass;
    nRef<nEntityObject> refSpawnedEntity;
    nRef<nEntityObject> parentEntity;
    bool global;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXSpawn::SetFXGlobal(bool global)
{
    this->global = global;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nFXSpawn::GetFXGlobal() const
{
    return this->global;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXSpawn::SetParentEntity(nEntityObject* entity)
{
    this->parentEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nFXSpawn::GetParentEntity() const
{
    return this->parentEntity.get();
}

//------------------------------------------------------------------------------
#endif /*N_FXSPAWN_H*/
