#include "shootem/shootemapp.h"
#include "shootem/shootemapp_model.h"

#include "models/model.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "util/nrandomlogic.h"

//------------------------------------------------------------------------------
/**
    run script that loads required resources, etc.
	and sets initial position of everything
*/
void ShootemApp::Init()
{
    this->bWireframe = false;
    this->bCameraOrtho = false;
    this->bShowBoxes = false;

    this->vecPlayerPos.set(0,0,0);
    this->vecPlayerRot.set(0,0,0);
    this->vecCameraOffset.set(0,3,-3);
    this->fCameraThreshold = 1.f;

    this->vecEye = this->vecPlayerPos + this->vecCameraOffset;
    //this->vecRot.set(n_deg2rad(60),0,0); //looking down 30 degrees
    this->vecRot.set(n_deg2rad(30),0,0);

    this->fPlayerSpeed = 5.f;
    this->fPlayerSize = 1.f;
    this->fTurnSpeed = n_deg2rad(360.f);

    this->fPlayerDyingTime = 2.f;
    this->fPlayerTimeElapsed = 0.f;

    this->fProjectileMaxTime = 2.f;
    this->fProjectileSpeed = 10.f;

    this->fEnemySpeed = 3.f;
    this->fEnemyHitTime = .2f;
    this->fEnemyDyingTime = 1.f;

    //initial level
    this->gameLevel = 1;
    this->ResetLevel();
}

//------------------------------------------------------------------------------

void ShootemApp::ResetLevel()
{
    //initialize player
    this->vecPlayerPos.set(0,0,0);

    this->fEnemySpeed = 3.f + float(gameLevel - 1);

    //initialize ground
    int numTiles = 4 + gameLevel;
    this->tiles.SetFixedSize(numTiles);

    min_x = -5.f;
    min_z = -5.f;
    max_x = min_x + 10.f;//tilesize
    max_z = min_z + 10.f * numTiles;//tilesize

    const vector4 colors[] = { vector4(1.0f, 1.0f, 1.0f, 1.0f),
                               vector4(0.0f, 1.0f, 0.0f, 1.0f),
                               vector4(0.0f, 1.0f, 1.0f, 1.0f),
                               vector4(1.0f, 0.0f, 0.0f, 1.0f),
                               vector4(1.0f, 0.0f, 1.0f, 0.0f) };

    vector3 tilePos( min_x, 0.f, min_z );

    for (int index=0; index<numTiles; index++)
    {
        this->tiles[index].vecPos = tilePos;
        this->tiles[index].vecScale.set( 10.f, 0.f, 10.f );
        this->tiles[index].color = colors[index % sizeof(colors)];
        tilePos.z += 10.f;
    }


    //initialize props
    {
        int numPropsPerTile = 10 + 2 * gameLevel;
        for (int tileIndex=0; tileIndex<numTiles; tileIndex++)
        {
            Tile& tile = this->tiles[tileIndex];
            float min_x = tile.vecPos.x;
            float min_z = tile.vecPos.z;
            for (int index=0; index<numPropsPerTile; index++)
            {
                Prop newProp;
                newProp.vecPos.x = min_x + n_rand_real(1.f) * tile.vecScale.x;//tilesize_x
                newProp.vecPos.z = min_z + n_rand_real(1.f) * tile.vecScale.z;//tilesize_z
                newProp.vecScale.set(1,1,1);
                newProp.highlight = false;
                //TODO- check for collision with other props?

                this->props.Append(newProp);
            }
        }
    }

    //initialize items (shoot and pick, or just pick)

    this->ResetGame();
}

//------------------------------------------------------------------------------

void ShootemApp::ResetGame()
{
    this->fPlayerTimeElapsed = 0.f;
    this->playerState = PS_Alive;

    this->projectiles.Reset();
    this->enemies.Reset();
}

//------------------------------------------------------------------------------
/**
*/
bool ShootemApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    N_REF_LOAD_MESH(this->refMeshGround, "plane", "proj:meshes/plane.n3d2");
    N_REF_LOAD_MESH(this->refMeshCylinder, "cylinder", "proj:meshes/cylinder.n3d2");
    N_REF_LOAD_MESH(this->refMeshSphere, "sphere", "proj:meshes/sphere.n3d2");
    N_REF_LOAD_MESH(this->refMeshCone, "cone", "proj:meshes/cone.nvx2");

    N_REF_LOAD_TEXTURE(this->refTextureGround, "checker", "proj:textures/checker.jpg");

    N_REF_LOAD_SHADER(this->refShaderColor, "color", "proj:shaders/color.fx");
    N_REF_LOAD_SHADER(this->refShaderDiffuse, "diffuse", "proj:shaders/diffuse.fx");

    if (!this->LoadModels()) return false;

    return true;
}

