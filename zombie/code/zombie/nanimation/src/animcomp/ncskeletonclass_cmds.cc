#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncskeletonclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/ncskeletonclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSkeletonClass)
    // FIXME NCMDPROTONATIVECPP_DECLARE_INARG doesn't create the right argument type
    NSCRIPT_ADDCMD_COMPCLASS('ASSA', void, SetStateAnim, 2 , (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGSA', const char *, GetStateAnim, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFM', void, SetStateMotion, 2 , (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFN', const char *, GetStateMotion, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ABJT', void, BeginJoints, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASJN', void, SetJoint, 5, (int, int&, const vector3&, const quaternion&, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASJT', void, SetNamedJoint, 6, (int, int&, const vector3&, const quaternion&, const vector3&, const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AEJT', void, EndJoints, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGNJ', int, GetNumJoints, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAJN', void, AddJointName, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACN', const nString, GetJointName, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGJT', void, GetJoint, 0, (), 5, (int, int&, vector3&, quaternion&, vector3&));
    NSCRIPT_ADDCMD_COMPCLASS('ABST', void, BeginStates, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASSS', void, SetState, 3, (int, int, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASSJ', void, SetStateWithJointGroup, 4, (int, int, int, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASSN', void, SetStateName, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGSN', const char *, GetStateName, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AEST', void, EndStates, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGNS', int, GetNumStates, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ABCL', void, BeginClips, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASCL', void, SetClip, 3, (int, int, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASCP', void, SetClipAndCurves, 4, (int, int, int , const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AECL', void, EndClips, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGNC', int, GetNumClips, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASPO', void, SetPerceptionOffset, 2, (int, vector3), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGPO', vector3, GetPerceptionOffset, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGBN', unsigned int, GetJointByName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGCA', const char *, GetClipAt, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASAC', void, SetStateAnimChannel, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASSB', void, SetStateBBox, 3 , (int, vector3&, vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGSB', void, GetStateBBox, 1, (int), 2, (vector3&, vector3&));
    NSCRIPT_ADDCMD_COMPCLASS('ASFT', void, SetFadeInTime, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGFT', float, GetFadeInTime, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ABJG', void, BeginJointGroups, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASGJ', void, SetGroupNumberJoints, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASTG', void, SetGroup, 5, (int, int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AEJG', void, EndJointGroups, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAGS', void, AssignGroupToState, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MBTJ', void, BeginTweakableJoints, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSTJ', void, SetTweakableJoint, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('METJ', void, EndTweakableJoints, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAB', void, BeginAttachments, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAA', void, SetAttachmentHelper, 5, (int, const char *, int, vector3, quaternion), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAC', void, EndAttachments, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAM', void, RemoveAnimState, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACO', bool, ChangeAnimStateName, 2, (int, const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADL', void, SetSubmissiveLeaderState, 2, (const nString&, const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADM', int, GetLeaderStateOf, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADN', const nString, GetLeaderStateNameOf, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAQ', bool, SaveClass, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFO', void, SetStateFollowsJoint, 2, (const nString&, const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFP', const nString, GetStateFollowsJoint, 1, (const nString&), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncSkeletonClass::SaveCmds(nPersistServer *ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        nCmd* cmd;
        int numJoints = this->GetCharacter().GetSkeleton().GetNumJoints();
        int numStates = this->animStateArray.GetNumStates();
        int numJointGroups = this->GetCharacter().GetSkeleton().GetNumberJointGroups();
        int numDynAttachments = this->dynamicAttachmentPool.Size();

        // --- BeginJoints ---
        if (numJoints != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'ABJT');
            cmd->In()->SetI(this->GetCharacter().GetSkeleton().GetNumJoints());
            ps->PutCmd(cmd);
        }

        // --- SetJoint ---
        for (int i = 0; i < numJoints; i++)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASJT');
            cmd->In()->SetI(i);
            cmd->In()->SetI(this->character.GetSkeleton().GetJointAt(i).GetParentJointIndex());

            /// @todo ma.garcias - NCMDPROTONATIVECPP_DECLARE_INARG doesn't create the right argument type
            //cmd->In()->SetV3(this->character.GetSkeleton().GetJointAt(i).GetPoseTranslate());
            //cmd->In()->SetV4(vector4(q0.x, q0.y, q0.z, q0.w));
            //cmd->In()->SetV3(this->character.GetSkeleton().GetJointAt(i).GetPoseScale());

            const vector3& translate = this->character.GetSkeleton().GetJointAt(i).GetPoseTranslate();
            const quaternion& rotate = this->character.GetSkeleton().GetJointAt(i).GetPoseRotate();
            const vector3& scale = this->character.GetSkeleton().GetJointAt(i).GetPoseScale();

            cmd->In()->SetF(translate.x);
            cmd->In()->SetF(translate.y);
            cmd->In()->SetF(translate.z);

            cmd->In()->SetF(rotate.x);
            cmd->In()->SetF(rotate.y);
            cmd->In()->SetF(rotate.z);
            cmd->In()->SetF(rotate.w);

            cmd->In()->SetF(scale.x);
            cmd->In()->SetF(scale.y);
            cmd->In()->SetF(scale.z);

            //name parameter!
            cmd->In()->SetS(this->character.GetSkeleton().GetJointAt(i).GetName().Get());
            ps->PutCmd(cmd);
        }

        // --- EndJoints ---
        if (numJoints != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AEJT');
            ps->PutCmd(cmd);
        }

        // --- BeginJointGroups ---
        if (numJointGroups != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'ABJG');
            cmd->In()->SetI(numJointGroups);
            ps->PutCmd(cmd);
        }

        // --- SetJointGroup ---
        for (int i = 0; i < numJointGroups; i++)
        {
            nJointGroup jointGroup = this->GetCharacter().GetSkeleton().GetJointGroupAt(i);

            // --- SetGroupNumberJoints ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASGJ');
            cmd->In()->SetI(i);
            cmd->In()->SetI(jointGroup.GetGroupSize());
            ps->PutCmd(cmd);

            //set all jointIndices in groups of 4
            for (int jointIndex = 0; jointIndex < jointGroup.GetGroupSize();)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'ASTG');
                cmd->In()->SetI(i);

                for (int j=0; j<4; j++, jointIndex++)
                {
                    if (jointIndex < jointGroup.GetGroupSize())
                    {
                        cmd->In()->SetI(jointGroup.GetJointIndexAt(jointIndex));
                    }
                    else
                    {
                        cmd->In()->SetI(0);
                    }
                }
                ps->PutCmd(cmd);
            }        
        }

        // --- EndJointGroups ---
        if (numJointGroups != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AEJG');
            ps->PutCmd(cmd);
        }


        // --- BeginStates ---
        if (numStates != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'ABST');
            cmd->In()->SetI(this->animStateArray.GetNumStates());
            ps->PutCmd(cmd);
        }

        // --- SetState ---
        for (int stateIndex = 0; stateIndex< numStates; stateIndex++)
        {
            /// --- SetStateWithJointGroup ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASSJ');
            cmd->In()->SetI(stateIndex);
            cmd->In()->SetI(this->animStateArray.GetStateAt(stateIndex).GetAnimGroupIndex());
            cmd->In()->SetI(this->animStateArray.GetStateAt(stateIndex).GetJointGroup());
            cmd->In()->SetF(this->animStateArray.GetStateAt(stateIndex).GetFadeInTime());
            ps->PutCmd(cmd);

            // --- SetStateAnim ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASSA');
            cmd->In()->SetI(stateIndex);
            cmd->In()->SetS(this->animStateArray.GetStateAt(stateIndex).GetAnimFile());
            ps->PutCmd(cmd);

            // --- SetStateMotion ---
            if (this->animStateArray.GetStateAt(stateIndex).GetMotionFile())
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'AAFM');
                cmd->In()->SetI(stateIndex);
                cmd->In()->SetS(this->animStateArray.GetStateAt(stateIndex).GetMotionFile());
                ps->PutCmd(cmd);
            }            

            // --- SetStateName ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASSN');
            cmd->In()->SetI(stateIndex);
            cmd->In()->SetS(this->animStateArray.GetStateAt(stateIndex).GetName().Get());
            ps->PutCmd(cmd);

            // --- SetStateBoundingBox ---
            bbox3 box;
            this->animStateArray.GetStateAt(stateIndex).GetBoundingBox(box);
            vector3 center = box.center();
            vector3 extents = box.extents();
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASSB');

            cmd->In()->SetI(stateIndex);

            cmd->In()->SetF(center.x);
            cmd->In()->SetF(center.y);
            cmd->In()->SetF(center.z);

            cmd->In()->SetF(extents.x);
            cmd->In()->SetF(extents.y);
            cmd->In()->SetF(extents.z);

            ps->PutCmd(cmd);

            // --- SetPerceptionOffset ---
            cmd = ps->GetCmd(this->GetEntityClass(), 'ASPO');
            cmd->In()->SetI(stateIndex);
            vector3 poffset = this->animStateArray.GetStateAt(stateIndex).GetPerceptionOffset();
            cmd->In()->SetF(poffset.x);
            cmd->In()->SetF(poffset.y);
            cmd->In()->SetF(poffset.z);
            ps->PutCmd(cmd);


            // --- BeginClips ---
            if (this->animStateArray.GetStateAt(stateIndex).GetNumClips() != 0)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'ABCL');
                cmd->In()->SetI(stateIndex);
                cmd->In()->SetI(this->animStateArray.GetStateAt(stateIndex).GetNumClips());
                ps->PutCmd(cmd);
            }

            // --- SetClip ---
            for (int clipIndex = 0; clipIndex < this->animStateArray.GetStateAt(stateIndex).GetNumClips(); clipIndex++)
            {
                nVariable::Handle varHandle = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetWeightChannelHandle();
                cmd = ps->GetCmd(this->GetEntityClass(), 'ASCL');
                cmd->In()->SetI(stateIndex);
                cmd->In()->SetI(clipIndex);
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(varHandle));
                ps->PutCmd(cmd);
            }

            // --- EndClips ---
            if (this->animStateArray.GetStateAt(stateIndex).GetNumClips() != 0)
            {
                cmd = ps->GetCmd(this->GetEntityClass(), 'AECL');
                cmd->In()->SetI(stateIndex);
                ps->PutCmd(cmd);
            }

        }

        // --- EndStates ---
        if (numStates != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AEST');
            ps->PutCmd(cmd);
        }

        // --- Begin DynamicAttachments ---
        if (numDynAttachments != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAB');
            cmd->In()->SetI(numDynAttachments);
            ps->PutCmd(cmd);
        }

        // --- DynamicAttachments ---
        for (int i=0; i<numDynAttachments; i++)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAA');
            cmd->In()->SetI(i);
            cmd->In()->SetS(this->dynamicAttachmentPool[i].GetName().Get());
            cmd->In()->SetI(this->dynamicAttachmentPool[i].GetJointIndex());

            vector3 pos = this->dynamicAttachmentPool[i].GetTranslation();
            cmd->In()->SetF(pos.x);
            cmd->In()->SetF(pos.y);
            cmd->In()->SetF(pos.z);

            quaternion quat = this->dynamicAttachmentPool[i].GetRotation();
            cmd->In()->SetF(quat.x);
            cmd->In()->SetF(quat.y);
            cmd->In()->SetF(quat.z);
            cmd->In()->SetF(quat.w);

            ps->PutCmd(cmd);
        }

        // --- End DynamicAttachments ---
        if (numDynAttachments != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAC');
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
