#include "snakes/snakesapp.h"

#include "snakes/snakes.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "util/nrandomlogic.h"

//------------------------------------------------------------------------------

void SnakesApp::Init()
{
	//run script that loads required resources, etc.

	//and sets initial position of everything

}

//------------------------------------------------------------------------------

bool SnakesApp::Open()
{
    nGfxServer2* pGfxServer = nGfxServer2::Instance();
    
    //board mesh- square-
    this->refBoardMesh = pGfxServer->NewMesh( "board" );
    if ( !this->LoadResource( this->refBoardMesh, "proj:meshes/plane_xz.n3d2" ) )
        return false;

    //board shader- solid unlit
    this->refBoardShader = pGfxServer->NewShader( "solid" );
    if ( !this->LoadResource( this->refBoardShader, "proj:shaders/diffuse.fx" ) )
        return false;

    //board texture- simple tiled floor
    this->refBoardTexture = pGfxServer->NewTexture( "acera2" );
    if ( !this->LoadResource( this->refBoardTexture, "proj:textures/sidewalk.dds" ) )
        return false;

    //snake mesh- simple sphere
    this->refSnakeMesh = pGfxServer->NewMesh( "sphere" );
    if ( !this->LoadResource( this->refSnakeMesh, "proj:meshes/sphere.n3d2" ) )
        return false;

    //snake shader- solid, lit, shadow
    this->refSnakeShader = pGfxServer->NewShader( "default" );
    if ( !this->LoadResource( this->refSnakeShader, "proj:shaders/default.fx" ) )
        return false;

    //setup a board for the sphere to move-
    //this is roughly a level- when a level is completed, a new level is added
    this->board.fWidth = 50;
    this->board.fHeight = 40;
    n_setseed( int( nTimeServer::Instance()->GetTime() * 12345 ) );
    for ( int index = 0; index < 10; ++index )
    {
        Seed newSeed;
        newSeed.order = index;
        do
        {
            newSeed.vPosition.x = 2.f + n_rand_real( 1.f ) * (this->board.fWidth - 2.f);
            newSeed.vPosition.z = n_rand_real( 1.f ) * this->board.fHeight;
        }
        while ( board.FindSeed( newSeed.vPosition, 1.f ) );
        this->board.seeds.PushBack( newSeed );
    }

	//TODO- load these from a script instead?
	//create the snake object(s)
    SnakeState localSnake;
    localSnake.pSnake = n_new(Snake);
	localSnake.pSnake->SetRadius( 1.f );
	localSnake.pSnake->SetSpeed( 8.f );
	localSnake.pSnake->SetNumJoints( 15 );
    //the local snake always starts at spawn point 0
    //further snakes occupy other slots
	localSnake.pSnake->SetPosition( vector3(1.f,0.f,1.f) );
	localSnake.pSnake->SetForward( vector3(0.f,0.f,1.f) );

	localSnake.pSnake->Initialize();

    //first seed to collect is 0
    localSnake.order = 0;

    this->snakes.Append( localSnake );

    this->gameState = State_Game;

    return true;
}

//------------------------------------------------------------------------------

void SnakesApp::Close()
{
    this->refBoardMesh->Release();
    this->refBoardShader->Release();
    this->refBoardTexture->Release();
    this->refSnakeMesh->Release();
    this->refSnakeShader->Release();

    for ( int index = 0; index < this->snakes.Size(); ++index )
    {
        n_delete( this->snakes.At(index).pSnake );
    }

    this->snakes.Reset();
}

//------------------------------------------------------------------------------

