#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//  nmeshbuilder_tangent.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
/**
    Build triangle normals, tangents and binormals.
    The tangents require a valid uv-mapping in texcoord layer 0.
*/
void
nMeshBuilder::BuildTriangleNormals(int layer)
{
    // compute face normals and tangents
    int triangleIndex;
    int numTriangles = this->GetNumTriangles();
    vector3 v0, v1;
    vector2 uv0, uv1;
    vector3 n, t, b;
    const bool calculateTangent(layer>=0);
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);

        const Vertex& vertex0 = this->GetVertexAt(index[0]);
        const Vertex& vertex1 = this->GetVertexAt(index[1]);
        const Vertex& vertex2 = this->GetVertexAt(index[2]);

        // compute the face normal
        v0 = vertex1.GetCoord() - vertex0.GetCoord();
        v1 = vertex2.GetCoord() - vertex0.GetCoord();
        n = v0 * v1;
        n.norm();
        tri.SetNormal(n);

        if (calculateTangent)
        {
            // compute the tangents
            float x1 = vertex1.GetCoord().x - vertex0.GetCoord().x;
            float x2 = vertex2.GetCoord().x - vertex0.GetCoord().x;
            float y1 = vertex1.GetCoord().y - vertex0.GetCoord().y;
            float y2 = vertex2.GetCoord().y - vertex0.GetCoord().y;
            float z1 = vertex1.GetCoord().z - vertex0.GetCoord().z;
            float z2 = vertex2.GetCoord().z - vertex0.GetCoord().z;

            float s1 = vertex1.GetUv(layer).x - vertex0.GetUv(layer).x;
            float s2 = vertex2.GetUv(layer).x - vertex0.GetUv(layer).x;
            float t1 = vertex1.GetUv(layer).y - vertex0.GetUv(layer).y;
            float t2 = vertex2.GetUv(layer).y - vertex0.GetUv(layer).y;

            float l = (s1 * t2 - s2 * t1);        
            // catch singularity
            if (l == 0.0f)
            {
                l = 0.0001f;
            }
            float r = 1.0f / l;
            vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
            vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

            // Gram-Schmidt orthogonalize
            t = (sdir - n * (n % sdir));
            t.norm();

            // calculate handedness
            float h;
            if (((n * sdir) % tdir) < 0.0f)
            {
                h = -1.0f;
            }
            else
            {
                h = +1.0f;
            }

            // Zombie compatibility with previous version
            t = -t;
            h = -h;
            // end Zombie

            b = (n * t) * h;

            // degenerate projections, always length == 1.0
            if ( t.len() < 0.1f) 
            {
                // Gram-Schmidt orthogonalize
                b = (tdir - n * (n % tdir));
                b.norm();
                t =  ( b * h ) * n ;
            }

            tri.SetTangent(t);
            tri.SetBinormal(b);
            tri.SetHandedness(h);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Generates the per-vertex tangents by averaging the
    per-triangle tangents and binormals which must be computed
    beforehand. Note that the vertex normals will not be touched!
    Internally, the method will create a clean mesh which contains
    only vertex coordinates and normals, and computes connectivity
    information from the resulting mesh. The result is that 
    tangents and binormals are averaged for smooth edges, as defined
    by the existing normal set.

    @param  allowVertexSplits   true if vertex splits are allowed, this
                                will give better results at mirrored UV
                                edges, but the number of vertices may
                                change, this may be problematic for
                                blendshapes
*/
void
nMeshBuilder::BuildVertexTangents(bool allowVertexSplits)
{
    if (allowVertexSplits)
        this->BuildVertexTangentsWithSplits();
    else
        this->BuildVertexTangentsWithoutSplits();
}

//------------------------------------------------------------------------------
/**
*/
void 
nMeshBuilder::BuildVertexTangentsWithSplits()
{
    // inflate the mesh, this generates 3 (possibly redundant) vertices
    // for each triangle
    this->Inflate();

    // create a pure coord mesh to get correct triangle connectivity
    nMeshBuilder cleanMesh = *this;
    nArray< nArray<int> > collapsMap(0, 0);
    collapsMap.SetFixedSize(this->GetNumVertices());
    cleanMesh.ForceVertexComponents(Vertex::COORD | Vertex::NORMAL);
    cleanMesh.Cleanup(&collapsMap);

    // create a connectivity map, which contains for each vertex
    // the triangle indices which use this vertex
    nArray< nArray<int> > vertexTriangleMap(0, 0);
    cleanMesh.BuildVertexTriangleMap(vertexTriangleMap);

    // compute averaged vertex tangents
    int vertexIndex = 0;
    int numVertices = cleanMesh.GetNumVertices();
    vector3 avgTangent;
    vector3 avgBinormal;
    nFixedArray<vector3> averagedTangents(this->GetNumVertices());
    nFixedArray<vector3> averagedBinormals(this->GetNumVertices());
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        avgTangent.set(0.0f, 0.0f, 0.0f);
        avgBinormal.set(0.0f, 0.0f, 0.0f);
        int numVertexTris = vertexTriangleMap[vertexIndex].Size();
        int vertexTriIndex;
        for (vertexTriIndex = 0; vertexTriIndex < numVertexTris; vertexTriIndex++)
        {
            const Triangle& tri = cleanMesh.GetTriangleAt(vertexTriangleMap[vertexIndex][vertexTriIndex]);
            avgTangent += tri.GetTangent();
            avgBinormal += tri.GetBinormal();
        }
        avgTangent.norm();
        avgBinormal.norm();

        int i;
        for (i = 0; i < collapsMap[vertexIndex].Size(); i++)
        {
            averagedTangents[collapsMap[vertexIndex][i]] = avgTangent;
            averagedBinormals[collapsMap[vertexIndex][i]] = avgBinormal;
        }
    }

    // fill tangents by deciding for each vertex whether to 
    // use the triangle tangent or the averaged tangent this is
    // done by comparing the averaged and the triangle vertex,
    // if they are close enough to each other, the averaged tangent
    // is used
    int triangleIndex = 0;
    int numTriangles = this->GetNumTriangles();
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        const vector3& triTangent = this->GetTriangleAt(triangleIndex).GetTangent();
        const vector3& triBinormal = this->GetTriangleAt(triangleIndex).GetBinormal();
        int i;
        for (i = 0; i < 3; i++)
        {
            vertexIndex = triangleIndex * 3 + i;
            const vector3& avgTangent = averagedTangents[vertexIndex];
            const vector3& avgBinormal = averagedBinormals[vertexIndex];

            if ((0 == avgTangent.compare(triTangent, 1.0f)) && (0 == avgBinormal.compare(triBinormal, 1.0f)))
            {
                // use averaged tangent for this vertex
                this->GetVertexAt(vertexIndex).SetTangent(avgTangent);
                this->GetVertexAt(vertexIndex).SetBinormal(avgBinormal);
            }
            else
            {
                // use triangle tangent for this vertex
                this->GetVertexAt(vertexIndex).SetTangent(triTangent);
                this->GetVertexAt(vertexIndex).SetBinormal(triBinormal);
            }
        }
    }

    // do a final cleanup, removing redundant vertices
    this->Cleanup(0);
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshBuilder::BuildVertexTangentsWithoutSplits()
{
    // NOTE: this is the traditional method to generate UVs

    // create a clean coord/normal-only mesh, record the cleanup operation
    // in a collaps map so that we can inflate-copy the new vertex
    // components into the original mesh afterwards
    nArray< nArray<int> > collapsMap(0, 0);
    collapsMap.SetFixedSize(this->GetNumVertices());
    nMeshBuilder cleanMesh = *this;
    cleanMesh.ForceVertexComponents(Vertex::COORD | Vertex::NORMAL | Vertex::BINORMAL);
    cleanMesh.Cleanup(&collapsMap);

    // create a connectivity map which contains for each vertex
    // the triangle indices which share the vertex
    nArray< nArray<int> > vertexTriangleMap(0, 0);
    cleanMesh.BuildVertexTriangleMap(vertexTriangleMap);

    // for each vertex...
    int vertexIndex = 0;
    int numVertices = cleanMesh.GetNumVertices();
    vector3 avgTangent;
    vector3 avgBinormal;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        avgTangent.set(0.0f, 0.0f, 0.0f);
        avgBinormal.set(0.0f, 0.0f, 0.0f);

        // for each triangle sharing this vertex...
        int numVertexTris = vertexTriangleMap[vertexIndex].Size();
        n_assert(numVertexTris > 0);
        int vertexTriIndex;
        for (vertexTriIndex = 0; vertexTriIndex < numVertexTris; vertexTriIndex++)
        {
            const Triangle& tri = cleanMesh.GetTriangleAt(vertexTriangleMap[vertexIndex][vertexTriIndex]);
            avgTangent += tri.GetTangent();
            avgBinormal += tri.GetBinormal();
        }

        // renormalize averaged tangent and binormal
        avgTangent.norm();
        avgBinormal.norm();

        cleanMesh.GetVertexAt(vertexIndex).SetTangent(avgTangent);
        cleanMesh.GetVertexAt(vertexIndex).SetBinormal(avgBinormal);
    }

    // inflate-copy the generated vertex tangents and binormals to the original mesh
    this->InflateCopyComponents(cleanMesh, collapsMap, Vertex::TANGENT | Vertex::BINORMAL);
}

