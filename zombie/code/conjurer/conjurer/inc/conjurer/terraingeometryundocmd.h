#ifndef N_TERRAINGEOMETRY_UNDO_CMDS_H
#define N_TERRAINGEOMETRY_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nterraingeometry_undocmds.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for geometry undo command int the terrain application state

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "conjurer/terrainundocmd.h"
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/nterrainline.h"
#include "mathlib/rectangle.h" 

//------------------------------------------------------------------------------

/// Path of terrain related undo data objects
extern const char* TerrainUndoPath;

//------------------------------------------------------------------------------
class TerrainGeometryUndoCmd: public TerrainUndoCmd
{
public:

    /// Constructor
    TerrainGeometryUndoCmd( nFloatMap* oldHMap, nFloatMap* newHMap,  int x0, int z0, int areaSize );

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

protected:
private:
};

//------------------------------------------------------------------------------

#endif