//------------------------------------------------------------------------------

void ShootemApp::Close()
{
    N_REF_RELEASE(this->refMeshGround);
    N_REF_RELEASE(this->refMeshCylinder);
    N_REF_RELEASE(this->refMeshSphere);
    N_REF_RELEASE(this->refMeshCone);

    N_REF_RELEASE(this->refTextureGround);

    N_REF_RELEASE(this->refShaderColor);
    N_REF_RELEASE(this->refShaderDiffuse);

    this->ReleaseModels();

    this->projectiles.Clear();
    this->tiles.Clear();
    this->props.Clear();
}

//------------------------------------------------------------------------------

void ShootemApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

    //toggle options
    if (inputServer->GetButton("wireframe"))
        this->bWireframe = !this->bWireframe;

    if (inputServer->GetButton("ortho"))
        this->bCameraOrtho = !this->bCameraOrtho;

    if (inputServer->GetButton("toggle"))
        this->bShowBoxes = !this->bShowBoxes;

    switch (this->playerState)
    {
    case PS_Dying:
        this->fPlayerTimeElapsed += fTimeElapsed;
        if (this->fPlayerTimeElapsed > this->fPlayerDyingTime)
        {
            this->ResetGame();
        }
        break;

    case PS_Alive:
        //camera rotate
        float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
        float mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));
        float angleSpace = this->fTurnSpeed * 10.f * fTimeElapsed;

        if (inputServer->GetButton("right_pressed"))
        {
            this->vecPlayerRot.y -= mouse_x * angleSpace;//unlike cameras, rotation is interpreted the other way
            this->vecPlayerRot.x -= mouse_y * angleSpace;
        }

        //camera move
        float moveSpace = this->fPlayerSpeed * fTimeElapsed;

        vector3 vecMove;
        if (inputServer->GetButton("forward"))
        {
            vecMove.z += moveSpace;
        }
        if (inputServer->GetButton("backward"))
        {
            vecMove.z -= moveSpace;
        }
        if (inputServer->GetButton("strafe_left"))
        {
            vecMove.x -= moveSpace;
        }
        if (inputServer->GetButton("strafe_right"))
        {
            vecMove.x += moveSpace;
        }

        //update player position
        vector3 playerPos = this->vecPlayerPos + vecMove;
        playerPos.x = n_clamp(playerPos.x, min_x, max_x);//adjust to tilesize

        Prop* prop = this->CheckProps(playerPos, this->fPlayerSize);
        if (prop)
        {
            prop->highlight = true;
            vector3 vecBack = playerPos - prop->vecPos;//push away
            vecBack.norm();
            vecBack *= this->fPlayerSize - vector3::distance(playerPos, prop->vecPos);
            playerPos += vecBack;
        }

        this->vecPlayerPos = playerPos;

        //end of level?
        if (this->vecPlayerPos.z > max_z)
        {
            this->OnLevelEnd();
        }

        //check for collisions with enemies
        Enemy* enemy = this->CheckEnemies(playerPos, this->fPlayerSize);
        if (enemy)
        {
            this->OnPlayerHit();
        }

        //update camera position applying threshold
        matrix44 matWorld;
        matWorld.rotate_y(this->vecPlayerRot.y);
        //matWorld.invert_simple();
        this->vecEye = this->vecPlayerPos + matWorld * this->vecCameraOffset;
        this->vecRot.set(n_deg2rad(30), this->vecPlayerRot.y, 0);//vecCameraRotOffset+vecPlayerRot
        
        //vector3 eyePos = this->vecPlayerPos + this->vecCameraOffset;
        //if ((eyePos.z - this->vecEye.z) > this->fCameraThreshold)
        //{
        //    this->vecEye.z = eyePos.z - this->fCameraThreshold;
        //}
        //else if ((this->vecEye.z - eyePos.z) > this->fCameraThreshold)
        //{
        //    this->vecEye.z = eyePos.z + this->fCameraThreshold;
        //}
        //this->vecEye.x = eyePos.x;
        
        //shoot
        if (inputServer->GetButton("fire"))
        {
            this->AddProjectile();
        }
    }

    //update projectiles
    this->TickProjectiles(fTimeElapsed);

    //update enemies
    this->TickEnemies(fTimeElapsed);
}

//------------------------------------------------------------------------------

