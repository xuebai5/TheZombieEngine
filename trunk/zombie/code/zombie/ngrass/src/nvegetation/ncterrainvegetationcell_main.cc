//------------------------------------------------------------------------------
//  ncterrainvegetationcell_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchngrass.h"
#include "nvegetation/ncterrainvegetationcell.h"
#include "resource/nresourceserver.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "nvegetation/ncterrainvegetationclass.h"

#ifndef NGAME
#include "ngeomipmap/nbytemap.h"
#include "nasset/nentityassetbuilder.h"
#include "ngeomipmap/nbytemapbuilder.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "kernel/nfileserver2.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncTerrainVegetationCell,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationCell::ncTerrainVegetationCell()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationCell::~ncTerrainVegetationCell()
{
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    this->UnloadEditionResources();
#endif
#endif
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncTerrainVegetationCell::InitInstance( nObject::InitInstanceMsg /*initType*/ )
{
    // The iniInstance of editor components is called before others components
    /*
    if( loaded )
    {
        // Empty
    } else
    {
        // Empty
    }
    */
}


//------------------------------------------------------------------------------
/**
    @param x the x coordinate in world-space
    @param z the z coordinate in world-space
    @param x output the index of subcell in x
    @param x output the index of subcell in z
*/
void
ncTerrainVegetationCell::getSubCellIndex(float x, float z , int &indexX , int &indexZ)
{

    ncTerrainGMMCell* gmmCell = this->GetComponentSafe<ncTerrainGMMCell>();
    const bbox3& bbox         = gmmCell->GetBBox();
    nEntityObject* outdoor    = gmmCell->GetOutdoor()->GetEntityObject();
    n_assert(outdoor);

    int div = outdoor->GetClassComponent<ncTerrainVegetationClass>()->GetMaxSubCellIndexRelative();

    float valX = n_floor ( ( x - bbox.vmin.x)/( bbox.vmax.x -bbox.vmin.x)*div );
    float valZ = n_floor ( ( z - bbox.vmin.z)/( bbox.vmax.z -bbox.vmin.z)*div );

    indexX = static_cast<int>(n_max( 0 , n_min (  div - 1 , valX  ) ));
    indexZ = static_cast<int>(n_max( 0 , n_min (  div - 1, valZ ) ));
}

//------------------------------------------------------------------------------
/**
*/
int 
ncTerrainVegetationCell::getSubGroupIdx(int x, int z)
{
    ncTerrainGMMCell* gmmCell = this->GetComponentSafe<ncTerrainGMMCell>();
    return gmmCell->GetOutdoor()->GetClassComponent<ncTerrainVegetationClass>()->GetSubGroupIdx( x, z );
}


#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationCell::CreateRefGrowthMap()
{
    // create resource if not exists
    if (!this->refGrowthMap.isvalid())
    {  
        nString fileName( ncTerrainVegetationClass::GetGrowthMapFileName( this->GetEntityObject() ) );
        this->refGrowthMap = static_cast<nByteMap *> (
            nResourceServer::Instance()->NewResource(
                "nbytemap",  fileName.Get(), nResource::Other) );

        n_assert(this->refGrowthMap.isvalid());
        this->refGrowthMap->SetFilename(fileName);
    }

    if (!this->refUndoGrowthMap.isvalid())
    {  
        this->refUndoGrowthMap = static_cast<nByteMap *> (
            nResourceServer::Instance()->NewResource(
                "nbytemap",  0, nResource::Other) );
        this->refUndoGrowthMap->SetFilename(0);
        n_assert(this->refUndoGrowthMap.isvalid());
    }
}



//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationCell::LoadEditionResources(void)
{
    this->CreateRefGrowthMap();

    ncTerrainGMMCell*       gmmCell  = this->GetComponentSafe<ncTerrainGMMCell>();
    ncTerrainMaterialClass* matClass = gmmCell->GetOutdoor()->GetClassComponentSafe<ncTerrainMaterialClass>();
    nTerrainCellInfo*       cellInfo = matClass->GetTerrainCellInfo( gmmCell->GetCellX() , gmmCell->GetCellZ() );
    int growthMapValue = cellInfo->GetGrowthMapValue();

    if ( growthMapValue < 0 ) // if <0 the file is valid
    {
        if ( this->refGrowthMap->Load() )
        {
            // Temporal solution for remove empty growth
            // @todo remove this
            nuint8 val;
            if( this->refGrowthMap->GetUniqueValue(val) )
            {
                this->refGrowthMap->SetDirty(); 
            }
            return true;
        }
        n_printf("ncTerrainVegetationCell: Error loading growthmap '%s'\n", this->refGrowthMap->GetFilename() );
        growthMapValue = 0;
    }

    // Create it
    int growthMapSize = gmmCell->GetOutdoor()->GetClassComponent<ncTerrainVegetationClass>()->GetGrowthMapSizeByCell();
    this->refGrowthMap->SetSize(growthMapSize);
    this->refUndoGrowthMap->SetSize(growthMapSize);

    this->refGrowthMap->SetUsage( nByteMap::CreateEmpty );
    if ( ! this->refGrowthMap->Load() )
    {
        return false;
    }

    this->refUndoGrowthMap->SetUsage( nByteMap::CreateEmpty );
    if ( ! this->refUndoGrowthMap->Load() )
    {
        return false;
    }

    this->refGrowthMap->FillValue(static_cast<nuint8>(growthMapValue) );
    this->refUndoGrowthMap->FillValue(static_cast<nuint8>(growthMapValue) );

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationCell::CreateEditionResources()
{    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationCell::UnloadEditionResources(void)
{
    if (this->refGrowthMap.isvalid())
    {
        if ( this->refGrowthMap->IsLoaded() )
        {
            this->refGrowthMap->Unload();
        }
        this->refGrowthMap->Release();
        this->refGrowthMap.invalidate();
    }

    if (this->refUndoGrowthMap.isvalid())
    {
        this->refUndoGrowthMap->Release();
        this->refUndoGrowthMap.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationCell::IsValidEditionResources()
{
    return this->refGrowthMap.isvalid() && this->refGrowthMap->IsLoaded();
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationCell::SaveEditionResources(void)
{
    // save heightmap
    if (this->refGrowthMap.isvalid())
    {
        if (this->refGrowthMap->IsDirty()) 
        {

            ncTerrainGMMCell*       gmmCell  = this->GetComponentSafe<ncTerrainGMMCell>();
            ncTerrainMaterialClass* matClass = gmmCell->GetOutdoor()->GetClassComponentSafe<ncTerrainMaterialClass>();
            nTerrainCellInfo*       cellInfo = matClass->GetTerrainCellInfo( gmmCell->GetCellX() , gmmCell->GetCellZ() );
            //int growthMapValue = cellInfo->GetGrowthMapValue()
            nuint8 val;

            this->refGrowthMap->CleanDirty();
            if ( this->refGrowthMap->GetUniqueValue(val) )
            {
                cellInfo->SetGrowthMapValue( val );
                // Delete growth map file
                if ( nFileServer2::Instance()->FileExists( refGrowthMap->GetFilename() ) )
                {
                    nFileServer2::Instance()->DeleteFile( refGrowthMap->GetFilename() );
                }

            } else
            {
                cellInfo->SetGrowthMapValue( -1 ); // use file
                //return this->refGrowthMap->Save();
                nByteMapBuilder builder;
                builder.SaveByteMapOpenIL( refGrowthMap.get() , refGrowthMap->GetFilename().Get() );
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
nByteMap* 
ncTerrainVegetationCell::GetGrowthMap()
{
    return this->refGrowthMap.get_unsafe();
}



//------------------------------------------------------------------------------
/**
*/
nByteMap* 
ncTerrainVegetationCell::GetValidGrowthMap()
{
    if ( ! refGrowthMap.isvalid() ) // Create it by Demand
    {
        if  ( ! this->LoadEditionResources() )   //File not existed
        {
            return 0;
        } 
    }
    return this->refGrowthMap.get();
}

//------------------------------------------------------------------------------
/**
*/
nByteMap* 
ncTerrainVegetationCell::GetUndoGrowthMap()
{
    if ( this->refUndoGrowthMap.isvalid() )
    {
        return this->refUndoGrowthMap.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationCell::UpdateTextureBeforeRender( bool val)
{
    this->updateTexture = val;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationCell::GetUpdateTextureBeforeRender()
{
    return this->updateTexture;
}

#endif //#ifndef NGAME
#endif //#ifndef __ZOMBIE_EXPORTER__


//------------------------------------------------------------------------------
