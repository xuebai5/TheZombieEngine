/*-----------------------------------------------------------------------------
    @file ncterraingmm_main.cc
    @ingroup NebulaTerrain
    @author Mateu Batle Sastre

    (c) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "nspatial/ncspatialquadtree.h"
#include "zombieentity/ncassetclass.h"
#include "nscene/ncsceneclass.h"
#include "nscene/nscenenode.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialquadtree.h"
#include "ngeomipmap/ncache.h"
#include "ngeomipmap/ngmmmeshcacheentry.h"
#include "ngeomipmap/ngmmlodsystem.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncTerrainGMM, nComponentObject);

//------------------------------------------------------------------------------
ncTerrainGMM::ncTerrainGMM() :
    attaching(false),
    blocksLeveled(0),
    lodsLeveled(0),
    trianglesLeveled(0),
    unpaintedCellsInvisible(false)
{
    /// empty
}

//------------------------------------------------------------------------------
ncTerrainGMM::~ncTerrainGMM()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncTerrainGMM::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // empty
}

//---------------------------------------------------------------------------
ncTerrainGMMCell *
ncTerrainGMM::GetLeafCell(nArray<ncSpatialQuadtreeCell*> * cells, int bx, int bz) const
{
    if ((bx < 0) || (bx >= this->numBlocks) || (bz < 0) || (bz >= this->numBlocks))
    {
        return 0;
    }

    n_assert((bz * this->numBlocks + bx) < cells->Size());
    return cells->At(bz * this->numBlocks + bx)->GetComponentSafe<ncTerrainGMMCell>();
}

//---------------------------------------------------------------------------
void
ncTerrainGMM::InitLeafCells(ncSpatialQuadtree * qt)
{
    n_assert(qt);

    // get the leaf cells, which are ordered left to right (x), top to bottom (z)
    nArray<ncSpatialQuadtreeCell*> * leafCells = qt->GetLeafCells();

    // get the class terrain component
    ncTerrainGMMClass * terrainClass = this->GetClassComponent<ncTerrainGMMClass>();
    n_assert(terrainClass);
    n_assert(terrainClass->GetDepth() > 0);
    n_assert(terrainClass->GetHeightMap() != 0);
    n_assert(terrainClass->GetNumLODLevels() > 0);
    n_assert(terrainClass->GetBlockSize() > 0);

    // get scene node from the class (passed through to all the leaf cells)
    ncSceneClass * scene = this->GetClassComponent<ncSceneClass>();
    n_assert(scene);
    nSceneNode * rootNode = scene->GetRootNode();

    //<CUTRE>
    if (rootNode->IsA("ngeomipmapnode"))
    {
        nGeoMipMapNode * gmmnode = static_cast<nGeoMipMapNode*> (rootNode);
        gmmnode->SetTerrainGMM(this);
    }
    //</CUTRE>

    // get the number of blocks
    this->numBlocks = terrainClass->GetNumBlocks();
    n_assert(this->numBlocks > 0);
    n_assert(leafCells->Size() == (this->numBlocks * this->numBlocks));

    ncTerrainMaterialClass * terrainMaterialClass = this->GetClassComponent<ncTerrainMaterialClass>();
    n_assert(terrainMaterialClass);

    // iterate array and initialize properly
    for(int bz = 0;bz < this->numBlocks;bz++)
    {
        for(int bx = 0;bx < this->numBlocks;bx++)
        {
            ncTerrainGMMCell* cell = this->GetLeafCell(leafCells, bx, bz);
            n_assert(cell);

            // set neighbor cells, outdoor object and LOD params
            ncTerrainGMMCell* cellN = this->GetLeafCell(leafCells, bx, bz - 1);
            ncTerrainGMMCell* cellE = this->GetLeafCell(leafCells, bx + 1, bz);
            ncTerrainGMMCell* cellS = this->GetLeafCell(leafCells, bx, bz + 1);
            ncTerrainGMMCell* cellW = this->GetLeafCell(leafCells, bx - 1, bz);

            cell->SetCellXZ(static_cast<nint16> (bx), static_cast<nint16> (bz));
            cell->SetNeighbors(cellN, cellE, cellS, cellW);
            cell->SetOutdoor(this);

            nTerrainCellInfo * cellInfo = terrainMaterialClass->GetTerrainCellInfo(bx, bz);
            cellInfo->SetTerrainCell(cell->GetEntityObject());

            ncSpatialQuadtreeCell * qtcell = cell->GetComponentSafe<ncSpatialQuadtreeCell>();
            qtcell->SetBX(bx);
            qtcell->SetBZ(bz);
        }
    }

    /// Initialize the north west cell
    this->cellNW = this->GetLeafCell(leafCells, 0, 0);
    n_assert(this->cellNW);

    // get the entity class of the cell
    ncSceneClass* sceneClass = this->cellNW->GetClassComponent<ncSceneClass>();
    n_assert(sceneClass);
    sceneClass->SetRootNode(rootNode);
}

//---------------------------------------------------------------------------
void
ncTerrainGMM::InitLODParams(ncSpatialQuadtree * qt)
{
    n_assert(qt);

    // get the leaf cells, which are ordered left to right (x), top to bottom (z)
    nArray<ncSpatialQuadtreeCell*> * leafCells = qt->GetLeafCells();

    // get the class terrain component
    ncTerrainGMMClass * terrainClass = this->GetClassComponent<ncTerrainGMMClass>();
    n_assert(terrainClass);
    n_assert(terrainClass->GetDepth() > 0);
    nFloatMap * hm = terrainClass->GetHeightMap();
    n_assert(hm);
    int numLODLevels = terrainClass->GetNumLODLevels();
    n_assert(numLODLevels > 0);
    int blocksize = terrainClass->GetBlockSize();
    n_assert(blocksize > 0);

    n_assert(leafCells->Size() == (this->numBlocks * this->numBlocks));

    // iterate array and initialize properly
    for(int bz = 0;bz < this->numBlocks;bz++)
    {
        for(int bx = 0;bx < this->numBlocks;bx++)
        {
            ncTerrainGMMCell* cell = this->GetLeafCell(leafCells, bx, bz);
            n_assert(cell);

            cell->InitLODParams(hm, numLODLevels, bx * (blocksize - 1), bz * (blocksize - 1), blocksize);
        }
    }

    qt->AdjustBBoxes();
}

//---------------------------------------------------------------------------
void
ncTerrainGMM::InitInnerCells(ncSpatialQuadtree * qt)
{
    // start at the top cell and go down
    ncSpatialQuadtreeCell * root = qt->GetRootCell();
    this->InitInnerCellsParents(root);
    this->InitInnerCellsNeighbors(root);
}

//---------------------------------------------------------------------------
void
ncTerrainGMM::InitInnerCellsParents(ncSpatialQuadtreeCell * root)
{
    /**
    The process is done top-down. For every cell, one of the children is set.
    Only one children is needed since we have neighbor information for all nodes.
    Neighbor info is more useful for LOD algorithms, than simple parent / children
    relationship. 
    */

    if (root->GetNumSubcells() > 0)
    {
        n_assert( root->GetNumSubcells() == 4 );
        ncSpatialQuadtreeCell** childs = root->GetSubcells();        

        // get terrain GMM cell component and set outdoor
        ncTerrainGMMCell * cell = root->GetComponent<ncTerrainGMMCell>();
        n_assert(cell);
        cell->SetOutdoor(this);

        // scan all the children first (built bottom up)
        for(int i = 0;i < 4;i++)
        {
            ncTerrainGMMCell * cellchild = childs[i]->GetComponent<ncTerrainGMMCell>();
            if (cellchild)
            {
                this->InitInnerCellsParents(childs[i]);
            }
        }
    }
}