void SnakesApp::Tick( float fTimeElapsed )
{
    nInputServer* pInputServer( nInputServer::Instance() );

    switch ( this->gameState )
    {
        case State_Menu:
            //Start Menu
            break;
        
        case State_Score:
        //Score Board
            //Tick()
            break;
        
        case State_Game:
        {
            //read input, update orientation of the snake head
            SnakeState& localSnake = this->snakes.At(0);

            if ( pInputServer->GetButton( "reset" ) )
            {
                localSnake.pSnake->Initialize();
            }

            if ( pInputServer->GetButton( "left" ) )
            {
                localSnake.pSnake->NewPathNode();
                localSnake.pSnake->SetForward( vector3(1.f,0.f,0.f) );
            }
            else if ( pInputServer->GetButton( "right" ) )
            {
                localSnake.pSnake->NewPathNode();
                localSnake.pSnake->SetForward( vector3(-1.f,0.f,0.f) );
            }
            else if ( pInputServer->GetButton( "up" ) )
            {
                localSnake.pSnake->NewPathNode();
                localSnake.pSnake->SetForward( vector3(0.f,0.f,1.f) );
            }
            else if ( pInputServer->GetButton( "down" ) )
            {
                localSnake.pSnake->NewPathNode();
                localSnake.pSnake->SetForward( vector3(0.f,0.f,-1.f) );
            }

            //tick all snakes in the game
            for ( int index = 0; index < this->snakes.Size(); ++index )
            {
                SnakeState& curSnake = this->snakes[index];

                //advance their position using their speed and orientation
                curSnake.pSnake->Tick( fTimeElapsed );

                //check for going out-of-board
                vector3 vPosition( curSnake.pSnake->GetPosition() );
                if ( !this->board.Contains( vPosition ) )
                {
                    curSnake.pSnake->Freeze();
                }

                //check for collision with itself or another snake
                if ( curSnake.pSnake->CheckCollision( curSnake.pSnake ) )
                {
                    //move snake to initial position, then initialize
                    //curSnake->Initialize();
                    curSnake.pSnake->Freeze();
                }

                Seed* pSeed = this->board.FindSeed( curSnake.pSnake->GetPosition(), curSnake.pSnake->GetRadius() );
                if ( pSeed )
                {
                    //check if it is the current one, otherwise freeze
                    if ( pSeed->order == curSnake.order )//&& pSeed.color == curSnake.color
                    {
                        //remove the seed from the list
                        this->board.RemSeed( pSeed );

                        //collect next seed
                        curSnake.order = curSnake.order + 1;

                        //grow
                        curSnake.pSnake->AddJoint();

                        //end game if we've run out of seeds
                        if ( this->board.seeds.Empty() )
                        {
                            curSnake.pSnake->Freeze();
                            //this->gameState = State_Score;
                        }
                    }
                    else
                    {
                        curSnake.pSnake->Freeze();
                    }
                }
            }

        //TODO- tick the camera to follow the head of the local snake
        }
    }
}

//------------------------------------------------------------------------------

