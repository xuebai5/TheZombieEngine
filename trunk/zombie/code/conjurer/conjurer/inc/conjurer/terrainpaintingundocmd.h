#ifndef N_TERRAINPAINTING_UNDO_CMDS_H
#define N_TERRAINPAINTING_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nterrainpainting_undocmds.h
    
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class for painting undo command int the terrain application state

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
class TerrainPaintingUndoCmd: public UndoCmd
{
public:

    /// Constructor
    TerrainPaintingUndoCmd( nByteMap* oldHMap, nByteMap* newHMap,  int bx0, int bz0, nTerrainMaterial::LayerHandle layerHnd );

    /// Destructor
    virtual ~TerrainPaintingUndoCmd();

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

    // Layer affected
    nTerrainMaterial::LayerHandle layerHnd;
};

//------------------------------------------------------------------------------

#endif

