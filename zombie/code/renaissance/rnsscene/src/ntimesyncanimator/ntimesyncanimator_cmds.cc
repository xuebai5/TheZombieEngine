//------------------------------------------------------------------------------
//  ntimesyncanimator_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ntimesyncanimator/ntimesyncanimator.h"

NSCRIPT_INITCMDS_BEGIN(nTimeSyncAnimator)
    NSCRIPT_ADDCMD('CSTC', void,  SetCycleTime,  1, (float), 0, ());
    NSCRIPT_ADDCMD('CGTC', float, GetCycleTime,  0, (),      0, ());
    NSCRIPT_ADDCMD('CSTO', void,  SetOffsetTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGTO', float, GetOffsetTime, 0, (),      0, ());
NSCRIPT_INITCMDS_END()
//---------------------------------------------------------------------------
/**
*/
bool
nTimeSyncAnimator::SaveCmds(nPersistServer * ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        ps->Put(this, 'CSTC', this->cycleTime);
        ps->Put(this, 'CSTO', this->offsetTime);
        return true;
    }
    return false;
}


//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------