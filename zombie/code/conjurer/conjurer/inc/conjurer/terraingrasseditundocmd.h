#ifndef N_TERRAINGRASSEDIT_UNDO_CMDS_H
#define N_TERRAINGRASSEDIT_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nterraingrassedit_undocmds.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for grass edit undo commands in the terrain application state

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"
#include "conjurer/terrainundocmd.h"
#include "ngeomipmap/nbytemap.h"
#include "ngeomipmap/nterrainline.h"
#include "ngeomipmap/nterrainmaterial.h"

//------------------------------------------------------------------------------

/// Path of terrain related undo data objects
extern const char* TerrainUndoPath;

class nByteMap;

//------------------------------------------------------------------------------
class TerrainGrassEditUndoCmd: public UndoCmd
{
public:

    /// Constructor
    TerrainGrassEditUndoCmd( nByteMap* oldHMap, nByteMap* newHMap,  int bx0, int bz0 );

    /// Destructor
    virtual ~TerrainGrassEditUndoCmd();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

protected:
private:
    // Refresh terrain
    bool RefreshTerrain();

    // First execution
    bool firstExecution;

    // Reference to the main floatmap and the buffered one
    nRef<nByteMap> oldHMap, newHMap;

    // Floatmap that contains the redo data
    nRef<nByteMap> redoByteMap;

    // Floatmap that contains the undo data
    nRef<nByteMap> undoByteMap;

    // Affected weightmap coords
    int weightMapX;
    int weightMapZ;
};

//------------------------------------------------------------------------------

#endif

