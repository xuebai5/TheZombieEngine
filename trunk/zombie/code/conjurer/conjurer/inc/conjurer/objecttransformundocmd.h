#ifndef N_OBJECTTRANSFORM_UNDO_CMDS_H
#define N_OBJECTTRANSFORM_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nobjecttransformundocmd.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for object transform undo cmds

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
class ObjectTransformUndoCmd: public UndoCmd
{
public:

    /// Constructor
    ObjectTransformUndoCmd( nArray<InguiObjectTransform>* objects );

    /// Destructor
    virtual ~ObjectTransformUndoCmd();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

protected:

    struct ObjTransformUndoInfo {
        // Entity id affected by the transform
        nEntityObjectId entityId;

        // Transforms for undo and redo
        transform44 undoTransform, redoTransform;

        // Entity was dirty before this command?
        bool entityWasDirty;

        // Tells if entity was selected by user, or it was contained in an indoor (to restore selection)
        bool selectedByUser;

    };

    bool firstExecution;

    // Array of undo info for affected objects
    nArray<ObjTransformUndoInfo> undoInfo;

private:
};

//------------------------------------------------------------------------------

#endif

