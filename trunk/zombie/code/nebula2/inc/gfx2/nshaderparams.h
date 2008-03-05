#ifndef N_SHADERPARAMS_H
#define N_SHADERPARAMS_H
//------------------------------------------------------------------------------
/**
    @class nShaderParams
    @ingroup Gfx2

    A container for shader parameters. A shader parameter block
    can be applied to a shader with one call (instead of issuing dozens
    of method calls to set parameters).
    
    Note that only simple datatypes (not arrays) can be kept in shader
    parameter blocks.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nshaderstate.h"
#include "gfx2/nshaderarg.h"
#include "gfx2/ngfxlog.h"
#include "util/narray.h"

#ifndef __NEBULA_NO_LOG__
#include "kernel/nlogclass.h"
#include "gfx2/ntexture2.h"
#endif

//------------------------------------------------------------------------------
class nShaderParams
{
public:
    /// constructor
    nShaderParams();
    /// destructor
    ~nShaderParams();
    /// clear array
    void Clear();
    /// copy array
    void Copy(nShaderParams& shaderParams);
    /// get number of valid parameters in object
    int GetNumValidParams() const;
    /// return true if parameter is valid
    bool IsParameterValid(nShaderState::Param p) const;
    /// clear a parameter
    void ClearArg(nShaderState::Param p);
    /// set a single parameter
    void SetArg(nShaderState::Param p, const nShaderArg& arg);
    /// set a single parameter, optimization for matrix44
    void SetArg(nShaderState::Param p, const matrix44 & arg);
    /// set a single parameter, optimization for vector4
    void SetArg(nShaderState::Param p, const vector4 & arg);
    /// set a set of parameters from another parameter block
    void SetParams(const nShaderParams& shaderParams);
    /// get a single parameter
    const nShaderArg& GetArg(nShaderState::Param p) const;
    /// clear a single parameter
    void ClearParam(nShaderState::Param p);
    /// get shader parameter using direct index
    nShaderState::Param GetParamByIndex(int index) const;
    /// get shader argument using direct index
    const nShaderArg& GetArgByIndex(int index) const;

    #ifndef __NEBULA_NO_LOG__
    /// log shader parameters
    void Log() const;
    #endif

private:
    class ParamAndArg
    {
    public:
        /// default constructor
        ParamAndArg();
        /// constructor
        ParamAndArg(nShaderState::Param p, const nShaderArg& a);
        /// destructor
        ~ParamAndArg();

        nShaderState::Param param;
        nShaderArg arg;
    };
    char paramIndex[nShaderState::NumParameters];   // index into paramArray, -1 for invalid params
    nArray<ParamAndArg> paramArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::ParamAndArg::ParamAndArg() :
    param(nShaderState::InvalidParameter)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::ParamAndArg::ParamAndArg(nShaderState::Param p, const nShaderArg& a) :
    param(p),
    arg(a)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::ParamAndArg::~ParamAndArg()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::Clear()
{
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        this->paramIndex[i] = -1;
    }
    this->paramArray.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::Copy(nShaderParams& shaderParams)
{
    *this = shaderParams;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::nShaderParams() :
    paramArray(0, 8)
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::~nShaderParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShaderParams::GetNumValidParams() const
{
    return this->paramArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nShaderArg&
nShaderParams::GetArgByIndex(int index) const
{
    n_assert(index >= 0 && index < this->paramArray.Size());
    return this->paramArray[index].arg;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param
nShaderParams::GetParamByIndex(int index) const
{
    n_assert(index >= 0 && index < this->paramArray.Size());
    return this->paramArray[index].param;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderParams::IsParameterValid(nShaderState::Param p) const
{
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    return (-1 != this->paramIndex[p]);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::ClearArg(nShaderState::Param p)
{
    n_assert(p < nShaderState::NumParameters);
    char index = this->paramIndex[p];
    if (index != -1)
    {
        // fix parameters pointing to greater indices
        int i;
        for (i = 0; i < nShaderState::NumParameters; i++)
        {
            if (this->paramIndex[i] > index)
            {
                --this->paramIndex[i];
            }
        }
        this->paramArray.Erase(index);
        this->paramIndex[p] = -1;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetArg(nShaderState::Param p, const nShaderArg& arg)
{
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    char index = this->paramIndex[p];

    ParamAndArg paramAndArg(p, arg);
    if (index == -1)
    {
        this->paramArray.Append(paramAndArg);
        this->paramIndex[p] = static_cast<char>( this->paramArray.Size() - 1);
    }
    else
    {
        this->paramArray[index] = paramAndArg;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetParams(const nShaderParams& params)
{
    int i;
    int numValidParams = params.GetNumValidParams();
    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = params.GetParamByIndex(i);
        const nShaderArg& curArg = params.GetArgByIndex(i);
        this->SetArg(curParam, curArg);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const nShaderArg&
nShaderParams::GetArg(nShaderState::Param p) const
{
    static nShaderArg invalidArg;
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    char index = this->paramIndex[p];
    if (index != -1)
    {
        return this->paramArray[index].arg;
    }
    else
    {
        n_assert2(index != -1, "Shader parameter wasn't set!");
        return invalidArg;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::ClearParam(nShaderState::Param p)
{
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    char index = this->paramIndex[p];
    
    if (-1 != index)
    {
        // remove arg
        this->paramArray.Erase(index);
        this->paramIndex[p] = -1;

        // fix remaining indices
        int i;
        for (i = 0; i < nShaderState::NumParameters; i++)
        {
            if (this->paramIndex[i] >= index)
            {
                --this->paramIndex[i];
            }
        }
    }
}

//------------------------------------------------------------------------------
inline
void 
nShaderParams::SetArg(nShaderState::Param p, const matrix44 & arg)
{
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    char index = this->paramIndex[p];

    ParamAndArg * paramAndArg = 0;
    if (index == -1)
    {
        paramAndArg = this->paramArray.Reserve(1);
        paramAndArg->param = p;
        this->paramIndex[p] = static_cast<char>( this->paramArray.Size() - 1);
    }
    else
    {
        paramAndArg = &this->paramArray[index];
    }
    n_assert(paramAndArg);

    paramAndArg->arg.SetMatrix44(&arg);
}

//------------------------------------------------------------------------------
inline
void 
nShaderParams::SetArg(nShaderState::Param p, const vector4 & arg)
{
    n_assert(p >= 0 && p < nShaderState::NumParameters);
    char index = this->paramIndex[p];

    ParamAndArg * paramAndArg = 0;
    if (index == -1)
    {
        paramAndArg = this->paramArray.Reserve(1);
        paramAndArg->param = p;
        this->paramIndex[p] = static_cast<char>( this->paramArray.Size() - 1);
    }
    else
    {
        paramAndArg = &this->paramArray[index];
    }
    n_assert(paramAndArg);

    paramAndArg->arg.SetVector4(arg);
}

//------------------------------------------------------------------------------
#ifndef __NEBULA_NO_LOG__
inline
void 
nShaderParams::Log() const
{
    NLOG(gfx, (NLOG_SHADERPARAMS | 0, "Begin ShaderParams=0x%x numParams=%d", this, this->GetNumValidParams()));
    for(int i = 0;i < this->GetNumValidParams();i++)
    {
        const nShaderArg & arg = this->GetArgByIndex(i);
        matrix44 mat;
        nString argValue;
        nString row;

        switch( arg.GetType() )
        {
        case nShaderState::Void:
            break;
        case nShaderState::Bool:
            argValue.SetBool( arg.GetBool() );
            break;
        case nShaderState::Int:
            argValue.SetInt( arg.GetInt() );
            break;
        case nShaderState::Float:
            argValue.SetFloat( arg.GetFloat() );
            break;
        case nShaderState::Float4:
            argValue.SetVector4( arg.GetVector4() );
            break;
        case nShaderState::Matrix44:
            //mat = arg.GetMatrix44();
            //row.SetVector4( mat.m[0] );
            //argValue.Append(row);
            break;
        case nShaderState::Texture:
            argValue.Set( arg.GetTexture()->GetFilename().Get() );
            break;
        default:
            argValue = "unknown type";
            break;
        };

        NLOG(gfx, (NLOG_SHADERPARAMS | 0, "Param %d - %s=%s (%s)",
            i,
            nShaderState::ParamToString( this->GetParamByIndex(i) ),
            argValue.Get(),
            nShaderState::TypeToString( arg.GetType() ) ));
    }
}
#endif

//------------------------------------------------------------------------------
#endif
