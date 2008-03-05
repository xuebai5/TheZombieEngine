#ifndef N_TERRAIN_HOLE_UNDO_CMDS_H
#define N_TERRAIN_HOLE_UNDO_CMDS_H
//------------------------------------------------------------------------------
/**
    @file nterrainholeundocmd.h
    
    @ingroup NebulaConjurerEditor

    @author Dominic Ashby

    @brief Class for terrain hole building undo commands in the terrain application state

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nundo/undocmd.h"
#include "nundo/nundoserver.h"

//------------------------------------------------------------------------------

class TerrainHoleUndoCmd: public UndoCmd
{
public:

    /// Constructor
    TerrainHoleUndoCmd( nTerrainLine * terrainLine );

    /// Destructor
    virtual ~TerrainHoleUndoCmd();

    /// Execute
    virtual bool Execute( void );
    
    /// Unexecute
    virtual bool Unexecute( void );

    /// Get byte size
    virtual int GetSize( void );

protected:

    /// line used to create the hole
    nTerrainLine * definingLine;

    /// hole added
    nTerrainLine * holeCreated;
private:

    /// invalidate the hole cache for the given outdoor
    void InvalidateHoleCache( nEntityObject * outdoor );

};

//------------------------------------------------------------------------------

#endif

