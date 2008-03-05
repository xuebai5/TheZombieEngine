#ifndef N_RPSHADER_H
#define N_RPSHADER_H
//------------------------------------------------------------------------------
/**
    @class nRpShader
    @ingroup RenderPath
    @brief A render path shader definition.

    (C) 2004 Radon Labs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "util/narray.h"
#include "kernel/nref.h"
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
class nRpShader
{
public:
    /// constructor
    nRpShader();
    /// destructor
    ~nRpShader();
    /// assignment operator
    void operator=(const nRpShader& rhs);
    /// set shader name
    void SetName(const char* n);
    /// get shader name
    const char* GetName() const;
    /// set shader file
    void SetFilename(const char* n);
    /// get shader file
    const char* GetFilename() const;
    /// set shader file
    void SetTechnique(const char* n);
    /// get shader file
    const char* GetTechnique() const;
    /// set shader bucket index
    void SetBucketIndex(int i);
    /// get shader bucket index
    int GetBucketIndex() const;
    /// validate the shader
    void Validate();
    /// get shader pointer
    nShader2* GetShader() const;

private:
    nString name;
    nString filename;
    nString technique;
    int bucketIndex;
    nRef<nShader2> refShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpShader::operator=(const nRpShader& rhs)
{
    this->name        = rhs.name;
    this->filename    = rhs.filename;
    this->technique   = rhs.technique;
    this->bucketIndex = rhs.bucketIndex;
    this->refShader   = rhs.refShader;
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
nRpShader::SetName(const char* n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRpShader::GetName() const
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpShader::SetFilename(const char* n)
{
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRpShader::GetFilename() const
{
    return this->filename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpShader::SetTechnique(const char* t)
{
    this->technique = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRpShader::GetTechnique() const
{
    return this->technique.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpShader::SetBucketIndex(int i)
{
    n_assert(i >= 0);
    this->bucketIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpShader::GetBucketIndex() const
{
    return this->bucketIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRpShader::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
#endif
