#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nskingeometrynode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nskingeometrynode.h"
#include "kernel/npersistserver.h"

static void n_beginjointpalette(void* slf, nCmd* cmd);
static void n_setjointindices(void* slf, nCmd* cmd);
static void n_endjointpalette(void* slf, nCmd* cmd);
static void n_getjointpalettesize(void* slf, nCmd* cmd);
static void n_getjointindex(void* slf, nCmd* cmd);
static void n_setstatechannel(void* slf, nCmd* cmd);
static void n_getstatechannel(void* slf, nCmd* cmd);
static void n_setskeletonlevel(void* slf, nCmd* cmd);
static void n_getskeletonlevel(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nskingeometrynode

    @cppclass
    nSkinGeometryNode

    @superclass
    ngeometrynode

    @classinfo
    A geometry decorator that is animated by a character component.
*/
void
n_initcmds_nSkinGeometryNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_beginjointpalette_i",         'BGJP', n_beginjointpalette);
    cl->AddCmd("v_setjointindices_iiiiiiiii",   'SJID', n_setjointindices);
    cl->AddCmd("v_endjointpalette_v",           'EDJP', n_endjointpalette);
    cl->AddCmd("i_getjointpalettesize_v",       'GJPS', n_getjointpalettesize);
    cl->AddCmd("i_getjointindex_ii",            'GJIX', n_getjointindex);
    cl->AddCmd("v_setstatechannel_s",           'SSCN', n_setstatechannel);
    cl->AddCmd("s_getstatechannel_v",           'GSCN', n_getstatechannel);
    cl->AddCmd("v_setskeletonlevel_i",          'MSSL', n_setskeletonlevel);
    cl->AddCmd("i_getskeletonlevel_v",          'MGSL', n_getskeletonlevel);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginjointpalette
    @input
    i(JointPaletteSize)
    @output
    v
    @info
    Begin defining the joint palette.
*/
static void
n_beginjointpalette(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    self->BeginJointPalette(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjointindices
    @input
    i(PaletteStartIndex), i...(JointIndices)
    @output
    v
    @info
    Set up to 8 joint indices in a joint palette.
*/
static void
n_setjointindices(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    int palIndex  = cmd->In()->GetI();
    int i;
    int ji[8];
    for (i = 0; i < 8; i++)
    {
        ji[i] = cmd->In()->GetI();
    }
    self->SetJointIndices(palIndex, ji[0], ji[1], ji[2], ji[3], ji[4], ji[5], ji[6], ji[7]);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endjointpalette
    @input
    v
    @output
    v
    @info
    Finish defining a fragment's joint palette.
*/
static void
n_endjointpalette(void* slf, nCmd* /*cmd*/)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    self->EndJointPalette();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjointpalettesize
    @input
    v
    @output
    i(JointPaletteSize)
    @info
    Get joint palette size.
*/
static void
n_getjointpalettesize(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    cmd->Out()->SetI(self->GetJointPaletteSize());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjointindex
    @input
    i(PaletteIndex)
    @output
    i(JointIndex)
    @info
    Get the joint index at a fragment's joint palette index.
*/
static void
n_getjointindex(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    cmd->Out()->SetI(self->GetJointIndex(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstatechannel
    @input
    s(StateChannelName)
    @output
    v
    @info
    Set the name of the channel which controls the current animation state.
*/
static void
n_setstatechannel(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    self->SetStateChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstatechannel
    @input
    v
    @output
    s(StateChannelName)
    @info
    Get the name of the channel which controls the current animation state.
*/
static void
n_getstatechannel(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    cmd->Out()->SetS(self->GetStateChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskeletonlevel
    @input
    i(SkeletonLevel)
    @output
    v
    @info
    Set the level of the skeleton to retrieve the updated character.
*/
static void
n_setskeletonlevel(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    self->SetSkeletonLevel(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskeletonlevel
    @input
    v
    @output
    i(SkeletonLevel)
    @info
    Get the level of the skeleton to retrieve the updated character.
*/
static void
n_getskeletonlevel(void* slf, nCmd* cmd)
{
    nSkinGeometryNode* self = (nSkinGeometryNode*) slf;
    cmd->Out()->SetI(self->GetSkeletonLevel());
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkinGeometryNode::SaveCmds(nPersistServer* ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        nCmd* cmd;

        int numJoints = this->GetJointPaletteSize();
        if (numJoints > 0)
        {
            //--- beginjointpalette ---
            cmd = ps->GetCmd(this, 'BGJP');
            cmd->In()->SetI(numJoints);
            ps->PutCmd(cmd);

            int palIndex;
            for (palIndex = 0; palIndex < numJoints;)
            {
                //--- setjointindices ---
                cmd = ps->GetCmd(this, 'SJID');
                cmd->In()->SetI(palIndex);
                int lastIndex = palIndex + 8;
                for (; palIndex < lastIndex; palIndex++)
                {
                    if (palIndex < numJoints)
                    {
                        cmd->In()->SetI(this->GetJointIndex(palIndex));
                    }
                    else
                    {
                        cmd->In()->SetI(0);
                    }
                }
                ps->PutCmd(cmd);
            }

            //--- endjointpalette ---
            cmd = ps->GetCmd(this, 'EDJP');
            ps->PutCmd(cmd);
        }
        
        //--- setstatechannel ---
        cmd = ps->GetCmd(this, 'SSCN');
        cmd->In()->SetS(this->GetStateChannel());
        ps->PutCmd(cmd);

        //--- setskeletonlevel ---
        if (this->GetSkeletonLevel() != -1)
        {
            cmd = ps->GetCmd(this, 'MSSL');
            cmd->In()->SetI(this->GetSkeletonLevel());
            ps->PutCmd(cmd);
        }

       return true;
    }
    return false;
}
