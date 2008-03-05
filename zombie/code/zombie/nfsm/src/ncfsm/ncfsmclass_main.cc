//-----------------------------------------------------------------------------
//  ncfsmclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "ncfsm/ncfsmclass.h"
#include "nfsmserver/nfsmserver.h"

nNebulaComponentClass(ncFSMClass,nComponentClass);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncFSMClass)

    NSCRIPT_ADDCMD_COMPCLASS('ESPF', void, SetParentFSM, 1, (nFSM*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGPF', nFSM*, GetParentFSM, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESTI', void, SetConditionPoolingFrequency, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGTI', int, GetConditionPoolingFrequency, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESFN', void, SetParentFSMByName, 1, (const char*), 0, ());

NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncFSMClass::ncFSMClass()
    : ticksInterval(1)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Set the parent FSM
*/
void
ncFSMClass::SetParentFSM( nFSM* parentFSM )
{
    if ( parentFSM )
    {
        this->parentFSMName = parentFSM->GetName();
    }
    else
    {
        this->parentFSMName = "";
    }
}

//-----------------------------------------------------------------------------
/**
    Get the parent FSM
*/
nFSM*
ncFSMClass::GetParentFSM() const
{
    return nFSMServer::Instance()->GetFSM( this->parentFSMName.Get() );
}

//-----------------------------------------------------------------------------
/**
    Set the frequency at which the script condition transitions are checked
*/
void
ncFSMClass::SetConditionPoolingFrequency( int ticksInterval )
{
    this->ticksInterval = ticksInterval;
}

//-----------------------------------------------------------------------------
/**
    Set the parent FSM by name (used for persistence)
*/
void
ncFSMClass::SetParentFSMByName( const char* fsmName )
{
    this->parentFSMName = fsmName;
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncFSMClass::SaveCmds( nPersistServer* ps )
{
    ps->Put( this->GetEntityClass(), 'ESFN', this->parentFSMName.Get() );
    ps->Put( this->GetEntityClass(), 'ESTI', this->ticksInterval );
    return true;
}

//-----------------------------------------------------------------------------
