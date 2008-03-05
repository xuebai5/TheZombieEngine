#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncassetclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncassetclass.h"
#include "zombieentity/ninstscenebuilder.h"
#include "zombieentity/nbatchscenebuilder.h"
#include "zombieentity/nloaderserver.h"
#include "nscene/nscenenode.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ncscenelodclass.h"
#include "nscene/ncsceneragdollclass.h"
#include "nscene/nskingeometrynode.h"
#include "nmaterial/nmaterialserver.h"
#include "nspatial/ncspatialclass.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "ncragdoll/ncragdollclass.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "nspatial/nchorizonclass.h"
#include "ncsound/ncsoundclass.h"
#endif
#include "nscene/nanimator.h"
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nfileserver2.h"
#ifndef NGAME
#include "kernel/ndependencyserver.h"
#include "ndebug/nceditorclass.h"
#include "nscene/nsurfacenode.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentClass(ncAssetClass, ncLoaderClass);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAssetClass)

    NSCRIPT_ADDCMD_COMPCLASS('MSLF', void, SetLevelResource, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GSLF', const char *, GetLevelResource, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADZ', int, GetNumLevelResources, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGNL', int, GetNumLevels, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSRG', void, SetRagdollResource, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGRG', const char *, GetRagdollResource, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAEB', int, GetNumRagdollResources, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFH', void, ClearRagdollResources, 0, (), 0, ());

    NSCRIPT_ADDCMD_COMPCLASS('AADT', void, CreateAnimator, 3, (const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MADA', void, AddAnimator, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADV', int, GetNumAnimators, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADX', const char*, GetAnimatorNodeAt, 1, (int), 0, ());

    NSCRIPT_ADDCMD_COMPCLASS('MCSD', void, ClearSceneDependencies, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGND', int, GetNumSceneDependencies, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSSD', void, SetSceneDependency, 5, (int, const char *, const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MASD', void, AddSceneDependency, 5, (int, const char *, const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSOI', void, SetSceneIntOverride, 4, (int, const char *, const char *, int), 0, ());

#ifdef NGAME
    //hack- define dummy method
    NSCRIPT_ADDCMD_COMPCLASS('MSAE', void, SetAssetEditable, 1, (bool), 0, ());
#else
    NSCRIPT_ADDCMD_COMPCLASS('MSLA', void, SetLevelAsset, 3, (nString&, nString&, nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLA', void, GetLevelAsset, 0, (), 3, (nString&, nString&, nString&));
    NSCRIPT_ADDCMD_COMPCLASS('MSRA', void, SetRagdollAsset, 3, (nString&, nString&, nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGRA', void, GetRagdollAsset, 0, (), 3, (nString&, nString&, nString&));
    NSCRIPT_ADDCMD_COMPCLASS('MSAE', void, SetAssetEditable, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGAE', bool, GetAssetEditable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSAI', void, SetAssetInstanced, 3, (bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGAI', void, GetAssetInstanced, 0, (), 3, (bool&, bool&, bool&));
    NSCRIPT_ADDCMD_COMPCLASS('MSAF', void, SetAssetInstanceFrequency, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGAF', void, GetAssetInstanceFrequency, 0, (), 3, (int&, int&, int&));
    NSCRIPT_ADDCMD_COMPCLASS('MSBD', void, SetBatchDepthShapes, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBD', bool, GetBatchDepthShapes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSBM', void, SetBatchShapesByMaterial, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBM', bool, GetBatchShapesByMaterial, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSBL', void, SetBatchDepthShapesByCell, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBL', bool, GetBatchDepthShapesByCell, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSSL', void, SetShadowCasterLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGSL', int,  GetShadowCasterLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSDS', void, SetDisableShadowLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGDS', int,  GetDisableShadowLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSAP', void, SetMaterialProfile, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGAP', void, GetMaterialProfile, 0, (), 3, (int&, int&, int&));
    NSCRIPT_ADDCMD_COMPCLASS('MSNM', void, SetNumMaterialsByLevel, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGNM', void, GetNumMaterialsByLevel, 0, (), 3, (int&, int&, int&));
    NSCRIPT_ADDCMD_COMPCLASS('ZLDR', bool, LoadResources, 0, (), 0, ());

#endif
   
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    constructor
*/
ncAssetClass::ncAssetClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncAssetClass::~ncAssetClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::InitInstance(nObject::InitInstanceMsg initType)
{
    if ( ! this->GetEntityClass()->IsUserCreated() )
    {
        return; // the native class has not a resource
    }
    if( initType != nObject::NewInstance )
    {
        #ifndef NGAME
        /*** Load Asset for ncEditor***/
        ncEditorClass * editor = this->GetComponent<ncEditorClass>();
        if( editor )
        {
            // create filename of debug chunk
            nString path = this->GetResourceFile();
            if( path.GetExtension() )
            {
                path.StripExtension();
            }
            path.Append( "/debug/chunk.n2" );

            nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
            n_assert(ps);
            if( ps )
            {
                if( nFileServer2::Instance()->FileExists( path.Get() ) )
                {
                    nKernelServer::Instance()->PushCwd( this->GetEntityClass() );
                    nKernelServer::Instance()->Load( path.Get(), false );
                    nKernelServer::Instance()->PopCwd();
                }
            }
        }
        #endif//!NGAME
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncAssetClass::GetNumLevels() const
{
    return this->levelResources.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetLevelResource(int level, const char* fileName)
{
    int i;
    for (i = 0; i < this->GetNumLevels(); ++i)
    {
        if (this->levelResources[i].level == level)
        {
            if (!this->levelResources[i].resourceFiles.Find(nString(fileName)))
            {
                this->levelResources[i].resourceFiles.Append(fileName);

                this->Invalidate();
            }
            return;
        }
    }

    LevelResource newLevelResource;
    newLevelResource.level = level;
    newLevelResource.resourceFiles.Append(fileName);
    #ifndef NGAME
    newLevelResource.numMaterials = 0;
    #endif
    this->levelResources.Append(newLevelResource);

    this->Invalidate();
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncAssetClass::GetLevelResource(int level, int resourceIdx) const
{
    int i;
    for (i = 0; i < this->GetNumLevels(); ++i)
    {
        if (this->levelResources[i].level == level)
        {
            return this->levelResources[i].resourceFiles[resourceIdx].Get();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncAssetClass::GetNumLevelResources(int level) const
{
    int i;
    for (i = 0; i < this->GetNumLevels(); ++i)
    {
        if (this->levelResources[i].level == level)
        {
            return this->levelResources[i].resourceFiles.Size();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetRagdollResource(int level, const char *fileName)
{
    int i;
    for (i = 0; i < this->ragdollResources.Size(); ++i)
    {
        if (this->ragdollResources[i].level == level)
        {
            this->ragdollResources[i].resourceFiles.Append(fileName);
            return;
        }
    }

    LevelResource newLevelResource;
    newLevelResource.level = level;
    newLevelResource.resourceFiles.Append(fileName);
    #ifndef NGAME
    newLevelResource.numMaterials = 0;
    #endif
    this->ragdollResources.Append(newLevelResource);
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncAssetClass::GetRagdollResource(int level) const
{
    int i;
    for (i = 0; i < this->ragdollResources.Size(); ++i)
    {
        if (this->ragdollResources[i].level == level)
        {
            return this->ragdollResources[i].resourceFiles[0].Get();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncAssetClass::GetNumRagdollResources(int level) const
{
    int i;
    for (i = 0; i < this->ragdollResources.Size(); ++i)
    {
        if (this->ragdollResources[i].level == level)
        {
            return this->ragdollResources[i].resourceFiles.Size();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::ClearRagdollResources()
{
    if (this->ragdollResources.Size() > 0)
    {
        this->ragdollResources.Clear();
    }
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncAssetClass::LoadSceneResource(nSceneNode* libSceneRoot, const char *resourceFile)
{
    nSceneNode *sceneRes;

    // try to load scene resource as a standalone file
    if (nKernelServer::Instance()->GetFileServer()->FileExists(resourceFile))
    {
        sceneRes = (nSceneNode *) this->LoadResource(libSceneRoot, resourceFile);
        n_assert(sceneRes);
    }
    else
    {
        // try to load scene resource from asset folder
        nString className(resourceFile);
        className.StripTrailingSlash();
        className = className.ExtractFileName();
        
        nString scenePath(resourceFile);
        scenePath.StripTrailingSlash();
        scenePath.Append("/");
        scenePath.Append("scene/");
        scenePath.Append(className.Get());
        scenePath.Append(".n2");

        if (nKernelServer::Instance()->GetFileServer()->FileExists(scenePath.Get()))
        {
            sceneRes = (nSceneNode *) this->LoadResource(libSceneRoot, scenePath);
            n_assert(sceneRes);
        }
        else
        {
            sceneRes = 0;
        }
    }

    // optimize scene resource
    //TEMP- for editor only, keep this until there is a process to build game assets
    ncDictionaryClass* varContext = this->GetComponent<ncDictionaryClass>();
    if (varContext)
    {
        if (varContext->GetBoolVariable("batchDepthShapes") ||
            varContext->GetBoolVariable("batchShapesByMaterial") ||
            varContext->GetBoolVariable("batchDepthShapesByCell"))
        {
            if (sceneRes)
            {
                nBatchSceneBuilder batchSceneBuilder;
                // batch shapes in depth pass (reduce depth pass to a single call)
                if (varContext->GetBoolVariable("batchDepthShapes"))
                {
                    batchSceneBuilder.SetRootNode(sceneRes);
                    nSceneNode* batchSceneRes = batchSceneBuilder.BuildBatchedDepthScene();
                    if (batchSceneRes)
                    {
                        sceneRes = batchSceneRes;
                    }
                }
                // batch by material (multiple MAX objects in brushes)
                if (varContext->GetBoolVariable("batchShapesByMaterial"))
                {
                    batchSceneBuilder.SetRootNode(sceneRes);
                    nSceneNode* batchSceneRes = batchSceneBuilder.BuildBatchedSceneByMaterial();
                    if (batchSceneRes)
                    {
                        sceneRes = batchSceneRes;
                    }
                }
                // batch shapes in depth pass by cell objects (MAX objects in single cell)
                if (varContext->GetBoolVariable("batchDepthShapesByCell"))
                {
                    batchSceneBuilder.SetRootNode(sceneRes);
                    nSceneNode* batchSceneRes = batchSceneBuilder.BuildBatchedDepthSceneByCell();
                    if (batchSceneRes)
                    {
                        sceneRes = batchSceneRes;
                    }
                }
            }
        }
    }

    #ifndef NGAME
    if ( sceneRes && this->GetAssetEditable() )
    {
        sceneRes->BindDirtyDependence( this->GetEntityClass() );
    }
    #endif
    
    if (sceneRes)
    {
        this->LoadAnimators(sceneRes);
    }

    return sceneRes;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncAssetClass::LoadResources()
{
    if (this->AreResourcesValid())
    {
        return true;
    }

    // initialize root node for scene resources
    nKernelServer* kernelServer = nKernelServer::Instance();
    nSceneNode* libSceneRoot = (nSceneNode*) kernelServer->Lookup("/lib/scene");
    if (!libSceneRoot)
    {
        libSceneRoot = (nSceneNode*) kernelServer->New("ntransformnode", "/lib/scene");
    }

    // load resources for scene component class
    ncSceneClass *sceneClass = this->entityClass->GetComponent<ncSceneClass>();

    if (sceneClass)
    {
        // invalidate all class animators, to remove all invalid ones
        for (int i = 0; i < this->GetNumAnimators(); ++i)
        {
            this->animatorArray[i].isValid = false;
        }

        if (this->GetResourceFile())
        {
            // load scene resource
            nSceneNode* sceneRes = this->LoadSceneResource(libSceneRoot, this->GetResourceFile());
            if (sceneRes)
            {
                // load scene dependencies on top of the resource
                this->LoadSceneDependencies(0, sceneRes);

                // get number of materials in the resource
                //this->numMaterials[0] = ...

                // HACK- explicitly assign LOD to all skin shapes in the resource
                if (this->GetComponent<ncCharacterClass>())
                {
                    this->SetSkeletonLevelForSkinnedGeometry(sceneRes, 0);
                }

                if (sceneClass->GetInstanceEnabled())
                {
                    const char *streamName = sceneRes->GetName();//TEMP!
                    sceneRes = this->LoadInstancedScene(sceneRes, streamName);
                }

                this->GetEntityClass()->LockMutex();
                sceneClass->SetRootNode(sceneRes);
                this->GetEntityClass()->UnlockMutex();

                //HACK! do not preload class resources for terrains, it crashes...
                //it will be safely loaded at terrain initialization
                #ifndef __ZOMBIE_EXPORTER__
                if (!this->entityClass->GetComponent<ncTerrainGMMClass>())
                {
                    sceneClass->LoadResources();

                    // load class overrides from global variables
                    this->LoadDefaultSceneVariables(sceneClass);
                }
                #endif
            }
        }
    }

    // recursively load resources for LOD classes, if any
    // TODO and scene plug classes too!
    ncSceneLodClass *sceneLodClass = this->GetComponent<ncSceneLodClass>();
    if (sceneLodClass)
    {
        int level;
        for (level = 0; level < this->GetNumLevels(); ++level)
        {
            #ifndef NGAME
            this->levelResources[level].numMaterials = 0;
            #endif

            for (int resourceIdx = 0; resourceIdx < this->GetNumLevelResources(level); resourceIdx++)
            {
                nString levelResource = this->GetLevelResource(level, resourceIdx);

                nSceneNode* sceneRes = this->LoadSceneResource(libSceneRoot, levelResource.Get());
                if (sceneRes)
                {
                    // load scene dependencies on top of the resource
                    this->LoadSceneDependencies(level, sceneRes);

                    #ifndef NGAME
                    // get number of different materials in the resource
                    this->levelResources[level].numMaterials += this->GetNumMaterials(sceneRes);
                    #endif

                    // HACK- explicitly assign LOD to all skin shapes in the resource
                    if (this->GetComponent<ncCharacterClass>())
                    {
                        this->SetSkeletonLevelForSkinnedGeometry(sceneRes, level);
                    }

                    // initialized scene resources
                    if (sceneLodClass->GetLevelInstanced(level))
                    {
                        const char *streamName = sceneRes->GetName();//TEMP!
                        sceneRes = this->LoadInstancedScene(sceneRes, streamName);
                    }

                    this->GetEntityClass()->LockMutex();
                    sceneLodClass->SetLevelRoot(level, sceneRes);
                    this->GetEntityClass()->UnlockMutex();
                }
            }
        }

        // select the shadow caster resource from a different level
        ncDictionaryClass* dictionaryClass = this->GetComponent<ncDictionaryClass>();
        if (dictionaryClass && dictionaryClass->IsVariableValid("ShadowCasterLevel"))
        {
            int shadowLevel = dictionaryClass->GetIntVariable("ShadowCasterLevel");

            int disableShadowLevel = -1;
            if (dictionaryClass->IsVariableValid("DisableShadowLevel"))
            {
                disableShadowLevel = dictionaryClass->GetIntVariable("DisableShadowLevel");
            }

            this->LoadShadowCasterLevel(sceneLodClass, shadowLevel, disableShadowLevel);
        }

        // BATCH- create a fake batched lod for a given distance
        // use only the first root node, inserts are not implemented here
        if (dictionaryClass && dictionaryClass->GetFloatVariable("CreateFakeBatchLevel"))
        {
            float newLevelDistance = dictionaryClass->GetFloatVariable("CreateFakeBatchLevel");

            this->LoadFakeBatchLevel(sceneLodClass, newLevelDistance);
        }

        // load class overrides from global variables
        this->LoadDefaultSceneVariables(sceneClass);

        sceneLodClass->LoadResources();
    }

    // load skeleton resources
    ncCharacterClass *charComp = this->entityClass->GetComponent<ncCharacterClass>();
    if (charComp)
    {
        //for (int i = 0; i< charComp->GetNumberLevelSkeletons(); i++)
        //{
        //    if (!charComp->GetSkeletonClassPointer(i)->LoadResources())
        //    {
        //        return false;
        //    }
        //}

        // load ragdoll resource for all levels
        nString ragdollSkeletonClass;
        if (charComp->GetRagdollSkeletonClass(ragdollSkeletonClass))
        {
            ncSceneRagdollClass* ragdollComp = this->GetComponent<ncSceneRagdollClass>();
            if (ragdollComp)
            {
                int level;
                for (level = 0; level < this->ragdollResources.Size(); ++level)
                {
                    const char *levelResource = this->GetRagdollResource(level);
                    n_assert(levelResource);
                    nSceneNode* sceneRes = this->LoadSceneResource(libSceneRoot, levelResource);
                    if (sceneRes)
                    {
                        ragdollComp->SetRagdollRoot(level, sceneRes);
                    }
                }
            }
        }
    }

#ifndef __ZOMBIE_EXPORTER__
    // load terrain geometry data (heightmap)
    ncTerrainGMMClass * terrainComp = this->entityClass->GetComponent<ncTerrainGMMClass>();
    if (terrainComp)
    {
        if (!terrainComp->LoadResources())
        {
            return false;
        }
    }

    ncTerrainVegetationClass * terrainVeg = this->GetComponent<ncTerrainVegetationClass>();
    if (terrainVeg)
    {
        if ( ( ! terrainVeg->LoadScene()) ||  (!terrainVeg->LoadResources()) )
        {
            return false;
        }
    }

    // load terrain material data
    ncTerrainMaterialClass * terrainMaterialComp = this->entityClass->GetComponent<ncTerrainMaterialClass>();
    if (terrainMaterialComp)
    {
        if (!terrainMaterialComp->LoadResources())
        {
            return false;
        }
    }

    // load horizon information
    ncHorizonClass * horizonClass = this->entityClass->GetComponent<ncHorizonClass>();
    if (horizonClass)
    {
        horizonClass->SetSideSizeScale(terrainComp->GetBlockSideSizeScaled());
        if (!horizonClass->LoadResources())
        {
            return false;
        }
    }

    // load sound data
    ncSoundClass * soundClass = this->entityClass->GetComponent<ncSoundClass>();
    if( soundClass )
    {
        if( ! soundClass->LoadResources() )
        {
            return false;
        }
    }
#endif

    // load rest of components
    // FIXME other components
    //if (spatialComp)
    //if (physicsComp)
    //if (spaceComp)
    //if (AIComp)
    //...
    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::UnloadResources()
{
    n_assert(this->AreResourcesValid());

    // release resources for scene component class
    ncSceneClass *sceneComp = this->entityClass->GetComponent<ncSceneClass>();
    if (sceneComp)
    {
        // release scene resource, if any
        nSceneNode *sceneRes = sceneComp->GetRootNode();
        if (sceneRes)
        {
            //release instanced scene, if any
            if (sceneComp->GetInstanceEnabled())
            {
                nInstSceneBuilder instSceneBuilder(sceneRes);
                instSceneBuilder.DestroyInstancedScene();
            }

            sceneComp->UnloadResources();
            sceneRes->Release();
        }
    }

    // release resources for scene lod component class
    ncSceneLodClass *sceneLodComp = this->GetComponent<ncSceneLodClass>();
    if (sceneLodComp)
    {
        for (int level = 0; level < sceneLodComp->GetNumLevels(); level++)
        {
            for (int index = 0; index < sceneLodComp->GetNumLevelRoots(level); ++index)
            {
                nSceneNode* levelRoot = sceneLodComp->GetLevelRoot(level, index);
                if (levelRoot)
                {
                    //unload instanced scene, if any
                    if (sceneLodComp->GetLevelInstanced(level))
                    {
                        nInstSceneBuilder instSceneBuilder(levelRoot);
                        instSceneBuilder.DestroyInstancedScene();
                    }

                    levelRoot->Release();
                }
            }
        }
        sceneLodComp->UnloadResources();
    }

    // release character resources
    ncCharacterClass *charComp = this->entityClass->GetComponent<ncCharacterClass>();
    nString ragdollSkeletonClass;
    if (charComp && charComp->GetRagdollSkeletonClass(ragdollSkeletonClass))
    {
        // release indices to all ragdoll levels
        ncSceneRagdollClass* ragdollComp = this->GetComponent<ncSceneRagdollClass>();
        if (ragdollComp)
        {
            ragdollComp->ClearRagdollLevels();
        }
    }


#ifndef __ZOMBIE_EXPORTER__
    // load terrain geometry data (heightmap)
    ncTerrainGMMClass * terrainComp = this->entityClass->GetComponent<ncTerrainGMMClass>();
    if (terrainComp)
    {
        terrainComp->UnloadResources();
    }

    // load terrain geometry data (heightmap)
    ncTerrainVegetationClass * terrainVeg = this->GetComponent<ncTerrainVegetationClass>();
    if (terrainVeg)
    {
        terrainVeg->UnloadResources();
        terrainVeg->UnloadScene();
    }

    // load terrain material data
    ncTerrainMaterialClass * terrainMaterialComp = this->entityClass->GetComponent<ncTerrainMaterialClass>();
    if (terrainMaterialComp)
    {
        terrainMaterialComp->UnloadResources();
    }

    // load horizon information
    ncHorizonClass * horizonClass = this->entityClass->GetComponent<ncHorizonClass>();
    if (horizonClass)
    {
        horizonClass->UnloadResources();
    }

    // unload sound resources
    ncSoundClass * soundClass = this->entityClass->GetComponent<ncSoundClass>();
    if( soundClass )
    {
        soundClass->UnloadResources();
    }
#endif
}

//------------------------------------------------------------------------------
/**
    FIXME these checks are too heavy, store instead a resourcesValid variable
    but this needs to ensure that in case of a class not being fully loaded, 
    the partially loaded asset is released.
*/
bool
ncAssetClass::AreResourcesValid()
{
    // skip ragdoll skeleton classes, they don't own any resources
    ncRagDollClass* ragdollClass = this->entityClass->GetComponent<ncRagDollClass>();
    if (ragdollClass)
    {
        return true;
    }

    // check scene class resources
    ncSceneLodClass *sceneLodClass = this->entityClass->GetComponent<ncSceneLodClass>();
    if (sceneLodClass && this->GetNumLevels() > 0)
    {
        if (!sceneLodClass->IsValid())
        {
            return false;
        }
    }
    else
    {
        ncSceneClass *sceneClass = this->entityClass->GetComponent<ncSceneClass>();
        if (sceneClass && this->GetResourceFile() && !sceneClass->IsValid())
        {
            return false;
        }
    }

#ifndef __ZOMBIE_EXPORTER__
    // check terrain geometry data (heightmap)
    ncTerrainVegetationClass * terrainVeg = this->GetComponent<ncTerrainVegetationClass>();
    if (terrainVeg&& !terrainVeg->IsValid())
    {
        return false;
    }

    ncTerrainGMMClass * terrainComp = this->entityClass->GetComponent<ncTerrainGMMClass>();
    if (terrainComp && !terrainComp->IsValid())
    {
        return false;
    }

    // check terrain material data
    ncTerrainMaterialClass * terrainMaterialComp = this->entityClass->GetComponent<ncTerrainMaterialClass>();
    if (terrainMaterialComp && !terrainMaterialComp->IsValid())
    {
        return false;
    }

    // NOTE: do not test for horizon resource intentionally (it is not mandatory to have it)
#endif

    /// TODO: check sound resources
    // there is not check because files can be not founded

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncAssetClass::LoadInstancedScene(nSceneNode* rootNode, const char *streamName)
{
    n_assert(rootNode);

    // build and load instanced scene
    nInstSceneBuilder instSceneBuilder;
    instSceneBuilder.SetRootNode(rootNode);
    instSceneBuilder.SetInstanceStream(streamName);
    instSceneBuilder.BuildInstancedScene();

    return rootNode;
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::CreateAnimator(const char *animatorType, const char *animatorName, const char *node)
{
    nRoot* libAnims = static_cast<nRoot*>(nKernelServer::ks->Lookup("/lib/anims"));
    if (!libAnims)
    {
        //create /lib/anims
        libAnims = static_cast<nRoot*>(nKernelServer::ks->New("nroot", "/lib/anims"));
        n_assert(libAnims);
    }

    //create /lib/anims/assetName
    nString assetName(this->GetResourceFile());
    assetName.StripTrailingSlash();
    assetName.StripExtension();
    this->animatorsNOH = "/lib/anims/";
    this->animatorsNOH.Append(assetName.ExtractFileName());

    nRoot* animsRoot = static_cast<nRoot*>(nKernelServer::ks->New("nroot", this->animatorsNOH.Get()));
    n_assert(animsRoot);

    nString path(animsRoot->GetFullName());
    path += "/";
    path += animatorName;

    nAnimator* animator = static_cast<nAnimator*>(nKernelServer::ks->New(animatorType, path.Get()));
    n_assert2(animator, "Failed to create scene animator.");

    AnimatorEntry newEntry;
    newEntry.refAnimator = animator;
    newEntry.targetNodePath = node;
    newEntry.isValid = true;
    this->animatorArray.Append(newEntry);

    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(animator, nObject::LoadedInstance);
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncAssetClass::FindShadowResource(nSceneNode* sceneRes)
{
    nSceneNode* childNode;
    for (childNode = (nSceneNode*) sceneRes->GetHead();
         childNode; 
         childNode = (nSceneNode*) childNode->GetSucc())
    {
        //extract the first 12 characters of the node name to know it's a shadow
        nString nodeName(childNode->GetName());
        if (nodeName.Length() > 12)
        {
            nodeName = nodeName.ExtractRange(0, 12);
        }
        if (nodeName.BeginBy("shape.shadow"))
        {
            return childNode;
        }
        else
        {
            nSceneNode* shadowRes = this->FindShadowResource(childNode);
            if (shadowRes)
            {
                return shadowRes;
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncAssetClass::LoadShadowResource(int shadowLevel)
{
    ncSceneLodClass* sceneLodClass = this->GetComponent<ncSceneLodClass>();
    for (int index = 0; index < sceneLodClass->GetNumLevelRoots(shadowLevel); ++index)
    {
        nSceneNode* shadowRes = this->FindShadowResource(sceneLodClass->GetLevelRoot(shadowLevel, index));
        if (shadowRes)
        {
            //TODO- clone shadow resource as [/lib/scene/[class]_shadow] ?
            //shadowRes = shadowRes->Clone();
            return shadowRes;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetSkeletonLevelForSkinnedGeometry(nSceneNode* sceneRes, int level)
{
    n_assert(sceneRes);
    nSceneNode* childNode;
    for (childNode = (nSceneNode*) sceneRes->GetHead();
         childNode;
         childNode = (nSceneNode*) childNode->GetSucc())
    {
        if (childNode->IsA("nskingeometrynode"))
        {
            static_cast<nSkinGeometryNode*>(childNode)->SetSkeletonLevel(level);
        }
        this->SetSkeletonLevelForSkinnedGeometry(childNode, level);
    }
}

//------------------------------------------------------------------------------
/**
*/
nAnimator*
ncAssetClass::GetAnimatorAt(int index)
{
    n_assert(index < this->GetNumAnimators());
    return this->animatorArray[index].refAnimator.get();
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::AddAnimator(const char *name, const char *targetPath)
{
    if (nKernelServer::ks->Lookup(name))
    {
        AnimatorEntry newEntry;
        newEntry.refAnimator = (nAnimator*) nKernelServer::ks->Lookup(name);
        newEntry.targetNodePath = targetPath;
        this->animatorArray.Append(newEntry);

        this->Invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncAssetClass::GetNumAnimators() const
{
    return this->animatorArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncAssetClass::GetAnimatorNodeAt(int index) const
{
    return this->animatorArray[index].targetNodePath.Get();
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::LoadAnimators(nSceneNode* sceneRes)
{
    n_assert(sceneRes);
    int numAnimators = this->GetNumAnimators();
    int i;
    for(i = 0; i < numAnimators; ++i)
    {
        // test if scenenode exists
        nString targetNode(sceneRes->GetFullName());
        targetNode.Append("/");
        targetNode.Append(this->animatorArray[i].targetNodePath.Get());
        nSceneNode *sceneNode = static_cast<nSceneNode*>(nKernelServer::ks->Lookup(targetNode.Get()));
        if (sceneNode)
        {
            // remove old animator if it exists
            for (int j = 0; j< sceneNode->GetNumAnimators(); j++)
            {
                nString oldAnimator = sceneNode->GetAnimatorAt(j);
                nString animatorName = oldAnimator.ExtractFileName();
                if (animatorName == this->animatorArray[i].refAnimator->GetName())
                {
                    //FIXME ma.garcias crash when trying to removeanimator in strcmp
                    //add release animator
                    //sceneNode->RemoveAnimator( oldAnimator.Get() );
                }
            }

            // add animator to scenenode
            nString animatorNOH = this->animatorsNOH;
            animatorNOH += "/";
            animatorNOH += this->animatorArray[i].refAnimator->GetName();

            sceneNode->AddAnimator(animatorNOH.Get());
            this->animatorArray[i].isValid = true;
        }
        else
        {
            // leave animator as invalid, when saving class will be removed
            nEntityClassServer::Instance()->SetEntityClassDirty(this->GetEntityClass(), true);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Clear set of scene dependencies for a level
*/
void
ncAssetClass::ClearSceneDependencies()
{
    this->sceneDependencies.Clear();
}

//------------------------------------------------------------------------------
/**
    Get number of scene dependencies for a level
*/
int
ncAssetClass::GetNumSceneDependencies(int level)
{
    int numDependencies = 0;
    for (int index = 0; index < this->sceneDependencies.Size(); /*empty*/)
    {
        if (sceneDependencies[index].level == level)
        {
            ++numDependencies;
        }
    }
    return numDependencies;
}

//------------------------------------------------------------------------------
/**
    @param  target  relative path to target node
    @param  file    path to file
    @param  command command to execute on target node
    @param  server  NOH path to dependency server
*/
void
ncAssetClass::SetSceneDependency(int level, const char *target, const char *file, const char *command, const char *server)
{
    if (!this->sceneDependencies.Find(SceneDependency(level, target, file, command, server)))
    {
        this->sceneDependencies.Append(SceneDependency(level, target, file, command, server));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::AddSceneDependency(int /*level*/, const char * /*target*/, const char * /*path*/, const char * /*command*/, const char * /*server*/)
{
    //TODO- add dependencies from editor tools, to persist them when saving class
    //...
}

//------------------------------------------------------------------------------
/**
    @param  level   resource level to apply the dependency
    @param  target  relative path to target node
    @param  command command to execute on target node
    @param  value   int value to override
*/
void
ncAssetClass::SetSceneIntOverride(int level, const char *target, const char *command, int value)
{
    if (!this->sceneDependencies.Find(SceneDependency(level, target, command, value)))
    {
        this->sceneDependencies.Append(SceneDependency(level, target, command, value));
    }
}

//------------------------------------------------------------------------------
/**
    overwrite the scene dependencies
*/
void
ncAssetClass::LoadSceneDependencies(int level, nSceneNode* sceneRes)
{
    nKernelServer::ks->PushCwd(sceneRes);

    for (int index = 0; index < this->sceneDependencies.Size(); ++index)
    {
        SceneDependency& sceneDep = this->sceneDependencies[index];
        if (sceneDep.level == level && sceneDep.refTargetNode.isvalid())
        {
            if (sceneDep.value.GetType() == nArg::Void)
            {
                sceneDep.refTargetNode->SetDependency(sceneDep.fileName, sceneDep.serverPath, sceneDep.command);
            }
            else
            {
                // dispatch manually the command to the target node
                nCmdProto *cmdProto = sceneDep.refTargetNode->GetClass()->FindCmdByName(sceneDep.command.Get());
                n_assert2(cmdProto, "Invalid command in scene dependencies.");
                nCmd *cmd = cmdProto->NewCmd();

                switch (sceneDep.value.GetType())
                {
                case nArg::Int:
                    cmd->In()->SetI(sceneDep.value.GetI());
                    break;
                    //...add any other overridable types required
                default:
                    n_assert_always();
                }

                cmd->Rewind();
                cmdProto->Dispatch(sceneDep.refTargetNode, cmd);
                cmdProto->RelCmd(cmd);
            }
        }
    }

    nKernelServer::ks->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::LoadFakeBatchLevel(ncSceneLodClass* sceneLodClass, float levelDistance)
{
    nSceneNode* sceneRes;
    int numLevels = sceneLodClass->GetNumLevels();
    if (numLevels == 0)
    {
        sceneRes = sceneLodClass->GetRootNode();
    }
    else
    {
        sceneRes = sceneLodClass->GetLevelRoot(numLevels - 1, 0);
    }
    
    if (sceneRes)
    {
        nBatchSceneBuilder batchSceneBuilder;

        batchSceneBuilder.SetRootNode(sceneRes);
        nSceneNode* batchSceneRes = batchSceneBuilder.BuildFakeBatchLevel();
        if (batchSceneRes)
        {
            int level;
            if (numLevels == 0)
            {
                sceneLodClass->SetLevelRoot(0, sceneRes);
                level = 1;
            }
            else
            {
                level = numLevels - 1;
            }
            sceneLodClass->SetLevelRoot(level, batchSceneRes);//fake level

            // set a distance for the fake level
            ncSpatialClass* spatialComp = this->GetComponent<ncSpatialClass>();
            if (spatialComp)
            {
                spatialComp->SetLevelDistance(level, levelDistance);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::LoadShadowCasterLevel(ncSceneLodClass* sceneLodClass, int shadowLevel, int disableLevel)
{
    if (shadowLevel != -1 && shadowLevel < this->GetNumLevels())
    {
        // find the shadow caster sub-scene in the level, and clone it
        nSceneNode* shadowRes = this->LoadShadowResource(shadowLevel);
        if (shadowRes)
        {
            //TODO- clone it first?
            for (int level = 0; level < this->GetNumLevels(); ++level)
            {
                if (level != shadowLevel)
                {
                    for (int index = 0; index < sceneLodClass->GetNumLevelRoots(level); ++index)
                    {
                        nSceneNode* oldShadowRes = this->FindShadowResource(sceneLodClass->GetLevelRoot(level, index));
                        if (oldShadowRes)
                        {
                            oldShadowRes->Release();
                        }
                    }

                    if (level != disableLevel)
                    {
                        sceneLodClass->SetLevelRoot(level, shadowRes);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::LoadDefaultSceneVariables(ncSceneClass* sceneClass)
{
    // if the class has no specific variables to load default values from,
    // use the default values provided here.
    nVariableServer* varServer = nVariableServer::Instance();

    // get this information from material server
    const nMaterialDb& materialDb = nMaterialServer::Instance()->GetMaterialDb();
    const nArray<nMaterialDb::ShaderParam>& shaderParams = materialDb.GetShaderParams();
    nShaderParams& shaderOverrides = sceneClass->GetShaderOverrides();
    ncDictionaryClass* varContext = this->GetComponent<ncDictionaryClass>();

    for (int index = 0; index < shaderParams.Size(); ++index)
    {
        if (shaderOverrides.IsParameterValid(shaderParams[index].param))
        {
            continue;
        }

        const nVariable* globalVar(0);
        
        if (shaderParams[index].varHandle != nVariable::InvalidHandle)
        {
            //get the variable from the global context
            //using either the default channel in the materialdb,
            //or a custom string variable specific for this class
            if (varContext && varContext->VariableContext().GetVariable(shaderParams[index].varHandle))
            {
                nVariable *strVar = varContext->VariableContext().GetVariable(shaderParams[index].varHandle);
                if (strVar->GetType() == nVariable::String)
                {
                    globalVar = varServer->GetGlobalVariable(strVar->GetString());
                }
            }
            else
            {
                globalVar = varServer->GetGlobalVariable(shaderParams[index].varHandle);
            }

            //copy the variable value
            if (globalVar)
            {
                switch (shaderParams[index].type)
                {
                case nShaderState::Float:
                    shaderOverrides.SetArg(shaderParams[index].param, nShaderArg(globalVar->GetFloat()));
                    break;
                case nShaderState::Float4:
                    shaderOverrides.SetArg(shaderParams[index].param, nShaderArg(globalVar->GetFloat4()));
                    break;
                case nShaderState::Int:
                    shaderOverrides.SetArg(shaderParams[index].param, nShaderArg(globalVar->GetInt()));
                    break;
                }
            }
        }
    }

    #ifndef NGAME
    //register this class to be notified when any of these global variables is changed
    nVariableServer::Instance()->BindSignal(nVariableServer::SignalGlobalVariableChanged,
                                            this,
                                            &ncAssetClass::GlobalVariableChanged,
                                            0);
    #endif //NGAME
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::GlobalVariableChanged()
{
    ncSceneClass* sceneClass = this->GetComponent<ncSceneClass>();
    this->LoadDefaultSceneVariables(sceneClass);
    sceneClass->SetLastEditedTime(nTimeServer::Instance()->GetFrameTime());
}
#endif //NGAME

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::ReloadResources()
{
    if (!this->AreResourcesValid())
    {
        return;
    }

#ifndef NGAME
    // get all loaded entities of this class-
    nEntityClass* entityClass = this->GetEntityClass();

    // traverse all entities of this class and unload them all.
    nEntityObject* entityObject = nEntityObjectServer::Instance()->GetFirstEntityObject();
    while (entityObject)
    {
        if (entityObject->GetClass() == entityClass)
        {
            ncLoader* loaderComp = entityObject->GetComponent<ncLoader>();
            if (loaderComp->AreComponentsValid())
            {
                // schedule entity for reloading in next iteration
                loaderComp->UnloadComponents();
                nLoaderServer::Instance()->EntityNeedsLoading(entityObject);
            }
        }
        entityObject = nEntityObjectServer::Instance()->GetNextEntityObject();
    }


#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
ncAssetClass::SaveCmds(nPersistServer * ps)
{
    if (ncLoaderClass::SaveCmds(ps))
    {
        nCmd *cmd;

        #ifndef NGAME
        if (this->GetAssetEditable())
        {
            // persist the scene resource through a dependency server
            nDependencyServer *depServer = ps->GetDependencyServer("ncassetclass", "setresourcefile");
            if (depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
            {
                //HACK- provide the parent path to the dependency server
                nString path(this->GetResourceFile());
                ncSceneClass* sceneComp = this->GetComponent<ncSceneClass>();
                if (sceneComp && sceneComp->GetRootNode())
                {
                    depServer->SaveObject((nObject *) sceneComp->GetRootNode(), "setresourcefile", path);
                    sceneComp->GetRootNode()->RecruseSetObjectDirty(false);
                }
            }
        }
        #endif

        if (this->GetNumLevels() > 0)
        {
            #ifndef NGAME
            int editableLevel = -1;
            ncEditorClass* editorComp = this->GetComponent<ncEditorClass>();
            if (editorComp && editorComp->IsSetClassKey("editableLevel"))
            {
                editableLevel = editorComp->GetClassKeyInt("editableLevel");
            }
            #endif

            int level;
            for (level = 0; level < this->GetNumLevels(); ++level)
            {
                LevelResource& levelResource = this->levelResources[level];

                int index;
                for (index = 0; index < this->GetNumLevelResources(level); ++index)
                {
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSLF');
                    cmd->In()->SetI(levelResource.level);
                    cmd->In()->SetS(levelResource.resourceFiles[index].Get());
                    ps->PutCmd(cmd);

                    #ifndef NGAME
                    if (this->GetAssetEditable() && level == editableLevel)
                    {
                        // save the level scene resource through a dependency server
                        nDependencyServer *depServer = ps->GetDependencyServer("ncassetclass", "setresourcefile");
                        if (depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
                        {
                            //HACK- provide the parent path to the dependency server
                            nString path(levelResource.resourceFiles[index]);
                            ncSceneLodClass* sceneLodComp = this->GetComponent<ncSceneLodClass>();
                            if (sceneLodComp && sceneLodComp->GetLevelRoot(level, index))
                            {
                                depServer->SaveObject((nObject*) sceneLodComp->GetLevelRoot(level, index), "setlevelresource", path);
                            }
                        }
                    }
                    #endif
                }
            }
        }

        // --- cleanragdollresource ---
        if( ps->GetSaveType() == nPersistServer::SAVETYPE_PERSIST )
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAFH');
            ps->PutCmd(cmd);
        }

        for (int i = 0; i < this->ragdollResources.Size(); ++i)
        {
            // --- setragdollresource ---
            LevelResource& levelResource = this->ragdollResources[i];

            int j;
            for (j = 0; j< this->GetNumRagdollResources(i); ++j)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'MSRG');
                cmd->In()->SetI(levelResource.level);
                cmd->In()->SetS(levelResource.resourceFiles[j].Get());
                ps->PutCmd(cmd);
            }
        }

        int numAnimators = this->GetNumAnimators();
        for (int i = 0; i < numAnimators; ++i)
        {
            if (this->animatorArray[i].isValid)
            {
                // --- createanimator ---
                nAnimator* animator = this->GetAnimatorAt(i);
                n_assert2(animator, "Null pointer");
                nString animatorName("anim.");
                animatorName += i;

                cmd = ps->GetCmd(this->GetEntityClass(), 'AADT');
                cmd->In()->SetS(animator->GetClass()->GetName());
                cmd->In()->SetS(animatorName.Get());
                cmd->In()->SetS(this->animatorArray[i].targetNodePath.Get());

                if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd))
                {
                    if (!animator->SaveCmds(ps))
                    {
                        return false;
                    }
                    ps->EndObject(true);
                }
            }
        }

        // persist scene dependencies, sorted by level
        int numDeps = this->sceneDependencies.Size();
        if (numDeps > 0)
        {
            // --- clearscenedependencies ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'MCSD');
            ps->PutCmd(cmd);

            for (int index = 0; index < numDeps; ++index)
            {
                const SceneDependency& dep = this->sceneDependencies[index];

                #ifndef NGAME
                if (dep.isDirty)
                {
                    //TODO- try to persist the dependency using the dependency server
                    //if failed (the node is not valid, etc):
                    //continue;
                }
                #endif

                if (dep.value.GetType() == nArg::Void)
                {
                    // --- setscenedependency ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSSD');
                    cmd->In()->SetI(dep.level);
                    cmd->In()->SetS(dep.refTargetNode.getname());
                    cmd->In()->SetS(dep.fileName.Get());
                    cmd->In()->SetS(dep.command.Get());
                    cmd->In()->SetS(dep.serverPath.Get());
                    ps->PutCmd(cmd);
                }
                else if (dep.value.GetType() == nArg::Int)
                {
                    // --- setsceneintoverride ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSOI');
                    cmd->In()->SetI(dep.level);
                    cmd->In()->SetS(dep.refTargetNode.getname());
                    cmd->In()->SetS(dep.command.Get());
                    cmd->In()->SetI(dep.value.GetI());
                    ps->PutCmd(cmd);
                }
            }
        }

        return true;
    }
    return false;
}

#ifdef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetAssetEditable(const bool /*value*/)
{
    // empty
}

#else

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetLevelAsset(nString& /*asset0*/, nString& /*asset1*/, nString& /*asset2*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::GetLevelAsset(nString& asset0, nString& asset1, nString& asset2)
{
    if (this->GetNumLevels() > 0) asset0.Set(this->GetLevelResource(0, 0));
    if (this->GetNumLevels() > 1) asset1.Set(this->GetLevelResource(1, 0));
    if (this->GetNumLevels() > 2) asset2.Set(this->GetLevelResource(2, 0));
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetRagdollAsset(nString& /*asset0*/, nString& /*asset1*/, nString& /*asset2*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::GetRagdollAsset(nString& asset0, nString& asset1, nString& asset2)
{
    int numRagdollLevels = this->ragdollResources.Size();
    if (numRagdollLevels > 0) asset0.Set(this->GetRagdollResource(0));
    if (numRagdollLevels > 1) asset1.Set(this->GetRagdollResource(1));
    if (numRagdollLevels > 2) asset2.Set(this->GetRagdollResource(2));
}

//------------------------------------------------------------------------------
/**
*/
void
ncAssetClass::SetAssetEditable(const bool value)
{
    if (value && !this->GetAssetEditable())
    {
        // Set dependency, if change the scenenode then class is dirty
        ncSceneClass *sceneComp = this->entityClass->GetComponent<ncSceneClass>();
        if (sceneComp)
        {
            nRoot* sceneRes = sceneComp->GetRootNode();
            if (sceneRes)
            {
                 sceneRes->BindDirtyDependence( this->GetEntityClass() );
            }   
        }
    }

    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass)
    {
        editorClass->SetClassKeyInt("AssetEditable", value ? 1 : 0);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncAssetClass::GetAssetEditable() const
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("AssetEditable"))
    {
        return editorClass->GetClassKeyInt("AssetEditable") == 1;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    helper script methods for outgui inspectors
*/
void
ncAssetClass::SetInstanceEnabled(int level, bool value)
{
    ncSceneLodClass* sceneLodClass = this->GetComponent<ncSceneLodClass>();
    if (sceneLodClass && this->GetNumLevels() > 0)
    {
        if (level < this->GetNumLevels())
        {
            sceneLodClass->SetLevelInstanced(level, value);
        }
    }
    else
    {
        ncSceneClass* sceneClass = this->GetComponent<ncSceneClass>();
        if (sceneClass && level == 0)
        {
            sceneClass->SetInstanceEnabled(value);
        }
    }
}

bool
ncAssetClass::GetInstanceEnabled(int level)
{
    ncSceneLodClass* sceneLodClass = this->GetComponent<ncSceneLodClass>();
    if (sceneLodClass && this->GetNumLevels() > 0)
    {
        if (level < this->GetNumLevels())
        {
            return sceneLodClass->GetLevelInstanced(level);
        }
    }
    else
    {
        ncSceneClass* sceneClass = this->GetComponent<ncSceneClass>();
        if (sceneClass)
        {
            return sceneClass->GetInstanceEnabled();
        }
    }
    return false;
}

void
ncAssetClass::SetAssetInstanced(bool enabled0, bool enabled1, bool enabled2)
{
    this->SetInstanceEnabled(0, enabled0);
    this->SetInstanceEnabled(1, enabled1);
    this->SetInstanceEnabled(2, enabled2);

    this->Invalidate();
}

void
ncAssetClass::GetAssetInstanced(bool& enabled0, bool& enabled1, bool& enabled2)
{
    enabled0 = this->GetInstanceEnabled(0);
    enabled1 = this->GetInstanceEnabled(1);
    enabled2 = this->GetInstanceEnabled(2);
}

void
ncAssetClass::SetAssetInstanceFrequency(int /*frequency0*/, int /*frequency1*/, int /*frequency2*/)
{
    /// @todo ma.garcias - specify the number of instances by level
    //this->SetInstanceFrequency(0, frequency0);
    //this->SetInstanceFrequency(1, frequency1);
    //this->SetInstanceFrequency(2, frequency2);

    //this->Invalidate();
}

void
ncAssetClass::GetAssetInstanceFrequency(int& /*frequency0*/, int& /*frequency1*/, int& /*frequency2*/)
{
    //frequency0 = this->GetInstanceFrequency(0);
    //frequency1 = this->GetInstanceFrequency(1);
    //frequency2 = this->GetInstanceFrequency(2);
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetBoolOptionVariable(const char* key, bool value)
{
    ncEditorClass* editorComp = this->GetComponent<ncEditorClass>();
    if (editorComp)
    {
        editorComp->SetClassKeyInt(key, value ? 1 : 0);
    }

    //TEMP- we need to keep this here until a game working copy is assembled
    //with the assets already assembled
    ncDictionaryClass* varContext = this->GetComponent<ncDictionaryClass>();
    if (varContext)
    {
        varContext->SetBoolVariable(key, value);
    }

    this->Invalidate();
}

bool
ncAssetClass::GetBoolOptionVariable(const char* key)
{
    ncEditorClass* editorComp = this->GetComponent<ncEditorClass>();
    if (editorComp && editorComp->IsSetClassKey(key))
    {
        return (editorComp->GetClassKeyInt(key) == 1);
    }
    return false;
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetIntOptionVariable(const char* key, int value)
{
    ncEditorClass* editorComp = this->GetComponent<ncEditorClass>();
    if (editorComp)
    {
        editorComp->SetClassKeyInt(key, value);
    }

    //TEMP- we need to keep this here until a game working copy is assembled
    //with the assets already assembled
    ncDictionaryClass* varContext = this->GetComponent<ncDictionaryClass>();
    if (varContext)
    {
        varContext->SetIntVariable(key, value);
    }

    this->Invalidate();
}

int
ncAssetClass::GetIntOptionVariable(const char* key)
{
    ncEditorClass* editorComp = this->GetComponent<ncEditorClass>();
    if (editorComp && editorComp->IsSetClassKey(key))
    {
        return (editorComp->GetClassKeyInt(key));
    }
    return -1;
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetBatchDepthShapes(bool value)
{
    this->SetBoolOptionVariable("batchDepthShapes", value);
}

bool
ncAssetClass::GetBatchDepthShapes()
{
    return this->GetBoolOptionVariable("batchDepthShapes");
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetBatchShapesByMaterial(bool value)
{
    this->SetBoolOptionVariable("batchShapesByMaterial", value);
}

bool
ncAssetClass::GetBatchShapesByMaterial()
{
    return this->GetBoolOptionVariable("batchShapesByMaterial");
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetBatchDepthShapesByCell(bool value)
{
    this->SetBoolOptionVariable("batchDepthShapesByCell", value);
}

bool
ncAssetClass::GetBatchDepthShapesByCell()
{
    return this->GetBoolOptionVariable("batchDepthShapesByCell");
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetShadowCasterLevel(int level)
{
    this->SetIntOptionVariable("ShadowCasterLevel", level);
}

int
ncAssetClass::GetShadowCasterLevel()
{
    return this->GetIntOptionVariable("ShadowCasterLevel");
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetDisableShadowLevel(int level)
{
    this->SetIntOptionVariable("DisableShadowLevel", level);
}

int
ncAssetClass::GetDisableShadowLevel()
{
    return this->GetIntOptionVariable("DisableShadowLevel");
}

//------------------------------------------------------------------------------
void
ncAssetClass::SetMaxMaterialLevel(int lod, int level)
{
    ncSceneLodClass* sceneLodClass = this->GetComponent<ncSceneLodClass>();
    if (sceneLodClass && this->GetNumLevels() > 0)
    {
        if (lod < this->GetNumLevels())
        {
            sceneLodClass->SetLevelMaterialProfile(lod, level);
        }
    }
    else
    {
        ncSceneClass* sceneClass = this->GetComponent<ncSceneClass>();
        if (sceneClass && lod == 0)
        {
            sceneClass->SetMaxMaterialLevel(level);
        }
    }
}

int
ncAssetClass::GetMaxMaterialLevel(int lod)
{
    ncSceneLodClass* sceneLodClass = this->GetComponent<ncSceneLodClass>();
    if (sceneLodClass && this->GetNumLevels() > 0)
    {
        if (lod < this->GetNumLevels())
        {
            return sceneLodClass->GetLevelMaterialProfile(lod);
        }
    }
    else
    {
        ncSceneClass* sceneClass = this->GetComponent<ncSceneClass>();
        if (sceneClass)
        {
            return sceneClass->GetMaxMaterialLevel();
        }
    }
    return false;
}

void
ncAssetClass::SetMaterialProfile(int level0, int level1, int level2)
{
    this->SetMaxMaterialLevel(0, level0);
    this->SetMaxMaterialLevel(1, level1);
    this->SetMaxMaterialLevel(2, level2);
}

void
ncAssetClass::GetMaterialProfile(int& level0, int& level1, int& level2)
{
    level0 = this->GetMaxMaterialLevel(0);
    level1 = this->GetMaxMaterialLevel(1);
    level2 = this->GetMaxMaterialLevel(2);
}

//------------------------------------------------------------------------------
nArray<nString> ncAssetClass::surfaceArray;

void
ncAssetClass::CountMaterials(nSceneNode* sceneNode)
{
    if (sceneNode->IsA("ngeometrynode"))
    {
        const char* surface = static_cast<nGeometryNode*>(sceneNode)->GetSurface();
        if (!surfaceArray.Find(surface))
        {
            surfaceArray.Append(surface);
        }
    }

    nSceneNode* curChild = static_cast<nSceneNode*>(sceneNode->GetHead());
    while (curChild)
    {
        CountMaterials(curChild);
        curChild = static_cast<nSceneNode*>(curChild->GetSucc());
    }
}

int
ncAssetClass::GetNumMaterials(nSceneNode* sceneNode)
{
    surfaceArray.Reset();

    // build a string list of different materials in the level
    CountMaterials(sceneNode);

    // using the list of collected surfaces built a user library
    // useful to select materials comprehensively
    nString materialName;
    for (int index = 0; index < surfaceArray.Size(); ++index)
    {
        nSurfaceNode* surface = static_cast<nSurfaceNode*>(nKernelServer::ks->Lookup(surfaceArray[index].Get()));
        if (surface && surface->GetTexture(nShaderState::diffMap))
        {
            nString textureName(surface->GetTexture(nShaderState::diffMap));
            textureName.StripExtension();
            materialName.Format("/usr/materials/%s.%s", this->GetEntityClass()->GetName(), textureName.ExtractFileName().Get());
            if (!nKernelServer::ks->Lookup(materialName.Get()))
            {
                nEnv* env = static_cast<nEnv*>(nKernelServer::ks->New("nenv", materialName.Get()));
                env->SetS(surface->GetTexture(nShaderState::diffMap));
            }
        }
    }

    // return number of materials in the level
    return surfaceArray.Size();
}

void
ncAssetClass::SetNumMaterialsByLevel(int /*num0*/, int /*num1*/, int /*num2*/)
{
    // empty: this is a read-only property
}

void
ncAssetClass::GetNumMaterialsByLevel(int& num0, int& num1, int& num2)
{
    if (this->GetNumLevels() > 0) num0 = this->levelResources[0].numMaterials;
    if (this->GetNumLevels() > 1) num1 = this->levelResources[1].numMaterials;
    if (this->GetNumLevels() > 2) num2 = this->levelResources[2].numMaterials;
}

#endif //NGAME
