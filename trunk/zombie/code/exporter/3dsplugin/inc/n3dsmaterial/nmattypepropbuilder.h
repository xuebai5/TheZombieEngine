#ifndef N_3DS_MATERIAL_TYPE_EXPORTER_H
#define N_3DS_MATERIAL_TYPE_EXPORTER_H

#include "util/nsortedarray.h"
#include "util/nstring.h"

class nMaterial;

//------------------------------------------------------------------------------
/**
    @class nMatTypePropBuilder
    @ingroup n3dsMaxMaterial
    @brief builder of nMaterial.
    nmaterials is a properties of material type
    example  has a bumpmap, specualar ,....

*/

class nMatTypePropBuilder
{

public:
    class Param
    {
    public:
        Param();
        Param(const char* name, const char* value);
        nString name;
        nString value;
    };

    static int __cdecl ParamSorter(const Param* elm0, const Param* elm1);
    typedef nSortedArray<Param, ParamSorter> List;


    nMatTypePropBuilder();
    nMatTypePropBuilder(nMaterial& matType);
    void GetFrom(nMaterial& matType);
    void SetTo(nMaterial& matType) const;
    void AddParam( const Param& param);
    void AddParam(const char* name, const char* value);
    bool HasParam(const char* name, const char* value) const;
    bool HasParam(const Param& param) const;

    void operator +=(const nMatTypePropBuilder& rhs);
    const char* GetUniqueString();

    const List& GetList() const;
private:
    List   params;
    bool stringValid;
    nString  stringKey;

};


inline
const nMatTypePropBuilder::List& 
nMatTypePropBuilder::GetList() const
{
    return this->params;
}


inline 
void 
nMatTypePropBuilder::AddParam(const char* name, const char* value)
{
    AddParam(Param(name,value));
}

inline 
nMatTypePropBuilder::Param::Param()
{
}

inline
nMatTypePropBuilder::Param::Param(const char* name, const char* value)
{
    this->name = name;
    this->value = value;
}


static 
inline 
nMatTypePropBuilder operator +(const nMatTypePropBuilder& elm1, const nMatTypePropBuilder& elm2) 
{
    nMatTypePropBuilder mat = elm1;
    mat += elm2;
    return mat;
}


#endif