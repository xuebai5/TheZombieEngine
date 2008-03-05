//------------------------------------------------------------------------------
//  ngameplayenums.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ngameplayenums.h"
#include "kernel/ntypes.h"

#include <cstring>

//------------------------------------------------------------------------------
namespace nGameplayEnums {

struct ngpActionTable {
    ngpAction action;
    const char * name;
} action_names [] = {
    { GP_ACTION_NONE ,          "None" },
	{ GP_ACTION_IDLE,           "idle" },
    { GP_ACTION_COMBATIDLE,     "cidle" },
    { GP_ACTION_DRIVINGIDLE,    "drivingidle" },
	{ GP_ACTION_WALK,           "walk" },
    { GP_ACTION_WALKPRONE,      "walkprone" },
	{ GP_ACTION_RUN,            "run" },
	{ GP_ACTION_SPRINT,         "sprint" },
	{ GP_ACTION_STRAFELEFT,     "strafeleft" },
	{ GP_ACTION_STRAFERIGHT,    "straferight" },
	{ GP_ACTION_JUMPSTART,      "jumpstart" },
    { GP_ACTION_JUMPENDSUC,     "jumpendsuccess" },
    { GP_ACTION_JUMPENDFAIL,    "jumpendfail" },
    { GP_ACTION_JUMPATTACK,     "jumpattack" },
    { GP_ACTION_BATTLECRY,      "battlecry" },
    { GP_ACTION_JUMPOBSTACLE,   "jumpobstacle" },
	{ GP_ACTION_JUMPMID,        "jumpmid" },
	{ GP_ACTION_JUMPEND,        "jumpend" },
	{ GP_ACTION_DIE,            "die" },
	{ GP_ACTION_DRIVE,          "drive" },
	{ GP_ACTION_SWIM,           "swim" },
	{ GP_ACTION_DIVE,           "dive" },
	{ GP_ACTION_SHOOT,          "shootsemi" },
    { GP_ACTION_SHOOTSILENCER,  "shootsilencer" },
	{ GP_ACTION_SHOOTAUTO,      "shootauto" },
    { GP_ACTION_MELEEATTACK,    "hit" },
    { GP_ACTION_MELEEBLOCK,     "hitdefense" },
    { GP_ACTION_IMPACT,         "impact" },
    { GP_ACTION_MINIIMPACT,     "minimpact" },
	{ GP_ACTION_LEFTCLAW,       "clawleft" },
	{ GP_ACTION_RIGHTCLAW,      "clawright" },
	{ GP_ACTION_BOTHCLAWS,      "clawboth" },
	{ GP_ACTION_FLY,            "fly" },
	{ GP_ACTION_MOP,            "mop" },
	{ GP_ACTION_LEAP,           "leap" },
	{ GP_ACTION_TAUNT,          "taunt" },
	{ GP_ACTION_BURROW,         "burrow" },
	{ GP_ACTION_ROLL,           "rota" },
	{ GP_ACTION_CLIMB,          "climb" },
	{ GP_ACTION_FASTFIRSTRELOAD, "reloadquickfirst" },
	{ GP_ACTION_FASTSECONDRELOAD, "reloadquicksecond" },
	{ GP_ACTION_FULLRELOAD,     "reloadfull" },
	{ GP_ACTION_FULLFIRSTRELOAD, "reloadfullfirst" },
	{ GP_ACTION_FULLSECONDRELOAD, "reloadfullsecond" },
	{ GP_ACTION_ENDRELOAD,      "reloadend" },
    { GP_ACTION_ENDRELOADCROUCH, "endreloadcrouch" },
    { GP_ACTION_ENDRELOADPRONE, "endreloadprone" },
    { GP_ACTION_COCKRELOAD,     "reloadcock" },
    { GP_ACTION_COCKRELOADCROUCH, "cockreloadcrouch" },
	{ GP_ACTION_COCKRELOADPRONE, "cockreloadprone" },
	{ GP_ACTION_HIDEWEAPON,     "weapondown" },
	{ GP_ACTION_SHOWWEAPON,     "weaponup" },
    { GP_ACTION_WEAPONACC,      "weaponacc" },
	{ GP_ACTION_WEAPONDISMOUNTACC, "weapondismountacc" },
    { GP_ACTION_INJECTION,      "injection" },
    { GP_ACTION_LOOKAROUND,     "lookaround" },
    { GP_ACTION_SILENCERON,     "silenceron" },
    { GP_ACTION_SILENCERONCROUCH, "silenceroncrouch" },
    { GP_ACTION_SILENCERONPRONE, "silenceronprone" },
    { GP_ACTION_SILENCEROFF,    "silenceroff" },
    { GP_ACTION_SWITCH,         "switch" },
    { GP_ACTION_FLASHLIGHT,     "flashlight" },
	{ GP_ACTION_STAND,          "stand" },
	{ GP_ACTION_IRONSIGHT,      "ironsight" },
    { GP_ACTION_IRONSIGHTOFF,   "ironsightoff" },
    { GP_ACTION_LEANIRONSIGHT,  "leanironsight" },
	{ GP_ACTION_CROUCH,         "crouch" },
    { GP_ACTION_CROUCHSTANDUP,  "crouchstandup" },
    { GP_ACTION_CROUCHPRONE,    "crouchprone" },
	{ GP_ACTION_PRONE,          "prone" },
    { GP_ACTION_PRONESTANDUP,   "pronestandup" },
    { GP_ACTION_PRONECROUCH,    "pronecrouch" },
	{ GP_ACTION_SEAT,           "seat" },
	{ GP_ACTION_LEAN,           "lean" },
    { GP_ACTION_NONE, 0 }
};

//------------------------------------------------------------------------------
/**
    @param action action to get the name
    @returns name of action
*/
const char *
GetActionString( const ngpAction action )
{
    int i = 0;

    while( action_names[ i ].name && action_names[ i ].action != action )
    {
        ++i;
    }

    return action_names[ i ].name;
}

//------------------------------------------------------------------------------
/**
    @param name name of the action
    @returns action
*/
ngpAction
GetActionEnum( const char * name )
{
    int i = 0;

    while( action_names[ i ].name && ( n_stricmp(  action_names[ i ].name, name ) != 0 ) )
    {
        ++i;
    }

    return action_names[ i ].action;
}

//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
