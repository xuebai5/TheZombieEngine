#ifndef N_NCCELLHORIZON_H
#define N_NCCELLHORIZON_H
/*-----------------------------------------------------------------------------
    @class ncCellHorizon
    @ingroup NebulaSpatialSystem
    @author Mateu Batle Sastre

    @brief horizon information for a cell

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/ncomponentobject.h"

//------------------------------------------------------------------------------
class nGfxServer2;
class nHorizonSegment;

//------------------------------------------------------------------------------
class ncCellHorizon : public nComponentObject
{

    NCOMPONENT_DECLARE(ncCellHorizon, nComponentObject);

public:

    enum Direction
    {
        North = 0,
        East,
        South,
        West
    };

    /// constructor
    ncCellHorizon();

    // get horizon segment information for specified edge of the cell
    nHorizonSegment * GetHorizonSegment(Direction dir) const;

    /// draw all 4 horizon segments for this cell
    void DrawHorizonSegments(nGfxServer2 * gfx);

protected:

    /// destructor
    virtual ~ncCellHorizon();

    /// drawd all segments for a cell (and subcells)
    void DrawHorizonSegments(nGfxServer2 * gfx, ncCellHorizon * cell);
    /// draw an horizon segment
    void DrawHorizonSegments(nGfxServer2 * gfx, Direction dir);

};

//------------------------------------------------------------------------------
#endif // N_NCCELLHORIZON_H
