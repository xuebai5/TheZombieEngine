#ifndef N_UNDOCMDSCRIPT_H
#define N_UNDOCMDSCRIPT_H
//------------------------------------------------------------------------------
/**
    @file nundocmdscript.h
    @class nUndoCmdScript
    @ingroup NebulaConjurerEditor

    @brief Class that represents an undo command

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "nundo/undocmd.h"
#include "kernel/nscriptserver.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class UndoCmdByScript : public UndoCmd
{
public:
    /// constructor
    UndoCmdByScript( const nString& undo, const nString& redo );
    /// destructor
    ~UndoCmdByScript();

    /// execute
    bool Execute();
    
    /// unexecute
    bool Unexecute();

    /// set the Execute command
    void SetExecute( const nString& commands );

    /// set the Unexecute command
    void SetUnexecute( const nString& commands );

    /// Get byte size
    int GetSize( void );

    /// sets the scripting server to use
    static void SetScriptServer( nScriptServer* server );

private:
    /// script server
    static nRef<nScriptServer> refScriptServer;
    
    /// execute
    nString executeCommands;

    /// unexecute
    nString unexecuteCommands;

};
//------------------------------------------------------------------------------

#endif

