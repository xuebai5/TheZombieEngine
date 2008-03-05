#ifndef N_IMPOSTORBUILDER_H
#define N_IMPOSTORBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nImpostorBuilder
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Builds the scene resource and texture(s) required to render
    imposted geometry. It exposes a builder interface for: 
    * creating the impostor resource for an individual graphic asset
    * creating a texture atlas from these other resources

    (C) 2005 Conjurer Services, S.A.
*/

class nSceneNode;
class nSceneGraph;
//------------------------------------------------------------------------------
class nImpostorBuilder
{
public:
    /// constructor
    nImpostorBuilder();
    /// destructor
    ~nImpostorBuilder();

    /// set source class for the impostor
    void SetClass(const char *className);
    /// build impostor resource for the current class
    void BuildImpostorAsset();

    /// get the path to the generated impostor asset
    const char* GetImpostorAsset();

private:
    /// build texture set for the impostor asset
    void BuildImpostorTextures();
    /// build scene for the impostor asset
    nSceneNode* BuildImpostorScene();
    /// save render target to texture file
    bool SaveRenderTarget(nTexture2* texture, const char *filename);
    /// copy texture to save
    nTexture2* CopyTextureFrom(nTexture2*);

    nString className;
    nString impostorAssetName;
    nString impostorTexturePath;
    nString impostorNormalPath;
    int sourceLevel;
    bbox3 impostorBBox;

    static int uniqueSurfaceId;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nImpostorBuilder::SetClass(const char *className)
{
    this->className.Set(className);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nImpostorBuilder::GetImpostorAsset()
{
    return impostorAssetName.IsEmpty() ? impostorAssetName.Get() : 0;
}

//------------------------------------------------------------------------------
#endif /*N_IMPOSTORBUILDER_H*/
