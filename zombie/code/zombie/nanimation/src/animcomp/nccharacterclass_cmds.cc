#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacterclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "nscene/ncscenelodclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncCharacterClass)
    //<OBSOLETE>
    NSCRIPT_ADDCMD_COMPCLASS('ASSC', void, SetSkeletonClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGSC', const char *, GetSkeletonClass, 0, (), 0, ());
    //</OBSOLETE>
    NSCRIPT_ADDCMD_COMPCLASS('AABG', void, SetSkeletonLevelClass, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABH', const char *, GetSkeletonLevelClass, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABI', void, SetPhysicsSkeletonClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABJ', const char *, GetPhysicsSkeletonClass, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABF', int, GetNumberLevelSkeletons, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABQ', void, SetRagdollSkeletonClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AABR', bool, GetRagdollSkeletonClass, 1, (nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACX', void, SetFirstPersonLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACY', int, GetFirstPersonLevel, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncCharacterClass::SaveCmds(nPersistServer *ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        nCmd* cmd;

        ncSceneLodClass *sceneLodComp = this->GetComponentSafe<ncSceneLodClass>();
        
        if (this->GetNumberLevelSkeletons() == 1)
        {
            // --- SetSkeletonClass ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASSC');
            cmd->In()->SetS(this->GetSkeletonClass());
            ps->PutCmd(cmd);
        }
        else
        {
            // --- SetSkeletonLevelClass ---
            for (int i=0; (sceneLodComp) && (i< this->GetNumberLevelSkeletons()) && (i< sceneLodComp->GetNumLevels()); i++)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'AABG');
                cmd->In()->SetI(i);
                cmd->In()->SetS(this->GetSkeletonLevelClass(i));
                ps->PutCmd(cmd);
            }
        }
        // --- SetPhysicsSkeletonClass ---
        if (this->physicsSkeletonIndex != -1)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AABI');
            cmd->In()->SetS(this->GetPhysicsSkeletonClass());
            ps->PutCmd(cmd);
        }

        // --- SetRagdollSkeletonClass ---
        nString ragSkeletonClass;
        if (this->GetRagdollSkeletonClass(ragSkeletonClass))
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AABQ');
            cmd->In()->SetS(ragSkeletonClass.Get());
            ps->PutCmd(cmd);
        }
        
        // --- SetFirstPersonLevel ---
        if (this->GetFirstPersonLevel() != -1)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AACX');
            cmd->In()->SetI(this->GetFirstPersonLevel());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
