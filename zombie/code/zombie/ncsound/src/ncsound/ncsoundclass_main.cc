#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  ncsoundclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncsound/ncsoundclass.h"

//NSIGNAL_DEFINE(ncTransformClass, ...);

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSoundClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSoundClass)
    NSCRIPT_ADDCMD_COMPCLASS('EGST', nSoundEventTable*, GetSoundTable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESGT', nSoundEventTable*, SafeGetSoundTable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JBST', nSoundEventTable *, BeginSoundTable, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSoundClass::ncSoundClass():
    soundTable(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSoundClass::~ncSoundClass()
{
    if ( this->soundTable )
    {
        this->soundTable->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSoundClass::SaveCmds(nPersistServer * ps)
{
    if ( this->soundTable )
    {
        // get constructor cmd (BeginSoundTable)
        nCmd * cmd = ps->GetCmd(this->GetEntityClass(), 'JBST');
        cmd->Rewind();
        if (ps->BeginObjectWithCmd(this->soundTable, cmd))
        {
            if (!this->soundTable->SaveCmds(ps))
            {
                return false;
            }
            ps->EndObject(false);
        } 
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Get sound events table
*/
nSoundEventTable*
ncSoundClass::GetSoundTable() const
{
    return this->soundTable;
}

//------------------------------------------------------------------------------
/**
    @brief Get sound events table
*/
nSoundEventTable*
ncSoundClass::SafeGetSoundTable()
{
    if ( ! this->soundTable )
    {
        // create sound table if not exists
        this->soundTable = static_cast<nSoundEventTable*>( nKernelServer::Instance()->New("nsoundeventtable") );
        n_assert( this->soundTable );
    }
    return this->soundTable;
}

//------------------------------------------------------------------------------
nSoundEventTable*
ncSoundClass::BeginSoundTable()
{
    if ( this->soundTable )
    {
        this->soundTable->Release();
        this->soundTable = 0;
    }

    // create sound table if not exists
    this->soundTable = static_cast<nSoundEventTable*>( nKernelServer::Instance()->New("nsoundeventtable") );
    n_assert( this->soundTable );

    // report this to persist server
    if (this->soundTable)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(this->soundTable, nObject::LoadedInstance);
    }
    else
    {
        n_error("nSoundEventTable::BeginSoundTable failed failed!\n");
    }

    return this->soundTable;
}

//------------------------------------------------------------------------------
/**
    @returns true if can load the resources
*/
bool
ncSoundClass::LoadResources()
{
    if( this->soundTable )
    {
        return this->soundTable->LoadSounds();
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    returns true if can unload the resources
*/
bool
ncSoundClass::UnloadResources()
{
    if( this->soundTable )
    {
        return this->soundTable->UnloadSounds();
    }

    return false;
}

//------------------------------------------------------------------------------