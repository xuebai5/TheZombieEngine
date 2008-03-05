#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nswitchnode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nswitchnode.h"
#include "kernel/npersistserver.h"

static void n_setchannel(void* slf, nCmd* cmd);
static void n_getchannel(void* slf, nCmd* cmd);
static void n_setindexnode(void* slf, nCmd* cmd);
static void n_getindexnode(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nswitchnode
    
    @cppclass
    nLodNode
    
    @superclass
    ntransformnode

    @classinfo
    A selector scene node.
*/
void
n_initcmds_nSwitchNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setchannel_s",    'SCHN', n_setchannel);
    cl->AddCmd("s_getchannel_v",    'GCHN', n_getchannel);
    cl->AddCmd("v_setindexnode_is", 'SIDN', n_setindexnode);
    cl->AddCmd("s_getindexnode_i",  'GIDN', n_getindexnode);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel
    @input
    s(channelName)
    @output
    v
    @info
    Set channel name for selector variable.
*/
static void
n_setchannel(void* slf, nCmd* cmd)
{
    nSwitchNode *self = (nSwitchNode *) slf;
    self->SetChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel
    @input
    v
    @output
    s(ChannelName)
    @info
    Get channel name for selector variable.
*/
static
void
n_getchannel(void* slf, nCmd* cmd)
{
    nSwitchNode *self = (nSwitchNode *) slf;
    cmd->Out()->SetS(self->GetChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setindexnode
    @input
    v
    @output
    i(childIndex),s(nodeName)
    @info
    Set specific node for switch index.
*/
static
void
n_setindexnode(void* slf, nCmd* cmd)
{
    nSwitchNode *self = (nSwitchNode *) slf;
    int index = cmd->In()->GetI();
    const char *name = cmd->In()->GetS();
    self->SetIndexNode(index, name);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getindexnode
    @input
    i(childIndex)
    @output
    s(nodeName)
    @info
    Get specific node for switch index.
*/
static
void
n_getindexnode(void* slf, nCmd* cmd)
{
    nSwitchNode *self = (nSwitchNode *) slf;
    const char *name = self->GetIndexNode(cmd->In()->GetI());
    cmd->Out()->SetS(name);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSwitchNode::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setchannel ---
        if (this->GetChannel())
        {
            cmd = ps->GetCmd(this, 'SCHN');
            cmd->In()->SetS(this->GetChannel());
            ps->PutCmd(cmd);
        }

        //--- setindexnode ---
        for (int i = 0; i < this->indexNodes.Size(); i++)
        {
            cmd = ps->GetCmd(this, 'SIDN');
            cmd->In()->SetI(this->indexNodes[i].childIndex);
            cmd->In()->SetS(this->indexNodes[i].nodeName.Get());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
