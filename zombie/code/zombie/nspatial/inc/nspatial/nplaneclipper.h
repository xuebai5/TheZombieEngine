#ifndef N_PLANECLIPPER_H
#define N_PLANECLIPPER_H

/**
    @class nPlaneClipper
    @ingroup NebulaSpatialSystem
    @brief Encapsulates a set of clipping planes, handling
    of 'active clipping plane' flags for recursive traversal of
    spatial data structures.

    This is just a simple plane clipping class, allowing you to represent various
    primitives such as view frustums, shadow silhouettes, and convex hulls.  Objects
    are tested against the positive side of the plane; if the object is on the negative
    side of any plane, it is definitively outside the region and rejected.  
    Thus the class implements a test where all the
    planes face inward; if you want to tell if the object is outside of a region you'll
    have to reverse the sense of the flags returned.

    Objects inside the region always return a true test result, and objects outside the
    region always return a false test result.  Objects straddling a boundary will
    return the test result of the flags that you passed in.  In this manner, you get two
    sorts of tests depending on what you pass in:
    -If you pass in @c VisitorFlags(false) you will only get back a true TestResult if the
       object is completely within the region.
    -If you pass in a @c VisitorFlags(true) you will only get back a false TestResult if the
       object is completely outside the region.
    These two possibilities have different uses depending on whether or not you're
    determining potentially visible objects or completely obscured objects.

    Also included is the concept of 'active planes' for recursive object testing 
    (i.e., octrees and quadtrees).  When a given tree node is determined to be fully on the
    positive side of a given plane X, you know all the child objects will be fully
    on the positive side of plane X as well, so there is no need to test any child objects
    against plane X since you already know the test will pass.  
    When there are no active planes left you know the
    object and all its children are fully determined, so you can
    trivially add them all without doing any more clipping checks.

    (C) 2006 Conjurer Services, S.A.
*/
#include "mathlib/plane.h"
#include "mathlib/sphere.h"
#include "mathlib/bbox.h"
#include "mathlib/cylinder.h"

/** VisitorFlags helper struct.  

    This class contains a boolean flag, for specifying that a primitive is
    visible or contained or whatever.  There is also a "antitest" flag that says an object is
    certainly NOT visible or contained, for when you want that information.
    
    Also within the class is a bunch of bits that be used by the visitor
    for transferring state between calls to the visitor. Using the view frustum visitor as an example, if
    you call VisibilityTest() for a bounding box and that bbox is found to be totally on the 'inside' 
    of one of the frustum sides, the bit flags for that side will be turned off in the returned VisitorFlags
    value.  For nested data structures, you can use this updated VisitorFlags when doing VisibilityTest()
    calls on enclosed nodes; because of the bit flags, these child nodes won't require the full set of
    plane tests.

    You can of course ignore all this for simplicity and always pass in the default constructed VisitorFlags.
    The visitor should still work and return the exact same set of elements, it will just be a bit slower
    because it is doing some redundant tests.  You may also want to avoid the whole bitflag thing when using
    simple hierarchy elements or debugging scaffolding elements.

    The exact meaning of the activeflags varies with the visitor, so don't try to take the VisitorFlags
    result from one visitor and use it in another visitor...

    'clippingPlane' is the index of the plane that produced 'antitest = true'. In other words, is the 
    plane that culled the tested object. It should be -1 while 'antitest' is false, and greater than one
    when 'antitest' is true.
*/
struct VisitorFlags
{
    /// by default test=true, antitest=false, such that objects are rejected only if they are known not to be visible
    VisitorFlags(bool c=true, bool d=false, unsigned short initflags=~0) : 
                 test(c), antitest(d), activeflags(initflags), clippingPlane(-1) {}
    VisitorFlags(const VisitorFlags &copyme) : 
                 test(copyme.test), 
                 antitest(copyme.antitest), 
                 activeflags(copyme.activeflags), 
                 clippingPlane(copyme.clippingPlane) {}

    bool TestResult() const { return test; }
    bool AntiTestResult() const { return antitest; }

    bool test, antitest;
    unsigned short activeflags;
    int clippingPlane;

    enum { MAXVISITORFLAGS = 15 };
};

// this is currently limited to leave some bitflags available in the VisitorFlags struct
// NOTE: keep this up-to-date, so visitors can add flags on the higher bits
#define N_PLANECLIPPER_MAXPLANES 16

struct ClipVertexInfo
{
    bool clipped;  // is the vertex clipped?
    int planes[N_PLANECLIPPER_MAXPLANES]; // if the vertex is clipped, by which planes
    int numPlanes; // how many clipping planes
    unsigned short planesMask;
};

