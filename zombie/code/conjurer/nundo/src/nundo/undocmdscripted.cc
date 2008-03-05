#include "precompiled/pchnundoserver.h"
//------------------------------------------------------------------------------
//  UndoCmd.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "nundo/undocmdscripted.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
UndoCmdScripted::UndoCmdScripted()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
UndoCmdScripted::~UndoCmdScripted()
{
    for ( int i = 0; i < this->doList.Size(); i++ )
    {
        nCmdProto* proto = doList[ i ].command->GetProto();
        proto->RelCmd( doList[ i ].command );
    }

    for ( int i = 0; i < this->undoList.Size(); i++ )
    {
        nCmdProto* proto = undoList[ i ].command->GetProto();
        proto->RelCmd( undoList[ i ].command );
    }
};

//------------------------------------------------------------------------------
/**
    Execute
*/
bool
UndoCmdScripted::Execute( void )
{
    for ( int i = 0; i < this->doList.Size(); i++ )
    {
        nCmdProto* proto = doList[i].command->GetProto();
        if ( ! proto->Dispatch( doList[i].receiver, doList[i].command ) )
        {
            return false;
        }
    }
    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
*/
bool
UndoCmdScripted::Unexecute( void )
{
    for ( int i = 0; i < this->undoList.Size(); i++ )
    {
        nCmdProto* proto = undoList[i].command->GetProto();
        if ( ! proto->Dispatch( undoList[i].receiver, undoList[i].command ) )
        {
            return false;
        }
    }
    return true;
}
//------------------------------------------------------------------------------
/**
    Get byte size
    @return byte size of the command
*/
int
UndoCmdScripted::GetSize( void )
{
    return sizeof( this ) + ( doList.Size() + undoList.Size() ) * sizeof( nCmd );
}
//------------------------------------------------------------------------------
/**
    Add a scripted command to the 'do' list
    @param receiver The object receiving the command dispatch
    @param cmd The scripted command
*/
void
UndoCmdScripted::AddDoCommand( nRef<nRoot>& receiver, nCmd* cmd )
{
    n_assert( receiver.isvalid() );
    n_assert( ! this->IsLinked() );
    cmdSlot c;
    c.command = cmd;
    c.receiver = receiver;
    this->doList.Append( c );
}
//------------------------------------------------------------------------------
/**
    Add a scripted command to the 'undo' list
*/
void
UndoCmdScripted::AddUndoCmd( nRef<nRoot>& receiver, nCmd* cmd )
{
    n_assert( receiver.isvalid() );
    n_assert( ! this->IsLinked() );
    cmdSlot c;
    c.command = cmd;
    c.receiver = receiver;
    this->undoList.Append( c );
}
//------------------------------------------------------------------------------
