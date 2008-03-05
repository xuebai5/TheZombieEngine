#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ninstshapenode_cmds.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------
#include "nscene/ninstshapenode.h"
#include "kernel/npersistserver.h"

NSCRIPT_INITCMDS_BEGIN(nInstShapeNode)
    NSCRIPT_ADDCMD('SISN', void, SetInstanceStream, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GISN', const char *, GetInstanceStream, 0, (), 0, ());
    NSCRIPT_ADDCMD('SNIN', void, SetShape, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GNIN', const char *, GetShape, 0, (), 0, ());
    NSCRIPT_ADDCMD('SFRQ', void, SetFrequency, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GFRQ', int, GetFrequency, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nInstShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setshape ---
        if (this->refShapeNode.isvalid())
        {
            cmd = ps->GetCmd(this, 'SNIN');
            cmd->In()->SetS(this->refShapeNode.getname());
            ps->PutCmd(cmd);

            // --- setfrequency ---
            cmd = ps->GetCmd(this, 'SFRQ');
            cmd->In()->SetI(this->numBatchedInstances);
            ps->PutCmd(cmd);
        }

        // --- setinstancestream ---
        if (!this->instanceStreamName.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SISN');
            cmd->In()->SetS(this->instanceStreamName.Get());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
