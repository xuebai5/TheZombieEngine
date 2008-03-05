#ifndef N_PREVIEWVIEWPORT_H
#define N_PREVIEWVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nPreviewViewport
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A Conjurer-specific viewport for previewing classes.
    It implements rendering of a local class instance using a viewport
    entity with no visibility determination, and its own scene graph.
    It handles input using the standard maya camera control from nebula2.

    (C) 2005 Conjurer Services, S.A.
*/
#include "napplication/nappviewport.h"
#include "mathlib/polar.h"
#include "tools/nmayacamcontrol.h"

class nSceneGraph;
class nEntityObject;
//------------------------------------------------------------------------------
class nPreviewViewport : public nAppViewport
{
public:
    /// constructor
    nPreviewViewport();
    /// destructor
    virtual ~nPreviewViewport();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// initialize viewport
    virtual bool Open();
    /// shutdown viewport
    virtual void Close();
    /// reset the viewport to the default values
    virtual void Reset();
    /// do per-frame work
    virtual void Trigger();
    /// called on state to perform 3d rendering
    virtual void OnRender3D();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();

    /// handle viewport input
    virtual bool HandleInput(nTime frameTime);
    /// called when SetVisible has changed
    virtual void VisibleChanged();

    /// get scene graph
    nSceneGraph *GetSceneGraph();

    /// set class to preview
    void SetClassName(const char* className);
    /// get class to preview
    const char* GetClassName();
    /// load target entity
    bool LoadTargetEntity();
    /// unload target entity
    void UnloadTargetEntity();

    // get the Camera-Control instance
    nMayaCamControl& GetCamControl();

private:
    /// initialize viewport entity
    bool LoadViewport();
    /// release viewport entity
    void UnloadViewport();

    nString viewportClassName;
    nString lightClassName;
    nString className;
    nString previewClassName;
    nRef<nSceneGraph> refSceneGraph;
    nRef<nEntityObject> refViewport;
    nRef<nEntityObject> refTargetEntity;
    nRef<nEntityObject> refLightEntity;

    nMayaCamControl camControl;
    bool moveLight;
};

//------------------------------------------------------------------------------
/**
*/
inline 
nSceneGraph*
nPreviewViewport::GetSceneGraph()
{
    return this->refSceneGraph.get();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
nPreviewViewport::SetClassName(const char* name)
{
    this->className = name;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const char*
nPreviewViewport::GetClassName()
{
    return this->className.IsEmpty() ? 0 : this->className.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nMayaCamControl&
nPreviewViewport::GetCamControl()
{
    return this->camControl;
}

//------------------------------------------------------------------------------
#endif
