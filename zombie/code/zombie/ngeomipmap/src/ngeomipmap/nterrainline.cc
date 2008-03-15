#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  nheightmaptools_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngeomipmap/nterrainline.h"
#include "kernel/nclass.h"
#include "kernel/nfileserver2.h"
#include "tools/nbmpfile.h"
#include "kernel/nfile.h"
#include "ngeomipmap/nterrainline.h"
#include "gfx2/nMesh2.h"
#include "gfx2/nShader2.h"
#include "tools/nmeshbuilder.h"
#include "nspatial/nspatialserver.h"
#include "nphysics/ncphyterrain.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nTerrainLine, "nobject");
//------------------------------------------------------------------------------
// Some handy macros
#define v2d( a ) vector2( a.x, a.z )
#define vecrot90( a ) vector2(-a.y, a.x)

const float nTerrainLine::lineTerrainMargin = 0.001f;

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nTerrainLine::nTerrainLine( ) :
    debugShader(NULL),
    wrapped( false ),
    closed( false ),
    offset( 0.0f ),
    lineMargin( 0.1f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nTerrainLine::~nTerrainLine()
{
    this->Clear();
}
//------------------------------------------------------------------------------
/**
    Set current working heightmap
    @param hmap The heightmap to work with
*/
void
nTerrainLine::SetHeightMap( nFloatMap* hmap )
{
    n_assert(hmap);
    heightMap.set(hmap);
}
//------------------------------------------------------------------------------
/**
    Get current working heightmap
    @return The current working heightmap, or 0 if not set
*/
nFloatMap*
nTerrainLine::GetHeightMap()
{    
    if ( ! heightMap.isvalid() )
    {
        this->heightMap = nSpatialServer::Instance()->GetOutdoorEntity()->GetComponent<ncPhyTerrain>()->GetHeightMapInfo();
        if( !this->heightMap )
            return 0;
    }

    return heightMap.get();
}
//------------------------------------------------------------------------------
/**
    Clear the line (erase all points)
*/
void
nTerrainLine::Clear( void )
{
    int np = points.Size();
    for (int i = 0; i < np; i++ )
    {
        n_delete( points[i] );
    }
    points.Reset();

    positions.Reset();

    closed = false;
    wrapped = false;
}
//------------------------------------------------------------------------------
/**
    Get line's vertex buffer
    @return Vertex buffer, or NULL if the line has no points
*/
vector3*
nTerrainLine::GetVertexBuffer( void ) const
{
    if ( positions.Size() > 0 )
    {
        return &positions[0];
    }
    else
    {
        return NULL;
    }
}
//------------------------------------------------------------------------------
/**
    Get number of vertex positions. If the line is closed, this function doesn't count the last,
    repeated point position.
    @return Number of vertex positions,
*/
int
nTerrainLine::GetNumVertices( void ) const
{
    return positions.Size();
}
//------------------------------------------------------------------------------
/**
    Get a line point
    @param i Point index
    @return A pointer to the point

    Note: If the line is closed, it has one more point that GetNumVertices(),
    but doesn't have one more vertex (position).
*/
nTerrainLinePoint*
nTerrainLine::GetPoint( int i ) const
{
    n_assert( i >= 0 && i < points.Size() );
    return points[i];
}
//------------------------------------------------------------------------------
/**
    Get a line point position
    @param i Point index ( 0 <= i < GetNumVertices() )
    @return The point position
*/
vector3
nTerrainLine::GetPointPosition( int i ) const
{
    n_assert( i >= 0 && i < positions.Size() );
    return positions[i];
}
//------------------------------------------------------------------------------
/**
    Add a point to the end of the line
    @param pos The position of the point in LC
    @return True if the point was added. Otherwise, the specified 2d point lies outside of the heightmap, or it is
    too close to the heightmap grid, or the new edge would make the line autointersect.

    Can't if it is already closed
*/
bool
nTerrainLine::AddPoint( vector2 pos )
{
    if ( closed )
    {
        return false;
    }

    n_assert( this->GetHeightMap() );

    float h;
    vector3 normal;
    if ( this->GetHeightMap()->GetHeightNormal( pos.x, pos.y, h, normal ) && this->CanAddPoint( pos ) )
    {
        positions.Append( vector3( pos.x, h + this->offset, pos.y) );
        points.Append( n_new( nTerrainLinePoint( positions.Size() - 1 ) ) );
    }
    else
    {
        return false;
    }

    if ( wrapped && points.Size() > 1 )
    {
        WrapSegment( points.Size() - 2 );
    }

    return true;
}
//------------------------------------------------------------------------------
/**
    Remove a point. If the point is the last one and the line is closed, the line becomes open again.
    @param i Point index (0 <= i < number of points)
    
    Note: If the line is closed, it has one more point that GetNumVertices(),
    but doesn't have one more vertex (position).

    --- not debugged (bugs?)
*/
void
nTerrainLine::RemovePoint( int i )
{
    n_assert( i >= 0 && i < points.Size() );

    points.Erase( i );

    if ( closed && i == points.Size() )
    {
        closed = false;
    }
    else
    {
        positions.Erase( i );

        for ( int j = i; j < points.Size(); j++ )
        {
            points[ i ]->vertex --;
        }
    }

}
//------------------------------------------------------------------------------
/**
    Wrap the line to the terrain (intersect with it in 2d and generate segments as needed)
*/
void
nTerrainLine::Wrap()
{
    if ( this->wrapped )
    {
        return;
    }
    n_assert( this->GetHeightMap() );

    int addedPoints(0);
    for ( int i(0); i < points.Size() - 1; i+= addedPoints + 1 )
    {
        addedPoints = WrapSegment( i );

        // -- debugging loop
        for (int j(0);j<points.Size();++j){
            float f(positions[points[j]->vertex].y);
            if (f < 0.0f  || f > this->GetHeightMap()->GetHeightScale())
            {
                break;
            }
        }
        // --

    }
    wrapped = true;
}
//------------------------------------------------------------------------------
/**
    Get if the line is closed
    @return True if the line is closed
*/
bool
nTerrainLine::IsClosed( void ) const
{
    return closed;
}
//------------------------------------------------------------------------------
/**
    Get if the line is wrapped
    @return True if the line is wrapped
*/
bool
nTerrainLine::IsWrapped( void ) const
{
    return wrapped;
}
//------------------------------------------------------------------------------
/**
    Get if the line is clockwise
    @return Get if the line is clockwise

    If the line is not closed the returned value is always false
*/
bool
nTerrainLine::IsCW( void ) const
{
    if ( !closed )
    {
        return false;
    }
    return isCW;
}
//------------------------------------------------------------------------------
/**
    Close the line
    @return True if the line was closed (can't if it is already or has less than three points)
*/
const bool
nTerrainLine::Close()
{
    n_assert( this->GetHeightMap() );

    if ( closed || points.Size() < 3 )
    {
        return false;
    }

    if ( ! CanAddPoint( v2d(this->positions[0]), false ) )
    {
        return false;
    }

    points.Append( n_new( nTerrainLinePoint( 0 ) ) );

    closed = true;

    if ( this->wrapped )
    {
        WrapSegment( points.Size() - 2 );
    }

    CheckCW();

    return true;
}
//------------------------------------------------------------------------------
/**
    Refresh point heights from the heightmap
*/
void
nTerrainLine::RefreshHeights( void )
{
    n_assert( this->GetHeightMap() );

    for (int i=0; i < positions.Size(); i++)
    {
        float h;
        vector3 normal;
        this->GetHeightMap()->GetHeightNormal( positions[i].x, positions[i].z, h, normal );
        positions[i].y = h + this->offset;
    }
}
//------------------------------------------------------------------------------
/**
    Copy a line
    @param line The line to copy data into this one.
*/
void
nTerrainLine::Copy( nTerrainLine* /*line*/ )
{
    n_assert2_always( "Not implemented yet." );

    this->Clear();

/**    @todo (when this function is needed): copy points and pos
        closed, wrapped
        check cw if closed
*/
}
//------------------------------------------------------------------------------
/**
    Set height offset, wich is added always to the points height
    @param offset Offset
*/
void
nTerrainLine::SetOffset( const float offset )
{
    this->offset = offset;
}
//------------------------------------------------------------------------------
/**
    Get height offset, wich is added always to the points height
    @return The current offset
*/
const float
nTerrainLine::GetOffset() const
{
    return this->offset;
}

//------------------------------------------------------------------------------
/**
    Set line margin
    @param margin lineMargin
*/
void
nTerrainLine::SetLineMargin( const float margin )
{
    this->lineMargin = margin;
}

//------------------------------------------------------------------------------
/**
    Get line margin
    @return The current line margin
*/
const float
nTerrainLine::GetLineMargin() const
{
    return this->lineMargin;
}

//------------------------------------------------------------------------------
/**
    @brief Get terrain points bounding box containing the line
    @param x0
    If x1 or z1 are -1 on return, the line doesn't intersect with the heightmap
*/
void
nTerrainLine::GetTerrainPointsBB( int& x0, int& z0, int& x1, int& z1 )
{
    int hmapSize = this->GetHeightMap()->GetSize();

    // Get terrain points bounding box containing the line
    x0 = hmapSize + 1;
    z0 = x0;
    x1 = -1;
    z1 = -1;
    for ( int i = 0; i < this->points.Size() - 1; i++ )
    {
        int c = points[ i ]->face0;
        if ( c == -1 ) 
        {
            continue;
        }
        c = c >> 1;
        int x = c % ( hmapSize - 1 );
        int z = c / ( hmapSize - 1 );
        if ( x < x0 ) {
            x0 = x;
        }
        if ( z < z0 ) {
            z0 = z;
        }
        if ( x > x1 ) {
            x1 = x;
        }
        if ( z > z1 ) {
            z1 = z;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Generate a polygon
    @param mb Meshbuilder for creating the polygon
    @return Success
*/
bool
nTerrainLine::GeneratePolygon( nMeshBuilder* mb, nString meshPath )
{
    n_assert( this->closed );
    n_assert( this->GetHeightMap() );
    nFloatMap* hmap = this->GetHeightMap();
    int hmapSize = hmap->GetSize();
    int lineSize = points.Size() - 1;

    // Reset mesh builder state
    mb->Clear();

    if ( lineSize < 3 )
    {
        // Empty polygon
        return true;
    }

    // If the line is CCW, invert it to make it CW
    if ( ! this->IsCW() )
    {
        this->Invert();
    }

    // Get terrain points bounding box containing the line
    int x0 = hmapSize + 1;
    int z0 = x0;
    int x1 = -1;
    int z1 = -1;
    this->GetTerrainPointsBB( x0, z0, x1, z1 );

    if ( x1 == -1 || z1 == -1 )
    {
        // The line doesn't intersect with the terrain, so we can triangulate it directly.
        this->polyInfo.terrainPointsCount = 0;
        this->polyInfo.linePointsCount = lineSize;
        this->polyInfo.hmapSize = hmapSize;
        this->polyInfo.mb = mb;
        this->polyInfo.polygonPoints = n_new_array( vector3, lineSize );

        // Fill line points in polyInfo.polygonPoints and in the meshbuilder
        nMeshBuilder::Vertex v;
        for ( int i = 0; i < lineSize; i++ )
        {
            this->polyInfo.polygonPoints[ i ] = this->positions[ i ];
            v.SetCoord( this->positions[ i ] );
            v.SetColor( vector4(1.0f,0.4f,0.1f,0.5f) ); //i&1? vector4(0.5f,0.8f,1.0f,1.0f) : vector4(0.2f,0.2f,0.9f,1.0f) );
            v.SetUv(0, vector2(0.0f, 1.0f));
            mb->AddVertex( v );
        }
        this->polyInfo.usedPoints = NULL;

        nArray<int> entireLine;
        for (int i = 0; i < this->GetNumVertices(); i++)
        {
            entireLine.Append( i );
        }
        // @todo fix Triangulate, it might get into infinite loop for some cases (big/small polylines)
        // enable n3d2 mesh save below if Triangulate is saved
        //Triangulate( entireLine );
    }
    else
    {
        // General case: the line is intersected with the terrain, so a scan over all terrain triangles in the bounding box must be done.

        int nx = min( x1 - x0 + 2, hmapSize );
        int nz = min( z1 - z0 + 2, hmapSize );

        n_assert( nx > 0 && nz > 0 ); // Should not happen

        // Fill polygon info for triangulation
        int np = nx * nz;
        this->polyInfo.x0 = x0;
        this->polyInfo.z0 = z0;
        this->polyInfo.x1 = x1;
        this->polyInfo.z1 = z1;
        this->polyInfo.nx = nx;
        this->polyInfo.nz = nz;
        this->polyInfo.hmapSize = hmapSize;
        this->polyInfo.mb = mb;
        this->polyInfo.linePointsCount = lineSize;
        this->polyInfo.terrainPointsCount = np;
        // Build an array with the terrain points belonging to the polygon
        this->polyInfo.polygonPoints = n_new_array( vector3,  lineSize + np );
        // Another aux array for terrain vertex use
        this->polyInfo.usedPoints = n_new_array( bool, np );

        // Fill line points in polyInfo.polygonPoints and in the meshbuilder
        nMeshBuilder::Vertex v;
        for ( int i = 0; i < lineSize; i++ )
        {
            this->polyInfo.polygonPoints[ i ] = this->positions[ i ];
            v.SetCoord( this->positions[ i ] );
            v.SetColor( vector4(1.0f,0.4f,0.1f,0.5f) ); //i&1? vector4(0.5f,0.8f,1.0f,1.0f) : vector4(0.2f,0.2f,0.9f,1.0f) );
            v.SetUv(0, vector2(0.0f, 1.0f));
            mb->AddVertex( v );
        }

        // Fill terrain points in polyInfo.polygonPoints and in the meshbuilder
        for ( int z = 0; z < nz; z++ )
        {
            for ( int x = 0; x < nx; x++ )
            {
                int i = z * nx + x;
                vector3 pos = vector3( hmap->GetGridScale() * ( x0 + x ),
                                      hmap->GetHeightLC( x0 + x, z0 + z ),
                                      hmap->GetGridScale() * ( z0 + z ));
                
                this->polyInfo.polygonPoints[ this->polyInfo.linePointsCount + i ] = pos;
                
                // Also store a usage flag for each vertex (if false, the point won't belong to the polygon)
                this->polyInfo.usedPoints[ i ] = IsPointInside( v2d( this->polyInfo.polygonPoints[ this->polyInfo.linePointsCount + i ] ) );

                v.SetCoord( pos );
                v.SetColor( vector4(0.5f,0.2f,0.05f,0.5f) ); //i&1? vector4(0.5f,0.8f,1.0f,1.0f) : vector4(0.2f,0.2f,0.9f,1.0f) );
                v.SetUv(0, vector2(0.0f, 1.0f));
                mb->AddVertex( v );
            }
        }

        // Run through all terrain triangles in the bounding box
        for ( int z = 0; z < nz - 1; z++ )
        {
            for ( int x = 0; x < nx - 1; x++ )
            {
                //int i = z * nz + x;
                int it = ( z + z0 ) * ( hmapSize - 1 ) + x + x0;

                int face = 2 * it;
                for ( int nf = 0; nf < 2; nf ++ )
                {
                    // Run through the line searching a point in the terrain face
                    int p0 = 0;
                    int pointsLeft = lineSize;
                    // Search a point in the line that enters in this terrain face.
                    while ( pointsLeft > 0 && points[ p0 ]->face1 != face )
                    {
                        p0 = ( p0 + 1 ) % lineSize;
                        pointsLeft --;
                    }

                    if ( pointsLeft > 0)
                    {
                        // Found it. Make a call to find closed lines of intersection and triangulate them
                        GenPolyRecursive( p0, lineSize, face );
                    }

                    face++;
                }
            }
        }
    }

    n_delete_array( this->polyInfo.polygonPoints );
    n_delete_array( this->polyInfo.usedPoints );


    // Optimization: remove non-used terrain points
    mb->CleanVertex(-1);


    // Testing: save and reload the generated mesh. It is stored in an nArray and displayed in drawdebug

    // Save to file
    nString filename = nFileServer2::Instance()->ManglePath( meshPath.Get(), false );
#if 0
    // @todo enable again if Triangulate is fixed
    mb->SaveN3d2(nKernelServer::Instance()->GetFileServer(), filename.Get());

    // Reload again for visualization ( testing )
    int last = debugPolygons.Size();
    debugPolygons.Append( nGfxServer2::Instance()->NewMesh(0) );
    debugPolygons[last]->SetFilename(filename.Get());
    bool success = debugPolygons[last]->Load();
	n_assert(success);
#endif

    // Load a fixed shader for viewing the polygons
    if ( this->debugShader == NULL )
    {
	    this->debugShader = nGfxServer2::Instance()->NewShader("terr_pol_shader");
	    this->debugShader->SetFilename("shaders:terrpol.fx");
	    n_verify( this->debugShader->Load() );
    }

    return true;
}
//------------------------------------------------------------------------------
/**
    Internal recursive function for triangulation. Called only within GeneratePolygon.
    @param beginPoint First point to scan. Must be one that enters into the terrain face.
    @param pointsLeft number of points to scan
*/
void
nTerrainLine::GenPolyRecursive( int beginPoint, int pointsLeft, int face )
{
    // Make a nArray that represents the closed line that will be triangulated
    nArray<int> closedLine( 20, 20 );
    
    int lineSize = points.Size() - 1;
    int p0 = beginPoint;

    while ( pointsLeft > 0 )
    {
        // Add points to the closedLine until this line "exits" from the current terrain face
        int previousEdgePoint = p0;
        closedLine.Append( points[ p0 ]->vertex );
        pointsLeft --;
        int startingSegment = points[ p0 ]->edge1;
        int endingSegment;
        p0 = ( p0 + 1 ) % lineSize;
        while ( pointsLeft > 0 && points[ p0 ]->face0 == -1 )
        {
            closedLine.Append( points[ p0 ]->vertex );
            p0 = ( p0 + 1 ) % lineSize;
            pointsLeft --;
        }
        closedLine.Append( points[ p0 ]->vertex );
        pointsLeft --;
        endingSegment = points[ p0 ]->edge0;

        // Now add points from terrain segments until the line again enters in the terrain face (if it does)
        float t0 = points[ p0 ]->parameter0;
        float t1;
        bool done = false;
        while ( !done )
        {
            // Find the terrain point we need to follow, to continue closing the closedLine.
            // endingSegment is the terrain segment we need to close (it must be free of intersections between t0 and t1 to do so)

            bool adding;  // This flag tells wether we are adding or subtracting area from the triangle

            if ( endingSegment == startingSegment )
            {
                // The line enters and exits in the same segment. Find the other segment cap
                if ( pointsLeft <= 0 )
                {
                    t1 = points[ beginPoint ]->parameter1;
                    done = true; // We are closing the last segment
                }
                else
                {
                    t1 = points[ previousEdgePoint ]->parameter1;
                }

                if ( this->isCW ) {
                    if ( t1 < t0 ) // invertir condicion si la linea esta al reves
                    {
                        adding = false;
                        t1 = 1.0f; // o 0.0f si la linea esta al reves
                    }
                    else
                    {
                        adding = true;
                        done = true; // We are closing the last segment
                    }
                }
                else
                {
                    if ( t1 >= t0 ) 
                    {
                        adding = false;
                        t1 = 0.0f;
                    }
                    else
                    {
                        adding = true;
                        done = true; 
                    }
                }
            }
            else
            {
                adding = false;
                //t1 = 1.0f; // o 0.0f si la linea esta al reves
                if ( this->isCW )
                {
                    t1 = 1.0f;
                }
                else
                {
                    t1 = 0.0f;
                }
            }

            // Find the closest intersection in endingSegment with parameter between t0 and t1.
            // If there is one, we continue closedLine with that point.
            int p1 = ( p0 + 1 ) % lineSize;
            int closest = -1;
            float closestP = t1;
            int pointsLeft2 = lineSize;
            while ( pointsLeft2 > 0 )
            {
                if (( points[ p1 ]->face1 == face &&
                      points[ p1 ]->edge1 == endingSegment &&
                      points[ p1 ]->parameter1 > t0 &&
                      points[ p1 ]->parameter1 < t1 &&
                      abs( points[ p1 ]->parameter1 - t0 ) < closestP ))
                {
                    closest = p1;
                    closestP = abs( points[ p1 ]->parameter1 - t0 );
                }
                p1 = ( p1 + 1 ) % lineSize;
                pointsLeft2 --;
            }
            
            // If there is intersection (the line enters again in the same segment between t0 and t1), 
            if ( closest != -1 )
            {
                // Jump to the next point that enters this terrain face, and make
                // a recursive call between jumpPoint and 'closest' point with the remaining points
                int jumpPoint = ( p0 + 1 ) % lineSize;
                int pointsJumped = 0;
                for ( int i = jumpPoint ; i != closest; i = ( i + 1 ) % lineSize )
                {
                    pointsJumped ++;
                }
                if ( pointsJumped > pointsLeft )
                {
                    pointsJumped = pointsLeft;
                }
                
                pointsLeft -= pointsJumped;
                p0 = closest;
                done = true;

                while ( pointsJumped > 0 && points[ jumpPoint ]->face1 != face )
                {
                    jumpPoint = ( jumpPoint + 1 ) % lineSize;
                    pointsJumped --;
                }
                if ( pointsJumped > 0 )
                {
                    GenPolyRecursive( jumpPoint, pointsJumped, face);
                }

                // If the line has ended, triangulate
                if ( pointsLeft <= 0 )
                {
                    Triangulate( closedLine );
                    closedLine.Reset();
                }

            }
            else
            {
                // If we are in a single segment, adding area to the polygon, do it
                if ( adding )
                {
                    // Triangulate the closedLine and reset it.
                    Triangulate( closedLine );
                    closedLine.Reset();

                    // Jump p0 to the next point that enters this terrain face, making another recursive call
                    // with the points jumped
                    previousEdgePoint = p0;
                    while ( pointsLeft > 0 && points[ p0 ]->face1 != face )
                    {
                        p0 = ( p0 + 1 ) % lineSize;
                        pointsLeft --;
                    }
                    t0 = points[ p0 ]->parameter0;

                    // Start again main loop in the current line state

                }
                else
                {
                    // We've reached a terrain point, add it to the closedLine.
                    int x = (face / 2) % ( this->polyInfo.hmapSize - 1 );
                    int z = (face / 2) / ( this->polyInfo.hmapSize - 1 );

                    // Nota: si la linea esta invertida, es = endingSegment sin el + 1 % 3
                    int terrainPoint;// = ( endingSegment + 1 ) % 3;
                    if ( this->isCW )
                    {
                        terrainPoint = ( endingSegment + 1 ) % 3;
                    }
                    else
                    {
                        terrainPoint = endingSegment;
                    }

                    if ( ( face & 1 ) && ( terrainPoint != 2) )
                    {
                        terrainPoint += 1 + (terrainPoint & 1);
                    }
                    if ( terrainPoint & 1)
                    {
                        x ++;
                    }
                    if ( terrainPoint & 2)
                    {
                        z ++;
                    }
                    x = x - this->polyInfo.x0;
                    z = z - this->polyInfo.z0;
                    terrainPoint = z * this->polyInfo.nx + x;

                    closedLine.Append( this->polyInfo.linePointsCount + terrainPoint );
                    
                    // Process edge change, and continue
                    // Si la linea esta invertida, es un - 1 (ojo al %) y es t0 = 1.0f
                    //endingSegment = ( endingSegment + 1 ) % 3;
                    //t0 = 0.0f;

                    if ( this->isCW )
                    {
                        endingSegment = ( endingSegment + 1 ) % 3;
                        t0 = 0.0f;
                    }
                    else
                    {
                        endingSegment = endingSegment - 1;
                        if ( endingSegment < 0 )
                        {
                            endingSegment = 2;
                        }
                        t0 = 1.0f;
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
/**
    Tell if a 2d point is inside the line
    @param p0 Point to check
    @param ignoreSegment Optional parameter: ignore this segment. (0..number of vertices - 1)
    @return Wether the point is inside the line
    If specified, the ignoreSegment segment will not be included in the test. 
    The line must be closed, otherwise the test always fail.
*/
bool
nTerrainLine::IsPointInside( vector2 p0, int ignoreSegment) const
{
    if ( ! closed )
    {
        return false;
    }

    int np = this->positions.Size();

    // Get p0 point normal in 2d
    vector2 normal, p1;
    if ( ignoreSegment == -1 )
    {
        normal = vector2( 1.0f, 0.12345f );
    }
    else
    {
        p1 = v2d( this->positions[ ( ignoreSegment + 1 ) % np ] );
        normal =  p1 - p0;
        normal = vecrot90( normal );
    }
    normal.norm();

    // Calculate distance from p0 to farthest point in the line
    float maxDist = 0.0f;
    for (int i = 0; i < np; i++)
    {
        float d = ( v2d( this->positions[ i ] ) - p0 ).len();
        if ( d > maxDist )
        {
            maxDist = d;
        }
    }
    // p0-p1 is the segment to check number of intersections
    p1 = p0 + normal * ( maxDist * 1.1f );

    // Count number of intersections
    float t;
    vector2 vi;
    int numIntersections = 0;
    for ( int i = 0; i < np; i ++ )
    {
        if ( i == ignoreSegment )
        {
            continue;
        }
        if ( Intersect2Segments( p0, p1,
                                 v2d( this->positions[ i ] ),
                                 v2d( this->positions[ ( i + 1 ) % np ] ),
                                 vi, t ) )
        {
            numIntersections ++;
        }
    }

    // Count intersections that lie exactly in line vertices
    for ( int i = 0; i < np; i ++ )
    {
        vector2 d = v2d( positions[i] ) - p0;
        if ( d % normal == 0.0f )
        {
            numIntersections++;
        }
    }

    // If the number of intersections is odd, the point is inside.
    if ( numIntersections & 1 )
    {
        return true;
    }
    else
    {
        return  false;
    }
}

//------------------------------------------------------------------------------
/**
    Internal function for triangulation. Uses data from the struct this->polyInfo
    @param closedLine array of indexes to polygon vertices, representing a closed line.
    @param np Number of points in the closed line.
*/
void
nTerrainLine::Triangulate( nArray<int>& closedLine )
{
    int np = closedLine.Size();

    if ( np < 3 )
    {
        return;
    }

    nMeshBuilder::Triangle tri;
    
    // Loop until the line is triangulated
    int i = 0;
    const float MIN = float( 15.0 / ( 2.0f * float(PI) ) );
    float minAngle = MIN;
    int loop = np;
    while ( np > 2 )
    {

        // The new triangle will be p0-p1-p2, so we need to check if the segment p0-p2 is free of intersections
        // and that the triangle doesn't contain other edges. (Segments p0-p1 and p1-p2 alreay belong to the line)
        if ( CanTriangulate( i, closedLine, minAngle ) )
        {
            // We can add the triangle
            tri.SetVertexIndices( closedLine[ i ], closedLine[ ( i + 1 ) % np ], closedLine[ ( i + 2 ) % np ] );
            this->polyInfo.mb->AddTriangle( tri );
            // Remove the isolated point
            closedLine.EraseQuick( ( i + 1 ) % np );
            // And continue triangulating the closed line, with one point less.
            np --;
            i = i % np;

            minAngle = MIN;
            loop = np;
        }
        else
        {
            i = ( i + 1 ) % np;
            loop --;
            if ( loop <= 0 )
            {
                minAngle = 0.0f;
                loop = 0;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Wrap a line segment to the heightmap
    @param p0 First point of the line segment to wrap. The second point is the next in the buffer
    @return The number of new points generated
*/
int
nTerrainLine::WrapSegment(int p0)
{

    n_assert( p0 >= 0 && p0 < points.Size() - 1 );

    // Current starting segment is invalid because we start inside a terrain triangle
    int curSegment = -1;
    vector3 pos0 = positions[ points[ p0 ]->vertex ];
    vector3 pos1 = positions[ points[ p0 + 1 ]->vertex ];

    // Get the beginning face
    int curFace;
    if ( points[ p0 ]->face1 != -1 )
    {
        // Get it from the previous edge
        curFace = points[ p0 ]->face1;
    }
    else
    {
        // Get it from the point position
        float xF = pos0.x / this->GetHeightMap()->GetGridScale();
        float zF = pos0.z / this->GetHeightMap()->GetGridScale();
        int x = int( xF );
        int z = int( zF );
        curFace = 2 * ( x + z * ( this->GetHeightMap()->GetSize() - 1 ) );
        if ( (xF - x) + (zF - z) >= 1.0f )
        {
            curFace ++;
        }
    }

    // Iterate until there is no intersection between the segment and the terrain faces
    int returnValue = 0;
    bool intersect = true;
    while ( intersect )
    {
        intersect = false;
        float t0 = 0.0f;
        vector3 ipoint;

        // Iterate through the 3 segments of this face
        int segm;
        for ( segm = 0; segm < 3; segm++ )
        {
            if ( segm == curSegment )
            {
                // Ignore the segment we come from
                continue;
            }
            vector3 segmB, segmM;
            GetFaceSegment( curFace, segm, segmB, segmM );
            vector3 sM = segmB + segmM;
            vector2 ipoint2;

            if ( Intersect2Segments( v2d(segmB), v2d(sM), v2d(pos0), v2d(pos1), ipoint2, t0) && t0 > 0.0f )
            {
                intersect = true;
                float h;
                vector3 normal;
                this->GetHeightMap()->GetHeightNormal( ipoint2.x, ipoint2.y, h, normal);
                ipoint.set(ipoint2.x, h + this->offset, ipoint2.y);

                break;
            }
        }
        if ( intersect )
        {
            // Get the face and segment on the other side of the intersection (viewed from the next face)
            int nextface, nextsegm;
            if ( ! GetNextTerrainFaceSegment( curFace, segm, nextface, nextsegm) )
            {
                // We reached terrain boundary
                nextface = -1;
                nextsegm = -1;
            }

            /*if ( t0 == 0.0f )
            {
                n_assert2_always("Debug line intersection: Special case. (handle it?). It should not happen");
            }*/
            // If the intersection is exactly in the end of the line segment, don't create a new one
            if ( (pos1 - ipoint).len() <= 0.001f )
            {
                // Just assign the terrain face and segment info to the point
                points[ p0 + 1 ]->face0 = curFace;
                points[ p0 + 1 ]->edge0 = segm;
                points[ p0 + 1 ]->parameter0 = t0;
                points[ p0 + 1 ]->face1 = nextface;
                points[ p0 + 1 ]->edge1 = nextsegm;
                points[ p0 + 1 ]->parameter1 = 1.0f - t0;
            }
            // Otherwise, create it
            else
            {
                // Insert the new point
                positions.Insert( p0 + 1, ipoint );
                int n = points.Size();
                if ( this->closed )
                {
                    n --;
                }
                for (int i = p0 + 1; i < n; i++)
                {
                    points[i]->vertex ++;
                }
                returnValue ++;
                nTerrainLinePoint* newPoint = n_new(nTerrainLinePoint( p0 + 1, curFace, segm, t0, nextface, nextsegm, 1.0f - t0));
                points.Insert( p0 + 1, newPoint );

                // Move to the next point and continue
                p0++;
            }

            pos0 = positions[ points[ p0 ]->vertex ];
            pos1 = positions[ points[ p0 + 1 ]->vertex ];
            curFace = nextface;
            curSegment = nextsegm;

            // If we reached the terrain boundary, we've finished
            if ( curSegment == -1 )
            {
                intersect = false;
            }
        }
    }
    return returnValue;
}
//------------------------------------------------------------------------------
/**
    Generate a face segment in LC
    @param face The face wich we want to generate a segment
    @param segment The segment of the face
    @param b The returned segment begin
    @param m The returned segment vector
*/
void
nTerrainLine::GetFaceSegment(int face, int segment, vector3& b, vector3& m)
{
    n_assert( this->GetHeightMap() );
    n_assert( segment >= 0 && segment < 3 );

    int n = this->GetHeightMap()->GetSize() - 1;
    float gs = this->GetHeightMap()->GetGridScale();

    n_assert( face >= 0 && face < 2 * n * n );

    int x = face >> 1;
    int z = x / n;
    x = x - z * n;
    vector3 pNW( x * gs, this->GetHeightMap()->GetHeightLC(x, z), z * gs );

    if ( ! (face & 1 ) ) {
        // N-W face

        if ( segment == 0 )
        {
            b = pNW;
            m = vector3( gs, 0.0f, 0.0f );

        }
        else if ( segment == 1)
        {
            b = pNW + vector3(gs, 0.0f, 0.0f );
            m = vector3( -gs, 0.0f, gs );

        }
        else
        {
            b = pNW + vector3( 0.0f, 0.0f, gs );
            m = vector3( 0.0f, 0.0f, -gs );

        }
    }
    else
    {
        // S-E face

        if ( segment == 0 )
        {
            b = pNW + vector3( gs, 0.0f, 0.0f );
            m = vector3( 0.0f, 0.0, gs);

        }
        else if ( segment == 1)
        {
            b = pNW + vector3( gs, 0.0f, gs );
            m = vector3( -gs, 0.0f, 0.0f );

        }
        else
        {
            b = pNW + vector3( 0.0f, 0.0f, gs );
            m = vector3( gs, 0.0f, -gs );

        }

    }
}

//------------------------------------------------------------------------------
/**
    Get next face and segment when crossing from a terrain face through one of its segments
    @param faceFrom Starting face
    @param segmentFrom We cross the face through this segment
    @param faceTo On success, we exit to this face
    @param segmentTo On success, this is the crossed edge (relative to the exit face)
    @return If returned false, we attempted to cross a boundary edge


               s0
        #################
        # Face 0     #  #               Face0       Face2
        #          #    #                   Face1       Face3   ...
    s2  #     s1 #      #
        #      #  s2    # s0
        #    #          #
        #  #    Face 1  #
        #################
                s1  

*/
bool
nTerrainLine::GetNextTerrainFaceSegment(int faceFrom, int segmentFrom, int &faceTo, int &segmentTo)
{
    n_assert( this->GetHeightMap() );
    n_assert( segmentFrom >= 0 && segmentFrom < 3 );

    int n = this->GetHeightMap()->GetSize() - 1;
    n_assert( faceFrom >= 0 && faceFrom < 2 * n * n );
    
    int x = faceFrom >> 1;
    int z = x / n;
    x = x - z * n;

    if ( ! (faceFrom & 1 ) ) {
        // N-W face

        if ( segmentFrom == 0 )
        {
            if ( z == 0 )
            {
                return false;
            }
            faceTo = faceFrom - 2 * n + 1;
            segmentTo = 1;

        }
        else if ( segmentFrom == 1)
        {
            faceTo = faceFrom + 1;
            segmentTo = 2;

        }
        else
        {
            if ( x == 0 )
            {
                return false;
            }
            faceTo = faceFrom - 1;
            segmentTo = 0;

        }
    }
    else
    {
        // S-E face

        if ( segmentFrom == 0 )
        {
            if ( x == n-1 )
            {
                return false;
            }
            faceTo = faceFrom + 1;
            segmentTo = 2;

        }
        else if ( segmentFrom == 1)
        {
            if ( z == n-1 )
            {
                return false;
            }
            faceTo = faceFrom + 2 * n - 1;
            segmentTo = 0;

        }
        else
        {
            faceTo = faceFrom  - 1;
            segmentTo = 1;

        }

    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Tell if a triangle formed by two consecutive segments of a closed line, is free of
    intersections with the rest of the closed line.
    @param segment Begin point of the two consecutive segments to check
    @param closedLine nArray of indexes to polygon vertices, representing a closed line.
    @param minAngle If the triangle would have an angle less than minAngle, the test returrns failure
    @return A bool value telling if there is intersection

    The closed line is defined by closedLine (nArray of indexes), while the vector3 array is in
    this->polyInfo.polygonPoints
*/
bool
nTerrainLine::CanTriangulate( int segment, nArray<int>& closedLine, float minAngle )
{
    float t;
    vector2 vi;
    int np = closedLine.Size();

    if ( np <= 3 )
    {
        return true;
    }

    vector2 p0 = v2d( this->polyInfo.polygonPoints[ closedLine[ ( segment     ) % np ] ] );
    vector2 p1 = v2d( this->polyInfo.polygonPoints[ closedLine[ ( segment + 1 ) % np ] ] );
    vector2 p2 = v2d( this->polyInfo.polygonPoints[ closedLine[ ( segment + 2 ) % np ] ] );

    vector2 seg01 = p1 - p0;
    vector2 normalseg02 = p2 - p0;

    minAngle = cos( minAngle );
    if ( minAngle != 0.0f && ( seg01 % normalseg02 > abs( seg01.len() * normalseg02.len() * minAngle ) ) )
    {
        return false;
    }

    normalseg02 = vector2( -normalseg02.y, normalseg02.x );

    // Check if the p0-p2 segment is inside the line (assuming here that the line is CW )
    if ( seg01.x * normalseg02.x + seg01.y * normalseg02.y > 0 )
    {
        return false;
    }

    // Check if one of the adjacent segments to p0-p2 falls inside the pretended new triangle. If so, can't triangulate
    vector2 adj1 = v2d( this->polyInfo.polygonPoints[ closedLine[ ( segment + 3 ) % np ] ] );
    int j = segment - 1;
    if ( j < 0 ) j += np;
    vector2 adj2 = v2d( this->polyInfo.polygonPoints[ closedLine[ j ] ] );

    if ( np > 3 && ( PointInsideTriangle2d( adj1, p0, p1, p2 ) ||
                     PointInsideTriangle2d( adj2, p0, p1, p2 ) ) )
    {
        return false;
    }
        
    // Check line of sight between p0 and p2
    int i = ( segment + 3 ) % np;
    for ( int n = np - 4; n > 0; n -- )
    {
        if ( Intersect2Segments( p0, p2,
                                 v2d( this->polyInfo.polygonPoints[ closedLine[ i ] ] ),
                                 v2d( this->polyInfo.polygonPoints[ closedLine[ ( i + 1 ) % np ] ] ),
                                 vi, t ) )
        {
            return false;
        }

        i = ( i + 1 ) % np;
    }
    return true;
}
//------------------------------------------------------------------------------
/**
    Tell if a new point can be added to the line.
    @param newPoint The 2d position of the new point
    @return A bool value telling if the point can be added to the line. Otherwise,
    the specified 2d point lies outside of the heightmap, or it is too close to
    the heightmap grid, or the new edge would make the line autointersect. Also, if
    the line is closed the function returns false.
*/
bool
nTerrainLine::CanAddPoint( const vector2& newPoint, bool checkFirstSegment )
{
    int np = this->positions.Size();

    if ( np == 0 )
    {
        return true;
    }

    // Check if line is closed, or newPoint is too close to the last point
    if ( closed || ( v2d( positions.Back() ) - newPoint ).len() < this->lineMargin )
    {
        return false;
    }

    // Check if newPoint lies too close to the heightmap grid
    if ( this->GetHeightMap() )
    { 
        float gscale = this->GetHeightMap()->GetGridScale();
        float remainderx = newPoint.x / gscale - int( newPoint.x / gscale );
        float remainderz = newPoint.y / gscale - int( newPoint.y / gscale );
        float remainderxz = remainderx + remainderz;
        if ( remainderx < lineTerrainMargin || remainderz < lineTerrainMargin || 
             remainderx > ( 1.0f - lineTerrainMargin ) || remainderz > ( 1.0f - lineTerrainMargin ) ||
             ( remainderxz > ( 1.0f - lineTerrainMargin ) && remainderxz < ( 1.0f + lineTerrainMargin ) ) )
        {
            return false;
        }
    }

    // If line has less than three points, for sure the new point doesn't autointersect
    if ( np < 3 ) 
    {
        return true;
    }

    vector2 lastPoint = v2d( this->positions[ np - 1 ] );

    // Check line of sight between lastPoint and newPoint
    float t;
    vector2 vi;
    int i = 0;
    if ( ! checkFirstSegment )
    {
        i = 1;
    }
    while ( i < np - 2 )
    {
        if ( Intersect2Segments( lastPoint, newPoint,
                                 v2d( this->positions[ i ] ),
                                 v2d( this->positions[ i + 1 ] ),
                                 vi, t ) )
        {
            return false;
        }

        i ++;
    }

    return true;
}
//------------------------------------------------------------------------------
/**
    Check if the line is CW or CCW. Actualizes the flag isCW, only if the line is closed
*/
void
nTerrainLine::CheckCW( void )
{
    if ( ! closed )
    {
        return;
    }

    // Get point in the middle of segment 0
    vector2 p0 = v2d( this->positions[ 0 ] );
    vector2 p1 = v2d( this->positions[ 1 ] );
    p0 = p0 + ( p1 - p0 ) * 0.5f;

    // Use the function IsPointInside(), ignoring segment 0. If the number of intersections is odd,
    // the line is CW. Otherwise it's CCW
    if ( IsPointInside( p0, 0) )
    {
        isCW = true;
    }
    else
    {
        isCW = false;
    }
}
//------------------------------------------------------------------------------
/**
    Invert the line points order
*/
void
nTerrainLine::Invert( void )
{
    n_assert( this->closed );
    int np = points.Size();
    
    // Number of point to permute
    int nper = np / 2;

    nTerrainLinePoint* aux;

    // Permute points and positions
    for (int i = 0; i < nper; i++)
    {
        aux = points[ i ];
        int otherPoint = np - i - 1;
        points[ i ] = points[ otherPoint ];
        points[ otherPoint ] = aux;

        int temp = points[i]->face0;
        points[i]->face0 = points[i]->face1;
        points[i]->face1 = temp;
        temp = points[i]->edge0;
        points[i]->edge0 = points[i]->edge1;
        points[i]->edge1 = temp;
        float ftemp = points[i]->parameter0;
        points[i]->parameter0 = points[i]->parameter1;
        points[i]->parameter1 = ftemp;

        temp = points[otherPoint]->face0;
        points[otherPoint]->face0 = points[otherPoint]->face1;
        points[otherPoint]->face1 = temp;
        temp = points[otherPoint]->edge0;
        points[otherPoint]->edge0 = points[otherPoint]->edge1;
        points[otherPoint]->edge1 = temp;
        ftemp = points[otherPoint]->parameter0;
        points[otherPoint]->parameter0 = points[otherPoint]->parameter1;
        points[otherPoint]->parameter1 = ftemp;
    }

    isCW = ! isCW;
}
//------------------------------------------------------------------------------
/**
    Get intersection between 2 line segments in 2d
    @param b0 Start of segment0
    @param e0 End of segment0
    @param b1 Start of segment1
    @param e1 End of segment1
    @param vi Intersection point, only if returned true
    @param t0 segment0 parameter of intersection point (0..1), only if returned true
    @return True if there is an intersection
*/
bool
nTerrainLine::Intersect2Segments(const vector2& b0, const vector2& e0, const vector2& b1, const vector2& e1, vector2 &vi, float& t0)
{
    // Get slope of segment1 in segment0 space (m)
    vector2 m0 = e0 - b0;
    float m0l = m0.len();
    n_assert2( m0l, "Segment with length 0 is illegal" )
    m0 /= m0l;
    vector2 m1 = e1 - b1;
    float f = m0.x * m1.x + m0.y * m1.y;

    float m1l = m1.len();
    n_assert2( m1l, "Segment with length 0 is illegal" );
    float m = ( -m0.y * m1.x + m0.x * m1.y )  / f;

    // Get begin of segment1 in segment0 space (x, y)
    vector2 b01 = b1 - b0;
    float x = m0.x * b01.x + m0.y * b01.y;
    //float b01l = b01.len();
    float y = - m0.y * b01.x + m0.x * b01.y;

    // Calculate intersection parameter
    t0 = x - y / m;

    // Check if intersection is inside first segment
    if ( t0 < 0.0f || t0 > m0l )
    {
        return false;
    }

    // Calculate intersection point
    vi = b0 + m0 * t0;

    // Get intersection point parameter in segment1
    vector2 b1i =  vi - b1;
    m1 /= m1l;
    float t1 = b1i.x * m1.x + b1i.y * m1.y;
    // Check if intersection point is also inside segment 1
    if ( t1 < 0.0f || t1 > m1l ) 
    {
        return false;
    }

    t0 /= m0l;

    return true;
}
//------------------------------------------------------------------------------
/**
    @brief Tell if a point is inside a 2d triangle
    @param a The point to test
    @param p0 First triangle point
    @param p1 second triangle point
    @param p2 third triangle point
    @return True if the point is inside the triangle
*/
bool
nTerrainLine::PointInsideTriangle2d(vector2& a, vector2& p0, vector2& p1, vector2& p2)
{
    vector2 ap0 = p0 - a;
    vector2 ap2 = p2 - a;

    if ( (p1 - p0).rotate90() % ap0 > 0 )
    {
        return false;
    }
    if ( (p2 - p1).rotate90() % ap2 > 0 )
    {
        return false;
    }
    if ( (p0 - p2).rotate90() % ap2 > 0 )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Get distance from 2d point to 2d segment
    @param a point to get distance from segment
    @param p0 first point of segment
    @param p1 The other point of the segment
*/
float
nTerrainLine::GetPointSegmentDistance2d(vector2& a, vector2& p0, vector2& p1)
{
    vector2 s = p1 - p0;
    vector2 r = a - p0;
    float x = r % s;
    if ( x < 0.0f )
    {
        return r.len();
    }
    else if ( x > 1.0f )
    {
        return vector2( a - p1 ).len();
    }

    s.norm();
    vector2 q = r - ( s * ( r % s ) );
    return q.len();
}

//------------------------------------------------------------------------------
/**
    Functionality to persist the object
*/
bool 
nTerrainLine::SaveCmds(nPersistServer* ps)
{
    if( !nObject::SaveCmds(ps) )
        return false;

    nCmd* cmd(0);

    // persisting offset
    cmd = ps->GetCmd( this, 'DSOF');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetOffset() );

    ps->PutCmd(cmd);    

    // persisting orientation
    cmd = ps->GetCmd( this, 'DSCW');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetB( this->IsCW() );
    
    ps->PutCmd(cmd);

    // persisting points
    for( int index(0); index < this->positions.Size(); ++index )
    {
        cmd = ps->GetCmd( this, 'DADP' );

        n_assert2( cmd, "Error command not found" );

        vector3 pos(this->positions[ index ]);

        cmd->In()->SetF( pos.x );
        cmd->In()->SetF( pos.z );

        ps->PutCmd(cmd);    
    }

    // persisting if wrapped
    if( this->IsWrapped() )
    {
        cmd = ps->GetCmd( this, 'DWRP');

        n_assert2( cmd, "Error command not found" );

        ps->PutCmd(cmd);    
    }

    // persisting if closed
    if( this->IsClosed() )
    {
        cmd = ps->GetCmd( this, 'DCLS');

        n_assert2( cmd, "Error command not found" );

        ps->PutCmd(cmd);    
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Sets if the line is clockwise

    @param world it's the world where the object will be come to existance

    history:
        - 11-Jan-2006   David Reyes    created
*/
void nTerrainLine::SetCW( const bool is )
{
    this->isCW = is;
}

//-----------------------------------------------------------------------------
/**
    Add a point

    @param a component a
    @param b component b

    history:
        - 11-Jan-2006   David Reyes    created
*/
void nTerrainLine::AddPoint( const float a, const float b)
{
    this->AddPoint( vector2( a,b ) );
}


//-----------------------------------------------------------------------------
/**
    Gets a bounding box in the plane defined by x and z

    @param bb bounding box data structure to be filled

    history:
        - 23-Jun-2006   David Reyes    created
*/
void nTerrainLine::GetBBinXZ( bbox3& bb )
{
    if( !this->positions.Size() ) 
    {
        return;
    }

    vector2 leftTop(this->positions[0].x,this->positions[0].z);
    vector2 rightBotton(this->positions[0].x,this->positions[0].z);

    for( int index(1); index < this->positions.Size(); ++index )
    {
        if( this->positions[ index ].x < leftTop.x )
        {
            leftTop.x = this->positions[ index ].x;
        } 
        else if( this->positions[ index ].x > rightBotton.x )
        {
            rightBotton.x = this->positions[ index ].x;
        }

        if( this->positions[ index ].z < leftTop.y )
        {
            leftTop.y = this->positions[ index ].z;
        } 
        else if( this->positions[ index ].z > rightBotton.y )
        {
            rightBotton.y = this->positions[ index ].z;
        }
    }

    float distanceX(rightBotton.x - leftTop.x);
    float distanceZ(rightBotton.y - leftTop.y);

    bb = bbox3( vector3(
        leftTop.x + distanceX / float(2),
        0,
        leftTop.y + distanceZ / float(2)),
        vector3( distanceX, float(.1), distanceZ )
        );
}

#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------ 
NSCRIPT_INITCMDS_BEGIN(nTerrainLine)
    NSCRIPT_ADDCMD('DSOF', void, SetOffset, 1, (const float), 0, ());
    NSCRIPT_ADDCMD('DGOF', const float, GetOffset, 0, (), 0, ());
    NSCRIPT_ADDCMD('DCLS', const bool, Close, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSCW', void, SetCW, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD('DWRP', void, Wrap, 0, (), 0, ());
    NSCRIPT_ADDCMD('DADP', void, AddPoint, 2, (const float,const float), 0, ());
NSCRIPT_INITCMDS_END()
//------------------------------------------------------------------------------ 