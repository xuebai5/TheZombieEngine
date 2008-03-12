#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsmaterial/nmattypepropbuilder.h"
#include "nmaterial/nmaterial.h"

nMatTypePropBuilder::nMatTypePropBuilder() : stringValid(false)
{

}
nMatTypePropBuilder::nMatTypePropBuilder(nMaterial& matType)
{
    this->GetFrom(matType);
}

void 
nMatTypePropBuilder::GetFrom(nMaterial& matType)
{
    int idx;

    for (idx=0; idx < matType.GetNumParams(); idx++ )
    {
        const char* paramName;
        const char* paramValue;

        matType.GetParamAt( idx, &paramName, &paramValue );

        Param param(paramName, paramValue);
        this->params.Append(param);
    }

    this->stringValid = false;
}

void 
nMatTypePropBuilder::SetTo(nMaterial& matType) const
{
    int idx;
    for (idx=0; idx < params.Size(); idx++ )
    {
        Param& param = params[idx];
        matType.AddParam( param.name.Get() , param.value.Get() );
    }

}

int
__cdecl
nMatTypePropBuilder::ParamSorter(const Param* elm0, const Param* elm1)
{

    int val;
    val= strcmp(elm0->name.Get(), elm1->name.Get());
    if (val == 0)
    {
        val = strcmp(elm0->value.Get(), elm1->value.Get());
    }

    return val;
}


void 
nMatTypePropBuilder::AddParam( const Param& param)
{
    params.Append(param);
    this->stringValid = false;
}

bool 
nMatTypePropBuilder::HasParam(const char* name, const char* value) const
{
    const Param param(name,value);
    return (params.FindIndex(param) != -1);
}

bool 
nMatTypePropBuilder::HasParam(const Param& param) const
{
    return (params.FindIndex(param) != -1);
}

const char* 
nMatTypePropBuilder::GetUniqueString()
{
    if (!this->stringValid)
    {
        stringKey="";
        int idx;

        for (idx=0; idx < params.Size(); idx++ )
        {
            Param& param = params[idx];
            stringKey+="PAR#";
            stringKey+=param.name;
            stringKey+=".";
            stringKey+=param.value;
            stringKey+="$";
        }
        stringKey+="mat";
        this->stringValid = true;
    }
    return this->stringKey.Get();
}

void 
nMatTypePropBuilder::operator+=(const nMatTypePropBuilder& rhs)
{
    int idx;
    this->stringValid = false;

    for (idx=0; idx < rhs.params.Size(); idx ++ )
    { 
        this->params.Append( rhs.params[idx] );
    }
}

