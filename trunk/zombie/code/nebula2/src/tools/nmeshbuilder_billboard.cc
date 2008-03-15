#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//  nmeshbuilder_billboard.cc
//  (c) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
/**
*/
 void 
nMeshBuilder::TransformBillBoard()
{
    nArray< nArray<int> > vertexTriangleMap(0, 0);
    BuildVertexTriangleMap(vertexTriangleMap);// foreach vertex the triangles are shorted by index
    

    const int numVertex = GetNumVertices();
    const int numTriangles = GetNumTriangles();




    int triangleIndex;
    bool *TriangleNoExplored = new bool [numTriangles];
    for (triangleIndex=0; triangleIndex < numTriangles ; triangleIndex++ )
    {
        TriangleNoExplored[triangleIndex]=true;
    }

    int vertexIndex;
    
    for ( vertexIndex = 0; vertexIndex < numVertex; vertexIndex++)
    {
       const nArray<int> &listTriangle = vertexTriangleMap[vertexIndex];
       if (2 == listTriangle.Size())
       {
           const int t1=listTriangle[0];
           const int t2=listTriangle[1];

           if (TriangleNoExplored[t1]&&TriangleNoExplored[t2]) 
           {
               TriangleNoExplored[t1]=false;
               TriangleNoExplored[t2]=false;

               Quad quad;
               quad.set(this->triangleArray[t1],this->triangleArray[t2],this);
               quad.transformVertex();

           }
    
       }
        
    }
    delete [] TriangleNoExplored;
}

//------------------------------------------------------------------------------
/**
*/
void 
nMeshBuilder::BuildTrianglePlane(nArray<plane>& planeArray) const
{
  int triangleIndex=0;
    const int numTriangles = GetNumTriangles();
    planeArray.SetFixedSize(numTriangles);
    for (triangleIndex=0; triangleIndex<numTriangles; triangleIndex ++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);

        const Vertex& vertex0 = this->GetVertexAt(index[0]);
        const Vertex& vertex1 = this->GetVertexAt(index[1]);
        const Vertex& vertex2 = this->GetVertexAt(index[2]);

        const vector3& v0= vertex0.GetCoord();
        const vector3& v1= vertex1.GetCoord();
        const vector3& v2= vertex2.GetCoord();

        planeArray[triangleIndex].set(v0,v1,v2);
        
    }

}

//------------------------------------------------------------------------------
/**
*/
void 
nMeshBuilder::Quad::set (const Triangle& t1,const Triangle& t2,nMeshBuilder* mesh)
{
    n_assert(mesh);
    t1.GetVertexIndices(this->idxV[0],this->idxV[1],this->idxV[2]);
    int v[3];
    t2.GetVertexIndices(v[0],v[1],v[2]);
    int i;
    i=0;
    for (i=0 ; v[i] == idxV[0] || v[i] == idxV[1] || v[i] == idxV[2]; i++)
    {
        n_assert(i<4);
    }
    idxV[3]=v[i];
    myMesh=mesh;
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshBuilder::Quad::transformVertex()
{
    n_assert(myMesh);
    vector3 pivot(0.0f, 0.0f, 0.0f);
    pivot+=myMesh->vertexArray[idxV[0]].coord;
    pivot+=myMesh->vertexArray[idxV[1]].coord;
    pivot+=myMesh->vertexArray[idxV[2]].coord;
    pivot+=myMesh->vertexArray[idxV[3]].coord;

    pivot*=0.25f; //average point;


   // important idxv[0] ... idxv[2] is the same triangle and sort
    Vertex& vertex0 = myMesh->GetVertexAt(idxV[0]);
    Vertex& vertex1 = myMesh->GetVertexAt(idxV[1]);
    Vertex& vertex2 = myMesh->GetVertexAt(idxV[2]);
    

    // compute the face normal
    vector3 v0,v1;
    v0 = vertex1.GetCoord() - vertex0.GetCoord();
    v1 = vertex2.GetCoord() - vertex0.GetCoord();
    normal = v0 * v1;
    normal.norm();

    vector3 vecX;
    vector3 vecY;
    vecX=vector3(0.0,1.0,0.0)*normal; //cross product
    vecY=normal*vecX;

    int i;
    for (i=0; i<4 ; i++)
    {
        vector3 position;
        Vertex& vertex = myMesh->GetVertexAt(idxV[i]);
        position = vertex.coord - pivot; //translate position
        vector2 newUV;

        newUV.x = vecX % position; //dot product
        newUV.y = vecY % position;
        vertex.SetUv(3,newUV);
        vertex.SetCoord(pivot);
    }
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder::Quad::Quad() : myMesh(0)
{
}

//------------------------------------------------------------------------------
/**
*/
void 
nMeshBuilder::TransformSphericalNormals()
{
    vector3 centerPoint(0.0f,0.0f,0.0f);
    const int numVertex = GetNumVertices();
    int vertexIndex;
   
     // first calculate the center point
    /*
    int triangleIndex=0;
    const int numTriangles = GetNumTriangles();
    for (triangleIndex=0; triangleIndex<numTriangles; triangleIndex ++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);
        int i;
        for (i=0;i<3;i++)
        {
            centerPoint += this->GetVertexAt(index[i]).coord;
        }
    }

    float Count;
    Count = 3.0f*(float)numTriangles;
    */
    
    for ( vertexIndex = 0; vertexIndex < numVertex; vertexIndex++)
    {
         centerPoint += this->GetVertexAt(vertexIndex).coord;
    }
    float Count;
    Count = (float)numVertex;

    centerPoint*=(1.0f/Count); // average

    // Calculate new normals    
    for ( vertexIndex = 0; vertexIndex < numVertex; vertexIndex++)
    {
        vector3 normal;
        Vertex& vertex0 = vertexArray[vertexIndex];
        normal = vertex0.coord - centerPoint;
        normal.norm();
        vertex0.SetNormal(normal);

    }
}
