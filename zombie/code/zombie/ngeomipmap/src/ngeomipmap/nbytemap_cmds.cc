#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  nbytemap_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngeomipmap/nbytemap.h"

//------------------------------------------------------------------------------
static void n_setsize(void * o, nCmd * cmd);
static void n_fillvalue(void * o, nCmd * cmd);

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
void
n_initcmds_nByteMap(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsize_i", 'SESI', n_setsize);
    cl->AddCmd("v_fillvalue_i", 'FIHE', n_fillvalue);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setsize(void * o, nCmd * cmd)
{
    nByteMap * self = static_cast<nByteMap *> (o);
    self->SetSize( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_fillvalue(void * o, nCmd * cmd)
{
    nByteMap * self = static_cast<nByteMap *> (o);
    self->FillValue( nuint8(cmd->In()->GetI()) );
}

//------------------------------------------------------------------------------
