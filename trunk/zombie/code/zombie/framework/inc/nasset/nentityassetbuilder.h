#ifndef N_ENTITYASSETBUILDER_H
#define N_ENTITYASSETBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nEntityAsset
    @ingroup NebulaAssetSystem
    @author Cristobal Castillo

    @brief This class exposes access to the internals an entity asset
    including saving assets and retrieving paths for saving specific
    entity class resources.

    The base for all paths and namespaces is the entity class. The path
    for storing component resources is determined from the base class.
    NOH resources and dependencies are automatically persisted when saving
    the asset, whereas external resources must be stored by the client
    application (eg. exporter tool).

    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nref.h"

class nSceneNode;
class nEntityClass;

namespace nEntityAssetBuilder
{
    /// Get the scene File, full path
    nString GetSceneFile( const nEntityClass* entityClass);
    /// Get the scene File, full path
    nString GetSceneFileForAsset(const nString& assetPath );
    //  Get the path of scene file
    nString GetScenePath( const nEntityClass* entityClass);
    //  Get the path of scene file
    nString GetScenePathForAsset( const nString& assetPath );
    /// Get the Noh path for scene
    nString GetSceneNOH( const nEntityClass* entityClass);
    /// Get the Noh path for scene
    nString GetSceneNOHForAsset(const nString& assetPath);
    /// return the nSceneNode of entity class
    nSceneNode* GetSceneRoot(const nEntityClass* entityClass );
    /// return the nSceneNode for this asset
    nSceneNode* GetSceneRootForAsset(const nString& assetPath);
    /// Save scene of this entity
    void SaveSceneRoot( const nEntityClass* entityClass );
    /// Save scene for this asset
    void SaveSceneRootForAsset(const nString& assetPath );
    /// retutn the path root of growth material
    nString GetGrowthTerrrainMaterialNOH();
    /// return the Growth terrain material NOH
    nString GetGrowthTerrrainMaterialNOH(const nEntityClass* entityClass);

    nString GetMeshesPath( const nEntityClass* entityClass);
    nString GetMeshesPathForAsset( const nString& assetPath);
    nString GetMaterialPath( const nEntityClass* entityClass);
    nString GetMaterialPathForAsset( const nString& assetPath);
    nString GetTerrainPath( const nEntityClass* entityClass, bool isOnlyForConjurer = false);
    nString GetTerrainPathForAsset( const nString& assetPath, bool isOnlyForConjurer = false);
    nString GetAnimsPath( const nEntityClass* entityClass);
    nString GetAssetName( const nEntityClass* entityClass);
    nString GetAssetPath( const nEntityClass* entityClass);

    /// return the class name of neIndoorBrushClass for this neIndoorClass
    nString GetIndoorBrushClassName(const nEntityClass* indoorClass);
    /// return the class name of nePortalClass for this neIndoorClass
    nString GetPortalClassName(const nEntityClass* indoorClass);
    /// return the class name of wrapper for this neIndoorClass
    nString GetIndoorFacadeClassName(const nEntityClass* indoorClass);
    /// return the class name of wrapper for this neIndoorClass
    nString GetIndoorFacadeClassName(const nString& className);
    
    /// return the Default asset path for a class
    nString GetDefaultAssetPath(const nString& className);
    /// return the Default asset path for a lod 
    nString GetDefaultAssetPathForLOD(const nString& className, int level);
    /// return the Default asset path for a lod
    nString GetDefaultAssetPathForLOD(const nEntityClass* entityClass, int level);
    /// return the Default asset path for a ragdoll
    nString GetDefaultAssetPathForRagdoll(const nString& className, int level);
    /// return the Default asset path for a ragdoll
    nString GetDefaultAssetPathForRagdoll(const nEntityClass* entityClass, int level);

    /// Create a path of mesh
    bool MakeMeshPath(const nEntityClass* entityClass);
    /// Create a path for asset
    bool MakeMeshPathForAsset(const nString& assetPath);

    /// Create a path of mesh
    bool MakeMaterialPath(const nEntityClass* entityClass);
    /// Create a path for asset
    bool MakeMaterialPathForAsset(const nString& assetPath);
    /// Create path for terrain growtrh materials

    /// Create the terrain editor path 
    bool MakeTerrainPath(const nEntityClass* entityClass ,bool isOnlyForConjurer = false);
    /// Create the terrain editor path 
    bool MakeTerrainPathForAsset(const nString& assetPath, bool isOnlyForConjurer = false);
    
};

//------------------------------------------------------------------------------
/**
*/
#endif