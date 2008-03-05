#ifndef N_TERRAIN_UNDO_CMDS_H
#define N_TERRAIN_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nterrain_undocmds.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Superclass for terrain undo commands in the terrain application state

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/nterrainline.h"
#include "mathlib/rectangle.h" 

//------------------------------------------------------------------------------

/// Path of terrain related undo data objects
extern const char* TerrainUndoPath;

//------------------------------------------------------------------------------
class TerrainUndoCmd: public UndoCmd
{
public:

    struct IntRectangle
    {
        int x0, z0, x1, z1;

        void Set(int x0, int z0, int x1, int z1)
        {
            this->x0 = x0;
            this->z0 = z0;
            this->x1 = x1;
            this->z1 = z1;
        }
    };

    /// Constructor
    TerrainUndoCmd( nFloatMap* oldHMap, nFloatMap* newHMap,  int x0, int z0, int areaSize );

    /// Destructor
    virtual ~TerrainUndoCmd();

    /// Execute
    virtual bool Execute( void ) = 0;
    
    /// Unexecute
    virtual bool Unexecute( void ) = 0;

    /// Get byte size
    virtual int GetSize( void );

protected:

    bool firstExecution;

    // Reference to the main floatmap and the buffered one
    nRef<nFloatMap> oldHMap, newHMap;

    // Position of the area covered by the command
    int x0, z0;

    // Size of the squared area
    int areaSize;

    // Floatmap that contains the redo data
    nRef<nFloatMap> redoFloatMap;

    // Floatmap that contains the undo data
    nRef<nFloatMap> undoFloatMap;
private:
};

//------------------------------------------------------------------------------

#endif

