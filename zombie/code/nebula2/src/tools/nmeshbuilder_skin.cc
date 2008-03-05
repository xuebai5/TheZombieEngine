#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//  nmeshbuilder_skin.cc
//  (C) 2006 Traganaion
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
/**
    Calculate the weight and skin indices in the baricentric of triangle.

*/
void
nMeshBuilder::BuildTriangleWeights()
{
    
    const int numTriangles = this->GetNumTriangles();
    for (int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);
        nArray<SkinIndexAndWeight> indexAndWeightList1(12,16);
        nArray<SkinIndexAndWeight> indexAndWeightList2(12,16);

        //The baricentric point is (Vertex[0].coord + Vertex[1].coord + vertex[2].coord) / 3
        //Use the theory the center of mass http://en.wikipedia.org/wiki/Barycenter
        //                                  http://es.wikipedia.org/wiki/Baricentro
        // The mass in each vertex is 1

        const float avg(1.f/3.f); // In the baricentric all vertex has the same influence ( mass 1)
        for (int idx=0; idx<3;++idx)
        {
            const Vertex& vertex0 = this->GetVertexAt(index[idx]);
            const vector4& jointInidices = vertex0.GetJointIndices();
            const vector4& weights = vertex0.GetWeights();
            indexAndWeightList1.Append( SkinIndexAndWeight( int(jointInidices.x), avg*weights.x) );
            indexAndWeightList1.Append( SkinIndexAndWeight( int(jointInidices.y), avg*weights.y) );
            indexAndWeightList1.Append( SkinIndexAndWeight( int(jointInidices.z), avg*weights.z) );
            indexAndWeightList1.Append( SkinIndexAndWeight( int(jointInidices.w), avg*weights.w) );
        }

        // Now unify the same joint index
        indexAndWeightList1.QSort( SkinIndexAndWeight::SorterByIndex);
        int previousIndex = indexAndWeightList1[0].idx;
        indexAndWeightList2.Append(indexAndWeightList1[0]);
        for ( int idx = 1; idx < indexAndWeightList1.Size(); ++idx)
        {
            const SkinIndexAndWeight& indexAndWeight = indexAndWeightList1[idx];
            if ( indexAndWeight.idx != previousIndex )
            {
                previousIndex = indexAndWeight.idx ;
                indexAndWeightList2.Append(indexAndWeight);
            } else
            {
                indexAndWeightList2.Back().weight += indexAndWeight.weight;
            }
        }

        while (indexAndWeightList2.Size() < 4 ) //Always need 4 skin indices
        {
            indexAndWeightList2.Append(SkinIndexAndWeight(0,0.f)); //Append  the null weight
        }
        
        // Select the the four index of most significant weight
        indexAndWeightList2.QSort( SkinIndexAndWeight::SorterByWeight);
        vector4 jointInidices((float)indexAndWeightList2[0].idx,
                                (float)indexAndWeightList2[1].idx,
                                (float)indexAndWeightList2[2].idx,
                                (float)indexAndWeightList2[3].idx);
        tri.SetJointIndices(jointInidices);
                                
        vector4 weights(indexAndWeightList2[0].weight,
                        indexAndWeightList2[1].weight, 
                        indexAndWeightList2[2].weight,
                        indexAndWeightList2[3].weight);
        float avgWeight= ( weights.x + weights.y + weights.z + weights.w); 
        avgWeight = fabs(avgWeight) < N_TINY ? 1.f : (1.f/avgWeight);
        tri.SetWeights(weights*avgWeight);

    }
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder::SkinIndexAndWeight::SkinIndexAndWeight()
{

}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder::SkinIndexAndWeight::SkinIndexAndWeight(int idx, float weight) :
    idx(idx),
    weight(weight)
{
}

nMeshBuilder::SkinIndexAndWeight::~SkinIndexAndWeight()
{
    //empty
}

int 
nMeshBuilder::SkinIndexAndWeight::SorterByWeight(const void* elm0, const void* elm1)
{
    const SkinIndexAndWeight* e0 = static_cast<const SkinIndexAndWeight*>(elm0);
    const SkinIndexAndWeight* e1 = static_cast<const SkinIndexAndWeight*>(elm1);

    if ( e0->weight > e1->weight )
    {
        return -1;
    } else if ( e0->weight < e1->weight )
    {
        return 1;
    } else
    {
        return e0->idx - e1->idx;
    }
}

int 
nMeshBuilder::SkinIndexAndWeight::SorterByIndex(const void* elm0, const void* elm1)
{
    const SkinIndexAndWeight* e0 = static_cast<const SkinIndexAndWeight*>(elm0);
    const SkinIndexAndWeight* e1 = static_cast<const SkinIndexAndWeight*>(elm1);
    return e0->idx - e1->idx;
}