#include "precompiled/pchrnsstates.h"
//------------------------------------------------------------------------------
//  rnsaistate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "rnsstates/rnsaistate.h"

#include "napplication/napplication.h"
#include "napplication/nappviewport.h"

#include "misc/nconserver.h"
#include "nphysics/nphysicsworld.h"
#include "nphysics/nphygeomspace.h"

#include "input/ninputserver.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "util/nstream.h"

#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"

#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmesh.h"
#include "ncaimovengine/ncaimovengine.h"

#include "npathfinder/npathfinder.h"

#include "nspatial/ncspatialquadtree.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "util/nrandomlogic.h"



nNebulaClass( RnsAiState, "nappstate" );
//------------------------------------------------------------------------------
/**
*/
RnsAiState::RnsAiState():
    viewerPos( 0,0,0 ),
    showPhysics( false ),
    showPathInfo ( false ),
    showNavMesh ( true ),
    pathFinder(0),
    actionManager(0),
    showPathNodes (false),
    showEntityDebugInfo ( true ),
    loaded ( false ),
    entity (0),
    navMesh (0)
{
    for ( int i=0; i<NUM_JP; i++ )
    {
        this->jp[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
RnsAiState::~RnsAiState()
{
    // Destroyes all entities
    this->ShutDownAISystem();
    this->DestroyAIEntities();

    this->shot->Release();      
}

//------------------------------------------------------------------------------
/**
    DestroyAIEntities
*/
void
RnsAiState::DestroyAIEntities()
{
    if ( this->entity )
    {
        this->entity->Release();
        this->entity = 0;
    }

    for ( int i=0; i<NUM_JP; i++ )
    {
        nEntityObject* agent = this->jp[i];

        if ( agent )
        {
            agent->Release();
            agent = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param application application that create the state
*/
void
RnsAiState::OnCreate( nApplication * application )
{
    nAppState::OnCreate( application );

    /*this->refViewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
    n_assert( this->refViewport.isvalid() );

    this->shot = static_cast<nPhyGeomRay*>( nObject::kernelServer->New( "nphygeomray" ) );

    this->CreateNavigationMesh();
    this->InitAISystem();*/
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void
RnsAiState::OnStateEnter( const nString & /*prevState*/ )
{
    if ( !this->loaded )
    {
        this->refViewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
        n_assert( this->refViewport.isvalid() );

        this->shot = static_cast<nPhyGeomRay*>( nObject::kernelServer->New( "nphygeomray" ) );
        this->shot->SetCategories( 0 );
        this->shot->SetCollidesWith( -1 );

        // Wait @TODO
        this->CreateNavigationMesh();
        //this->TestStream();
        this->InitAISystem();

        this->viewMatrix.ident();

        this->refViewport->SetVisible(true);
        this->loaded = true;
    }
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
RnsAiState::OnStateLeave( const nString & /*nextState*/ )
{
    //this->refViewport->SetVisible(false);
	this->ShutDownAISystem();
}

//------------------------------------------------------------------------------
/**
*/
void
RnsAiState::OnFrame()
{

    // update input
    this->HandleInput(static_cast<float>( this->app->GetFrameTime() ));

    // Shot
    vector3 viewPosition(this->ConvertScreenToViewSpace( vector2(0.5, 0.5) ));

    matrix44 invProj(nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection));            
    invProj.invert();

    vector3 position( this->viewMatrix.pos_component());
    this->shotDirection = this->viewMatrix * viewPosition;
    this->shotDirection = this->shotDirection - position;

    this->shot->SetLength( 1000 );
    this->shot->SetDirection( this->shotDirection );
    this->shot->SetPosition( position );
    
    nPhyCollide::nContact contact;
    int num( nPhysicsServer::Instance()->Collide( this->shot, 1, &contact ) );
    
    if ( num )
    {
        nPhysicsGeom * geom = contact.GetGeometryA();

        if ( geom  ) // && geom->HasAttributes (nPhysicsGeom::ground) ) // OUTDOOR TEST
        {
            this->shotObj = geom->Id();
            contact.GetContactPosition( this->shotPosition );
        }
    }

    if ( this->entity )
    {
        ncGameplayLiving* gamePlay = this->entity->GetComponent <ncGameplayLiving>();
        n_assert(gamePlay);

        if ( gamePlay )
        {
            gamePlay->Run (0);
        }
    }

    for ( int i=0; i<NUM_JP; i++ )
    {
        nEntityObject* agent = this->jp[i];

        if ( agent )
        {
            ncGameplayLiving* gamePlay = agent->GetComponent <ncGameplayLiving>();
            n_assert(gamePlay);

            if ( gamePlay )
            {
                gamePlay->Run (0);
            }
        }
    }

	this->refViewport->SetFrameId(this->app->GetFrameId());
	this->refViewport->SetTime( this->app->GetTime() );
	this->refViewport->SetFrameTime(this->app->GetFrameTime());
	this->refViewport->Trigger();
}

//------------------------------------------------------------------------------
/**
*/
void
RnsAiState::OnRender3D()
{
    this->refViewport->OnRender3D();
}

//------------------------------------------------------------------------------
/**
*/
void
RnsAiState::OnRender2D()
{
#ifndef NGAME
    nGfxServer2* server = nGfxServer2::Instance();

    if( this->showPhysics )
    {
        nPhysicsServer::Instance()->Draw (server);
    }

    // Draw the cross at the center of the screen
    this->DrawCross (server);
#endif

    if ( this->showPathInfo && this->pathFinder )
    {
        //this->pathFinder->DrawPath (server);
    }
    
    // Draw auxiliary path finder debug info
    if ( this->pathFinder )
    {
     //   this->pathFinder->DrawNodePath(server, this->showPathNodes);
    }
    
#ifndef NGAME
    this->showEntityDebugInfo = true;
    if ( this->showEntityDebugInfo )
    { 
        if ( this->entity )
        {
            this->entity->GetComponentSafe<ncAIMovEngine>()->Draw (server);
        }

        for ( int i=0; i<NUM_JP; i++ )
        {
            nEntityObject* agent = this->jp[i];

            if ( agent )
            {
                agent->GetComponentSafe<ncAIMovEngine>()->Draw (server);
            }
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    DrawCross
*/
void
RnsAiState::DrawCross (nGfxServer2* server)
{
    vector4 color (1.f, 0.f, 0.f, 1.f);
    vector3 vertical[2];
    vector3 horizontal[2];
    float length = 0.1f;

    vertical[0]     = this->shotPosition + vector3 (-length, 0, 0);
    vertical[1]     = this->shotPosition + vector3 (+length, 0, 0);
    horizontal[0]   = this->shotPosition + vector3 (0, 0, -length);
    horizontal[1]   = this->shotPosition + vector3 (0, 0, +length);

    server->BeginLines();
    server->DrawLines3d (vertical, 2, color);
    server->DrawLines3d (horizontal, 2, color);

    vertical[0]     = this->shotPosition + vector3 (-length, 0, -0.001f);
    vertical[1]     = this->shotPosition + vector3 (+length, 0, -0.001f);
    horizontal[0]   = this->shotPosition + vector3 (-0.001f, 0, -length);
    horizontal[1]   = this->shotPosition + vector3 (-0.001f, 0, +length);
    server->DrawLines3d (vertical, 2, color);
    server->DrawLines3d (horizontal, 2, color);

    server->EndLines();
}

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsAiState::HandleInput(const float frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    if( inputServer->GetButton("exit") )
    {
        this->app->SetQuitRequested(true);
    }
    if ( inputServer->GetButton("wait") )
    {
        nGPActionManager * action = nGPActionManager::Instance();
        n_assert(action);
        
        if ( action )         
        {
            nArg arg1[2]; 
            nArg arg2[4]; 

            // Shot
            vector3 viewPosition(this->ConvertScreenToViewSpace( vector2(0.5, 0.5) ));

            vector3 position( this->viewMatrix.pos_component());
            this->shotDirection = this->viewMatrix * viewPosition;
            this->shotDirection = this->shotDirection - position;

            this->shot->SetLength( 1000 );
            this->shot->SetDirection( this->shotDirection );
            this->shot->SetPosition( position );

            nPhyCollide::nContact contact;
            int num( nPhysicsServer::Instance()->Collide( this->shot, 1, &contact ) );

            if ( num )
            {
                nPhysicsGeom * geom = contact.GetGeometryA();

                if ( geom  ) // && geom->HasAttributes (nPhysicsGeom::ground) ) // OUTDOOR TEST
                {
                    this->shotObj = geom->Id();
                    contact.GetContactPosition( this->shotPosition );
                }
            }
            arg2[0].SetO(this->entity);            
            arg2[1].SetF(this->shotPosition.x);
            arg2[2].SetF(0); //this->shotPosition.y);
            arg2[3].SetF(this->shotPosition.z);
            action->QueueAction( "ngpmovement", 4, arg2, true );
            // espera
            arg1[0].SetO(this->entity);
            arg1[1].SetI(n_rand_int(30));
            action->QueueAction( "ngpwaitsometime", 2, arg1, true );
        }
    }

//#ifndef NGAME // SetTerrainVisible if not define
//	if( inputServer->GetButton("show_geometry") )
//	{
//		nSpatialServer* spatialServer = nSpatialServer::Instance();
//		n_assert(spatialServer);
//
//		if ( spatialServer )
//		{
//			spatialServer->SetTerrainVisible(!spatialServer->IsTerrainVisible());
//		}
//	}
//#endif
    // toggle console
    if( inputServer->GetButton("console") )
    {
        nConServer::Instance()->Toggle();
    }

    // toggle show
    if ( inputServer->GetButton("show_nav_mesh") || inputServer->GetButton("show_mesh_builder"))
    {
        this->showNavMesh = !this->showNavMesh;
    }

    if ( inputServer->GetButton("show_physics"))
    {
        this->showPhysics = !this->showPhysics;
    }

    if ( inputServer->GetButton("show_path_info") )
    {
        this->showPathInfo = !this->showPathInfo;
    }

    if ( inputServer->GetButton("show_path_nodes") )
    {
        this->showPathNodes = !this->showPathNodes;
    }

    if ( inputServer->GetButton("show_entity_debug") )
    {
        this->showEntityDebugInfo = !this->showEntityDebugInfo;
    }

    if ( inputServer->GetButton("reset_ai_state") )
    {
        this->InitAIEntities();
    }

    // Setup source/goal points
    if ( inputServer->GetButton("userkey1"))
    {
		//this->MoveEntity();
		//this->FaceEntity();
    }

    if ( inputServer->GetButton("userkey2"))
    {
        bool paused = this->entity->GetComponentSafe<ncAIMovEngine>()->IsPaused();
        
        this->entity->GetComponentSafe<ncAIMovEngine>()->SetPause (!paused);

        for ( int i=0; i<NUM_JP; i++ )
        {
            nEntityObject* agent = this->jp[i];

            if ( agent )
            {
                agent->GetComponentSafe<ncAIMovEngine>()->SetPause (!paused);
            }
        }
    }

    if ( inputServer->GetButton("PrimaryAttack") )
    {
        this->MoveEntity();
		//this->FaceEntity();
		//this->JumpEntity();
    }

	if ( inputServer->GetButton("SecondaryAttack") )
	{
		this->FaceEntity();
	}

    if ( inputServer->GetButton("userkey3"))
    {
        //vector3 position = this->entity.GetPosition();
        //vector3 final (0.05f, 0.f, -2.0f);
        
        //final += position;s
        //this->entity.Move (final);
    }

    // update camera
    float lookHori = 0.0f;
    float lookVert = 0.0f;

    lookHori = inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right");
    lookVert = inputServer->GetSlider("slider_down") - inputServer->GetSlider("slider_up");

    const float lookVelocity = 0.25f;
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    this->viewMatrix.ident();
    this->viewMatrix.rotate_x( this->viewerAngles.theta );
    this->viewMatrix.rotate_y( this->viewerAngles.rho );

    vector3 dirMove = viewMatrix * vector3( 0, 0, 1 );
    vector3 dirStep = viewMatrix * vector3( 1, 0 ,0 );

    float speed = 5.f;

    if( inputServer->GetButton("Forward") )
    {
        this->viewerPos -= dirMove*frameTime*10.0f*speed;
    }

    if( inputServer->GetButton("Backward") )
    {
        this->viewerPos += dirMove*frameTime*10.0f*speed;
    }

    if( inputServer->GetButton("StrafeLeft") )
    {
        this->viewerPos -= dirStep*frameTime*10.0f*speed;
    }

    if( inputServer->GetButton("StrafeRight") )
    {
        this->viewerPos += dirStep*frameTime*10.0f*speed;
    }

    this->viewMatrix.translate( this->viewerPos );

    //this->refViewport->SetViewMatrix( this->viewMatrix );
    this->refViewport->SetViewerAngles(this->viewerAngles);
    this->refViewport->SetViewerPos(this->viewerPos);
}

//------------------------------------------------------------------------------
/**
    CreateNavigationMesh
*/
void
RnsAiState::CreateNavigationMesh()
{
    // Get the navigation mesh for the outdoor
    nEntityObject * outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    n_assert2( outdoor, "An outdoor is required.");
    if ( outdoor )
    {
        this->navMesh = outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh();
        n_assert2( this->navMesh->IsValid(), "A navigation mesh for the outdoor is required.");
    }

    // Initializes the pathfinder
    this->CreatePathFinder();
    this->pathFinder->SetMesh (this->navMesh);
}

//------------------------------------------------------------------------------
/**
    CreatePathFinder
*/
void
RnsAiState::CreatePathFinder()
{
    this->pathFinder = static_cast<nPathFinder*>(nKernelServer::Instance()->New("npathfinder"));
}

//------------------------------------------------------------------------------
/**
    MoveEntity
*/
void
RnsAiState::MoveEntity()
{
    if ( this->shotObj )
    {
		vector3 facingPoint(50.0f, 0, 50.f);
		ncAIMovEngine* movEngine = this->entity->GetComponentSafe<ncAIMovEngine>();
		n_assert (movEngine);

		if ( movEngine )
		{
			//movEngine->SetStyle (nPathFinder::PATH_ZIGZAG);
			movEngine->MoveTo (this->shotPosition);
			//movEngine->MoveToFacing (this->shotPosition, facingPoint);
		}
    }	
}

//------------------------------------------------------------------------------
/**
	FaceEntity
*/
void
RnsAiState::FaceEntity()
{
	if ( this->shotObj )
	{
		this->entity->GetComponentSafe <ncAIMovEngine>()->FaceTo (this->shotPosition);
	}	
}

//------------------------------------------------------------------------------
/**
	JumpEntity
*/
void
RnsAiState::JumpEntity()
{
	if ( this->shotObj )
	{
		this->entity->GetComponentSafe <ncAIMovEngine>()->Jump();
	}	
}

//------------------------------------------------------------------------------
/**
    MoveToMouse
*/
void
RnsAiState::MoveToMouse()
{
    nInputServer* inputServer = nInputServer::Instance();
    n_assert(inputServer);
    nInputEvent* event = inputServer->FirstEvent();
    n_assert(event);

    int mouseX = event->GetAbsXPos();
    int mouseY = event->GetAbsYPos();

    vector2 mousePosition (float(mouseX), (float)mouseY);
    vector3 screenPosition = this->ConvertScreenToViewSpace (mousePosition);
    vector3 entityPosition (screenPosition.x, 0.f, -screenPosition.y);

    this->entity->GetComponentSafe<ncAIMovEngine>()->MoveTo (entityPosition);
}

//------------------------------------------------------------------------------
/**
    SetPathPoint
*/
void
RnsAiState::SetPathPoint()
{
    if ( this->shotObj )
    {
        // Get the position
        if ( !this->bStart )
        {
            this->start = this->shotPosition;
            this->bStart = true;
//            this->pathFinder->SetStart (this->start);
        }
        else
        {
            this->goal = this->shotPosition;
            this->bStart = false;
  //          this->pathFinder->SetGoal (this->goal);

//            this->pathFinder->FindPath (this->start, this->goal);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param screenCoord position in the screen
    @returns position in the space
*/
vector3 
RnsAiState::ConvertScreenToViewSpace( const vector2& screenCoord )
{
   // get current inverted projection matrix
   matrix44 invProj = nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection);
   invProj.invert();

   vector3 screenCoord3D((screenCoord.x - 0.5f) * 2.0f, -(screenCoord.y - 0.5f) * 2.0f, 1.0f);
   vector3 viewCoord = invProj * screenCoord3D;

   // get near plane
   float nearZ = nGfxServer2::Instance()->GetCamera().GetNearPlane();
   return viewCoord * nearZ * 1.1f;
}

//------------------------------------------------------------------------------
/**
    InitAISystem
*/
void
RnsAiState::InitAISystem()
{
    this->actionManager = static_cast<nGPActionManager*>(nObject::kernelServer->New("ngpactionmanager"));
    this->InitAIEntities();
}

//------------------------------------------------------------------------------
/**
    InitAIEntities
*/
void
RnsAiState::InitAIEntities()
{
    // Initializes the entity
    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();
    n_assert( entityServer );
    nstream args;

    if ( entityServer )
    {
        vector3 offset = vector3 (17.f, 0.f, 0.f);
        vector3 position = this->navMesh->GetCenterPosition() + offset;
        //float initial = 10.f;

        this->entity = entityServer->NewLocalEntityObject ("Jeanphy");

        ncPhysicsObj* phyObject(this->entity->GetComponent<ncPhysicsObj>());

        n_assert2( phyObject, "It should have a physics component." );

        if( !phyObject->GetWorld() )
        {
            phyObject->InsertInTheWorld();
        }

        // FIXME: remove this
        nLevelManager::Instance()->GetCurrentLevel()->AddEntity(this->entity);
        this->entity->GetComponentSafe<ncTransform>()->SetPosition (position);        
    }
}

//------------------------------------------------------------------------------
/**
    ShutDownAISystem
*/
void
RnsAiState::ShutDownAISystem()
{
	if ( this->pathFinder )
    {
        this->pathFinder->Release();
        this->pathFinder = 0;
    }

    if ( this->actionManager )
    {
        this->actionManager->Release();
        this->actionManager = 0;
    }

    // Shutdown all ENTITIES DINAMICLY CREATED 
    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();
    n_assert (entityServer);

    if ( this->entity )
    {
        entityServer->RemoveEntityObject (this->entity);
        this->entity = 0;
    }

    this->pathFinder = 0;
    this->actionManager = 0;
}

//------------------------------------------------------------------------------
/**
    TestStream
*/
void
RnsAiState::TestStream()
{
    nstream test;
    int i= INT_MAX;
    float f= 15.34f;
    bool b = false;
    char string[80];
    vector3 v3(1.f, 2.f, 3.f);
    vector4 v4(4.f, 3.f, 2.f, 1.f);

    test.SetWrite (true);
    test.UpdateBool (b);
    test.UpdateInt(i);
    test.UpdateFloat(f);
    test.UpdateString("stream test");    
    test.UpdateVector3(v3);
    test.UpdateVector4(v4);

    // Recovering
    test.SetWrite (false);
    test.UpdateBool (b);
    test.UpdateInt(i);
    test.UpdateFloat(f);
    test.UpdateString(string);
    test.UpdateVector3(v3);
    test.UpdateVector4(v4);

    // Output
    FILE* fp = fopen ("E:\\streamtest.txt", "w+");

    if ( fp )
    {
        fprintf (fp, "Bool: %s\n", b ? "true" : "false");
        fprintf (fp, "Float: %4f\n", f);
        fprintf (fp, "Int: %i\n", i);
        fprintf (fp, "String: %s\n", string);
        fprintf (fp, "Vector3: (%4f, %4f, %4f)\n", v3.x, v3.y, v3.z);
        fprintf (fp, "Vector4: (%4f, %4f, %4f, %4f)\n", v4.x, v4.y, v4.z, v4.w);

        fclose (fp);
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
