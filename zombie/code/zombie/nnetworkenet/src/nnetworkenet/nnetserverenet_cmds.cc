//------------------------------------------------------------------------------
//  nnetserverenet_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkenet/nnetserverenet.h"

//------------------------------------------------------------------------------
static void n_setserverport(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnetserverenet
    @cppclass
    nNetServerEnet
    @superclass
    nroot
    @classinfo
    The zombie network server object.
*/
void
n_initcmds_nNetServerEnet(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setserverport_s",     'SSPT', n_setserverport);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setserverport
    @input
    s(port)
    @output
    v
    @info
    Set the name of the server port.
*/
static void
n_setserverport(void* slf, nCmd* cmd)
{
    nNetServerEnet* self = (nNetServerEnet*) slf;
    const char* port = cmd->In()->GetS();
    self->SetServerPort(port);
}

//------------------------------------------------------------------------------
/**
    @cmd
    open
    @input
    v
    @output
    b(Success)
    @info
    Open the server.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nNetServerEnet* self = (nNetServerEnet*) slf;
    cmd->Out()->SetB(self->Open());
}

//------------------------------------------------------------------------------
/**
    @cmd
    close
    @input
    v
    @output
    v
    @info
    Close the server.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nNetServerEnet* self = (nNetServerEnet*) slf;
    self->Close();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isopen
    @input
    v
    @output
    b(OpenFlag)
    @info
    Return the open status of the server.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nNetServerEnet* self = (nNetServerEnet*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