//------------------------------------------------------------------------------
/**
    Generates the per-vertex normals by averaging the
    per-triangle normals which must be computed or exist
    beforehand. Note that only the vertex normals will be touched!

    29-Mar-2004 Johannes  added for nmax
*/
void
nMeshBuilder::BuildVertexNormals()
{
    // create a clean coord/normal-only mesh, record the cleanup operation
    // in a collaps map so that we can inflate-copy the new vertex
    // components into the original mesh afterwards
    nArray< nArray<int> > collapsMap(0, 0);
    collapsMap.SetFixedSize(this->GetNumVertices());
    nMeshBuilder cleanMesh = *this;
    cleanMesh.ForceVertexComponents(Vertex::COORD | Vertex::NORMAL);
    cleanMesh.Cleanup(&collapsMap);

    // create a connectivity map which contains for each vertex
    // the triangle indices which share the vertex
    nArray< nArray<int> > vertexTriangleMap(0, 0);
    cleanMesh.BuildVertexTriangleMap(vertexTriangleMap);

    // for each vertex...
    int vertexIndex = 0;
    const int numVertices = cleanMesh.GetNumVertices();
    vector3 avgNormal;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        avgNormal.set(0.0f, 0.0f, 0.0f);

        // for each triangle sharing this vertex...
        int numVertexTris = vertexTriangleMap[vertexIndex].Size();
        n_assert(numVertexTris > 0);
        int vertexTriIndex;
        for (vertexTriIndex = 0; vertexTriIndex < numVertexTris; vertexTriIndex++)
        {
            const Triangle& tri = cleanMesh.GetTriangleAt(vertexTriangleMap[vertexIndex][vertexTriIndex]);
            avgNormal += tri.GetNormal();
        }

        // renormalize averaged normal
        avgNormal.norm();

        cleanMesh.GetVertexAt(vertexIndex).SetNormal(avgNormal);
    }

    // inflate-copy the generated vertex normals to the original mesh
    this->InflateCopyComponents(cleanMesh, collapsMap, Vertex::NORMAL);
}

