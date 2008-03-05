#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ngeometrynode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "kernel/npersistserver.h"
#include "kernel/ndependencyserver.h"

static void n_setsurface(void* slf, nCmd* cmd);
static void n_getsurface(void* slf, nCmd* cmd);
static void n_setstream(void* slf, nCmd* cmd);
static void n_getstream(void* slf, nCmd* cmd);
static void n_setstreamindex(void* slf, nCmd* cmd);
static void n_getstreamindex(void* slf, nCmd* cmd);
static void n_setworldcoord(void* slf, nCmd* cmd);
static void n_getworldcoord(void* slf, nCmd* cmd);
#ifndef NGAME
static void n_getsurfacenode(void* slf, nCmd* cmd);
#endif

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngeometrynode

    @cppclass
    nGeometryNode
    
    @superclass
    nabstractshadernode

    @classinfo
    This one can render a simple static mesh.
*/
void
n_initcmds_nGeometryNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsurface_s",        'SSFC', n_setsurface);
    cl->AddCmd("s_getsurface_v",        'GSFC', n_getsurface);
    cl->AddCmd("v_setstream_s",         'SSTG', n_setstream);
    cl->AddCmd("s_getstream_v",         'GSTG', n_getstream);
    cl->AddCmd("v_setstreamindex_i",    'SSTI', n_setstreamindex);
    cl->AddCmd("i_getstreamindex_v",    'GSTI', n_getstreamindex);
    cl->AddCmd("v_setworldcoord_b",     'SWCD', n_setworldcoord);
    cl->AddCmd("b_getworldcoord_v",     'GWCD', n_getworldcoord);
#ifndef NGAME
    cl->AddCmd("o_getsurfacenode_v",    'CGSN', n_getsurfacenode);
#endif
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsurface
    @input
    s(surface node)
    @output
    v
    @info
    Set the surface for the mesh.
*/
static void
n_setsurface(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    self->SetSurface(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsurface
    @input
    v
    @output
    s(surface node)
    @info
    Gets the surface for the geometry.
*/
static void
n_getsurface(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    cmd->Out()->SetS(self->GetSurface());
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @cmd
    getsurfacenode
    @input
    v
    @output
    o(surface node)
    @info
    Gets the surface for the geometry.
*/
static void
n_getsurfacenode(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    cmd->Out()->SetO(self->GetSurfaceNode());
}
#endif

//------------------------------------------------------------------------------
/**
    @cmd
    setstream
    @input
    s(stream node)
    @output
    v
    @info
    Set the stream for the mesh.
*/
static void
n_setstream(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    self->SetStream(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstream
    @input
    v
    @output
    s(stream node)
    @info
    Gets the stream for the geometry.
*/
static void
n_getstream(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    cmd->Out()->SetS(self->GetStream());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstreamindex
    @input
    i(stream index)
    @output
    v
    @info
    Set the stream index for the geometry.
*/
static void
n_setstreamindex(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    self->SetStreamIndex(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstreamindex
    @input
    v
    @output
    i(stream index)
    @info
    Gets the stream index for the geometry.
*/
static void
n_getstreamindex(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    cmd->Out()->SetI(self->GetStreamIndex());
}



//------------------------------------------------------------------------------
/**
*/
static void
n_setworldcoord(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    self->SetWorldCoord(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_getworldcoord(void* slf, nCmd* cmd)
{
    nGeometryNode* self = (nGeometryNode*) slf;
    cmd->Out()->SetB(self->GetWorldCoord());
}

//------------------------------------------------------------------------------
/**
*/
bool
nGeometryNode::SaveCmds(nPersistServer* ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;
        nDependencyServer *depServer = ps->GetDependencyServer("ngeometrynode", "setsurface");

        // --- setsurface ---
        if (this->refSurfaceNode.isvalid())
        {
            if (depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
            {
                cmd = ps->GetCmd(this, 'SDCY');
                nString path;
                depServer->SaveObject((nObject *) this->GetSurfaceNode(), "setsurface", path);
                cmd->In()->SetS(path.Get());
                cmd->In()->SetS(depServer->GetFullName().Get());
                cmd->In()->SetS("setsurface");
                ps->PutCmd(cmd);
            }
            else
            {
                cmd = ps->GetCmd(this, 'SSFC');
                cmd->In()->SetS(this->refSurfaceNode.getname());
                ps->PutCmd(cmd);
            }
        }

        // --- setstream ---
        if (this->refStreamGeometry.isvalid())
        {
            depServer = ps->GetDependencyServer("ngeometrynode", "setstream");

            if (depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
            {
                //TODO- persist stream geometry node
                nString path;
                depServer->SaveObject(this->refStreamGeometry.get(), "setstream", path);
                cmd = ps->GetCmd(this, 'SDCY');
                cmd->In()->SetS(path.Get());
                cmd->In()->SetS(depServer->GetFullName().Get());
                cmd->In()->SetS("setstream");
                ps->PutCmd(cmd);
            }
            else
            {
                cmd = ps->GetCmd(this, 'SSTG');
                cmd->In()->SetS(this->refStreamGeometry.getname());
                ps->PutCmd(cmd);
            }
        }

        // --- setstreamindex ---
        if (this->GetStreamIndex() != -1)
        {
            cmd = ps->GetCmd(this, 'SSTI');
            cmd->In()->SetI(this->GetStreamIndex());
            ps->PutCmd(cmd);
        }

        // --- setworldcoord ---
        if (this->GetWorldCoord())
        {
            cmd = ps->GetCmd(this, 'SWCD');
            cmd->In()->SetB(this->GetWorldCoord());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
