#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nbatchscenebuilder.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nbatchscenebuilder.h"
#include "nscene/nstaticbatchnode.h"
#include "nscene/nsurfacenode.h"
#include "nmaterial/nmaterialnode.h"

//------------------------------------------------------------------------------
/**
*/
nBatchSceneBuilder::nBatchSceneBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBatchSceneBuilder::~nBatchSceneBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    assemble all opaque shapes for the depth pass,
    different only in geometry params
*/
nSceneNode*
nBatchSceneBuilder::BuildBatchedDepthScene()
{
    n_assert(this->refSceneRoot.isvalid());

    nArray<nShapeNode*> shapeNodes;
    this->BatchDepthShapesFromNode(this->refSceneRoot.get(), shapeNodes);

    // now build a nStaticBatchNode for the collected meshes
    if (shapeNodes.Size() > 1)
    {
        nArray<nString> shapeMeshes;
        for (int i = 0; i < shapeNodes.Size(); ++i)
        {
            shapeMeshes.Append(shapeNodes[i]->GetMesh());
            shapeNodes[i]->SetPassEnabled(FOURCC('dept'), false);
        }

        nKernelServer::ks->PushCwd(this->refSceneRoot.get());
        nStaticBatchNode* batchNode = (nStaticBatchNode*) this->refSceneRoot->Find("batch.depth");
        if (!batchNode)
        {
            batchNode = (nStaticBatchNode*) nKernelServer::ks->New("nstaticbatchnode", "batch.depth");
        }
        batchNode->SetMeshArray(shapeMeshes);
        //assign depth surface
        nSceneNode* depthSurface = this->BuildDepthSurface();
        batchNode->SetSurface(depthSurface->GetFullName().Get());

        //TODO- generate unique names for opaque depth shapes?
        nKernelServer::ks->PopCwd();
    }

    return this->refSceneRoot.get();
}

//------------------------------------------------------------------------------
/**
    for all static shape nodes, retrieve its surface, and its material
    for all materials with a depth pass, assemble together all static meshes
    and create a single mesh with a single depth shader (shared by every such shape)
    all opaque geometry from such a scene resource can be optimized to a single depth call
    opaque == (!alpha && !clipmap && !skinning && !swinging && !notculling & !depthbias)
*/
nSceneNode*
nBatchSceneBuilder::BatchDepthShapesFromNode(nSceneNode* parentNode, nArray<nShapeNode*>& shapeNodes)
{
    // create a node depending on the type of the current one
    const char* nodeClass = parentNode->GetClass()->GetName();

    if (!strcmp(nodeClass, "nshapenode"))
    {
        bool batchShape = false;
        nKernelServer::ks->PushCwd(parentNode);
        nSurfaceNode* surfaceNode = static_cast<nGeometryNode*>(parentNode)->GetSurfaceNode();
        nKernelServer::ks->PopCwd();
        n_assert(surfaceNode);

        if (surfaceNode->IsA("nmaterialnode"))
        {
            // check that the material doesn't use any per-shape texture or value
            const char* materialPath = static_cast<nMaterialNode*>(surfaceNode)->GetMaterial();
            nKernelServer::ks->PushCwd(surfaceNode);
            nMaterial* material = (nMaterial*) nKernelServer::ks->Lookup(materialPath);
            nKernelServer::ks->PopCwd();

            batchShape = !material->HasParam("mat", "shadowvolume") &&
                         !material->HasParam("mat", "singlecolorpass") &&
                         !material->HasParam("pro", "notculling") &&
                         !material->HasParam("pro", "depthbias") &&
                         !material->HasParam("map", "clipmap") &&
                         !material->HasParam("hasalpha", "true") &&
                         !material->HasParam("deform", "billboard") &&
                         !material->HasParam("deform", "swinging") &&
                         !material->HasParam("deform", "skinned") &&
                         !material->HasParam("deform", "morpher");
        }

        if (batchShape)
        {
            // add the mesh from the shape node to the batch list
            nShapeNode* shapeNode = static_cast<nShapeNode*>(parentNode);
            shapeNodes.Append(shapeNode);
        }
    }

    // traverse down subnodes
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) parentNode->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        this->BatchDepthShapesFromNode(curChild, shapeNodes);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    assemble all opaque shapes for the depth pass,
    different only in geometry params