//------------------------------------------------------------------------------
/**
    Build triangle tangents. The tangents require a valid 
    uv-mapping in texcoord layer 0.

    29-Mar-2004 Johannes  added for nmax
*/
/*void
nMeshBuilder::BuildTriangleTangents()
{
    int triangleIndex;
    int numTriangles = this->GetNumTriangles();
    vector3 v0;
    vector3 v1;
    vector2 uv0, uv1;
    vector3 n, t, b;
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);

        const Vertex& vertex0 = this->GetVertexAt(index[0]);
        const Vertex& vertex1 = this->GetVertexAt(index[1]);
        const Vertex& vertex2 = this->GetVertexAt(index[2]);
       
        v0 = vertex1.GetCoord() - vertex0.GetCoord();
        v1 = vertex2.GetCoord() - vertex0.GetCoord();
     
        // compute the tangents
        uv0 = vertex1.GetUv(0) - vertex0.GetUv(0);
        uv1 = vertex2.GetUv(0) - vertex0.GetUv(0);
        t = (v0 * uv1.y) - (v1 * uv0.y);
        
        t.norm();
        tri.SetTangent(t);
    }
}*/


//------------------------------------------------------------------------------
/**
    Build triangle  tangents and binormals. The tangents require a valid 
    uv-mapping in texcoord layer 0. A new mesh reduced to coord and uv0 
    components will be used for the computation to ensure proper vertex sharing 
    between triangles.
*/
void
nMeshBuilder::BuildTriangleOnlyTangentBinormal(const int layer)
{
    // compute face normals and tangents
    int triangleIndex;
    int idx;
    int numTriangles;
    int numVertices;
    int numDifferentVertex;
    nArray<int> vertexToUniqueVertex;

    this->BuildTriangleNormals(layer); // Create tangent, binormal and handedness for the triangle, ( the normal don't care)
    this->BuildVertexHandedness(); // Create new vertex if necesary ,( split the mirror vertex)

    numTriangles = this->GetNumTriangles();
    numVertices = this->GetNumVertices();
    numDifferentVertex = this->GetMapVertexOfSamePosNormalsUV( layer , vertexToUniqueVertex );

    vector3* mapTan = n_new_array( vector3, numDifferentVertex ); // for average tangent
    vector3* mapBin = n_new_array( vector3, numDifferentVertex ); // for average binormal , use it if the tan avg = 0;
    n_assert(mapTan);
    n_assert(mapBin);
    
    /// add the tangent in vertex for each triangle
    vector3  t, b;
    int index[3];
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        tri.GetVertexIndices(index[0], index[1], index[2]);
        t = tri.GetTangent();
        b = tri.GetBinormal();
        for ( int i = 0 ; i < 3 ; i++)
        {
            const vector3& normal = GetVertexAt(index[i]).GetNormal();
            vector3 tangent  = normal * ( t * normal );
            vector3 binormal = normal * ( b * normal);
            tangent.norm();
            binormal.norm();
            mapTan[ vertexToUniqueVertex[ index[i] ] ] += tangent;
            mapBin[ vertexToUniqueVertex[ index[i] ] ] += binormal;
        }
    }


    // calculate the average tangent and binormal
    for ( idx = 0; idx < numDifferentVertex ; idx ++)
    {
        mapBin[idx].norm();
        mapTan[idx].norm();
    }


    for (idx = 0 ; idx < numVertices; idx ++)
    {
        Vertex& vertex = this->GetVertexAt(idx);
        const vector3& normal = vertex.GetNormal();
        vector3 binormal = normal * ( mapBin[ vertexToUniqueVertex[ idx] ] * normal );
        vector3 tangent =  normal * ( mapTan[ vertexToUniqueVertex[ idx] ] * normal );
        binormal.norm();
        tangent.norm();

        // detect any degenerate projections or mirrored vertex
        // Invalid average tangent   ( length always == 1.0 )
        if ( tangent.len() < 0.1f) 
        {
            tangent = ( ( binormal * vertex.GetHandedness() ) * normal) ;
        }

        if ( binormal.len() < 0.1f) // Invalid average binormal
        {
            binormal = (normal * tangent) * vertex.GetHandedness();
        }

        vertex.SetBinormal( binormal );
        vertex.SetTangent( tangent); 
    }

    n_delete_array( mapBin );
    n_delete_array( mapTan );

    /*
    // Rapid solution for testing the result of average the mirror vertex
    float *dirTan = n_new_array( float ,numVertices );
    float *dirBin = n_new_array( float ,numVertices );

    for( idx =0 ; idx < numVertices ; ++idx )
    {
       int idx2;
       Vertex& vertex1= GetVertexAt(idx) ;
       vector3 tan1( vertex1.GetTangent() );
       vector3 bin1( vertex1.GetBinormal() );
       vector3 tan( tan1 );
       vector3 bin( bin1 );

       for( idx2 = idx+1 ;idx2 < numVertices ;  ++idx2)
       {
           Vertex& vertex2= GetVertexAt(idx2) ;
           vector3 tan2( vertex2.GetTangent() );
           vector3 bin2( vertex2.GetBinormal() );

           if ( vertex1.GetCoord().isequal( vertex2.GetCoord() , 0.0f )   &&
               vertex1.GetNormal().isequal( vertex2.GetNormal() , 0.0f )  &&
                vertex1.GetUv(layer).isequal( vertex2.GetUv( layer ) , 0.0f )
              )
           {
               dirTan[ idx2 ] = 0.0f < ( tan1 % tan2)  ? 1.0f : -1.0f;
               dirBin[ idx2 ] = 0.0f < ( bin1 % bin2)  ? 1.0f : -1.0f;
               tan += tan2*dirTan[ idx2 ]; // change the sign if necesary for average
               bin += bin2*dirBin[ idx2 ];
           }
       }

       bin  = vertex1.GetNormal() * (  bin * vertex1.GetNormal() );
       tan  = vertex1.GetNormal() * (  tan * vertex1.GetNormal() );
       bin.norm();
       tan.norm();
       vertex1.SetBinormal( bin );
       vertex1.SetTangent( tan );

       for( idx2 = idx+1 ;idx2 < numVertices ;  ++idx2)
       {
           Vertex& vertex2= GetVertexAt(idx2) ;
           if ( vertex1.GetCoord().isequal( vertex2.GetCoord() ,0.0f)  &&
               vertex1.GetNormal().isequal( vertex2.GetNormal() ,0.0f ) &&
                vertex1.GetUv(layer).isequal( vertex2.GetUv( layer ) , 0.0f )
              )
           {
                Vertex& vertex2= GetVertexAt(idx2) ;
                vertex2.SetBinormal( bin * dirBin[idx2] ) ;
                vertex2.SetTangent( tan * dirTan[ idx2 ] );
           }
       }
    }
    n_delete( dirTan );
    n_delete( dirBin );
    */

}



