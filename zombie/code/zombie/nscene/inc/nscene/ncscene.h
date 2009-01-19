#ifndef NC_SCENE_H
#define NC_SCENE_H
//------------------------------------------------------------------------------
/**
    @class ncScene
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for entities with levels of detail.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "variable/nvariable.h"
#include "util/nstring.h"
#include "gfx2/nshaderparams.h"
#include "mathlib/bbox.h"
#include "animcomp/ndynamicattachment.h"

class nSceneGraph;
class ncTransform;
class nSceneNode;
class ncSceneClass;

//------------------------------------------------------------------------------
class ncScene : public nComponentObject
{

    NCOMPONENT_DECLARE(ncScene,nComponentObject);

public:
    /// flags
    enum Flag
    {
        ShapeVisible     = (1<<0),
        ShadowVisible    = (1<<1),
        Occluded         = (1<<2),
        CastShadows      = (1<<3),
        DoOcclusionQuery = (1<<4),
        AlwaysOnTop      = (1<<5),

        AllFlags = (ShapeVisible|ShadowVisible|CastShadows),
        NoFlags = 0,
    };

    /// constructor
    ncScene();
    /// destructor
    virtual ~ncScene();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);
    /// component initialization
    void InitInstance(nObject::InitInstanceMsg initType);

    /// load the graphic object
    virtual bool Load();
    /// unload the graphic object
    virtual void Unload();
    /// render the object
    virtual void Render(nSceneGraph *sceneGraph);

    /// update plugs
    void Update(const matrix44& transform);

    /// get valid
    virtual bool IsValid();
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// set frameid
    void SetFrameId(const uint frameId);
    /// get frameid
    const uint GetFrameId();
    /// get local bounding box
    const bbox3& GetLocalBox();

    /// set index for attach
    void SetAttachIndex(const int index);
    /// get index for attach 
    const int GetAttachIndex();
    /// set flags for attach
    void SetAttachFlags(const int flags);
    /// get flags for attach 
    const int GetAttachFlags();
    /// set visibility flag
    void SetFlag(int flagMask, bool value);
    /// get visibility flag
    bool GetFlag(int flagMask);

    /// get root node
    nSceneNode *GetRootNode();
    /// access to shader parameter overrides
    nShaderParams& GetShaderOverrides();
    /// get pass enabled flags
    void SetPassEnabledFlags(const int passFlags);
    /// get pass enabled flags
    int GetPassEnabledFlags() const;
    /// set max material level
    void SetMaxMaterialLevel(const int index);
    /// get max material level
    int GetMaxMaterialLevel() const;
    /// appends a new var to localVarArray, returns the index
    int AddLocalVar(const nVariable& value);
    /// returns local variable at given index
    nVariable& GetLocalVar(int index);
    /// clear local variables
    void ClearLocalVars();
    /// find local variable by variable handle
    nVariable* FindLocalVar(nVariable::Handle handle);

    /// reset the link array
    void ClearLinks();
    /// add a link to another render context
    bool AddLink(nEntityObject* link);
    /// remove a link to another render context
    void RemoveLink(nEntityObject* link);
    /// get the number of links
    int GetNumLinks() const;
    /// get linked render context at index
    nEntityObject* GetLinkAt(int index) const;
    /// get linked render context at index, with cell id
    nEntityObject* GetLinkAt(int index, int& cellId) const;
    /// get parent entity
    nEntityObject* GetParentEntity();
    /// set parent entity
    void SetParentEntity(nEntityObject* entity);

    /// update transform if attached to a character
    void UpdateParentTransform();

    /// get world plug posititon and rotation
    void GetWorldPlugData( const nString& plugName, vector3& pos, quaternion& rotation);

    //NSIGNAL_DECLARE('XXXX', void, xxx, 0, (), 0, ())

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

    /// plug
    void Plug(const char *, nEntityObjectId);
    /// unplug
    void UnPlug(nEntityObjectId);
    /// get plug name
    const nString GetPlugName(int);
    /// get plug name of a plugged entity
    const nString GetPlugNameEntity(nEntityObjectId);
    /// get number of plugs
    int GetNumPlugs();
    /// get plugged entity
    nEntityObjectId GetPluggedEntity(int);

    /// set if a pass is enabled by default
    void SetPassEnabled(const char *, bool);
    /// get if a pass is enabled by default
    bool GetPassEnabled(const char *);
    /// set if entity is hidden
    void SetHiddenEntity(bool);
    /// get if entity is hidden
    bool GetHiddenEntity ()const;

    #ifndef NGAME
    /// linear fog distances (end, start)
    void SetLinearFogDistances(float, float);
    void GetLinearFogDistances(float&, float&);
    /// layered fog distances (top, end, range)
    void SetLayeredFogDistances(float, float, float);
    void GetLayeredFogDistances(float&, float&, float&);
    /// ocean waves parameters (Frequency, Amplitude, Velocity)
    void SetWaveFrequency(float);
    void GetWaveFrequency(float&);
    void SetWaveAmplitude(float);
    void GetWaveAmplitude(float&);
    void SetWaveVelocity(float, float);
    void GetWaveVelocity(float&, float&);
    /// texture scale (TexGenS)
    void SetTexScale(float, float);
    void GetTexScale(float&, float&);
    /// fresnel term (FresnelBias, FresnelPower)
    void SetFresnel(float, float);
    void GetFresnel(float&, float&);
    #endif

protected:

    friend class nRenderPathNode;

    /// sort links by priority
    void SortLinks();

    /// a qsort() hook for sorting linked lights by priority
    static int __cdecl LightSorter(nEntityObject* const*, nEntityObject* const*);

    /// a qsort() hook for sorting linked lights by priority
    static int __cdecl PrioritySorter(const void *, const void *);

    /// render attached and plugged entities
    void RenderAttachedEntities(nSceneGraph *sceneGraph);

    nRef<nSceneNode> refRootNode;
    nVariable::Handle timeHandle;
    nTime time;
    uint frameid;
    nShaderParams shaderOverrides;
    int passEnabledFlags;
    nArray<nVariable> localVarArray;
    nArray<nEntityObjectId> plugArray;

    int maxMaterialLevel;
    int attachIndex;
    int attachFlags;

    bool hiddenEntity;

    /// scene component class
    ncSceneClass* sceneClass;

    /// array of light entities linked to this one
    nArray<nEntityObject*> linkArray;
    /// the array of links has been updated, needs re-sorting
    bool linksAreDirty;

    /// persisted data for plugs, slot name and entityobjectid
    nArray<nString> slotName;
    nArray<nEntityObjectId> entityObjId;

    /// attach parent entity
    nRef<nEntityObject> refParentEntity;

    #ifndef NGAME
    nTime lastTimeRendered;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nSceneNode *
ncScene::GetRootNode()
{
    return this->refRootNode.isvalid() ? this->refRootNode.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncScene::IsValid()
{
    return this->refRootNode.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncScene::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
ncScene::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncScene::SetFrameId(const uint frameId)
{
    this->frameid = frameId;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const uint
ncScene::GetFrameId()
{
    return this->frameid;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncScene::SetAttachIndex(const int index)
{
    this->attachIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const int
ncScene::GetAttachIndex()
{
    return this->attachIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncScene::SetAttachFlags(const int flags)
{
    this->attachFlags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const int
ncScene::GetAttachFlags()
{
    return this->attachFlags;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
ncScene::SetFlag(int flagMask, bool value)
{
    if (value)
    {
        this->attachFlags |= flagMask;
    }
    else
    {
        this->attachFlags &= ~flagMask;
    }
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
ncScene::GetFlag(int flagMask)
{
    return (this->attachFlags & flagMask) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
ncScene::SetPassEnabledFlags(const int passFlags)
{
    this->passEnabledFlags = passFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline 
int
ncScene::GetPassEnabledFlags() const
{
    return this->passEnabledFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncScene::SetMaxMaterialLevel(int index)
{
    this->maxMaterialLevel = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncScene::GetMaxMaterialLevel() const
{
    return this->maxMaterialLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline 
nShaderParams&
ncScene::GetShaderOverrides()
{
    return this->shaderOverrides;
}

//------------------------------------------------------------------------------
/**    
*/
inline
int
ncScene::AddLocalVar(const nVariable& value)
{
    this->localVarArray.Append(value);
    return this->localVarArray.Size()-1;
}

//------------------------------------------------------------------------------
/**    
*/
inline
nVariable&
ncScene::GetLocalVar(int index)
{
    return this->localVarArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable*
ncScene::FindLocalVar(nVariable::Handle handle)
{
    int i;
    int num = this->localVarArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->localVarArray[i].GetHandle() == handle)
        {
            return &(this->localVarArray[i]);
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncScene::ClearLocalVars()
{
    this->localVarArray.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncScene::ClearLinks()
{
    this->linkArray.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncScene::GetNumLinks() const
{
    return this->linkArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
ncScene::GetLinkAt(int index) const
{
    return this->linkArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncScene::SetParentEntity(nEntityObject* entity)
{
    this->refParentEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
ncScene::GetParentEntity()
{
    return this->refParentEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    @param value new hidden flag
*/
inline
void
ncScene::SetHiddenEntity( bool value )
{
    this->hiddenEntity = value;
}

//------------------------------------------------------------------------------
/**
    @returns true if is hidden
*/
inline
bool
ncScene::GetHiddenEntity()const
{
    return this->hiddenEntity;
}

//------------------------------------------------------------------------------
#endif
