#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nstaticbatchnode_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nstaticbatchnode.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nStaticBatchNode)
    NSCRIPT_ADDCMD('BMES', void, BeginMeshes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SMAT', void, SetMeshAt, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD('GMAT', const char *, GetMeshAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EMES', void, EndMeshes, 0, (), 0, ());
    NSCRIPT_ADDCMD('GMES', int, GetNumMeshes, 0, (), 0, ());
    NSCRIPT_ADDCMD('SPAT', void, SetPositionAt, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD('SEAT', void, SetEulerAt, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD('SQAT', void, SetQuatAt, 2, (int, const quaternion&), 0, ());
    NSCRIPT_ADDCMD('SSAT', void, SetScaleAt, 2, (int, float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nStaticBatchNode::SaveCmds(nPersistServer* ps)
{
    // NOT A BUG- shape node info must never be persisted
    if (nGeometryNode::SaveCmds(ps))
    {
        nCmd* cmd;
        static const vector3 nullVec;
        static const vector3 oneVec(1.0f, 1.0f, 1.0f);

        // --- beginmeshes ---
        cmd = ps->GetCmd(this, 'BMES');
        cmd->In()->SetI(this->meshArray.Size());
        ps->PutCmd(cmd);

        int i;
        for (i = 0; i < this->meshArray.Size(); ++i)
        {
            BatchedMesh& batchedMesh = this->meshArray.At(i);

            // --- setmeshat ---
            cmd = ps->GetCmd(this, 'SMAT');
            cmd->In()->SetI(i);
            cmd->In()->SetS(batchedMesh.meshFile.Get());
            ps->PutCmd(cmd);

            if (batchedMesh.hasTransform)
            {
                // --- setpositionat ---
                const vector3& pos = batchedMesh.modelTransform.gettranslation();
                if (!pos.isequal(nullVec, 0.0f))
                {
                    cmd = ps->GetCmd(this, 'SPAT');
                    cmd->In()->SetI(i);
                    cmd->In()->SetF(pos.x);
                    cmd->In()->SetF(pos.y);
                    cmd->In()->SetF(pos.z);
                    ps->PutCmd(cmd);
                }

                if (batchedMesh.modelTransform.iseulerrotation())
                {
                    // --- seteulerat ---
                    const vector3& euler = batchedMesh.modelTransform.geteulerrotation();
                    if (!euler.isequal(nullVec, 0.0f))
                    {
                        cmd = ps->GetCmd(this, 'SEAT');
                        cmd->In()->SetI(i);
                        cmd->In()->SetF(n_rad2deg(euler.x));
                        cmd->In()->SetF(n_rad2deg(euler.y));
                        cmd->In()->SetF(n_rad2deg(euler.z));
                        ps->PutCmd(cmd);
                    }
                }
                else
                {
                    // --- setquatat ---
                    static const quaternion identQuat;
                    const quaternion& quat = batchedMesh.modelTransform.getquatrotation();
                    if (!quat.isequal(identQuat, 0.0f))
                    {
                        cmd = ps->GetCmd(this, 'SQAT');
                        cmd->In()->SetI(i);
                        cmd->In()->SetF(quat.x);
                        cmd->In()->SetF(quat.y);
                        cmd->In()->SetF(quat.z);
                        cmd->In()->SetF(quat.w);
                        ps->PutCmd(cmd);
                    }
                }

                // --- setscaleat ---
                const vector3& scale = batchedMesh.modelTransform.getscale();
                if (!scale.isequal(oneVec, 0.0f))
                {
                    cmd = ps->GetCmd(this, 'SSAT');
                    cmd->In()->SetI(i);
                    cmd->In()->SetF(scale.x);
                    ps->PutCmd(cmd);
                }
            }
        }

        // --- endmeshes ---
        cmd = ps->GetCmd(this, 'EMES');
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
