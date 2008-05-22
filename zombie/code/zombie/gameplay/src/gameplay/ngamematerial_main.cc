//-----------------------------------------------------------------------------
//  ngamematerial_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"

#include "nspecialfx/nfxmaterial.h"
#include "gameplay/ngamematerial.h"
#include "nphysics/nphymaterial.h"
#include "gameplay/ngamematerialserver.h"
#ifndef __ZOMBIE_EXPORTER__
#include "nsoundscheduler/nsoundscheduler.h"
#include "nsoundlibrary/nsoundlibrary.h"
#endif
#include "kernel/nlogclass.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nGameMaterial, "nroot");


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 01-Aug-2004   Zombie        created
*/
nGameMaterial::nGameMaterial() : 
    id( "" ),
    physicsMaterial(0),
    soundEventsMap(1),
    isHard(false)
#ifndef NGAME
    ,hasToBeRemoved(false)
#endif
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 01-Aug-2004   Zombie        created
*/
nGameMaterial::~nGameMaterial()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Returns the material id.

    @return material id

    history:
        - 01-Aug-2004   Zombie        created
*/
int nGameMaterial::GetMaterialId() const
{
    n_assert2( this->id != nTag(""), "Material not created." );

    return this->id.KeyMap();
}

//-----------------------------------------------------------------------------
/**
    Creates the material.

    history:
        - 01-Aug-2004   Zombie        created
*/
void nGameMaterial::Create()
{
}

//-----------------------------------------------------------------------------
/**
    Returns the game material name.

    @return game material name

    history:
        - 01-Aug-2004   Zombie        created
*/
const char* nGameMaterial::GetGameMaterialName() const
{
    return this->GetName();
}

//-----------------------------------------------------------------------------
/**
    Returns the game material id.

    @return game material id

    history:
        - 01-Aug-2004   Zombie        created
*/
const uint nGameMaterial::GetGameMaterialId() const
{
    return this->id.KeyMap();
}

//-----------------------------------------------------------------------------
/**
    Sets physics material.

    @param material a physics material

    history:
        - 01-Aug-2004   Zombie        created
*/
void nGameMaterial::SetPhysicsMaterial( nPhyMaterial* material )
{
    n_assert2( material, "Error: No valid material." );

#ifndef NGAME
    if( !material )
        return;
#endif

    this->physicsMaterial = material;
}

//-----------------------------------------------------------------------------
/**
    Gets the physics material.

    @return a physics material

    history:
        - 01-Aug-2004   Zombie        created
*/
nPhyMaterial* nGameMaterial::GetPhysicsMaterial() const
{
    return this->physicsMaterial;
}

//-----------------------------------------------------------------------------
/**
    Updates the information (should be called after the material properties had been changed).

    history:
        - 28-Oct-2005   Zombie        created
*/
void nGameMaterial::Update()
{
    if( this->physicsMaterial )
    {
        this->physicsMaterial->Update();
    }
}

//-----------------------------------------------------------------------------
/**
    Updates the information (should be called after the material properties had been changed).

    history:
        - 31-Oct-2005   Zombie        created
*/
void nGameMaterial::Add() 
{
    nGameMaterialServer::Instance()->Add( this );   
}

