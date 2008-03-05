#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloadarea_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nloadarea.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nLoadArea)

    NSCRIPT_ADDCMD('SDSC', void, SetDescription, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GDSC', const char *, GetDescription, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSPC', void, SetSpaceId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('GSPC', nEntityObjectId, GetSpaceId, 0, (), 0, ());
    NSCRIPT_ADDCMD('BCEL', void, BeginCells, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SCAT', void, SetCellAt, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GCAT', int, GetCellAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GNMC', int, GetNumCells, 0, (), 0, ());
    NSCRIPT_ADDCMD('ECEL', void, EndCells, 0, (), 0, ());

    #ifndef NGAME
    /*NSCRIPT_ADDCMD('SWIZ', void, SetWizard, 1, (nEntityObjectId), 0, ());*/
    NSCRIPT_ADDCMD('MSIS', void, SetAreaStatsCounter, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD('MLDS', bool, LogDebugStats, 0, (), 0, ());
    #endif //NGAME

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    destructor
*/
bool
nLoadArea::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd *cmd;

        //--- setdescription ---
        if (this->GetDescription())
        {
            cmd = ps->GetCmd(this, 'SDSC');
            cmd->In()->SetS(this->GetDescription());
            ps->PutCmd(cmd);
        }

        //--- setspaceid ---
        if (this->GetSpaceId() != nEntityObjectServer::IDINVALID)
        {
            cmd = ps->GetCmd(this, 'SSPC');
            cmd->In()->SetI(this->GetSpaceId());
            ps->PutCmd(cmd);
        }

        int numCells = this->GetNumCells();
        if (numCells > 0)
        {
            //--- begincells ---
            cmd = ps->GetCmd(this, 'BCEL');
            cmd->In()->SetI(this->GetNumCells());
            ps->PutCmd(cmd);

            for (int index = 0; index < numCells; ++index)
            {
                //--- setcellat---
                cmd = ps->GetCmd(this, 'SCAT');
                cmd->In()->SetI(index);
                cmd->In()->SetI(this->GetCellAt(index));
                ps->PutCmd(cmd);
            }

            //--- endcells ---
            cmd = ps->GetCmd(this, 'ECEL');
            ps->PutCmd(cmd);
        }

        return true;
    }

    return false;
}
