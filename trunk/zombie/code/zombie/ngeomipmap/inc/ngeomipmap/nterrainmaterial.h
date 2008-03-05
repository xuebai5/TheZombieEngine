#ifndef N_NTERRAINMATERIAL_H
#define N_NTERRAINMATERIAL_H
//------------------------------------------------------------------------------
/**
    @file nterrainmaterial.h
    @class nTerrainMaterial
    @ingroup NebulaTerrain

    @author Juan Jose Luna Espinosa

    @brief Container class for edit-time terrain surface material

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/ncmdprotonativecpp.h"
#include "util/nstring.h"
#include "ngeomipmap/nterrainmaterial.h"
#include "gameplay/ngamematerial.h"

//------------------------------------------------------------------------------
class nPersistServer;
class nFloatMap;
class nTexture2;

//------------------------------------------------------------------------------
class nTerrainMaterial : public nObject
{
public:

    // handle to the layer
    typedef int LayerHandle;

    // invalid handle
    #define InvalidLayerHandle -1
    #define InvalidLayerIndex -1

    // type of projection used in the terrain
    enum ProjectionType
    {
        ProjXZ,
        ProjXY,
        ProjZY
    };

    // constructor
    nTerrainMaterial();

    /// Nebula persistency
    virtual bool SaveCmds(nPersistServer * ps);

    /// load resources
    bool LoadResources();
    /// unload resources
    void UnloadResources(void);
    /// save resources
    bool SaveResources(void);

    /// set the texture to use for this terrain layer
    void SetTextureFileName(const char *);
    /// get texture path for this layer
    const nString & GetTextureFileName ();
    /// set projection type
    void SetProjection(int);
    /// get projection type
    int GetProjection ();
    /// set UV scaling
    void SetUVScale(vector2);
    /// get UV scaling
    vector2 GetUVScale ();
    /// get layer handle
    LayerHandle GetLayerHandle ();
    /// Set layer handle
    void SetLayerHandle(int);
    /// Set game material by name
    void SetGameMaterialByName(const char *);
    /// Get game material name
    const char *  GetGameMaterialName();
    /// Get game material id
    nGameMaterial * GetGameMaterial();

    nString GetTextureThumbnail(int);
    void SetMaskColor(vector4);
    vector4 GetMaskColor ();
    void SetLabel(nString);
    nString GetLabel ();

    // Obsolete methods
    void SetLayerPath(const nString &);
    const nString & GetLayerPath() const;
    void SetLayerUsePath(const nString &);
    const nString & GetLayerUsePath() const;

    /// Get texture pointer
    nTexture2* GetTexture();

    // set the terrain cell material transform U parameter
    void GetTerrainCellTransformU(const vector4 & cellpos, vector4 & transformU) const;
    // set the terrain cell material transform V parameter
    void GetTerrainCellTransformV(const vector4 & cellpos, vector4 & transformV) const;

    // importing material
    void SetImportMaterialState(bool state);

protected:

    // destructor
    virtual ~nTerrainMaterial();

    /// Layer handle
    int layerHnd;

    /// Texture file path
    nString textureFileName;

    /// Projection type
    ProjectionType projectionType;

    /// UV scale
    vector2 scaleUV;

    /// Label
    nString label;

    /// Color for editor purposes
    vector4 maskColor;

    /// Game material name
    nString gameMaterialName;
    nRef<nGameMaterial> gameMaterial;

     // Texture reference
    nRef<nTexture2> refTexture;

    // true when importing material
    bool importMaterialState;

};

//------------------------------------------------------------------------------
#endif //N_NTERRAINMATERIAL_H
