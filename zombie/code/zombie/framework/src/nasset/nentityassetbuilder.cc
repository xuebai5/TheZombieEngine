#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nentityassetbuilder.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"
#include "entity/nentityclass.h"
#include "nscene/nscenenode.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndependencyserver.h"
#include "zombieentity/ncloaderclass.h"

namespace nEntityAssetBuilder
{
    nString GetSceneFile( const nEntityClass* entityClass)
    {
        return GetSceneFileForAsset( GetAssetPath( entityClass ) );
    }

    nString GetSceneFileForAsset(const nString& assetPath )
    {
        return ( assetPath + "/scene/") + assetPath.ExtractFileName() + ".n2" ;
    }

    nString GetScenePath( const nEntityClass* entityClass)
    {
        return GetScenePathForAsset( GetAssetPath( entityClass ) );
    }

    nString GetScenePathForAsset( const nString& assetPath )
    {
        return ( assetPath + "/scene/");
    }

    nString GetSceneNOH( const nEntityClass* entityClass)
    {
       nString tmp("/lib/scene/");
       return tmp+GetAssetName(entityClass);
    }

    nString GetSceneNOHForAsset(const nString& assetPath)
    {
        return "/lib/scene/" + assetPath.ExtractFileName();
    }


    nString GetGrowthTerrrainMaterialNOH()
    {
        return nString("/lib/terrainGrowthMaterials");
    }

    nString GetGrowthTerrrainMaterialNOH(const nEntityClass* entityClass)
    {
        return GetGrowthTerrrainMaterialNOH() + "/" + entityClass->GetName();
    }


    nSceneNode* GetSceneRoot(const nEntityClass* entityClass )
    {
        return static_cast<nSceneNode*>(nKernelServer::ks->Lookup( GetSceneNOH( entityClass).Get() ));
    }

    nSceneNode* GetSceneRootForAsset(const nString& assetPath)
    {
        return static_cast<nSceneNode*>(nKernelServer::ks->Lookup( GetSceneNOHForAsset( assetPath).Get() ));
    }

    void SaveSceneRoot( const nEntityClass* entityClass )
    {
        SaveSceneRootForAsset( GetAssetPath ( entityClass ) );
    }
    
    void SaveSceneRootForAsset(const nString& assetPath )
    {
        nString fileName =  nEntityAssetBuilder::GetScenePathForAsset( assetPath );
        nFileServer2::Instance()->MakePath( fileName.Get() );
        fileName = nEntityAssetBuilder::GetSceneFileForAsset(  assetPath );

        nSceneNode* sceneNode = nEntityAssetBuilder::GetSceneRootForAsset( assetPath );
        n_assert2(sceneNode, "the entity has not a scene" );

        nDependencyServer::InitGlobal();
        sceneNode->SaveAs( fileName.Get() );
        nDependencyServer::InitGlobal();
    }

    nString GetMeshesPathForAsset( const nString& assetPath)
    {
        return assetPath + "/meshes/";

    }

    nString GetMeshesPath( const nEntityClass* entityClass)
    {
        return  GetMeshesPathForAsset( GetAssetPath( entityClass ) );

    }

    nString GetTerrainPath( const nEntityClass* entityClass, bool isOnlyForConjurer )
    {
        return  GetTerrainPathForAsset( GetAssetPath( entityClass ), isOnlyForConjurer );
    }

    nString GetTerrainPathForAsset( const nString& assetPath, bool isOnlyForConjurer )
    {
        if (isOnlyForConjurer)
        {
            return assetPath + "/debug/terrain/";
        } else
        {
            return assetPath + "/terrain/";
        }
    }

    nString GetMaterialPathForAsset( const nString& assetPath)
    {
        return assetPath + "/meshes/";

    }

    nString GetMaterialPath( const nEntityClass* entityClass)
    {
        return  GetMeshesPathForAsset( GetAssetPath( entityClass ) );

    }

    nString GetAnimsPath( const nEntityClass* entityClass)
    {
        return GetAssetPath( entityClass ) + "/anims/";
    }

    nString GetAssetName(const nEntityClass* entityClass)
    {
        nString className( GetAssetPath( entityClass ) );
        return className.ExtractFileName();
    }

    nString GetAssetPath( const nEntityClass* entityClass)
    {
        const ncLoaderClass * loader = entityClass->GetComponent<ncLoaderClass>();
        n_assert(loader);
        nString path( loader->GetResourceFile() );
        path.StripTrailingSlash();
        return path;
    }

    nString GetDefaultAssetPath(const nString& className)
    {
        return "wc:export/assets/" + className;
    }

    nString GetDefaultAssetPathForLOD(const nString& className, int level)
    {
        // the lower 'a' is for assets
        return "wc:export/assets/a" + className + "_lod" + nString( level ); 
    }

    nString GetDefaultAssetPathForLOD(const nEntityClass* entityClass, int level)
    {
        // the lower 'a' is for assets
        return GetDefaultAssetPathForLOD( entityClass->nClass::GetName() , level); 
    }

    nString GetDefaultAssetPathForRagdoll(const nString& className, int level)
    {
        // the lower 'a' is for assets
        return "wc:export/assets/a" + className + "_ragdoll" + nString( level ); 
    }

    nString GetDefaultAssetPathForRagdoll(const nEntityClass* entityClass, int level)
    {
        // the lower 'a' is for assets
        return GetDefaultAssetPathForRagdoll( entityClass->nClass::GetName() , level); 
    }

    nString GetIndoorBrushClassName(const nEntityClass* indoorClass)
    {
        nString tmp = indoorClass->nClass::GetName();
        return tmp + "_indoor_brush";
    }

    nString GetPortalClassName(const nEntityClass* indoorClass)
    {
        nString tmp = indoorClass->nClass::GetName();
        return tmp + "_indoor_portal";
    }

    nString GetIndoorFacadeClassName(const nEntityClass* indoorClass)
    {
        nString tmp = indoorClass->nClass::GetName();
        return tmp + "_indoor_facade";
    }

    nString GetIndoorFacadeClassName(const nString& className)
    {
        return className + "_indoor_facade";
    }

    bool MakeMeshPath( const nEntityClass* entityClass)
    {
        return MakeMeshPathForAsset( GetAssetPath (entityClass) );
    }

    bool MakeMeshPathForAsset(const nString& assetPath)
    {
        nString meshName = GetMeshesPathForAsset( assetPath );
        return nFileServer2::Instance()->MakePath( meshName.Get() );
    }

    bool MakeMaterialPath( const nEntityClass* entityClass)
    {
        return MakeMeshPathForAsset( GetAssetPath (entityClass) );
    }

    bool MakeMaterialPathForAsset(const nString& assetPath)
    {
        nString materialName = GetMaterialPathForAsset( assetPath );
        return nFileServer2::Instance()->MakePath( materialName.Get() );
    }

    bool MakeTerrainPath(const nEntityClass* entityClass ,bool isOnlyForConjurer)
    {
        return MakeTerrainPathForAsset( GetAssetPath (entityClass) , isOnlyForConjurer );
    }

    bool MakeTerrainPathForAsset(const nString& assetPath,bool isOnlyForConjurer)
    {
        nString path = GetTerrainPathForAsset( assetPath , isOnlyForConjurer);
        return nFileServer2::Instance()->MakePath( path );
    }
};