#ifndef N_UNDO_COMMAND_SCRIPTED_H
#define N_UNDO_COMMAND_SCRIPTED_H
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "nundo/undocmd.h"

//------------------------------------------------------------------------------
/**
    @file undoCmdscripted.h
    @class UndoCmd
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief UndoCmd subclass for scripted commands.
    The command consists of a list of scripted commands.

    (C) 2004 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class UndoCmdScripted : public UndoCmd
{
public:

    // constructor
    UndoCmdScripted();
    
    // destructor
    virtual ~UndoCmdScripted();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

    /// Add a scripted command to the 'do' list
    void AddDoCommand( nRef<nRoot>& receiver, nCmd* cmd );

    /// Add a scripted command to the 'undo' list
    void AddUndoCmd( nRef<nRoot>& receiver, nCmd* cmd );

protected:
private:

    struct cmdSlot
    {
        nRef<nRoot> receiver;
        nCmd* command;
    };
    nArray<cmdSlot> undoList;
    nArray<cmdSlot> doList;

};
//------------------------------------------------------------------------------

#endif