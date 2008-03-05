#ifndef N_RPSEQUENCE_H
#define N_RPSEQUENCE_H
//------------------------------------------------------------------------------
/**
    @class nRpSequence
    @ingroup NebulaRenderPathSystem
    @brief Encapsulates a sequence shader in a render path. This is the lowest
    level component which defines the shader states for mesh rendering.
    
    (C) 2004 RadonLabs GmbH
*/    
#include "gfx2/nshader2.h"

class nRenderPath2;
//------------------------------------------------------------------------------
class nRpSequence
{
public:
    /// constructor
    nRpSequence();
    /// destructor
    ~nRpSequence();
    /// set render path
    void SetRenderPath(nRenderPath2* renderPath);
    /// get render path
    nRenderPath2* GetRenderPath();
    /// assignment operator
    void operator=(const nRpSequence& rhs);
    /// set shader path
    void SetShaderAlias(const char* p);
    /// get shader path
    const char* GetShaderAlias() const;
    /// set optional technique
    void SetTechnique(const char* tec);
    /// get optional technique
    const char* GetTechnique() const;
    /// get shader index of embedded shader
    int GetShaderBucketIndex() const;
    /// begin rendering the sequence  shader
    void Begin();
    /// end rendering the sequence shader
    void End();
    /// get shader pointer
    nShader2* GetShader() const;

private:
    friend class nRpPhase;

    /// validate the sequence object
    void Validate();

    nRenderPath2* renderPath;
    nString shaderAlias;
    nString technique;
    nRef<nShader2> refShader;
    int rpShaderIndex;
    int bucketIndex;
    bool inBegin;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::SetRenderPath(nRenderPath2* renderPath)
{
    this->renderPath = renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::operator=(const nRpSequence& rhs)
{
    this->shaderAlias = rhs.shaderAlias;
    this->refShader = rhs.refShader;
    this->technique = rhs.technique;
    if (this->refShader.isvalid())
    {
        this->refShader->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::SetShaderAlias(const char* p)
{
    this->shaderAlias = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRpSequence::GetShaderAlias() const
{
    return this->shaderAlias.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::SetTechnique(const char* tec)
{
    this->technique = tec;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRpSequence::GetTechnique() const
{
    return this->technique.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRpSequence::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpSequence::GetShaderBucketIndex() const
{
    n_assert(this->bucketIndex != -1);
    return this->bucketIndex;
}

//------------------------------------------------------------------------------
#endif
