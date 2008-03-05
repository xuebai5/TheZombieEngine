//-----------------------------------------------------------------------------
//  nmissionhandler_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/nmissionhandler.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nMissionHandler )
    NSCRIPT_ADDCMD('LCLR', void, Clear, 0, (), 0, () );
    NSCRIPT_ADDCMD('EAOB', void, AppendObjective, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('EGON', int, GetObjectivesNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGNI', const char*, GetObjectiveNameByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGDI', const char*, GetObjectiveDescriptionByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESNI', void, SetObjectiveNameByIndex, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('ESDI', void, SetObjectiveDescriptionByIndex, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('EDOI', void, DeleteObjectiveByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESSN', void, SetObjectiveState, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('JGSS', const char * , GetObjectiveStateStringByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESSS', void, SetObjectiveStateStringByIndex, 2, (int, const char*), 0, ());

    cl->BeginSignals(1);
    N_INITCMDS_ADDSIGNAL( ObjectiveStateChanged )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nMissionHandler::SaveCmds( nPersistServer* ps )
{
    ps->Put( this, 'LCLR' );

    // Objectives list
    for ( int i(0); i < this->objectives.Size(); ++i )
    {
        Objective& objective( this->objectives[i] );
        ps->Put( this, 'EAOB', objective.name.Get(), objective.description.Get() );
        ps->Put( this, 'ESSN', objective.name.Get(), this->StateIdToLabel( objective.state ) );
    }

    return true;
}