//------------------------------------------------------------------------------
/**
    Build index map of vertex. This map is a relation the old index with the new index.
    the vertex with the same position , normal and uv has a same new index.
    @return numDifferentVertex
*/
int 
nMeshBuilder::GetMapVertexOfSamePosNormalsUV(const int layer, nArray< int >& vertexToUniqueVertex)
{
    int idx;
    const int numVertices = this->GetNumVertices();
    int numDifferentVertex = 0;

    vertexToUniqueVertex.SetFixedSize( numVertices );
    for ( idx = 0 ; idx < numVertices ; idx ++)
    {
        vertexToUniqueVertex[idx] = -1;
    }

    /// sort the vertex by position , normal and uv0

    PosNormalUV* mapVertex = n_new_array( PosNormalUV, numVertices );

    for (  idx = 0 ; idx < numVertices ; ++ idx)
    {
        const Vertex& vertex = this->GetVertexAt( idx );

        mapVertex[idx].idx = idx;
        mapVertex[idx].pos    = vertex.GetCoord();
        mapVertex[idx].normal = vertex.GetNormal();
        mapVertex[idx].uv = vertex.GetUv( layer);
        mapVertex[idx].hand = vertex.GetHandedness();
    }

    qsort( mapVertex, numVertices, sizeof(PosNormalUV), PosNormalUV::vertexSorter );

    idx= 0;
    while(  idx < numVertices )
    {
        PosNormalUV& current = mapVertex[idx];
        n_assert( -1 == vertexToUniqueVertex[ current.idx] );
        vertexToUniqueVertex[ current.idx] = numDifferentVertex;
        idx++;        
        
        while ( idx < numVertices && 0 == PosNormalUV::compare( current , mapVertex[idx] ) )
        {
            n_assert( -1 == vertexToUniqueVertex[ mapVertex[idx].idx ] );
            vertexToUniqueVertex[ mapVertex[idx].idx ] = numDifferentVertex;
            idx++;
        }
        numDifferentVertex++;
    }

    for ( idx = 0 ; idx < numVertices ; idx ++)
    {
        n_assert( vertexToUniqueVertex[idx] != -1 );
    }

    n_delete_array(mapVertex);

    return numDifferentVertex;

}

