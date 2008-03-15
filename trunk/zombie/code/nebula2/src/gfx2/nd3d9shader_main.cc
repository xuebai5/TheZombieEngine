//------------------------------------------------------------------------------
//  nd3d9shader_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchndirect3d9.h"
#include "gfx2/nd3d9shader.h"
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9shaderinclude.h"
#include "gfx2/nshaderparams.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/nlogclass.h"
#include <climits>

// FIXME required in nsignalemitter.h !!
#include "entity/nentity.h"

nNebulaClass(nD3D9Shader, "nshader2");

const int LOG_SHADER= NLOG2;

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::nD3D9Shader() :
    refGfxServer("/sys/servers/gfx"),
    effect(0),
    inBeginPass(false),
    hasBeenValidated(false),
    didNotValidate(false),
    curTechniqueNeedsSoftwareVertexProcessing(false)
{
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));
    memset(this->parameterSizes, 0, sizeof(this->parameterSizes));
    memset(this->entryIndex, -1, sizeof(this->entryIndex));

    nResourceServer::Instance()->BindSignal(nResourceServer::SignalResourceReloaded, this, &nD3D9Shader::OnResourceReloaded, 0);
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::~nD3D9Shader()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::UnloadResource()
{
    n_assert(this->IsLoaded());
    n_assert(this->effect);

    nD3D9Server* gfxServer = this->refGfxServer.get();
    n_assert(gfxServer->d3d9Device);

    // if this is the currently set shader, unlink from gfx server
    if (gfxServer->GetShader() == this)
    {
        gfxServer->SetShader(0);
    }

    // release d3dx resources
    this->effect->Release();
    this->effect = 0;

    // reset current shader params
    this->curParams.Clear();

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    Load D3DX effects file.
*/
bool
nD3D9Shader::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(0 == this->effect);

    // mangle path name
    nString filename = this->GetFilename();
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename.Get());

    //load fx file...
    nFile* file = nFileServer2::Instance()->NewFileObject();

    // open the file
    if (!file->Open(mangledPath.Get(), "r"))
    {
        NLOG(resource, (NLOGUSER | LOG_SHADER | 0, "nD3D9Shader: could not load shader file '%s'!", mangledPath.Get()));
        return false;
    }

    // get size of file
    int fileSize = file->GetSize();

    bool success = this->Load(file, 0, fileSize);

    file->Close();
    file->Release();

    return success;
}

//------------------------------------------------------------------------------
/**
    signal handler for reloaded resources 
*/
void
nD3D9Shader::OnResourceReloaded(nResource* rsrc)
{
    // need to clear the parameter cache
    // just in case a reloaded texture was in one of the slots.
    if (rsrc->GetType() == nResource::Texture)
    {
        this->curParams.Clear();
    }
}

//------------------------------------------------------------------------------
/**
    Load D3DX effect from open file.
*/
bool
nD3D9Shader::Load()
{
    return nShader2::Load();
}

