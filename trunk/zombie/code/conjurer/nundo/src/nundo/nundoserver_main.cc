#include "precompiled/pchnundoserver.h"
//------------------------------------------------------------------------------
//  nundoserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "nundo/nundoserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nUndoServer, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nUndoServer, EntityModified );

//------------------------------------------------------------------------------
const char* undoServerPath = "/sys/servers/undo";

//------------------------------------------------------------------------------
/**
    Constructor
*/
nUndoServer::nUndoServer():
    currentCmd( 0 ),
    cmdCount( 0 ),
    totalSize( 0 ),
    sizeLimit( 20000000 ),
    seqNameInt( 0 )
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Destructor
*/
nUndoServer::~nUndoServer()
{
    this->Clean();
}

//------------------------------------------------------------------------------
/**
    Get singleton instance
    @return Singleton instance
*/
nUndoServer* nUndoServer::Instance( void )
{
    return (nUndoServer*) nKernelServer::Instance()->Lookup( undoServerPath );
}

//------------------------------------------------------------------------------
/**
    Execute a new command
    @param cmd The command to execute
    @return Success in command execution

    The responsibility of deleting the command is left to the undo manager.
*/
bool
nUndoServer::NewCommand( UndoCmd* cmd )
{
    n_assert ( cmd );
    if ( ! cmd->IsValid() )
    {
        delete cmd;
        return false;
    }

    if ( cmd->Execute() )
    {

        // Make a pass to remove references
        UndoCmd* c = this->currentCmd;
        if ( c != NULL )
        {
            c = (UndoCmd*) c->GetSucc();
        } else
        {
            c = (UndoCmd*) commandList.GetHead();
        }
        while ( c != NULL )
        {
            UndoCmd* cnext = (UndoCmd*)( c->GetSucc() );
            c->RemoveReferences();
            c = cnext;
        }

        // Delete undone commands before inserting the new command
        c = this->currentCmd;
        if ( c != NULL )
        {
            c = (UndoCmd*) c->GetSucc();
        } else
        {
            c = (UndoCmd*) commandList.GetHead();
        }
        while ( c != NULL )
        {
            UndoCmd* cnext = (UndoCmd*)( c->GetSucc() );
            RemoveCommand( c );
            c = cnext;
        }

        // Add the executed command to the undo/redo list
        commandList.AddTail( cmd );
        this->cmdCount ++;
        this->totalSize += cmd->GetSize();
        this->currentCmd = cmd;

        // Memory limit management
        bool deleteOldestCmd = ( ! cmd->chainRedo ) && ( totalSize  > sizeLimit );
        while ( deleteOldestCmd && this->cmdCount > 0 )
        {
            UndoCmd* node = (UndoCmd*) commandList.GetHead();
            deleteOldestCmd = node->chainRedo || totalSize  > sizeLimit;
            if ( this->currentCmd == node )
            {
                this->currentCmd = static_cast<UndoCmd*>( this->currentCmd->GetSucc() );
            }
            RemoveCommand( node );

        }

        return true;
    }
    else
    {
        // If the command fails to execute, don't add it
        delete cmd;
        return false;
    }
}
//------------------------------------------------------------------------------
/**
    Check if it's possible to undo
    @return True if it's possible to undo
*/
bool
nUndoServer::CanUndo( void )
{
    return currentCmd != NULL;
}
//------------------------------------------------------------------------------
/**
    Check if it's possible to redo
    @return True if it's possible to redo
*/
bool
nUndoServer::CanRedo( void )
{
    if ( currentCmd != NULL )
    {
        return ( currentCmd->GetSucc() != NULL );

    } else
    {
        return ( ! commandList.IsEmpty() );

    }
}
//------------------------------------------------------------------------------
/**
    Undo last done command
    @return True if there was a command to undo
*/
bool
nUndoServer::Undo( void )
{
    if ( currentCmd == NULL )
    {
        return false;
    }

    bool cont = true;
    while ( currentCmd && cont )
    {
        n_verify( currentCmd->Unexecute() );

        currentCmd->SetUndone( true );

        cont = currentCmd->chainUndo;
        
        currentCmd = (UndoCmd*) currentCmd->GetPred();
    }

    return true;
}  
//------------------------------------------------------------------------------
/**
    Redo last undone command
    @return True if there was a command to redo
*/
bool
nUndoServer::Redo( void )
{
    UndoCmd* c = currentCmd;
    if ( c != NULL )
    {
        c = (UndoCmd*)( c->GetSucc() );
    } else
    {
        c = (UndoCmd*) commandList.GetHead();
    }

    if ( c == NULL )
    {
        return false;
    }

    bool cont = true;
    while ( c && cont )
    {
        n_verify( c->Execute() );

        c->SetUndone( false );

        cont = c->chainRedo;        
        
        if ( cont )
        {
            c = (UndoCmd*)( c->GetSucc() );
        }
    }
    currentCmd = c;
    return true;

}
//------------------------------------------------------------------------------
/**
    Remove a command from the list
    @param cmd The command

    Note: Removing a command also calls its destructor.
*/
void
nUndoServer::RemoveCommand( UndoCmd* cmd )
{
    n_assert( cmd && cmd->IsLinked() );

    cmd->Remove();
    cmdCount --;
    totalSize -= cmd->GetSize();
    if ( totalSize < 0 )
    {
        totalSize = 0;
    }
    n_delete( cmd );
}
//------------------------------------------------------------------------------
/**
    Get a sequential name for undo objects
    @return A sequential name
*/
nString
nUndoServer::GetSequentialName( void )
{
    nString n("Undo");
    n.AppendInt( seqNameInt++ );
    return n;
}
//------------------------------------------------------------------------------
/**
    Get memory used to store the undo objects
    @return Memory used in bytes
*/
int
nUndoServer::GetMemoryUsed( void )
{
    return this->totalSize;
}
//------------------------------------------------------------------------------
/**
    Get memory limit to store the undo objects
    @return Memory limit in bytes
*/
int
nUndoServer::GetMemoryLimit( void )
{
    return this->sizeLimit;
}
//------------------------------------------------------------------------------
/**
    Set memory limit to store the undo objects
    @param size Memory limit in bytes
*/
void
nUndoServer::SetMemoryLimit( int size )
{
    this->sizeLimit = size;
}
//------------------------------------------------------------------------------
/**
    Delete all undo commands
*/
void
nUndoServer::Clean()
{
    // pass to remove references
    UndoCmd* cmd = (UndoCmd*) commandList.GetHead() ;
    while ( cmd )
    {
        cmd->RemoveReferences();
        cmd = (UndoCmd*) cmd->GetSucc() ;
    }

    // pass to delete commands
    while ( this->cmdCount )
    {
        this->RemoveCommand( (UndoCmd*) this->commandList.GetTail() );
    }
    this->currentCmd = NULL;
}
//------------------------------------------------------------------------------