void ShootemApp::AddProjectile()
{
    Projectile& projectile = this->projectiles.PushBack(Projectile());
    projectile.vecPos.set( vecPlayerPos );
    projectile.vecDir.set( 0.f, 0.f, 1.f );
    projectile.vecSize.set( .1f, .1f, .1f );
    projectile.fTimeElapsed = 0.f;
}

//------------------------------------------------------------------------------

void ShootemApp::TickProjectiles(float fTimeElapsed)
{
    int index=0;
    while (index < this->projectiles.Size())
    {
        Projectile& proj = this->projectiles.At(index);
        proj.fTimeElapsed += fTimeElapsed;
        if (proj.fTimeElapsed > this->fProjectileMaxTime)
        {
            projectiles.EraseQuick(index);
            continue;
        }

        proj.vecPos += proj.vecDir * this->fProjectileSpeed * fTimeElapsed;

        //TODO- check for collisions
        Enemy* enemy = this->CheckEnemies(proj.vecPos, proj.vecSize.x);
        if (enemy)
        {
            this->OnEnemyHit(enemy);
            projectiles.EraseQuick(index);
            continue;
        }

        index++;
    }
}

//------------------------------------------------------------------------------

void ShootemApp::DrawProjectiles()
{
    this->BeginDraw( this->refShaderColor, this->refMeshSphere );
    this->BeginPass( this->refShaderColor, 0 );
    this->refShaderColor->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShaderColor->SetVector4( nShaderState::MatDiffuse, vector4(1,0,0,1) );
    int numProjectiles = this->projectiles.Size();
    for (int index=0; index<numProjectiles; index++)
    {
        Projectile& proj = this->projectiles.At(index);
        this->Draw( proj.vecPos, proj.vecSize );
    }
    this->EndPass( this->refShaderColor );
    this->EndDraw( this->refShaderColor );
}

//------------------------------------------------------------------------------

void ShootemApp::DrawProps()
{
    //if (this->bShowBoxes)
    //{
        this->BeginDraw( this->refShaderColor, this->refMeshCone );
        this->BeginPass( this->refShaderColor, 0 );
        this->refShaderColor->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
        int numProps = this->props.Size();
        for (int index=0; index<numProps; index++)
        {
            Prop& prop = this->props.At(index);

            if (prop.highlight)
            {
                this->refShaderColor->SetVector4( nShaderState::MatDiffuse, vector4(1,1,0,1) );
                prop.highlight = false;
            }
            else
                this->refShaderColor->SetVector4( nShaderState::MatDiffuse, vector4(0,1,0,1) );

            matrix44 matWorld;
            matWorld.scale(prop.vecScale);
            matWorld.rotate_x(n_deg2rad(90.f));
            vector3 pos(prop.vecPos);
            pos += vector3(0,prop.vecScale.y,0);//model offset
            matWorld.translate(pos);
            this->Draw(matWorld);
        }
        this->EndPass( this->refShaderColor );
        this->EndDraw( this->refShaderColor );
    //}
    //else
    //{
    //    int numProps = this->props.Size();
    //    for (int index=0; index<numProps; index++)
    //    {
    //        Prop& prop = this->props.At(index);

    //        matrix44 matWorld;
    //        vector3 scale(prop.vecScale);
    //        scale *= .3f;
    //        matWorld.scale(scale);
    //        matWorld.translate(prop.vecPos);

    //        this->DrawModel(this->models[Model_Tree], matWorld);
    //    }
    //}
}

//------------------------------------------------------------------------------

ShootemApp::Prop* ShootemApp::CheckProps(const vector3& pos, float fDistance)
{
    int numProps = this->props.Size();
    float fDistSq = fDistance * fDistance;
    for (int index=0; index<numProps; index++)
    {
        vector3 dist(pos - this->props[index].vecPos);
        if (dist.lensquared() < fDistSq)
            return &this->props[index];
    }
    return 0;
}

//------------------------------------------------------------------------------

void ShootemApp::SpawnEnemies()
{
    //create a wave of enemies by instancing a row of them
    //the spawn point is a fixed number of units in front of the player
    //they follow a fixed trajectory (a path) and can shoot projectiles

    int numEnemies = 5 + (gameLevel - 1) * 3;
    vector3 vecPos = this->vecPlayerPos;
    vecPos.z += 10.f;

    if (vecPos.z > max_z) return;

    //float min_x = -5.f;
    //do {
        vecPos.x = 1.f;//min_x + n_rand_real(1.f) * 10.f;
    //} while( this->CheckProps(vecPos, 1.f));//size

    for (int index=0; index<numEnemies; index++)
    {
        Enemy newEnemy;
        newEnemy.state = ES_Alive;
        newEnemy.hitPoints = this->gameLevel;
        newEnemy.vecPos = vecPos;
        newEnemy.vecScale.set(1.f,1.f,1.f);
        newEnemy.color.set(1,0,0,1);
        this->enemies.Append(newEnemy);

        vecPos.z += 2.f;//spread
    }
}