//---------------------------------------------------------------------------
void
ncTerrainGMM::InitInnerCellsNeighbors(ncSpatialQuadtreeCell * root)
{
    if (root->GetNumSubcells() > 0)
    {
        n_assert( root->GetNumSubcells() == 4 );
        ncSpatialQuadtreeCell** childs = root->GetSubcells();

        // get terrain GMM cell component
        ncTerrainGMMCell * cell = root->GetComponent<ncTerrainGMMCell>();
        if (!cell)
        {
            return;
        }

        // scan all the children first (built bottom up)
        for(int i = 0;i < 4;i++)
        {
            this->InitInnerCellsNeighbors(childs[i]);
        }

        ncSpatialQuadtreeCell * qtRoot = root->GetComponent<ncSpatialQuadtreeCell>();
        ncSpatialQuadtreeCell ** qtChildren = qtRoot->GetSubcells();
        int numSubCells = qtRoot->GetNumSubcells();
        n_assert(numSubCells == 4);

        /**
            The distribution of subcells is done like this:
                2 3
                0 1
        */

        ncTerrainGMMCell * cellN = 0;
        ncTerrainGMMCell * cellS = 0;
        ncTerrainGMMCell * cellW = 0;
        ncTerrainGMMCell * cellE = 0;

        // get the northest child
        if (!cellN)
        {
            cellN = qtChildren[2]->GetComponent<ncTerrainGMMCell>();
        }
        if (!cellN)
        {
            cellN = qtChildren[3]->GetComponent<ncTerrainGMMCell>();
        }

        // get the southest child
        if (!cellS)
        {
            cellS = qtChildren[0]->GetComponent<ncTerrainGMMCell>();
        }
        if (!cellS)
        {
            cellS = qtChildren[1]->GetComponent<ncTerrainGMMCell>();
        }

        // get the westest child
        if (!cellW)
        {
            cellW = qtChildren[2]->GetComponent<ncTerrainGMMCell>();
        }
        if (!cellW)
        {
            cellW = qtChildren[0]->GetComponent<ncTerrainGMMCell>();
        }

        // get the eastest child 
        if (!cellE)
        {
            cellE = qtChildren[3]->GetComponent<ncTerrainGMMCell>();
        }
        if (!cellE)
        {
            cellE = qtChildren[1]->GetComponent<ncTerrainGMMCell>();
        }

        // move to neighbors
        if (cellN)
        {
            cellN = cellN->GetNeighbor(ncTerrainGMMCell::North);
        }
        if (cellE)
        {
            cellE = cellE->GetNeighbor(ncTerrainGMMCell::East);
        }
        if (cellW)
        {
            cellW = cellW->GetNeighbor(ncTerrainGMMCell::West);
        }
        if (cellS)
        {
            cellS = cellS->GetNeighbor(ncTerrainGMMCell::South);
        }

        // now go their parents
        if (cellN)
        {
            cellN = cellN->GetComponent<ncSpatialQuadtreeCell>()->GetParentCell()->GetComponent<ncTerrainGMMCell>();
        }
        if (cellS)
        {
            cellS = cellS->GetComponent<ncSpatialQuadtreeCell>()->GetParentCell()->GetComponent<ncTerrainGMMCell>();
        }
        if (cellW)
        {
            cellW = cellW->GetComponent<ncSpatialQuadtreeCell>()->GetParentCell()->GetComponent<ncTerrainGMMCell>();
        }
        if (cellE)
        {
            cellE = cellE->GetComponent<ncSpatialQuadtreeCell>()->GetParentCell()->GetComponent<ncTerrainGMMCell>();
        }

        // now set them in the current
        cell->SetNeighbors(cellN, cellE, cellS, cellW);
    }
}

