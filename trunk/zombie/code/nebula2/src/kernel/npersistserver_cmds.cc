//-------------------------------------------------------------------
//  npersistserver_cmds.cc
//  (C) 2002 RadonLabs GmbH
//-------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/npersistserver.h"

static void n_setsavelevel(void *, nCmd *);
static void n_getsavelevel(void *, nCmd *);
static void n_setsaverclass(void*, nCmd*);
static void n_getsaverclass(void*, nCmd*);
static void n_setdependencyclass(void*, nCmd*);
static void n_getdependencyclass(void*, nCmd*);
static void n_setsavetype(void *, nCmd *);
static void n_getsavetype(void *, nCmd *);

//-------------------------------------------------------------------
/**
    @scriptclass
    npersistserver

    @cppclass
    nPersistServer

    @superclass
    nroot

    @classinfo
    The nPersistServer object can be found under /sys/servers/persist 
    and offers some commands to influence the way how objects 
    are saved. All load/save operations of objects run finally through 
    the persis server. That's all you need to know about it... 
*/
void n_initcmds_nPersistServer(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsavelevel_i",          'SSLV', n_setsavelevel);
    cl->AddCmd("i_getsavelevel_v",          'GSLV', n_getsavelevel);
    cl->AddCmd("v_setsaverclass_s",         'SSCL', n_setsaverclass);
    cl->AddCmd("s_getsaverclass_v",         'GSCL', n_getsaverclass);
    cl->AddCmd("v_setdependencyclass_sss",  'SDCL', n_setdependencyclass);
    cl->AddCmd("s_getdependencyclass_ss",   'GDCL', n_getdependencyclass);
    cl->AddCmd("v_setsavetype_i",           'SSTY', n_setsavetype);
    cl->AddCmd("i_getsavetype_v",           'GSTY', n_getsavetype);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setsavelevel

    @input
    i (SaveLevel)

    @output
    v

    @info
    Set the save level which filters commands to be saved.
    The default save level of 0 will let all commands through,
    a save level of 1 will only let commands with a level of
    1 or greater through. 

    This is useful for selectively saving a set of object attributes
    without having to write several save routines (savegames are
    an example).
*/
static void n_setsavelevel(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    self->SetSaveLevel(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    getsavelevel

    @input
    v

    @output
    i (SaveLevel)

    @info
    Get the current save level.
*/
static void n_getsavelevel(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    cmd->Out()->SetI(self->GetSaveLevel());
}

//-------------------------------------------------------------------
/**
    @cmd
    setsaverclass

    @input
    s(SaverClass)

    @output
    v

    @info
    Set name of nScriptServer derived class which should be
    used for saving objects.
*/
static void n_setsaverclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    self->SetSaverClass(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getsaverclass

    @input
    v

    @output
    s(SaverClass)

    @info
    Get class name of saver nScriptServer.
*/
static void n_getsaverclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    cmd->Out()->SetS(self->GetSaverClass().Get());
}

//-------------------------------------------------------------------
/**
*/
static void n_setdependencyclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    nString objclass(cmd->In()->GetS());
    nString objcmd(cmd->In()->GetS());
    nString depclass(cmd->In()->GetS());
    self->SetDependencyClass(objclass, objcmd, depclass);
}

//-------------------------------------------------------------------
/**
*/
static void n_getdependencyclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    nString objclass(cmd->In()->GetS());
    nString objcmd(cmd->In()->GetS());
    cmd->Out()->SetS(self->GetDependencyClass(objclass, objcmd).Get());
}

//-------------------------------------------------------------------
/**
    @cmd
    setsavetype

    @input
    i (SaveTYpe)

    @output
    v

    @info
    Set the save type which specifies the type of information to 
    save. This might be consulted checked by the SaveCmds in order
    to know what kind of commands to save.
*/
static void n_setsavetype(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    self->SetSaveType(nPersistServer::nSaveType(cmd->In()->GetI()));
}

//-------------------------------------------------------------------
/**
    @cmd
    getsavetype

    @input
    v

    @output
    i (SaveType)

    @info
    Get the current save type.
*/
static void n_getsavetype(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    cmd->Out()->SetI(self->GetSaveType());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
