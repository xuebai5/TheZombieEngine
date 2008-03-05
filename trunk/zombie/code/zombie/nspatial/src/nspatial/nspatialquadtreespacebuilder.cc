#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialquadtreespacebuilder.cc
//  (C) 2004 Conjurer Services, S.A.
//  @author Miquel Angel Rujula <>
//------------------------------------------------------------------------------

#include "nspatial/nspatialquadtreespacebuilder.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
    constructor 1
*/
nSpatialQuadtreeSpaceBuilder::nSpatialQuadtreeSpaceBuilder()
{
    
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
nSpatialQuadtreeSpaceBuilder::nSpatialQuadtreeSpaceBuilder(ncSpatialQuadtree *quadtreeSpace, 
                                                           const bbox3 &quadtreeBox,
                                                           const bbox3 &terrainBox, 
                                                           int treeDepth)
{
    n_assert2(quadtreeSpace, "miquelangel.rujula: NULL pointer to spatial quadtree space component!");
    n_assert2(treeDepth >= 0, "miquelangel.rujula: quadtree's depth has to be positive!");

    this->m_quadtreeSpace = quadtreeSpace;
    this->SetTerrainBBox(terrainBox);
    this->BuildSpace(quadtreeBox, treeDepth);
}

//------------------------------------------------------------------------------
/**
    constructor 3
*/
nSpatialQuadtreeSpaceBuilder::nSpatialQuadtreeSpaceBuilder(ncSpatialQuadtree *quadtreeSpace)
{
    this->SetQuadtreeSpace(quadtreeSpace);
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialQuadtreeSpaceBuilder::~nSpatialQuadtreeSpaceBuilder()
{
}

//------------------------------------------------------------------------------
/**
    Builds the quadtree space from scratch.
*/
void
nSpatialQuadtreeSpaceBuilder::BuildSpace(const bbox3 &sizeBox, int treeDepth)
{
    n_assert2(treeDepth >= 0, "miquelangel.rujula");

    // set the quadtree's depth
    this->m_quadtreeSpace->SetDepth(treeDepth);
    ncSpatialQuadtreeCell * rootCell;
    rootCell = this->CreateSubtree(sizeBox, treeDepth, 0, 0);
    this->m_quadtreeSpace->AddRootCell( rootCell );
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialQuadtreeCell *
nSpatialQuadtreeSpaceBuilder::CreateSubtree(const bbox3 & treeBox, int curDepth, int bx, int bz)
{
    bool isLeafCell = false;
    bool hasTerrain = false;
    ncSpatialQuadtreeCell * children[4] = { 0, 0, 0, 0 }; 
    ncSpatialQuadtreeCell * cell = 0;

    // set the cell's id
    int cellId = this->m_quadtreeSpace->GetTotalNumCells();
    // increase the number of cells of the quadtree space
    this->m_quadtreeSpace->IncTotalNumCells();

    if (curDepth > 0)
    {
        bbox3 cellBox;
        int cellBX, cellBZ;
        float x = (treeBox.vmin.x + treeBox.vmax.x) * 0.5f;
        float z = (treeBox.vmin.z + treeBox.vmax.z) * 0.5f;

        for(int i = 0;i < 4;i++)
        {
            float x0, x1, z0, z1;

            // initialize cell coords (continous and discrete)
            if (i & 1) 
            {   // i == 1, 3
                x0 = x; 
                x1 = treeBox.vmax.x; 
                cellBX = 2 * bx + 1;
            }
            else       
            {   // i == 0, 2
                x0 = treeBox.vmin.x; 
                x1 = x;
                cellBX = 2 * bx;
            }

            if (i & 2) 
            {   // i == 2, 3
                z0 = treeBox.vmin.z; 
                z1 = z; 
                cellBZ = 2 * bz;
            }
            else       
            {   // i == 0, 1
                z0 = z; 
                z1 = treeBox.vmax.z; 
                cellBZ = 2 * bz + 1;
            }

            // calculate bbox
            cellBox.vmin = vector3(x0, treeBox.vmin.y, z0);
            cellBox.vmax = vector3(x1, treeBox.vmax.y, z1);

            // create subtree first
            children[i] = this->CreateSubtree(cellBox, curDepth - 1, cellBX, cellBZ);

            if (children[i]->GetEntityObject())
            {
                hasTerrain = true;
            }
        }
    }
    else
    {
        // end of recursion
        isLeafCell = true;
        hasTerrain = this->terrainBox.contains( treeBox.center() );
    }

    // determine which type of cell create
    if ( hasTerrain )
    {
        nEntityObject* newSubcell = nEntityObjectServer::Instance()->NewLocalEntityObject("neoutdoorcell");
        cell = newSubcell->GetComponent<ncSpatialQuadtreeCell>();
    }
    else
    {
        cell = n_new(ncSpatialQuadtreeCell);
    }

    // initialize all cell parameters
    cell->SetParentSpace(this->m_quadtreeSpace);
    cell->SetDepth( this->m_quadtreeSpace->GetDepth() - (curDepth - 1) );
    cell->SetBBox(treeBox);
    cell->SetId(cellId);
    cell->SetBX(bx);
    cell->SetBZ(bz);

    // is a leaf cell. Insert it in the leaf cells array
    if (isLeafCell) 
    {
        if (hasTerrain)
        {
            this->AddLeafCell(cell);
        }
    }
    else
    {
        // add the new subcells to the given one
        for(int i = 0;i < 4;i++)
        {
            cell->AddSubCell( children[i] );
            children[i]->SetParentCell( cell );
        }
    }

    return cell;
}

//------------------------------------------------------------------------------
/**
    Search the cell that completely contains the parameter bounding box, starting 
    from the given quadtree cell. It tries to find the one that best fits the 
    bounding box.
*/
ncSpatialQuadtreeCell *
nSpatialQuadtreeSpaceBuilder::SearchCell(ncSpatialQuadtreeCell *quadCell, bbox3 &box)
{
    if (!quadCell->GetBBox().contains(box))
    {
        // at least the parameter quadtree cell has to contain the bounding box
        return NULL;
    }

    ncSpatialQuadtreeCell** subcells = 0;
    // now, we'll try to find a subcell which the bounding box fits better
    while (!quadCell->IsLeaf())
    {
        subcells = quadCell->GetSubcells();
        int i;
        for (i = 0; i < 4; i++)
        {
            if (subcells[i]->GetBBox().contains(box))
            {
                quadCell = subcells[i];
                break;
            }        
        }

        if ( i == 4 )
        {
            // we haven't found any child cell containing the bounding box,
            // so we'll return the actual quadtree cell
            break;
        }
    }

    return quadCell;
}

//------------------------------------------------------------------------------
/**
    add a cell to the leaf cells array in its place (array is sorted by position)
*/
void 
nSpatialQuadtreeSpaceBuilder::AddLeafCell(ncSpatialQuadtreeCell* cell)
{
    n_assert2(cell, "miquelangel.rujula: NULL pointer to cell!");

    ncSpatialQuadtreeCell *curCell = 0;

    // find cell's position in the array
    int i;
    for (i = 0; i < this->m_quadtreeSpace->m_leafCells.Size(); i++)
    {
        curCell = this->m_quadtreeSpace->m_leafCells[i];
        n_assert(curCell);

        if (cell->GetBBox().vmax.z < curCell->GetBBox().vmax.z)
        {
            break;
        }
        else
        {
            if (cell->GetBBox().vmax.x < curCell->GetBBox().vmax.x)
            {
                break;
            }
        }
    }
    
    // insert the cell in the correponding position
    this->m_quadtreeSpace->m_leafCells.Insert(i, cell);
}