//-----------------------------------------------------------------------------
/**
    Inits object

    history:
        - 31-Oct-2005   Zombie        created
*/
void nGameMaterial::InitInstance(nObject::InitInstanceMsg)
{
    id = nTag( this->GetName() );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Returns if any of the materials components is dirty (changed).

    history:
        - 31-Oct-2005   Zombie        created
*/
const bool nGameMaterial::IsDirty() const
{
    if( this->physicsMaterial )
    {
        if( this->physicsMaterial->IsObjectDirty() )
        {
            return true;
        }
    }

    // @note: here shold be the other components information
    

    if( this->GetToBeRemoved() )
    {
        return true;
    }

    return false; // if no component was dirty
}

//-----------------------------------------------------------------------------
/**
    Return if the object it's meant to be removed.

    @return true/false

    history:
        - 04-Nov-2005   Zombie        created
*/
bool nGameMaterial::GetToBeRemoved() const
{
    return this->hasToBeRemoved;
}

//-----------------------------------------------------------------------------
/**
    Set to be removed.

    @param has true/false

    history:
        - 04-Nov-2005   Zombie        created
*/
void nGameMaterial::SetToBeRemoved( bool has )
{
    if( has )
    {
        this->HideInEditor();
    }
    else
    {
        this->UnHideInEditor();
    }

    this->hasToBeRemoved = has;
}

#endif

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::SetFxMaterialObject(nFxMaterial* material)
{
    this->fxMaterial = material;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::SetFxMaterial(const char * material)
{
    this->fxMaterial = material;
}

//-----------------------------------------------------------------------------
/**
*/
const char *
nGameMaterial::GetFxMaterial() const
{
    return this->fxMaterial.getname();
}

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::AddSoundEvent(const char *event, const char *soundId)
{
    n_assert( event && soundId && *event && *soundId );

#ifndef __ZOMBIE_EXPORTER__
    nSoundLibrary* soundLibrary = nSoundScheduler::Instance()->GetSoundLibrary();
    n_assert( soundLibrary );
    int soundIndex = soundLibrary->GetSoundIndex( soundId );

    if ( soundIndex == -1 )
    {
        NLOG( soundLog, (0, "Error: Tried to add non-existing sound in game material '%s' (event '%s', material '%s')", soundId, event, this->GetName() ) );
        return;
    }
#endif

    if ( ! this->soundEventsMap[event] )
    {
        nString soundIdStr(soundId);
        this->soundEventsMap.Add( nString(event), &soundIdStr );
    }
}

//-----------------------------------------------------------------------------
/**
*/
const char*
nGameMaterial::GetSoundEvent(const char *event)
{
    nString * soundId = this->soundEventsMap[event];
    if ( soundId )
    {
        return soundId->Get();
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::SetHitSoundEvent(const char *soundId)
{
    n_assert( soundId && *soundId );

#ifndef __ZOMBIE_EXPORTER__
    nSoundLibrary* soundLibrary = nSoundScheduler::Instance()->GetSoundLibrary();
    n_assert( soundLibrary );
    int soundIndex = soundLibrary->GetSoundIndex( soundId );

    if ( soundIndex == -1 )
    {
        NLOG( soundLog, (0, "Error: Tried to add non-existing sound in game material '%s' (event '%s', material '%s')", soundId, "HIT", this->GetName() ) );
        return;
    }
#endif

    if ( ! this->soundEventsMap["HIT"] )
    {
        nString soundIdStr(soundId);
        this->soundEventsMap.Add( "HIT", &soundIdStr );
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::SetSlideSoundEvent(const char *soundId)
{
    n_assert( soundId && *soundId );

#ifndef __ZOMBIE_EXPORTER__
    nSoundLibrary* soundLibrary = nSoundScheduler::Instance()->GetSoundLibrary();
    n_assert( soundLibrary );
    int soundIndex = soundLibrary->GetSoundIndex( soundId );

    if ( soundIndex == -1 )
    {
        NLOG( soundLog, (0, "Error: Tried to add non-existing sound in game material '%s' (event '%s', material '%s')", soundId, "SLIDE", this->GetName() ) );
        return;
    }
#endif

    if ( ! this->soundEventsMap["SLIDE"] )
    {
        nString soundIdStr(soundId);
        this->soundEventsMap.Add( "SLIDE", &soundIdStr );
    }
}

//-----------------------------------------------------------------------------
/**
*/
const char*
nGameMaterial::GetHitSoundEvent()
{
    nString * soundId = this->soundEventsMap["HIT"];
    if ( soundId )
    {
        return soundId->Get();
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
const char*
nGameMaterial::GetSlideSoundEvent()
{
    nString * soundId = this->soundEventsMap["SLIDE"];
    if ( soundId )
    {
        return soundId->Get();
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial*
nGameMaterial::GetFxMaterialObject()
{
    if( this->fxMaterial.isvalid())
    {
        return this->fxMaterial.get();
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGameMaterial::SetHard( bool value )
{
    this->isHard = value;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGameMaterial::GetHard() const
{
    return this->isHard;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