*/
nSceneNode*
nBatchSceneBuilder::BuildBatchedDepthSceneByCell()
{
    n_assert(this->refSceneRoot.isvalid());

    //traverse all root nodes for indoor cells
    nSceneNode* cellNode;
    for (cellNode = (nSceneNode*) this->refSceneRoot->GetHead();
         cellNode;
         cellNode = (nSceneNode*) cellNode->GetSucc())
    {
        //traverse all root nodes for objects within cells
        nSceneNode* objectNode;
        for (objectNode = (nSceneNode*) cellNode->GetHead();
             objectNode;
             objectNode = (nSceneNode*) objectNode->GetSucc())
        {
            //batch depth shapes as usual for this
            this->SetRootNode(objectNode);
            this->BuildBatchedDepthScene();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
nBatchSceneBuilder::BuildDepthSurface()
{
    nSurfaceNode* depthSurface = (nSurfaceNode*) nKernelServer::ks->Lookup("/lib/surfaces/depth");
    if (!depthSurface)
    {
        depthSurface = (nSurfaceNode*) nKernelServer::ks->New("nsurfacenode", "/lib/surfaces/depth");
        depthSurface->SetShader(FOURCC('dept'), "shaders:default_dept.fx");
    }

    depthSurface->AddRef();

    return depthSurface;
}

//------------------------------------------------------------------------------
/**
    if there is more than a shape within the same resource with the same
    material, batch them all, and invalidate the passes for the original ones.
*/
nSceneNode*
nBatchSceneBuilder::BuildBatchedSceneByMaterial()
{
    n_assert(this->refSceneRoot.isvalid());

    this->shapesByMaterial.Clear();

    this->CollectShapesByMaterial(this->refSceneRoot.get());

    int materialIndex = 0;
    for (int i = 0; i < this->shapesByMaterial.Size(); ++i)
    {
        if (this->shapesByMaterial[i].shapes.Size() > 1)
        {
            nString batchName;
            batchName.Format("batch.%u", materialIndex++);
            this->shapesByMaterial[i].material->AddRef();

            nKernelServer::ks->PushCwd(this->refSceneRoot.get());
            nStaticBatchNode* batchNode = (nStaticBatchNode*) this->refSceneRoot->Find(batchName.Get());
            if (!batchNode)
            {
                batchNode = (nStaticBatchNode*) nKernelServer::ks->New("nstaticbatchnode", batchName.Get());
            }
            int numShapes = this->shapesByMaterial[i].shapes.Size();
            batchNode->BeginMeshes(numShapes);
            for (int shape = 0; shape < numShapes; ++shape)
            {
                batchNode->SetMeshAt(shape, this->shapesByMaterial[i].shapes[shape]->GetMesh());
                this->shapesByMaterial[i].shapes[shape]->Release();
            }
            batchNode->EndMeshes();
            batchNode->SetSurface(this->shapesByMaterial[i].material->GetFullName().Get());
            nKernelServer::ks->PopCwd();
        }
    }

    return this->refSceneRoot.get();
}

//------------------------------------------------------------------------------
/**
    assemble all shapes into a single shape- for debug purpose only
*/
void
nBatchSceneBuilder::CollectShapesByMaterial(nSceneNode* sceneRoot)
{
    if (!strcmp(sceneRoot->GetClass()->GetName(), "nshapenode"))
    {
        nShapeNode* geometry = static_cast<nShapeNode*>(sceneRoot);
        nSurfaceNode* material = static_cast<nGeometryNode*>(sceneRoot)->GetSurfaceNode();

        MaterialEntry* materialEntry = this->shapesByMaterial.Find(material);
        if (materialEntry)
        {
            materialEntry->shapes.Append(geometry);
        }
        else
        {
            MaterialEntry& newEntry = this->shapesByMaterial.PushBack(MaterialEntry(material));
            newEntry.shapes.Append(geometry);
        }
    }

    // recursively do the same for children
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) sceneRoot->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        this->CollectShapesByMaterial(curChild);
    }
}

//------------------------------------------------------------------------------
/**
    assemble all shapes into a single shape- for debug purpose only
*/
nSceneNode*
nBatchSceneBuilder::BuildFakeBatchLevel()
{
    n_assert(this->refSceneRoot.isvalid());

    return 0;
}
