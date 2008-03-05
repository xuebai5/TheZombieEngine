#include "precompiled/pchnanimation.h"

#include "ncragdoll/ncragdollclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncRagDollClass,ncSkeletonClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncRagDollClass)
    NSCRIPT_ADDCMD_COMPCLASS('AAFI', void, SetRagType, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFJ', const char *, GetRagType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABM', void, SetRagdollJoint, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABN', int, GetRagdollJoint, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACB', void, SetJointCorrespondence, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACC', int, GetGfxLodJointCorrespondence, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACD', int, GetGfxRagJointCorrespondence, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACH', int, GetGfxLodJointCorrespondenceByIndex, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACI', int, GetGfxRagJointCorrespondenceByIndex, 2, (int, int), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
*/
bool ncRagDollClass::SaveCmds(nPersistServer* ps)
{
    if (ncSkeletonClass::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- SetRagType ---
        if (this->ragDollType != INVALIDTYPE)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAFI');
            cmd->In()->SetS(this->GetRagType());
            ps->PutCmd(cmd);
        }

        // --- SetRagdollJoint ---
        for (int i=0; i< this->ragdollJoints.Size(); i++)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AABM');
            int gfxJoint = this->ragdollJoints.GetKeyAt(i);
            int phyJoint = this->ragdollJoints.GetElementAt(i);
            cmd->In()->SetI(gfxJoint);
            cmd->In()->SetI(phyJoint);
            ps->PutCmd(cmd);
        }
        
        // --- SetJointCorrespondence ---
        int lodLevels = this->lodRagCorrespondence.Size();
        for (int lodLevel=0; lodLevel< lodLevels; lodLevel++)
        {
            int numCorrespond = this->lodRagCorrespondence[lodLevel]->GetNumCorrespondences();
            for (int i=0; i< numCorrespond; i++)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'AACB');
                int ragJointIdx = this->lodRagCorrespondence[lodLevel]->GetRagCorrespondenceByIndex(i);
                int lodJointIdx = this->lodRagCorrespondence[lodLevel]->GetLodCorrespondenceByIndex(i);
                cmd->In()->SetI(lodLevel);
                cmd->In()->SetI(lodJointIdx);
                cmd->In()->SetI(ragJointIdx);
                ps->PutCmd(cmd);
            }
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
