#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nlevelmapviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nlevelmapviewport.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "gfx2/nshapeserver.h"
#include "mathlib/rectangle.h"

#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

nNebulaScriptClass(nLevelMapViewport, "nappviewport");

//------------------------------------------------------------------------------
/**
*/
nLevelMapViewport::nLevelMapViewport() :
    bgcolor(1.0f, 1.0f, 1.0f, 0.7f)
{
    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelCreated, 
                                          this,
                                          &nLevelMapViewport::OnLevelLoaded,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nLevelMapViewport::OnLevelLoaded,
                                          0);
}

//------------------------------------------------------------------------------
/**
*/
nLevelMapViewport::~nLevelMapViewport()
{
    if (this->IsOpen())
    {
        this->SetVisible(false);
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nLevelMapViewport::Open()
{
    // load quad mesh for the map
    this->refQuadMesh = nGfxServer2::Instance()->NewMesh("shape_plane");
    n_assert(this->refQuadMesh.isvalid());
    if (!this->refQuadMesh->IsLoaded())
    {
        this->refQuadMesh->SetFilename("wc:libs/system/meshes/plane.n3d2");
        this->refQuadMesh->Load();
        n_assert(this->refQuadMesh->IsLoaded());
    }

    // load shader for the map
    this->refMapShader = nGfxServer2::Instance()->NewShader("shape_map");
    n_assert(this->refMapShader.isvalid());
    if (!this->refMapShader->IsLoaded())
    {
        this->refMapShader->SetFilename("shaders:shape_map.fx");
        this->refMapShader->Load();
        n_assert(this->refMapShader->IsLoaded());
    }

    // HACK refresh level entities- viewports are created after the level is loaded
    this->OnLevelLoaded();

    // reset view matrix to default values to visualize the whole quad
    this->Reset();

    return nAppViewport::Open();
}

//------------------------------------------------------------------------------
/**
    Shutdown viewport, release the scene graph, debug module,
    and the viewport and camera entities.
*/
void
nLevelMapViewport::Close()
{
    n_assert(this->IsOpen());

    // unload quad mesh for the map
    // unload shader for the map

    nAppViewport::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::Reset()
{
    this->SetViewerPos(vector3(0.0f, 0.0f, -2.0f));
    this->SetViewerAngles(0.0f, 0.0f);//look down
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::VisibleChanged()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::OnLevelLoaded()
{
    this->refOutdoorEntity.invalidate();
    nLevel* curLevel = nLevelManager::Instance()->GetCurrentLevel();
    if (curLevel)
    {
        nEntityObjectId outdoorEntityId = curLevel->FindEntity("outdoor");
        if (outdoorEntityId)
        {
            this->refOutdoorEntity = nEntityObjectServer::Instance()->GetEntityObject(outdoorEntityId);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::Trigger()
{
    //...get the position of the viewer on the map
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::OnRender3D()
{
    // empty: nothing to attach to the scene servers
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelMapViewport::OnRender2D()
{
    n_assert(this->IsOpen());

    if (this->refOutdoorEntity.isvalid())
    {
        //... draw the global texture with some alpha value
        //ncTerrainGMMClass* terrainClass = this->refOutdoorEntity->GetClassComponentSafe<ncTerrainGMMClass>();
        ncTerrainMaterialClass* tlm = this->refOutdoorEntity->GetClassComponentSafe<ncTerrainMaterialClass>();

        nTexture2* tex = tlm->GetGlobalTexture();
        n_assert( tex && tex->IsLoaded() );

        static matrix44 ident;

        // set viewport rectangle
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->SetTransform(nGfxServer2::View, this->GetViewMatrix());//TEMP
        nCamera2 cam(this->GetCamera());
        gfxServer->SetCamera(cam);
        nViewport& vp = gfxServer->GetViewport();
        this->GetViewport(vp.x, vp.y, vp.width, vp.height);
        gfxServer->SetViewport(vp);

        nShapeServer* shapeServer = nShapeServer::Instance();
        shapeServer->SetShader(this->refMapShader.get());
        shapeServer->BeginShapes();

        this->refMapShader->SetTexture(nShaderState::DiffMap0, tex);
        shapeServer->DrawShape(this->refQuadMesh.get(), ident, this->bgcolor);

        //...draw entities- according to the debug module
        //...cameras -only the active ones
        //...indoors
        //...
        //need a general method to convert position in the terrain to position in the map space
        //and how to draw the icons, batching the quads in the terrain

        shapeServer->EndShapes();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nLevelMapViewport::HandleInput(nTime frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    // if not dragging and the mouse is moved on the map, start sliding
    float mouse_x = inputServer->GetSlider("slider_right") - inputServer->GetSlider("slider_left");
    float mouse_y = inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down");

    bool checkPos = false;

    vector3 viewerPos = this->GetViewerPos();

    if (inputServer->GetButton("buton1"))
	{
        // @todo use frameTime to make viewport movement independent from frame rate
        viewerPos.x += mouse_x * 0.3f;
        viewerPos.y += mouse_y * 0.3f;
        checkPos = true;
	}

    if (inputServer->GetButton("up"))
    {
        viewerPos.z += this->GetViewerVelocity() * (float) frameTime;
        checkPos = true;
    }

    if (inputServer->GetButton("down"))
    {
        viewerPos.z -= this->GetViewerVelocity() * (float) frameTime;
        checkPos = true;
    }

    if (inputServer->GetButton("reset"))
    {
        this->Reset();
        return true;
    }

    // apply position constraints
    if (checkPos)
    {
        static rectangle clipRect(vector2(-1.0f, -1.0f), vector2(1.0f, 1.0f));
        nCamera2 cam = this->GetCamera();
        matrix44 viewProjection = this->GetViewMatrix() * cam.GetProjection();

        // center if the viewport if the rectangle is completely contained
        vector4 topLeft = viewProjection * vector4(-1.0f, -1.0f, 0.0f, 1.0f);
        vector4 botRight = viewProjection * vector4(1.0f, 1.0f, 0.0f, 1.0f);
        if (topLeft.x > -topLeft.w && botRight.x < botRight.w && 
            topLeft.y > -topLeft.w && botRight.y < botRight.w)
        {
            viewerPos.x = 0.0f;
            viewerPos.y = 0.0f;
        }
        else
        {
            // align to the viewport- move viewer pos to the place that makes the
            // projected corner fall on the viewport's border
            if (topLeft.x > -topLeft.w)
            {
                //viewerPos.x = ...
            }
            if (topLeft.y > -topLeft.w)
            {
                //viewerPos.y = ...
            }
            if (botRight.x < botRight.w)
            {
                //viewerPos.x = ...
            }
            if (botRight.y < botRight.w)
            {
                //viewerPos.y = ...
            }
        }

        // @todo apply z constraints

        this->SetViewerPos(viewerPos);
        return true;
    }

    // move active conjurer viewport to the clicked position
    // ...

    return false;
}
