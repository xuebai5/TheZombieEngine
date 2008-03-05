#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshapedeformeranimator_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshapedeformeranimator.h"
#include "kernel/npersistserver.h"

static void n_setdeformationgrade(void* slf, nCmd* cmd);
static void n_setdeformationchannel(void* slf, nCmd* cmd);
static void n_getdeformationchannel(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshapedeformeranimator
    @cppclass
    nShapeDeformerAnimator
    @superclass
    nanimator
    @classinfo
*/
void
n_initcmds_nShapeDeformerAnimator(nClass* cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_setdeformationgrade_f",       'AABK', n_setdeformationgrade);
    cl->AddCmd("v_setdeformationchannel_s",     'AABL', n_setdeformationchannel);
    cl->AddCmd("s_getdeformationchannel_v",     'AABM', n_getdeformationchannel);
    
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd setdeformationgrade
    @output
      v
    @input
      f(unnamed) -- No info
    @info 
      No information
*/
static void
n_setdeformationgrade(void* slf, nCmd* cmd)
{
    nShapeDeformerAnimator* self = static_cast<nShapeDeformerAnimator*>(slf);
    self->SetDeformationGrade(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd setdeformationchannel
    @output
      v
    @input
      s(unnamed) -- No info
    @info 
      No information
*/
static void
n_setdeformationchannel(void* slf, nCmd* cmd)
{
    nShapeDeformerAnimator* self = static_cast<nShapeDeformerAnimator*>(slf);
    self->SetDeformationChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd getdeformationchannel
    @output
      s(unnamed) -- No info
    @input
      v
    @info 
      No information
*/
static void
n_getdeformationchannel(void* slf, nCmd* cmd)
{
    nShapeDeformerAnimator* self = static_cast<nShapeDeformerAnimator*>(slf);
    cmd->Out()->SetS(self->GetDeformationChannel());
}

//------------------------------------------------------------------------------
/**
*/
bool
nShapeDeformerAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setdeformationchannel ---
        cmd = ps->GetCmd(this, 'AABL');
        cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(this->deformationGradeHandle));
        ps->PutCmd(cmd);


        ////--- setdeformationgrade ---
        //cmd = ps->GetCmd(this, 'AABK');
        //cmd->In()->SetF(this->deformationGrade);
        //ps->PutCmd(cmd);


        return true;
    }
    return false;
}


