//------------------------------------------------------------------------------
//  RnsWanderState.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchrnsstates.h"

//-----------------------------------------------------------------------------

#include "input/ninputserver.h"
#include "kernel/nkernelserver.h"
#include "nphysics/nphysicsserver.h"
#include "napplication/napplication.h"
#include "entity/nentityobjectserver.h"
#include "rnsstates/RnsWanderState.h"
#include "zombieentity/nctransform.h"
#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphymaterial.h"
#include "nphysics/ncphycharacter.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "ncgamecamera/ncgamecamera.h"

#ifndef NGAME
#include "nlayermanager/nlayermanager.h"
#include "ndebug/ndebugserver.h"
#include "ndebug/nceditor.h"
#endif

//-----------------------------------------------------------------------------

nNebulaScriptClass( RnsWanderState, "nappstate" );

//-----------------------------------------------------------------------------

namespace
{
    float cameraZDistance(10);
    float cameraYDistance(5);
}

//-----------------------------------------------------------------------------
/**
    Default constructor.

    history:
        - 05-Oct-2005   Zombie         created
*/
RnsWanderState::RnsWanderState() :
    input(0),
    physicCharacter(0),
    oldViewport(0),
    frameProportion(float(1)),
    transformCharacter(0),
    physicsObject(0),
    orientation(0,0,0),
    phyMaterial(0),
    transformDummy(0),
    inputInputed( false ),
    dummyCharacter(0),
    gCamera(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 05-Oct-2005   Zombie         created
*/
RnsWanderState::~RnsWanderState()
{
    // Empty
    this->Destroy(); // just in case
}


//-----------------------------------------------------------------------------
/**
    Called when state is created.

    @param application application running this state

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnCreate(nApplication* application)
{
    nAppState::OnCreate( application );

    this->input = nInputServer::Instance();

    n_assert2( this->input, "Failed to grab the input server." );

    // Check for player material
    if( ! nKernelServer::Instance()->Lookup("/usr/physics/rnsmaterial") )
    {
        // creating the player material
        this->phyMaterial = static_cast<nPhyMaterial*>( nKernelServer::Instance()->New( "nphymaterial", "/usr/physics/rnsmaterial" ) );

        n_assert2( this->phyMaterial, "Failed to create the player material." );
        if( this->phyMaterial )
        {
            this->phyMaterial->SetCoeficientPyramidFriction(phyreal(0));
            this->phyMaterial->SetCoeficientBounce(phyreal(.0001));
            this->phyMaterial->SetCoeficientSoftnessERP( phyreal(1) );
            this->phyMaterial->SetCoeficientSoftnessCFM(0);
            this->phyMaterial->SetCoeficientSlip(phyreal(0));
            this->phyMaterial->Update();
        }
    }
    else
    {
        this->phyMaterial = static_cast<nPhyMaterial*>(
            nKernelServer::Instance()->Lookup("/usr/physics/rnsmaterial"));
    }

}

//-----------------------------------------------------------------------------
/**
    Called when state is becoming active.

    @param prevState string id of an state

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnStateEnter(const nString& prevState)
{
    nAppState::OnStateEnter( prevState );

    this->previousState = prevState;

    this->PrepareViewPort();

#ifndef NGAME
    // hide the mouse cursor
    nGfxServer2::Instance()->SetCursorVisibility( nGfxServer2::Custom );
#endif

    /// creates the player
    this->CreatePlayer();

    this->gCamera = nEntityObjectServer::Instance()->NewLocalEntityObject( "negamecamera" )->GetComponent<ncGameCamera>();

    n_assert2( this->gCamera, "Failed to create camera." );

    this->gCamera->SetViewPort( this->viewport );

    this->gCamera->SetCameraType( ncGameCamera::thirdperson );

    this->gCamera->SetAnchorPoint( this->dummyCharacter );

    this->gCamera->AddAttributes( ncGameCamera::lookat );

    this->gCamera->SetCameraOffset( vector3( 0,5,-8 ) );

    this->gCamera->Build();

    /// initing
    this->frameProportion = float(1);

    this->orientation = vector3(0,0,0);
}

//-----------------------------------------------------------------------------
/**
    Called when state is becoming inactive.

    @param prevState string id of an state

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnStateLeave(const nString& nextState)
{
    nAppState::OnStateLeave( nextState );

    nEntityObjectServer::Instance()->RemoveEntityObject( this->gCamera->GetEntityObject());

    this->viewport->SetVisible( false );

    this->viewport->Close(); 

    this->Destroy();
}

//-----------------------------------------------------------------------------
/**
    Called on state to perform state logic.

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnFrame()
{
    nAppState::OnFrame();

    // expected 60 FPS (should be a var holding it)
    this->frameProportion = float(this->app->GetFrameTime()) / (float(1)/float(60));


    ncPhysicsObj* pobj(this->dummyCharacter->GetComponent<ncPhysicsObj>());

    pobj->Disable();
    // trigger physics servers
    nPhysicsServer::Instance()->Run(phyreal(this->app->GetFrameTime()));
    pobj->Enable();


    // update input
    this->UpdateInput();

    // viewport logic
    this->viewport->SetFrameId(this->app->GetFrameId());
    this->viewport->SetFrameTime(this->app->GetFrameTime());
    this->viewport->SetTime(this->app->GetTime());
    this->viewport->Trigger();

    // updates camera
    this->UpdateCamera();

    this->UpdatePlayerMovement();
}

//-----------------------------------------------------------------------------
/**
    Called on state to perform 3d rendering.

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnRender3D()
{
    nAppState::OnRender3D();

    this->viewport->OnRender3D();
}

//-----------------------------------------------------------------------------
/**
    Called on state to perform 2d rendering.

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnRender2D()
{
    nAppState::OnRender2D();
#ifndef NGAME
    nDebugServer::Instance()->GetFlagEnabled( "rnswanderview", "debugtexts" );
#endif
}

//-----------------------------------------------------------------------------
/**
    Called before nSceneServer::RenderScene().

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnFrameBefore()
{
    nAppState::OnFrameBefore();
}

//-----------------------------------------------------------------------------
/**
    Called after nSceneServer::RenderScene()

    history:
        - 05-Oct-2005   Zombie         created
*/
void RnsWanderState::OnFrameRendered()
{
    nAppState::OnFrameRendered();
}

//-----------------------------------------------------------------------------
/**
    Prepares viewport.

    history:
        - 24-Oct-2005   Zombie         created
*/
void RnsWanderState::PrepareViewPort()
{
    if( !this->viewport.isvalid() )
    {
        // load viewport for renaissance states
        this->viewport = static_cast<nAppViewport*>(kernelServer->LoadAs("home:data/appdata/conjurer/viewport/rnswanderview.n2", "/usr/rnswanderview"));
        n_assert(this->viewport.isvalid());
        n_assert(this->viewport->IsA("nappviewport"));
    }
    
    this->viewport->Open();    

    this->viewport->SetVisible( true );

    this->viewport->SetNearPlane( float(1) );
}

//-----------------------------------------------------------------------------
/**
    Updates input information.

    history:
        - 24-Oct-2005   Zombie         created
*/
void RnsWanderState::UpdateInput()
{
    this->inputInputed = false;

    if( !this->previousState.IsEmpty() )
    {
        // checking if exiting from this state
        if( this->input->GetButton( "exit" ) )
        {
            this->app->SetState( this->previousState );
        }

        if( this->input->GetButton( "Forward" ) )
        {
            const matrix44& camera( this->viewport->GetViewMatrix() );

            vector3 dir(camera.z_component());

            dir.y = 0;

            dir.norm();

            static float force(float(-20000));

            dir *= force * this->frameProportion;

            this->physicsObject->AddForce( dir );           
        }

        if( this->input->GetButton( "Backward" ) )
        {
            const matrix44& camera( this->viewport->GetViewMatrix() );

            vector3 dir(camera.z_component());

            dir.y = 0;

            dir.norm();

            static float force(float(20000));

            dir *= force * this->frameProportion;

            this->physicsObject->AddForce( dir );           
        }

        if( this->input->GetButton( "TurnLeft" ) )
        {
            //this->inputInputed = true;

            static float inc(float(-.1));

            orientation.y += inc * this->frameProportion;
        }

        if( this->input->GetButton( "TurnRight" ) )
        {
            vector3 charOrientation(this->transformCharacter->GetEuler());            

            static float inc(float(.1));

            orientation.y += inc * this->frameProportion;
        }

        if( this->input->GetButton( "StrafeLeft" ) )
        {
            const matrix44& camera( this->viewport->GetViewMatrix() );

            vector3 dir(camera.x_component());

            dir.y = 0;

            dir.norm();

            static float force(float(-20000));

            dir *= force * this->frameProportion;

            this->physicsObject->AddForce( dir );           
        }

        if( this->input->GetButton( "StrafeRight" ) )
        {
            const matrix44& camera( this->viewport->GetViewMatrix() );

            vector3 dir(camera.x_component());

            dir.y = 0;

            dir.norm();

            static float force(float(20000));

            dir *= force * this->frameProportion;

            this->physicsObject->AddForce( dir );           
        }

        // getting mouse
        float yaw(input->GetSlider("slider_left") - input->GetSlider("slider_right"));

        if( yaw != 0 )
        {
            static float speed(float(.5));
            orientation.y += yaw * speed * this->frameProportion;
        }

        float pitch(input->GetSlider("slider_down") - input->GetSlider("slider_up"));

        if( pitch != 0 )
        {
            static float speed(float(.5));
            cameraYDistance += pitch * speed * this->frameProportion;
        }      

        if( input->GetButton("buton1") )
        {
            static float speed(float(-.5));
            cameraZDistance += 1 * speed * this->frameProportion;
        }

        if( input->GetButton("buton0") )
        {
            static float speed(float(.5));
            cameraZDistance += 1 * speed * this->frameProportion;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the player information.

    history:
        - 25-Oct-2005   Zombie         created
*/
void RnsWanderState::CreatePlayer()
{
    this->physicCharacter = nEntityObjectServer::Instance()->NewLocalEntityObject( "nephycharacter" );

    n_assert2( this->physicCharacter, "Error, failed to create a physics character" );

    this->transformCharacter = this->physicCharacter->GetComponent<ncTransform>();
    
    this->physicsObject = this->physicCharacter->GetComponent<ncPhysicsObj>();

    this->physicsObject->AutoInsertInSpace();

    this->physicsObject->SetMaterial( this->phyMaterial->Id() );

    this->transformCharacter->SetPosition( this->oldViewport->GetViewerPos() );

    this->CreateGraphicDummy();
}

//-----------------------------------------------------------------------------
/**
    Destroys info.

    history:
        - 25-Oct-2005   Zombie         created
*/
void RnsWanderState::Destroy()
{
    /// destroying character
    if( this->physicCharacter )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->physicCharacter );
        this->physicCharacter = 0;
    }

    /// cleaning vars
    this->transformCharacter = 0;
    this->physicsObject = 0;

    if( this->dummyCharacter )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->dummyCharacter );
        this->dummyCharacter = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the previous viewport.

    history:
        - 25-Oct-2005   Zombie         created
*/
void RnsWanderState::SetPreviousViewport( nAppViewport* prevViewport )
{
    this->oldViewport = prevViewport;
}

//-----------------------------------------------------------------------------
/**
    Updates camera.

    history:
        - 25-Oct-2005   Zombie         created
*/
void RnsWanderState::UpdateCamera()
{
    this->gCamera->Update( this->app->GetFrameTime() );
}

//-----------------------------------------------------------------------------
/**
    Manage player movement.

    history:
        - 26-Oct-2005   Zombie         created
*/
void RnsWanderState::UpdatePlayerMovement()
{
    vector3 velocity;

    this->physicsObject->GetLinearVelocity( velocity );

    nPhyRigidBody* body(this->physicsObject->GetBody());

    static float resistance( float(1) );

    if( this->inputInputed )
    {

        if( velocity.len() > body->GetLinearVelocityThresholdAutoDisable() )
        {
            this->physicsObject->SetLinearVelocity( velocity * resistance );
        }

    }
    else
    {
        this->physicsObject->SetLinearVelocity(vector3(0,velocity.y,0));
    }

    /// updating dummy
    this->transformDummy->SetPosition( this->transformCharacter->GetPosition() );

    vector3 euler( orientation );

    this->transformDummy->SetEuler( euler );
}

//-----------------------------------------------------------------------------
/**
    Creates a graphic dummy representing the player.

    history:
        - 26-Oct-2005   Zombie         created
*/
void RnsWanderState::CreateGraphicDummy()
{
    this->dummyCharacter = nEntityObjectServer::Instance()->NewLocalEntityObject( "Dummy" );

    n_assert2( this->dummyCharacter, "Failed to create the dummy graphic." );

    this->transformDummy = dummyCharacter->GetComponent<ncTransform>();

    n_assert2( this->transformDummy, "The component ncTransform from the dummy player it's missing." );

    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert( level );

#ifndef NGAME
    nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
    n_assert( layerManager );

    ncEditor* editorComp = this->dummyCharacter->GetComponent<ncEditor>();
    if ( editorComp )
    {
        editorComp->SetLayerId( layerManager->GetSelectedLayerId() );
    }
#endif
    level->AddEntity( this->dummyCharacter );

    ncPhysicsObj* pobj(this->dummyCharacter->GetComponent<ncPhysicsObj>());

    pobj->AutoInsertInSpace();
}

//-----------------------------------------------------------------------------
/**
    Begin Scripting
*/
NSCRIPT_INITCMDS_BEGIN(RnsWanderState);
NSCRIPT_INITCMDS_END();
//-----------------------------------------------------------------------------
/**
    End Scripting
*/