class nPlaneClipper {
public:
    /// A default constructed clipper will always return true for it clipping test.
    nPlaneClipper();
    /// copy constructor
    nPlaneClipper(const nPlaneClipper &copyme);
    /// Construct the clipper with a set of planes.
    nPlaneClipper(const plane *clipplanes, int numplanes);

    /// copy another plane clipper
    virtual void Copy(const nPlaneClipper &copyme);

    /// Test a bounding box against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    void TestBBox(const bbox3 &boxtest, const VisitorFlags &in, VisitorFlags &out) const;

    /// Test a bounding box against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    void TestBBox(const vector3 &bboxCenter, const vector3 &bboxExtents, const VisitorFlags & in, VisitorFlags & out) const;

    /// Test a cylinder against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    VisitorFlags TestCylinderQuick(const cylinder& cyl, VisitorFlags in) const;

    /// Test a bounding sphere against the planes.  Returns true if the geometry is 
    /// completely contained within the region surrounded by the planes.  Returns
    /// false if the geometry is definitively outside the region.  Returns whatever
    /// TestResult() state you passed in if the geometry is not definitively inside or outside of
    /// the region, possibly with some of the active_flags turned off.
    VisitorFlags TestSphere(const sphere &spheretest, VisitorFlags in) const;

    /// Test a point against the planes.  Returns true if the geometry is 
    /// completely or partially contained within the region surrounded by the planes.
    void TestPoint(const vector3 &pointtest, const VisitorFlags &ri, VisitorFlags &out) const;

    /// Test a set of points that form a portal against the planes. Returns true if all the points are
    /// completely or partially contained within the region sorrounded by the planes. 
    /// It fills 'vertsInfo' with the clipping information for each point (clipped or not, by which
    /// planes, how many planes, and a mask of that planes.
    VisitorFlags TestPortalVertices(const vector3 *points, const int numPoints, 
                                    VisitorFlags in, struct ClipVertexInfo *vertsInfo) const;

    /// Get the planes
    const plane *GetPlanes() const;
    /// Get the number of planes
    int GetNumPlanes() const;
    /// Append a plane
    void Append(const plane &p);

protected:
    plane m_planes[N_PLANECLIPPER_MAXPLANES];
    int m_numplanes;
};

//------------------------------------------------------------------------------
/**
*/
inline
nPlaneClipper::nPlaneClipper():
m_numplanes(0)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
nPlaneClipper::nPlaneClipper(const nPlaneClipper &copyme)
{
    this->Copy(copyme);
}

//------------------------------------------------------------------------------
/**
*/
inline
nPlaneClipper::nPlaneClipper(const plane *clipplanes, int numplanes) :
m_numplanes(numplanes)
{
    // initialize from user-supplied planes
    for (int i=0; i < numplanes; ++i)
    {
        this->m_planes[i] = clipplanes[i];
    }
}

//------------------------------------------------------------------------------
/**
    copy another plane clipper
*/
inline
void
nPlaneClipper::Copy(const nPlaneClipper &copyme)
{
    this->m_numplanes = copyme.m_numplanes;

    for (int i(0); i < copyme.m_numplanes; ++i)
    {
        this->m_planes[i] = copyme.m_planes[i];
    }
}

//------------------------------------------------------------------------------
/**
    Returns a visibility code indicating the culling status of the
    given axis-aligned box.  The result_info passed in should indicate
    which planes might cull the box, by setting the corresponding
    bit in in.active_planes.
    If the box is completely culled, in.clippingPlane is set to the index of the 
    plane that culled the box.
*/
inline
void
nPlaneClipper::TestBBox(const bbox3 &boxtest, const VisitorFlags &in, VisitorFlags &out) const
{
    return this->TestBBox(boxtest.center(), boxtest.extents(), in, out);
}

