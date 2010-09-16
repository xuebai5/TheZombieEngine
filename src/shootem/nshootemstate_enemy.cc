#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nshooterstate_enemy.cc
//  (C) 2010 M.A. Garcias <ma@magarcias.com>
//------------------------------------------------------------------------------
#include "shootem/nshootemstate.h"

#include "zombieentity/nctransform.h"
#include "nspatial/ncspatial.h"

//------------------------------------------------------------------------------

void nShootemState::InitEnemies()
{
    nClass* enemyClass = nKernelServer::Instance()->FindClass("Scv");

    //find entities corresponding to enemies, hide them until spawn
    nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject();
    while (entity)
    {
        //check the class
        if (entity->IsA(enemyClass))
        {
            ncTransform* transform = entity->GetComponentSafe<ncTransform>();
            ncSpatial* spatial = entity->GetComponentSafe<ncSpatial>();
            const vector3& size = spatial->GetBBoxExtents();
            float radius2D = size.x * size.x + size.z * size.z;

            Enemy newEnemy;
            newEnemy.vecPos = transform->GetPosition();
            newEnemy.radius2D = radius2D;
            newEnemy.color.set(1,1,1,1);
            newEnemy.state = ES_Idle;
            newEnemy.hitPoints = 1;
            newEnemy.fTimeElapsed = 0;

            newEnemy.refEntity.set(entity);

            this->enemies.Append(newEnemy);
        }

        entity = nEntityObjectServer::Instance()->GetNextEntityObject();
    }
}

//------------------------------------------------------------------------------

void nShootemState::SpawnEnemies()
{
    //activate enemies at a distance from the player
    float distSq = this->fEnemySpawnDistance*this->fEnemySpawnDistance;
    for (int index=0; index<this->enemies.Size(); index++)
    {
        Enemy& enemy = this->enemies[index];
        if (enemy.state == ES_Idle)
        {
            if (vector3(this->playerPos - enemy.vecPos).lensquared() < distSq)
            {
                enemy.state = ES_Active;
            }
        }
    }
}

//------------------------------------------------------------------------------

void nShootemState::TickEnemies(float fTimeElapsed)
{
    //TEMP- now it only changes them from Idle to Active
    this->SpawnEnemies();

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
                enemy.state = ES_Active;
            //fall through

        case ES_Active:
            {
                vector3 vecMove(this->playerPos - enemy.vecPos);
                vecMove.y = 0;
                vecMove.norm();
                polar2 angles(vecMove);
                vecMove *= this->fEnemySpeed * fTimeElapsed;
                enemy.vecPos += vecMove;

                //adjust to terrain
                this->AdjustHeight( enemy.vecPos );

                //update the entity position
                ncTransform* transform = enemy.refEntity->GetComponentSafe<ncTransform>();
                transform->SetPosition(enemy.vecPos);
                transform->SetPolar(angles);
            }
            break;

        case ES_Dying:
            enemy.fTimeElapsed += fTimeElapsed;
            if (enemy.fTimeElapsed > this->fEnemyDyingTime)
            {
                nEntityObjectServer::Instance()->RemoveEntityObject(enemy.refEntity);
                this->enemies.EraseQuick(index);
                continue;
            }
        }

        index++;
    }
}

//------------------------------------------------------------------------------

nShootemState::Enemy* nShootemState::CheckEnemies(const vector3& pos, float radius)
{
    int numEnemies = this->enemies.Size();
    float radiusSq = radius * radius;
    for (int index=0; index<numEnemies; index++)
    {
        Enemy& enemy = this->enemies[index];
        if (enemy.state == ES_Dying)
            continue;

        vector3 dist(pos - enemy.vecPos);
        dist.y = 0;//compare in the XZ plane only
        if (dist.lensquared() < (enemy.radius2D + radiusSq))
            return &this->enemies[index];
    }
    return 0;
}
