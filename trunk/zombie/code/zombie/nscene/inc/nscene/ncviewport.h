#ifndef NC_VIEWPORT_H
#define NC_VIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class ncViewport
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Viewport component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscene.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class ncViewport : public ncScene
{

    NCOMPONENT_DECLARE(ncViewport,ncScene);

public:
    /// constructor
    ncViewport();
    /// destructor
    virtual ~ncViewport();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);
    /// component initialization
    void InitInstance(nObject::InitInstanceMsg initType);
    /// attach the node hierarchy to the scene graph and set the view transform
    virtual void Render(nSceneGraph *sceneGraph);

    /// set viewport camera
    void SetCamera(const nCamera2& camera);
    /// get viewport camera
    const nCamera2& GetCamera();
    /// get viewport rectangle (for nviewportnode only)
    const nViewport& GetViewport();
    /// enable or disable a render pass
    void SetPassEnabled(nFourCC pass, bool enabled);

    /// set viewport rectangle
    void SetViewport(float, float, float, float);
    /// get viewport rectangle
    void GetViewport(float&, float&, float&, float&);
    /// set perspective projection (aov, aspect, nearp, farp)
    void SetPerspective(float, float, float, float);
    /// set orthogonal projection (width, height, nearp, farp)
    void SetOrthogonal(float, float, float, float);

    #ifndef NGAME
    /// material profile to use
    void SetMaterialProfile(int);
    int GetMaterialProfile();
    /// background color
    void SetBackgroundColor(const vector3&);
    void GetBackgroundColor(float&, float&, float&);

    #endif

        #ifndef NGAME
        #endif

private:
    nCamera2 camera;
    nViewport viewport;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncViewport::SetCamera(const nCamera2& camera) 
{
    this->camera = camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nCamera2&
ncViewport::GetCamera() 
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncViewport::SetViewport(float x, float y, float width, float height)
{
    this->viewport.x = x;
    this->viewport.y = y;
    this->viewport.width = width;
    this->viewport.height = height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncViewport::GetViewport(float& x, float& y, float& width, float& height)
{
    x = this->viewport.x;
    y = this->viewport.y;
    width = this->viewport.width;
    height = this->viewport.height;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nViewport&
ncViewport::GetViewport()
{
    return this->viewport;
}

//------------------------------------------------------------------------------
#endif
