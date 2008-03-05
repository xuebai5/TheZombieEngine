#ifndef GAMEPLAYENUMS_H
#define GAMEPLAYENUMS_H

enum ngpAction
{
	GP_ACTION_NONE = -1,
	GP_ACTION_IDLE,
    GP_ACTION_COMBATIDLE,
    GP_ACTION_DRIVINGIDLE,
	GP_ACTION_WALK,
    // added for sound event
    GP_ACTION_WALKPRONE,
	GP_ACTION_RUN,
	GP_ACTION_SPRINT,
	GP_ACTION_STRAFELEFT,
	GP_ACTION_STRAFERIGHT,
	GP_ACTION_JUMPSTART,
    GP_ACTION_JUMPENDSUC,
    GP_ACTION_JUMPENDFAIL,
	GP_ACTION_JUMPMID,
	GP_ACTION_JUMPEND,
    GP_ACTION_JUMPATTACK,
    GP_ACTION_BATTLECRY,
    GP_ACTION_JUMPOBSTACLE,
	GP_ACTION_DIE,
	GP_ACTION_DRIVE,
	GP_ACTION_SWIM,
	GP_ACTION_DIVE,
	GP_ACTION_SHOOT,
    // added for sound event
    GP_ACTION_SHOOTSILENCER,
	GP_ACTION_SHOOTBURST,
	// added for sound event
	GP_ACTION_SHOOTBURSTSILENCER,
	GP_ACTION_SHOOTAUTO,
    GP_ACTION_MELEEATTACK,
    GP_ACTION_MELEEBLOCK,
    GP_ACTION_IMPACT,
    GP_ACTION_MINIIMPACT,
	GP_ACTION_LEFTCLAW,
	GP_ACTION_RIGHTCLAW,
	GP_ACTION_BOTHCLAWS,
	GP_ACTION_FLY,
	GP_ACTION_MOP,
	GP_ACTION_LEAP,
	GP_ACTION_TAUNT,
	GP_ACTION_BURROW,
	GP_ACTION_ROLL,
	GP_ACTION_CLIMB,
	GP_ACTION_FASTFIRSTRELOAD,
	GP_ACTION_FASTSECONDRELOAD,
	GP_ACTION_FULLRELOAD,
	GP_ACTION_FULLFIRSTRELOAD,
	GP_ACTION_FULLSECONDRELOAD,
	GP_ACTION_ENDRELOAD,
    // added for sound event
    GP_ACTION_ENDRELOADCROUCH,
    // added for sound event
    GP_ACTION_ENDRELOADPRONE,
    GP_ACTION_COCKRELOAD,
    // added for sound event
    GP_ACTION_COCKRELOADCROUCH,
    // added for sound event
	GP_ACTION_COCKRELOADPRONE,
	GP_ACTION_HIDEWEAPON,
	GP_ACTION_SHOWWEAPON,
    // added for sound event
    GP_ACTION_SHOWWEAPONCROUCH,
    // added for sound event
    GP_ACTION_SHOWWEAPONPRONE,
    GP_ACTION_WEAPONACC,
	GP_ACTION_WEAPONDISMOUNTACC,
    GP_ACTION_INJECTION,
    GP_ACTION_LOOKAROUND,
    GP_ACTION_SILENCERON,
    // added for sound event
    GP_ACTION_SILENCERONCROUCH,
    // added for sound event
    GP_ACTION_SILENCERONPRONE,
    GP_ACTION_SILENCEROFF,
    GP_ACTION_SWITCH,
    GP_ACTION_FLASHLIGHT,

	GP_ACTION_STAND,        // Transitions between states
	GP_ACTION_IRONSIGHT,
    // added for sound event
    GP_ACTION_IRONSIGHTOFF,
    GP_ACTION_LEANIRONSIGHT,
	GP_ACTION_CROUCH,
    // added for sound event
    GP_ACTION_CROUCHSTANDUP,
    // added for sound event
    GP_ACTION_CROUCHPRONE,
	GP_ACTION_PRONE,
    // added for sound event
    GP_ACTION_PRONESTANDUP,
    // added for sound event
    GP_ACTION_PRONECROUCH,
	GP_ACTION_SEAT,
	GP_ACTION_LEAN,

	GP_MAX_ACTIONS          // Number of actions
};

//------------------------------------------------------------------------------
namespace nGameplayEnums {
    /// get the string name of a action
    const char * GetActionString( const ngpAction action );
    /// get the action enum of a name
    ngpAction GetActionEnum( const char * name );
}

#endif