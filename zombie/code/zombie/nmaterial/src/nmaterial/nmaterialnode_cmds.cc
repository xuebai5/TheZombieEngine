#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialnode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialnode.h"
#include "kernel/npersistserver.h"
#include "kernel/ndependencyserver.h"

static void n_setmaterial( void* slf, nCmd* cmd );
static void n_getmaterial( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmaterialnode

    @cppclass
    nMaterialNode
    
    @superclass
    nsurfacenode

    @classinfo
    A material node defines a surface through an abstract material.
*/
void
n_initcmds_nMaterialNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmaterial_s",   'SMAT', n_setmaterial);
    cl->AddCmd("s_getmaterial_v",   'GMAT', n_getmaterial);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setmaterial(void* slf, nCmd* cmd)
{
    nMaterialNode* self = (nMaterialNode*) slf;
    const char *path = cmd->In()->GetS();
    self->SetMaterial( path );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_getmaterial(void* slf, nCmd* cmd)
{
    nMaterialNode* self = (nMaterialNode*) slf;
    const char *material = self->GetMaterial( );
    cmd->Out()->SetS( material );
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialNode::SaveCmds(nPersistServer* ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;
        nDependencyServer *depServer = ps->GetDependencyServer("nmaterialnode", "setmaterial");

        // --- setmaterial ---
        if (this->refMaterial.isvalid() && depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
        {
            cmd = ps->GetCmd(this, 'SDCY');
            nString path;
            depServer->SaveObject((nObject *) this->refMaterial.get(), "setmaterial", path);
            cmd->In()->SetS(path.Get());
            cmd->In()->SetS(depServer->GetFullName().Get());
            cmd->In()->SetS("setmaterial");
            ps->PutCmd(cmd);
        }
        else if (this->refMaterial.getname())
        {
            cmd = ps->GetCmd(this, 'SMAT');
            cmd->In()->SetS(this->refMaterial.getname());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
