#ifndef N_COMMONVIEWPORT_H
#define N_COMMONVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nCommonViewport
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A Conjurer-specific editor viewport. It implements rendering
    using a local viewport entity, and a camera entity for visibility
    determination. It is a full-featured viewport, with an embedded
    debug module, custom input handling, and some simple option to tweak
    the rendering and control behavior.

    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ncmdprotonativecpp.h"
#include "napplication/nappviewport.h"
#include "entity/nentitytypes.h"
#include "entity/nentityobject.h"

class nSceneGraph;
class nEntityObject;
class ncSpatialCamera;

//------------------------------------------------------------------------------
class nCommonViewport : public nAppViewport
{
public:
    /// constructor
    nCommonViewport();
    /// destructor
    virtual ~nCommonViewport();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// reset the viewport to the default values
    virtual void Reset();
    /// do per-frame work
    virtual void Trigger();
    /// called on state to perform 3d rendering
    virtual void OnRender3D();

    /// initialize viewport
    virtual bool Open();
    /// shutdown viewport
    virtual void Close();

    /// handle viewport input
    virtual bool HandleInput(nTime frameTime);
    /// called when SetVisible has changed
    virtual void VisibleChanged();

    /// called when a level is loaded
    virtual void OnLevelLoaded();

    /// get scene graph
    nSceneGraph *GetSceneGraph();
    /// set spatial camera
    void SetSpatialCamera(ncSpatialCamera *camera);
    /// get spatial camera
    ncSpatialCamera* GetSpatialCamera();
    /// enable or disable a scene pass
    void SetPassEnabled(nFourCC pass, bool enabled);
    /// set viewport time
    void SetPassEnabledFlags(uint passEnabledFlags);
    /// get viewport time
    uint GetPassEnabledFlags();
    /// set the use octrees visibility flag
    void SetUseOctreeVisibility(bool useOctrees);
    /// set light visibility enabled
    void SetObeyLightLinks(bool obeyLightLinks);
    /// get light visibility enabled
    bool GetObeyLightLinks();
    /// set light visibility enabled
    void SetObeyMaxDistance(bool obeyMaxDistance);
    /// get light visibility enabled
    bool GetObeyMaxDistance();

    /// set scene resource
    void SetResourceFile(const char *);
    /// get scene resource
    const char * GetResourceFile();

    /// get viewport entity
    nEntityObject* GetViewportEntity();
    /// begin a viewport entity (SCRIPTING ONLY!)
    nEntityObject* BeginViewportEntity(const char *);
    /// get mouse x factor
    float GetMouseXFactor ()const;
    /// set mouse x factor
    void SetMouseXFactor(float);
    /// get mouse y factor
    float GetMouseYFactor ()const;
    /// set mouse y factor
    void SetMouseYFactor(float);
    /// get mouse y invert
    bool GetMouseYInvert ()const;
    /// set mouse y invert
    void SetMouseYInvert(bool);

    /// set max distance to render entities
    void SetMaxRenderDistance(float);
    /// get max distance to render entities
    float GetMaxRenderDistance();

    /// set the spatial camera's occlusion radius
    void SetOcclusionRadius(float rad);
    /// get the spatial camera's occlusion radius
    float GetOcclusionRadius() const;
    /// set the maximum number of occluders to take into account for occlusion
    void SetMaxNumOccluders(int);
    /// get the maximum number of occluders to take into account for occlusion
    int GetMaxNumOccluders() const;
    /// set the minimum value of the area-angle test to take into account an occluder
    void SetAreaAngleMin(float);
    /// get the minimum value of the area-angle test to take into account an occluder
    float GetAreaAngleMin() const;

private:
    friend class nViewportDebugModule;

    /// initialize viewport entity
    bool LoadViewport();
    /// release viewport entity
    void UnloadViewport();
    /// refresh viewport resource
    void Refresh();

    /// handle input in navigator game cam mode
    bool HandleInputGameCam(nTime frameTime);

    nString resourceFile;

protected:
    /// refresh viewport resources
    virtual bool ReloadViewport();

    nRef<nSceneGraph> refSceneGraph;
    nRef<nEntityObject> refSpatialCamera;
    nRef<nEntityObject> refViewport;

    float mouseXfactor;
    float mouseYfactor;

    bool obeyLightLinks;
    bool drawDecals;
    bool reloadViewport;
    bool obeyMaxDistance;
    float maxRenderDistance;

    int numVisibleEntities;
    int numVisibleDecals;

    #if __NEBULA_STATS__
    nProfiler profAttachTrigger;
    nProfiler profAttachViewport;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline 
nSceneGraph *
nCommonViewport::GetSceneGraph()
{
    return this->refSceneGraph.get();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
nCommonViewport::SetResourceFile(const char *name)
{
    this->resourceFile = name;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const char *
nCommonViewport::GetResourceFile()
{
    return this->resourceFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nCommonViewport::GetViewportEntity()
{
    n_assert(this->IsOpen());
    return this->refViewport.get();
}

//-----------------------------------------------------------------------------
/**
    @param value new mouse x factor
*/
inline
void
nCommonViewport::SetMouseXFactor( float value )
{
    this->mouseXfactor = n_sgn( this->mouseXfactor ) * value;
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse x factor
*/
inline
float
nCommonViewport::GetMouseXFactor() const
{
    return n_abs( this->mouseXfactor );
}

//-----------------------------------------------------------------------------
/**
    @param value new mouse y factor
*/
inline
void
nCommonViewport::SetMouseYFactor( float value )
{
    this->mouseYfactor = n_sgn( this->mouseYfactor ) * value;
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse y factor
*/
inline
float
nCommonViewport::GetMouseYFactor() const
{
    return n_abs( this->mouseYfactor );
}

//-----------------------------------------------------------------------------
/**
    @param value invert value
*/
inline
void
nCommonViewport::SetMouseYInvert( bool value )
{
    this->mouseYfactor = n_abs( this->mouseYfactor );
    if( value )
    {
        this->mouseYfactor *= -1.0f;
    }
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse y invert flag
*/
inline
bool
nCommonViewport::GetMouseYInvert() const
{
    if( this->mouseYfactor < 0.0f )
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCommonViewport::GetObeyMaxDistance()
{
    return this->obeyMaxDistance;
}

//------------------------------------------------------------------------------
#endif
