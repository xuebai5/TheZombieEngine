#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  nresourceserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "resource/nresourceserver.h"

static void n_getnumresources(void* o, nCmd* cmd);
static void n_getresourcebytesize(void* o, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
void
n_initcmds_nResourceServer(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_getnumresources_i", 'GNBR', n_getnumresources );
    cl->AddCmd("i_getresourcebytesize_i", 'GRBS', n_getresourcebytesize );
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumresources
    @input
    i(resource type id)
    @output
    i(number of resources)
    @info
    Return the number of resources of the given type
*/
static void
n_getnumresources(void* slf, nCmd* cmd)
{
    nResourceServer* self = static_cast<nResourceServer*>(slf);

    nResource::Type type = static_cast<nResource::Type>( cmd->In()->GetI() );
    int nb = self->GetNumResources(type);
    cmd->Out()->SetI(nb);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getresourcebytesize
    @input
    i(resource type id)
    @output
    i(memory occupied in bytes)
    @info
    Return the memory that all the resources of the given type occupies
*/
static void
n_getresourcebytesize(void* slf, nCmd* cmd)
{
    nResourceServer* self = static_cast<nResourceServer*>(slf);

    nResource::Type type = static_cast<nResource::Type>( cmd->In()->GetI() );
    int bytes = self->GetResourceByteSize(type);
    cmd->Out()->SetI(bytes);
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
