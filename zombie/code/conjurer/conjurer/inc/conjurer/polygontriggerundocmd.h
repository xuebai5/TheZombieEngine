#ifndef N_POLYGONTRIGGER_UNDO_CMDS_H
#define N_POLYGONTRIGGER_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file PolygonTriggerUndoCmd.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for polygon trigger editing undo cmds

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
class PolygonTriggerUndoCmd: public UndoCmd
{
public:

    /// Constructor
    PolygonTriggerUndoCmd( nEntityObject* polygonTrigger, nArray<vector3>* prevPoints, nArray<vector3>* newPoints );

    /// Destructor
    virtual ~PolygonTriggerUndoCmd();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

protected:

    nRefEntityObject refPolygonTrigger;

    /// Array of points for undo
    nArray<vector3> prevPoints;

    /// Array of points for redo
    nArray<vector3> newPoints;
    
    bool entityWasDirty;

private:
};

//------------------------------------------------------------------------------

#endif

