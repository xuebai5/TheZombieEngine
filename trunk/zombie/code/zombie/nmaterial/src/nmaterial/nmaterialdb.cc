#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialdb.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialdb.h"
#include "nmaterial/nmaterialdbxmlparser.h"
#include "nmaterial/nmaterial.h"

//------------------------------------------------------------------------------
/**
*/
nMaterialDb::nMaterialDb() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialDb::~nMaterialDb()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Open the XML document. This will just load the XML document and
    initialize the shader path. The rest of the initialization happens
    inside nMaterialDb::Open(). This 2-step approach is necessary to
    prevent a shader initialization chicken/egg problem 
*/
bool
nMaterialDb::OpenXml()
{
    this->xmlParser.SetDatabase(this);
    if (!this->xmlParser.OpenXml())
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the XML document. This method should be called after
    nMaterialDb::Open() to release the memory assigned to the XML
    document data.
*/
void
nMaterialDb::CloseXml()
{
    this->xmlParser.CloseXml();
}

//------------------------------------------------------------------------------
/**
    Open the render path. This will parse the xml file which describes
    the render path and configure the render path object from it.
*/
bool
nMaterialDb::Open()
{
    n_assert(!this->isOpen);
    if (!xmlParser.ParseXml())
    {
        return false;
    }
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the render path. This will delete all embedded objects.
*/
void
nMaterialDb::Close()
{
    n_assert(this->isOpen);
    this->shaderParams.Clear();
    this->materialParams.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDb::FindParam(const char* type, const char* name, Param& param) const
{
    int numParams = this->materialParams.Size();
    nString paramType(type);
    nString paramName(name);
    for (int i = 0; i < numParams; ++i)
    {
        const Param& curParam = this->materialParams[i];
        if (curParam.type == paramType && curParam.name == paramName)
        {
            param = curParam;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDb::FindShaderParam(nShaderState::Param param, ShaderParam& shaderParam)
{
    int numParams = this->shaderParams.Size();
    for (int i = 0; i < numParams; ++i)
    {
        const ShaderParam& curParam = this->shaderParams[i];
        if (curParam.param == param)
        {
            shaderParam = this->shaderParams[i];
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Computes the set of shader parameters that are required for
    this combination of material parameters.
*/
void
nMaterialDb::GetShaderParams(nMaterial* material, nArray<nShaderState::Param>& shaderParams) const
{
    shaderParams.Clear();
    int numParams = material->GetNumParams();
    for (int i = 0; i < numParams; ++i)
    {
        const char* paramName;
        const char* paramValue;
        material->GetParamAt(i, &paramName, &paramValue);

        Param param;
        if (this->FindParam(paramName, paramValue, param))
        {
            for (int iparam = 0; iparam < param.shaderParams.Size(); ++iparam)
            {
                nShaderState::Param shaderParam = nShaderState::StringToParam(param.shaderParams[iparam].Get());
                if (shaderParam != nShaderState::InvalidParameter && !shaderParams.Find(shaderParam))
                {
                    shaderParams.Append(shaderParam);
                }
            }
        }
    }
}

#ifdef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
    Computes the set of shader parameters that are required for
    this combination of material parameters.
*/
void
nMaterialDb::GetShaderParams(nMaterial* material, nFixedArray<bool>& shaderParamsUse) const
{
    n_assert(shaderParamsUse.Size() == nShaderState::NumParameters)
    for (int idx = 0; idx < shaderParamsUse.Size() ; ++idx)
    {
        shaderParamsUse[idx] = false;
    }
    nArray<nShaderState::Param> shaderParamsList; 
    this->GetShaderParams(material, shaderParamsList);

    for (int idx = 0; idx < shaderParamsList.Size() ; ++idx)
    {
        shaderParamsUse[shaderParamsList[idx]] = true;
    }
}
#endif // __ZOMBIE_EXPORTER__

//------------------------------------------------------------------------------
/**
    Computes the vertex component mask with all vertex components
    required for this combination of material parameters.
*/
int
nMaterialDb::GetVertexFlags(nMaterial* material) const
{
    int vertexFlags = nMesh2::Coord;//is this correct?
    int i;
    int numParams = material->GetNumParams();
    for (i = 0; i < numParams; ++i)
    {
        const char* paramName;
        const char* paramValue;
        material->GetParamAt(i, &paramName, &paramValue);

        Param param;
        if (this->FindParam(paramName, paramValue, param))
        {
            vertexFlags |= param.vertexComponentFlags;
        }
    }

    return vertexFlags;
}

//------------------------------------------------------------------------------
/**
    Converts the flags into a string of the form
    "Coord|Normal|Uv0|"
    (includes a trailing '|')
*/
nString
nMaterialDb::VertexCompMaskToString(int vertexComp)
{
    nString vertexString;
    if (vertexComp)
    {
        if (vertexComp & nMesh2::Coord) vertexString += "Coord|";
        if (vertexComp & nMesh2::Normal) vertexString += "Normal|";
        if (vertexComp & nMesh2::Uv0) vertexString += "Uv0|";
        if (vertexComp & nMesh2::Uv1) vertexString += "Uv1|";
        if (vertexComp & nMesh2::Uv2) vertexString += "Uv2|";
        if (vertexComp & nMesh2::Uv3) vertexString += "Uv3|";
        if (vertexComp & nMesh2::Color) vertexString += "Color|";
        if (vertexComp & nMesh2::Tangent) vertexString += "Tangent|";
        if (vertexComp & nMesh2::Binormal) vertexString += "Binormal|";
        if (vertexComp & nMesh2::Weights) vertexString += "Weights|";
        if (vertexComp & nMesh2::JIndices) vertexString += "JIndices|";
        if (vertexComp & nMesh2::Coord4) vertexString += "Coord4|";
    }
    return vertexString;
}
//------------------------------------------------------------------------------
/**
    Converts a set of vertexComp flags represented as a string of the form
    "ReadOnly|NeedsVertexShader"
    into the corresponding integer representation.
    Trailing '|'s are ignored//, as is case.
*/
int
nMaterialDb::StringToVertexCompMask(const char* vertexCompString)
{
    int vertexComp = 0;
    nString workingString = vertexCompString;
    if (!workingString.IsEmpty())
    {
        const char* compString = workingString.GetFirstToken("|");
        while (compString)
        {   
            if (!strcmp(compString, "Coord"))
            {
                vertexComp |= nMesh2::Coord;
            }
            else if (!strcmp(compString, "Normal"))
            {
                vertexComp |= nMesh2::Normal;
            }
            else if (!strcmp(compString, "Uv0"))
            {
                vertexComp |= nMesh2::Uv0;
            }
            else if (!strcmp(compString, "Uv1"))
            {
                vertexComp |= nMesh2::Uv1;
            }
            else if (!strcmp(compString, "Uv2"))
            {
                vertexComp |= nMesh2::Uv2;
            }
            else if (!strcmp(compString, "Uv3"))
            {
                vertexComp |= nMesh2::Uv3;
            }
            else if (!strcmp(compString, "Color"))
            {
                vertexComp |= nMesh2::Color;
            }
            else if (!strcmp(compString, "Tangent"))
            {
                vertexComp |= nMesh2::Tangent;
            }
            else if (!strcmp(compString, "Binormal"))
            {
                vertexComp |= nMesh2::Binormal;
            }
            else if (!strcmp(compString, "Weights"))
            {
                vertexComp |= nMesh2::Weights;
            }
            else if (!strcmp(compString, "JIndices"))
            {
                vertexComp |= nMesh2::JIndices;
            }
            else if (!strcmp(compString, "Coord4"))
            {
                vertexComp |= nMesh2::Coord4;
            }
            else
            {
                n_error("nMaterialDb::StringToVertexCompMask: "
                        "Invalid component string '%s' in ", compString);
            }
            compString = workingString.GetNextToken("|");
        }
    }
    return vertexComp;
}
