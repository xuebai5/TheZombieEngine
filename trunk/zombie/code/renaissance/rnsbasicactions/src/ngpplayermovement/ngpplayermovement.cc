//------------------------------------------------------------------------------
//  ngpplayermovement.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpplayermovement/ngpplayermovement.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPPlayerMovement, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGPPlayerMovement )
    NSCRIPT_ADDCMD('INIT', bool, Init, 9, 
        (nEntityObject*, const float, const vector3&, const int, const int, 
        const vector2&,const float, const float, const int), 
        0, ());

    NNETWORKTYPE_ONLYSERVER();
    NNETWORKTYPE_AHEAD();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPPlayerMovement::nGPPlayerMovement()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPPlayerMovement::~nGPPlayerMovement()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity where apply the movement
    @param timestamp time of the movement
    @param forward movement in the forward direction
    @param step movement in the lateral direction
    @param yaw lookup turn in vertical axis
    @param pitch lookup turn in horizontal axis
    @param flags aditional movement flags
    @returns true if basic action can be init
*/
bool
nGPPlayerMovement::Init( nEntityObject* entity, const float timestamp, 
                        const vector3 & position, const int forward, const int step, 
                        const vector2 & angle, const float yaw, const float pitch, const int flags )
{
    bool valid = entity != 0;

    if ( valid )
    {

        nNetworkManager * network = nNetworkManager::Instance();
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if ( network && player )
        {
            if( network->IsServer() )
            {
                player->ServerMove( timestamp, position, forward, step, angle, yaw, pitch, flags );
            }
            else
            {
                player->MoveAutonomous( timestamp, forward, step, yaw, pitch, flags );
            }
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
