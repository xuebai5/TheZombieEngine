#ifndef N_LEVELMAPVIEWPORT_H
#define N_LEVELMAPVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nLevelMapViewport
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A Conjurer-specific viewport for displaying the current terrain.
    It renders an orthogonal view of the current level's terrain entity, 
    with some simple controls to zoom in, slide the map view, and select
    a spot to move the current editor viewport to.
    Additionally, it provides an embedded debug module to control viewing
    aspects of the map: show/hide entities and stats, lock view height, etc.

    (C) 2005 Conjurer Services, S.A.
*/
#include "napplication/nappviewport.h"
#include "ngeomipmap/nterrainglobaltexturebuilder.h"

//------------------------------------------------------------------------------
class nLevelMapViewport : public nAppViewport
{
public:
    /// constructor
    nLevelMapViewport();
    /// destructor
    virtual ~nLevelMapViewport();
    /// object persistence
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

    /// set scene resource
    void SetBgColor(const vector4&);
    /// get scene resource
    const vector4& GetBgColor();

private:
    /// called when a level is loaded
    void OnLevelLoaded();

    nRef<nEntityObject> refOutdoorEntity;
    nRef<nMesh2> refQuadMesh;
    nRef<nShader2> refMapShader;
    vector4 bgcolor;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nLevelMapViewport::SetBgColor(const vector4& color)
{
    this->bgcolor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLevelMapViewport::GetBgColor()
{
    return this->bgcolor;
}

//------------------------------------------------------------------------------
#endif
