#ifndef N_POLYGON_H
#define N_POLYGON_H
//-------------------------------------------------------------------
/**
    @class triangle
    @ingroup NebulaMathDataTypes

    defines a polygon in 3d space
*/

#include "util/narray.h"
#include "mathlib/bbox.h"
#include "mathlib/vector.h"

#define ALIGN_EPSILON 0.005f

class polygon 
{
public:
    /// Constructors
    polygon ();
    polygon (const nArray<vector3>& points);
    polygon (const polygon& copy);

    /// Destructor
    ~polygon ();

    /// Reset the polygon
    void Reset();

    /// Set the polygon
    void Set (const nArray<vector3>& points);

    /// Get the midpoint of the polygon
    vector3 Midpoint() const;

    /// Get the number of vertices
    int GetNumVertices() const;

    /// Get a indexed point
    vector3 GetVertex (int index) const;
    
    /// Says if a given point is inside the polygon
    bool IsPointInside (const vector3& point) const;
    bool IsPointInside (float x, float z) const;

    /// Says if the polygon is convex
    bool IsConvex() const;

    /// Sort vertices in counterclock way
    void SortCounterClock (bool keepPoints = true);

    /// Give the bounding box
    void GetBBox (bbox3* bbox) const;

    /// Says if a polygon can merge with this, forming a new convex polygon
    bool CanMerge (const polygon* poly) const;

    /// Merge two polygons
    void Join2Polygon (polygon* poly);

    /// Says if a given polygon shares an edge with this polygon
    bool ShareEdge (const polygon* poly, line3* edge = 0) const;

    /// Says if a given polygon shares part of an edge with this polygon
    bool PartialEdge (const polygon* poly) const;

    /// Tells if the polygon has the given edge, even if its vertices are reversed
    bool HasEdge (const vector3& A, const vector3& B) const;
    bool HasEdge( const line3& edge ) const;

    /// Tells if two edges match, even if their vertices are reversed
    static bool EdgesMatch( const line3& edge1, const line3& edge2 );

    /// Gets the area of the polygon
    float GetArea() const;

    /// Gets the width of the polygon
    float GetWidth() const;

    /// Gets the height of the polygon
    float GetHeight() const;

    /// Removes all aligned vertices 
    bool CleanVertices (float range = ALIGN_EPSILON);

    /// Removes a vertex
    void RemoveVertex (int index);

    /// Copies a polygon
    void Copy (const polygon& original);

    /// Says if the polygon has a given vertex
    bool HasVertex (const vector3& vertex) const;
    bool HasVertex (float x, float z) const;

    /// Says if a point if at one edge of the polygon
    bool IsAtEdge (const vector3& vertex) const;
    bool IsAtEdge (float x, float z) const;

    /// Gets th closest vertex to a given point
    vector3 GetClosestVertex (const vector3& point) const;
    vector3 GetClosestVertex (float x, float z) const;

    /// Get the closest point to the given point, assuming a flat XZ polygon
    void GetClosestPoint2d( const vector3& point, vector3& closestPoint ) const;

    /// Set value for a given vertex
    void SetVertex (int index, const vector3& value);
    /// Insert a vertex
    void InsertVertex( int index, const vector3& vertex );

    /// Says if three points are aligned
    bool AreAligned (const vector3& a, const vector3& b, const vector3& c, float range) const;
    /// Says if two edges are overlapping each other, optionally returning the overlapping segment
    bool GetOverlappingSegment( const line3& edge1, const line3& edge2, float* splitPoints1 = 0, float* splitPoints2 = 0 ) const;
    /// Split on both polygons those edges that overlap but whose vertices don't match
    void SplitOverlappingEdges( polygon& poly );

    /// With the set of points of the polygon create the minimun rectancle possible
    void MakeRectacle();    

    /// Get min distance from point to polygon
    float GetDistance2d( const vector3& a ) const;

	/// Get min distance from point to polygon and wich segment is closest
    float GetDistance2d( const vector3& a, int& segment ) const;

	/// Get vertices read-only
    const nArray<vector3>& GetVerticesReadOnly() const;

    /// Get a segment's width
    float GetSegmentWidth( int segmentIndex ) const;

private:
    /// Gives the sum of the angles between a given 3D point and every vertex
    float CalculateAngleSum (const vector3& point) const;

    /// Find the index where a vertex is
    int GetVertexIndex (const vector3& vertex) const;

    /// Merge with other polygon, forming a new convex polygon
    void Merge (const polygon* poly, const line3& commonEdge);

    /// Get the vertex nearest to the downleft corner
    vector3 GetDownLeft() const;
    bool GetClosestUp (const vector3& start, vector3* next, const nArray<vector3>& vertices) const;
    bool GetClosestDown (const vector3& start, vector3* next, const nArray<vector3>& vertices, bool keepPoints) const;

    /// Get vertices
    nArray<vector3>& GetVertices() const;
    
    /// Get distance from pointto 2d segment
    float GetDistance2dSegment( const vector3& a, vector3& p0, vector3& p1) const;

    nArray<vector3> points;     /// Number of points of the polygon
};

//------------------------------------------------------------------------------
/**
    Reset
*/
inline
void
polygon::Reset()
{
    this->points.Clear();
}

//------------------------------------------------------------------------------
/**
    GetNumVertixes
*/
inline
int
polygon::GetNumVertices() const
{
    return this->points.Size();
}

//------------------------------------------------------------------------------
/**
    GetVertex
*/
inline
vector3
polygon::GetVertex (int index) const
{
    return this->points[index];
}

//------------------------------------------------------------------------------
/**
    GetVertices
*/
inline
nArray<vector3>&
polygon::GetVertices() const
{
    return const_cast<nArray<vector3>&>(this->points);
}

//------------------------------------------------------------------------------
/**
    GetVertices
*/
inline
const nArray<vector3>&
polygon::GetVerticesReadOnly() const
{
    return const_cast<nArray<vector3>&>(this->points);
}

#endif