/*-----------------------------------------------------------------------------
    @file nfloatmap_cmds.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nFloatMap persistence and scripting

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"

//------------------------------------------------------------------------------
static void n_setsize(void * o, nCmd * cmd);
static void n_fillheight(void * o, nCmd * cmd);
static void n_setgridscale(void * o, nCmd * cmd);
static void n_getgridscale(void * o, nCmd * cmd);
static void n_setheightscale(void * o, nCmd * cmd);
static void n_setheightoffset(void * o, nCmd * cmd);
static void n_getextent(void * o, nCmd * cmd);

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
void
n_initcmds_nFloatMap(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsize_i", 'SESI', n_setsize);
    cl->AddCmd("v_fillheight_f", 'FIHE', n_fillheight);
    cl->AddCmd("v_setgridscale_f", 'SEGS', n_setgridscale);
    cl->AddCmd("f_getgridscale_v", 'GEGS', n_getgridscale);
    cl->AddCmd("v_setheightscale_f", 'SEHS', n_setheightscale);
    cl->AddCmd("v_setheightoffset_f", 'SEHO', n_setheightoffset);
    cl->AddCmd("f_getextent_v", 'FHGE', n_getextent);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setsize(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    self->SetSize( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_fillheight(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    self->FillHeight( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setgridscale(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    self->SetGridScale( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_getgridscale(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    cmd->Out()->SetF( self->GetGridScale() );
}
//------------------------------------------------------------------------------
/**
*/
static
void
n_getextent(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    cmd->Out()->SetF( self->GetExtent() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setheightscale(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    self->SetHeightScale( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
*/
static
void
n_setheightoffset(void * o, nCmd * cmd)
{
    nFloatMap * self = static_cast<nFloatMap *> (o);
    self->SetHeightOffset( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------