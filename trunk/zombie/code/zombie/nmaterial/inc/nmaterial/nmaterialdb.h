#ifndef N_MATERIALDB_H
#define N_MATERIALDB_H
//------------------------------------------------------------------------------
/**
    @class nMaterialDb
    @ingroup NebulaMaterialSystem
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A material database contains the information on shader parameters
    required by a material description.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "gfx2/nshaderstate.h"
#include "nmaterial/nmaterialdbxmlparser.h"
#include "variable/nvariableserver.h"

class nMaterial;
//------------------------------------------------------------------------------
class nMaterialDb
{
public:
    /// @struct nMatShaderParam
    /// a shader parameter
    struct ShaderParam
    {
        ShaderParam();
        ~ShaderParam();
        nShaderState::Param param;
        nShaderState::Type type;
        nVariable::Handle varHandle;
    };

    /// @struct nMatParam
    /// a material parameter
    struct Param
    {
    public:
        Param();
        ~Param();
        nString type;
        nString name;
        int vertexComponentFlags;
        nArray<nString> shaderParams;
    };

    /// constructor
    nMaterialDb();
    /// destructor
    ~nMaterialDb();
    /// set xml filename
    void SetFilename(const nString& n);
    /// get xml filename
    const nString& GetFilename() const;
    /// open the XML document
    bool OpenXml();
    /// close the XML document
    void CloseXml();
    /// open the object, the XML document must be open
    bool Open();
    /// close the object
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// set the shader path
    void SetShaderPath(const nString& p);
    /// get the shader path, valid after OpenXmlDoc()
    const nString& GetShaderPath() const;
    /// add a shader param
    void AddShaderParam(ShaderParam& param);
    /// get array of shader parameters
    const nArray<ShaderParam>& GetShaderParams() const;
    /// add a material param
    void AddMaterialParam(Param& param);
    /// get array of material parameters
    const nArray<Param>& GetMaterialParams() const;
    /// add a global variable
    void AddVariable(const nVariable& var);
    /// get global variable handles
    const nArray<nVariable::Handle>& GetVariableHandles() const;

    /// find a material parameter by type and name
    bool FindParam(const char* type, const char* name, Param& param) const;
    /// find a material parameter by type and name
    bool FindShaderParam(nShaderState::Param, ShaderParam& shaderParam);
    /// get set of texture shader parameters for this material
    void GetShaderParams(nMaterial* material, nArray<nShaderState::Param>& shaderParams) const;
#ifdef __ZOMBIE_EXPORTER__
    /// get set of texture shader parameters for this material
    void GetShaderParams(nMaterial* material, nFixedArray<bool>& shaderParams) const;
#endif
    /// get set of vertex components for this material
    int GetVertexFlags(nMaterial* material) const;

    /// helper function for setting Usage from script
    static nString VertexCompMaskToString(int flags);
    /// helper function for getting Usage from script
    static int StringToVertexCompMask(const char* flagString);

private:
    friend class nMaterialDbXmlParser;

    bool isOpen;
    nString xmlFilename;
    nString shaderPath;
    nMaterialDbXmlParser xmlParser;
    nArray<Param> materialParams;
    nArray<ShaderParam> shaderParams;
    nArray<nVariable::Handle> variableHandles;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDb::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nMaterialDb::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDb::SetFilename(const nString& n)
{
    this->xmlFilename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nMaterialDb::GetFilename() const
{
    return this->xmlFilename;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterialDb::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nMaterialDb::Param>&
nMaterialDb::GetMaterialParams() const
{
    return this->materialParams;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDb::AddMaterialParam(Param& param)
{
    this->materialParams.Append(param);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nMaterialDb::ShaderParam>&
nMaterialDb::GetShaderParams() const
{
    return this->shaderParams;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDb::AddShaderParam(ShaderParam& param)
{
    this->shaderParams.Append(param);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialDb::AddVariable(const nVariable& var)
{
    this->variableHandles.Append(var.GetHandle());
    if (!nVariableServer::Instance()->GetGlobalVariable(var.GetHandle()))
    {
        nVariableServer::Instance()->SetGlobalVariable(var);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nVariable::Handle>&
nMaterialDb::GetVariableHandles() const
{
    return this->variableHandles;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialDb::ShaderParam::ShaderParam() :
    param(nShaderState::InvalidParameter),
    type(nShaderState::InvalidType),
    varHandle(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialDb::ShaderParam::~ShaderParam()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialDb::Param::Param() :
    vertexComponentFlags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialDb::Param::~Param()
{
    // empty
}

//------------------------------------------------------------------------------
#endif
