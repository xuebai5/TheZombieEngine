#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshapenode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nshapenode.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nShapeNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nShapeNode::nShapeNode() :
    groupIndex(0),
    primitiveType(nGfxServer2::TriangleList),
    meshUsage(nMesh2::WriteOnce)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShapeNode::~nShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShapeNode::UnloadMesh()
{
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Load new mesh, release old one if valid. Also initializes the groupIndex
    member.
*/
bool
nShapeNode::LoadMesh()
{
    if ((!this->refMesh.isvalid()) && (!this->meshName.IsEmpty()))
    {
        // append mesh usage to mesh resource name
        nString resourceName;
        resourceName.Format("%s_%d", this->meshName.Get(), this->GetMeshUsage());

        // get a new or shared mesh
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(resourceName.Get());
        n_assert(mesh);
        if (!mesh->IsLoaded())
        {
            mesh->SetFilename(this->meshName.Get());
            mesh->SetUsage(this->GetMeshUsage());

            if (this->refMeshResourceLoader.isvalid())
            {
                mesh->SetResourceLoader(this->refMeshResourceLoader.getname());
            }

            if (!mesh->Load())
            {
                n_printf("nMeshNode: Error loading mesh '%s'\n", this->meshName.Get());
                mesh->Release();
                return false;
            }
        }
        this->refMesh = mesh;
        this->SetLocalBox(this->refMesh->Group(this->groupIndex).GetBoundingBox());
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nShapeNode::LoadResources()
{
    if (nGeometryNode::LoadResources())
    {
        bool success = this->LoadMesh();

        //force sorting shapes with the same vertex declaration together
        if (success)
        {
            this->streamId |= (this->refMesh->GetVertexComponents() & 0x0000ffff)<<16;
        }
        return success;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nShapeNode::UnloadResources()
{
    this->UnloadMesh();
    nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Perform pre-instancing actions needed for rendering geometry. This
    is called once before multiple instances of this shape node are
    actually rendered.
*/
bool
nShapeNode::Apply(nSceneGraph* sceneGraph)
{
    if (nGeometryNode::Apply(sceneGraph))
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        n_assert(this->refMesh->IsValid());
        
        // set mesh, vertex and index range
        gfxServer->SetMesh(this->refMesh, this->refMesh);
        const nMeshGroup& curGroup = this->refMesh->Group(this->groupIndex);
        gfxServer->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
        gfxServer->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());
    
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexed().

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nShapeNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (nGeometryNode::Render(sceneGraph, entityObject))
    {
        nGfxServer2::Instance()->DrawIndexedNS(this->primitiveType);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Set the resource name. The mesh resource name consists of the
    filename of the mesh.
*/
void
nShapeNode::SetMesh(const char* name)
{
    n_assert(name);
    this->UnloadMesh();
    this->resourcesValid = false;
    this->meshName = name;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nShapeNode::GetMesh() const
{
    return this->meshName.IsEmpty() ? 0 : this->meshName.Get();
}

//------------------------------------------------------------------------------
/**
    Set the resource loader used to load the mesh data.  If this is NULL, then
    the mesh is loaded through the default mesh loading code.
*/
void
nShapeNode::SetMeshResourceLoader(const char* resourceLoaderPath)
{
    this->refMeshResourceLoader = resourceLoaderPath;
}

//------------------------------------------------------------------------------
/**
    Get the mesh resource loader.

    @return resource loader name or null when there is no resource loader
*/
const char *
nShapeNode::GetMeshResourceLoader()
{
    if (this->refMeshResourceLoader.isvalid())
    {
        return this->refMeshResourceLoader.getname();
    }
    else
    {
        return 0;
    }
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
    resolve debug mesh for current instance
*/
nMesh2*
nShapeNode::GetMeshResource()
{
    return this->refMesh.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    resolve debug mesh for current instance
*/
nMesh2*
nShapeNode::GetDebugMesh(nSceneGraph* /*sceneGraph*/, nEntityObject* /*entityObject*/, const char *name)
{
    n_assert(name);
    nString meshName;
    meshName.Format("%s_%s", this->GetMesh(), name);

    nMesh2* mesh = 0;
    mesh = static_cast<nMesh2*>(nResourceServer::Instance()->FindResource(meshName.Get(), nResource::Mesh));
    if (mesh)
    {
        return mesh;
    }

    if (this->refMesh.isvalid())
    {
        nMesh2::VertexComponent vertexcomp;

        // get vertex component
        if (nString(name) == "normal")
        {
            vertexcomp = nMesh2::Normal;
        }
        else if (nString(name) == "tangent")
        {
            vertexcomp = nMesh2::Tangent;
        }
        else if (nString(name) == "binormal")
        {
            vertexcomp = nMesh2::Binormal;
        }
        else
        {
            return 0;
        }

        // build mesh for drawing normals / tangents / binormals
        if (this->refMesh->GetVertexComponents() & vertexcomp)
        {
            int numVertices = this->refMesh->GetNumVertices();
            int vertexWidth = this->refMesh->GetVertexWidth();
            int offset = this->refMesh->GetVertexComponentOffset(vertexcomp);
            mesh = nGfxServer2::Instance()->NewMesh(meshName.Get());
            mesh->SetNumVertices(numVertices * 2);
            mesh->SetVertexComponents(nMesh2::Coord);
            mesh->Load();
            n_assert(mesh->IsEmpty());

            float* srcVertices = this->refMesh->LockVertices();
            float* dstVertices = mesh->LockVertices();
            n_assert(dstVertices);
            n_assert(srcVertices);
            for (int i = 0; i < numVertices; i++)
            {
                vector3 pos, other, sum;
                // copy position component
                pos.set(srcVertices[0], srcVertices[1], srcVertices[2]);
                // get vector for source component
                other.set(srcVertices[offset+0], srcVertices[offset+1], srcVertices[offset+2]);
                pos += other * .1f;
                memcpy(dstVertices, srcVertices, 3 * sizeof(float));
                dstVertices += 3;
                dstVertices[0] = pos.x;
                dstVertices[1] = pos.y;
                dstVertices[2] = pos.z;
                dstVertices += 3;
                srcVertices += vertexWidth;
            }
            mesh->UnlockVertices();
            this->refMesh->UnlockVertices();

            return mesh;
        }
    }

    return 0;
}
#endif