//------------------------------------------------------------------------------

void ShootemApp::TickEnemies(float fTimeElapsed)
{
    if (this->enemies.Empty())
    {
        this->SpawnEnemies();
    }

    int index=0;
    while (index < this->enemies.Size())
    {
        //TODO- depending on state
        Enemy& enemy = this->enemies.At(index);
        switch (enemy.state)
        {
        case ES_Hit:
            enemy.fTimeElapsed += fTimeElapsed;
            if (enemy.fTimeElapsed > this->fEnemyHitTime)
                enemy.state = ES_Alive;
            //fall through

        case ES_Alive:
            {
                vector3 vecMove(0,0,-1);
                vecMove.x = float(n_sgn(this->vecPlayerPos.x - enemy.vecPos.x));
                vecMove.norm();
                vecMove *= this->fEnemySpeed * fTimeElapsed;
                enemy.vecPos += vecMove;
                if (enemy.vecPos.z < this->vecPlayerPos.z - 10.f)
                {
                    this->enemies.EraseQuick(index);
                    continue;
                }
            }
            break;

        case ES_Dying:
            enemy.fTimeElapsed += fTimeElapsed;
            if (enemy.fTimeElapsed > this->fEnemyDyingTime)
            {
                this->enemies.EraseQuick(index);
                continue;
            }
        }

        index++;
    }
}

//------------------------------------------------------------------------------

ShootemApp::Enemy* ShootemApp::CheckEnemies(const vector3& pos, float fDistance)
{
    int numEnemies = this->enemies.Size();
    float fDistSq = fDistance * fDistance;
    for (int index=0; index<numEnemies; index++)
    {
        if (this->enemies[index].state == ES_Dying)
            continue;

        vector3 dist(pos - this->enemies[index].vecPos);
        if (dist.lensquared() < fDistSq)
            return &this->enemies[index];
    }
    return 0;
}

//------------------------------------------------------------------------------

void ShootemApp::OnEnemyHit(Enemy* enemy)
{
    n_assert(enemy);
    enemy->hitPoints--;
    if (enemy->hitPoints > 0)
        enemy->state = ES_Hit;
    else
        enemy->state = ES_Dying;

    enemy->fTimeElapsed = 0.f;
}

//------------------------------------------------------------------------------

void ShootemApp::OnPlayerHit()
{
    this->playerState = PS_Dying;
    this->fPlayerTimeElapsed = 0.f;
}

//------------------------------------------------------------------------------

void ShootemApp::OnLevelEnd()
{
    this->gameLevel++;
    this->ResetLevel();
}

//------------------------------------------------------------------------------

void ShootemApp::DrawEnemies()
{
    if (this->bShowBoxes)
    {
        this->BeginDraw( this->refShaderColor, this->refMeshCylinder );
        this->BeginPass( this->refShaderColor, 0 );
        this->refShaderColor->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );

        int numEnemies = this->enemies.Size();
        for (int index=0; index<numEnemies; index++)
        {
            Enemy& enemy = this->enemies.At(index);

            vector4 color(enemy.color);
            if (enemy.state == ES_Hit)
                color.lerp(vector4(1,1,0,1), enemy.fTimeElapsed / this->fEnemyHitTime);
            if (enemy.state == ES_Dying)
                color.lerp(enemy.color, vector4(0,0,0,1), enemy.fTimeElapsed / this->fEnemyDyingTime );

            this->refShaderColor->SetVector4( nShaderState::MatDiffuse, color );

            matrix44 matWorld;
            matWorld.scale(enemy.vecScale);
            matWorld.scale(vector3(.5f,.5f,.5f));
            matWorld.rotate_x(n_deg2rad(90.f));
            matWorld.translate(enemy.vecPos + vector3(0,1,0));

            this->Draw(matWorld);
        }
        this->EndPass( this->refShaderColor );
        this->EndDraw( this->refShaderColor );
    }
    else
    {
        int numEnemies = this->enemies.Size();
        for (int index=0; index<numEnemies; index++)
        {
            Enemy& enemy = this->enemies.At(index);

            vector4 color(1,1,1,1);
            if (enemy.state == ES_Hit)
                color.lerp(vector4(1,1,0,1), enemy.fTimeElapsed / this->fEnemyHitTime);
            if (enemy.state == ES_Dying)
                color.lerp(color, vector4(0,0,0,1), enemy.fTimeElapsed / this->fEnemyDyingTime );

            this->shaderParams.SetArg(nShaderState::MatDiffuse, color);
            
            matrix44 matWorld;
            matWorld.scale(enemy.vecScale);
            matWorld.translate(enemy.vecPos);

            this->DrawModel(this->models[Model_Enemy], matWorld);
        }
    }
}

