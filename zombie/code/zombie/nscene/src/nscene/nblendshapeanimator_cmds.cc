#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nblendshapeanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nblendshapeanimator.h"
#include "kernel/npersistserver.h"

static void n_setanimation(void* slf, nCmd* cmd);
static void n_getanimation(void* slf, nCmd* cmd);
static void n_setanimationgroup(void* slf, nCmd* cmd);
static void n_getanimationgroup(void* slf, nCmd* cmd);

static void n_beginstates(void* slf, nCmd* cmd);
static void n_setstate(void* slf, nCmd* cmd);
static void n_setstateanim(void* slf, nCmd* cmd);
static void n_getstateanim(void* slf, nCmd* cmd);
static void n_setstatename(void* slf, nCmd* cmd);
static void n_endstates(void* slf, nCmd* cmd);
static void n_getnumstates(void* slf, nCmd* cmd);
static void n_settargetchange(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nblendshapeanimator
    @cppclass
    nBlendShapeAnimator
    @superclass
    nanimator
    @classinfo
    Animate a blendshape node.
*/

void
n_initcmds_nBlendShapeAnimator(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setanimation_s",      'SANI', n_setanimation);
    cl->AddCmd("s_getanimation_v",      'GANI', n_getanimation);
    cl->AddCmd("v_setanimationgroup_i", 'SANG', n_setanimationgroup);
    cl->AddCmd("i_getanimationgroup_v", 'GANG', n_getanimationgroup);

    cl->AddCmd("v_beginstates_i",       'AAAV', n_beginstates);
    cl->AddCmd("v_setstate_ii",         'AAAW', n_setstate);
    cl->AddCmd("v_setstateanim_is",     'AAAX', n_setstateanim);
    cl->AddCmd("s_getstateanim_i",      'AAAY', n_getstateanim);
    cl->AddCmd("v_setstatename_is",     'AAAZ', n_setstatename);
    cl->AddCmd("v_endstates_v",         'AABA', n_endstates);
    cl->AddCmd("i_getnumstates_v",      'AABB', n_getnumstates);
    cl->AddCmd("v_settargetchange_iiii",'AABC', n_settargetchange);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimation
    @input
    s(Animation File)
    @output
    v
    @info
    Set the name of the animation resource.
*/
static void
n_setanimation(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->SetAnimation(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimation
    @input
    v
    @output
    s(Animation File)
    @info
    Get the name of the animation resource.
*/
static void
n_getanimation(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetS(self->GetAnimation());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimationgroup
    @input
    i(GroupIndxe)
    @output
    v
    @info
    Set the group of the animation to be used.
*/
static void
n_setanimationgroup(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->SetAnimationGroup(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimationgroup
    @input
    v
    @output
    i(GroupIndex)
    @info
    Get the index of the animation group used.
*/
static void
n_getanimationgroup(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetI(self->GetAnimationGroup());
}

//------------------------------------------------------------------------------
/**
    @cmd beginstates
    @output
      v
    @input
      i(numStates)
    @info 
      Begin adding states.
*/
static void
n_beginstates(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->BeginStates(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd setstate
    @output
      v
    @input
      i(stateIndex), i(groupIndex)
    @info 
      Set group for state.
*/
static void
n_setstate(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int groupIndex = cmd->In()->GetI();
    //float fadeInTime = cmd->In()->GetF();
    self->SetState( stateIndex, groupIndex/*, fadeInTime*/ );
}

//------------------------------------------------------------------------------
/**
    @cmd setstateanim
    @output
      v
    @input
      i(stateIndex), s(animFile)
    @info 
      Set animation file for state.
*/
static void
n_setstateanim(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    const char* animFile = cmd->In()->GetS();
    self->SetStateAnim( stateIndex, animFile);
}

//------------------------------------------------------------------------------
/**
    @cmd getstateanim
    @output
      s(animFile)
    @input
      i(stateIndex)
    @info 
      Get animation file for state.
*/
static void
n_getstateanim(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetS( self->GetStateAnim( cmd->In()->GetI() ));
}

//------------------------------------------------------------------------------
/**
    @cmd setstatename
    @output
      v
    @input
      i(stateIndex), s(stateName)
    @info 
      Set state name.
*/
static void
n_setstatename(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    const char* stateName = cmd->In()->GetS();
    self->SetStateName( stateIndex, stateName);
}

//------------------------------------------------------------------------------
/**
    @cmd endstates
    @output
      v
    @input
      v
    @info 
      End declaring animation states.
*/
static void
n_endstates(void* slf, nCmd* /*cmd*/)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->EndStates();
}

//------------------------------------------------------------------------------
/**
    @cmd getnumstates
    @output
      i(numStates)
    @input
      v
    @info 
      Get number of states.
*/
static void
n_getnumstates(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetI( self->GetNumStates() );
}

//------------------------------------------------------------------------------
/**
    @cmd settargetchange
    @output
      v
    @input
      i(stateIndex), i(keyIndex), i(curveIndex), i(targetIndex)
    @info 
      Set target change.
*/
static void
n_settargetchange(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int keyIndex   = cmd->In()->GetI();
    int curveIndex = cmd->In()->GetI();
    int targetIndex = cmd->In()->GetI();
    self->SetTargetChange(stateIndex, keyIndex, curveIndex, targetIndex);
}

//------------------------------------------------------------------------------
/**
*/
bool
nBlendShapeAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        int numStates = this->GetNumStates();

        if( numStates != 0 )
        {
            //--- beginstates ---
            cmd = ps->GetCmd(this, 'AAAV');
            cmd->In()->SetI(numStates);
            ps->PutCmd(cmd);

            for( int i=0; i< numStates; i++)
            {
                nBlendState& blendState = this->blendStateArray.GetStateAt(i);
                
                //--- setstate ---
                cmd = ps->GetCmd(this, 'AAAW');
                cmd->In()->SetI(i);
                cmd->In()->SetI( blendState.GetAnimGroupIndex());
                //cmd->In()->SetF( blendState.GetFadeInTime());
                ps->PutCmd(cmd);

                //--- setstateanim ---
                cmd = ps->GetCmd(this, 'AAAX');
                cmd->In()->SetI(i);
                cmd->In()->SetS( blendState.GetAnimFile());
                ps->PutCmd(cmd);

                //--- setstatename ---
                cmd = ps->GetCmd(this, 'AAAZ');
                cmd->In()->SetI(i);
                cmd->In()->SetS( blendState.GetName().Get());
                ps->PutCmd(cmd);

                nBlendTargetKeyArray& blendTargetArray = blendState.GetTargetKeyArray();

                for( int j=0; j< blendTargetArray.GetNumberTargetKeys(); j++)
                {
                    //--- setchangestate ---
                    cmd = ps->GetCmd(this, 'AABC');
                    cmd->In()->SetI(i);
                    cmd->In()->SetI(blendTargetArray.GetBlendTargetKeyAt(j).GetKeyIndex());
                    cmd->In()->SetI(blendTargetArray.GetBlendTargetKeyAt(j).GetCurveIndex());
                    cmd->In()->SetI(blendTargetArray.GetBlendTargetKeyAt(j).GetTargetIndex());
                    ps->PutCmd(cmd);
                }
            }

            //--- endstates ---
            cmd = ps->GetCmd(this, 'AABA');
            ps->PutCmd(cmd);            
        }
        return true;
    }
    return false;
}
