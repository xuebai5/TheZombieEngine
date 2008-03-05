#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nstaticbatchnode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nstaticbatchnode.h"
#include "nscene/nbatchmeshloader.h"

nNebulaScriptClass(nStaticBatchNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nStaticBatchNode::nStaticBatchNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStaticBatchNode::~nStaticBatchNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetMeshArray(nArray<nString>& meshArray)
{
    //TODO- this can't happen if the mesh is already loaded: invalidate it!
    n_assert(!meshArray.Empty());
    this->BeginMeshes(meshArray.Size());
    int i;
    for (i = 0; i < meshArray.Size(); ++i)
    {
        this->SetMeshAt(i, meshArray[i].Get());
    }
    this->EndMeshes();
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::BeginMeshes(int count)
{
    this->meshArray.SetFixedSize(count);
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetMeshAt(int index, const char *mesh)
{
    this->meshArray.At(index).meshFile.Set(mesh);
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetPositionAt(int index, const vector3& pos)
{
    this->meshArray.At(index).modelTransform.settranslation(pos);
    this->meshArray.At(index).hasTransform = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetEulerAt(int index, const vector3& euler)
{
    this->meshArray.At(index).modelTransform.seteulerrotation(euler);
    this->meshArray.At(index).hasTransform = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetQuatAt(int index, const quaternion& quat)
{
    this->meshArray.At(index).modelTransform.setquatrotation(quat);
    this->meshArray.At(index).hasTransform = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::SetScaleAt(int index, const float scale)
{
    this->meshArray.At(index).modelTransform.setscale(vector3(scale, scale, scale));
    this->meshArray.At(index).hasTransform = true;
}

//------------------------------------------------------------------------------
/**
*/
const char *
nStaticBatchNode::GetMeshAt(int index)
{
    return this->meshArray.At(index).meshFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
bool
nStaticBatchNode::HasTransformAt(int index)
{
    return this->meshArray.At(index).hasTransform;
}

//------------------------------------------------------------------------------
/**
*/
const matrix44&
nStaticBatchNode::GetTransformAt(int index)
{
    return this->meshArray.At(index).modelTransform.getmatrix();
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticBatchNode::EndMeshes()
{
    //
}

//------------------------------------------------------------------------------
/**
*/
int
nStaticBatchNode::GetNumMeshes()
{
    return this->meshArray.Size();
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nStaticBatchNode::LoadResources()
{
    if (nGeometryNode::LoadResources())
    {
        if (!this->refMesh.isvalid() && this->meshArray.Size() > 0)
        {
            // get a new or shared mesh
            nMesh2* mesh = nGfxServer2::Instance()->NewMesh(this->GetFullName().Get());
            n_assert(mesh);
            if (!mesh->IsLoaded())
            {
                mesh->SetFilename(this->GetFullName().Get());
                mesh->SetUsage(this->GetMeshUsage());
                mesh->SetResourceLoader(nBatchMeshLoader::Instance()->GetFullName().Get());

                if (!mesh->Load())
                {
                    n_printf("nStaticBatchNode: Error loading batch: '%s'\n", this->GetFullName().Get());
                    mesh->Release();
                    return false;
                }

                this->refMesh = mesh;
            }
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nStaticBatchNode::UnloadResources()
{
    // unload mesh from set of pieces
    nShapeNode::UnloadResources();
}
