//------------------------------------------------------------------------------
//  nccellhorizon.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnspatial.h"
#include "nspatial/nccellhorizon.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/nchorizonclass.h"
#include "nspatial/nhorizonsegment.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncCellHorizon, nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncCellHorizon)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
ncCellHorizon::ncCellHorizon()
{
    /// empty
}

//------------------------------------------------------------------------------
ncCellHorizon::~ncCellHorizon()
{
    /// empty
}

//------------------------------------------------------------------------------
nHorizonSegment *
ncCellHorizon::GetHorizonSegment(Direction dir) const
{
    const ncSpatialQuadtreeCell * spatialCell = this->GetComponentSafe<ncSpatialQuadtreeCell>();
    n_assert(spatialCell->IsLeaf());
    
    const ncSpatialSpace * spatialSpace = spatialCell->GetParentSpace();
    n_assert(spatialSpace);

    const ncHorizonClass * hc = spatialSpace->GetClassComponentSafe<ncHorizonClass>();
    
    static struct {
        int incx;
        int incz;
        bool alongx;
    } dirLookup[]= {
        //North,
        { 0, 0, true },
        //East,
        { 0, 0, false },
        //South,
        { 0, 1, true },
        //West
        { 1, 0, false }
    };

    n_assert((dir >= North) && (dir <= West));
    int bx = spatialCell->GetBX() + dirLookup[dir].incx;
    int bz = spatialCell->GetBZ() + dirLookup[dir].incz;
    bool alongx = dirLookup[dir].alongx;

    return hc->GetHorizonSegment(bx, bz, alongx);
}

//------------------------------------------------------------------------------
void
ncCellHorizon::DrawHorizonSegments(nGfxServer2 * gfx, Direction dir)
{
    nHorizonSegment * hs = this->GetHorizonSegment(dir);
    if (hs)
    {
        vector3 vertexList[] = { hs->GetStart(), hs->GetEnd() };
        gfx->BeginLines();
        gfx->DrawLines3d (vertexList, 2, vector4(0.f, 1.f, 0.f, 1.f));
        gfx->EndLines();
    }
}

//------------------------------------------------------------------------------
void
ncCellHorizon::DrawHorizonSegments(nGfxServer2 * gfx)
{
    ncSpatialQuadtreeCell * spatialCell = this->GetComponentSafe<ncSpatialQuadtreeCell>();
    ncSpatialSpace * spatialSpace = spatialCell->GetParentSpace();
    if (spatialSpace)
    {
        ncHorizonClass * hc = spatialSpace->GetClassComponentSafe<ncHorizonClass>();
        if (hc && hc->IsValid())
        {
            this->DrawHorizonSegments(gfx, this);
        }
    }
}

//------------------------------------------------------------------------------
void
ncCellHorizon::DrawHorizonSegments(nGfxServer2 * gfx, ncCellHorizon * cell)
{
    ncSpatialQuadtreeCell * spatialCell = this->GetComponentSafe<ncSpatialQuadtreeCell>();
    if (!spatialCell->IsLeaf())
    {
        int numSubCells = spatialCell->GetNumSubcells();
        ncSpatialQuadtreeCell ** subcells = spatialCell->GetSubcells();
        for(int i = 0;i < numSubCells;i++)
        {
            ncCellHorizon * cellHorizon = subcells[i]->GetComponentSafe<ncCellHorizon>();
            cell->DrawHorizonSegments(gfx, cellHorizon);
        }
    }
    else
    {
        cell->DrawHorizonSegments(gfx, North);
        cell->DrawHorizonSegments(gfx, East);
        cell->DrawHorizonSegments(gfx, South);
        cell->DrawHorizonSegments(gfx, West);
    }
}
