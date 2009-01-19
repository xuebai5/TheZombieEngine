#ifndef NC_SCENECLASS_H
#define NC_SCENECLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "util/nstring.h"
#include "gfx2/ninstancestream.h"
#include "gfx2/ntexturearray.h"
#include "nscene/nscenenode.h"
#include "animcomp/ndynamicattachment.h"

//------------------------------------------------------------------------------
class ncSceneClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSceneClass,nComponentClass);

public:
    enum Flags
    {
        LightmapStatic   = (1<<0),
        LightmapReceiver = (1<<1),
        AlwaysOnTop      = (1<<2),
    };

    /// constructor
    ncSceneClass();
    /// destructor
    virtual ~ncSceneClass();
    /// Initialize the component
    virtual void InitInstance(nObject::InitInstanceMsg initType);
    /// load class resources
    virtual bool LoadResources();
    /// unload class resources
    virtual void UnloadResources();
    /// return true if valid (if root node is set)
    virtual bool IsValid();
    /// invalidate the render context
    virtual void Invalidate();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// get pass enabled flags
    const int GetPassEnabledFlags() const;

    /// get plugs
    const nArray<nDynamicAttachment>& GetPlugs() const;
    /// get a particular plug
    nDynamicAttachment GetPlug(int plugIdx) const;

    /// gets plug data if plugName found in plug array
    bool GetPlugData( const nString& plugName, vector3& position, quaternion& rotation);

    /// access to shader parameter overrides
    nShaderParams& GetShaderOverrides();

    //NSIGNAL_DECLARE('XXXX', void, xxx, 0, (), 0, ())

    /// set the scene root node
    void SetRootNode(nSceneNode*);
    /// get the scene root node
    nSceneNode* GetRootNode() const;

    /// begin plugs
    void BeginPlugs(int);
    /// add plug
    void AddPlug(int, char*, vector3, quaternion);
    /// end plugs
    void EndPlugs();

    /// set if a pass is enabled by default
    void SetPassEnabled(const char *, bool);
    /// get if a pass is enabled by default
    bool GetPassEnabled(const char *);

    /// set max material level to use
    void SetMaxMaterialLevel(int);
    /// get max material level to use
    int GetMaxMaterialLevel() const;

    /// set if geometry is statically lightmapped
    void SetLightmapStatic(bool);
    /// get if geometry is statically lightmapped
    bool GetLightmapStatic() const;
    /// set if geometry receives static lightmaps
    void SetLightmapReceiver(bool);
    /// get if geometry receives static lightmaps
    bool GetLightmapReceiver() const;
    /// set if geometry is drawn on top
    void SetAlwaysOnTop(bool);
    /// get if geometry is drawn on top
    bool GetAlwaysOnTop() const;

    /// set vector shader override
    void SetVectorOverride(const char *, vector4);
    /// get vector shader override
    vector4 GetVectorOverride(const char *);
    /// set float shader override
    void SetFloatOverride(const char *, float);
    /// get float shader override
    float GetFloatOverride(const char *);
    /// set int shader override
    void SetIntOverride(const char *, int);
    /// get int shader override
    int GetIntOverride(const char *);
    /// set texture override
    void SetTextureOverride(const char *, const char *);
    /// get texture override
    const char * GetTextureOverride(const char *);

    /// set if geometry instancing is enabled
    void SetInstanceEnabled(bool);
    /// get if geometry instancing is enabled
    bool GetInstanceEnabled();
    /// begin instance stream declaration
    void BeginInstanceStreamDecl();
    /// add instance stream component
    void AddInstanceStreamComp(const char *, const char *);
    /// end instance stream declaration
    void EndInstanceStreamDecl();

    #ifndef NGAME
    /// timestamp of last edition to refresh objects
    void SetLastEditedTime(nTime);
    nTime GetLastEditedTime();
    /// access the particle scene node for edition
    void SetParticleNodeDummy(nObject *);
    nObject *GetParticleNodeDummy() const;
    /// interface for inspectors to set the MinDist, MaxDist fade distances
    void SetFadeDistances(float, float);
    void GetFadeDistances(float&, float&);
    #endif

protected:
    nRef<nSceneNode> refRootNode;
    nArray<nDynamicAttachment> plugArray;

    nShaderParams shaderOverrides;
    nTextureArray textureArray;

    int geometryFlags;
    int passEnabledFlags;
    int maxMaterialLevel;

    bool instanceEnabled;
    bool inBeginInstanceStreamDecl;
    nInstanceStream::Declaration decl;
    //nRef<nInstanceStream> refInstanceStream;

    #ifndef NGAME
    nTime lastEditedTime;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncSceneClass::IsValid()
{
    return this->refRootNode.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::Invalidate()
{
    n_assert(this->refRootNode.isvalid());
    this->refRootNode.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::SetRootNode(nSceneNode* node)
{
    n_assert(node);
    this->refRootNode = node;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneNode*
ncSceneClass::GetRootNode() const
{
    return this->refRootNode.isvalid() ? this->refRootNode.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
ncSceneClass::GetPassEnabledFlags() const
{
    return this->passEnabledFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::BeginPlugs(int numPlugs)
{
    this->plugArray.SetFixedSize(numPlugs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::AddPlug(int plugIndex, char *plugName, vector3 pos, quaternion quat)
{
    this->plugArray[plugIndex] = nDynamicAttachment(nString(plugName), -1, pos, quat);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::EndPlugs()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nDynamicAttachment>&
ncSceneClass::GetPlugs() const
{
    return this->plugArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicAttachment
ncSceneClass::GetPlug(int plugIdx) const
{
    return this->plugArray[plugIdx];
}

//------------------------------------------------------------------------------
/**
*/
inline 
nShaderParams&
ncSceneClass::GetShaderOverrides()
{
    return this->shaderOverrides;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncSceneClass::SetLightmapStatic(bool value)
{
    if (value)
    {
        this->geometryFlags |= LightmapStatic;
    }
    else
    {
        this->geometryFlags &= ~LightmapStatic;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
ncSceneClass::GetLightmapStatic() const
{
    return (this->geometryFlags & LightmapStatic) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncSceneClass::SetLightmapReceiver(bool value)
{
    if (value)
    {
        this->geometryFlags |= LightmapReceiver;
    }
    else
    {
        this->geometryFlags &= ~LightmapReceiver;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
ncSceneClass::GetLightmapReceiver() const
{
    return (this->geometryFlags & LightmapReceiver) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneClass::SetAlwaysOnTop(bool value)
{
    if (value)
    {
        this->geometryFlags |= AlwaysOnTop;
    }
    else
    {
        this->geometryFlags &= ~AlwaysOnTop;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
ncSceneClass::GetAlwaysOnTop() const
{
    return (this->geometryFlags & AlwaysOnTop) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncSceneClass::SetTextureOverride(const char *paramName, const char* texName)
{
    nShaderState::Param param = nShaderState::StringToParam(paramName);
    this->textureArray.SetTexture(param, texName);
    #ifndef NGAME
    this->SetLastEditedTime(nTimeServer::Instance()->GetFrameTime());
    #endif //NGAME
}

//------------------------------------------------------------------------------
/**
*/
inline 
const char *
ncSceneClass::GetTextureOverride(const char *paramName)
{
    nShaderState::Param param = nShaderState::StringToParam(paramName);
    return this->textureArray.GetTexture(param);
}

//------------------------------------------------------------------------------
#endif