void SnakesApp::Render()
{
    nGfxServer2* pGfxServer = nGfxServer2::Instance();
    pGfxServer->Clear( nGfxServer2::AllBuffers, .3f, .3f, .3f, 1.f, 1.f, 0 );

    //switch ( gamestate )
    //Start Menu- draw options
    
    //Score Board- draw results
    
    //Game- draw board, snakes, shadows, debug options
    //case GameMode
    {
        Snake* localSnake = this->snakes.At(0).pSnake;

        //set camera view
        matrix44 matView;
        vector3 vCamera( localSnake->GetPosition() );//follow the head of the snake
        //vector3 vForward( localSnake->GetForward() );
        vector3 vForward( vector3(0.f,0.f,1.f) );
        vCamera -= vForward * 20.f;//distance to camera
        vCamera += vector3( 0.0f, 30.0f, 0.0f );//distance above the ground
        matView.rotate_x( n_deg2rad(-60) );
        //keep constant camera angle
        matView.rotate_y( n_deg2rad(180) );
        //matView.rotate_y( n_acos( vForward  % vector3(0.f, 0.f, -1.f) ) );//FIXME- why -1???
        matView.translate( vCamera );
        //matView.lookatLh( /*to*/ vector3( 0.0f, 0.0f, 0.0f ), /*up*/ vector3( 0.0f, 1.0f, 0.0f ) );
        //matView.lookatLh( localSnake->GetPosition(), vector3( 0.f, 1.f, 0.f ) );
        matView.invert_simple();
        pGfxServer->SetTransform( nGfxServer2::View, matView );

        nCamera2 cam;
        pGfxServer->SetCamera( cam );

        //draw board
        int nPasses = this->BeginDraw( this->refBoardShader, this->refBoardMesh );
        for ( int index = 0; index < nPasses; ++index )
        {
            this->BeginPass( this->refBoardShader, index );

            //the board mesh is 10 units wide, so we need to draw once per each 10 units
            int nRows = int( this->board.fHeight / 10.f );
            int nCols = int( this->board.fWidth / 10.f );
            for ( int iRow = 0; iRow < nRows; ++iRow )
            {
                for ( int iCol = 0; iCol < nCols; ++iCol )
                {
                    //compute position of the 10x10 square for the current row and col
                    vector3 vScale( 10.f, 10.f, 10.f );
                    vector3 vPosition( 10.f * iCol, 0.f, 10.f * iRow );
                    //set material values and textures
                    //this->refBoardShader->SetInt( nShaderState::FillMode, nShaderState::Wireframe );
                    this->refBoardShader->SetTexture( nShaderState::DiffMap0, this->refBoardTexture );
                    this->Draw( vPosition, vScale );
                }
            }

            this->EndPass( this->refBoardShader );
        }

        this->EndDraw( this->refBoardShader );

        //TODO- draw seeds with a colored number each
        for ( int index = 0; index < this->board.seeds.Size(); ++index )
        {
            Seed& curSeed = this->board.seeds.At(index);

            nString strValue( curSeed.order );
            vector4 vColor( 0.f, 1.f, 0.f, 1.f );
            matrix44 matViewProj( matView );
            matViewProj *= pGfxServer->GetCamera().GetProjection();

            vector4 vClipPos = matViewProj * vector4( curSeed.vPosition );
            if ( vClipPos.x < -vClipPos.w || vClipPos.x > vClipPos.w || 
                 vClipPos.y < -vClipPos.w || vClipPos.y > vClipPos.w )
            {
                continue;
            }
            vClipPos.x = vClipPos.x / vClipPos.w;
            vClipPos.y = -vClipPos.y / vClipPos.w;
            pGfxServer->Text( strValue.Get(), vColor, vClipPos.x, vClipPos.y );
        }

        //draw snakes, waypoints and seeds
        nPasses = this->BeginDraw( this->refSnakeShader, this->refSnakeMesh );
        for ( int index = 0; index < nPasses; ++index )
        {
            this->BeginPass( this->refSnakeShader, index );

            for ( int iJoint = 0; iJoint < localSnake->GetNumJoints(); ++iJoint )
            {
                vector4 vColor;
                if ( localSnake->IsFrozen() )
                    vColor.set(0.f,0.f,1.f,1.f);
                else if ( iJoint == 0 )
                    vColor.set(1.f,0.f,0.f,1.f);
                else
                    vColor.set(1.f,1.f,0.f,1.f);
                this->refSnakeShader->SetVector4( nShaderState::matDiffuse, vColor );

                const SnakeJoint& joint = localSnake->GetJointAt(iJoint);
                vector3 vScale = vector3(.5f,.5f,.5f) * localSnake->GetRadius();
                //matWorld.set_translation( localSnake->GetPosition() );
                vector3 vPosition( joint.vPosition );
                vPosition.y = localSnake->GetRadius();
                this->Draw( vPosition, vScale );
            }

            //draw the path nodes
            /*this->refSnakeShader->SetVector4( nShaderState::matDiffuse, vector4(0.f,1.f,0.f,1.f) );
            PathNode* curNode = localSnake->GetPath();
            while ( curNode )
            {
                vector3 vScale(.3f,.3f,.3f);
                vector3 vPosition( curNode->vPosition );
                this->Draw( vPosition, vScale );

                curNode = localSnake->GetNextNode( curNode );
            }*/

            //draw the seeds
            for ( int index = 0; index < this->board.seeds.Size(); ++index )
            {
                Seed& curSeed( this->board.seeds.At(index) );
                this->refSnakeShader->SetVector4( nShaderState::matDiffuse, vector4(0.f,1.f,0.f,1.f) );
                vector3 vPosition( curSeed.vPosition );
                vector3 vScale( .5f,.5f,.5f );
                this->Draw( vPosition, vScale );
            }

            this->EndPass( refSnakeShader );
        }
        this->EndDraw( refSnakeShader );
    }

    //TODO- draw console and text on top of everything
}
