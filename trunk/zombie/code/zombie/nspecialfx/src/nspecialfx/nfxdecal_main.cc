#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxdecal_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxdecal.h"
#include "nspecialfx/nfxserver.h"
#include "nspecialfx/ncspecialfxclass.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncdictionary.h"
#include "entity/nentity.h"
#include "entity/nentityclassserver.h"
#include "nscene/ncsceneclass.h"
#include "nscene/nscenegraph.h"
#include "nscene/nsceneserver.h"

#include "util/nrandomlogic.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nFXDecal, "nfxobject");

//------------------------------------------------------------------------------
/**
*/
nFXDecal::nFXDecal() :
    decalPassIndex(-1),
    hasRandomRotation(false),
    randomRotation(0,0,0,1),
    initialRotation(0,0,0,1)
    //stickKey(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nFXDecal::~nFXDecal()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nFXDecal::Load()
{
    nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(this->GetFXClass());
    ncSceneClass* sceneClass = entityClass->GetComponent<ncSceneClass>();
    if (entityClass && sceneClass)
    {
        //get and preload the scene resource for the decal class
        ncLoaderClass* loaderClass = entityClass->GetComponent<ncLoaderClass>();
        if (loaderClass->AreResourcesValid() || loaderClass->LoadResources())
        {

            if ( sceneClass->GetRootNode() )
            {
                //force loading scene resources
                //no lods in decals!
                sceneClass->GetRootNode()->PreloadResources();
                
                this->refEntityClass = entityClass;
                this->decalPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('dcal'));
                ncSpecialFXClass* fxClass = entityClass->GetComponent<ncSpecialFXClass>();
                if (fxClass)
                {
                    n_assert( fxClass->GetType() == ncSpecialFXClass::FX_DECAL );
                    if( fxClass->GetType() != ncSpecialFXClass::FX_DECAL )
                    {
                        return false;
                    }

                    this->SetUnique(fxClass->GetUnique());
                    this->SetLifeTime(fxClass->GetLifeTime());
                }
                return true;
            } else
            {
                NLOG(  specialfx , ( 1| nFXServer::CREATION , "The %s has not a valid resource\n", entityClass->GetName() ) );
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nFXDecal::Trigger(nTime curTime)
{
    nFXObject::Trigger(curTime);

    // update the position of the decal with the target entity
    // ...or leave it to the target entity update(transform) method
    if (this->refTargetEntity.isvalid())
    {
        if (this->IsAlive())
        {
            //TODO- update stick values so that it can be faded away, etc.
        }
    }
    else
    {
        this->alive = false;//force calling Stop() at nFXServer::Trigger()
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXDecal::Restart()
{
    // set the position of the decal
    // TODO- relative to the target, for now only on static surfaces
    if (this->refTargetEntity.isvalid())
    {
        nFXServer::Instance()->StickEffect(this->refTargetEntity.get(), this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nFXDecal::Stop()
{
    // remove the decal from the list of sticks
    // TODO- relative to the target, for now only on static surfaces
    if (this->refTargetEntity.isvalid())
    {
        nFXServer::Instance()->UnstickEffect(this->refTargetEntity.get(), this);
    }
}

//------------------------------------------------------------------------------
/**
    Render
*/
bool
nFXDecal::Render(nSceneGraph* sceneGraph)
{
    if (this->refTargetEntity.isvalid())
    {
        //no lods in decals!
        ncSceneClass* sceneClass = this->refEntityClass->GetComponentSafe<ncSceneClass>();
        if (sceneClass)
        {
            nSceneNode* sceneRoot = sceneClass->GetRootNode();
            n_assert(sceneRoot);
            sceneGraph->BeginGroup(this->decalPassIndex, sceneRoot, this->refTargetEntity.get());

            if( this->hasRandomRotation)
            {
                quaternion quat = this->initialRotation * this->randomRotation;
                this->transform.setquatrotation(quat);
            }

            sceneGraph->SetTopModelTransform(this->GetTransform().getmatrix());

            vector3 pos, scale;
            quaternion qq;
            this->GetTransform().getmatrix().get(scale, qq, pos);
            NLOG(specialfx, (9| nFXServer::RENDER, "Render Point: %f %f %f rotation: %f %f %f %f", pos.x, pos.y, pos.z, qq.x, qq.y, qq.z, qq.w ));

            sceneRoot->Attach(sceneGraph, this->refTargetEntity.get());
            sceneGraph->EndGroup();
            return true;
        }
    }
    return false;
}


//------------------------------------------------------------------------------
/**
*/
void
nFXDecal::SetRandomRotation(bool random)
{
    //FIXME has to be called after setting the orientation
    this->hasRandomRotation = random;
    if( random )
    {
        // Rotate in Y Axis
        float alpha = n_rand_real_in_range(0.f, 2.f*N_PI);
        this->randomRotation = quaternion( 0.f, n_sin(alpha/2), 0.f, n_cos(alpha/2));
        this->initialRotation = this->transform.getquatrotation();  
    }
}

//------------------------------------------------------------------------------
/**
*/
quaternion
nFXDecal::GetRandomRotation()
{
    return this->randomRotation;
}

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nFXDecal)
    NSCRIPT_ADDCMD('MSDE', void, SetTargetEntity, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('MGDE', nEntityObject*, GetTargetEntity, 0, (), 0, ());
    NSCRIPT_ADDCMD('AAFK', void, SetRandomRotation, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('AAFL', quaternion, GetRandomRotation, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