//------------------------------------------------------------------------------
/**
    function for compare
    @return -if i0 < i1  return -1 else if i0=i1 return 0 else return 1
*/
int __cdecl 
nMeshBuilder::PosNormalUV::compare(const PosNormalUV& i0 , const PosNormalUV& i1)
{
    int v;
    v = i0.pos.compare( i1.pos , 0.0f);
    if (v != 0) return v;

    v = i0.normal.compare( i1.normal , 0.0f );
    if (v != 0) return v;

    v = i0.uv.compare( i1.uv , 0.0f );
    if (v != 0) return v;

    if  ( i0.hand == i1.hand )
    {
        return 0;
    } else
    {
        return ( i0.hand < i1.hand ) ? -1 : 1 ;
    }
}

//------------------------------------------------------------------------------
/**
    standaard function for qsort
    @return -if i0 < i1  return -1 else if i0=i1 return 0 else return 1
*/
int __cdecl 
nMeshBuilder::PosNormalUV::vertexSorter(const void* elm0, const void* elm1)
{
    PosNormalUV* i0 = (PosNormalUV*)elm0;
    PosNormalUV* i1 = (PosNormalUV*)elm1;

    return compare(*i0, *i1);

}
//------------------------------------------------------------------------------
/**
    Create the handedness component for vertex, need the handedness for triangle.
    If one vertex has a two handedness , create new vertex and change the triangle.
*/
void
nMeshBuilder::BuildVertexHandedness()
{
    int idx;
    int*  mirrorVertex = n_new_array( int, GetNumVertices() );
    n_assert(mirrorVertex);

    for ( idx = 0 ; idx < this->GetNumVertices() ; ++idx)
    {
        mirrorVertex[ idx ] = -1; // Mark not has a mirror
        this->GetVertexAt( idx ).SetHandedness( 0.0f ); // Mark not use
    }

    int index[3];
    int newIndex[3];
    for ( idx = 0; idx < this->GetNumTriangles() ; ++idx )
    {
        Triangle& tri = this->GetTriangleAt(idx);
        tri.GetVertexIndices(index[0], index[1], index[2]);
        for ( int i = 0 ; i < 3 ; i++)
        {
            Vertex&  vertex = GetVertexAt(index[i]);
            newIndex[i] = index[i];
            if ( 0.0f == vertex.GetHandedness() ) // handednees == 0 , 1.0 , -1.0 
            {
                vertex.SetHandedness( tri.GetHandedness() );

            } else if (  vertex.GetHandedness() != tri.GetHandedness() )  
            {
                if ( -1 == mirrorVertex[ index[i] ] ) // if not has a mirror , create it and apend to end
                {
                    Vertex newVertex = vertex;
                    newVertex.SetHandedness( tri.GetHandedness() );
                    mirrorVertex[ index[i] ] = GetNumVertices();
                    this->AddVertex( newVertex); //append to end
                }
                newIndex[i] = mirrorVertex[ index[i] ] ; // use alternative
            }
        }
        tri.SetVertexIndices( newIndex[0], newIndex[1], newIndex[2] );
    }

    for ( idx = 0; idx < this->GetNumTriangles() ; ++idx )
    {
        Triangle& tri = this->GetTriangleAt(idx);
        tri.GetVertexIndices(index[0], index[1], index[2]);
        n_assert( tri.GetHandedness()  == GetVertexAt( index[0] ).handedness &&
                  tri.GetHandedness()  == GetVertexAt( index[1] ).handedness &&
                  tri.GetHandedness()  == GetVertexAt( index[2] ).handedness );
    }

    n_delete_array( mirrorVertex );
}


//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