//------------------------------------------------------------------------------
/**
    Load D3DX effect from open file.
*/
bool
nD3D9Shader::Load(nFile *file, int /*offset*/, int fileSize)
{
    n_assert(!this->IsValid());
    n_assert(0 == this->effect);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert_return(d3d9Dev,false);

    // mangle path name
    nString filename = this->GetFilename();
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename.Get());

    ID3DXBuffer* errorBuffer = 0;

    // create include file handler
    nString shaderPath(mangledPath.Get());
    nD3D9ShaderInclude includeHandler(shaderPath.ExtractDirName());

    // get global state manager from gfx server
    ID3DXEffectStateManager* effectStateManager = this->refGfxServer->GetEffectStateManager();
    n_assert_return(effectStateManager, false);

    // get the highest supported shader profiles
    LPCSTR vsProfile = D3DXGetVertexShaderProfile(d3d9Dev);
    LPCSTR psProfile = D3DXGetPixelShaderProfile(d3d9Dev);

    if (0 == vsProfile)
    {
        n_printf("Invalid Vertex Shader profile! Fallback to vs_2_0!\n");
        vsProfile = "vs_2_0";
    }

    if (0 == psProfile)
    {
        n_printf("Invalid Pixel Shader profile! Fallback to ps_2_0!\n");
        psProfile = "ps_2_0";
    }

    //n_printf("Shader profiles: %s %s\n", vsProfile, psProfile);
    LPCSTR vendorName = "NVIDIA";//TEMP! get actual vendor name from

    // create macro definitions for shader compiler
    D3DXMACRO defines[] = {
        { "VS_PROFILE", vsProfile },
        { "PS_PROFILE", psProfile },
        { "VS_CURRENT", "vs_2_0" },
        { "PS_CURRENT", "ps_2_0" },
        { vendorName, "1" },
    #ifdef NGAME
        { "NGAME", "1" },
    #endif
        { 0, 0 },
    };

    //for Leipzig conference 2006, defines for compile with maximun profile
    D3DXMACRO definesMaxProfile[] = {
        { "VS_PROFILE", vsProfile },
        { "PS_PROFILE", psProfile },
        { "VS_CURRENT", vsProfile },
        { "PS_CURRENT", psProfile },
        { vendorName, "1" },
    #ifdef NGAME
        { "NGAME", "1" },
    #endif
        { 0, 0 },
    };

    bool retry;
    do
    {
        retry = false;
        hr = this->LoadEffect(file, fileSize, defines, mangledPath, &errorBuffer);

        if (FAILED(hr) && ( 0 != strcmp(psProfile,"ps_2_0")) ) // retry with the maximun ps an vs
        {
            //for Leipzig conference 2006, defines for compile with maximun profile
            NLOG(resource, (LOG_SHADER | 0, "nD3D9Shader: ps_2_0 failed to load fx file '%s' with:\n\n%s\t retry with:%s\n",
                            mangledPath.Get(), errorBuffer ? errorBuffer->GetBufferPointer() : "No D3DX error message.", psProfile));
            hr = this->LoadEffect(file, fileSize, definesMaxProfile, mangledPath, &errorBuffer);

            NLOGCOND(resource, !FAILED(hr), (LOG_SHADER | 9, "nD3D9Shader: compile file '%s' with: %s",
                                             mangledPath.Get(),  psProfile));
        }

        if (FAILED(hr))
        {
            NLOG(resource, (LOG_SHADER | 0, "nD3D9Shader: failed to load fx file '%s' with:\n\n%s\n",
                 mangledPath.Get(), errorBuffer ? errorBuffer->GetBufferPointer() : "No D3DX error message."));

#if 0 // change to 1 for show assert when shader error and reload it
            static bool ignoreAlways = false;
            if (!ignoreAlways)
            {
                // if debugger is present close the file, so programmer can save it
                //if (IsDebuggerPresent())
                {
                    file->Close();

                    ignoreAlways = n_error("nD3D9Shader: failed to load fx file '%s' with:\n\n%s\n"
                        "Press SHIFT + Ignore to ignore always",
                        mangledPath.Get(),
                        errorBuffer ? errorBuffer->GetBufferPointer() : "No D3DX error message.");

                    // open the file
                    if (!file->Open(mangledPath.Get(), "r"))
                    {
                        NLOG(resource, (NLOGUSER | LOG_SHADER | 0, "nD3D9Shader: could not load shader file '%s'!", mangledPath.Get()));
                        return false;
                    }

                    // get size of file
                    fileSize = file->GetSize();

                    if (ignoreAlways)
                    {
                        retry = true;
                        ignoreAlways = false;
                    }
                }
            }
#endif // if 0
        }

        // free the buffer of compilation errors.
        if (errorBuffer)
        {
            errorBuffer->Release();
            errorBuffer = 0;
        }

    } while (retry);

    if (FAILED(hr))
    {
        return false;
    }

    n_assert(this->effect);

    // set state manager
    //this->effect->SetStateManager(effectStateManager);

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetState(Valid);

    // validate the effect
    this->ValidateEffect();

    return true;
}
//------------------------------------------------------------------------------
/**
*/
HRESULT
nD3D9Shader::LoadEffect(nFile *file, int fileSize, const D3DXMACRO *defines, const nString &mangledFileName, LPD3DXBUFFER *errorBuffer)
{
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert_return(d3d9Dev, D3DERR_INVALIDCALL);

    // get global effect pool from gfx server
    ID3DXEffectPool* effectPool = this->refGfxServer->GetEffectPool();
    n_assert_return(effectPool, D3DERR_INVALIDCALL);

    HRESULT hr;
    nString shaderPath(mangledFileName.Get());
    nD3D9ShaderInclude includeHandler(shaderPath.ExtractDirName());

#if N_D3D9_DEBUG
        DWORD compileFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;

        hr = D3DXCreateEffectFromFile(
                d3d9Dev,            // pDevice
                mangledFileName.Get(),  // file name
                defines,            // pDefines
                &includeHandler,    // pInclude
                compileFlags,       // Flags
                effectPool,         // pPool
                &(this->effect),    // ppEffect
                errorBuffer);       // ppCompilationErrors

#else
        // allocate data for file and read it
        void* buffer = n_malloc(fileSize);
        n_assert(buffer);
        file->Seek(0, nFile::START);
        file->Read(buffer, fileSize);

        DWORD compileFlags = D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;

        // create effect
        hr = D3DXCreateEffect(
                d3d9Dev,            // pDevice
                buffer,             // pFileData
                fileSize,           // DataSize
                defines,            // pDefines
                &includeHandler,    // pInclude
                compileFlags,       // Flags
                effectPool,         // pPool
                &(this->effect),    // ppEffect
                errorBuffer);       // ppCompilationErrors

        n_free(buffer);
#endif
    return hr;
}


