#ifndef N_FXDECAL_H
#define N_FXDECAL_H
//------------------------------------------------------------------------------
/**
    @class nFXDecal
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    (C) 2006 Conjurer Services, S.A.
*/
#include "nspecialfx/nfxobject.h"

class nEntityObject;
class nSceneGraph;
//------------------------------------------------------------------------------
class nFXDecal : public nFXObject
{
public:
    /// constructor
    nFXDecal();
    /// destructor
    ~nFXDecal();

    /// update the effect for current frame
    virtual void Trigger(nTime curTime);

    /// restart the effect
    virtual void Restart();

    /// stop the effect
    virtual void Stop();

    /// load the effect
    virtual bool Load();

    /// attach the decal to the current scene
    virtual bool Render(nSceneGraph*);

    /// set target entity
    void SetTargetEntity(nEntityObject*);
    /// get target entity
    nEntityObject* GetTargetEntity();
    /// set random rotation
    void SetRandomRotation(bool);
    /// get random rotation
    quaternion GetRandomRotation();

private:
    nRef<nEntityClass> refEntityClass;
    nRef<nEntityObject> refTargetEntity;
    int decalPassIndex;

    bool hasRandomRotation;
    quaternion randomRotation;
    quaternion initialRotation;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXDecal::SetTargetEntity(nEntityObject* entity)
{
    this->refTargetEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nFXDecal::GetTargetEntity()
{
    return this->refTargetEntity.get_unsafe();
}

#endif /*N_FXOBJECT_H*/
