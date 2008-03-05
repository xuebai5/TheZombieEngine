//------------------------------------------------------------------------------
//  nnetclientenet_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkenet/nnetclientenet.h"

//------------------------------------------------------------------------------
static void n_setserverport(void* slf, nCmd* cmd);
static void n_setserverhost(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_connect(void* slf, nCmd* cmd);
static void n_disconnect(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_clientstate(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_sendmessage(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnetclientenet
    @cppclass
    nNetClientEnet
    @superclass
    nroot
    @classinfo
    The zombie network client object.
*/
void
n_initcmds_nNetClientEnet(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setserverport_s",     'SSPT', n_setserverport);
    cl->AddCmd("v_setserverhost_s",     'SSHT', n_setserverhost);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_connect_v",           'CONT', n_connect);
    cl->AddCmd("v_disconnect_v",        'DCNT', n_disconnect);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("s_clientstate_v",       'CLST', n_clientstate);
    cl->AddCmd("v_sendmessage_sii",     'SNDM', n_sendmessage);
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
    nNetClientEnet* self = (nNetClientEnet*) slf;
    const char* port = cmd->In()->GetS();
    self->SetServerPort(port);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setserverhost
    @input
    s(host)
    @output
    v
    @info
    Set the localitation of the server.
*/
static void
n_setserverhost(void* slf, nCmd* cmd)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    const char* host = cmd->In()->GetS();
    self->SetServerHost(host);
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
    Open the client.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    cmd->Out()->SetB(self->Open());
}

//------------------------------------------------------------------------------
/**
    @cmd
    connect
    @input
    v
    @output
    v
    @info
    Connect with the server.
*/
static void n_connect(void* slf, nCmd* /*cmd*/)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    self->Connect();
}

//------------------------------------------------------------------------------
/**
    @cmd
    disconnect
    @input
    v
    @output
    v
    @info
    Disconnect of server.
*/
static void n_disconnect(void* slf, nCmd* /*cmd*/)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    self->Disconnect();
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
    Close the client.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
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
    Return the open status of the client.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    clientstate
    @input
    v
    @output
    b(state)
    @info
    Return the states of the client.
*/
static void
n_clientstate(void* slf, nCmd* cmd)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    nNetClientEnet::State state = self->ClientState();
    switch( state )
    {
    case nNetClientEnet::Invalid:
        cmd->Out()->SetS("Invalid");
        break;
    case nNetClientEnet::Create:
        cmd->Out()->SetS("Create");
        break;
    case nNetClientEnet::Connecting:
        cmd->Out()->SetS("Connecting");
        break;
    case nNetClientEnet::Connected:
        cmd->Out()->SetS("Connected");
        break;
    case nNetClientEnet::Closing:
        cmd->Out()->SetS("Closing");
        break;
    default:
        n_assert2_always( "Invalid Client State");
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    sendmessage
    @output
    s(buffer)
    i(size)
    i(type)
    @input
    v
    @info
    send a message to the server.
*/
static void 
n_sendmessage(void* slf, nCmd* cmd)
{
    nNetClientEnet* self = (nNetClientEnet*) slf;
    const char* buffer = cmd->In()->GetS();
    int size = cmd->In()->GetI();
    int type = cmd->In()->GetI();
    self->SendMessage( buffer, size, (nNetUtils::MessageType)type );
}

//------------------------------------------------------------------------------
