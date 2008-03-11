#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterrainpainting_undocmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/terrainpaintingundocmd.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "conjurer/ninguiterraintoolpaint.h"
//#include "ngeomipmap/nbytemap.h"
#include "ngeomipmap/nbytemapbuilder.h"
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
TerrainPaintingUndoCmd::TerrainPaintingUndoCmd( nByteMap* oldHMap, nByteMap* newHMap, int x0, int z0, nTerrainMaterial::LayerHandle layerHnd )
{
    // layer bytemap pointers can be zero, just ignore this undo cmds
    if ( ! ( newHMap && oldHMap ) )
    {
        this->newHMap = 0;
        this->oldHMap = 0;
        this->firstExecution = true;
        this->layerHnd = layerHnd;
        this->valid = true;
        return;
    }

    if ( oldHMap->GetSize() != newHMap->GetSize() )
    {
        this->valid = false;
        return;
    }

    int areaSize = oldHMap->GetSize();

    this->valid = true;
    this->firstExecution = true;

    this->newHMap = newHMap;
    this->oldHMap = oldHMap;

    nString name = "TerrainPaint";
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

    name = "TerrainPaint";
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

    // Store layer handle
    this->layerHnd = layerHnd;
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
TerrainPaintingUndoCmd::~TerrainPaintingUndoCmd()
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
TerrainPaintingUndoCmd::Execute()
{
    // layer bytemap pointers can be zero, just ignore this undo cmds
    if (!this->oldHMap.isvalid() || !this->newHMap.isvalid())
    {
        return true;
    }
    
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

    // get entity object
    nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject(oid);

    // get terrain material class component
    ncTerrainMaterialClass * tm = object->GetClassComponent<ncTerrainMaterialClass>();
    n_assert(tm);

    // get weightmap
    //nTerrainCellInfo * wMap = tm->GetTerrainCellInfo( this->weightMapX, this->weightMapZ );
    //n_assert( wMap );

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
TerrainPaintingUndoCmd::Unexecute()
{
    // layer bytemap pointers can be zero, just ignore this undo cmds
    if (!this->oldHMap.isvalid() || !this->newHMap.isvalid())
    {
        return true;
    }

    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    // get outdoor object
    nEntityObjectId oid = level->FindEntity("outdoor");
    if (!oid)
    {
        return false;
    }

    // get entity object
    nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject(oid);

    // get terrain material class component
    ncTerrainMaterialClass * tm = object->GetClassComponent<ncTerrainMaterialClass>();
    n_assert(tm);

    //nTerrainCellInfo * wMap = tm->GetTerrainCellInfo( this->weightMapX, this->weightMapZ );
    //n_assert( wMap );

    nByteMapBuilder bmBuilder;

    // Copy data from undo bytemap to new bytemap
    bmBuilder.Copy(undoByteMap, oldHMap);
    // Copy data from undo bytemap to old bytemap
    bmBuilder.Copy(undoByteMap, newHMap);

    return this->RefreshTerrain();
}
//------------------------------------------------------------------------------

bool
TerrainPaintingUndoCmd::RefreshTerrain()
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

        //nTerrainCellInfo * wMap = tm->GetTerrainCellInfo(this->weightMapX, this->weightMapZ );
        //n_assert( wMap );

        // Update layer use
        //wMap->CalculateLayerUse( this->layerHnd );g

        // Update weight texture in the layer manager
        int wX0 = this->weightMapX;
        int wX1 = min( this->weightMapX + 1, tm->GetMaterialNumBlocks() );
        int wZ0 = this->weightMapZ;
        int wZ1 = min( this->weightMapZ + 1, tm->GetMaterialNumBlocks() );

        //tm->RefreshWeightTexturePortion( wX0, wZ0, wX1, wZ1 );

        // Refresh the global texture
        //tm->RefreshGlobalTexturePortion( wX0, wZ0, wX1, wZ1 );

        // Notify the material info for this layer has been updated
        nString paintPath( terrainEditorPath );
        paintPath += "/toolPaint";
        nInguiTerrainToolPaint* toolPaint = static_cast<nInguiTerrainToolPaint*>( nKernelServer::Instance()->Lookup( paintPath.Get() ) );
        toolPaint->MakeRectangleDirtyMaterial( oldHMap->GetSize() * wX0, oldHMap->GetSize() * wZ0,
                                               oldHMap->GetSize() * wX1, oldHMap->GetSize() * wZ1 );

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Get byte size
*/
int TerrainPaintingUndoCmd::GetSize( void )
{
    int size = 0;

    if (this->undoByteMap.isvalid())
    {
        size += undoByteMap->GetByteSize();
    }

    if (this->redoByteMap.isvalid())
    {
        size += redoByteMap->GetByteSize();
    }

    return size;
}

//------------------------------------------------------------------------------