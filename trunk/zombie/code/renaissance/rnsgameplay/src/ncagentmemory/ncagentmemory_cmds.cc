#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncagentmemory_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncagentmemory/ncagentmemory.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAgentMemory)
    NSCRIPT_ADDCMD_COMPOBJECT('RSRT', void, SetRecallTime, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSLT', void, UpdateRecallTime, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncAgentMemory::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- recall time
        if (!ps->Put (this->entityObject, 'RSRT', this->recallTime))
        {
            return false;
        }
        
        return true;
    }

    return false;
}
