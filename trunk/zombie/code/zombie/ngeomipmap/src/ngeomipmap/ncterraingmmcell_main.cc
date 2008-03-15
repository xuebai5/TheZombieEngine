//------------------------------------------------------------------------------
//  ncterraingmmcell_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "nscene/ncsceneclass.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ngmmindexbuilder.h"

#include "nphysics/ncphyterraincell.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialquadtree.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncTerrainGMMCell, ncScene);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTerrainGMMCell)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainGMMCell::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // how to specify the scene node is valid ?
    //this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainGMMCell::Load()
{
    //return ncScene::Load();
    n_assert(!this->IsValid());
    
    ncTerrainGMM *terrainComp = this->GetOutdoor();
    n_assert(terrainComp);
    ncSceneClass *sceneClass = terrainComp->GetClassComponent<ncSceneClass>();
    n_assert2(sceneClass, "ncTerrainGMMCell::Load(): outdoor entity class has no scene component.");

    nSceneNode *rootNode = sceneClass->GetRootNode();
    n_assert(rootNode);
    rootNode->EntityCreated(this->GetEntityObject());
    rootNode->PreloadResources();
    this->refRootNode = rootNode;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainGMMCell::Unload()
{
    //ncScene::Unload();
    if (this->IsValid())
    {
        this->refRootNode->EntityDestroyed(this->GetEntityObject());
        this->refRootNode.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainGMMCell::SetCellXZ(int bx, int bz)
{
    n_assert(bx >= 0);
    n_assert(bz >= 0);

    this->bx = static_cast<nint16> (bx);
    this->bz = static_cast<nint16> (bz);
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainGMMCell::GetCellX() const
{
    return this->bx;
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainGMMCell::GetCellZ() const
{
    return this->bz;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainGMMCell::SetNeighbors(
    ncTerrainGMMCell * north,
    ncTerrainGMMCell * east,
    ncTerrainGMMCell * south,
    ncTerrainGMMCell * west)
{
    this->neighbor[North] = north;
    this->neighbor[East] = east;
    this->neighbor[South] = south;
    this->neighbor[West] = west;
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainGMMCell * 
ncTerrainGMMCell::GetNeighbor(Direction dir) const
{
    return this->neighbor[dir];
}

//------------------------------------------------------------------------------
void 
ncTerrainGMMCell::SetOutdoor(ncTerrainGMM * outdoor)
{
    this->outdoor = outdoor;
    if (outdoor)
    {
        this->outdoorClass = outdoor->GetClassComponent<ncTerrainGMMClass>();
    }
    else
    {
        this->outdoorClass = 0;
    }
}

//------------------------------------------------------------------------------
ncTerrainGMM * 
ncTerrainGMMCell::GetOutdoor() const
{
    return this->outdoor;
}

//------------------------------------------------------------------------------
/**
    Initializes internal data structures for the terrain block, needed before
    going to used LOD selection.

    Calculate the minimum delta squared values for each level of detail for this
    terrain block. We don't calculate yet the D2 (D squared) parameter, since
    D2 depends on the specific camera and viewport settings.

    For each level of detail, it takes the greatest delta (difference in height)
    from that level to the missing vertices.

    D2 = Delta2 * C2

    The bounding box and center of the terrain blocks is calculated too.
*/
void 
ncTerrainGMMCell::InitLODParams(nFloatMap * hm, int numLODLevels, int startx, int startz, int blockSize)
{
    n_assert(hm);
    n_assert(numLODLevels > 0 && numLODLevels <= MaxLODLevels );
    n_assert(blockSize > 0);

    // if not a leaf cell
    ncSpatialQuadtreeCell * qtCell = this->GetComponent<ncSpatialQuadtreeCell>();
    if (qtCell->GetNumSubcells() > 0)
    {
        ncSpatialQuadtreeCell ** qtChildren = qtCell->GetSubcells();

        ncTerrainGMMCell * cell = 0;
        this->bbox.begin_extend();
        float currMaxY(-FLT_MAX);
        float currMinY(FLT_MAX);
        for (int i(0); i < 4; ++i)
        {
            cell = qtChildren[i]->GetComponent<ncTerrainGMMCell>();
            if ( cell )
            {
                // it's an entity with a gmm component (it has terrain)
                const bbox3 &terrBox = cell->GetBBox();
                this->bbox.extend(terrBox);

                // calculate the max y of the terrain cells
                if ( terrBox.vmax.y > currMaxY )
                {
                    currMaxY = terrBox.vmax.y;
                }

                // calculate the min y of the terrain cells
                if ( terrBox.vmin.y < currMinY )
                {
                    currMinY = terrBox.vmin.y;
                }
            }
            else
            {
                // it isn't an entity, just a spatial component
                this->bbox.extend(qtChildren[i]->GetBBox());
            }
        }

        this->bbox.vmax.y = currMaxY;
        this->bbox.vmin.y = currMinY;
        
        bbox3 cellBox(this->bbox);
        cellBox.vmin.y -= ncSpatialQuadtree::BOXMARGIN;
        qtCell->SetBBox(this->bbox);

        // update parent
        if (qtCell->GetParentCell())
        {
            cell = qtCell->GetParentCell()->GetComponent<ncTerrainGMMCell>();
            if (cell)
            {
                cell->InitLODParams(hm, numLODLevels, startx, startz, blockSize);
            }
        }
    }
    // for leaf cells
    else
    {
        float max_delta = 0;
        float heights[4]; 

        // d for level 0 is always 0
        this->minDelta2[0] = 0;
        
        // update bounding box and its center
        hm->CalculateBoundingBoxHC(startx, startz, startx + blockSize - 1, startz + blockSize - 1, this->bbox);
        this->center = this->bbox.center();

        // for each mip map levels
        for (int level = 1; level < numLODLevels; level++)
        {
            int step = 1 << level;
            int stepdiv2 = (step >> 1);

            // for each triangle quad
            for (int z = startz; z < startz + blockSize - 1; z += step)
            {
                for (int x = startx; x < startx + blockSize - 1; x += step)
                {
                    heights[0] = hm->GetHeightLC(x, z);
                    heights[1] = hm->GetHeightLC(x, z + step);
                    heights[2] = hm->GetHeightLC(x + step, z);
                    heights[3] = hm->GetHeightLC(x + step, z + step);

#if 1
                    // This implementation just calculates the maximum delta between a 
                    // level of detail and the previous level of detail
                    float heights2[5];

                    heights2[0] = hm->GetHeightLC(x + stepdiv2, z);
                    heights2[1] = hm->GetHeightLC(x, z + stepdiv2);
                    heights2[2] = hm->GetHeightLC(x + stepdiv2, z + stepdiv2);
                    heights2[3] = hm->GetHeightLC(x + step, z + stepdiv2);
                    heights2[4] = hm->GetHeightLC(x + stepdiv2, z + step);

                    float delta;
                    // check triangle heights 0 - 2 - 1
                    // check points height2 points: 0 1 2

                    // calculate the difference between the original height and the mean of heights 
                    // of the two related vertices in the triangle
                    delta = fabsf((heights[0] + heights[1]) / 2.0f - heights2[1]);
                    max_delta = n_max(delta, max_delta);
                    delta = fabsf((heights[0] + heights[2]) / 2.0f - heights2[0]);
                    max_delta = n_max(delta, max_delta);
                    delta = fabsf((heights[1] + heights[2]) / 2.0f - heights2[2]);
                    max_delta = n_max(delta, max_delta);

                    // check triangle heights 1 - 2 - 3
                    // check points height2 points: 3 4
                    delta = fabsf((heights[1] + heights[3]) / 2.0f - heights2[4]);
                    max_delta = n_max(delta, max_delta);
                    delta = fabsf((heights[2] + heights[3]) / 2.0f - heights2[3]);
                    max_delta = n_max(delta, max_delta);
#else
                    // This implementation takes the maximum delta between a level of detail
                    // and the maximum level of detail
                    for (int zi = 1; zi < step; zi++)
                    {
                        for (int xi = 1; xi < step; xi++)
                        {
                            float interp_height = this->BilinearInterpolate(heights, (static_cast<float>(xi)) / step, (static_cast<float>(zi)) / step);
                            float height = hm->GetHeightLC(x + xi, z + zi);
                            float delta = fabsf(height - interp_height);
                            max_delta = n_max(delta, max_delta);
                        }
                    }
#endif
                }
            }

            // Keep max delta between mip map levels to ensure it increases
            this->minDelta2[level] = max_delta * max_delta;
        }

        // update parent
        if (qtCell->GetParentCell())
        {
            ncTerrainGMMCell * parent = qtCell->GetParentCell()->GetComponent<ncTerrainGMMCell>();
            if (parent)
            {
                parent->InitLODParams(hm, numLODLevels, startx, startz, blockSize);
            }
        }
    }
}

//------------------------------------------------------------------------------
const bbox3 & 
ncTerrainGMMCell::GetBBox() const
{
    return this->bbox;
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainGMMCell::GetIndexKey()
{
    return outdoorClass->GetIndexBuilder()->CalcIndexGroup(
        this->GetGeometryLODLevel(), 
        this->GetNeighborLOD(North),
        this->GetNeighborLOD(East),
        this->GetNeighborLOD(South),
        this->GetNeighborLOD(West) );
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainGMMCell::HasHoles() const
{
    const ncPhyTerrainCell * phyCell = this->GetComponentSafe<ncPhyTerrainCell>();
    if (phyCell)
    {
        return phyCell->IsThereAnyHole();
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Sets the children cells unpainted (pink colored) as invisible when true or
    not (always visible, when false).
    @return true if all children cells were marked as invisible (that will only
    happen for enable = false)
*/
bool
ncTerrainGMMCell::SetUnpaintedCellsInvisible(bool enable)
{
    bool invisible = true;

    ncTerrainMaterialClass * matc = outdoorClass->GetComponentSafe<ncTerrainMaterialClass>();

    // check the terrain cell info to see if painted or not
    ncSpatialQuadtreeCell * spatialQTCell = this->GetComponentSafe<ncSpatialQuadtreeCell>();

    // check if node is leaf or not
    if (!spatialQTCell->GetNumSubcells())
    {
        // check the terrain cell info to see if painted or not
        if (enable)
        {
            nTerrainCellInfo * cellInfo = matc->GetTerrainCellInfo(this->bx, this->bz);
            invisible = cellInfo->IsEmpty();
        }
        else
        {
            invisible = false;
        }
    }
    else
    {
        ncSpatialQuadtreeCell ** qtChildren = spatialQTCell->GetSubcells();
        for(int i = 0;i < spatialQTCell->GetNumSubcells();i++)
        {
            ncTerrainGMMCell * child = qtChildren[i]->GetComponent<ncTerrainGMMCell>();
            if (child)
            {
                invisible &= child->SetUnpaintedCellsInvisible(enable);
            }
        }        
    }

    spatialQTCell->SetActive( !invisible );

    return invisible;
}
