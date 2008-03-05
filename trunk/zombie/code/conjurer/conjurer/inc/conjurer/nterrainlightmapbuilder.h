#ifndef N_TERRAINLIGHTMAPBUILDER_H
#define N_TERRAINLIGHTMAPBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nTerrainLightmapBuilder
    @ingroup NebulaConjurerEditor

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
class nTerrainMaterial;
class nSceneGraph;
class ncTerrainGMMCell;
class nLightEnvNode;

extern const char * whiteTexturePath;
//------------------------------------------------------------------------------
class nTerrainLightMapBuilder
{
public:
    // constructor
    nTerrainLightMapBuilder();
    // constructor
    nTerrainLightMapBuilder(const char* lightClassName, int lightMapSize, int shadowMapSize,int globalLightMapsize, nEntityObjectId lightId, const float maxDistance , const vector2 & offset, const float diskSize);
    // destructor
    virtual ~nTerrainLightMapBuilder();

    /// set the range of terrain blocks for which to generate the terrain
    void SetSelectedCells(const nArray<nRefEntityObject> entities);
    /// set if existing files must be overwritten
    void SetOverwriteFiles(bool overwriteFiles);

    /// create light enviroment class
    bool CreateLightEnvironmentClass();
    /// creaet light object and add it to the current level
    void CreateLightEnvironmentObject();
    /// Generate the global Texture
    bool GenerateGlobalLightMap(const char* lightClassName, int globalLightMapsize );

private:

    /// destroy outlight object
    void DestroyLightObject();

    /// create lightmaps
    bool CreateTerrainLightMaps();

    /// generate global lightmap texture
    bool GenerateGlobalLightMapTexture();
    /// generate global shadowmap texture
    void GenerateGlobalShadowMapTexture();
    /// generate mini textures
    void GenerateMiniTextures();
    /// generate mini shadowmap textures
    void GenerateMiniShadowMapTextures();

    /// set shadow data
    void SceneShadowMapData(nEntityObject* currentCell, nString& texturefileName, const vector4& orthogonal, const vector3& euler, const vector3& position, const vector4& deformation);

    /// get shadow texture name
    nString GetShadowTextureName(int bx, int bz);
    /// get lightmap texture name
    nString GetLightmapTextureName(int bx, int bz);

    /// attach current cell entities
    void AttachCurrentCellEntities(nSceneGraph* sceneGraph, nEntityObject* currentCell, bool isShadowMap);
    /// attach an array of entity objects
    void AttachEntityArray(nSceneGraph* sceneGraph, const nArray<nEntityObject*>& entities);
    /// attach an array of entity references
    void AttachEntityArray(nSceneGraph* sceneGraph, const nArray<nRef<nEntityObject> >& entities);
    /// attach static shadow casting entity
    void AttachShadowEntity(nSceneGraph* sceneGraph, nEntityObject* entity);
    /// attach light entity
    void AttachLightEntity(nSceneGraph* sceneGraph);
    /// attach viewport entity
    void AttachViewport(nSceneGraph* sceneGraph);

    /// compute texture resolution
    float ComputeLightMapResolution();
    /// save light map texture
    bool SaveLightMapTexture(int bx, int bz);
    /// save shadow map texture
    bool SaveShadowMapTexture(int bx, int bz);
    /// copy texture from to
    nTexture2* CopyTexture( nRef<nTexture2> from );
    /// save global textures
    void SaveGlobalShadowMapTexture();
    /// convert lightmap class name to valid class name
    bool ConvertLightNameToValidClassName();
    /// Get the best orientation, return euler
    vector3 GetBestOrientation();
   /// Get the best DEformation, matrix2x2
    vector4 GetBestDeformation(const vector3& lightEuler);
    /// gett cells for genreate lightmaps of shadowmap
    void GetCells(nArray<nEntityObject*>& terrainBlocks);
    ///generate mini shadowmap textures for a cell
    bool GenerateMiniShadowMapTexture(nEntityObject* cell, nSceneGraph* sceneGraph, const vector3& lightEuler, const vector4& deformation);
    /// Return the best postion and orthogonal
    void GetPositionAndOrthogonal(ncTerrainGMMCell* currentCell,vector3& position, vector4& orthogonal, const vector3& lightEuler, const vector4& deformation);
    /// Create new surface and put it to terrain, return the previous surface
    nString TerrainSetNewSurface( nFourCC passFourCC , const char* shaderFileName);
    /// Restore the surface to terrain
    void TerrainRestoreSurface(const nString&);
    /// Return the BBOX in camera space of terrain's projection
    bbox3 GetBBoxOfTerrainTransformed(ncTerrainGMMCell* currentCell, const matrix44& toCameraSpace) const;

    /// lightmap class
    nEntityClass* lightMapClass;
    nString lightMapClassName;

    /// lightmap object
    nEntityObject* lightMapObject;
    nEntityObjectId lightId;

    /// range of terrain cells
    nArray<nEntityObject*> selectedCells;

    /// texture paths
    nString lightTexturePath;
    nString shadowTexturePath;

    /// scene root
    nLightEnvNode* libSceneRoot;

    /// neoutdoor
    nRef<nEntityObject> refOutDoorObject; //only ncTerrainGMM is needed
    nRef<nEntityObject> refLightObject;
    nRef<nEntityObject> refViewportObject;
    nRef<nEntityObject> refCameraObject;

    /// render target
    nRef<nTexture2> refRenderTarget;
    nRef<nTexture2> refShadowRenderTarget;

    nArray<nTexture2*> shadowMapTextures;

    /// overwrite existing files when generating textures
    bool overwriteExistingFiles;
    /// texture size
    int lightMapSize;
    /// the size of global lightmap
    int globalLightMapSize;
    int  shadowMapSize;
    float maxDistance;
    // The terrain offset
    vector2 offset;
    // The disk size for shadow projection
    float antiAliasingDiskSize;

};

//------------------------------------------------------------------------------
#endif
