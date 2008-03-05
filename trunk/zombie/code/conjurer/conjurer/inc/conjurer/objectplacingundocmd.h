#ifndef N_OBJECTPLACING_UNDO_CMDS_H
#define N_OBJECTPLACING_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nobjectplacingundocmd.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for object placing and deleting undo cmds. It doesn't delete the entites immediatly, it leaves them from the spaces
            and only removes entities when the undo cmd is destroyed and there are no mre references to the entity.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
class ObjectPlacingUndoCmd: public UndoCmd
{
public:

    /// Constructor with entity pointer
    ObjectPlacingUndoCmd( nEntityObject* object, const char * label, bool isDelete = false);
    
    /// Constructor with entity pointers
    ObjectPlacingUndoCmd( nArray<nEntityObject*>* objects, const char * label, bool isDelete = false);

    /// Constructor with nRefEntityObject's
    ObjectPlacingUndoCmd( nArray<nRefEntityObject>* objects, const char * label, bool isDelete = false);

    /// Constructor with entity ids
    ObjectPlacingUndoCmd( nArray<nEntityObjectId>* objects, const char * label, bool isDelete = false );

    /// Destructor
    virtual ~ObjectPlacingUndoCmd();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

    /// Remove references to external objects
    virtual void RemoveReferences();

    /// Insert objects in the world (normal and physic spaces)
    void InsertObjects();
    
    /// Remove objects from the world (normal and physic spaces)
    void RemoveObjects();

protected:

    struct ObjPlaceUndoInfo {
        // Entity id affected by the transform
        nEntityObjectId entityId;

        // Entity class name
        nString className;

        // Reference to entity
        nRef<nEntityObject> refEntity;

        // Transforms for undo and redo
        transform44 placedInstanceTransform;
    };

    // Add entity to the command
    void AddEntity(nEntityObject* object);

    /// First execution flag
    bool firstExecution;

    /// If this flag is true, the Do and Undo work inversed
    bool isDeleteCmd;

    // Array of undo info
    nArray<ObjPlaceUndoInfo> undoInfo;

private:
};

//------------------------------------------------------------------------------

#endif

