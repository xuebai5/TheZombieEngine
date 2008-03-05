#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "napplication/nappviewport.h"

nNebulaScriptClass(nAppViewport, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAppViewport::nAppViewport() :
    relSize(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f)),
    isOpen(false),
    isVisible(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAppViewport::~nAppViewport()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetVisible(const bool visible)
{
    if (visible != this->isVisible)
    {
        this->isVisible = visible;

        this->VisibleChanged();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::Reset()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::Trigger()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::OnRender3D()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::OnRender2D()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::OnFrameBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::OnFrameRendered()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewport::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewport::IsOpen()
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Called when the viewport has changed the visible attribute.
    Override in subclasses.
*/
void
nAppViewport::VisibleChanged()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Override input handling in subclasses. Return true if input handled.
*/
bool
nAppViewport::HandleInput(nTime /*frameTime*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewport::GetVisible()
{
    return this->isVisible;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetFrameId(const uint frameId)
{
    this->frameId = frameId;
}

//------------------------------------------------------------------------------
/**
*/
const uint
nAppViewport::GetFrameId()
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetFrameTime(nTime t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
*/
nTime
nAppViewport::GetTime() const
{
    return this->curTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetTime(nTime t)
{
    this->curTime = t;
}

//------------------------------------------------------------------------------
/**
*/
nTime
nAppViewport::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetRelSize(float relx, float rely, float relw, float relh)
{
    this->relSize.set(vector2(relx, rely), vector2(relx + relw, rely + relh));

    // update the camera for the viewport rectangle
    nCamera2 camera = this->GetCamera();
    camera.SetAspectRatio((float) nGfxUtils::absx(relw) / (float) nGfxUtils::absy(relh));
    camera.SetFovType(nCamera2::Hor);
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::GetRelSize(float& x, float& y, float& w, float& h)
{
    x = this->relSize.v0.x;
    y = this->relSize.v0.y;
    w = this->relSize.width();
    h = this->relSize.height();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewport(float x, float y, float w, float h)
{
    this->viewportRect.set(vector2(x, y), vector2(x + w, y + h));

    float nx, ny, nw, nh;
    nx = nGfxUtils::relx((uint) x);
    ny = nGfxUtils::rely((uint) y);
    nw = nGfxUtils::relx((uint) w);
    nh = nGfxUtils::rely((uint) h);
    this->relSize.set(vector2(nx,ny), vector2(nx+nw,ny+nh));

    // update the camera for the viewport rectangle
    nCamera2 camera = this->GetCamera();
    camera.SetAspectRatio(w / h);
    camera.SetFovType(nCamera2::Hor);
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::GetViewport(float& x, float& y, float& w, float& h) const
{
    if (relSize.size().isequal(vector2(0.0f, 0.0f), 0.0f))
    {
        x = this->viewportRect.v0.x;
        y = this->viewportRect.v0.y;
        w = this->viewportRect.v1.x - this->viewportRect.v0.x;
        h = this->viewportRect.v1.y - this->viewportRect.v0.y;
    }
    else
    {
        // if there is a relsize, compute absolute from there
        x = (float) nGfxUtils::absx(this->relSize.v0.x);
        y = (float) nGfxUtils::absy(this->relSize.v0.y);
        w = (float) nGfxUtils::absx(this->relSize.width());
        h = (float) nGfxUtils::absy(this->relSize.height());
    }
}

//------------------------------------------------------------------------------
/**
*/
const rectangle&
nAppViewport::GetViewport() const
{
    return this->viewportRect;
}

//------------------------------------------------------------------------------
/**
*/
const rectangle&
nAppViewport::GetRelSize() const
{
    return this->relSize;
}

//------------------------------------------------------------------------------
/**
*/
transform44&
nAppViewport::Transform()
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
const matrix44&
nAppViewport::GetViewMatrix()
{
    return this->transform.getmatrix();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetCamera(const nCamera2& cam)
{
    this->camera = cam;
}

//------------------------------------------------------------------------------
/**
*/
const nCamera2&
nAppViewport::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewerPos(const vector3& viewerPos)
{
    this->Transform().settranslation(viewerPos);
}

//------------------------------------------------------------------------------
/**
*/
const vector3&
nAppViewport::GetViewerPos()
{
    return this->Transform().gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewerAngles(float theta, float rho)
{
    this->Transform().seteulerrotation(vector3(theta, rho, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::GetViewerAngles(float& theta, float& rho)
{
    n_assert(this->Transform().iseulerrotation());
    const vector3& euler = this->Transform().geteulerrotation();
    theta = euler.x;
    rho = euler.y;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewerAngles(const polar2& angles)
{
    this->SetViewerAngles(angles.theta, angles.rho);
}

//------------------------------------------------------------------------------
/**
*/
const polar2&
nAppViewport::GetViewerAngles()
{
    n_assert(this->Transform().iseulerrotation());
    const vector3& euler = this->Transform().geteulerrotation();

    static polar2 viewerAngles;
    viewerAngles.theta = euler.x;
    viewerAngles.rho = euler.y;
    return viewerAngles;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewerZoom(const vector3& viewerZoom)
{
    this->viewerZoom = viewerZoom;

    nCamera2 camera = this->GetCamera();
    if (camera.GetType() != nCamera2::Orthogonal)
    {
        return;
    }

	float vpAspect = this->viewportRect.width() / this->viewportRect.height();
	camera.SetOrthogonal( this->viewerZoom.x * vpAspect, this->viewerZoom.y, -camera.GetFarPlane(), camera.GetFarPlane());

    camera.SetWidth( this->viewerZoom.x * vpAspect );
    camera.SetHeight( this->viewerZoom.y );

    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
const vector3&
nAppViewport::GetViewerZoom()
{
    return this->viewerZoom;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetNearPlane(float nearPlane)
{
    nCamera2 camera = this->GetCamera();
    camera.SetNearPlane(nearPlane);
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
float
nAppViewport::GetNearPlane()
{
    return this->GetCamera().GetNearPlane();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetFarPlane(float farPlane)
{
    nCamera2 camera = this->GetCamera();
    camera.SetFarPlane(farPlane);
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
float
nAppViewport::GetFarPlane()
{
    return this->GetCamera().GetFarPlane();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetAngleOfView(float angleOfView)
{
    nCamera2 camera = this->GetCamera();
    camera.SetAngleOfView(angleOfView);
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
float
nAppViewport::GetAngleOfView()
{
    return this->GetCamera().GetAngleOfView();
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetIsometricView(bool isometric)
{
    if (isometric)
    {
        if (this->camera.GetType() == nCamera2::Orthogonal)
        {
            return;
        }

		// save perspective AOV, near plane and aspect ratio
		this->perspAspectRatio = this->camera.GetAspectRatio();
		this->perspNearPlane = this->camera.GetNearPlane();
		this->perspAOV = this->camera.GetAngleOfView();
		this->camera.SetType( nCamera2::Orthogonal );

		// update the camera for the viewport rectangle
		this->SetViewerZoom( this->viewerZoom );
    }
    else
    {
        if (this->camera.GetType() == nCamera2::Perspective)
        {
            return;
        }
        this->camera.SetPerspective(this->perspAOV, this->perspAspectRatio, this->perspNearPlane, this->camera.GetFarPlane());
    }
    this->SetCamera(camera);
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewport::GetIsometricView()
{
    nCamera2 camera = this->GetCamera();
    return camera.GetType() == nCamera2::Orthogonal;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewport::SetViewerVelocity(float viewerVelocity)
{
    this->viewerVelocity = viewerVelocity;
}

//------------------------------------------------------------------------------
/**
*/
float
nAppViewport::GetViewerVelocity()
{
    return this->viewerVelocity;
}
