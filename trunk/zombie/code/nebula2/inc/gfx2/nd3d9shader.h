#ifndef N_D3D9SHADER_H
#define N_D3D9SHADER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Shader
    @ingroup Gfx2

    A nShader2 subclass using the D3D9FX framework.

    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"
#include <d3dx9.h>

class nD3D9Server;

//------------------------------------------------------------------------------
class nD3D9Shader : public nShader2
{
public:
    /// constructor
    nD3D9Shader();
    /// destructor
    virtual ~nD3D9Shader();
    /// create or append an instance stream declaration for this shader
    virtual int UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl);
    /// return true if technique exists in shader
    virtual bool HasTechnique(const char* t) const;
    /// set a technique
    virtual bool SetTechnique(const char* t);
    /// get current technique
    virtual const char* GetTechnique() const;
    /// is parameter used by effect?
    virtual bool IsParameterUsed(nShaderState::Param p);
    /// set bool parameter
    virtual void SetBool(nShaderState::Param p, bool val);
    /// set int parameter
    virtual void SetInt(nShaderState::Param p, int val);
    /// set float parameter
    virtual void SetFloat(nShaderState::Param p, float val);
    /// set vector4 parameter
    virtual void SetVector4(nShaderState::Param p, const vector4& val);
    /// set vector3 parameter
    virtual void SetVector3(nShaderState::Param p, const vector3& val);
    /// set float4 parameter
    virtual void SetFloat4(nShaderState::Param p, const nFloat4& val);
    /// set matrix parameter
    virtual void SetMatrix(nShaderState::Param p, const matrix44& val);
    /// set texture parameter
    virtual void SetTexture(nShaderState::Param p, nTexture2* tex);
    /// clear parameter
    virtual void ClearParameter(nShaderState::Param p);

    /// set bool[] parameter
    virtual void SetBoolArray(nShaderState::Param p, const bool* array, int count);
    /// set int[] parameter
    virtual void SetIntArray(nShaderState::Param p, const int* array, int count);
    /// set float[] parameter
    virtual void SetFloatArray(nShaderState::Param p, const float* array, int count);
    /// set vector[] parameter
    virtual void SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count);
    /// set vector4[] parameter
    virtual void SetVector4Array(nShaderState::Param p, const vector4* array, int count);
    /// set matrix array parameter
    virtual void SetMatrixArray(nShaderState::Param p, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count);
    /// set a whole shader parameter block at once
    virtual void SetParams(const nShaderParams& params);
    
    /// reset parameter slots
    virtual void ResetParameterSlots();
    /// begin a parameter slot
    virtual bool BeginParameterSlot();
    /// end parameter slot
    virtual void EndParameterSlot();
    /// flush parameter slots
    virtual void FlushParameterSlots();
    
    /// begin applying the shader, returns number of passes
    virtual int Begin(bool saveState);
    /// begin a pass
    virtual void BeginPass(int pass);
    /// commit changes during pass before rendering
    virtual void CommitChanges();
    /// end a pass
    virtual void EndPass();
    /// finish applying the shader
    virtual void End();

    /// loads the resource
    virtual bool Load();
    /// loads the resource from within an open file
    virtual bool Load(nFile* file, int offset, int length);

    /// signal hook to catch reloaded textures
    void OnResourceReloaded(nResource* rsrc);

#ifndef __NEBULA_NO_LOG__
    /// log shader parameters
    virtual void LogShaderParams() const;
#endif    

protected:
    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();
    /// called when contained resource may become lost 
    virtual void OnLost();
    /// called when contained resource may be restored
    virtual void OnRestored();
    /// load the effect, wrapper of directx load effect
    HRESULT LoadEffect(nFile *file,int fileSize,const D3DXMACRO* defines,const nString& mangledFileName,LPD3DXBUFFER * ppCompilationErrors);
    /// find the first valid technique and make current
    void ValidateEffect();
    /// update the parameter handle mapper table
    void UpdateParameterHandles();
    /// convert a D3DX parameter handle to a nShaderState parameter
    nShaderState::Param D3DXParamToShaderStateParam(D3DXHANDLE h);
    
    /// get the float4 array for collectable parameters
    nFloat4 *GetFloat4Array(nShaderState::Param p);
    /// get the matrix array for collectable parameters
    matrix44 *GetMatrixArray(nShaderState::Param p);
    /// get the int array for collectable parameters
    int *GetIntArray(nShaderState::Param p);
    /// get the float array for collectable parameters
    float *GetFloatArray(nShaderState::Param p);

    
    /// switch to hardware/software processing as needed by this shader
    void SetVertexProcessingMode();

    friend class nD3D9Server;
    
    nAutoRef<nD3D9Server> refGfxServer;
    ID3DXEffect* effect;
    bool hasBeenValidated;
    bool didNotValidate;
    bool inBeginPass;
    bool curTechniqueNeedsSoftwareVertexProcessing;
    D3DXHANDLE parameterHandles[nShaderState::NumParameters]; // map shader states to D3DX handles
    nShaderParams curParams;        // mirrored to avoid redundant parameters setting
    
    class SlotEntry
    {
    public:
        /// default constructor
        SlotEntry();
        /// other constructor
        SlotEntry(const nShaderState::Param p, const nShaderState::Type t, const char i);
        
        nShaderState::Param param;  // shader parameter
        nShaderState::Type type;    // parameter type
        char currentSlotIndex;      // index for current slot
        char arrayIndex;            // index into <type>Array, -1 for none
        bool assigned;              // parameter has been assigned in the slot
    };
    char parameterSizes[nShaderState::NumParameters];       // size of array parameters
    int entryIndex[nShaderState::NumParameters];            // indices of array entry, -1 for none
    nArray<SlotEntry> slotEntry;

    nArray<nArray<nFloat4> >  float4Arrays; // list of array, each shader param has a array of float4
    nArray<nArray<matrix44> > matrixArrays;
    nArray<nArray<float> >    floatArrays;
    nArray<nArray<int> >      intArrays;
};

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Shader::SlotEntry::SlotEntry() :
    param(nShaderState::InvalidParameter),
    type(nShaderState::Void),
    currentSlotIndex(0),
    arrayIndex(-1),
    assigned(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9Shader::SlotEntry::SlotEntry(const nShaderState::Param p, const nShaderState::Type t, const char i) :
    param(p),
    type(t),
    arrayIndex(i),
    currentSlotIndex(0),
    assigned(false)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
