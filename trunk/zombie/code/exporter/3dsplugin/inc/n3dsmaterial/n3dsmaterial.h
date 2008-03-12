#ifndef N_3DS_MATERIAL_H
#define N_3DS_MATERIAL_H
#include "util/narray.h"
#include "n3dsmaterial/nabstractpropbuilder.h"
#include "n3dsanimator/ntexanimatorbuilder.h"

#define ZOMBIE_MTL_CLASS_ID Class_ID(0x126313e1, 0x5648549f)

//------------------------------------------------------------------------------
/**
    @class n3dsMaterial
    @ingroup n3dsMaxMaterial
    @brief interface for 3ds max material to nebula material
*/
class n3dsMaterial
{
public:
    n3dsMaterial();
    /// Initialize interface with a 3ds material
    void SetMaterial(Mtl* mat);
    //nArray<nMaterialNode* > listNodes;
    Mtl* mtl;
    /// return if is a billboard
    bool IsBillBoard() const;
    /// return if a compute spherical normals
    bool IsSphereNormal() const;
    /// return has a lightmap
    bool HasLightMap() const;
    /// return has bumpmap
    bool NeedTangentUV0() const;
    /// return has normapMap
    bool NeedTangentUV3() const;
    /// return the material's name
    const char* GetName() const;
    /// Set the unique name
    void SetUniqueName(const nString unique);
    /// Get the unique Name
    const nString& GetUniqueName();
    /// return if material exporter is valid
    bool IsValid() const;
    /// return if material exporter has as invalid shader type
    bool IsInValidShaderType() const;
    /// return igf the objetct with this material emitt shadows
    bool IsEmitterShadow() const;

    /// Return the material properties for shape, example lightmap
    nAbstractPropBuilder& GetMatShape();
    /// Return the material properties
    nAbstractPropBuilder& GetMatSurface();
    /// return components needs for material
    int GetMeshComponents() const;
    /// prepare data and export
    void PrepareAndExport();


private:
    struct nTexAnim : public nTexAnimatorBuilder
    {
        // If the texuture is for shape or is for surface
        bool isToShape;
    };


    nString nameMaterial;
    nString uniqueName;

    bool     notExport;
    bool     DiffMap;
    bool     ClipMap;
    bool     IlumMap;
    bool     LightMap;
    bool     hasSpec; //has sepeular
    bool     SpecMap; //Specular map
    bool     bumpMapIsNormalMap;
    bool     shadowEmitter;
    ///bool     BumpMapAnim;
    //nTexAnimatorBuilder bumpAnimator;


    nString  texDiffuse;
    nString  texSpecular;
    nString  texClip;
    nString  texIlum;
    nString  texLight;

    void GetNameProperties();
    vector4 matAmbient;
    vector4 matDiffuse;
    vector4 matSpecular;
    float   SpecularLevel;
    float   SpecularPow;
    void GetMap(int map,bool &valid,nString &path);
    bool propBillBoard;
    bool propSphereNormal;
    bool propSwing;
    bool propFogDisable;
    int meshComponents;
    nAbstractPropBuilder propMatSurface;
    nAbstractPropBuilder propMatShape;
    /// list animators
    nArray<nTexAnim> animators;
    /// if the material is checked 
    bool isPrepared;

    /// collect info from zombie material
    void ExportZombieMaterial(nArray<nTexAnim>& animators);
    /// colect info froms zombie texture
    void ExportZombieTexture(Texmap *map,  nAbstractPropBuilder* surface, nString &name, nArray<nTexAnim>& animators);
    /// Create a material for standard 3ds material
    void CreateNode();
    /// delete unnecesary variables and animators
    bool ReduceAndCheck(nArray<nTexAnim>& animators);
    /// copy all textures
    bool CollectTextures(nArray<nTexAnim>& animators);
    /// Create texture animators
    void CreateAnimators(nArray<nTexAnim>& animators);
    /// set the error material
    void SetErrorMaterial();
    /// apply conversion from change the material shader
    void ConvertMatType();
    /// put neuter values for param
    bool PutDefaultValuesForParam(int index, nMatTypePropBuilder* matLibrary );
              
};

inline
nAbstractPropBuilder&
n3dsMaterial::GetMatShape()
{
    return this->propMatShape;

}
inline
nAbstractPropBuilder&
n3dsMaterial::GetMatSurface()
{
    return this->propMatSurface;

}

inline
bool 
n3dsMaterial::NeedTangentUV0() const
{
    n_assert(this->isPrepared);
    if ( (propMatSurface.GetAbstractType() == nAbstractPropBuilder::NMATERIAL) ||
         (propMatSurface.GetAbstractType() == nAbstractPropBuilder::SHADERTYPELIBRARY)
        )
    {
        return propMatSurface.GetMatType().HasParam("map","bumpmap") ||
               propMatSurface.GetMatType().HasParam("map","parallax");
    } else
    { 
        return false;
    } 
}

inline
bool 
n3dsMaterial::NeedTangentUV3() const
{
    n_assert(this->isPrepared);
    if ( (propMatSurface.GetAbstractType()== nAbstractPropBuilder::NMATERIAL) ||
         (propMatSurface.GetAbstractType()== nAbstractPropBuilder::SHADERTYPELIBRARY)
        )
    {
        return propMatSurface.GetMatType().HasParam("map","normalmap");
    } else
    {
        return false;
    }
}
#endif

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------