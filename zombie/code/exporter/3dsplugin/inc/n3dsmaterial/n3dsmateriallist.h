#ifndef N_3DS_MATERIAL_LIST_H
#define N_3DS_MATERIAL_LIST_H

#include "util/narray.h"
#include "n3dsmaterial/nabstractpropbuilder.h"
#include "n3dsmaterial/nmaterialbuilderserver.h"
//#include "n3dsanimator/ntexanimatorbuilder.h"
#include "n3dsmaterial/n3dsmaterial.h"
#include "util/nmaptable.h"
#include "util/nmaptabletypes.h"

class nAnimatorBuilderServer;

//------------------------------------------------------------------------------
/**
    @class n3dsMaterialList
    @ingroup n3dsMaxMaterial
    @brief list og interface for 3ds max material to nebula material
*/
class n3dsMaterialList
{
public:
    /// default constructor
    n3dsMaterialList();
    /// default destructor
    ~n3dsMaterialList();
    /// return id of n3dsMaterial
    int GetMaterialId(IGameMaterial *material);
    /// return id of sub n3dsMaterial
    int GetSubMaterialId(IGameMaterial *material,int num);
    /// return is multimaterial
    bool IsMultitype(IGameMaterial *material);
    /// return a 3ds material by id n3dsMaterial
    const n3dsMaterial &Getn3dsMaterial(int idx);
    /// Prepare material and mark one for export
    void  Preparen3dsMaterial(int idx);
    //void Save();
    /// Assign the material to shape
    void SetMaterial(nGeometryNode* shape, int idMaterial, nMatTypePropBuilder* shapeToMaterial = 0);
    /// Asign the shadow material to gemetry node
    void SetShadowMaterial(nGeometryNode* shape, nMatTypePropBuilder* shapeToMaterial = 0);
    /// return if n3dsMaterial is NSURFACE
    bool IsShaderNode(int idMaterial);
    /// Append bumpMap for create normal map
    nString AppendBumpMapTexture(const nString& fileName );
    /// append textures for collect, return new name
    nString AppendTexture( const nString& fileName );
    /// Create all normals maps from bump map
    void CreateBumpMaps();
    /// Initialize data
    void InitializedData();
    /// return the animator builder
    nAnimatorBuilderServer* GetAnimatorBuilder() const;
    /// Save
    void SaveResourceFile();
    /// Rerturn the builder of the material type
    nMatTypePropBuilder* GetMatTypePropBuilder(const char* materialTypeName);
   
protected:
    /// return the id of max material
    int  GetMaterialId3ds(Mtl *mtl);
    /// skeep the shell material , example lightmaps , normalmap generate by 3dsmax
    Mtl* SkeepShellMaterial(Mtl *mtl);
    /// Append the max material from igame to list
    bool Append( IGameMaterial* mtl , const nString& uniqueName );
    /// append the max material to list
    bool Append( Mtl* mtl , const nString& uniqueName );

private:
    /// class operator , hash function  for material pointer
    struct MtlHash
    {
        unsigned int operator()(Mtl* op) const
        {
            size_t val = size_t(op);
            val = val / sizeof(Mtl);
            return static_cast<unsigned int>( val );
        } 
    };

    /// class for bumpmap hashtable
    struct BumpMap
    {
        nString name;
        unsigned int Hash() const;
        bool operator == (const BumpMap& elm1) const;
        bool operator <  (const BumpMap& elm1) const;
    };

    nMaterialBuilderServer builder;
    nAnimatorBuilderServer* animBuilder;
    nRoot* rootMaterialNode;
    nRoot* rootMaterialType;
    nArray<n3dsMaterial> index2MaterialExport;
    nMapTable<int, Mtl*, MtlHash> mtlToIndex;
    nMapTable<nString, BumpMap, nMapTableHash<BumpMap> > listBumpMaps;
    /// Cache of nMatTypePropBuilder with library name
    nMapTableTypes<nMatTypePropBuilder*>::NString listMatTypeLibrary;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimatorBuilderServer* 
n3dsMaterialList::GetAnimatorBuilder() const
{
    return this->animBuilder;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3dsMaterialList::IsShaderNode(int idMaterial)
{
    return this->index2MaterialExport[idMaterial].GetMatSurface().GetAbstractType() == nAbstractPropBuilder::NSURFACE;
}

//------------------------------------------------------------------------------
/**
*/
inline
unsigned int 
n3dsMaterialList::BumpMap::Hash() const
{
    return nStringHash(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3dsMaterialList::BumpMap::operator == (const BumpMap& elm1) const
{
    return this->name == elm1.name;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3dsMaterialList::BumpMap::operator <  (const BumpMap& elm1) const
{
    int val = strcmp(this->name.Get() , elm1.name.Get() );
    return val < 0;
}


#endif