//---------------------------------------------------------------------------
ncTerrainGMMCell * 
ncTerrainGMM::GetNorthWestCell() const
{
    return this->cellNW;
}

//---------------------------------------------------------------------------
/**
*/
void
ncTerrainGMM::SetUnpaintedCellsInvisible(bool enable)
{
    // check if scene node is a valid ngeomipmapnode, if not just turn enable to false
    ncSceneClass * scene = this->GetClassComponent<ncSceneClass>();
    n_assert(scene);
    nSceneNode * rootNode = scene->GetRootNode();
    if (!rootNode->IsA("ngeomipmapnode"))
    {
        enable = false;
    }

    ncSpatialQuadtree * qt = this->GetComponentSafe<ncSpatialQuadtree>();
    ncSpatialQuadtreeCell * qtcell = qt->GetRootCell();
    n_assert(qtcell);
    ncTerrainGMMCell * cell = qtcell->GetComponentSafe<ncTerrainGMMCell>();
    cell->SetUnpaintedCellsInvisible(enable);
    this->unpaintedCellsInvisible = enable;
}

//---------------------------------------------------------------------------
bool
ncTerrainGMM::GetUnpaintedCellsInvisible()
{
    return this->unpaintedCellsInvisible;
}

//---------------------------------------------------------------------------
ncTerrainGMMCell *
ncTerrainGMM::GetTerrainCell(int bx, int bz)
{
    ncTerrainMaterialClass * terrainMaterialClass = this->GetClassComponent<ncTerrainMaterialClass>();
    n_assert_return(terrainMaterialClass, 0);

    nTerrainCellInfo * cellInfo = terrainMaterialClass->GetTerrainCellInfo(bx, bz);
    n_assert_return(cellInfo, 0);

    nEntityObject * entity = cellInfo->GetTerrainCell();
    n_assert_return(entity, 0);

    return entity->GetComponent<ncTerrainGMMCell>();
}

