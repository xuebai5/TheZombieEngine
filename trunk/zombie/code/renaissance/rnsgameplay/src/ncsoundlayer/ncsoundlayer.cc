#include "precompiled/pchrnsgameplay.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "kernel/nlogclass.h"
#include "zombieentity/nctransform.h"
#include "nphysics/nphysicsserver.h"
#include "gameplay/ngamematerial.h"
#include "nphysics/nphygeomtrans.h"
#include "nphysics/ncphysicsobj.h"
#include "nnavmesh/nnavutils.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSoundLayer,ncSound);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSoundLayer)
	NSCRIPT_ADDCMD_COMPOBJECT('IPLS', void, PlaySound, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JPSM', void, PlaySoundMaterial, 3, (int, const char *, bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IPST', void, PlaySoundTime, 4, (int, int, float, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JPTM', void, PlaySoundTimeMaterial, 5, (int, int, const char *, float, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSM', void, SetSoundMaterial, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGRN', nString, GetRealEventName, 2, (const char *, int), 0, ());
    /*NSCRIPT_ADDCMD_COMPOBJECT('JSTO', void, StopSound, 0, (), 0, ());*/
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Constructor
*/
ncSoundLayer::ncSoundLayer() :
    currentSound(GP_ACTION_NONE),
    currentMaterial(""),
    looped(false),
    calcVar(false),
    duration(0.0f),
    ncSound()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncSoundLayer::~ncSoundLayer()
{
    this->currentSound = GP_ACTION_NONE;
    this->currentMaterial = 0;
}

//------------------------------------------------------------------------------
/**
    GetRealEventName

    @param event - the user event name
    @param descriptor - the description of the event

    @return the real name (in disc) of an event. This is for events that have
    more than one animation/sound. The sound launched depends on animation.
*/
nString
ncSoundLayer::GetRealEventName( const char* event, int descriptor )
{
    nString realName(event);
    if ( descriptor > -1 )
    {
        // put the zero to left if needed
        if ( descriptor < 10 )
        {
            realName.Append("0");
        }
        realName.AppendInt(descriptor);
    }    
    return realName;
}
//------------------------------------------------------------------------------
/**
    GetStringAction

    @param action - the action given by the enum of actions

    @return the corresponding string of the given action
*/
const char*
ncSoundLayer::GetStringAction (ngpAction action) const
{
    const char* string = 0;

    switch (action)
    {
    case GP_ACTION_IDLE:
        string = "idle";
        break;

    case GP_ACTION_WALK:
        string = "walk";
        break;

    case GP_ACTION_WALKPRONE:
        string = "walkprone";
        break;
    case GP_ACTION_RUN:
        string = "run";
        break;

    case GP_ACTION_SPRINT:
        string = "sprint";
        break;

    case GP_ACTION_STRAFELEFT:        
    case GP_ACTION_STRAFERIGHT:
        string = "strafe";
        break;

    case GP_ACTION_JUMPSTART:
        string = "jumpstart";
        break;

    case GP_ACTION_JUMPEND:
        string = "jumpend";
        break;

    case GP_ACTION_JUMPENDSUC:
        string = "jumpendsuccess";
        break;

    case GP_ACTION_JUMPENDFAIL:
        string = "jumpendfail";
        break;

    case GP_ACTION_JUMPATTACK:
        string = "jumpattack";
        break;

    case GP_ACTION_JUMPOBSTACLE:
        string = "jumpobstacle";
        break;

    case GP_ACTION_DIE:
        string = "die";
        break;

    case GP_ACTION_DRIVE:
        string = "drive";
        break;

    case GP_ACTION_SWIM:
        string = "swim";
        break;

    case GP_ACTION_DIVE:
        string = "dive";
        break;

    case GP_ACTION_SHOOT:
        string = "shoot";
        break;

    case GP_ACTION_SHOOTSILENCER:
        string = "shootsilencer";
        break;

    case GP_ACTION_SHOOTBURST:
        string = "shootburst";
        break;

    case GP_ACTION_SHOOTBURSTSILENCER:
        string = "shootburstsilencer";
        break;

    case GP_ACTION_MELEEATTACK:
        string = "hit";
        break;
    case GP_ACTION_MELEEBLOCK:
        string = "hitdefense";
        break;

    case GP_ACTION_LEFTCLAW:
    case GP_ACTION_RIGHTCLAW:
    case GP_ACTION_BOTHCLAWS:
        string = "claw";
        break;

    case GP_ACTION_FLY:
        string = "fly";
        break;

    case GP_ACTION_MOP:
        string = "mop";
        break;

    case GP_ACTION_LEAP:
        string = "leap";
        break;

    case GP_ACTION_TAUNT:
        string = "taunt";
        break;

    case GP_ACTION_BURROW:
        string = "burrow";
        break;

    case GP_ACTION_ROLL:
        string = "roll";
        break;

    case GP_ACTION_CLIMB:
        string = "climb";
        break;

    case GP_ACTION_FASTFIRSTRELOAD:
        string = "fastreload";
        break;

    case GP_ACTION_FULLRELOAD:
        string = "fullreload";
        break;

    case GP_ACTION_ENDRELOAD:
        string = "endreload";
        break;
    case GP_ACTION_ENDRELOADCROUCH:
        string = "endreloadcrouch";
        break;
    case GP_ACTION_ENDRELOADPRONE:
        string = "endreloadprone";
        break;

    case GP_ACTION_COCKRELOAD:
        string = "cockreload";
        break;
    case GP_ACTION_COCKRELOADCROUCH:
        string = "cockreloadcrouch";
        break;
    case GP_ACTION_COCKRELOADPRONE:
        string = "cockreloadprone";
        break;

    case GP_ACTION_HIDEWEAPON:
        string = "hideweapon";
        break;

    case GP_ACTION_SHOWWEAPON:
        string = "showweapon";
        break;

    case GP_ACTION_SHOWWEAPONCROUCH:
        string = "showweaponcrouch";
        break;

    case GP_ACTION_SHOWWEAPONPRONE:
        string = "showweaponprone";
        break;

    case GP_ACTION_WEAPONACC:
        string = "weaponacc";
        break;

    case GP_ACTION_WEAPONDISMOUNTACC:
        string = "weapondismountacc";
        break;

    case GP_ACTION_SILENCERON:
        string = "silenceron";
        break;
    case GP_ACTION_SILENCERONCROUCH:
        string = "silenceroncrouch";
        break;
    case GP_ACTION_SILENCERONPRONE:
        string = "silenceronprone";
        break;

    case GP_ACTION_SILENCEROFF:
        string = "silenceroff";
        break;

    case GP_ACTION_STAND:
        string = "stand";
        break;

    case GP_ACTION_IRONSIGHT:
        string = "ironsight";
        break;

    case GP_ACTION_IRONSIGHTOFF:
        string = "ironsight";
        break;

    case GP_ACTION_CROUCH:
        string = "crouch";
        break;
    
    case GP_ACTION_CROUCHSTANDUP:
        string = "crouchstandup";
        break;

    case GP_ACTION_CROUCHPRONE:
        string = "crouchprone";
        break;

    case GP_ACTION_PRONE:
        string = "prone";
        break;
    
    case GP_ACTION_PRONESTANDUP:
        string = "pronestandup";
        break;

    case GP_ACTION_PRONECROUCH:
        string = "pronecrouch";
        break;

    case GP_ACTION_SEAT:
        string = "seat";
        break;

    case GP_ACTION_SWITCH:
        string = "switch";
        break;

    default:
        NLOG( soundLog, (0, "ncSoundLayer:GetStringAction(): There is no sound event for basic action '%d'", action ) );
    }

    return string;
}

//------------------------------------------------------------------------------
/**
    PlaySound
*/
void
ncSoundLayer::PlaySound (ngpAction action)
{
    this->PlaySound (int(action), false);
}

//------------------------------------------------------------------------------
/**
    PlaySound
*/
void
ncSoundLayer::PlaySound (int action, bool looped)
{
    const char* actionName = this->GetStringAction (ngpAction(action));

    // Remove this when it has sense to exist this part of code
    if ( actionName == 0 )
        return;

    n_assert(actionName);
    int times = looped ? 0 : 1;

    if ( !looped || this->currentSound != (ngpAction)action )
    {
        // The first thing we must do is stop the prev sound
        this->StopSound();

        this->Play (actionName, times);

        this->currentSound = (ngpAction)action;
        this->looped = looped;
    }
}

//------------------------------------------------------------------------------
/**
    PlaySoundMaterial
*/
void
ncSoundLayer::PlaySoundMaterial (int action, const char * material, bool looped)
{
    const char* actionName = this->GetStringAction (ngpAction(action));

    // Remove this when it has sense to exist this part of code
    if ( actionName == 0 )
        return;

    n_assert(actionName);
    int times = looped ? 0 : 1;

    if ( !looped || this->currentSound != (ngpAction)action )
    {
        // The first thing we must do is stop the prev sound
        this->StopSound();

        if ( material )
        {
            this->currentMaterial = material;
        }
        else
        {
            this->currentMaterial = "";
        }

        this->PlayMaterial (actionName, this->currentMaterial, times);

        this->currentSound = (ngpAction)action;
        this->looped = looped;
    }
}

//------------------------------------------------------------------------------
/**
    PlaySoundTime
*/
void
ncSoundLayer::PlaySoundTime (int action, int repeat, float duration, bool calcVar)
{
    const char* actionName = this->GetStringAction (ngpAction(action));

    // Remove this when it has sense to exist this piece of code
    if ( actionName == 0 )
        return;

    n_assert(actionName);

    if ( repeat != 0 || this->currentSound != (ngpAction)action || calcVar != this->calcVar || duration != this->duration )
    {
        this->StopSound();
        this->PlayExtended (actionName, this->currentMaterial, repeat, duration, calcVar);

        this->currentSound = (ngpAction)action;
        this->currentMaterial = 0;
        this->looped = repeat == 0;
        this->calcVar = calcVar;
        this->duration = duration;
    }
}

//------------------------------------------------------------------------------
/**
    PlaySoundTime
*/
void
ncSoundLayer::PlaySoundTimeMaterial (int action, int repeat, const char * material, float duration, bool calcVar)
{
    const char* actionName = this->GetStringAction (ngpAction(action));

    // Remove this when it has sense to exist this piece of code
    if ( actionName == 0 )
        return;

    n_assert(actionName);

    if ( repeat != 0 || this->currentSound != (ngpAction)action || calcVar != this->calcVar || duration != this->duration )
    {
        this->StopSound();
        this->PlayExtended (actionName, material, repeat, duration, calcVar);

        this->currentSound = (ngpAction)action;
        this->currentMaterial = material;
        this->looped = repeat == 0;
        this->calcVar = calcVar;
        this->duration = duration;
    }
}

//------------------------------------------------------------------------------
/**
    SetSoundMaterial
*/
void
ncSoundLayer::SetSoundMaterial (const char * material, float duration)
{
    int times = this->looped ? 0 : 1;

    this->currentMaterial = material;

    if ( this->currentSound != GP_ACTION_NONE )
    {
        const char* actionName = this->GetCurrentAction();
        this->PlayExtended( actionName, material, times, duration, true );
    }
}

//------------------------------------------------------------------------------
/**
    GetCurrentAction
*/
const char*
ncSoundLayer::GetCurrentAction() const
{
    return this->GetStringAction (this->currentSound);
}

//-----------------------------------------------------------------------------
/**
    @brief Get nGameMaterial name under entity
    @return Name of material under living entity or 0
*/
const char * 
ncSoundLayer::GetMaterialNameUnder()
{
    // Disable the object collision, cast a ray downwards to get material and enable collision again
    bool enable(false);
    if( this->GetComponentSafe< ncPhysicsObj >()->IsEnabled() )
    {
        this->GetComponentSafe< ncPhysicsObj >()->Disable();
        enable = true;
    }

    const int maxContacts = 10;
    nPhyCollide::nContact contacts[maxContacts];
    vector3 rayStart( this->GetComponentSafe<ncTransform>()->GetPosition() );
    float groundHeight = 0.0f;
    nNavUtils::GetGroundHeight( rayStart, groundHeight);
    rayStart.y = groundHeight;
    vector3 rayDirection(0.0f, -1.0f, 0.0f);
    const phyreal rayLength = 0.05f;
    int numContacts = nPhysicsServer::Instance()->Collide( rayStart, rayDirection, rayLength, maxContacts, contacts, nPhysicsGeom::All );

    if( enable )
    {
        this->GetComponentSafe< ncPhysicsObj >()->Enable();
    }

    // Find lowest collision and get its material
    const char * materialName = 0;
    if ( numContacts )
    {
        int lowestContact = 0;
        vector3 pos;
        contacts[0].GetContactPosition( pos );
        float lowestDist = pos.y;
        for ( int i = 1; i < numContacts; i++ )
        {
            contacts[i].GetContactPosition( pos );
            if ( pos.y < lowestDist )
            {
                lowestContact = i;
                lowestDist = pos.y;
            }
        }

        nGameMaterial* gmaterial(0);
        if ( contacts[lowestContact].GetGeometryA()->Type() == nPhysicsGeom::Ray )
        {
            gmaterial = contacts[lowestContact].GetGameMaterialB();
        }
        else
        {
            gmaterial = contacts[lowestContact].GetGameMaterialA();
        }
        if ( gmaterial )
        {
            materialName = gmaterial->GetGameMaterialName();
        }
    }
    return materialName;
}

//------------------------------------------------------------------------------
/**
    Update sound material if playing and the material changes
*/
void
ncSoundLayer::UpdateSoundMaterial(float duration)
{
    if ( this->IsPlaying() )
    {
        const char * material = this->GetMaterialNameUnder();
        if ( material && strcmp(this->currentMaterial.Get(), material) != 0 )
        {
               this->SetSoundMaterial( material, duration );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Stop sound
*/
void
ncSoundLayer::StopSound()
{
    this->currentSound = GP_ACTION_NONE;
    this->currentMaterial = 0;
    this->looped = false;
    this->calcVar = false;
    this->duration = 0.0f;

    ncSound::StopSound();
}

