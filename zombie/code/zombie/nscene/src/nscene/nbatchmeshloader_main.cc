#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nbatchmeshloader_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nbatchmeshloader.h"
#include "nscene/ngeometrynode.h"
#include "nscene/ninstshapenode.h"
#include "nscene/nstaticbatchnode.h"
#include "nscene/nstreamgeometrynode.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nBatchMeshLoader, "nresourceloader");

nBatchMeshLoader *nBatchMeshLoader::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nBatchMeshLoader::nBatchMeshLoader()
{
    if (!this->Singleton)
    {
        this->Singleton = this;
    }
}

//------------------------------------------------------------------------------
/**
*/
nBatchMeshLoader::~nBatchMeshLoader()
{
    if (this->Singleton == this)
    {
        this->Singleton = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nBatchMeshLoader*
nBatchMeshLoader::Instance()
{
    if (!nBatchMeshLoader::Singleton)
    {
        nBatchMeshLoader::Singleton = static_cast<nBatchMeshLoader*> (kernelServer->New("nbatchmeshloader", "/sys/loaders/batchmesh"));
        n_assert(nBatchMeshLoader::Singleton);
    }

    return nBatchMeshLoader::Singleton;
}

//------------------------------------------------------------------------------
/**
    Base Load() function for the nResourceLoader.  This is to be redefined by child classes

    @param filename         the full path to the to-be-loaded file
    @param callingResource  ptr to the nResource calling nResourceLoader::Load()

    @return                 success/failure
*/
bool
nBatchMeshLoader::Load(const char* filename, nResource *callingResource)
{
    n_assert(callingResource);
    n_assert(callingResource->IsA("nmesh2"));

    nGeometryNode* geometryNode = static_cast<nGeometryNode*>(kernelServer->Lookup(filename));
    if (!geometryNode)
    {
        return false;
    }

    if (geometryNode->IsA("ninstshapenode"))
    {
        return this->LoadInstancedMesh((nInstShapeNode*) geometryNode, (nMesh2*) callingResource);
    }

    if (geometryNode->IsA("nstaticbatchnode"))
    {
        return this->LoadStaticBatchMesh((nStaticBatchNode*) geometryNode, (nMesh2*) callingResource);
    }

    if (geometryNode->IsA("nstreamgeometrynode"))
    {
        return this->LoadStreamBatchMesh((nStreamGeometryNode*) geometryNode, (nMesh2*) callingResource);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nBatchMeshLoader::LoadInstancedMesh(nInstShapeNode* instNode, nMesh2* dstMesh)
{
    n_assert(instNode);
    n_assert(dstMesh);

    nMesh2* srcMesh;

    nGeometryNode *shapeNode = instNode->GetShapeNode();
    n_assert(shapeNode);

    if (shapeNode->IsA("nstaticbatchnode"))
    {
        // recursively load static batches using my own interface
        srcMesh = nGfxServer2::Instance()->NewMesh(instNode->GetShape());
        n_assert(srcMesh);
        srcMesh->SetUsage(nMesh2::ReadOnly);
        srcMesh->SetFilename(instNode->GetShape());
        if (!this->LoadStaticBatchMesh(static_cast<nStaticBatchNode*>(shapeNode), srcMesh))
        {
            srcMesh->Release();
            return false;
        }
    }
    else if (shapeNode->IsA("nshapenode"))
    {
        // single mesh
        srcMesh = this->LoadMesh(static_cast<nShapeNode*>(shapeNode)->GetMesh());
        if (!srcMesh)
        {
            return false;
        }
    }
    else
    {
        //unknown primitive type
        return false;
    }

    // collect mesh info from original mesh
    int srcNumVertices = srcMesh->GetNumVertices();
    int srcNumIndices = srcMesh->GetNumIndices();
    int srcVertexWidth = srcMesh->GetVertexWidth();

    int numInstances = instNode->GetNumInstances();

    // limit number of vertices to maximum allowed by a 16-bit index
    if (srcNumVertices * numInstances >= nMesh2::InvalidIndex)
    {
        numInstances = nMesh2::InvalidIndex / srcNumVertices;
        instNode->SetNumInstances(numInstances);
    }

    // create the pre-batched mesh with as many instances as needed.
    int dstVertexMask = srcMesh->GetVertexComponents() | nMesh2::JIndices;
    int dstNumIndices = srcNumIndices * numInstances;
    int dstNumVertices = srcNumVertices * numInstances;
    n_assert(dstNumVertices < nMesh2::InvalidIndex);

    instNode->SetNumVerticesPerInstance(srcMesh->GetNumVertices());
    instNode->SetNumIndicesPerInstance(srcMesh->GetNumIndices());

    dstMesh->SetNumVertices(dstNumVertices);
    dstMesh->SetNumIndices(dstNumIndices);
    dstMesh->SetVertexComponents(dstVertexMask);
    dstMesh->SetNumGroups(1);
    dstMesh->Group(0).SetFirstVertex(0);
    dstMesh->Group(0).SetNumVertices(dstNumVertices);
    dstMesh->Group(0).SetFirstIndex(0);
    dstMesh->Group(0).SetNumIndices(dstNumIndices);
    n_verify(dstMesh->CreateEmpty());

    // lock vertices and indices
    float* dstVertices = dstMesh->LockVertices();
    ushort* dstIndices = dstMesh->LockIndices();

    float* srcVertices = srcMesh->LockVertices();
    ushort* srcIndices = srcMesh->LockIndices();
    
    int dstVertexWidth = dstMesh->GetVertexWidth();

    // for every instance add the instance index to each vertex
    for (int instIndex = 0; instIndex < numInstances; ++instIndex)
    {
        for (int i = 0; i < srcNumVertices; ++i)
        {
            // copy mesh vertex
            float *dst = dstVertices + (dstVertexWidth * i) + (instIndex * srcNumVertices * dstVertexWidth);
            float *src = srcVertices + (srcVertexWidth * i);
            memcpy(dst, src, srcVertexWidth * sizeof(float));

            // add instance index- use JointIndices component
            dst[dstVertexWidth - 4] = (float) instIndex;
            dst[dstVertexWidth - 3] = 0.0f;
            dst[dstVertexWidth - 2] = 0.0f;
            dst[dstVertexWidth - 1] = 0.0f;
        }

        for (int i = 0; i < srcNumIndices; i++)
        {
            // copy index with appropiate index conversion
            dstIndices[i + (instIndex * srcNumIndices)] = (ushort) (srcIndices[i] + (instIndex * srcNumVertices));
        }
    }

    // unlock vertices and indices
    srcMesh->UnlockVertices();
    srcMesh->UnlockIndices();

    dstMesh->UnlockVertices();
    dstMesh->UnlockIndices();

    srcMesh->Release();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nBatchMeshLoader::LoadStreamBatchMesh(nStreamGeometryNode* batchNode, nMesh2* dstMesh)
{
    n_assert(batchNode);
    n_assert(dstMesh);
    
    //1- collect all source meshes, compute input vertex mask, batch size
    int dstNumVertices = 0;
    int dstNumIndices = 0;
    /// @todo ma.garcias - let the node state the vertex mask
    int dstVertexMask = nMesh2::AllComponents;

    // load all the primitive meshes
    nArray<nMesh2*> meshes;
    // push the stream node to retrieve shapes in relative paths
    kernelServer->PushCwd(batchNode);
    int numShapes = batchNode->GetNumShapes();
    for (int index = 0; index < numShapes; ++index)
    {
        nGeometryNode* shapeNode = batchNode->GetShapeNodeAt(index);
        n_assert(shapeNode);

        nMesh2* srcMesh = 0;
        if (shapeNode->IsA("nstaticbatchnode"))
        {
            // recursively load static batches using my own interface
            srcMesh = nGfxServer2::Instance()->NewMesh(0);
            n_assert(srcMesh);
            srcMesh->SetUsage(nMesh2::ReadOnly);
            srcMesh->SetFilename(shapeNode->GetFullName().Get());
            if (!this->LoadStaticBatchMesh(static_cast<nStaticBatchNode*>(shapeNode), srcMesh))
            {
                srcMesh->Release();
                srcMesh = 0;
            }
        }
        else if (shapeNode->IsA("nshapenode"))
        {
            // load single mesh
            srcMesh = this->LoadMesh(static_cast<nShapeNode*>(shapeNode)->GetMesh());
        }

        //add the mesh to the list
        if (srcMesh)
        {
            meshes.Append(srcMesh);
            dstNumVertices += srcMesh->GetNumVertices() * batchNode->GetFrequencyAt(index);
            dstNumIndices += srcMesh->GetNumIndices() * batchNode->GetFrequencyAt(index);
            dstVertexMask &= srcMesh->GetVertexComponents();
        }
        else
        {
            for (int index = 0; index < meshes.Size(); ++index)
            {
                meshes[index]->Release();
            }
            return false;
        }
    }
    kernelServer->PopCwd();

    if (dstNumVertices == 0)
    {
        return false;
    }

    //2- initialize the target mesh
    //this->InitializeMesh(vertexCompMask, numVertices, numIndices);
    dstVertexMask |= nMesh2::JIndices;
    dstMesh->SetVertexComponents(dstVertexMask);//force using the jindices
    dstMesh->SetNumVertices(dstNumVertices);
    dstMesh->SetNumIndices(dstNumIndices);
    dstMesh->SetNumGroups(1);
    dstMesh->Group(0).SetFirstVertex(0);
    dstMesh->Group(0).SetNumVertices(dstNumVertices);
    dstMesh->Group(0).SetFirstIndex(0);
    dstMesh->Group(0).SetNumIndices(dstNumIndices);
    n_verify(dstMesh->CreateEmpty());

    // compute component offset for every component of the output mesh, -1 if none
    int dstVertexWidth = dstMesh->GetVertexWidth();
    int dstVertexOffset[nMesh2::NumVertexComponents];// vertex offset for batched components
    for (int v = 0; v < nMesh2::NumVertexComponents; ++v)
    {
        nMesh2::VertexComponent vertexComp = static_cast<nMesh2::VertexComponent>(1<<v);
        dstVertexOffset[v] = dstVertexMask & (1<<v) ? dstMesh->GetVertexComponentOffset(vertexComp) : -1;
    }

    //3- fill buffers
    float* dstVertices = dstMesh->LockVertices();
    ushort* dstIndices = dstMesh->LockIndices();
    int batchedVertices = 0;
    int batchedIndices = 0;

    int slotIndex = 0;
    for (int meshIndex = 0; meshIndex < meshes.Size(); ++meshIndex)
    {
        // select source mesh for this slot depending on frequency
        nMesh2* srcMesh = meshes[meshIndex];

        // copy mesh into the target batch
        int srcNumVertices = srcMesh->GetNumVertices();
        int srcNumIndices = srcMesh->GetNumIndices();
        int srcVertexWidth = srcMesh->GetVertexWidth();
        int srcVertexOffset[nMesh2::NumVertexComponents];// vertex offset for batched components
        int srcVertexMask = srcMesh->GetVertexComponents();

        // get vertex mask for the source mesh
        int v;
        for (v = 0; v < nMesh2::NumVertexComponents; ++v)
        {
            nMesh2::VertexComponent vertexComp = static_cast<nMesh2::VertexComponent>(1<<v);
            srcVertexOffset[v] = srcVertexMask&(1<<v) ? srcMesh->GetVertexComponentOffset(vertexComp) : -1;
        }

        float* srcVertices = srcMesh->LockVertices();
        ushort* srcIndices = srcMesh->LockIndices();

        // copy all instances of this mesh
        for (int instIndex = 0; instIndex < batchNode->GetFrequencyAt(meshIndex); ++instIndex)
        {
            float instanceIndex[4] = {(float) slotIndex, 0.0f, 0.0f, 0.0f};

            // copy vertex components
            for (int i = 0; i < srcNumVertices; ++i)
            {
                float *dst = dstVertices + (dstVertexWidth * i) + (batchedVertices * dstVertexWidth);
                float *src = srcVertices + (srcVertexWidth * i);

                for (int v = 0; v < nMesh2::NumVertexComponents; ++v)
                {
                    if (dstVertexMask & (1<<v))
                    {
                        int compWidth = dstMesh->GetVertexWidthFromMask(1<<v);
                        int dstOffset = dstVertexOffset[v];//!=-1
                        if ((1<<v) == nMesh2::JIndices)
                        {
                            memcpy(dst + dstOffset, instanceIndex, compWidth * sizeof(float));
                        }
                        else
                        {
                            int srcOffset = srcVertexOffset[v];//!=-1
                            memcpy(dst + dstOffset, src + srcOffset, compWidth * sizeof(float));
                        }
                    }
                }
            }

            // transform texture coordinates
            for (int layer = 0; layer < nGfxServer2::MaxTextureStages; ++layer)
            {
                if (batchNode->HasUvTransformAt(meshIndex, layer))
                {
                    dstMesh->TransformUv(layer, dstVertices, batchedVertices, srcNumVertices, batchNode->GetUvTransformAt(meshIndex, layer));
                }
            }

            // copy indices as original index + index offset
            for (int i = 0; i < srcNumIndices; ++i)
            {
                dstIndices[i + batchedIndices] = (ushort) (srcIndices[i] + batchedVertices);
            }

            batchedVertices += srcNumVertices;
            batchedIndices += srcNumIndices;

            ++slotIndex;
        }

        // release source mesh
        srcMesh->UnlockVertices();
        srcMesh->UnlockIndices();
        srcMesh->Release();
    }

    dstMesh->UnlockVertices();
    dstMesh->UnlockIndices();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nBatchMeshLoader::LoadStaticBatchMesh(nStaticBatchNode* batchNode, nMesh2* batchMesh)
{
    n_assert(batchNode);
    n_assert(batchMesh);

    //1- collect all source meshes, compute vertex component mask
    int numVertices = 0;
    int numIndices = 0;
    /// @todo ma.garcias - set only coord for depth batches
    int batchVertexMask = nMesh2::AllComponents;
    
    bbox3 localBox;
    localBox.begin_extend();

    // get an array of all loaded meshes and totalize their vertex size
    nArray<nMesh2*> meshes;

    int i;
    int numMeshes = batchNode->GetNumMeshes();
    for (i = 0; i < numMeshes; ++i)
    {
        if (batchNode->GetMeshAt(i))
        {
            // get a new or shared mesh
            nMesh2* mesh = this->LoadMesh(batchNode->GetMeshAt(i));
            if (mesh)
            {
                batchVertexMask &= mesh->GetVertexComponents();// collect only common vertex components
                numVertices += mesh->GetNumVertices();
                numIndices += mesh->GetNumIndices();
                localBox.extend(mesh->Group(batchNode->GetGroupIndex()).GetBoundingBox());
                meshes.Append(mesh);
            }
            else
            {
                for (int index = 0; index < meshes.Size(); ++index)
                {
                    meshes[index]->Release();
                }
                return false;
            }
        }
    }

    //update local box in the target node
    localBox.end_extend();
    batchNode->SetLocalBox(localBox);

    if (numVertices == 0)
    {
        return false;
    }

    //2- initialize target mesh
    batchMesh->SetVertexComponents(batchVertexMask);
    batchMesh->SetNumVertices(numVertices);
    batchMesh->SetNumIndices(numIndices);
    batchMesh->SetNumGroups(1);
    batchMesh->Group(0).SetFirstVertex(0);
    batchMesh->Group(0).SetNumVertices(numVertices);
    batchMesh->Group(0).SetFirstIndex(0);
    batchMesh->Group(0).SetNumIndices(numIndices);
    n_verify(batchMesh->CreateEmpty());

    int batchVertexWidth = batchMesh->GetVertexWidth();
    int batchVertexOffset[nMesh2::NumVertexComponents];// vertex offset for batched components
    int v;
    for (v = 0; v < nMesh2::NumVertexComponents; ++v)
    {
        nMesh2::VertexComponent vertexComp = static_cast<nMesh2::VertexComponent>(1<<v);
        batchVertexOffset[v] = batchVertexMask & (1<<v) ? batchMesh->GetVertexComponentOffset(vertexComp) : -1;
    }

    //3- fill vertex and index buffers
    float* batchVertices = batchMesh->LockVertices();
    ushort* batchIndices = batchMesh->LockIndices();
    int batchedVertices = 0;
    int batchedIndices = 0;

    for (i = 0; i < meshes.Size(); ++i)
    {
        nMesh2* mesh = meshes[i];

        // copy mesh into the target batch
        int numMeshVertices = mesh->GetNumVertices();
        int numMeshIndices = mesh->GetNumIndices();
        int meshVertexWidth = mesh->GetVertexWidth();
        int meshVertexOffset[nMesh2::NumVertexComponents];// vertex offset for batched components
        int meshVertexMask = mesh->GetVertexComponents();
        int v;
        for (v = 0; v < nMesh2::NumVertexComponents; ++v)
        {
            nMesh2::VertexComponent vertexComp = static_cast<nMesh2::VertexComponent>(1<<v);
            meshVertexOffset[v] = meshVertexMask&(1<<v) ? mesh->GetVertexComponentOffset(vertexComp) : -1;
        }

        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();

        // copy vertex components
        int j;
        for (j = 0; j < numMeshVertices; ++j)
        {
            float *dst = batchVertices + (batchVertexWidth * j) + (batchedVertices * batchVertexWidth);
            float *src = vertices + (meshVertexWidth * j);

            int v;
            for (v = 0; v < nMesh2::NumVertexComponents; ++v)
            {
                if (batchVertexMask & (1<<v))
                {
                    int batchOffset = batchVertexOffset[v];//(!= -1)
                    int meshOffset = meshVertexOffset[v];//(!= -1)
                    int compWidth = mesh->GetVertexWidthFromMask(1<<v);
                    memcpy(dst + batchOffset, src + meshOffset, compWidth * sizeof(float));
                }
            }
        }

        // transform this instance:
        // needs to use the hasTransform, otherwise there is some depth-fighting
        if (batchNode->HasTransformAt(i))
        {
            batchMesh->Transform(batchVertices, batchedVertices, numMeshVertices, batchNode->GetTransformAt(i));
        }
        
        // copy indices
        for (j = 0; j < numMeshIndices; ++j)
        {
            // copy index adding the appropiate index offset
            batchIndices[j + batchedIndices] = (ushort) (indices[j] + batchedVertices);
        }

        mesh->UnlockVertices();
        mesh->UnlockIndices();

        mesh->Release();

        batchedVertices += numMeshVertices;
        batchedIndices += numMeshIndices;
    }

    // unlock vertices and indices
    batchMesh->UnlockVertices();
    batchMesh->UnlockIndices();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nMesh2*
nBatchMeshLoader::LoadMesh(const char *filename)
{
    // get a new or shared mesh
    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(filename);
    n_assert(mesh);

    mesh->SetFilename(filename);
    mesh->SetUsage(nMesh2::ReadOnly);

    if (!mesh->Load())
    {
        n_printf("nMeshNode: Error loading mesh '%s'\n", filename);
        mesh->Release();
        return 0;
    }
    
    if (mesh->IsLoaded())
    {
        return mesh;
    }
    else
    {
        mesh->Release();
        return 0;
    }
}
