#ifndef N_TERRAINLINE_H
#define N_TERRAINLINE_H
/*-----------------------------------------------------------------------------
    @file nterrainline.h
    @ingroup NebulaTerrain

    @author Juan Jose Luna

    @brief A line in 3d wrapped to the terrain. Point are given in 2d but the line
    is 3d, and the height position is taken froma the terrain. The line can also be
    intersected with the terrain to wrap it perfectly onto it (new points are generated)

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nroot.h"
#include "util/nstring.h"
#include "mathlib/nmath.h"
#include "mathlib/bbox.h"
#include "mathlib/triangle.h"
#include "ngeomipmap/nfloatmap.h"
#include "entity/nentity.h"
#include "gfx2/nmesh2.h"
//#include "tools/nmeshbuilder.h" 

class nMeshBuilder;
//------------------------------------------------------------------------------
/**
    @brief A line that is wrapped onto the terrain
*/
struct nTerrainLinePoint
{
    int vertex;
    int face0, edge0;
    int face1, edge1;
    float parameter0, parameter1;

    inline nTerrainLinePoint(int v, int f0 = -1, int e0 = -1, float p0 = 0.0f, int f1 = -1, int e1 = -1, float p1 = 1.0f)
    {
        vertex = v;

        face0 = f0;
        edge0 = e0;
        parameter0 = p0;

        face1 = f1;
        edge1 = e1;
        parameter1 = p1;
    }
};
    
//------------------------------------------------------------------------------
/**
    @brief A line that is wrapped onto the terrain
*/
class nTerrainLine : public nObject
{
public:

    // Polygon mesh ( testing )
    nArray<nMesh2*> debugPolygons;
    nShader2* debugShader;

    // Min. margin between line points and terrain points (terrain square relative)
    static const float lineTerrainMargin;

    /// constructor
    nTerrainLine();
    /// destructor
    ~nTerrainLine();

    /// Set heightmap to work with
    void SetHeightMap( nFloatMap* hmap );

    /// Get current working heightmap
    nFloatMap* GetHeightMap();

    /// Clear the line
    void Clear( void );

    /// Get line's vertex buffer
    vector3* GetVertexBuffer( void ) const;

    /// Get number of vertices
    int GetNumVertices( void ) const;

    /// Get a line point
    nTerrainLinePoint* GetPoint( int i ) const;
    
    /// Get a line point position
    vector3 GetPointPosition( int i ) const;

    /// Add a point
    bool AddPoint( vector2 pos );

    /// Remove a point
    void RemovePoint( int i );

    /// Get if the line is closed
    bool IsClosed( void ) const;

    /// Get if the line is wrapped
    bool IsWrapped( void ) const;

    /// Get if the line is clockwise (value is valid only if the line is closed)
    bool IsCW( void ) const;

    /// Refresh point heights from the heightmap
    void RefreshHeights( void );

    /// Get bounding box of line in terrain points coordinates
    void GetTerrainPointsBB( int& x0, int& z0, int& x1, int& z1 );

    /// Generate a polygon
    bool GeneratePolygon( nMeshBuilder* mb, nString meshPath );

    /// Tell if a 2d point is inside the line
    bool IsPointInside( vector2 p0, int ignoreSegment = -1) const;

    /// Copy a line
    void Copy( nTerrainLine* line );

    /// functionality to persist the object
    bool SaveCmds(nPersistServer* ps);

    /// begin:scripting

    /// set height offset, wich is added always to the points height
    void SetOffset(const float);

    /// get height offset, wich is added always to the points height
    const float GetOffset() const;

    /// set line margin
    void SetLineMargin(const float);

    /// get line margine
    const float GetLineMargin() const;

    /// close the line
    const bool Close();

    /// sets if the line is clockwise
    void SetCW(const bool);

    /// Wrap the line to the terrain (intersect with it and generate segments as needed)
    void Wrap();

    /// Add a point
    void AddPoint(const float,const float);

    /// end:scripting

    /// gets a bounding box in the plane defined by x and z
    void GetBBinXZ( bbox3& bb );

protected:
private:
    // Reference to the heightmap being edited
    nRef<nFloatMap> heightMap; 

    // Array of point positions (simple vector3 array)
    nArray<vector3> positions;

    // Array of points
    nArray<nTerrainLinePoint*> points;

    // Is wrapped? (intersected with the terrain to wrap it perfectly)
    bool wrapped;

    // Is closed? (last point references the first position, closing the line)
    bool closed;

    // Is clockwise? (only valid if closed)
    bool isCW;

    // Offset (constant height offset that is always added to each point)
    float offset;

    // Min. margin between line points (meters)
    float lineMargin;

    // Internal temp polygon info
    struct {
        int x0, z0, x1, z1, nx, nz; // Bounding box, (coords and size)
        int linePointsCount, terrainPointsCount, hmapSize; // number of line and terrain points. Size of the heightmap
        vector3* polygonPoints; // Polygon points buffer
        bool* usedPoints; // Array of used terrain points flags
        nMeshBuilder* mb; // Pointer to meshbuilder

    } polyInfo;

    void GenPolyRecursive(int beginPoint, int pointsLeft, int face);
    void Triangulate( nArray<int>& closedLine );

    /// Wrap a line segment to the heightmap
    int WrapSegment(int p0);

    /// Generate a face segment
    void GetFaceSegment(int face, int segment, vector3& b, vector3& m);

    /// Get next face and segment when crossing from a terrain face through one of its segments
    bool GetNextTerrainFaceSegment(int faceFrom, int segmentFrom, int &faceTo, int &segmentTo);

    /// Tell if two consecutive segments of a closed line can be triangulated
    bool CanTriangulate( int segment, nArray<int>& closedLine, float minAngle = 0.0f );

    /// Tell if a new point can be added to the line (wouldn't make it autointersect)
    bool CanAddPoint( const vector2& newPoint, bool checkFirstSegment = true );

    /// Check if the line is CW or CCW
    void CheckCW( void );

    /// Invert the line points order
    void Invert( void );

    /// Get intersection between 2 line segments in 2d
    static bool Intersect2Segments(const vector2& b0, const vector2& e0, const vector2& b1, const vector2& e1, vector2 &vi, float& t0);

    /// Tell if a point is inside a 2d triangle
    static bool PointInsideTriangle2d(vector2& a, vector2& p0, vector2& p1, vector2& p2);

    /// Get distance from 2d point to 2d segment
    float GetPointSegmentDistance2d(vector2& a, vector2& p0, vector2& p1);
};

//------------------------------------------------------------------------------

#endif N_TERRAINLINE_H