//------------------------------------------------------------------------------
/**
    Returns a visibility code indicating the culling status of the
    given axis-aligned box (center and extents).  The result_info passed 
    in should indicate which planes might cull the box, by setting the 
    corresponding bit in in.active_planes.
    If the box is completely culled, in.clippingPlane is set to the index of the 
    plane that culled the box.
*/
inline
void 
nPlaneClipper::TestBBox(const vector3 &bboxCenter, const vector3 &bboxExtents, const VisitorFlags & in, VisitorFlags & out) const
{
    // Check the box against each active frustum plane.
    int bit = 1;
    int allplanebits( (1<<m_numplanes) - 1 );
    out.activeflags = in.activeflags;
    for (int i = 0; i < m_numplanes; ++i, bit <<= 1)
    {
        // do the test only if the specified plane is active
        if ((bit & out.activeflags) != 0)
        {
            const plane&    p       = this->m_planes[i];
                        
            // Check box against this plane.
            float   d = p.distance(bboxCenter); //Calculate closest distance from center point to plane.
            float   extent_toward_plane = n_abs(bboxExtents.x * p.a) +
                                          n_abs(bboxExtents.y * p.b) +
                                          n_abs(bboxExtents.z * p.c);
            if (d < 0)
            {  
                if (-d > extent_toward_plane)
                {
                    // Box is definitively on the negative side of the plane, so it's culled
                    out.test = false;
                    out.antitest = true;
                    out.activeflags = 0;
                    out.clippingPlane = i;
                    return;
                } // else this plane is ambiguous so leave it active.
            } 
            else 
            {
                if (d > extent_toward_plane)
                {
                    // Box is accepted by this plane, so
                    // deactivate it, since neither this
                    // box or any contained part of it can
                    // ever be culled by this plane.
                    out.activeflags &= ~bit;
                    if ( (out.activeflags & allplanebits) == 0)
                    {
                        // This box is definitively inside all the culling
                        // planes, so there's no need to continue.
                        out.test = true;
                        out.antitest = false;
                        out.activeflags = 0;
                        out.clippingPlane = i;
                        return;
                    }
                } // else this plane is ambigious so leave it active.
            }
        }
    }

    // Box not definitively culled.  Return updated active plane flags.
    out.test = in.test;
    out.antitest = in.antitest;
    out.clippingPlane = -1;
}

//------------------------------------------------------------------------------
/**
    Returns a visibility code indicating the culling status of the
    given cylinder. The result_info passed in should indicate
    which planes might cull the cylinder, by setting the corresponding
    bit in in.active_planes.
    If the cylinder is completely culled, in.clippingPlane is set to the 
    index of the plane that culled the cylinder.

    This algorithm is not exact, but it is conservative. This means that some 
    cylinders that should be culled will not. But is slightly faster than a 
    exact determination.
*/
inline
VisitorFlags 
nPlaneClipper::TestCylinderQuick(const cylinder& cyl, VisitorFlags in) const
{
    // initialize the clipping plane
    in.clippingPlane = -1;

    vector3 p1(cyl.p1);
    vector3 p2(cyl.p2);

    bool interior1 = false;
    bool interior2 = false;

    // Check the box against each active frustum plane.
    int bit = 1;
    int allplanebits( (1<<m_numplanes) - 1 );
    for (int i = 0; i < m_numplanes; ++i, bit <<= 1)
    {
        // do the test only if the specified plane is active
        if ((bit & in.activeflags) != 0)
        {
            const plane& p = this->m_planes[i];

            // Check cylinder against this plane.
            float   d1 = p.distance(p1); //Calculate closest distance from p1 to plane.
            float   d2 = p.distance(p2); //Calculate closest distance from p2 to plane.

            interior1 = ((d1 + cyl.r) >= 0);
            interior2 = ((d2 + cyl.r) >= 0);

            if (!interior1)
            {
                if (!interior2)
                {
                    // the cylinder is definitively outside the frustum
                    VisitorFlags resultFlags(false, true);
                    resultFlags.clippingPlane = i;
                    return resultFlags;
                }
                
                // p1 was outside, so move it to the plane
                line3 l(p1, p2);
                float t;
                if (p.intersect(l, t))
                {
                    // set p1 the intersection point with the plane
                    p1 = l.b + (l.m * t);
                }
            }
            else if (!interior2)
            {
                // p2 was outside, so move it to the plane
                line3 l(p2, p1);
                float t;
                if (p.intersect(l, t))
                {
                    // set p1 the intersection point with the plane
                    p2 = l.b + (l.m * t);
                }
            }

            // Cylinder is accepted by this plane, so
            // deactivate it, since neither this
            // cylinder or any contained part of it can
            // ever be culled by this plane.
            in.activeflags &= ~bit;
            if ( (in.activeflags & allplanebits) == 0)
            {
                // This box is definitively inside all the culling
                // planes, so there's no need to continue.
                return VisitorFlags(true,false);
            }
        }
    }

    return in;  // Cylinder is not definitively culled.  Return updated active plane flags.
}

