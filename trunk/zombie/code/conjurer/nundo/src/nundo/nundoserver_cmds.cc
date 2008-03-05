#include "precompiled/pchnundoserver.h"
/*-----------------------------------------------------------------------------
    @file nundoserver_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nUndoServer persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "nundo/undocmdbyscript.h"
#include "nundo/nundoserver.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------

static void n_newcommand(void * o, nCmd * cmd);
static void n_undo(void * o, nCmd * cmd);
static void n_redo(void * o, nCmd * cmd);

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nUndoServer)

    cl->AddCmd("v_newcommand_ss", 'NWCM', n_newcommand );
    cl->AddCmd("v_undo_v", 'UNDO', n_undo );
    cl->AddCmd("v_redo_v", 'REDO', n_redo );
    
    NSCRIPT_ADDCMD('GMMU', int, GetMemoryUsed, 0, (), 0, ());
    NSCRIPT_ADDCMD('GMML', int, GetMemoryLimit, 0, (), 0, ());
    NSCRIPT_ADDCMD('SMML', void, SetMemoryLimit, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CLIN', void, Clean, 0, (), 0, ());


    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL( EntityModified );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()



//-----------------------------------------------------------------------------
/**
    @cmd
    newcommand
    @input
    o (NewCommand)
    @output
    v
    @info
    Adds a new command by scripting.
*/
static void
n_newcommand(void* slf, nCmd* cmd)
{
    nUndoServer* self = static_cast<nUndoServer*>(slf);

    nString undo(cmd->In()->GetS());
    nString redo(cmd->In()->GetS());

    self->NewCommand( n_new( UndoCmdByScript( undo,redo) ) );
}

//-----------------------------------------------------------------------------
/**
    @cmd
    undo
    @input
    v
    @output
    v
    @info
    Performs an undo.
*/
static void
n_undo(void* slf, nCmd* /*cmd*/)
{
    nUndoServer* self = static_cast<nUndoServer*>(slf);

    self->Undo();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    redo
    @input
    v
    @output
    v
    @info
    Performs an undo.
*/
static void
n_redo(void* slf, nCmd* /*cmd*/)
{
    nUndoServer* self = static_cast<nUndoServer*>(slf);

    self->Redo();
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nUndoServer::SaveCmds(nPersistServer * ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
