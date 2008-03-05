#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nstreamgeometrynode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nstreamgeometrynode.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nStreamGeometryNode)
    NSCRIPT_ADDCMD('BSHS', void, BeginShapes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SSAT', void, SetShapeAt, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD('GSAT', const char *, GetShapeAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESHS', void, EndShapes, 0, (), 0, ());
    NSCRIPT_ADDCMD('GSHS', int, GetNumShapes, 0, (), 0, ());
    NSCRIPT_ADDCMD('SUPA', void, SetUvPosAt, 3, (int, int, const vector2&), 0, ());
    NSCRIPT_ADDCMD('SUSA', void, SetUvScaleAt, 3, (int, int, const vector2&), 0, ());
    NSCRIPT_ADDCMD('SFQA', void, SetFrequencyAt, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GFQA', int, GetFrequencyAt, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nStreamGeometryNode::SaveCmds(nPersistServer* ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        nCmd* cmd;
        static const vector2 nullVec;
        static const vector2 oneVec(1.0f, 1.0f);

        //--- beginmeshes ---
        cmd = ps->GetCmd(this, 'BSHS');
        cmd->In()->SetI(this->streamSlots.Size());
        ps->PutCmd(cmd);

        int index;
        for (index = 0; index < this->streamSlots.Size(); ++index)
        {
            StreamSlot& slot = this->streamSlots[index];

            //--- setmeshat ---
            cmd = ps->GetCmd(this, 'SSAT');
            cmd->In()->SetI(index);
            cmd->In()->SetS(this->GetShapeAt(index));
            ps->PutCmd(cmd);

            for (int layer = 0; layer < nGfxServer2::MaxTextureStages; ++layer)
            {
                if (slot.useTextureTransform[layer])
                {
                    //--- setuvposat ---
                    const vector2& pos = slot.textureTransform[layer].gettranslation();
                    cmd = ps->GetCmd(this, 'SUVP');
                    cmd->In()->SetI(index);
                    cmd->In()->SetI(layer);
                    cmd->In()->SetF(pos.x);
                    cmd->In()->SetF(pos.y);
                    ps->PutCmd(cmd);

                    //--- setuvscaleat ---
                    const vector2& scale = slot.textureTransform[layer].getscale();
                    cmd = ps->GetCmd(this, 'SUVS');
                    cmd->In()->SetI(index);
                    cmd->In()->SetI(layer);
                    cmd->In()->SetF(scale.x);
                    cmd->In()->SetF(scale.y);
                    ps->PutCmd(cmd);
                }

            }

            //--- setfrequencyat ---
            if (slot.frequency > 1)
            {
                cmd = ps->GetCmd(this, 'SFQA');
                cmd->In()->SetI(index);
                cmd->In()->SetI(slot.frequency);
                ps->PutCmd(cmd);
            }
        }

        //--- endmeshes ---
        cmd = ps->GetCmd(this, 'ESHS');
        ps->PutCmd(cmd);

        return true;
    }

    return false;
}
