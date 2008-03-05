#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  terrainholeundcmd_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/terrainholeundocmd.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "nspatial/ncspatialquadtree.h"
#include "nphysics/ncphyterrain.h"

//------------------------------------------------------------------------------
/**
    Constructor

*/
TerrainHoleUndoCmd::TerrainHoleUndoCmd( nTerrainLine * terrainLine )
{
    if ( ! terrainLine )
    {
        this->valid = false;
    }

    this->valid = true;

    this->definingLine = static_cast<nTerrainLine*>( terrainLine->Clone() );

}
//------------------------------------------------------------------------------
/**
    Destructor
*/
TerrainHoleUndoCmd::~TerrainHoleUndoCmd()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool
TerrainHoleUndoCmd::Execute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    nMeshBuilder mb(1024,1024);
    this->definingLine->GeneratePolygon( &mb, nString("wc:libs/system/meshes/testpol.n3d2") );
    
    // Make physics hole
    nEntityObject * outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    n_assert2(outdoor, "Outdoor not found");

    this->holeCreated = outdoor->GetComponentSafe<ncPhyTerrain>()->AddHole( this->definingLine );

    this->InvalidateHoleCache(outdoor);

    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool
TerrainHoleUndoCmd::Unexecute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    nEntityObject * outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    n_assert2(outdoor, "Outdoor not found");

    outdoor->GetComponentSafe<ncPhyTerrain>()->RemoveHoleForLine( this->holeCreated );

    this->InvalidateHoleCache(outdoor);

	return true;
}
//------------------------------------------------------------------------------
/**
    Get byte size
*/
int 
TerrainHoleUndoCmd::GetSize( void )
{
    n_assert( this->definingLine );
    n_assert( this->holeCreated );
    return sizeof( nTerrainLine ) + sizeof( nTerrainLine );
}
//------------------------------------------------------------------------------
/**
    Invalidate hole cache for given outdoor
*/
void 
TerrainHoleUndoCmd::InvalidateHoleCache(nEntityObject * outdoor)
{
    n_assert2(outdoor, "No outdoor given.");

    nEntityClass * neclass = outdoor->GetEntityClass();
    n_assert_if(neclass)
    {
        ncTerrainGMMClass * terrainGMMClass = neclass->GetComponent<ncTerrainGMMClass>();
        if (terrainGMMClass)
        {
            nGMMHoleIndicesCache * holeIndicesCache = terrainGMMClass->GetHoleIndicesCache();
            if (holeIndicesCache)
            {
                holeIndicesCache->DiscardAll();
            }
        }
    }
}
