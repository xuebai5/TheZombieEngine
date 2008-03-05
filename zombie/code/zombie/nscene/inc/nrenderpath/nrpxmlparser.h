#ifndef N_RPXMLPARSER_H
#define N_RPXMLPARSER_H
//------------------------------------------------------------------------------
/**
    @class nRpXmlParser
    @ingroup NebulaRenderPathSystem

    Configure a nRenderPath2 object from an XML file.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/vector.h"
#include "variable/nvariable.h"
#include "gfx2/nshaderstate.h"
#include "util/nstring.h"
#include "util/narray.h"

class nRenderPath2;
class nRpPass;
class nRpPhase;
class TiXmlDocument;
class TiXmlElement;

//------------------------------------------------------------------------------
class nRpXmlParser
{
public:
    /// constructor
    nRpXmlParser();
    /// destructor
    ~nRpXmlParser();
    /// set pointer to render path object to initialize
    void SetRenderPath(nRenderPath2* rp);
    /// get pointer to render path object
    nRenderPath2* GetRenderPath() const;
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
	/// parse shader aliases
	void ParseShaders(TiXmlElement* elm, nRenderPath2* renderPath);
    /// parse RenderTarget XML element
    void ParseRenderTarget(TiXmlElement* elm, nRenderPath2* renderPath);
    /// create a variable from an XML element
    nVariable ParseVariable(nVariable::Type dataType, TiXmlElement* elm);
    /// parse a global variable under RenderPath
    void ParseGlobalVariable(nVariable::Type dataType, TiXmlElement* elm, nRenderPath2* renderPath);
    /// parse a Pass XML element
    void ParsePass(TiXmlElement* elm, nRenderPath2* renderPath);
    /// parse a shader state inside a pass
    void ParseShaderState(nShaderState::Type type, TiXmlElement* elm, nRpPass* pass);
    /// parse a phase
    void ParsePhase(TiXmlElement* elm, nRpPass* pass);
    /// parse a sequence
    void ParseSequence(TiXmlElement* elm, nRpPhase* phase);

    TiXmlDocument* xmlDocument;
    nRenderPath2* renderPath;
    nString shaderPath;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpXmlParser::SetRenderPath(nRenderPath2* rp)
{
    n_assert(rp);
    this->renderPath = rp;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath2*
nRpXmlParser::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
#endif