//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetBool(nShaderState::Param p, bool val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    this->curParams.SetArg(p, nShaderArg(val));
    n_dxverify2( 
        this->effect->SetBool(this->parameterHandles[p], val), 
        "SetBool() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetBoolArray(nShaderState::Param p, const bool* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);

    // FIXME Floh: is the C++ bool datatype really identical to the Win32 BOOL datatype?
    n_dxverify2( 
        this->effect->SetBoolArray(this->parameterHandles[p], (const BOOL*) array, count),
        "SetBoolArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetInt(nShaderState::Param p, int val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (this->parameterSizes[p] == 0)
    {
        this->curParams.SetArg(p, nShaderArg(val));
        n_dxverify2( 
            this->effect->SetInt(this->parameterHandles[p], val),
            "SetInt() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        int *intArray = this->GetIntArray(p);
        n_assert_return(intArray,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        intArray[slotEntry.currentSlotIndex] = val;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetIntArray(nShaderState::Param p, const int* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2( 
        this->effect->SetIntArray(this->parameterHandles[p], array, count),
        "SetIntArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat(nShaderState::Param p, float val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (this->parameterSizes[p] == 0)
    {
        this->curParams.SetArg(p, nShaderArg(val));
        n_dxverify2( 
            this->effect->SetFloat(this->parameterHandles[p], val),
            "SetFloat() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        float *floatArray = this->GetFloatArray(p);
        n_assert_return(floatArray,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        floatArray[slotEntry.currentSlotIndex] = val;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloatArray(nShaderState::Param p, const float* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2(
        this->effect->SetFloatArray(this->parameterHandles[p], array, count),
        "SetFloatArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (this->parameterSizes[p] == 0)
    {
        //this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
        this->curParams.SetArg(p, val);
        n_dxverify2(
            this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val),
            "SetVector() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        nFloat4 *float4Array = this->GetFloat4Array(p);
        n_assert_return(float4Array,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        float4Array[slotEntry.currentSlotIndex] = *(nFloat4*)&val;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    static vector4 v;
    v.set(val.x, val.y, val.z, 1.0f);
    if (this->parameterSizes[p] == 0)
    {
        //this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&v));
        this->curParams.SetArg(p, v);
        n_dxverify2(
            this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &v),
            "SetVector() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        nFloat4 *float4Array = this->GetFloat4Array(p);
        n_assert_return(float4Array,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        float4Array[slotEntry.currentSlotIndex] = *(nFloat4*)&v;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (this->parameterSizes[p] == 0)
    {
        //this->curParams.SetArg(p, nShaderArg(val));
        this->curParams.SetArg(p, reinterpret_cast<const vector4 &> (val));
        n_dxverify2(
            this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val),
            "SetVector() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        nFloat4 *float4Array = this->GetFloat4Array(p);
        n_assert_return(float4Array,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        float4Array[slotEntry.currentSlotIndex] = val;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2(
        this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count),
        "SetVectorArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4Array(nShaderState::Param p, const vector4* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2(
        this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count),
        "SetVectorArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (this->parameterSizes[p] == 0)
    {
        //this->curParams.SetArg(p, nShaderArg(&val));
        this->curParams.SetArg(p, val);
        n_dxverify2(
            this->effect->SetMatrix(this->parameterHandles[p], (CONST D3DXMATRIX*) &val),
            "SetMatrix() on shader failed!" );
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
    }
    else if (this->inParameterSlot)
    {
        matrix44 *matrixArray = this->GetMatrixArray(p);
        n_assert_return(matrixArray,);
        n_assert_return(this->entryIndex[p] != -1,);
        SlotEntry& slotEntry = this->slotEntry[this->entryIndex[p]];
        slotEntry.assigned = true;
        matrixArray[slotEntry.currentSlotIndex] = val;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixArray(nShaderState::Param p, const matrix44* array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2(
        this->effect->SetMatrixArray(this->parameterHandles[p], (CONST D3DXMATRIX*) array, count),
        "SetMatrixArray() on shader failed!" );
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    n_dxverify2(
        this->effect->SetMatrixPointerArray(this->parameterHandles[p], (CONST D3DXMATRIX**) array, count),
        "SetMatrixPointerArray() on shader failed!");
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert_return(this->effect && (p < nShaderState::NumParameters),);
    if (0 == tex)
    {
        n_dxverify2(this->effect->SetTexture(this->parameterHandles[p], 0),
            "SetTexture(0) on shader failed!");
        this->curParams.SetArg(p, nShaderArg((nTexture2*)0));
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumTextureChanges++;
        #endif
    }
    else
    {
        uint curTexUniqueId = 0;
        if (this->curParams.IsParameterValid(p))
        {
            nTexture2* curTex = this->curParams.GetArg(p).GetTexture();
            if (curTex)
            {
                curTexUniqueId = curTex->GetUniqueId();
            }
        }

        if ((!this->curParams.IsParameterValid(p)) || (curTexUniqueId != tex->GetUniqueId()))
        {
            this->curParams.SetArg(p, nShaderArg(tex));
            N_IFDEF_ASSERTS(HRESULT hr = )this->effect->SetTexture(this->parameterHandles[p], ((nD3D9Texture*)tex)->GetBaseTexture());
            #ifdef __NEBULA_STATS__
            this->refGfxServer->statsNumTextureChanges++;
            #endif
            n_dxtrace(hr, "SetTexture() on shader failed!");
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set a whole shader parameter block at once. This is slightly faster
    (and more convenient) then setting single parameters.
*/
void
nD3D9Shader::SetParams(const nShaderParams& params)
{
    #ifdef __NEBULA_STATS__
    nD3D9Server* gfxServer = this->refGfxServer.get();
    #endif
    int i;
    HRESULT hr = S_OK;

    n_assert_return(this->effect,);
    int numValidParams = params.GetNumValidParams();
    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = params.GetParamByIndex(i);
        
        // parameter used in shader?
        D3DXHANDLE handle = this->parameterHandles[curParam];
        if (handle != 0)
        {
            const nShaderArg& curArg = params.GetArgByIndex(i);

            // early out if parameter is void
            if (curArg.GetType() == nShaderState::Void)
            {
                continue;
            }

            // avoid redundant state switches
            if ((!this->curParams.IsParameterValid(curParam)) ||
                (!(curArg == this->curParams.GetArg(curParam))))
            {
                if (this->parameterSizes[curParam] == 0)
                {
                    this->curParams.SetArg(curParam, curArg);
                    switch (curArg.GetType())
                    {
                    case nShaderState::Void:
                        hr = S_OK;
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;
                        
                    case nShaderState::Int:
                        hr = this->effect->SetInt(handle, curArg.GetInt());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;
                        
                    case nShaderState::Float:
                        hr = this->effect->SetFloat(handle, curArg.GetFloat());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;
                        
                    case nShaderState::Float4:
                        hr = this->effect->SetVector(handle, (CONST D3DXVECTOR4*) &(curArg.GetFloat4()));
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;
                        
                    case nShaderState::Matrix44:
                        hr = this->effect->SetMatrix(handle, (CONST D3DXMATRIX*) curArg.GetMatrix44());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;
                        
                    case nShaderState::Texture:
                        hr = this->effect->SetTexture(handle, ((nD3D9Texture*)curArg.GetTexture())->GetBaseTexture());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumTextureChanges++;
                        #endif
                        break;

                    default:
                        n_assert3_always(("nD3D9Shader::SetParams: unknown shader param type=%s(%d) name=%s", nShaderState::TypeToString(curArg.GetType()), curArg.GetType(), nShaderState::ParamToString(curParam) ));
                        break;
                    }
                    n_dxtrace(hr, "Failed to set shader parameter in nD3D9Shader::SetParams");
                }
                else if (this->inParameterSlot)
                {
                    switch (curArg.GetType())
                    {
                    case nShaderState::Void:
                    case nShaderState::Bool:
                        /// @todo implement the bool shader param
                        hr = this->effect->SetBool(handle, curArg.GetBool());
                        break;
                        
                    case nShaderState::Int:
                        {
                            int *intArray = this->GetIntArray(curParam);
                            n_assert_if(intArray)
                            {
                                SlotEntry& slotEntry = this->slotEntry[this->entryIndex[curParam]];
                                slotEntry.assigned = true;
                                intArray[slotEntry.currentSlotIndex] = curArg.GetInt();
                            }
                        }
                        break;
                        
                    case nShaderState::Float:
                        {
                            float *floatArray = this->GetFloatArray(curParam);
                            n_assert_if(floatArray)
                            {
                                SlotEntry& slotEntry = this->slotEntry[this->entryIndex[curParam]];
                                slotEntry.assigned = true;
                                floatArray[slotEntry.currentSlotIndex] = curArg.GetFloat();
                            }
                        }
                        break;
                        
                    case nShaderState::Float4:
                        {
                            nFloat4 *float4Array = this->GetFloat4Array(curParam);
                            n_assert_if(float4Array)
                            {
                                SlotEntry& slotEntry = this->slotEntry[this->entryIndex[curParam]];
                                slotEntry.assigned = true;
                                float4Array[slotEntry.currentSlotIndex] = curArg.GetFloat4();
                            }
                        }
                        break;
                        
                    case nShaderState::Matrix44:
                        {
                            matrix44 *matrixArray = this->GetMatrixArray(curParam);
                            n_assert_if(matrixArray)
                            {
                                SlotEntry& slotEntry = this->slotEntry[this->entryIndex[curParam]];
                                slotEntry.assigned = true;
                                matrixArray[slotEntry.currentSlotIndex] = *curArg.GetMatrix44();
                            }
                        }
                        break;

                    case nShaderState::Texture:
                        hr = this->effect->SetTexture(handle, ((nD3D9Texture*)curArg.GetTexture())->GetBaseTexture());
                        break;

                    default:
                        n_assert3_always(("nD3D9Shader::SetParams: unknown shader param type=%s(%d) name=%s", nShaderState::TypeToString(curArg.GetType()), curArg.GetType(), nShaderState::ParamToString(curParam) ));
                        break;
                    }
                }
                #ifdef __NEBULA_STATS__
                gfxServer->statsNumRenderStateChanges++;
                #endif
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    D3DXEffect parameter handles.

    - 19-Feb-04 floh    Now also recognized parameters which are not used
                        by the shader's current technique.
*/
void
nD3D9Shader::UpdateParameterHandles()
{
    n_assert_return(this->effect,);
    HRESULT hr;
    
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));
    memset(this->parameterSizes, 0, sizeof(this->parameterSizes));
    
    // for each parameter in the effect...
    D3DXEFFECT_DESC effectDesc = { 0 };
    hr = this->effect->GetDesc(&effectDesc);
    n_dxtrace(hr, "GetDesc() failed in UpdateParameterHandles()");
    uint curParamIndex;
    for (curParamIndex = 0; curParamIndex < effectDesc.Parameters; curParamIndex++)
    {
        D3DXHANDLE curParamHandle = this->effect->GetParameter(NULL, curParamIndex);
        n_assert(NULL != curParamHandle);
        
        // get the associated Nebula2 parameter index
        D3DXPARAMETER_DESC paramDesc = { 0 };
        hr = this->effect->GetParameterDesc(curParamHandle, &paramDesc);
        n_dxtrace(hr, "GetParameterDesc() failed in UpdateParameterHandles()");
        nShaderState::Param nebParam = nShaderState::StringToParam(paramDesc.Name);
        if (nebParam != nShaderState::InvalidParameter)
        {
            this->parameterHandles[nebParam] = curParamHandle;
            n_assert( paramDesc.Elements <= CHAR_MAX );
            this->parameterSizes[nebParam] = static_cast<char>( paramDesc.Elements );
            // TODO initialize an array of the proper type if necessary
            //if (paramDesc.Elements > 0) {
            //create a buffer depending on the type
            //add the buffer to the right array: intArray, floatArray, etc.
            //}
        }
    }
}

//------------------------------------------------------------------------------
/**
    Return true if parameter is used by effect.
*/
bool
nD3D9Shader::IsParameterUsed(nShaderState::Param p)
{
    n_assert_return(p < nShaderState::NumParameters,false);
    return (0 != this->parameterHandles[p]);
}

//------------------------------------------------------------------------------
/**
    Clear a parameter from the stored parameters.
*/
void 
nD3D9Shader::ClearParameter(nShaderState::Param p)
{
    if (this->curParams.IsParameterValid(p))
    {
        this->curParams.ClearArg(p);
    }
}

//------------------------------------------------------------------------------
/**
    Find the first valid technique and set it as current.
    This sets the hasBeenValidated and didNotValidate members
*/
void
nD3D9Shader::ValidateEffect()
{
    n_assert(!this->hasBeenValidated);
    n_assert_return(this->effect,);
    n_assert_return(this->refGfxServer->d3d9Device,);
    nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
    n_assert_return(gfxServer,);
    n_assert_return(gfxServer->d3d9Device,);
    HRESULT hr;

    // get current vertex processing state
    bool origSoftwareVertexProcessing = gfxServer->GetSoftwareVertexProcessing();

    // set to hardware vertex processing (this could fail if it's a pure software processing device)
    gfxServer->SetSoftwareVertexProcessing(false);

    static nString dbgstr;
    dbgstr.Format("nD3D9Shader::ValidateEffect(shader: '%s')\n", this->GetFilename().Get());
    N_OUTPUTDEBUGSTRING(dbgstr.Get());

    // set on first technique that validates correctly
    D3DXHANDLE technique = NULL;
    hr = this->effect->FindNextValidTechnique(0, &technique);

    // NOTE: DON'T change this to SUCCEEDED(), since FindNextValidTechnique() may
    // return S_FALSE, which the SUCCEEDED() macro interprets as a success code!
    if (D3D_OK == hr)
    {
        // technique could be validated
        D3DXTECHNIQUE_DESC desc;
        this->effect->GetTechniqueDesc(this->effect->GetTechnique(0), &desc);
        this->SetTechnique(desc.Name);
        this->hasBeenValidated = true;
        this->didNotValidate = false;
        this->UpdateParameterHandles();
    }
    else
    {
        // shader did not validate with hardware vertex processing, try with software vertex processing
        gfxServer->SetSoftwareVertexProcessing(true);
        hr = this->effect->FindNextValidTechnique(0, &technique);
        this->hasBeenValidated = true;

        // NOTE: DON'T change this to SUCCEEDED(), since FindNextValidTechnique() may
        // return S_FALSE, which the SUCCEEDED() macro interprets as a success code!
        if (D3D_OK == hr)
        {
            // success with software vertex processing
            n_printf("nD3D9Shader() info: shader '%s' needs software vertex processing\n",  this->GetFilename());
            D3DXTECHNIQUE_DESC desc;
            this->effect->GetTechniqueDesc(this->effect->GetTechnique(0), &desc);
            this->SetTechnique(desc.Name);
            this->didNotValidate = false;
            this->UpdateParameterHandles();
        }
        else
        {
            // NOTE: looks like this has been fixed in the April 2005 SDK...

            // shader didn't validate at all, this may happen although the shader is valid
            // on older nVidia cards if the effect has a vertex shader, thus we simply force 
            // the first technique in the file as crurent
            n_printf("nD3D9Shader() warning: shader '%s' did not validate!\n", this->GetFilename());

            // NOTE: this works around the dangling "BeginPass()" in D3DX when a shader did
            // not validate (reproducible on older nVidia cards)
            this->effect->EndPass();
            D3DXTECHNIQUE_DESC desc;
            this->effect->GetTechniqueDesc(this->effect->GetTechnique(0), &desc);
            this->SetTechnique(desc.Name);
            this->didNotValidate = false;
            this->UpdateParameterHandles();
        }
    }

    // restore original software processing mode
    gfxServer->SetSoftwareVertexProcessing(origSoftwareVertexProcessing);

    N_OUTPUTDEBUGSTRING("nD3D9Shader::ValidateShader(finished)\n");
}

//------------------------------------------------------------------------------
/**
    This switches between hardware and software processing mode, as needed
    by this shader.
*/
void
nD3D9Shader::SetVertexProcessingMode()
{
    nD3D9Server* d3d9Server = (nD3D9Server*) nGfxServer2::Instance();
    d3d9Server->SetSoftwareVertexProcessing(this->curTechniqueNeedsSoftwareVertexProcessing);
}

//------------------------------------------------------------------------------
/**
    05-Jun-04   floh    saveState parameter
    26-Sep-04   floh    I misread the save state docs for DX9.0c, state saving
                        flags now correct again
*/
int
nD3D9Shader::Begin(bool saveState)
{
    n_assert(this->effect);

    // check if we already have been validated, if not, find the first
    // valid technique and set it as current
    if (!this->hasBeenValidated)
    {
        this->ValidateEffect();
    }

    if (this->didNotValidate)
    {
        return 0;
    }
    else
    {
        static nString dbgstr;
        dbgstr.Format("nD3D9Shader::Begin(shader: '%s', saveState: %s)\n", this->GetFilename().Get(), saveState ? "true" : "false");
        N_OUTPUTDEBUGSTRING(dbgstr.Get());

        // start rendering the effect
        UINT numPasses;
        DWORD flags;
        if (saveState) 
        {
            // save all state
            flags = 0;
        }
        else
        {
            // save no state
            flags = D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESAMPLERSTATE | D3DXFX_DONOTSAVESHADERSTATE;
        }
        this->SetVertexProcessingMode();
        n_dxverify2( 
            this->effect->Begin(&numPasses, flags),
            "nD3D9Shader: Begin() failed on effect" );

        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumShaderChanges++;
        #endif
        return numPasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::BeginPass(int pass)
{
    n_assert_return(this->effect,);
    n_assert(this->hasBeenValidated && !this->didNotValidate);

    static nString dbgstr;
    dbgstr.Format("nD3D9Shader::BeginPass(shader: '%s', curPass: %d)\n", this->GetFilename().Get(), pass);
    N_OUTPUTDEBUGSTRING(dbgstr.Get());

    this->SetVertexProcessingMode();
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    n_dxverify2( 
        this->effect->BeginPass(pass),
        "nD3D9Shader:BeginPass() failed on effect" );
#else
    n_dxverify2( 
        this->effect->Pass(pass),
        "nD3D9Shader: Pass() failed on effect" );
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::CommitChanges()
{
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    n_assert_return(this->effect,);
    n_assert(this->hasBeenValidated && !this->didNotValidate);

    this->SetVertexProcessingMode();
    n_dxverify2(this->effect->CommitChanges(),
                "nD3D9Shader: CommitChanges() failed on effect");
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::EndPass()
{
    static nString dbgstr;
    dbgstr.Format("nD3D9Shader::EndPass(shader: '%s')\n", this->GetName());
    N_OUTPUTDEBUGSTRING(dbgstr.Get());

#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    n_assert_return(this->effect,);
    n_assert(this->hasBeenValidated && !this->didNotValidate);
    
    this->SetVertexProcessingMode();
    n_dxverify2(this->effect->EndPass(),
                "nD3D9Shader: EndPass() failed on effect");
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::End()
{
    HRESULT hr;
    n_assert_return(this->effect,);
    n_assert(this->hasBeenValidated);

    static nString dbgstr;
    dbgstr.Format("nD3D9Shader::End(shader: '%s')\n", this->GetFilename().Get());
    N_OUTPUTDEBUGSTRING(dbgstr.Get());

    if (!this->didNotValidate)
    {
        this->SetVertexProcessingMode();
        hr = this->effect->End();
        n_dxtrace(hr, "End() failed on effect");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Shader::HasTechnique(const char* t) const
{
    n_assert_return(t,false);
    n_assert_return(this->effect,false);
    D3DXHANDLE h = this->effect->GetTechniqueByName(t);
    return (0 != h);
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Shader::SetTechnique(const char* t)
{
    n_assert_return(t,false);
    n_assert_return(this->effect,false);
    
    // get handle to technique
    D3DXHANDLE hTechnique = this->effect->GetTechniqueByName(t);
    if (0 == hTechnique)
    {
        n_error("nD3D9Shader::SetTechnique(%s): technique not found in shader file %s!\n", t, this->GetFilename());
        return false;
    }

    // check if technique needs software vertex processing (this is the
    // case if the 3d device is a mixed vertex processing device, and 
    // the current technique includes a vertex shader
    this->curTechniqueNeedsSoftwareVertexProcessing = false;
    if (nGfxServer2::Instance()->AreVertexShadersEmulated())
    {
        D3DXHANDLE hPass = this->effect->GetPass(hTechnique, 0);
        n_assert_return(0 != hPass,false);
        D3DXPASS_DESC passDesc = { 0 };
        N_IFDEF_ASSERTS(HRESULT hr = )this->effect->GetPassDesc(hPass, &passDesc);
        n_assert(SUCCEEDED(hr));
        if (passDesc.pVertexShaderFunction)
        {
            this->curTechniqueNeedsSoftwareVertexProcessing = true;
        }
    }

    // finally, set the technique
    HRESULT hr = this->effect->SetTechnique(hTechnique);
    if (FAILED(hr))
    {
        n_printf("nD3D9Shader::SetTechnique(%s) on shader %s failed!\n", t, this->GetFilename());
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nD3D9Shader::GetTechnique() const
{
    n_assert_return(this->effect,0);
    return this->effect->GetCurrentTechnique();
}

//------------------------------------------------------------------------------
/**
    This converts a D3DX parameter handle to a nShaderState::Param.
*/
nShaderState::Param
nD3D9Shader::D3DXParamToShaderStateParam(D3DXHANDLE h)
{
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        if (this->parameterHandles[i] == h)
        {
            return (nShaderState::Param) i;
        }
    }
    // fallthrough: invalid handle
    return nShaderState::InvalidParameter;;
}

//------------------------------------------------------------------------------
/**
    Create or update the instance stream declaration for this shader.
    Stream components will be appended, unless they already exist in the
    declaration. Returns the number of components appended.
*/
int
nD3D9Shader::UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl)
{
    n_assert_return(this->effect,0);

    int numAppended = 0;

    HRESULT hr;
    D3DXEFFECT_DESC fxDesc;
    hr = this->effect->GetDesc(&fxDesc);
    n_dxtrace(hr, "GetDesc() failed on effect");
    
    // for each parameter...
    uint paramIndex;
    for (paramIndex = 0; paramIndex < fxDesc.Parameters; paramIndex++)
    {
        D3DXHANDLE paramHandle = this->effect->GetParameter(NULL, paramIndex);
        n_assert(0 != paramHandle);

        D3DXHANDLE annHandle = this->effect->GetAnnotationByName(paramHandle, "Instance");
        if (annHandle)
        {
            BOOL b;
            hr = this->effect->GetBool(annHandle, &b);
            n_dxtrace(hr, 0);
            if (b)
            {
                // add parameter to stream declaration (if not already exists)
                nShaderState::Param param = this->D3DXParamToShaderStateParam(paramHandle);
                n_assert(nShaderState::InvalidParameter != param);

                // get parameter type
                D3DXPARAMETER_DESC paramDesc;
                hr = this->effect->GetParameterDesc(paramHandle, &paramDesc);
                n_dxtrace(hr, 0);
                nShaderState::Type type = nShaderState::Void;
                if (paramDesc.Type == D3DXPT_FLOAT)
                {
                    switch (paramDesc.Class)
                    {
                    case D3DXPC_SCALAR:         
                        type = nShaderState::Float; 
                        break;

                    case D3DXPC_VECTOR:         
                        type = nShaderState::Float4; 
                        break;

                    case D3DXPC_MATRIX_ROWS:
                    case D3DXPC_MATRIX_COLUMNS:
                        type = nShaderState::Matrix44;
                        break;

                    default:
                        n_assert2_always("nD3D9Shader::UpdateInstanceStreamDecl: invalid paramDesc.Class");
                    }
                }
                if (nShaderState::Void == type)
                {
                    n_error("nShader2: Invalid data type for instance parameter '%s' in shader '%s'!",
                        paramDesc.Name, this->GetFilename());
                    return 0;
                }
                
                // append instance stream component (if not exists yet)
                int i;
                bool paramExists = false;
                for (i = 0; i < decl.Size(); i++)
                {
                    if (decl[i].GetParam() == param)
                    {
                        paramExists = true;
                        break;
                    }
                }
                if (!paramExists)
                {
                    nInstanceStream::Component streamComponent(type, param);
                    decl.Append(streamComponent);
                    numAppended++;
                }
            }
        }
    }
    return numAppended;
}

//------------------------------------------------------------------------------
/**
    Reset parameter slots.
    Should be called before passing slot parameters to the shader
    in order to reset all slots in the array parameters.
*/
void
nD3D9Shader::ResetParameterSlots()
{
    nShader2::ResetParameterSlots();

    int i;
    for (i = 0; i < this->slotEntry.Size(); i++)
    {
        SlotEntry& slotEntry = this->slotEntry[i];
        slotEntry.assigned = false;
        slotEntry.currentSlotIndex = 0;
    }
}

//------------------------------------------------------------------------------
/**
    begin a parameter slot
*/
bool
nD3D9Shader::BeginParameterSlot()
{
    return nShader2::BeginParameterSlot();
}

//------------------------------------------------------------------------------
/**
    end parameter slot
*/
void
nD3D9Shader::EndParameterSlot()
{
    nShader2::EndParameterSlot();

    int index;
    for (index = 0; index < this->slotEntry.Size(); index++)
    {
        SlotEntry& entry = this->slotEntry[index];
        if (entry.assigned)
        {
            // update slot index for all assigned parameters
            entry.assigned = false;
            char arrayIndex = entry.arrayIndex;
            ++entry.currentSlotIndex;
            
            // retrieve and assign array parameter
            if (entry.currentSlotIndex == this->parameterSizes[entry.param])
            {
                switch (entry.type)
                {
                case nShaderState::Void:                    
                    break;

                case nShaderState::Int:
                    this->SetIntArray(entry.param, this->intArrays[arrayIndex].Begin(), entry.currentSlotIndex);
                    break;
                    
                case nShaderState::Float:
                    this->SetFloatArray(entry.param, this->floatArrays[arrayIndex].Begin(), entry.currentSlotIndex);
                    break;
                    
                case nShaderState::Float4:
                    this->SetFloat4Array(entry.param, this->float4Arrays[arrayIndex].Begin(), entry.currentSlotIndex);
                    break;
                    
                case nShaderState::Matrix44:
                    this->SetMatrixArray(entry.param, this->matrixArrays[arrayIndex].Begin(), entry.currentSlotIndex);
                    break;
                    
                default:
                    n_assert3_always(("nD3D9Shader::EndParameterSlot: unknown shader param type=%s(%d) name=%s", nShaderState::TypeToString(entry.type), entry.type, nShaderState::ParamToString(entry.param) ));
                    break;
                }
                
                entry.currentSlotIndex = 0;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    flush all assigned parameter slots.
    this should be used with parameters that is safe to flush
    to the shader with only partial contents -eg. instanced attributes
*/
void
nD3D9Shader::FlushParameterSlots()
{
    nShader2::FlushParameterSlots();

    int i;
    for (i = 0; i < this->slotEntry.Size(); i++)
    {
        SlotEntry& entry = this->slotEntry[i];
        if (entry.currentSlotIndex > 0)
        {
            // flush assigned slots for all assigned parameters
            char arrayIndex = entry.arrayIndex;

            switch (entry.type)
            {
            case nShaderState::Void:
                break;
                
            case nShaderState::Int:
                this->SetIntArray(entry.param, this->intArrays[arrayIndex].Begin(), this->intArrays[arrayIndex].Size());
                break;
                
            case nShaderState::Float:
                this->SetFloatArray(entry.param, this->floatArrays[arrayIndex].Begin(), this->floatArrays[arrayIndex].Size());
                break;

            case nShaderState::Float4:
                this->SetFloat4Array(entry.param, this->float4Arrays[arrayIndex].Begin(), this->float4Arrays[arrayIndex].Size());
                break;

            case nShaderState::Matrix44:
                this->SetMatrixArray(entry.param, this->matrixArrays[arrayIndex].Begin(), this->matrixArrays[arrayIndex].Size());
                break;

            default:
                n_assert3_always(("nD3D9Shader::FlushParameterSlots: unknown shader param type=%s(%d) name=%s", nShaderState::TypeToString(entry.type), entry.type, nShaderState::ParamToString(entry.param) ));
                break;
            }

            entry.assigned = false;
            entry.currentSlotIndex = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Retrieve the float4 array for the specified parameter,
    or create a new one if none exists.
*/
nFloat4*
nD3D9Shader::GetFloat4Array(nShaderState::Param param)
{
    int arrayIndex;
    int index = this->entryIndex[param];
    if (index == -1)
    {
        // push a new array entry
        this->float4Arrays.PushBack(nArray<nFloat4>(0, 0)).SetFixedSize(this->parameterSizes[param]);
        arrayIndex = this->float4Arrays.Size() - 1;
        
        // push a new float4 array
        n_assert_return(arrayIndex <= CHAR_MAX , 0);
        SlotEntry newSlotEntry(param, nShaderState::Float4, static_cast<char>(arrayIndex));
        this->slotEntry.Append(newSlotEntry);
        this->entryIndex[param] = this->slotEntry.Size() - 1;
    }
    else
    {
        arrayIndex = this->slotEntry[index].arrayIndex;
    }
    return this->float4Arrays[arrayIndex].Begin();
}

//------------------------------------------------------------------------------
/**
    Retrieve the float4 array for the specified parameter,
    or create a new one if none exists.
*/
float*
nD3D9Shader::GetFloatArray(nShaderState::Param param)
{
    int arrayIndex;
    int index = this->entryIndex[param];
    if (index == -1)
    {
        // push a new array entry
        this->floatArrays.PushBack(nArray<float>(0, 0)).SetFixedSize(this->parameterSizes[param]);
        arrayIndex = this->floatArrays.Size() - 1;
        
        // push a new float4 array
        n_assert_return(arrayIndex <= CHAR_MAX , 0);
        SlotEntry newSlotEntry(param, nShaderState::Float, static_cast<char>(arrayIndex));
        this->slotEntry.Append(newSlotEntry);
        this->entryIndex[param] = this->slotEntry.Size() - 1;
    }
    else
    {
        arrayIndex = this->slotEntry[index].arrayIndex;
    }
    return this->floatArrays[arrayIndex].Begin();
}

//------------------------------------------------------------------------------
/**
    Retrieve the float4 array for the specified parameter,
    or create a new one if none exists.
*/
int*
nD3D9Shader::GetIntArray(nShaderState::Param param)
{
    int arrayIndex;
    int index = this->entryIndex[param];
    if (index == -1)
    {
        // push a new array entry
        this->intArrays.PushBack(nArray<int>(0, 0)).SetFixedSize(this->parameterSizes[param]);
        arrayIndex = this->intArrays.Size() - 1;
        
        // push a new float4 array
        n_assert_return(arrayIndex <= CHAR_MAX, 0);
        SlotEntry newSlotEntry(param, nShaderState::Int, static_cast<char>(arrayIndex));
        this->slotEntry.Append(newSlotEntry);
        this->entryIndex[param] = this->slotEntry.Size() - 1;
    }
    else
    {
        arrayIndex = this->slotEntry[index].arrayIndex;
    }
    return this->intArrays[arrayIndex].Begin();
}


//------------------------------------------------------------------------------
/**
    Retrieve the matrix array for the specified parameter,
    or create a new one if none exists.
*/
matrix44 *
nD3D9Shader::GetMatrixArray(nShaderState::Param param)
{
    int arrayIndex;
    int index = this->entryIndex[param];
    if (index == -1)
    {
        // push a new array entry
        nArray<matrix44> newArray(this->parameterSizes[param], 1);
        this->matrixArrays.Append(newArray);
        arrayIndex = this->matrixArrays.Size() - 1;
        
        // push a new matrix array
        n_assert_return(arrayIndex <= CHAR_MAX , 0);
        SlotEntry newSlotEntry(param, nShaderState::Matrix44, static_cast<char>(arrayIndex));
        this->slotEntry.Append(newSlotEntry);
        this->entryIndex[param] = this->slotEntry.Size() - 1;
    }
    else
    {
        arrayIndex = this->slotEntry[index].arrayIndex;
    }
    return this->matrixArrays[arrayIndex].Begin();
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is lost.
*/
void
nD3D9Shader::OnLost()
{
    n_assert(Lost != this->GetState());
    if (this->effect)
    {
        this->effect->OnLostDevice();
        this->SetState(Lost);
    }

    // flush my current parameters (important! otherwise, seemingly redundant
    // state will not be set after OnRestore())!
    this->curParams.Clear();
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device has been restored.
*/
void
nD3D9Shader::OnRestored()
{
    n_assert(Lost == this->GetState());
    n_assert_return(this->effect,);
    this->effect->OnResetDevice();
    this->SetState(Valid);
}


#ifndef __NEBULA_NO_LOG__
//------------------------------------------------------------------------------
/**
    Send current shader parameters to output log
*/
void 
nD3D9Shader::LogShaderParams() const
{
    this->curParams.Log();
}
#endif
