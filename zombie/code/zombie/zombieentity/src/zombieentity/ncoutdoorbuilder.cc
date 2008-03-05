#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncoutdoorbuilder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "zombieentity/ncoutdoorbuilder.h"
#include "mathlib/bbox.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreeclass.h"
#include "nspatial/nspatialquadtreespacebuilder.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nphysics/ncphyterrain.h"
#include "ncnavmesh/ncnavmesh.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncOutdoorBuilder, nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncOutdoorBuilder)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    it's called once all the entity information is ready for the components
*/
void 
ncOutdoorBuilder::InitInstance(nObject::InitInstanceMsg initType) 
{
    if (initType == nObject::ReloadedInstance)
    {
        return;
    }
#ifndef __ZOMBIE_EXPORTER__
    ncTerrainGMMClass *terrainGMMClass = this->GetClassComponentSafe<ncTerrainGMMClass>();
    ncSpatialQuadtree *quadtree = this->GetComponentSafe<ncSpatialQuadtree>();
    ncSpatialQuadtreeClass *quadtreeClass = quadtree->GetClassComponentSafe<ncSpatialQuadtreeClass>();

    bbox3 & quadtreeClassBox = quadtreeClass->GetOriginalBBox();
    bbox3 terrainBox;
    terrainBox = this->GetBBoxFromTerrain(terrainGMMClass);
    vector3 nullVec;
    if (quadtreeClassBox.vmin.isequal(nullVec, 0.000001f) &&
        quadtreeClassBox.vmax.isequal(nullVec, 0.000001f))
    {
        // backwards compatibility (initialize the quadtree bounding box with terrain's one)
        quadtreeClass->SetOriginalBBox(terrainBox);
        quadtreeClassBox = quadtreeClass->GetOriginalBBox();
        quadtree->SetBBox(terrainBox);
    }
    
    // calculate depth
    //const bbox3 &quadtreeBox = quadtree->GetBBox();
    const bbox3 &quadtreeBox = quadtreeClassBox;
    int numBlocks = static_cast<int> ((quadtreeBox.extents().z * 2.f) / 
        ((terrainGMMClass->GetBlockSize() - 1) * terrainGMMClass->GetPointSeparation()));
    int depth = static_cast<int> (logf(static_cast<float>(numBlocks)) / logf(2.0f) + 0.5f);

    nSpatialQuadtreeSpaceBuilder builder;
    builder.SetQuadtreeSpace(quadtree);
    builder.SetTerrainBBox(terrainBox);
    builder.BuildSpace(quadtreeBox, depth);
    nSpatialServer::Instance()->RegisterSpace(quadtree);


    // initialize gmm terrain
    this->InitTerrainGMM();


    // initialize physics
    this->InitPhyTerrain(terrainGMMClass);

    // initialize navigation mesh
    if ( initType != nObject::NewInstance )
    {
        this->InitNavMesh();
    }
#endif // __ZOMBIE_EXPORTER__
}

#ifndef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
    get the bounding box from the terrain
*/
bbox3
ncOutdoorBuilder::GetBBoxFromTerrain(ncTerrainGMMClass *terrainGMMClass) const
{
    // get heightmap
    nFloatMap * hm = terrainGMMClass->GetHeightMap();
    n_assert(hm);

    // get bounding box info from the heightmap
    rectangle r;
    bbox3 bbox;
#ifndef __ZOMBIE_EXPORTER__
    hm->GetSurfaceLC(r);
#ifdef NGAME
    hm->CalculateBoundingBoxLC(r, bbox);
#else
    bbox.vmin.x = r.v0.x;
    bbox.vmin.y = hm->GetHeightOffset();
    bbox.vmin.z = r.v0.y;
    bbox.vmax.x = r.v1.x;
    bbox.vmax.y = hm->GetHeightOffset() + hm->GetHeightScale();
    bbox.vmax.z = r.v1.y;
#endif

    // correction for things higher than terrain
    bbox.vmax.y += ncSpatialQuadtree::BOXMARGIN;
    // correction for things lower than terrain
    bbox.vmin.y -= ncSpatialQuadtree::BOXMARGIN;
#endif

    return bbox;
}

//------------------------------------------------------------------------------
/**
    initialize gmm terrain
*/
void
ncOutdoorBuilder::InitTerrainGMM()
{
    // get the quadtree, which should be already built
    ncSpatialQuadtree *quadtree = this->GetComponent<ncSpatialQuadtree>();
    n_assert(quadtree);
    n_assert(quadtree->GetTotalNumCells() > 0);

    // get the leaf cells and begin to initialize the setup data
    ncTerrainGMM *terrainGMM = this->GetComponentSafe<ncTerrainGMM>();
    terrainGMM->InitLeafCells(quadtree);
    terrainGMM->InitInnerCells(quadtree);
    terrainGMM->InitLODParams(quadtree);

#ifdef NGAME
    terrainGMM->SetUnpaintedCellsInvisible(true);
#endif
    
}

//------------------------------------------------------------------------------
/**
    initialize physics terrain
*/
void
ncOutdoorBuilder::InitPhyTerrain(ncTerrainGMMClass *terrainGMMClass)
{
    ncPhyTerrain *phyTerrain = this->GetComponentSafe<ncPhyTerrain>();
    phyTerrain->SetInfoHM( terrainGMMClass );

    nPhysicsWorld* world(nPhysicsServer::Instance()->GetDefaultWorld());

    world->Add( this->GetEntityObject() );
}

//------------------------------------------------------------------------------
/**
    initialize navigation mesh
*/
void
ncOutdoorBuilder::InitNavMesh()
{
    // Get the nav mesh component
    ncNavMesh *navMesh = this->GetComponentSafe<ncNavMesh>();

    // Load the nav mesh
    navMesh->Load();

    // A new nav mesh has born! Let's update any waypoint that should be binded to it
    navMesh->GenerateExternalLinks();
}

#endif // __ZOMBIE_EXPORTER__
