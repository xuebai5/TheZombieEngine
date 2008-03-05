//------------------------------------------------------------------------------
//  ngpplayeradjust.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpplayermovement/ngpplayeradjust.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPPlayerAdjust, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPPlayerAdjust)
    NSCRIPT_ADDCMD('INIT', bool, Init, 7, 
        (nEntityObject*, const float, const int, const vector3 &, const vector3 &, 
        const vector2 &, const vector2 & ), 0, ());

    NNETWORKTYPE_INCLIENTS();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPPlayerAdjust::nGPPlayerAdjust()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPPlayerAdjust::~nGPPlayerAdjust()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity where apply the movement
    @param timestamp time of the movement
*/
bool
nGPPlayerAdjust::Init( nEntityObject * entity, const float timestamp, const int flags, 
                      const vector3 & position, const vector3 & velocity, 
                      const vector2 & angle, const vector2 & angleIncrement )
{
    bool valid = entity != 0;

    if( valid )
    {
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if( player )
        {
            player->AdjustPosition( timestamp, flags, position, velocity, angle, angleIncrement );
        }

        valid = player != 0;
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