//------------------------------------------------------------------------------

void ShootemApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->Clear( nGfxServer2::AllBuffers, .3f, .3f, .3f, 1.f, 1.f, 0 );

    //view, projection matrices

    this->matView.ident();
    this->matView.rotate_x( this->vecRot.x );//pitch
    this->matView.rotate_y( this->vecRot.y );//yaw
    this->matView.translate( this->vecEye );
    this->matView.invert_simple();

    gfxServer->SetTransform(nGfxServer2::View, matView);

    //nCamera2 cam;
    //cam.SetOrthogonal(10.f, 10.f, .1f, 5000.f);
    //gfxServer->SetCamera( cam );
    if (this->bCameraOrtho)
        this->matProj.orthoLh(10.f, 10.f, 0.1f, 100.f);
    else
        this->matProj.perspFovLh(n_deg2rad(60.0f), float(4.0f / 3.0f), 0.1f, 100.f);

    gfxServer->SetTransform(nGfxServer2::Projection, matProj);

    //draw the player
    vector4 playerColor(1.f,1.f,1.f,1.f);//blue
    vector3 playerScale(1.f,1.f,1.f);
    if (this->bShowBoxes)
    {
        playerColor.set(0,0,1,1);//blue
        playerScale.set(.5f,.5f,.5f);
    }

    if (this->playerState == PS_Dying)
    {
        float lerpVal = this->fPlayerTimeElapsed / this->fPlayerDyingTime;
        playerColor.lerp(playerColor, vector4(0,0,0,0), lerpVal);
        playerScale.lerp(playerScale, vector3(.5f,.5f,0.f), lerpVal);
    }

    if (this->bShowBoxes)
    {
        matrix44 matWorld;
        matWorld.scale(playerScale);
        matWorld.rotate_x(n_deg2rad(90.f));
        matWorld.translate(this->vecPlayerPos);

        this->BeginDraw( this->refShaderColor, this->refMeshCylinder );
        this->BeginPass( this->refShaderColor, 0 );
        this->refShaderColor->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
        this->refShaderColor->SetVector4( nShaderState::MatDiffuse, playerColor );
        this->Draw(matWorld);
        this->EndPass( this->refShaderColor );
        this->EndDraw( this->refShaderColor );
    }
    else
    {
        matrix44 matWorld;
        //matWorld.rotate_x(this->vecPlayerRot.x);
        matWorld.rotate_y(this->vecPlayerRot.y);
        matWorld.scale(playerScale);
        matWorld.translate(this->vecPlayerPos);
        this->shaderParams.SetArg(nShaderState::MatDiffuse, playerColor);
        this->DrawModel(this->models[Model_Player], matWorld);
    }

    //draw the stage
    this->DrawProps();

    //draw the enemies
    this->DrawEnemies();

    //draw the bullets
    this->DrawProjectiles();

    //draw the tiles
    this->BeginDraw( this->refShaderDiffuse, this->refMeshGround );
    this->BeginPass( this->refShaderDiffuse, 0 );
    this->refShaderDiffuse->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShaderDiffuse->SetTexture( nShaderState::DiffMap0, this->refTextureGround );

    int numTiles = this->tiles.Size();
    for (int tileIndex=0; tileIndex<numTiles; tileIndex++)
    {
        this->refShaderDiffuse->SetVector4( nShaderState::MatDiffuse, this->tiles[tileIndex].color );
        this->Draw( this->tiles[tileIndex].vecPos, this->tiles[tileIndex].vecScale );
    }

    this->EndPass( this->refShaderDiffuse );
    this->EndDraw( this->refShaderDiffuse );

    //render text
    float rowheight = 32.f / gfxServer->GetDisplayMode().GetHeight();
    nString str;
    //score

    //debug text
    str.Format("Projectiles = %d, Enemies = %d", this->projectiles.Size(), this->enemies.Size());
    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );//lower-left corner
}
