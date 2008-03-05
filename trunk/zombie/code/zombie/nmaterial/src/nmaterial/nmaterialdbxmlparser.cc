#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialdbxmlparser.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialdbxmlparser.h"
#include "nmaterial/nmaterialdb.h"
#include "kernel/nfileserver2.h"
#include "tinyxml/tinyxml.h"

//------------------------------------------------------------------------------
/**
*/
nMaterialDbXmlParser::nMaterialDbXmlParser() :
    xmlDocument(0),
    materialDb(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialDbXmlParser::~nMaterialDbXmlParser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDbXmlParser::OpenXml()
{
    n_assert(this->materialDb);
    n_assert(0 == this->xmlDocument);

    nString mangledPath = nFileServer2::Instance()->ManglePath(this->materialDb->GetFilename().Get());
    this->xmlDocument = n_new(TiXmlDocument);
    if (this->xmlDocument->LoadFile(mangledPath.Get()))
    {
        // initialize the shader path 
        TiXmlHandle docHandle(this->xmlDocument);
        TiXmlElement* elmMaterialDb = docHandle.FirstChildElement("MaterialDatabase").Element();
        n_assert(elmMaterialDb);
        this->materialDb->SetShaderPath(elmMaterialDb->Attribute("shaderpath"));
        return true;
    }
    else
    {
        n_delete(this->xmlDocument);
        this->xmlDocument = 0;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialDbXmlParser::CloseXml()
{
    n_assert(this->xmlDocument);
    n_delete(this->xmlDocument);
    this->xmlDocument = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDbXmlParser::ParseXml()
{
    n_assert(this->materialDb);
    n_assert(this->xmlDocument);

    TiXmlHandle docHandle(this->xmlDocument);
    TiXmlElement* elmMaterialDb = docHandle.FirstChildElement("MaterialDatabase").Element();
    n_assert(elmMaterialDb);

    // parse child elements
    TiXmlElement* child;
    for (child = elmMaterialDb->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("Float"))
        {
            this->ParseGlobalVariable(nVariable::Float, child, materialDb);
        }
        else if (child->Value() == nString("Float4"))
        {
            this->ParseGlobalVariable(nVariable::Float4, child, materialDb);
        }
        else if (child->Value() == nString("ShaderParam"))
        {
            this->ParseShaderParam(child, materialDb);
        }
        else if (child->Value() == nString("MaterialParam"))
        {
            this->ParseMaterialParam(child, materialDb);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse a ShaderParam xml element.
*/
void
nMaterialDbXmlParser::ParseShaderParam(TiXmlElement* elm, nMaterialDb* materialDb)
{
    n_assert(elm && materialDb);
    nMaterialDb::ShaderParam newShaderParam;
    newShaderParam.param = nShaderState::StringToParam(elm->Attribute("name"));
    newShaderParam.type = nShaderState::StringToType(elm->Attribute("type"));

    if (elm->Attribute("variable"))
    {
        newShaderParam.varHandle = nVariableServer::Instance()->GetVariableHandleByName(elm->Attribute("variable"));
    }

    materialDb->AddShaderParam(newShaderParam);
}

//------------------------------------------------------------------------------
/**
    Parse a material Param xml element.
*/
void
nMaterialDbXmlParser::ParseMaterialParam(TiXmlElement* elm, nMaterialDb* materialDb)
{
    n_assert(elm && materialDb);
    nMaterialDb::Param newParam;
    newParam.type = elm->Attribute("type");
    newParam.name = elm->Attribute("name");
    nString VertexFlagsStr = nMaterialDb::StringToVertexCompMask(elm->Attribute("vertexComp"));

    // parse children
    TiXmlElement* child;
    for (child = elm->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("param"))
        {
            // parse shader param requirement from material parameter
            nString shaderParamName(child->Attribute("name"));
            newParam.shaderParams.Append(shaderParamName);
        }
    }
    materialDb->AddMaterialParam(newParam);
}

//------------------------------------------------------------------------------
/**
    Parse a Float, Vector4, Int or Bool element inside a MaterialDb
    element (these are global variable definitions.
*/
void
nMaterialDbXmlParser::ParseGlobalVariable(nVariable::Type dataType, TiXmlElement* elm, nMaterialDb* materialDb)
{
    n_assert(elm && materialDb);
    nVariable var = this->ParseVariable(dataType, elm);
    if (var.GetHandle() != nVariable::InvalidHandle)
    {
        materialDb->AddVariable(var);
    }
}

//------------------------------------------------------------------------------
/**
    Create a nVariable from XML element attributes "name" and "value".
*/
nVariable
nMaterialDbXmlParser::ParseVariable(nVariable::Type dataType, TiXmlElement* elm)
{
    n_assert(elm);
    const char* varName = elm->Attribute("name");
    n_assert(varName);
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(varName);
    nVariable newVariable(dataType, varHandle);
    switch (dataType)
    {
        case nVariable::Int:
            newVariable.SetInt(this->GetIntAttr(elm, "value", 0));
            break;

        case nVariable::Float:
            newVariable.SetFloat(this->GetFloatAttr(elm, "value", 0.0f));
            break;

        case nVariable::Vector4:
            {
                vector4 v4(0.0f, 0.0f, 0.0f, 0.0f);
                newVariable.SetVector4(this->GetVector4Attr(elm, "value", v4));
            }
            break;
    }
    return newVariable;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDbXmlParser::HasAttr(TiXmlElement* elm, const char* name)
{
    n_assert(elm && name);
    return (elm->Attribute(name) != 0);
}

//------------------------------------------------------------------------------
/**
*/
int
nMaterialDbXmlParser::GetIntAttr(TiXmlElement* elm, const char* name, int defaultValue)
{
    n_assert(elm && name);
    int value;
    int ret = elm->QueryIntAttribute(name, &value);
    if (TIXML_SUCCESS == ret)
    {
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nMaterialDbXmlParser::GetFloatAttr(TiXmlElement* elm, const char* name, float defaultValue)
{
    n_assert(elm && name);
    double value;
    int ret = elm->QueryDoubleAttribute(name, &value);
    if (TIXML_SUCCESS == ret)
    {
        return float(value);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
vector4
nMaterialDbXmlParser::GetVector4Attr(TiXmlElement* elm, const char* name, const vector4& defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        nArray<nString> tokens;
        valStr.Tokenize(" ", tokens);
        vector4 value;
        value.x = tokens[0].AsFloat();
        value.y = tokens[1].AsFloat();
        value.z = tokens[2].AsFloat();
        value.w = tokens[3].AsFloat();
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFloat4
nMaterialDbXmlParser::GetFloat4Attr(TiXmlElement* elm, const char* name, const nFloat4& defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        nArray<nString> tokens;
        valStr.Tokenize(" ", tokens);
        nFloat4 value;
        value.x = tokens[0].AsFloat();
        value.y = tokens[1].AsFloat();
        value.z = tokens[2].AsFloat();
        value.w = tokens[3].AsFloat();
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDbXmlParser::GetBoolAttr(TiXmlElement* elm, const char* name, bool defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        if ((valStr == "true") || (valStr == "True") || 
            (valStr == "on") || (valStr == "On") ||
            (valStr == "yes") || (valStr == "Yes"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return defaultValue;
    }
}
