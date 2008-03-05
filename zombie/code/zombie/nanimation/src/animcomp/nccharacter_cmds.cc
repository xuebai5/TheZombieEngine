#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncanimation_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "entity/nentityobjectserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncCharacter)
    NSCRIPT_ADDCMD_COMPOBJECT('ASAS', void, SetActiveStateIndex, 4, (int, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ASAN', void, SetActiveStateName, 4, (const char*, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('APAU', void, Pause, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGNS', int, GetNumberStates, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AILI', bool, IsLoopedByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AILN', bool, IsLoopedByName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGSN', const nString, GetStateName, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGSI', int, GetStateIndex, 1, (nString), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AISP', bool, IsPlaying, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ASST', void, SetActiveAnimState, 4, (int, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAFK', void, SetActiveAnimStateWithSpeedFactor, 5, (int, bool, bool, bool, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGTD', float, GetStateDuration, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGPT', float, GetElapsedTime, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGLT', float, GetRemainingTime, 2, (int,bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGTO', vector3, GetPerceptionOffset, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ASTO', void, SetPerceptionOffset, 2, (int, vector3), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAJ', int, GetNumberAttachments, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAK', const char*, GetAttachmentName, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGAE', nEntityObjectId, GetAttachedEntity, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AATT', void, Attach, 2, (const char *, nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ADTT', void, Dettach, 1, (nEntityObjectId), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('AAAL', bool, RemoveAnimState, 2 , (bool, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAN', bool, ChangeAnimStateName, 3 , (bool, int, const nString&), 0, ());
#endif
    NSCRIPT_ADDCMD_COMPOBJECT('AACP', bool, SetFadeInTime, 3, (bool, int, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AADO', float, GetFadeInTime, 2, (bool, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAT', void, SetActiveMorphStateByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAU', int, GetActiveMorphStateByIndex, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AABS', void, SetRagdollEntityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AABT', nEntityObjectId, GetRagdollEntityId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AABU', void, SetRagdollActive, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACJ', bool, GetRagdollActive, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AADB', void, SetFirstPersonActive, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AADC', bool, GetFirstPersonActive, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFAS', void, SetFirstPersonActiveStateIndex, 4, (int, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFAN', void, SetFirstPersonActiveStateName, 4, (const char*, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFST', void, SetFirstPersonActiveAnimState, 4, (int, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAFL', void, SetFirstPersonActiveAnimStateWithSpeedFactor, 5, (int, bool, bool, bool, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFGN', int, GetFirstPersonNumberStates, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFSN', const nString, GetFirstPersonStateName, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AFGI', int, GetFirstPersonStateIndex, 1, (nString), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ASOF', void, SetFirstPersonOffset, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AGOF', const vector3&, GetFirstPersonOffset, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ASOI', int, GetFirstPersonOffsetIndex, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSJL', void, SetJointLookAtVector, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGJL', void, GetJointLookAtVector, 0, (), 2, (int&, vector3&));
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- SetActiveStateIndex ---
        if (!this->activeStates.Empty())
        {
            for (int i=0; i< this->activeStates.Size(); i++)
            {
                if (this->activeStates[i].GetStateIndex() != -1)
                {
                    cmd = ps->GetCmd(this->GetEntityObject(), 'ASST');
                    cmd->In()->SetI(this->activeStates[i].GetStateIndex());
                    cmd->In()->SetB(this->activeStates[i].GetBackwards());
                    cmd->In()->SetB(this->activeStates[i].GetRepeat());
                    cmd->In()->SetB(this->activeStates[i].GetTransition());
                    ps->PutCmd(cmd);
                }
            }
        }

        // --- SetFirstPersonActiveAnimationState ---
        if (!this->fpersonActiveStates.Empty())
        {
            if (this->fpersonActiveStates[0].GetStateIndex() != -1)
            {
                cmd = ps->GetCmd(this->GetEntityObject(), 'AFST');
                vector3 offset;
                cmd->In()->SetI(this->fpersonActiveStates[0].GetStateIndex());
                cmd->In()->SetB(this->fpersonActiveStates[0].GetBackwards());
                cmd->In()->SetB(this->fpersonActiveStates[0].GetRepeat());
                cmd->In()->SetB(this->fpersonActiveStates[0].GetTransition());
                ps->PutCmd(cmd);
            }
        }

        // --- Attachments ---
        if (! this->dynamicAttachmentPool.Empty())
        {
            for (int i=0; i< this->dynamicAttachmentPool.Size(); i++)
            {
                if (this->dynamicAttachmentPool[i].GetEntityObject() != 0 && nEntityObjectServer::Instance()->GetEntityObjectType(this->dynamicAttachmentPool[i].GetEntityObject()->GetId()) == nEntityObjectServer::Normal)
                {
                    cmd = ps->GetCmd(this->GetEntityObject(), 'AATT');
                    cmd->In()->SetS(this->dynamicAttachmentPool[i].GetName().Get());
                    cmd->In()->SetI(this->dynamicAttachmentPool[i].GetEntityObject()->GetId());
                    ps->PutCmd(cmd);
                }
            }
        }

        return true;
    }
    return false;
}

