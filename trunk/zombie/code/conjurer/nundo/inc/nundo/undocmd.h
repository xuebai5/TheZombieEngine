#ifndef N_UNDO_COMMAND_H
#define N_UNDO_COMMAND_H
//------------------------------------------------------------------------------
/**
    @file UndoCmd.h
    @class UndoCmd
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class that encapsulates an undoable application user command

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "util/nnode.h"
//------------------------------------------------------------------------------
class UndoCmd: public nNode
{
public:

    // constructor
    UndoCmd();

    // destructor
    virtual ~UndoCmd()
    {
        // empty
    }

    /// Execute
    virtual bool Execute( void ) = 0;
    
    /// Unexecute
    virtual bool Unexecute( void ) = 0;

    /// Get byte size
    virtual int GetSize( void )
    {
        return sizeof( this );
    }

    /// Remove references to external objects
    virtual void RemoveReferences()
    {
        // empty
    }

    /// Set label
    void SetLabel( nString& lab )
    {
        label = lab;
    }

    /// Get label
    nString& GetLabel( void )
    {
        return label;
    }

    /// tell if the command was created succesfully or there was an error ( memory allocation etc)
    bool IsValid();

    // Flags for chain undo and redo. If they're true, the server continues to undoing or redoing.
    // Useful for making a undo cmd composed of others already existing undo cmds
    bool chainUndo, chainRedo;

    void SetUndone( bool ud );

protected:
    
    nString label;

    // valid flag
    bool valid;

    // True when command has been undone
    bool undone;

private:

};
//------------------------------------------------------------------------------
/**
    Constructor
*/
inline
UndoCmd::UndoCmd():
    valid( true ),
    undone( false ),
    chainUndo( false ),
    chainRedo( false )
{
    // empty
}
//------------------------------------------------------------------------------
/**
    @brief Returns valid flag
*/
inline
bool
UndoCmd::IsValid()
{
    return this->valid;
}
//------------------------------------------------------------------------------
/**
    @brief Sets undone flag
*/
inline
void
UndoCmd::SetUndone( bool ud )
{
    this->undone = ud;
}
//------------------------------------------------------------------------------
#endif
