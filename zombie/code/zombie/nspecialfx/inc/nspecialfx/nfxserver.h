#ifndef N_FXSERVER_H
#define N_FXSERVER_H
//------------------------------------------------------------------------------
/**
    @class nFXServer
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nsafekeyarray.h"
#include "kernel/nlogclass.h"

#if __NEBULA_STATS__
#include "misc/nwatched.h"
#include "kernel/nprofiler.h"
#endif

class nFXObject;
class nFXSpawn;
class nFXDecal;
class nFXTracer;
class nSceneGraph;
class nEntityObject;
class nFxMaterial;
//------------------------------------------------------------------------------
class nFXServer : public nRoot
{
public:
    enum specialFxLogGroup
    {
        RENDER = NLOG1,
        CREATION = NLOG2,
        LAUNCH = NLOG3,
    };
    /// constructor
    nFXServer();
    /// destructor
    ~nFXServer();
    /// get instance pointer
    static nFXServer *Instance();

    // TODO- register available effect classes (to preload resources)
    /// add a spawn effect to the pool, return pointer to configure
    nFXSpawn* NewSpawnEffect(const char* className);
    /// add a decal effect to the pool, return pointer to configure
    nFXDecal* NewDecalEffect(const char* className);
    /// add a tracer effect to the pool, return pointer to configure
    //nFXTracer* NewTracerEffect(const char* className);

    /// clear pool of available effects, destroy all active ones
    void Clear();

    /// pull an effect (spawn it or retrieve from a pool)
    nFXObject* PullEffect(int key);

    /// kill an effect (set as dead and return it to the pool)
    void KillEffect(int key);

    /// remove an effect from the pool, destroy the object
    void RemoveEffect(int key);

    /// trigger all active effects
    void Trigger();

    /// add an effect linked to an entity
    void StickEffect(nEntityObject*, nFXObject*);

    /// remove an effect from an entity
    void UnstickEffect(nEntityObject*, nFXObject*);

    /// render effects attached to an entity
    void RenderEffects(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// load materials
    bool LoadFxMaterials();

    nFxMaterial* GetGenericFxMaterial();
#ifndef NGAME
    //save fx materials
    bool SaveFxMaterials( nPersistServer* ps);
#endif

    /// add a spawn effect to the pool by class
    nObject* NewSpawnEffectObject(const char *);
    /// add a decal effect to the pool by class
    nObject* NewDecalEffectObject(const char *);
    /// add a tracer effect to the pool by class
    // nObject* NewTracerEffectObject(const char *);
    /// pull an effect into the active array
    nObject* PullEffectObject(nObject*);
    /// kill an effect back into the pool
    void KillEffectObject(nObject*);
    /// remove an effect from the pool
    void RemoveEffectObject(nObject*);

#ifndef NGAME
    /// add fx material
    void AddFxMaterial( nString);
    /// remove fx material
    void RemoveFxMaterial( nString);
#endif

private:
    static nFXServer* Singleton;

protected:
    nStrList fxClassList;
    class FXClassPool : public nStrNode
    {
    public:
        nFXObject* GetHead();
        nFXObject* GetTail();
        nList fxList;
    };

    /// create a new pool of effects of a class
    FXClassPool* NewEffectClass(const char* className);

    nArray<nFXObject*> activeEffects;   /// current active effects
    nKeyArray<nFXObject*> effectsPool;  /// pool of available effects
                                        /// they can be retrieved by key to optimize access

    /// find all effects for an entity
    nArray<nFXObject*>& FindEntityFX(nEntityObject* entity);

    /// indexed list of effects for an entity
    nSafeKeyArray<nArray<nFXObject*> > entityEffects;

    nArray<nFxMaterial*> fxMaterialsPool;

    //The generic material
    nRef<nFxMaterial> fxGenericMaterial;

    #if __NEBULA_STATS__
    int statsTotalNumEffects;
    nProfiler profTrigger;
    nWatched watchNumEffects;
    nWatched watchNumActiveEffects;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nFXServer*
nFXServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif /*N_FXSERVER_H*/
