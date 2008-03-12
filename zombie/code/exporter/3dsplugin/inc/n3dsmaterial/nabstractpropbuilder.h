#ifndef N_3DS_ABSTRACT_PROP_BUILDER_H
#define N_3DS_ABSTRACT_PROP_BUILDER_H

#include "util/nsortedarray.h"
#include "util/nstring.h"
#include "nmattypepropbuilder.h"
#include "mathlib/vector.h"

class nMaterialBuilderServer;
class nAbstractShaderNode;
class nClass;

//------------------------------------------------------------------------------
/**
    @class nAbstractPropBuilder
    @ingroup n3dsMaxMaterial
    @brief builder of nMaterialNode or nSurfaceNode.

*/
class nAbstractPropBuilder
{

public:
    enum AbstractType
    {
        NMATERIAL,
        NSURFACE,
        SHADERTYPELIBRARY,
        NONE
    };

    ///todo: change the varName by nShadenShaderState::Param, small optimization
    class varTexture
    {
    public:
        varTexture();
        varTexture (const nString& varName, const nString& texName);
        nString varName;
        nString texName;
    };

    class varInt
    {
    public:
        varInt();
        varInt(const nString& varName, int val);
        nString varName;
        int val;
    };

    class varFloat
    {
    public:
        varFloat();
        varFloat(const nString& varName, float val);
        nString varName;
        float val;
    };

    class varVector
    {   
    public:
        varVector();
        varVector(const nString& varName, const vector4& val);
        nString varName;
        vector4 val;
    };

    class varShader
    {
    public:
        varShader();
        varShader(const nString& pass, const nString& val);
        nString varName;
        nString val;
    };

private:
    static int __cdecl TextureSorter(const varTexture* elm0, const varTexture* elm1);
public:
    typedef nSortedArray <varTexture, TextureSorter> nSortedTextures;


    nAbstractPropBuilder();
    /// remove unnecesary variables by nMatTypePropBuilder
    void Reduce();
    void SetTexture ( const varTexture& var);
    void SetTexture ( const nString& varName, const nString& texName);
    const nSortedTextures& GetTextureArray() const;
    nSortedTextures& GetTextureArray();
    void SetInt     ( const varInt&     var);
    void SetInt     ( const nString& varName, int val);
    void SetFloat   ( const varFloat&   var);
    void SetFloat   ( const nString& varName, float val);
    void SetVector  ( const varVector&  var);
    void SetVector  ( const nString& varName, const vector4& val);
    void SetShader  ( const varShader&  var);
    void SetShader  ( const nString& pass, const nString& val);
    void SetAnim    ( const nString& pathAnim);
    //void Set type material and put material type builder
    void SetMaterial( const nMatTypePropBuilder&   val);
    /// put the material type builder
    void SetMatType( const nMatTypePropBuilder&   val);
    /// Set type library and selelect it by name
    void SetMaterialTypeFromLibrary( const char* name);
    /// Set type library and selelect it by name
    void SetMaterialTypeFromLibrary( const char* name , const nMatTypePropBuilder&   val);
    /// return the name in the library of material type
    const char* GetMaterialTypeName() const;
    /// return if select of library invalid
    bool IsInvalidShader() const;
    /// return if select custom
    bool IsCustomShader() const;
    AbstractType GetAbstractType() const;
    const char*  GetNameClass();
    void SetAbstractType(AbstractType val);
    const char* GetUniqueString();
    void SetTo      ( nAbstractShaderNode* node);
    void operator +=(const nAbstractPropBuilder& rhs);
    const nMatTypePropBuilder& GetMatType() const;

    

private:

    friend class nMaterialBuilderServer;

    AbstractType thisType;
    nString  stringKey;
    bool stringValid;

    nMatTypePropBuilder matType;
    bool matTypeIsValid;
    /// only if material type is LIBRARY
    nString materialTypeNameInLibrary; 

    
    nSortedTextures textures;

    static int __cdecl FloatSorter(const varFloat* elm0, const varFloat* elm1);
    nSortedArray <varFloat, FloatSorter> floats;

    static int __cdecl IntSorter(const varInt* elm0, const varInt* elm1);
    nSortedArray <varInt, IntSorter> integers;

    static int __cdecl VectorSorter(const varVector* elm0, const varVector* elm1);
    nSortedArray <varVector, VectorSorter> vectors;

    static int __cdecl ShaderSorter(const varShader* elm0, const varShader* elm1);
    nSortedArray <varShader, ShaderSorter> shaders;

    static int __cdecl AnimSorter( const nString* elm0, const nString* elm1);
    nSortedArray <nString, AnimSorter> anims;


    static nClass* nmaterialnode;
    static nClass* nsurfacenode;

};

inline
const nMatTypePropBuilder&
nAbstractPropBuilder::GetMatType() const
{
    return this->matType;
}

inline
void 
nAbstractPropBuilder::SetAbstractType(AbstractType val)
{
    this->thisType = val;
}

inline
nAbstractPropBuilder::AbstractType 
nAbstractPropBuilder::GetAbstractType() const
{
    return this->thisType;
}

inline
void 
nAbstractPropBuilder::SetTexture (const nString& varName, const nString& texName)
{
    this->SetTexture(varTexture(varName,texName));
}

inline
void 
nAbstractPropBuilder::SetInt( const nString& varName, int val)
{
    this->SetInt(varInt(varName, val));
}

inline
void
nAbstractPropBuilder::SetFloat( const nString& varName, float val)
{
    this->SetFloat(varFloat(varName, val));
}

inline
void
nAbstractPropBuilder::SetVector  ( const nString& varName, const vector4& val)
{
    this->SetVector(varVector(varName,val));
}

inline
void
nAbstractPropBuilder::SetShader  ( const nString& pass, const nString& val)
{
    this->SetShader(varShader(pass,val));
}

inline
nAbstractPropBuilder::varInt::varInt()
{
}

inline
nAbstractPropBuilder::varInt::varInt(const nString& varName, int val)
{
    this->val = val;
    this->varName = varName;
}


inline
nAbstractPropBuilder::varFloat::varFloat()
{
}

inline
nAbstractPropBuilder::varFloat::varFloat(const nString& varName, float val)
{
    this->val = val;
    this->varName = varName;
}

inline
nAbstractPropBuilder::varVector::varVector()
{
}

inline
nAbstractPropBuilder::varVector::varVector(const nString& varName, const vector4& val)
{
    this->val = val;
    this->varName = varName;
}

inline
nAbstractPropBuilder::varTexture::varTexture()
{
}

inline
nAbstractPropBuilder::varTexture::varTexture (const nString& varName, const nString& texName)
{
    this->texName = texName;
    this->varName = varName;
}

inline
nAbstractPropBuilder::varShader::varShader()
{
}

inline
nAbstractPropBuilder::varShader::varShader (const nString& pass, const nString& val)
{
    this->val = val;
    this->varName = pass;
}
#endif