#ifndef N_VISIBILITYVISITOR_H
#define N_VISIBILITYVISITOR_H
//------------------------------------------------------------------------------
/**
    @class nVisibilityVisitor
    @ingroup NSpatialContribModule
    @brief Base class for visitors that collect all elements in a given region and visible from a given viewpoint.

    Various subclasses of this abstract class are used for specific types
    of visibility culling.  To add a new type of visibility visitor:

    1. Subclass this class.  Add in stacks for any data that depends on
    the coordinate system.

    2. override the Accept(nSpatialSector *visitee, int recursedepth)
    method to correctly cull the octree for that sector.  Invoke Visit()
    for each element you find visible.

    3. To make hierarchy objects work, override VisibilityTest(...) to
    perform a proper test for the given element.

    4. To handle sectors with different coordinate systems, override
    the Enter/LeaveLocalSpace methods, such that you transform into
    the sectors space and push the new state onto your stacks.

    5. In your class or a subclass, override Accept(nSpatialElement *, int)
    to process each visible element, either by accumulating the visible
    elements in a list, sending them to a render system, or whatever.

    (C) 2006 Conjurer Services, S.A.
*/

#include "mathlib/vector.h"
#include "mathlib/matrix.h"
//#include "nspatial/nspatialoctreespace.h"
//#include "nspatial/nspatialquadtreespace.h"
//#include "nspatial/nspatialindoorspace.h"

#include "util/narray.h"

//class nSpatialElement;
//class nSpatialCamera;
class nGfxServer2;
class nEntityObject;
class ncSpatialPortal;


class nVisibilityVisitor 
{
public:
    
    nVisibilityVisitor();
    nVisibilityVisitor(const vector3 &viewpoint);
    virtual ~nVisibilityVisitor();

    /** Reset any state use during a visibility sweep.
        If you use a visitor multiple times, reset it between each visibility collection 
    */
    virtual void Reset();

    /** process a space. Override this to do your special visibility culling
        This function will Visit() for each spatial element or cell it decides is visible in the space 
    */
    virtual void Visit(nSpatialSpace *visitee, int recursedepth);

    /** process an octree space. Override this to do your special visibility culling
        This function will Visit() for each spatial element or cell it decides is visible in the space 
    */
    virtual void Visit(nSpatialCell *visitee, int recursedepth);

    ///// process a spatial element after it has been determined to be visible.
    //virtual void Visit(nSpatialElement *visitee, int recursedepth);
    ///// process for lights
    //virtual void Visit(nSpatialLight *visitee, nSpatialIndoorCell* cell, int recursedepth);
    ///// process for cameras
    //virtual void Visit(nSpatialCamera *visitee, int recursedepth);

    /// process for entities
    virtual void Visit(nEntityObject *visitee, int recursedepth);
    /// process for spatial portal components
    //virtual void Visit(ncSpatialPortal *visitee, int recursedepth);

    /// process for octree spaces and octree cells
    //virtual void Visit(nSpatialOctreeSpace *visitee, int recursedepth);
    virtual void Visit(neOctreeSpace *visitee, int recursedepth);
    virtual void Visit(nSpatialOctreeCell  *visitee, int recursedepth);

    /// process for quadtree spaces and quadtree cells
    //virtual void Visit(nSpatialQuadtreeSpace *visitee, int recursedepth);
    virtual void Visit(neOutdoor *visitee, int recursedepth);
    virtual void Visit(nSpatialQuadtreeCell  *visitee, int recursedepth);

    /// process for indoor spaces and indoor cells
    virtual void Visit(nSpatialIndoorSpace *visitee, int recursedepth);
    virtual void Visit(nSpatialIndoorCell  *visitee, int recursedepth);

    /** Check if an element should be culled.

        Returns true if the element is (possibly) visible, false if it is not
    visible.

        Mainly used by hierarchy elements. */
    //virtual bool VisibilityTest(nSpatialElement *visitee);

    /** @brief get the current viewpoint in local sector space
        Needed for occlusion or LOD testing */
    vector3 &GetViewPoint() const;

    /** @brief Specify gfxserver to use for debug visualization.
        Comes in handy for debugging wierd occlusion artifacts. You can turn off
        debug visualization by setting this value to NULL (the default) */
    void VisualizeDebug(nGfxServer2 *gfx);

protected:
    /// we have a stack of viewpoints to handle sector-local coordinate systems
    nArray<vector3> m_viewpointstack;
    nGfxServer2 *m_gfxdebug;

    /**
       entering a new local space; the matrix given will transform from
       the current local system into the new space local coordinate system.
       This is used to possibly update a transform matrix or to transform
       the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();
};

inline void nVisibilityVisitor::VisualizeDebug(nGfxServer2 *gfx) { m_gfxdebug = gfx; }

#endif
