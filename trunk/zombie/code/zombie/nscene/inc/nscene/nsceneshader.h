#ifndef N_SCENESHADER_H
#define N_SCENESHADER_H
//------------------------------------------------------------------------------
/**
    @class nSceneShader
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>
    @brief An indexed shader for use within the scene subsystem.

    (C) 2005 Conjurer Services, S.A.
*/

class nShader2;
//------------------------------------------------------------------------------
class nSceneShader
{
public:
    /// constructor 1
    nSceneShader();
    /// constructor 2
    nSceneShader(nShader2* shader);
    /// set shader by name
    void SetShaderName(const char* shaderName);
    /// get shader by name
    const char* GetShaderName();
    /// set shader by filename
    void SetShader(const char* filename);
    /// set shader by filename
    const char* GetShader();
    /// set shader object
    void SetShaderObject(nShader2* shader);
    /// get shader object
    nShader2* GetShaderObject();
    /// get bucket index of shader
    int GetShaderIndex();
    /// set shader technique
    void SetTechnique(const char* technique);
    /// get shader technique
    const char* GetTechnique();
    /// set shader sequence
    void SetSequence(const char* sequence);
    /// get shader sequence
    const char* GetSequence();
    /// is entry valid
    bool IsValid();
    /// validate
    void Validate();

    /// begin shader
    int Begin();
    /// end shader
    void End();

    /// shader alias
    nString shaderName;
    /// shader filename
    nString filename;
    /// shader object
    nRef<nShader2> refShader;
    /// optional technique name
    nString technique;
    /// optional sequence name
    nString sequence;
    /// entry index
    int shaderIndex;
    /// in begin shader
    bool inBeginShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSceneShader::nSceneShader() :
    shaderIndex(-1),
    inBeginShader(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneShader::nSceneShader(nShader2* shader) :
    shaderIndex(-1),
    inBeginShader(false)
{
    this->refShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneShader::SetShaderName(const char* shaderName)
{
    this->shaderName = shaderName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSceneShader::GetShaderName()
{
    return this->shaderName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneShader::SetShader(const char* filename)
{
    this->filename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSceneShader::GetShader()
{
    return this->filename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneShader::SetShaderObject(nShader2* shader)
{
    this->refShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nSceneShader::GetShaderObject()
{
    return this->refShader.isvalid() ? this->refShader.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneShader::SetTechnique(const char* technique)
{
    this->technique = technique;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSceneShader::GetTechnique()
{
    return this->technique.IsEmpty() ? 0 : this->technique.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneShader::SetSequence(const char* sequence)
{
    this->sequence = sequence;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSceneShader::GetSequence()
{
    return this->sequence.IsEmpty() ? 0 : this->sequence.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSceneShader::GetShaderIndex()
{
    return this->shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneShader::IsValid()
{
    return this->refShader.isvalid();
}

//------------------------------------------------------------------------------
#endif /*N_SCENESHADER_H*/
