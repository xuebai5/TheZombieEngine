#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrenderpathnode_stats.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#if __NEBULA_STATS__

#include "nscene/nrenderpathnode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/ninstshapenode.h"
#include "nscene/nshapenode.h"

// helper lists to track which classes have been already visited
nArray<nTexture2*> nRenderPathNode::visitedTextures;
nArray<nMesh2*> nRenderPathNode::visitedMeshes;

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::BeginResourceStats()
{
    if (!this->statsLevel)
    {
        return;
    }

    visitedMeshes.Reset();
    visitedTextures.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::CollectGeometryStats(nGeometryNode* geometry)
{
    #ifndef NGAME
    static nClass* shapeNodeClass(kernelServer->FindClass("nshapenode"));
    static nClass* instShapeNodeClass(kernelServer->FindClass("ninstshapenode"));

    // retrieve texture stats from surface node
    nSurfaceNode* surfaceNode = geometry->GetSurfaceNode();
    if (surfaceNode)
    {
        this->CollectTextureStats(surfaceNode);
    }

    // retrieve mesh stats from streamed nodes
    nGeometryNode* streamNode = geometry->GetStreamGeometry();
    if (streamNode)
    {
        if (streamNode->IsA(instShapeNodeClass))
        {
            nMesh2* instMesh = static_cast<nInstShapeNode*>(streamNode)->GetMeshResource();
            if (instMesh && !visitedMeshes.Find(instMesh))
            {
                visitedMeshes.Append(instMesh);
            }
        }
    }
    else
    {
        // retrieve mesh stats from shape node
        if (geometry->IsA(shapeNodeClass))
        {
            nMesh2* mesh = static_cast<nShapeNode*>(geometry)->GetMeshResource();
            if (mesh && !visitedMeshes.Find(mesh))
            {
                visitedMeshes.Append(mesh);
            }
        }
    }
    //TODO- retrieve mesh stats from terrain nodes, etc.
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::CollectTextureStats(nAbstractShaderNode* textureNode)
{
    // collect texture ids
    for (int tex = 0; tex < textureNode->GetNumTextures(); ++tex)
    {
        nShaderState::Param param = textureNode->GetTextureParamAt(tex);
        if (textureNode->GetShaderParams().IsParameterValid(param))
        {
            nTexture2* texture = textureNode->GetShaderParams().GetArg(param).GetTexture();
            if (texture && !visitedTextures.Find(texture))
            {
                visitedTextures.Append(texture);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::EndResourceStats()
{
    if (!this->statsLevel)
    {
        return;
    }

    this->statsGeometrySize = 0;
    for (int index = 0; index < visitedMeshes.Size(); ++index)
    {
        this->statsGeometrySize += visitedMeshes[index]->GetByteSize();
    }

    this->statsTextureSize = 0;
    for (int index = 0; index < visitedTextures.Size(); ++index)
    {
        this->statsTextureSize += visitedTextures[index]->GetByteSize();
    }
}

#endif
