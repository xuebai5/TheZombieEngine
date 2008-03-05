//-----------------------------------------------------------------------------
//  ngamematerial_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchgameplay.h"
#include "gameplay/ngamematerial.h"
#include "kernel/npersistserver.h"
#include "nphysics/nphymaterial.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    ngamematerial

    @cppclass
    nGameMaterial

    @superclass
    nRoot

    @classinfo
    Represents a game material.
*/
NSCRIPT_INITCMDS_BEGIN(nGameMaterial)
    NSCRIPT_ADDCMD('DGMN', const char*, GetGameMaterialName, 0, (), 0, ());
    NSCRIPT_ADDCMD('DGMU', void, Update, 0, (), 0, ());
    NSCRIPT_ADDCMD('DGMI', const unsigned int, GetGameMaterialId, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSPM', void, SetPhysicsMaterial, 1, (nPhyMaterial*), 0, ());
    NSCRIPT_ADDCMD('DGPM', nPhyMaterial*, GetPhysicsMaterial, 0, (), 0, ());
    NSCRIPT_ADDCMD('AAEN', void, SetFxMaterial, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('AAEP', const char*, GetFxMaterial, 0, (), 0, ());
    NSCRIPT_ADDCMD('JASE', void , AddSoundEvent, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('JGSE', const char* , GetSoundEvent, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JSHE', void , SetHitSoundEvent, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JSSE', void , SetSlideSoundEvent, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JGHE', const char* , GetHitSoundEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD('JGLE', const char* , GetSlideSoundEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD('DGMA', void, Add, 0, (), 0, ());
    NSCRIPT_ADDCMD('LSHF', void, SetHard, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('LGHF', bool, GetHard , 0, (), 0, ());

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nGameMaterial::SaveCmds(nPersistServer* ps)
{
    if( !nObject::SaveCmds(ps) )
    {
        return false;
    }

    nCmd* cmd(0);

    ps->Put( this, 'LSHF', this->GetHard() );

    // persisting information related to the physics material (if any)
    if( this->physicsMaterial )
    {
        // there's one persisting
        cmd = ps->GetCmd( this, 'DSPM');

        n_assert2( cmd, "Missing command." );

        cmd->In()->SetO( this->physicsMaterial );

        ps->PutCmd(cmd);
    }

    // persisting information related to the fx material (if any)
    if( this->fxMaterial.isvalid() )
    {
        // there's one persisting
        cmd = ps->GetCmd( this, 'AAEN');

        n_assert2( cmd, "Missing command." );

        cmd->In()->SetS( this->fxMaterial.getname() );

        ps->PutCmd(cmd);
    }

    // // persisting information related to sound events mapping

    this->soundEventsMap.Begin();
    nString key;
    nString * info;
    this->soundEventsMap.Next(key, info);

    while ( info )
    {
        // Save addsound command (event, sound id)
        ps->Put( this, 'JASE', key.Get(), info->Get() );

        this->soundEventsMap.Next(key, info); 
    }

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
