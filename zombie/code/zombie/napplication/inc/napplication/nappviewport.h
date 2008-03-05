#ifndef N_APPVIEWPORT_H
#define N_APPVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nAppViewport
    @ingroup Application
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A simple application viewport object.
    Derive subclasses to get more advanced viewport behaviour.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ntypes.h"
#include "mathlib/transform44.h"
#include "mathlib/polar.h"
#include "gfx2/ngfxutils.h"

//------------------------------------------------------------------------------
class nAppViewport : public nRoot
{
public:
    /// constructor
    nAppViewport();
    /// destructor
    virtual ~nAppViewport();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    /// reset the viewport to the default position and orientation
    virtual void Reset();
    /// do per-frame work
    virtual void Trigger();
    /// perform input handling
    virtual bool HandleInput(nTime frameTime);
    /// called on state to perform 3d rendering
    virtual void OnRender3D();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();
    /// called before nSceneServer::RenderScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();

    /// initialize viewport
    virtual bool Open();
    /// shutdown viewport
    virtual void Close();
    /// return true if viewport initialized
    virtual bool IsOpen();

    /// called when the visible attribute has changed
    virtual void VisibleChanged();

    /// get viewport rectangle
    const rectangle& GetViewport() const;
    /// get viewport rectangle (0..1)
    const rectangle& GetRelSize() const;

    /// set frameid
    void SetFrameId(const uint frameId);
    /// get frameid
    const uint GetFrameId();
    /// set the current time
    void SetTime(nTime t);
    /// get the current time
    nTime GetTime() const;
    /// set the current frame time
    void SetFrameTime(nTime t);
    /// get the current frame time
    nTime GetFrameTime() const;

    /// access to the application camera's transform object
    transform44& Transform();
    /// return current view matrix
    const matrix44& GetViewMatrix();
    /// set viewer angles
    void SetViewerAngles(const polar2& viewerAngles);
    /// get viewer angles
    const polar2& GetViewerAngles();
    /// set camera projection
    void SetCamera(const nCamera2&);
    /// get camera projection
    const nCamera2& GetCamera() const;

    /// set relative viewport rect
    void SetRelSize(float, float, float, float);
    /// get relative viewport rect
    void GetRelSize(float&, float&, float&, float&);
    /// set viewport rect
    void SetViewport(float, float, float, float);
    /// get viewport rect
    void GetViewport(float&, float&, float&, float&) const;
    /// set viewport visible
    void SetVisible(bool);
    /// viewport is visible
    bool GetVisible();

    /// set viewer position
    void SetViewerPos(const vector3&);
    /// get viewer position
    const vector3& GetViewerPos();
    /// set viewer angles
    void SetViewerAngles(float, float);
    /// set viewer angles
    void GetViewerAngles(float&, float&);
    /// set near plane
    void SetNearPlane(float);
    /// get near plane
    float GetNearPlane();
    /// set far plane
    void SetFarPlane(float);
    /// get far plane
    float GetFarPlane();
    /// set angle of view
    void SetAngleOfView(float);
    /// get angle of view
    float GetAngleOfView();
    /// set isometric view
    void SetIsometricView(bool);
    /// get isometric view
    bool GetIsometricView();
    /// set viewer velocity
    void SetViewerVelocity(float);
    /// get viewer velocity
    float GetViewerVelocity();
    /// set viewer velocity
    void SetViewerZoom(const vector3&);
    /// get viewer velocity
    const vector3& GetViewerZoom();

private:
    rectangle viewportRect;
    rectangle relSize;

    nTime curTime;
    nTime frameTime;
    uint frameId;
    bool isOpen;
    bool isVisible;

    transform44 transform;
    vector3 viewerZoom;
    float viewerVelocity;
    nCamera2 camera;

	/// Saved perspective near distance when in isometric view
	float perspNearPlane;
	
	/// Saved perspective aspect ratio when in isometric view
	float perspAspectRatio;

	/// Saved perspective AOV when in isometric view
	float perspAOV;
};

//------------------------------------------------------------------------------
#endif
