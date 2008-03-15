#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterraingeometryundocmd_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/terraingeometryundocmd.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/ninguiterraintoolgeom.h"

//------------------------------------------------------------------------------
/**
    Constructor
    @param oldMap nFloatMap with the data previous to command execution
    @param newMap nFloatMap with the data after the command execution
    @param x0 x coordinate of the tool application point 
    @param z0 z coordinate of the tool application point 
    @param size size of the applied tool

    This command is created with a pointer to an old floatmap, another to a new one, and the position and size of a
    squared area that is supposed to have changed. It stores the info on both floatmaps creating two new floatmaps
    that cover the modified area. Undo and redo is achieved by simple copy of these smaller floatmaps, independent
    of the tool that was used in the command. Nevertheless, the label of the command should be set to a descriptive one.

*/
TerrainGeometryUndoCmd::TerrainGeometryUndoCmd( nFloatMap* oldHMap, nFloatMap* newHMap, int x0, int z0, int areaSize ):
    TerrainUndoCmd(oldHMap, newHMap, x0, z0, areaSize )
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool TerrainGeometryUndoCmd::Execute( void )
{

    bool ret = firstExecution;

    if ( ! TerrainUndoCmd::Execute() )
    {
        return false;
    }

    if ( ret )
    {
        return true;
    }

    nTerrainEditorState* ed = static_cast<nTerrainEditorState*>(nKernelServer::Instance()->Lookup("/app/conjurer/appstates/terrain"));
    n_assert(ed);
    nInguiTerrainToolGeom* tool = (nInguiTerrainToolGeom*)(ed->inguiTerrainTool[ 0 ]);
    tool->MakeRectangleDirtyGeometry( x0, z0, x0 + this->redoFloatMap->GetSize(), z0 + this->redoFloatMap->GetSize());

    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool TerrainGeometryUndoCmd::Unexecute( void )
{
    if ( ! TerrainUndoCmd::Unexecute() )
    {
        return false;
    }

    nTerrainEditorState* ed = static_cast<nTerrainEditorState*>(nKernelServer::Instance()->Lookup("/app/conjurer/appstates/terrain"));
    n_assert(ed);
    nInguiTerrainToolGeom* tool = (nInguiTerrainToolGeom*)(ed->inguiTerrainTool[ 0 ]);
    tool->MakeRectangleDirtyGeometry( x0, z0, x0 + this->redoFloatMap->GetSize(), z0 + this->redoFloatMap->GetSize());

    return true;
}
//------------------------------------------------------------------------------