#ifndef N_MATERIALDBXMLPARSER_H
#define N_MATERIALDBXMLPARSER_H
//------------------------------------------------------------------------------
/**
    @class nMaterialDbXmlParser
    @ingroup NebulaMaterialSystem

    Configure a Material database object from an XML file.

    (C) 2005 Conjurer Services, S.A.
*/

#include "variable/nvariable.h"

class nMaterialDb;
class TiXmlDocument;
class TiXmlElement;

//------------------------------------------------------------------------------
class nMaterialDbXmlParser
{
public:
    /// constructor
    nMaterialDbXmlParser();
    /// destructor
    ~nMaterialDbXmlParser();
    /// set pointer to render path object to initialize
    void SetDatabase(nMaterialDb* rp);
    /// get pointer to render path object
    nMaterialDb* GetDatabase() const;
    /// open the XML document
    bool OpenXml();
    /// close the XML document
    void CloseXml();
    /// parse the XML data and initialize the render path object
    bool ParseXml();
    /// get the shader path, valid after OpenXml()
    const nString& GetShaderPath() const;

private:
    /// return true if XML attribute exists
    bool HasAttr(TiXmlElement* elm, const char* name);
    /// get an integer attribute from an xml element
    int GetIntAttr(TiXmlElement* elm, const char* name, int defaultValue);
    /// get a float attribute from an xml element
    float GetFloatAttr(TiXmlElement* elm, const char* name, float defaultValue);
    /// get a vector4 attribute from an xml element
    vector4 GetVector4Attr(TiXmlElement* elm, const char* name, const vector4& defaultValue);
    /// get a float4 attribute from an xml element
    nFloat4 GetFloat4Attr(TiXmlElement* elm, const char* name, const nFloat4& defaultValue);
    /// get a bool attribute from an xml element
    bool GetBoolAttr(TiXmlElement* elm, const char* name, bool defaultValue);

    /// create a variable from an XML element
    nVariable ParseVariable(nVariable::Type dataType, TiXmlElement* elm);
    /// parse a global variable under RenderPath
    void ParseGlobalVariable(nVariable::Type dataType, TiXmlElement* elm, nMaterialDb* materialDb);
    /// parse ShaderParam XML element
    void ParseShaderParam(TiXmlElement* elm, nMaterialDb* materialDb);
    /// parse MaterialParam XML element
    void ParseMaterialParam(TiXmlElement* elm, nMaterialDb* materialDb);

    nMaterialDb* materialDb;
    TiXmlDocument* xmlDocument;
    nString shaderPath;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDbXmlParser::SetDatabase(nMaterialDb* db)
{
    n_assert(db);
    this->materialDb = db;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialDb*
nMaterialDbXmlParser::GetDatabase() const
{
    return this->materialDb;
}

//------------------------------------------------------------------------------
#endif
