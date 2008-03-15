//------------------------------------------------------------------------------
//  nscriptserver_cmds.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nscriptserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nloghandler.h"

//------------------------------------------------------------------------------

static void n_run(void *, nCmd *);
static void n_txtoutput(void *, nCmd *);
static void n_runscript(void *, nCmd *);

//-------------------------------------------------------------------
/**
    @scriptclass
    nscriptserver

    @cppclass
    nScriptServer

    @superclass
    nRoot

    @classinfo
*/
void n_initcmds_nScriptServer(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_run_s",   'RSCR', n_run);
    cl->AddCmd("s_txtoutput_v",   'TXTO', n_txtoutput);
    cl->AddCmd("s_runscript_s",   'RSCF', n_runscript);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    run

    @input
    s (Code)

    @output
    s (Message)

    @info
    Runs lines of code.
*/
static void n_run(void *o, nCmd *cmd)
{
    nScriptServer *self = static_cast<nScriptServer *>(o);
    
    nString message;

    if( self->Run(cmd->In()->GetS(),message) )
        cmd->Out()->SetS("");
    else
        cmd->Out()->SetS(message.Get());
}

//-------------------------------------------------------------------
/**
    @cmd
    txtoutput

    @input
    v

    @output
    s (Message)

    @info
    Retrieves the last txt output of a command.
*/
static void n_txtoutput(void *o, nCmd *cmd)
{
    nScriptServer* thisScript(static_cast<nScriptServer*>(o));

    nString output,alloutput;

    while( thisScript->FlushLastOutput( output ) )
    {
        alloutput += output;
        alloutput += "\n";
    }

    cmd->Out()->SetS( alloutput.Get() );
}

//-------------------------------------------------------------------
/**
    @cmd
    runscript

    @input
    s (file)

    @output
    s (Message)

    @info
    Runs a script file.
*/
static void n_runscript(void *o, nCmd *cmd)
{
    nScriptServer *self = static_cast<nScriptServer *>(o);
    
    nString message;

    if( self->RunScript(cmd->In()->GetS(),message) )
        cmd->Out()->SetS("");
    else
        cmd->Out()->SetS(message.Get());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