//------------------------------------------------------------------------------
/**
    Returns a visibility code indicating the culling status of the
    given sphere.  The result_info passed in should indicate
    which planes might cull the sphere, by setting the corresponding
    bit in in.active_planes.
    If the sphere is completely culled, in.clippingPlane is set to the index of the 
    plane that culled the sphere.
*/
inline
VisitorFlags 
nPlaneClipper::TestSphere(const sphere &spheretest, VisitorFlags in) const
{
    // initialize the clipping plane
    in.clippingPlane = -1;

    // Check the box against each active frustum plane.
    int bit = 1;
    int allplanebits( (1<<m_numplanes) - 1 );
    for (int i = 0; i < m_numplanes; ++i, bit <<= 1)
    {
        // do the test only if the specified plane is active
        if ((bit & in.activeflags) != 0)
        {
            const plane&    p       = this->m_planes[i];
                        
            // Check sphere against this plane.
            float   d = p.distance(spheretest.p); //Calculate closest distance from center point to plane.
            
            if (d < 0)
            {  
                if (-d > spheretest.r)
                {
                    // Sphere is definitively on the negative side of the plane, so it's culled
                    VisitorFlags resultFlags(false, true);
                    resultFlags.clippingPlane = i;
                    return resultFlags;
                } // else this plane is ambiguous so leave it active.
            } 
            else 
            {
                if (d > spheretest.r)
                {
                    // Sphere is accepted by this plane, so
                    // deactivate it, since neither this
                    // sphere or any contained part of it can
                    // ever be culled by this plane.
                    in.activeflags &= ~bit;
                    if ( (in.activeflags & allplanebits) == 0)
                    {
                        // This sphere is definitively inside all the culling
                        // planes, so there's no need to continue.
                        return VisitorFlags(true,false);
                    }
                } // else this plane is ambigious so leave it active.
            }
        }
    }

    return in;  // Sphere not definitively culled.  Return updated active plane flags.
}

//------------------------------------------------------------------------------
/**
    @todo miquelangel.rujula improve it
*/
inline
void
nPlaneClipper::TestPoint(const vector3 &pointtest, const VisitorFlags &ri, VisitorFlags &out) const
{
    // build a bbox from the point and test that
    bbox3 mybbox(pointtest, vector3(0,0,0));
    return TestBBox(mybbox, ri, out);
}

//------------------------------------------------------------------------------
/**
    Test a set of points forming a portal.
*/
inline
VisitorFlags 
nPlaneClipper::TestPortalVertices(const vector3 *points, 
                                  const int numPoints, 
                                  VisitorFlags in, 
                                  struct ClipVertexInfo *vertsInfo) const
{
    int andFlags = 0xffff;
    int orFlags  = 0;

    n_assert(numPoints == 4);

    for (int i = 0; i < numPoints; ++i)
    {
        vertsInfo[i].clipped = false;
        vertsInfo[i].planesMask = 0;
        vertsInfo[i].numPlanes = 0;

        // Check the point against each active frustum plane.
        int bit = 1;
        for (int j = 0; j < this->m_numplanes; ++j, bit <<= 1)
        {
            // do the test only if the specified plane is active
            if ((bit & in.activeflags) != 0)
            {
                float d = this->m_planes[j].distance(points[i]);
                if (d < 0)
                {  
                    // the point is clipped by 'j' plane
                    vertsInfo[i].clipped = true;
                    vertsInfo[i].planes[vertsInfo[i].numPlanes] = j;
                    ++(vertsInfo[i].numPlanes);
                    vertsInfo[i].planesMask |= (1<<j);
                } 
            }
        }

        andFlags &= vertsInfo[i].planesMask;
        orFlags  |= vertsInfo[i].planesMask;
    }

    if (0 == orFlags)
    {
        // all points are inside
        return VisitorFlags(true, false);
    }
    else if (0 != andFlags)
    {
        // all points are outside
        return VisitorFlags(false, true);
    }
    else
    {
        // some points are inside and some outside
        in.test = false;
        in.antitest = false;

        return in;
    }
}

//------------------------------------------------------------------------------
/**
    Get the planes
*/
inline
const plane *
nPlaneClipper::GetPlanes() const
{
    return this->m_planes;
}

//------------------------------------------------------------------------------
/**
    Get the number of planes
*/
inline
int 
nPlaneClipper::GetNumPlanes() const
{
    return this->m_numplanes;
}

//------------------------------------------------------------------------------
/**
    Append a plane
*/
inline
void 
nPlaneClipper::Append(const plane &p)
{
    this->m_planes[this->m_numplanes] = p;
    ++this->m_numplanes;
}

#endif
