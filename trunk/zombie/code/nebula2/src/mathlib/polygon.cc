//------------------------------------------------------------------------------
//  polygon.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "mathlib/polygon.h"

#define POLY_EPSILON 0.001f

//------------------------------------------------------------------------------
/**
    Constructor
*/
polygon::polygon()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
polygon::polygon (const nArray<vector3>& points)
{
    this->Set (points);
}


//------------------------------------------------------------------------------
/**
    Constructor
*/
polygon::polygon (const polygon& copy)
{
    this->Set (copy.GetVertices());
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
polygon::~polygon()
{
    points.Clear();
}

//------------------------------------------------------------------------------
/**
    Set 
*/
void 
polygon::Set (const nArray<vector3>& points)
{
    this->points.Clear();

    for ( int i=0; i<points.Size(); i++ )
    {
        this->points.Append (points[i]);
    }
}

//------------------------------------------------------------------------------
/**
    Midpoint
*/
vector3
polygon::Midpoint() const
{
    int iNumVertices = this->GetNumVertices();
    n_assert(iNumVertices);
    vector3 midPoint(0.f, 0.f, 0.f);

    if ( iNumVertices != 0 )
    {
        for ( int i=0; i<iNumVertices; i++ )
        {
            midPoint += this->points[i];
        }

        float fDivisor = 1.f / iNumVertices;
        midPoint *= fDivisor;
    }

    return midPoint;
}

//------------------------------------------------------------------------------
/**
    IsConvex

    @brief Says if the polygon is convex
    @return true is convex, false else
*/
bool 
polygon::IsConvex() const
{
    int iNumVertices = this->GetNumVertices();
    float angleSum = 0.f;

    // Go around the polygon and sum the angle at each vertex
    for ( int i=0; i<iNumVertices; i++ )
    {
        vector3 edge1, edge2;

        edge1 = this->points[(i+iNumVertices-1)%iNumVertices] - this->points[i];
        edge2 = this->points[(i+1)%iNumVertices] - this->points[i];
       
        edge1.norm();
        edge2.norm();

        float dot = edge1.dot (edge2);
        float angle = n_acos (dot);

        angleSum += angle;
    }

    float convexAngleSum = (float) (iNumVertices - 2) * N_PI;
    
    if ( angleSum < convexAngleSum - (float)iNumVertices * POLY_EPSILON )
    {
        return false;
    }        
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    IsPointInside

    @brief Says if a given 3D point is inside the polygon
    @return true if inside, false else
*/
bool 
polygon::IsPointInside (const vector3& point) const
{
    // We must calculate the sum of the angles between the 3D point and every 
    // single vertex. The sum of all angles must be TWO PI if inside, other
    // value else
    float angleSum = this->CalculateAngleSum (point);
   
    if ( angleSum < N_TWOPI + POLY_EPSILON && angleSum > N_TWOPI - POLY_EPSILON )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    IsPointInside

    @brief Says if a given 3D point is inside the polygon
    @return true if inside, false else
*/
bool
polygon::IsPointInside (float x, float z) const
{
    vector3 midpoint = this->Midpoint();
    vector3 point = vector3 (x, midpoint.y, z);
    
    return this->IsPointInside (point);
}

//------------------------------------------------------------------------------
/**
    CalculateAngleSum

    @brief Calculate the sum of all angles from a given 3D point and every vertex    
*/
float 
polygon::CalculateAngleSum (const vector3& point) const
{
    int iNumVertices = this->GetNumVertices();
    vector3 edge1, edge2;
    float angleSum = 0.f;

    for ( int i=0; i<iNumVertices; i++ )
    {
        edge1.x = this->points[i].x - point.x;
        edge1.y = this->points[i].y - point.y;
        edge1.z = this->points[i].z - point.z;
        edge2.x = this->points[(i+1)%iNumVertices].x - point.x;
        edge2.y = this->points[(i+1)%iNumVertices].y - point.y;
        edge2.z = this->points[(i+1)%iNumVertices].z - point.z;
        
        edge1.norm();
        edge2.norm();

        float dot = edge1.dot (edge2);
        float angle = n_acos (dot);

        angleSum += angle;
    }

    return angleSum;
}

//------------------------------------------------------------------------------
/**
    GetBBox

    @brief Calculate the bbox of the polygon
*/
void 
polygon::GetBBox (bbox3* bbox) const
{
    n_assert(bbox);

    if ( bbox )
    {
        int iNumVertices = this->points.Size();

        bbox->begin_extend();

        for ( int i=0; i<iNumVertices; i++ )
        {
            bbox->extend (this->points[i]);
        }
    } // if ( bbox )
}

//------------------------------------------------------------------------------
/**
    Join2Polygon
*/
void
polygon::Join2Polygon (polygon* poly)
{
    n_assert(poly);

    line3 commonEdge;

    if ( poly && ShareEdge (poly, &commonEdge) )
    {
        this->Merge (poly, commonEdge);
        this->SortCounterClock();
        poly->Reset();                          // The creator of this polygon must delete it
    }    
}

//------------------------------------------------------------------------------
/**
    CanMerge
*/
bool 
polygon::CanMerge (const polygon* poly) const
{
    n_assert(poly);

    bool bCanMerge = false;
    line3 commonEdge;
 
    // First, determine if there is a common edge
    if ( poly && ShareEdge (poly, &commonEdge) )
    {
        // We create a new polygon without the common edge
        // if convex, it's valid
        polygon newPolygon (this->points);        
                
        newPolygon.Merge (poly, commonEdge);
        newPolygon.SortCounterClock();
        bCanMerge = newPolygon.IsConvex();
    }

    return bCanMerge;
}

//------------------------------------------------------------------------------
/**
    Merge
*/
void 
polygon::Merge (const polygon* poly, const line3& commonEdge)
{
    n_assert(poly);

    // Insert the oher vertices at the first vertex of the edge
    vector3 insertPoint = commonEdge.start();
    int indexA = this->GetVertexIndex (insertPoint);
    int indexB = poly->GetVertexIndex (insertPoint);
    int iNumVertexPoly = poly->GetNumVertices();

    if ( indexA != -1 && indexB != -1 )
    {
        // Insert new vertexes
        for ( int i=0; i<iNumVertexPoly; i++ )
        {
            int rightVertex = (i + indexB) % iNumVertexPoly;
            vector3 newVertex = poly->GetVertex (rightVertex);

            if ( !newVertex.isequal(commonEdge.start(), POLY_EPSILON) && 
                 !newVertex.isequal(commonEdge.end(), POLY_EPSILON) )
            {
                this->GetVertices().Insert (indexA++, newVertex);
            }
        }
    }
    else
    {
        n_assert2_always( "Common edge found, but impossible to insert new vertexes");
    }

    this->SortCounterClock();
}

//------------------------------------------------------------------------------
/**
    ShareEdge
*/
bool 
polygon::ShareEdge (const polygon* poly, line3* edge) const
{
    n_assert(poly);

    bool bShare = false;

    if ( poly )
    {
        nArray<vector3> commonPoints;
        int iCommonVert = 0;

        // Count the number of common vertexes, more than one says that share edge 
        for ( int i=0; i<this->GetNumVertices() && iCommonVert < 2; i++ )
        {
            // We consider the same point if there is a distance less than POLY_EPSILON
            vector3 vertex = this->points[i];
            
            for ( int j=0; j<poly->GetNumVertices() && iCommonVert < 2; j++ )
            {
                vector3 polyVertex = poly->GetVertex (j);
                vector3 distance = polyVertex - vertex;

                if ( distance.len() < POLY_EPSILON )
                {
                    commonPoints.Append (vertex);
                    iCommonVert++;
                }
            } 
        } // external for 

        if ( iCommonVert > 1 )
        {
            if ( edge )
            {
                edge->set (commonPoints[0], commonPoints[1]);
            }
            
            bShare = true;
        }
    }

    return bShare;
}

//------------------------------------------------------------------------------
/**
    PartialEdge
*/
bool
polygon::PartialEdge (const polygon* poly) const
{
    n_assert(poly);

    // Iterate over all the edges pairs checking for overlapping
    for ( int i(0); i < this->GetNumVertices(); ++i )
    {
        line3 edge1( this->GetVertex(i), this->GetVertex( (i+1) % this->GetNumVertices() ) );
        for ( int j(0); j < poly->GetNumVertices(); ++j )
        {
            line3 edge2( poly->GetVertex(j), poly->GetVertex( (j+1) % poly->GetNumVertices() ) );
            if ( this->GetOverlappingSegment( edge1, edge2 ) )
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    GetVertexIndex
*/
int 
polygon::GetVertexIndex (const vector3& vertex) const
{
    int index = -1;

    for ( int i=0; i<this->GetNumVertices() && index < 0; i++ )
    {
        if ( this->points[i].isequal( vertex, POLY_EPSILON ) )
        {
            index = i;
        }
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    Tells if the polygon has the given edge, even if its vertices are reversed
*/
bool
polygon::HasEdge (const vector3& A, const vector3& B) const
{
    int indexA = this->GetVertexIndex(A);
    int indexB = this->GetVertexIndex(B);
    bool bEdge = false;

    if ( indexA != -1 && indexB != -1 )
    {
        int dif = n_abs( indexA - indexB );
        bEdge = ( dif == 1 ) || ( dif == this->GetNumVertices() - 1 );
    }

    return bEdge;
}

//------------------------------------------------------------------------------
/**
    Tells if the polygon has the given edge, even if its vertices are reversed
*/
bool
polygon::HasEdge( const line3& edge ) const
{
    return this->HasEdge( edge.start(), edge.end() );
}

//------------------------------------------------------------------------------
/**
    SortCounterClock
*/
void
polygon::SortCounterClock (bool keepPoints)
{
    if ( this->GetNumVertices() )
    {
        nArray<vector3> newOrder;

        // Get the point moreleft with minor z
        vector3 start = this->GetDownLeft();
        vector3 next;

        newOrder.Append (start);

        while ( this->GetClosestUp (start, &next, newOrder) )
        {
            newOrder.Append (next);
            start = next;        
        }

        while ( this->GetClosestDown(start, &next, newOrder, keepPoints) )
        {        
            newOrder.Append (next);
            start = next;
        }

        // Substitute the points
        this->Set (newOrder);
    }
}

//------------------------------------------------------------------------------
/**
    GetDownLeft
*/
vector3
polygon::GetDownLeft() const
{    
    vector3 point = this->points[0];

    for ( int i=1; i<this->points.Size(); i++ )
    {
        vector3 vertex = this->points[i];

        if ( vertex.x < point.x )
        {
            point = vertex;
        }
        else if ( vertex.x == point.x && vertex.z < point.z )
        {
            point = vertex;
        }
    }
    
    return point;
}

//------------------------------------------------------------------------------
/**
    GetClosestUp
*/
bool 
polygon::GetClosestUp (const vector3& start, vector3* next, const nArray<vector3>& vertices) const
{
    n_assert (next);

    bool bExist = false;

    if ( next )
    {
        float fMinDistance = FLT_MAX;        

        for ( int i=0; i<this->points.Size(); i++ )
        {
            vector3 vertex = this->points[i];
            bool bValid = true;

            // This vertex must not exist at the list of new vertices
            for ( int j=0; j<vertices.Size() && bValid; j++ )
            {
                vector3 v = vertices[j];
                bValid = !v.isequal (vertex, 0.f);
            }

            if ( bValid && vertex.z >= start.z )
            {
                float fDistance = n_abs (vertex.x - start.x);

                if ( fDistance < fMinDistance )
                {
                    *next = vertex;
                    fMinDistance = fDistance;
                    bExist = true;
                }
            }
        }
    }

    return bExist;
}

//------------------------------------------------------------------------------
/**
    GetClosestDown
*/
bool
polygon::GetClosestDown (const vector3& start, vector3* next, const nArray<vector3>& vertices, bool keepPoints) const
{
    n_assert (next);

    bool bExist = false;

    if ( next )
    {
        float fMaxAngle = -FLT_MAX;

        for ( int i=0; i<this->points.Size(); i++ )
        {
            vector3 vertex = this->points[i];
            bool bValid = true;

            // This vertex must not exist at the list of new vertices
            for ( int j=0; j<vertices.Size() && bValid; j++ )
            {
                vector3 v = vertices[j];
                bValid = !v.isequal (vertex, 0.f);
            }

            if ( vertices.Size() == 0 )
            {
                *next = vertex;
                return true;
            }

            if ( bValid )
            {
                vector3 edge1 = vertices[0] - start;
                vector3 edge2 = vertex - start;

                edge1.norm();
                edge2.norm();

                float dot = edge1.dot (edge2);
                float angle = n_acos (dot);
                vector3 cross = (edge1*edge2);
                bool right = cross.y < 0 || keepPoints; // Always keep the points of the polygon

                if ( angle > fMaxAngle && right)
                {
                    fMaxAngle = angle;
                    *next = vertex;
                    bExist = true;
                }
            }
        } // for
    }

    return bExist;
}

//------------------------------------------------------------------------------
/**
    GetArea
*/
float 
polygon::GetArea() const
{
    int vertices = this->points.Size();
    float area = 0.f;
    
    for ( int i=0; i<vertices; i++ )
    {
        int j = (i+1) % vertices;
        vector3 point1 = this->points[i];
        vector3 point2 = this->points[j];

        area += point1.x*point2.z;
        area -= point2.x*point1.z;
    }

    return n_abs (0.5f*area);
}

//------------------------------------------------------------------------------
/**
    GetWidth
*/
float
polygon::GetWidth() const
{
    // Gets the "min x" and the "max x" and show the differiences between them
    bbox3 bbox;

    this->GetBBox (&bbox);

    return n_abs (bbox.vmax.x - bbox.vmin.x);
}

//------------------------------------------------------------------------------
/**
    GetHeight
*/
float
polygon::GetHeight() const
{
    // Gets the "min z" and the "max z" and show the differiences between them
    bbox3 bbox;

    this->GetBBox (&bbox);

    return n_abs (bbox.vmax.z - bbox.vmin.z);
}

//------------------------------------------------------------------------------
/**
    CleanVertices

    @return true if remove some vertex
*/
bool
polygon::CleanVertices (float range)
{   
    bool change = false;

    for ( int i=0; i<this->points.Size(); ) 
    {
        int iVertices = this->points.Size();
        vector3 A = this->points[(i-1+iVertices)%iVertices];
        vector3 B = this->points[i];
        vector3 C = this->points[(i+1)%iVertices];

        if ( this->AreAligned (A, B, C, range) )
        {
            this->points.Erase (i);
            change = true;
        }
        else
        {
            i++;
        }
    }

    return change;
}

//------------------------------------------------------------------------------
/**
    AreAligned
*/
bool
polygon::AreAligned (const vector3& a, const vector3& b, const vector3& c, float range) const
{
    // Note, only uses 2D, forgotting Y component - fix this
    float det = (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
    return (bool) (det >= -range && det <= range );
}

//------------------------------------------------------------------------------
/**
    Says if two edges are overlapping each other, optionally returning the overlapping segment

    If edges just overlap each other on a single vertex, then their not
    considered to be overlapping.

    To get the overlapping segment you must provide two arrays of two floats.
    The overlapping segment is returned in form of split points: for each edge
    2 points are returned that indicate where the vertices of the second edge
    falls in the first edge. The points are given as the 't' factor of the line
    that the edge represents, where t=0 for edge.start() and t=1 for edge.end().
    Returned split points may be outside the edge, but they are always sorted
    (split_point[0] < split_point[1]).
*/
bool
polygon::GetOverlappingSegment( const line3& edge1, const line3& edge2, float* splitPoints1, float* splitPoints2 ) const
{
    // Get the overlapping factor of the projection of one edge over the other
    float overlap1 = edge1.closestpoint( edge2.start() );
    float overlap2 = edge1.closestpoint( edge2.end() );

    // If there's a plane that separates both edges, then edges don't overlap
    if ( ( overlap1 <= POLY_EPSILON && overlap2 <= POLY_EPSILON ) ||
         ( overlap1 >= 1 - POLY_EPSILON && overlap2 >= 1 - POLY_EPSILON ) )
    {
        return false;
    }

    // Finally, if both edges are over the same line they're overlapping
    if ( ( edge1.point(overlap1).isequal( edge2.start(), POLY_EPSILON ) &&
        edge1.point(overlap2).isequal( edge2.end(), POLY_EPSILON ) ) )
    {
        // Get the splitting points if the user has requested them
        if ( splitPoints1 )
        {
            if ( overlap1 < overlap2 )
            {
                splitPoints1[0] = overlap1;
                splitPoints1[1] = overlap2;
            }
            else
            {
                splitPoints1[0] = overlap2;
                splitPoints1[1] = overlap1;
            }
        }
        if ( splitPoints2 )
        {
            overlap1 = edge2.closestpoint( edge1.start() );
            overlap2 = edge2.closestpoint( edge1.end() );
            if ( overlap1 < overlap2 )
            {
                splitPoints2[0] = overlap1;
                splitPoints2[1] = overlap2;
            }
            else
            {
                splitPoints2[0] = overlap2;
                splitPoints2[1] = overlap1;
            }
        }

        // Edges overlap
        return true;
    }

    // Edges don't overlap
    return false;
}

//------------------------------------------------------------------------------
/**
    Split on both polygons those edges that overlap but whose vertices don't match
*/
void
polygon::SplitOverlappingEdges( polygon& poly )
{
    for ( int i(0); i < this->GetNumVertices(); ++i )
    {
        line3 edge1( this->GetVertex(i), this->GetVertex( (i+1) % this->GetNumVertices() ) );
        for ( int j(0); j < poly.GetNumVertices(); ++j )
        {
            line3 edge2( poly.GetVertex(j), poly.GetVertex( (j+1) % poly.GetNumVertices() ) );

            if ( !this->EdgesMatch( edge1, edge2 ) )
            {
                float splitPoints1[2];
                float splitPoints2[2];
                if ( this->GetOverlappingSegment( edge1, edge2, splitPoints1, splitPoints2 ) )
                {
                    if ( splitPoints1[0] > POLY_EPSILON && splitPoints1[0] < 1 - POLY_EPSILON )
                    {
                        this->InsertVertex( ++i, edge1.ipol(splitPoints1[0]) );
                    }
                    if ( splitPoints1[1] > POLY_EPSILON && splitPoints1[1] < 1 - POLY_EPSILON )
                    {
                        this->InsertVertex( ++i, edge1.ipol(splitPoints1[1]) );
                    }
                    if ( splitPoints2[0] > POLY_EPSILON && splitPoints2[0] < 1 - POLY_EPSILON )
                    {
                        poly.InsertVertex( ++j, edge2.ipol(splitPoints2[0]) );
                    }
                    if ( splitPoints2[1] > POLY_EPSILON && splitPoints2[1] < 1 - POLY_EPSILON )
                    {
                        poly.InsertVertex( ++j, edge2.ipol(splitPoints2[1]) );
                    }

                    // Only one overlapping edge allowed per call
                    return;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Tells if two edges match, even if their vertices are reversed
*/
bool
polygon::EdgesMatch( const line3& edge1, const line3& edge2 )
{
    return ( edge1.start().isequal( edge2.start(), POLY_EPSILON ) &&
             edge1.end().isequal( edge2.end(), POLY_EPSILON ) ) ||
           ( edge1.start().isequal( edge2.end(), POLY_EPSILON ) &&
             edge1.end().isequal( edge2.start(), POLY_EPSILON ) );
}

//------------------------------------------------------------------------------
/**
    RemoveVertex
*/
void
polygon::RemoveVertex (int index)
{
    n_assert(index<this->points.Size());

    if ( index < this->points.Size() )
    {
        this->points.Erase (index);
    }
}

//------------------------------------------------------------------------------
/**
    Copy
*/
void
polygon::Copy (const polygon& original)
{
    this->Reset();
    this->Set (original.GetVertices());
}

//------------------------------------------------------------------------------
/**
    HasVertex
*/
bool
polygon::HasVertex (const vector3& vertex) const
{
    return bool(this->GetVertexIndex (vertex) != -1);
}

//------------------------------------------------------------------------------
/**
    HasVertex
*/
bool
polygon::HasVertex (float x, float z) const
{
    bool found = false;

    for ( int i=0; i<this->points.Size() && !found; i++ )
    {
        vector3 vertex = this->points[i];
        found = bool (vertex.x == x && vertex.z == z);
    }
    
    return found;
}

//------------------------------------------------------------------------------
/**
    GetClosestVertex
*/
vector3 
polygon::GetClosestVertex (float x, float z) const
{
    vector3 midpoint = this->Midpoint();

    return this->GetClosestVertex (vector3(x, midpoint.y, z));
}

//------------------------------------------------------------------------------
/**
    GetClosestVertex   
*/
vector3
polygon::GetClosestVertex (const vector3& point) const
{
    float minDist = (float)INT_MAX;
    vector3 closest;

    for ( int i=0; i<this->points.Size(); i++ )
    {
        vector3 vertex = this->points[i];
        float distance = vector3::distance(point, vertex);

        if ( distance < minDist )
        {
            closest = vertex;
            minDist = distance;
        }
    }

    return closest;
}

//------------------------------------------------------------------------------
/**
    SetVertex  
*/
void
polygon::SetVertex (int index, const vector3& value)
{
    n_assert(index<this->points.Size());

    if ( index < this->points.Size() )
    {
        this->points[index] = value;
    }
}

//------------------------------------------------------------------------------
/**
    Insert a vertex
*/
void
polygon::InsertVertex( int index, const vector3& vertex )
{
    this->points.Insert( index, vertex );
}

//------------------------------------------------------------------------------
/**
    IsAtEdge
*/
bool
polygon::IsAtEdge (float x, float z) const
{
    vector3 midpoint = this->Midpoint();

    return this->IsAtEdge (vector3(x, midpoint.y, z));
}

//------------------------------------------------------------------------------
/**
    IsAtEdge
*/
bool
polygon::IsAtEdge (const vector3& point) const
{
    int iNumVertices = this->points.Size();
    bool edge = false;

    for ( int i=0; i<iNumVertices && !edge; i++ )
    {
        vector3 vertex1, vertex2;

        vertex1 = this->points[i];
        vertex2 = this->points[(i+1)%iNumVertices];

        edge = this->AreAligned (vertex1, point, vertex2, ALIGN_EPSILON);
    }

    return edge;
}

//------------------------------------------------------------------------------
/**
    MakeRectangle
*/
void
polygon::MakeRectacle()
{
    float minArea = FLT_MAX;    
    vector3 center;
    vector3 box[2];
    float long0, long1;
    nArray<vector3> listPoints;

    this->SortCounterClock (false);    

    // Clockwise :)
    for ( int l=this->points.Size()-1; l>=0; l-- )
    {
        vector3 p = this->points[l];

        bool found = false;

        for ( int n=0; n<listPoints.Size() && !found; n++ )
        {
            vector3 element = listPoints[n];
            found = element.isequal (p, FLT_EPSILON);
        }

        if ( !found )
        {
            listPoints.Append (p);
        }
    }    

    int numPoints = listPoints.Size();

    long0 = long1 = 0.f;

    // Loop through all edges; j trails i by 1, modulo numPoints
    for ( int i=0, j=numPoints-1; i<numPoints; j=i, i++ )
    {
        // Get the current edge (e0x, e0y), normalized
        vector3 e0 = listPoints[i] - listPoints[j];

        e0.y = 0.f;
        e0.norm();

        // Get an axis e1 orthogonala to edge e0
        vector3 e1 = vector3 (-e0.z, 0.f, e0.x);

        // Loop through all points to get maximum extents
        float min0 = 0.f, min1 = 0.f, max0 = 0.f, max1 = 0.f;

        for ( int k=0; k<numPoints; k++ )
        {
            // Project points onto axes e0 and e1 and keep track
            // of minimum and maximum values along both axes
            vector3 d = listPoints[k] - listPoints[j];
            d.y = 0.f;
            float dot = d.dot (e0);

            if ( dot < min0 )
            {
                min0 = dot;
            }

            if ( dot > max0 )
            {
                max0 = dot;
            }

            dot = d.dot (e1);
            
            if ( dot < min1 )
            {
                min1 = dot;
            }

            if ( dot > max1 )
            {
                max1 = dot;
            }
        }

        float area = (max0 - min0) * (max1 - min1);

        // if best so far, remember the area, center and axes
        if ( area < minArea )
        {
            vector3 sub0 = e0;
            vector3 sub1 = e1;
            vector3 subTotal;

            minArea = area;               
            sub0 *= (min0 + max0);
            sub1 *= (min1 + max1);
            subTotal = sub0 + sub1;
            subTotal *= 0.5f;

            sub1 *= 0.5f;
            sub0 *= 0.5f;

            center = listPoints[j] + subTotal;
            box[0] = e0;
            box[1] = e1;
            long0 = (min0 + max0);
            long1 = (min1 + max1);
        }
    }

    // Now, transformate this points into the rectangle
    vector3 midpoint = this->Midpoint();
    vector3 v0, v1, v2, v3;
    vector3 dir0, dir1;

    v0 = v1 = v2 = v3 = center;
    dir0 = box[0];
    dir1 = box[1];

    dir0 *= (n_abs(long0)*0.5f);
    dir1 *= (n_abs(long1)*0.5f);

    v0 = center - dir0 - dir1;
    v1 = center - dir0 + dir1;
    v2 = center + dir0 + dir1;
    v3 = center + dir0 - dir1;

    // Copy the heigh for the new rectangle
    v0.y = v1.y = v2.y = v3.y = midpoint.y;

    this->points.Clear();
    this->points.Append (v0);
    this->points.Append (v1);
    this->points.Append (v2);
    this->points.Append (v3);    
}

//------------------------------------------------------------------------------
/**
    @brief Get distance from 2d point to 2d segment
    @param a point to get distance from segment
    @param p0 first point of segment
    @param p1 The other point of the segment
*/
float
polygon::GetDistance2dSegment( const vector3& a, vector3& p0, vector3& p1 ) const
{
    vector3 s = p1 - p0;
    s.norm();
    vector3 r = a - p0;
    float x = r % s;
    vector3 q = r - ( s * x );
    return q.len();
}


//------------------------------------------------------------------------------
/**
    @brief Get distance from 2d point to polygon
    @param a point to get distance from polygon
*/
float
polygon::GetDistance2d( const vector3& a ) const
{
	int i;
	return this->GetDistance2d( a, i );
}
//------------------------------------------------------------------------------
/**
    @brief Get min distance from point to polygon and wich segment is closest
    @param a point to get distance from polygon
	@param segment returned index of closest segment
*/
float
polygon::GetDistance2d( const vector3& a, int& segment ) const
{
	segment = -1;

    if ( this->points.Size() < 2 )
    {
        return 0.0f;
    }

    float minD = -1.0f;
    int numP = this->points.Size();
    for ( int i = 0; i < numP; i++ )
    {
        float d = this->GetDistance2dSegment( a, this->points[i], this->points[(i+1) % numP] );
        if ( minD < 0.0f || d  < minD )
        {
             minD = d;
			 segment = i;
        }
    }

    return minD;
}

//------------------------------------------------------------------------------
/**
    Get the closest point to the given point, assuming a flat XZ polygon
*/
void
polygon::GetClosestPoint2d( const vector3& point, vector3& closestPoint ) const
{
    // If the point is over/under the polygon, just return the projection to it
    if ( this->IsPointInside( point.x, point.z ) )
    {
        n_assert( this->points.Size() > 0 );
        // Flat polygon assumed, so take any vertex.y
        closestPoint = vector3( point.x, this->points[0].y, point.z );
        return;
    }

    // Iterate through all edges looking for the closest point on each one of them
    float closestDistanceSq( FLT_MAX );
    int numPoints( this->points.Size() );
    for ( int i(0); i < numPoints; ++i )
    {
        // Get the closest point to the edge
        line3 edge( this->points[i], this->points[ (i+1) % numPoints ] );
        float p( edge.closestpoint( point ) );
        vector3 edgePoint;
        if ( p <= 0 )
        {
            edgePoint = edge.start();
        }
        else if ( p >= 1 )
        {
            edgePoint = edge.end();
        }
        else
        {
            edgePoint = edge.ipol( p );
        }

        // Keep the edge point if it's the closest one found so far
        float distanceSq( (edgePoint - point).lensquared() );
        if ( distanceSq < closestDistanceSq )
        {
            closestDistanceSq = distanceSq;
            closestPoint = edgePoint;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get a segment's width
    @param segmentIndex index of segment (0..GetNumVertices)
*/
float
polygon::GetSegmentWidth( int segmentIndex ) const
{
    n_assert( segmentIndex >= 0 && segmentIndex < this->points.Size() );
    return ( this->points[ segmentIndex ] - this->points[ ( segmentIndex + 1 ) % this->points.Size() ] ).len();
}
