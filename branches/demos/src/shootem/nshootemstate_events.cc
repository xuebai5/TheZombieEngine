#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nshooterstate_events.cc
//  (C) 2010 M.A. Garcias <ma@magarcias.com>
//------------------------------------------------------------------------------
#include "shootem/nshootemstate.h"

void nShootemState::OnEnemyHit(Enemy* enemy)
{
    n_assert(enemy);
    enemy->hitPoints--;
    if (enemy->hitPoints > 0)
        enemy->state = ES_Hit;
    else
        enemy->state = ES_Dying;

    enemy->fTimeElapsed = 0.f;
}

void nShootemState::OnPlayerHit()
{

}

void nShootemState::OnLevelEnd()
{

}
