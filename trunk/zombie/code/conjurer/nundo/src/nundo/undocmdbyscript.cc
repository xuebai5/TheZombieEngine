#include "precompiled/pchnundoserver.h"
//------------------------------------------------------------------------------
//  nundocmdbyscript.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "nundo/undocmdbyscript.h"

//------------------------------------------------------------------------------

nRef<nScriptServer> UndoCmdByScript::refScriptServer;

//------------------------------------------------------------------------------
/**
    Constructor
*/
UndoCmdByScript::UndoCmdByScript( const nString& redo, const nString& undo ) :
    executeCommands(redo),
    unexecuteCommands(undo)
{
    if( !this->refScriptServer.isvalid() )
    {
        n_assert2_always( "Hasn't been set a script server" );
        return;
    }
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
UndoCmdByScript::~UndoCmdByScript()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Execute command
*/
bool UndoCmdByScript::Execute()
{
    nString result;

    if( !this->refScriptServer->Run( this->executeCommands.Get(), result ) )
    {
        n_error( "Error doing script: %s\n%s", 
            this->executeCommands.Get(), 
            result.Get() );
        return false;
    }
    
    return true;

}

//------------------------------------------------------------------------------
/**
    Undo executed command
*/
bool UndoCmdByScript::Unexecute()
{
    nString result;

    if( !this->refScriptServer->Run( this->unexecuteCommands.Get(), result ) )
    {
        n_error( "Error undoing script: %s\n%s", 
            this->unexecuteCommands.Get(),
            result.Get() );
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Set the Execute command
*/
void UndoCmdByScript::SetExecute( const nString& commands )
{
    this->executeCommands = commands;
}

//------------------------------------------------------------------------------
/**
    Set the unde executed command
*/
void UndoCmdByScript::SetUnexecute( const nString& commands )
{
    this->unexecuteCommands = commands;
}

//------------------------------------------------------------------------------
/**
    Get size of command
*/
int
UndoCmdByScript::GetSize( void )
{
    return executeCommands.Length() + unexecuteCommands.Length() + 2;
}

//------------------------------------------------------------------------------
/**
    Sets the scripting server to use
*/
void UndoCmdByScript::SetScriptServer( nScriptServer* server )
{
    n_assert2( server, "Null pointer, server no valid" );

    refScriptServer = server;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
