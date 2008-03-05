#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterraingrassedit_undocmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/terraingrasseditundocmd.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "conjurer/ninguiterraintoolgrass.h"
//#include "ngeomipmap/nbytemap.h"
#include "ngeomipmap/nbytemapbuilder.h"
#include "nvegetation/ncterrainvegetationcell.h"
//------------------------------------------------------------------------------
/**
    Constructor
    @param oldMap nFloatMap with the data previous to command execution
    @param newMap nFloatMap with the data after the command execution
    @param x0 x coordinate of the weightmap
    @param z0 z coordinate of the weightmap
    @param size size of the applied tool

    This command is created with a pointer to an old floatmap, another to a new one, and the position of the
    weightmap that is supposed to have changed. It stores the info on both floatmaps creating two new floatmaps
    that cover the modified area.

*/
TerrainGrassEditUndoCmd::TerrainGrassEditUndoCmd( nByteMap* oldHMap, nByteMap* newHMap, int x0, int z0 )
{

    if ( ! ( newHMap && oldHMap && oldHMap->GetSize() == newHMap->GetSize() ) )
    {
        this->valid = false;
        return;
    }

    int areaSize = oldHMap->GetSize();

    this->valid = true;
    this->firstExecution = true;

    this->newHMap = newHMap;
    this->oldHMap = oldHMap;

    nString name = "TerrainGrassEdit";
    name.Append( ( nUndoServer::Instance())->GetSequentialName() );
    undoByteMap = static_cast<nByteMap*>( nResourceServer::Instance()->NewResource( "nbytemap", name.Get(), nResource::Other ) );
    if ( !undoByteMap )
    {
        this->valid = false;
        return;
    }
    undoByteMap->SetSize( areaSize );
    undoByteMap->SetUsage( nFloatMap::CreateEmpty );
    undoByteMap->Load();

    name = "TerrainGrassEdit";
    name.Append( ( nUndoServer::Instance())->GetSequentialName() );
    redoByteMap = static_cast<nByteMap*>( nResourceServer::Instance()->NewResource( "nbytemap", name.Get(), nResource::Other ) );
    if ( !redoByteMap )
    {
        this->valid = false;
        return;
    }
    redoByteMap->SetSize( areaSize );
    redoByteMap->SetUsage( nFloatMap::CreateEmpty );
    redoByteMap->Load();

    nByteMapBuilder bmBuilder;

    // Copy data from old floatmap to undo floatmap
    bmBuilder.Copy(oldHMap, undoByteMap);
    // Copy data from new floatmap to redo floatmap
    bmBuilder.Copy(newHMap, redoByteMap);


    // Store block coordinates
    this->weightMapX = x0;
    this->weightMapZ = z0;

}

//------------------------------------------------------------------------------
/**
    Destructor
*/
TerrainGrassEditUndoCmd::~TerrainGrassEditUndoCmd()
{
    if ( redoByteMap.isvalid() )
    {
        redoByteMap->Release();
    }
    if ( undoByteMap.isvalid() )
    {
        undoByteMap->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool
TerrainGrassEditUndoCmd::Execute()
{
    
    bool ret = this->firstExecution;

    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    // get outdoor object
    nEntityObjectId oid = level->FindEntity("outdoor");
    if (!oid)
    {
        return false;
    }

    nByteMapBuilder bmBuilder;

    if ( ret )
    {
        // Copy data from redo bytemap to new bytemap
        bmBuilder.Copy(redoByteMap, oldHMap);

        this->firstExecution = false;

        return true;
    }

    // Copy data from redo bytemap to new bytemap
    bmBuilder.Copy(redoByteMap, oldHMap);
    // Copy data from redo bytemap to old bytemap
    bmBuilder.Copy(redoByteMap, newHMap);

    return this->RefreshTerrain();
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool
TerrainGrassEditUndoCmd::Unexecute()
{
    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    // get outdoor object
    nEntityObjectId oid = level->FindEntity("outdoor");
    if (!oid)
    {
        return false;
    }

    nByteMapBuilder bmBuilder;

    // Copy data from undo bytemap to new bytemap
    bmBuilder.Copy(undoByteMap, oldHMap);
    // Copy data from undo bytemap to old bytemap
    bmBuilder.Copy(undoByteMap, newHMap);

    return this->RefreshTerrain();
}
//------------------------------------------------------------------------------

bool
TerrainGrassEditUndoCmd::RefreshTerrain()
{
    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    // get outdoor object
    nEntityObjectId oid = level->FindEntity("outdoor");
    if (oid)
    {
        // get entity object
        nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject(oid);

        // get terrain material class component
        ncTerrainMaterialClass * tm = object->GetClassComponent<ncTerrainMaterialClass>();
        n_assert(tm);

        nTerrainCellInfo * cellInfo = tm->GetTerrainCellInfo(this->weightMapX, this->weightMapZ );
        n_assert( cellInfo );

        // Get cell entity's vegetation component
        nEntityObject * cellEntity = cellInfo->GetTerrainCell();
        n_assert(cellEntity);

        ncTerrainVegetationCell * vegCell = cellEntity->GetComponentSafe<ncTerrainVegetationCell>();

        // Get cell grass bytemap
        nByteMap * grassByteMap = vegCell->GetValidGrowthMap();

        // Make cell grass update
        vegCell->UpdateTextureBeforeRender();

        // Make cell dirty
        grassByteMap->SetUserDirty();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Get byte size
*/
int TerrainGrassEditUndoCmd::GetSize( void )
{
    n_assert( undoByteMap );
    n_assert( redoByteMap );
    return undoByteMap->GetByteSize() + redoByteMap->GetByteSize();
}

//------------------------------------------------------------------------------