#ifndef N_EDITORVIEWPORT_H
#define N_EDITORVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nEditorViewport
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A Conjurer-specific editor viewport. It implements rendering
    using a local viewport entity, and a camera entity for visibility
    determination. It is a full-featured viewport, with an embedded
    debug module, custom input handling, and some simple option to tweak
    the rendering and control behavior.

    (C) 2005 Conjurer Services, S.A.
*/

#include "ncommonapp/ncommonviewport.h"

//------------------------------------------------------------------------------
class nEditorViewport : public nCommonViewport
{
public:
    /// constructor
    nEditorViewport();
    /// destructor
    virtual ~nEditorViewport();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    /// initialize viewport
    virtual bool Open();
    /// shutdown viewport
    virtual void Close();
    /// handle viewport input
    virtual bool HandleInput(nTime frameTime);

    /// called on state to perform 3d rendering
    virtual void OnRender3D();

    /// called when a level is loaded
    virtual void OnLevelLoaded();

    /// update shader overrides
    void UpdateShaderOverrides();

    /// set coordinate axes visible
    void SetAxesVisible(bool showAxes);
    /// get coordinate axes visible
    bool GetAxesVisible();
    /// set move light with viewer
    void SetMoveLight(bool moveLight);
    /// get move light with viewer
    bool GetMoveLight();
    /// set camera collect entities
    void SetCollectEntities(bool collect);
    /// get camera collect entities
    bool GetCollectEntities();

    /// get axes entity
    nEntityObject* GetAxesEntity();
    /// get light entity
    nEntityObject* GetLightEntity();

    /// get viewport time
    uint GetPassEnabledFlags();

    /// set debug module name
    void SetDebugModuleName(const char *);
    /// get debug module name
    const char * GetDebugModuleName();

    /// set statistics level
    void SetStatsLevel(int level);
    /// get statistics enabled
    int GetStatsLevel() const;

private:
    friend class nViewportDebugModule;
    /// refresh viewport resources
    virtual bool ReloadViewport();

    /// handle input in navigator game cam mode
    bool HandleInputGameCam(nTime frameTime);

    bool moveLight;
    bool showAxes;
    int statsLevel;

    nString debugModuleName;
    nRef<nViewportDebugModule> refDebugModule;

    /// standard light entity (shared by all viewports)
    static nRef<nEntityObject> refLightEntity;
    /// standard axes entity (shared by all viewports)
    static nRef<nEntityObject> refAxesEntity;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditorViewport::SetDebugModuleName(const char *name)
{
    this->debugModuleName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nEditorViewport::GetDebugModuleName()
{
    return this->debugModuleName.IsEmpty() ? this->GetName() : this->debugModuleName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditorViewport::SetAxesVisible(bool showAxes)
{
    this->showAxes = showAxes;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEditorViewport::GetAxesVisible()
{
    return this->showAxes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditorViewport::SetMoveLight(bool moveLight)
{
    this->moveLight = moveLight;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEditorViewport::GetMoveLight()
{
    return this->moveLight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditorViewport::SetStatsLevel(int value)
{
    this->statsLevel = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nEditorViewport::GetStatsLevel() const
{
    return this->statsLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nEditorViewport::GetAxesEntity()
{
    return this->refAxesEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nEditorViewport::GetLightEntity()
{
    return this->refLightEntity.get_unsafe();
}

//------------------------------------------------------------------------------
#endif