//---------------------------------------------------------------------------
/**
    Called when the an area of the heightmap has been updated. 
    Updates internal structures or cache.
    Invalidate all cache cells from the pool that collide with the specified
    rectangle (x0, z0) (x1, z1). The coordinate of the rectangle are given
    in hexels.
*/
void 
ncTerrainGMM::UpdateGeometry(int x0, int z0, int x1, int z1)
{
    // swap coords if inverted
    if ( x1 <= x0 )
    {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    if ( z1 <= z0 )
    {
        int t = z0;
        z0 = z1;
        z1 = t;
    }

    // get block coords
    ncTerrainGMMClass * terrainClass = this->GetClassComponent<ncTerrainGMMClass>();
    int bxmin = terrainClass->HexelCoordToBlockCoord(x0);
    int bxmax = terrainClass->HexelCoordToBlockCoord(x1);
    int bzmin = terrainClass->HexelCoordToBlockCoord(z0);
    int bzmax = terrainClass->HexelCoordToBlockCoord(z1);

    // discard entries in cache of mesh
    for(int bz = bzmin;bz <= bzmax;bz++)
    {
        for(int bx = bxmin;bx <= bxmax;bx++)
        {
            // discard the cache entry
            terrainClass->GetMeshCache()->Discard( nGMMMeshCacheEntry::ToKey(bx, bz) );
            // discard the cache entry for low lod
            terrainClass->GetLowResMeshCache()->Discard( nGMMLowMeshCacheEntry::ToKey(bx, bz) );
            // discard the hole indices cache
            for(int i = 0;i < terrainClass->GetIndexBuilder()->CountGroups(0);i++)
            {
                terrainClass->GetHoleIndicesCache()->Discard( nGMMHoleIndicesCacheEntry::ToKey(bx, bz, i) );
            }
            // recalculate lod paramters
            ncTerrainGMMCell * cell = this->GetTerrainCell(bx, bz);
            n_assert_if(cell)
            {
                cell->InitLODParams(
                    terrainClass->GetHeightMap(),
                    terrainClass->GetNumLODLevels(),
                    bx * (terrainClass->GetBlockSize() - 1), bz * (terrainClass->GetBlockSize() - 1), 
                    terrainClass->GetBlockSize());
            }
        }
    }
}
