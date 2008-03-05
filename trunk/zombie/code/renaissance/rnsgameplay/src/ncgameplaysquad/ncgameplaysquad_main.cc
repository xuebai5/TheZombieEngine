#include "precompiled/pchgameplay.h"

/**
    @file ncgameplaysquad_main.cc
*/

#include "ncgameplaysquad/ncgameplaysquad.h"
#include "entity/nentityserver.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplaySquad::ncGameplaySquad() : ncGameplayGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplaySquad::~ncGameplaySquad()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    SetSquadLeader
*/
void
ncGameplaySquad::SetSquadLeader (unsigned int idLeader)
{
    nEntityServer* entityServer = nEntityServer::Instance();
    n_assert(entityServer);

    if ( entityServer )
    {
        if ( idLeader !=0 )
        {
            this->leader = entityServer->GetEntityObject (idLeader);
        }
        else
        {
            this->leader = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    GetSquadLeader
*/
unsigned int
ncGameplaySquad::GetSquadLeader() const
{
    unsigned int idLeader = 0;

    if ( this->leader )
    {
        idLeader = this->leader->GetId();
    }

    return idLeader;
}