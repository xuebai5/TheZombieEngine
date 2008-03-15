#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterrainundocmd_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/terrainundocmd.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "resource/nresourceserver.h"
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------

const char* TerrainUndoPath = "/editor/terrain/undo/";

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
TerrainUndoCmd::TerrainUndoCmd( nFloatMap* oldHMap, nFloatMap* newHMap, int x0, int z0, int areaSize )
{
    if ( ! ( newHMap && oldHMap && oldHMap->GetSize() == newHMap->GetSize() && areaSize > 0 ) )
    {
        this->valid = false;
        return;
    }

    this->valid = true;
    this->firstExecution = true;

    this->newHMap = newHMap;
    this->oldHMap = oldHMap;
    this->x0 = x0;
    this->z0 = z0;
    this->areaSize = areaSize;

    nString name = "TerrainGeom";
    name.Append( ( nUndoServer::Instance())->GetSequentialName() );
    undoFloatMap = static_cast<nFloatMap*>( nResourceServer::Instance()->NewResource( "nfloatmap", name.Get(), nResource::Other ) );
    if ( !undoFloatMap )
    {
        this->valid = false;
        return;
    }
    undoFloatMap->SetSize( areaSize );
    undoFloatMap->SetUsage( nFloatMap::CreateEmpty );
    undoFloatMap->Load();

    name = "TerrainGeom";
    name.Append( ( nUndoServer::Instance())->GetSequentialName() );
    redoFloatMap = static_cast<nFloatMap*>( nResourceServer::Instance()->NewResource( "nfloatmap", name.Get(), nResource::Other ) );
    if ( !redoFloatMap )
    {
        this->valid = false;
        return;
    }
    redoFloatMap->SetSize( areaSize );
    redoFloatMap->SetUsage( nFloatMap::CreateEmpty );
    redoFloatMap->Load();

    nFloatMapBuilder fmBuilder;

    // Copy data from old floatmap to undo floatmap
    fmBuilder.Copy(oldHMap, -x0, -z0, undoFloatMap);
    // Copy data from new floatmap to redo floatmap
    fmBuilder.Copy(newHMap, -x0, -z0, redoFloatMap);

}
//------------------------------------------------------------------------------
/**
    Destructor
*/
TerrainUndoCmd::~TerrainUndoCmd()
{
    redoFloatMap->Release();
    undoFloatMap->Release();
}

//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool TerrainUndoCmd::Execute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    nFloatMapBuilder fmBuilder;

    // Don't execute the command when it's created, because it is executed before creation
    if ( firstExecution )
    {
        // Except for the copy in the 'old' buffer
        fmBuilder.Copy( redoFloatMap, x0, z0, oldHMap );

        firstExecution = false;
        return true;
    }

    fmBuilder.Copy( redoFloatMap, x0, z0, oldHMap );
    fmBuilder.Copy( redoFloatMap, x0, z0, newHMap );

    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool TerrainUndoCmd::Unexecute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    firstExecution = false;

    nFloatMapBuilder fmBuilder;
    fmBuilder.Copy( undoFloatMap, x0, z0, oldHMap );
    fmBuilder.Copy( undoFloatMap, x0, z0, newHMap );

    return true;
}
//------------------------------------------------------------------------------
/**
    Get byte size
*/
int TerrainUndoCmd::GetSize( void )
{
    n_assert( undoFloatMap );
    return 2 * undoFloatMap->GetSize() * undoFloatMap->GetSize() * sizeof( float );
}
//------------------------------------------------------------------------------