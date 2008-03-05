#ifndef N_CONJURERLAYOUT_H
#define N_CONJURERLAYOUT_H
//------------------------------------------------------------------------------
/**
    @class nConjurerLayout
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Conjurer-specific application viewportUI handle.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "napplication/nappviewportui.h"
#include "tools/nlinedrawer.h"

//------------------------------------------------------------------------------
class nConjurerLayout : public nAppViewportUI
{
public:
    /// constructor
    nConjurerLayout();
    /// destructor
    virtual ~nConjurerLayout();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    /// reset to default state
    virtual void Reset();
    /// perform layout logic
    virtual void Trigger();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();
    /// called when visible state changes
    virtual void VisibleChanged();

    /// get default viewer angles
    const polar2& GetDefViewerAngles();
    /// get preview viewport
    nAppViewport* GetPreviewViewport();

    /// set default viewer velocity
    void SetDefViewerVelocity(float);
    /// get default viewer velocity
    float GetDefViewerVelocity();
    /// set default viewer angles
    void SetDefViewerAngles(float, float);
    /// set default viewer angles
    void GetDefViewerAngles(float&, float&);
    /// set default viewer position
    void SetDefViewerPos(const vector3&);
    /// get default viewer position
    const vector3& GetDefViewerPos();
    /// set default viewer zoom
    void SetDefViewerZoom(const vector3&);
    /// get default viewer zoom
    const vector3& GetDefViewerZoom();

    /// set a viewport as the preview one
    void SetPreviewViewport(const char *);

private:
    /// handle layout input
    bool HandleInput();

    float defViewerVelocity;
    polar2 defViewerAngles;
    vector3 defViewerPos;
    vector3 defViewerZoom;

    nLineDrawer lineDraw;
    
    ViewportEntry previewEntry; ///< viewport entry for the preview
    ViewportEntry mapEntry;     ///< viewport entry for the map
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nConjurerLayout::SetDefViewerVelocity(float defViewerVelocity)
{
    this->defViewerVelocity = defViewerVelocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nConjurerLayout::GetDefViewerVelocity()
{
    return this->defViewerVelocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nConjurerLayout::SetDefViewerAngles(float theta, float rho)
{
    this->defViewerAngles.set(theta, rho);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nConjurerLayout::GetDefViewerAngles(float& theta, float& rho)
{
    theta = this->defViewerAngles.theta;
    rho = this->defViewerAngles.rho;
}

//------------------------------------------------------------------------------
/**
*/
inline
const polar2&
nConjurerLayout::GetDefViewerAngles()
{
    return this->defViewerAngles;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nConjurerLayout::SetDefViewerPos(const vector3& defViewerPos)
{
    this->defViewerPos = defViewerPos;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nConjurerLayout::GetDefViewerPos()
{
    return this->defViewerPos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nConjurerLayout::SetDefViewerZoom(const vector3& defViewerZoom)
{
    this->defViewerZoom = defViewerZoom;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nConjurerLayout::GetDefViewerZoom()
{
    return this->defViewerZoom;
}

//------------------------------------------------------------------------------
#endif
