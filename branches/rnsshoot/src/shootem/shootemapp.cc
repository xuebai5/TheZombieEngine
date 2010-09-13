#include "shootem/shootemapp.h"

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

    this->vecEye.set(0,5,3);
    this->vecRot.set(n_deg2rad(-60),0,0); //looking down 30 degrees

    this->vecPlayerPos.set(0,0,0);
    this->vecCameraOffset.set(0,5,3);
    this->fCameraThreshold = 2.f;

    this->fPlayerSpeed = 5.f;

    this->projectiles.Reset();

    this->fProjectileMaxTime = 2.f;
    this->fProjectileSpeed = 10.f;
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

    N_REF_LOAD_TEXTURE(this->refTextureGround, "checker", "proj:textures/checker.jpg");

    N_REF_LOAD_SHADER(this->refShaderColor, "color", "proj:shaders/color.fx");
    N_REF_LOAD_SHADER(this->refShaderDiffuse, "diffuse", "proj:shaders/diffuse.fx");

    return true;
}

//------------------------------------------------------------------------------

void ShootemApp::Close()
{
    N_REF_RELEASE(this->refMeshGround);
    N_REF_RELEASE(this->refMeshCylinder);
    N_REF_RELEASE(this->refMeshSphere);

    N_REF_RELEASE(this->refTextureGround);

    N_REF_RELEASE(this->refShaderColor);
    N_REF_RELEASE(this->refShaderDiffuse);
}

//------------------------------------------------------------------------------

void ShootemApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

    //toggle options
    if (inputServer->GetButton("wireframe"))
    {
        this->bWireframe = !this->bWireframe;
    }

    float moveSpace = this->fPlayerSpeed * fTimeElapsed;//=cameraSpeed

    //camera move
    vector3 vecMove;
    if (inputServer->GetButton("forward"))
    {
        vecMove.z -= moveSpace;
    }
    if (inputServer->GetButton("backward"))
    {
        vecMove.z += moveSpace;
    }
    if (inputServer->GetButton("strafe_left"))
    {
        vecMove.x -= moveSpace;
    }
    if (inputServer->GetButton("strafe_right"))
    {
        vecMove.x += moveSpace;
    }

    //update camera position
    this->vecPlayerPos += vecMove;

    //move the camera when the position of the player exceeds a margin
    vector3 eyePos = this->vecPlayerPos + this->vecCameraOffset;
    if ((eyePos.z - this->vecEye.z) > this->fCameraThreshold)
    {
        this->vecEye.z = eyePos.z - this->fCameraThreshold;
    }
    else if ((this->vecEye.z - eyePos.z) > this->fCameraThreshold)
    {
        this->vecEye.z = eyePos.z + this->fCameraThreshold;
    }

    //shoot
    if (inputServer->GetButton("fire"))
    {
        this->AddProjectile();
    }

    //update projectiles
    this->TickProjectiles(fTimeElapsed);
}

//------------------------------------------------------------------------------

void ShootemApp::AddProjectile()
{
    Projectile& projectile = this->projectiles.PushBack(Projectile());
    projectile.vecPos.set( vecPlayerPos );
    projectile.vecDir.set( 0.f, 0.f, -1.f );
    projectile.vecSize.set( .1f, .1f, .1f );
    projectile.fTimeElapsed = 0.f;
}

//------------------------------------------------------------------------------

void ShootemApp::TickProjectiles(float fTimeElapsed)
{
    int index=0;
    while (index < projectiles.Size())
    {
        Projectile& proj = projectiles.At(index);
        proj.fTimeElapsed += fTimeElapsed;
        if (proj.fTimeElapsed > this->fProjectileMaxTime)
        {
            projectiles.EraseQuick(index);
            continue;
        }

        proj.vecPos += proj.vecDir * this->fProjectileSpeed * fTimeElapsed;
        //TODO- check for collisions
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

    nCamera2 cam;
    //cam.SetOrthogonal(10.f, 10.f, .1f, 5000.f);
    gfxServer->SetCamera( cam );

    //draw the player
    matrix44 matWorld;
    matWorld.scale(vector3(.5f,.5f,.5f));
    matWorld.rotate_x(n_deg2rad(90.f));
    matWorld.translate(this->vecPlayerPos);

    this->BeginDraw( this->refShaderColor, this->refMeshCylinder );
    this->BeginPass( this->refShaderColor, 0 );
    this->refShaderColor->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShaderColor->SetVector4( nShaderState::MatDiffuse, vector4(1,0,1,1) );
    this->Draw(matWorld);
    this->EndPass( this->refShaderColor );
    this->EndDraw( this->refShaderColor );

    //draw the bullets
    this->DrawProjectiles();

    //draw the tiles
    this->BeginDraw( this->refShaderDiffuse, this->refMeshGround );
    this->BeginPass( this->refShaderDiffuse, 0 );
    this->refShaderDiffuse->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShaderDiffuse->SetTexture( nShaderState::DiffMap0, this->refTextureGround );
    this->refShaderDiffuse->SetVector4( nShaderState::MatDiffuse, vector4(1,1,1,1) );
    this->Draw( vector3( -5.f, 0.f, -5.f ), vector3( 10.f, 0.f, 10.f ) );
    this->EndPass( this->refShaderDiffuse );
    this->EndDraw( this->refShaderDiffuse );

    //render text
    float rowheight = 32.f / gfxServer->GetDisplayMode().GetHeight();
    nString str;
    str.Format("Projectiles.Size = %d", this->projectiles.Size());
    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );//lower-left corner
}

//------------------------------------------------------------------------------

bool
ShootemApp::LoadResource( nResource* pResource, const nString& strFilename )
{
    pResource->SetFilename( strFilename );
    return pResource->Load();
}

//------------------------------------------------------------------------------

int
ShootemApp::BeginDraw(nShader2 *pShader, nMesh2 *pMesh)
{
    int nPasses = this->BeginDraw(pShader);
    this->BeginDraw(pMesh);
    return nPasses;
}

//------------------------------------------------------------------------------

int
ShootemApp::BeginDraw(nShader2 *pShader)
{
    nGfxServer2::Instance()->SetShader( pShader );
    int nPasses = pShader->Begin( false );
    return nPasses;
}


//------------------------------------------------------------------------------

void
ShootemApp::BeginDraw(nMesh2 *pMesh)
{
    nGfxServer2* pGfxServer = nGfxServer2::Instance();
    pGfxServer->SetMesh( pMesh, pMesh );
    pGfxServer->SetVertexRange( 0, pMesh->GetNumVertices() );
    pGfxServer->SetIndexRange( 0, pMesh->GetNumIndices() );
}

//------------------------------------------------------------------------------

void
ShootemApp::BeginPass(nShader2 *pShader, int passIndex)
{
    pShader->BeginPass( passIndex );
}

//------------------------------------------------------------------------------

void
ShootemApp::Draw( const vector3& vPosition, const vector3& vScale )
{
    matrix44 matWorld;
    matWorld.scale( vScale );
    matWorld.translate( vPosition );
    this->Draw( matWorld );
}

//------------------------------------------------------------------------------

void
ShootemApp::Draw( const matrix44& matWorld )
{
    nGfxServer2* gfxServer( nGfxServer2::Instance() );
    gfxServer->SetTransform( nGfxServer2::Model, matWorld );
    gfxServer->DrawIndexedNS( nGfxServer2::TriangleList );
}

//------------------------------------------------------------------------------

void
ShootemApp::EndPass( nShader2* pShader )
{
    pShader->EndPass();
}

//------------------------------------------------------------------------------

void
ShootemApp::EndDraw( nShader2* pShader )
{
    pShader->End();
}
