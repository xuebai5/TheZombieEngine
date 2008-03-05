#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//  @author Miquel Angel Rujula <>
//------------------------------------------------------------------------------
#include "nspatial/nspatialserver.h"
#include "kernel/npersistserver.h"

static void n_insertentityflags(void* slf, nCmd* cmd);
static void n_connectindoors(void* slf, nCmd* cmd);
static void n_adjustquadtreebboxes(void* slf, nCmd* cmd);
static void n_setdohorizon(void* slf, nCmd* cmd);
static void n_isdoinghorizon(void* slf, nCmd* cmd);
#ifndef NGAME
static void n_seteditindoormode(void* slf, nCmd* cmd);
static void n_isineditindoormode(void* slf, nCmd* cmd);
static void n_toggleeditindoor(void* slf, nCmd* cmd);
static void n_setexecutionmode(void* slf, nCmd* cmd);
static void n_getexecutionmode(void* slf, nCmd* cmd);
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    @scriptclass
    nspatialserver

    @cppclass
    nSpatialServer

    @superclass
    nroot

    @classinfo
    nSpatialServer manages all the spatial information.
*/
void
n_initcmds_nSpatialServer(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_insertentityflags_oi", 'RIEF', n_insertentityflags);
    cl->AddCmd("v_connectindoors_v", 'R_CI', n_connectindoors);
    cl->AddCmd("v_adjustquadtreebboxes_f", 'RAQB', n_adjustquadtreebboxes);
    cl->AddCmd("v_setdohorizon_b", 'RSDH', n_setdohorizon);
    cl->AddCmd("b_isdoinghorizon_v", 'RIDH', n_isdoinghorizon);
#ifndef NGAME
    cl->AddCmd("v_seteditindoormode_b", 'RSEI', n_seteditindoormode);
    cl->AddCmd("b_isineditindoormode_v", 'RIEI', n_isineditindoormode);
    cl->AddCmd("v_toggleeditindoor_v", 'RTEI', n_toggleeditindoor);
    cl->AddCmd("v_setexecutionmode_i", 'RSEM', n_setexecutionmode);
    cl->AddCmd("i_getexecutionmode_v", 'RGEM', n_getexecutionmode);
#endif // !NGAME
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    insertentityflags
    @input
    o (entity)
    i (insertion flags)
    @output
    b (result)
    @info
    Insert an entity into the spatial system using the given flags
*/
static void
n_insertentityflags(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    nEntityObject *entity = static_cast<nEntityObject*>(cmd->In()->GetO());
    int flags = cmd->In()->GetI();
    
    cmd->Out()->SetB(self->InsertEntity(entity, flags));   
}

//------------------------------------------------------------------------------
/**
    @cmd
    connectindoors
    @input
    v
    @output
    v
    @info connect indoors that have a common portal
*/
static void 
n_connectindoors(void* slf, nCmd* /*cmd*/)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->ConnectIndoors();
}

//------------------------------------------------------------------------------
/**
    @cmd
    adjustquadtreebboxes
    @input
    v
    @output
    v
    @info adjust quadtree cells' bounding boxes with the given offset
*/
static void 
n_adjustquadtreebboxes(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->AdjustQuadtreeBBoxes(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    dohorizon
    @input
    b(flag)
    @output
    v
    @info enable or disable horizon culling
*/
static void 
n_setdohorizon(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->SetDoHorizonCulling(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isdoinghorizon
    @input
    v
    @output
    b (flag)
    @info get if it's doing horizon culling or not
*/
static void
n_isdoinghorizon(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    cmd->Out()->SetB(self->IsDoingHorizonCulling()); 
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @cmd
    seteditindoormode
    @input
    b (flag)
    @output
    v
    @info
    Set true or false the use edit indoor mode flag
*/
static void
n_seteditindoormode(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->SetEditIndoorMode(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isineditindoormode
    @input
    v
    @output
    b (flag)
    @info
    Says if it's in edit indoor mode
*/
static void
n_isineditindoormode(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    cmd->Out()->SetB(self->IsInEditIndoorMode()); 
}

//------------------------------------------------------------------------------
/**
    @cmd
    toggleeditindoor
    @input
    v
    @output
    v
    @info
    Toggles the indoor edition mode on/off
*/
static void
n_toggleeditindoor(void* slf, nCmd* /*cmd*/)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->ToggleEditIndoor();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setexecutionmode
    @input
    i (executionMode)
    @output
    v
    @info
    Set the spatial execution mode
*/
static void
n_setexecutionmode(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    self->SetExecutionMode(static_cast<nSpatialServer::ExecutionMode>(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getexecutionmode
    @input
    i (executionMode)
    @output
    v
    @info
    Get the spatial execution mode
*/
static void
n_getexecutionmode(void* slf, nCmd* cmd)
{
    nSpatialServer* self = static_cast<nSpatialServer*>(slf);
    cmd->Out()->SetI(self->GetExecutionMode()); 
}

#endif // !NGAME
