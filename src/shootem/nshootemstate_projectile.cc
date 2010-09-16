#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nshootemstate_projectile.cc
//  (C) 2010 M.A. Garcias <ma@magarcias.com>
//------------------------------------------------------------------------------
#include "shootem/nshootemstate.h"

#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nmesh2.h"

void nShootemState::AddProjectile()
{
    Projectile& projectile = this->projectiles.PushBack(Projectile());
    vector3 pos(this->playerPos);
    pos.y += 1.0f;
    projectile.vecPos.set(pos);//TODO- add offset
    projectile.vecRot.set(this->playerRot);
    matrix44 matWorld;
    matWorld.rotate_y(this->playerRot.y);
    projectile.vecDir = matWorld * vector3(0,0,1);
    projectile.vecSize.set( .1f, .1f, .1f );
    projectile.fTimeElapsed = 0.f;
}

//------------------------------------------------------------------------------

void nShootemState::TickProjectiles(float fTimeElapsed)
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
        //Enemy* enemy = this->CheckEnemies(proj.vecPos, proj.vecSize.x);
        //if (enemy)
        //{
        //    this->OnEnemyHit(enemy);
        //    projectiles.EraseQuick(index);
        //    continue;
        //}

        index++;
    }
}

//------------------------------------------------------------------------------

void nShootemState::DrawProjectiles()
{
    n_assert(this->refShaderColor.isvalid());
    n_assert(this->refMeshCone.isvalid());

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetShader(this->refShaderColor);
    this->refShaderColor->SetVector4( nShaderState::MatDiffuse, vector4(1,0,0,1) );

    int numPasses = this->refShaderColor->Begin(false);
    
    for (int pass=0; pass<numPasses; pass++)
    {
        this->refShaderColor->BeginPass(pass);

        gfxServer->SetMesh(this->refMeshCone, this->refMeshCone);
        gfxServer->SetVertexRange(0, this->refMeshCone->GetNumVertices());
        gfxServer->SetIndexRange(0, this->refMeshCone->GetNumIndices());

        int numProjectiles = this->projectiles.Size();
        for (int index=0; index<numProjectiles; index++)
        {
            const Projectile& proj = this->projectiles.At(index);
            matrix44 matWorld;
            matWorld.scale(proj.vecSize);
            matWorld.rotate_y(proj.vecRot.y);
            matWorld.translate(proj.vecPos);
            gfxServer->SetTransform(nGfxServer2::Model, matWorld);
            gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
        }

        this->refShaderColor->EndPass();
    }

    this->refShaderColor->End();
}
