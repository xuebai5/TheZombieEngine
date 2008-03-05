#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxserver_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxspawn.h"
#include "nspecialfx/nfxdecal.h"
#include "kernel/ntimeserver.h"
#include "entity/nentity.h"

#include "nspecialfx/nfxmaterial.h"
#include "kernel/nfileserver2.h"
#include "entity/nentityclassserver.h"

nFXServer* nFXServer::Singleton = 0;

nNebulaScriptClass(nFXServer, "nroot");

namespace
{
    const char* FxMaterialsPath("/usr/fx/");
    const char* FileFxMaterialsPath("wc:libs/materials/fx");
}

//------------------------------------------------------------------------------
static const char * specialfxLogNames[] = {
    "Render",
    "Creation",
    "Launch",
    0
};
// level 0..3 Error , level 4..7 warnings  , level 8 ..11 info
NCREATELOGLEVELGROUP ( specialfx , "Special FX System" , false , 12 ,  specialfxLogNames , NLOG_GROUP_MASK)

//------------------------------------------------------------------------------
/**
*/
nFXServer::nFXServer() :
    effectsPool(16, 16),
    entityEffects(16, 16),
    fxMaterialsPool(0,1)
    #if __NEBULA_STATS__
   ,statsTotalNumEffects(0),
    profTrigger("profSpecialFxTrigger", true),
    watchNumEffects("sfxNumEffects", nArg::Int),
    watchNumActiveEffects("sfxNumActiveEffects", nArg::Int)
    #endif
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nFXServer::~nFXServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::Clear()
{
    // remove all active effects
    while (this->effectsPool.Size() > 0)
    {
        this->RemoveEffect(this->effectsPool.GetKeyAt(0));
    }
    // destroy all registered fx classes
    nStrNode *strHead = this->fxClassList.GetHead();
    while (strHead)
    {
        nStrNode *next = strHead->GetSucc();
        strHead->Remove();
        n_delete(strHead);
        strHead = next;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFXServer::FXClassPool*
nFXServer::NewEffectClass(const char* className)
{
    //nClass* fxClass = kernelServer->FindClass(className);
    
    //N_LOG("no effect class found: '%s'", className);
    if (nEntityClassServer::Instance()->ExistEntityClass(className) )
    {
        //get a list for all objects of the same class, or create it
        FXClassPool* classPool = (FXClassPool*) this->fxClassList.Find(className);
        if (!classPool)
        {
            classPool = n_new(FXClassPool);
            classPool->SetName(className);
            this->fxClassList.AddHead(classPool);
        }
        return classPool;
    }
    else
    {
        NLOG( specialfx , ( 1 | nFXServer::CREATION , "The fx class not found: '%s'", className) );
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nFXSpawn*
nFXServer::NewSpawnEffect(const char* className)
{
    //this kind of effect spawns an actual entity of the referenced class
    //this class must be a specialfx class, in order to know things of it.
    //a pool of these can be used, and unique fx are always reused.
    //used for explosions, fires, tsunamis, etc.
    //also usable for screen space effect, like hit impacts
    FXClassPool* classPool = this->NewEffectClass(className);
    if (classPool)
    {
        nFXSpawn* spawnFX = static_cast<nFXSpawn*>(kernelServer->New("nfxspawn"));
        classPool->fxList.AddHead(&spawnFX->fxNode);//add to the list of nodes
        this->effectsPool.Add(spawnFX->GetKey(), spawnFX);
        spawnFX->SetFXClass(className); // needs a ncspecialfx component?

        // load class resources, and internal entity (reuse if unique)
        // also, it retrieves default values from the class
        if (spawnFX->Load())
        {
            #if __NEBULA_STATS__
            ++this->statsTotalNumEffects;
            #endif
            return spawnFX;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nFXDecal*
nFXServer::NewDecalEffect(const char* className)
{
    // this kind of effect doesn't create any entity, but is rendered as
    // many times as needed using the corresponding effect resource.
    FXClassPool* classPool = this->NewEffectClass(className);
    if (classPool)
    {
        nFXDecal* decalFX = static_cast<nFXDecal*>(kernelServer->New("nfxdecal"));
        classPool->fxList.AddHead(&decalFX->fxNode);//add to the list of nodes
        this->effectsPool.Add(decalFX->GetKey(), decalFX);
        decalFX->SetFXClass(className); // needs a ncspecialfx component?

        // load class resources and retrieve default values from the class
        if (decalFX->Load())
        {
            #if __NEBULA_STATS__
            ++this->statsTotalNumEffects;
            #endif
            return decalFX;
        }
    }

    return 0;
}

/*
nFXTracer*
nFXServer::NewTracerEffect(const char* className)
{
    //every instance of this effect is an entity that follows another, and
    //it never is destroyed, but a pool of these is used instead
}
*/

//------------------------------------------------------------------------------
/**
*/
nFXObject*
nFXServer::PullEffect(int key)
{
    if (this->effectsPool.HasKey(key))
    {
        //check the fxObject is alive or in the activeEffects list
        //(just in case someone added it this same frame!)
        nFXObject* fxObject = this->effectsPool.GetElement(key);
        if (fxObject->IsAlive() || this->activeEffects.Find(fxObject))
        {
            if (fxObject->IsUnique())
            {
                fxObject->alive = false;// force restart at Trigger();
                return fxObject;
            }
            
            //find first idle fx object of the same class
            nFXObject* newFX = fxObject->GetNext();
            for (; newFX && newFX->IsAlive(); newFX = newFX->GetNext());
            
            if (!newFX)
            {
                //clone the existing object and add it to the pool
                newFX = static_cast<nFXObject*>(fxObject->Clone());//with everything but the entity
                newFX->InsertNodeAfter(fxObject);
                this->effectsPool.Add(newFX->GetKey(), newFX);
                if (!newFX->Load())//initialize the entity, etc.
                {
                    newFX->Release();
                    return 0;
                }
                #if __NEBULA_STATS__
                ++this->statsTotalNumEffects;
                #endif
                fxObject = newFX;
            }
            else
            {
                fxObject = newFX;
                fxObject->alive = false;//force restart at Trigger();
            }
        }
        //start/reset effect
        this->activeEffects.Append(fxObject);
        return fxObject;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::KillEffect(int key)
{
    if (this->effectsPool.HasKey(key))
    {
        nFXObject* effect = this->effectsPool.GetElement(key);
        nArray<nFXObject*>::iterator fxIter = this->activeEffects.Find(effect);
        if (fxIter)
        {
            this->activeEffects.EraseQuick(fxIter);
            effect->alive = false;
            effect->Stop();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::RemoveEffect(int key)
{
    if (this->effectsPool.HasKey(key))
    {
        // if this effect is alive, kill it first
        nFXObject* effect = this->effectsPool.GetElement(key);
        this->KillEffect(key);
        this->effectsPool.Rem(key);
        effect->fxNode.Remove();
        effect->Release();
        #if __NEBULA_STATS__
        --this->statsTotalNumEffects;
        #endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::Trigger()
{
    // traverse the array and Trigger() all active effects-
    if (!this->activeEffects.Empty())
    {
        #if __NEBULA_STATS__
        this->profTrigger.Start();
        #endif

        nTime curTime = nTimeServer::Instance()->GetFrameTime();
        nArray<nFXObject*>::iterator fxIter = this->activeEffects.Begin();
        while (fxIter != this->activeEffects.End())
        {
            nFXObject* fxObject = *fxIter;
            fxObject->Trigger(curTime);
            if (!fxObject->IsAlive())
            {
                // if an effect is dead after trigger, put it back to the pool
                fxIter = this->activeEffects.EraseQuick(fxIter);
                fxObject->Stop();
            }
            else
            {
                ++fxIter;
            }
        }

        #if __NEBULA_STATS__
        this->profTrigger.Stop();
        this->watchNumEffects->SetI(this->statsTotalNumEffects);
        this->watchNumActiveEffects->SetI(this->activeEffects.Size());
        #endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::StickEffect(nEntityObject* entityObject, nFXObject* fxObject)
{
    if (!this->entityEffects.HasKey(entityObject->GetId()))
    {
        this->entityEffects.Add(entityObject->GetId(), nArray<nFXObject*>());
    }
    nArray<nFXObject*>& fxArray = this->entityEffects.GetElement(entityObject->GetId());
    fxArray.Append(fxObject);
}

//------------------------------------------------------------------------------
/**
    remove the effect from an entity-
    if no effects are left, remove the array to optimize further searches.
*/
void
nFXServer::UnstickEffect(nEntityObject* entityObject, nFXObject* fxObject)
{
    if (this->entityEffects.HasKey(entityObject->GetId()))
    {
        nArray<nFXObject*>& fxArray = this->entityEffects.GetElement(entityObject->GetId());
        nArray<nFXObject*>::iterator fxIter = fxArray.Find(fxObject);
        if (fxIter)
        {
            fxArray.EraseQuick(fxIter);
            if (fxArray.Empty())
            {
                this->entityEffects.Rem(entityObject->GetId());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::RenderEffects(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (this->entityEffects.HasKey(entityObject->GetId()))
    {
        nArray<nFXObject*>& fxArray = this->entityEffects.GetElement(entityObject->GetId());
        nArray<nFXObject*>::iterator fxIter;
        for (fxIter = fxArray.Begin(); fxIter != fxArray.End(); ++fxIter)
        {
            (*fxIter)->Render(sceneGraph);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nFxMaterial* 
nFXServer::GetGenericFxMaterial()
{
    return this->fxGenericMaterial.get_unsafe();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
*/
void
nFXServer::AddFxMaterial( nString matName)
{
    nString NOHPathName( FxMaterialsPath );

    NOHPathName.Append(matName);

    nFxMaterial* fxMaterial = static_cast<nFxMaterial*>( nKernelServer::Instance()->New( "nfxmaterial", NOHPathName.Get() ));

    this->fxMaterialsPool.Append(fxMaterial);
}

//-----------------------------------------------------------------------------
/**
*/
void
nFXServer::RemoveFxMaterial( nString matName )
{
    for( int i=0; i< this->fxMaterialsPool.Size(); i++)
    {
        if( strcmp(this->fxMaterialsPool[i]->GetName(), matName.Get()) == 0)
        {
            this->fxMaterialsPool[i]->SetToBeRemoved(true);
        }
    }
}
#endif

//-----------------------------------------------------------------------------
/**
*/
bool
nFXServer::LoadFxMaterials()
{
    nFileServer2* fserver(nKernelServer::Instance()->GetFileServer());

    n_assert2( fserver, "File server required." );

    nArray<nString> files = fserver->ListFiles( FileFxMaterialsPath );

    for( int index(0); index < files.Size(); ++index )
    {
        nString NOHPathName( FxMaterialsPath );
        nString name( files[index].ExtractFileName() );
        name.StripExtension();
        NOHPathName.Append( name);

        
        
        // create an object first
        nFxMaterial* fxMaterial( static_cast<nFxMaterial*>(
        nKernelServer::Instance()->New( "nfxmaterial", NOHPathName.Get() )));

        if( !fxMaterial )
        {
            return false;
        }

        if ( name == "undefined_material" )
        {
            this->fxGenericMaterial = fxMaterial;
        }

        // load scripting info
        nKernelServer::Instance()->PushCwd( fxMaterial );
        nKernelServer::Instance()->Load( files[ index ].Get(), false );
        nKernelServer::Instance()->PopCwd();

        this->fxMaterialsPool.Append(fxMaterial);
    }

    return true;
}
