#include "precompiled/pchndirect3d9.h"
//------------------------------------------------------------------------------
//  nd3d9mesh_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9mesh.h"
#include "kernel/nlogclass.h"

nNebulaClass(nD3D9Mesh, "nmesh2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Mesh::nD3D9Mesh() :
    d3dVBLockFlags(0),
    d3dIBLockFlags(0),
    vertexBuffer(0),
    indexBuffer(0),
    vertexDeclaration(0),
    privVertexBuffer(0),
    privIndexBuffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Mesh::~nD3D9Mesh()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    nD3D9Mesh support asynchronous resource loading.
*/
bool
nD3D9Mesh::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This method is either called directly from the nResource::Load() method
    (in synchronous mode), or from the loader thread (in asynchronous mode).
    The method must try to validate its resources, set the valid and pending
    flags, and return a success code.
    This method may be called from a thread.
*/
bool
nD3D9Mesh::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    bool success = nMesh2::LoadResource();

    if (success)
    {
        // create the vertex declaration from the vertex component mask
        this->CreateVertexDeclaration();
    }
    return success;
}

//------------------------------------------------------------------------------
/**
    Unload the d3d resources. Make sure that the resource are properly
    disconnected from the graphics server. This method is called from
    nResource::Unload() which serves as a wrapper for synchronous and
    asynchronous mode. This method will NEVER be called from a thread
    though.
*/
void
nD3D9Mesh::UnloadResource()
{
    n_assert(this->IsLoaded());

    nMesh2::UnloadResource();

    // release the d3d resource
    if (this->vertexBuffer)
    {
        this->vertexBuffer->Release();
        this->vertexBuffer = 0;
    }
    if (this->indexBuffer)
    {
        this->indexBuffer->Release();
        this->indexBuffer = 0;
    }
    if (this->vertexDeclaration)
    {
        this->vertexDeclaration->Release();
        this->vertexDeclaration = 0;
    }

    // release private buffers (if this is a ReadOnly mesh)
    if (this->privVertexBuffer)
    {
        n_free(this->privVertexBuffer);
        this->privVertexBuffer = 0;
    }
    if (this->privIndexBuffer)
    {
        n_free(this->privIndexBuffer);
        this->privIndexBuffer = 0;
    }

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is lost. We only need to
    react if our vertex and index buffers are not in D3D's managed pool.
    In this case, we need to unload ourselves...
*/
void
nD3D9Mesh::OnLost()
{
    if ( (WriteOnly & this->vertexUsage) && (this->IsValid() || this->IsEmpty()))
    {
        this->UnloadResource();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device has been restored. If our
    buffers are in the D3D's default pool, we need to restore ourselves
    as well, 
    
    Radon labs says: "and we need to set our state to empty, because the buffers 
    contain no data."
*/
void
nD3D9Mesh::OnRestored()
{
    if (WriteOnly & this->vertexUsage)
    {
        this->SetState(Unloaded);
        this->LoadResource();
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d vertex buffer and validate the vertexBuffer member.

    - 27-Sep-04     floh    DX7 compatibility fix: software processing meshes
                            now created in system mem, and without the WRITEONLY flag
*/
void
nD3D9Mesh::CreateVertexBuffer()
{
    n_assert(this->vertexBufferByteSize > 0);
    n_assert(0 == this->privVertexBuffer);
    n_assert(0 == this->vertexBuffer);
    n_assert(this->vertexBufferByteSize > 0);

    if (ReadOnly & this->vertexUsage)
    {
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // vertex buffer
        this->privVertexBuffer = n_malloc(this->vertexBufferByteSize);
        n_assert(this->privVertexBuffer);
    }
    else
    {
        nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
        n_assert(gfxServer->d3d9Device);

        // this is either a WriteOnce or a WriteOnly vertex buffer,
        // in both cases we create a D3D vertex buffer object
        HRESULT hr;

        DWORD d3dUsage = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool = D3DPOOL_MANAGED;
        this->d3dVBLockFlags = 0;
        if (WriteOnly & this->vertexUsage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dVBLockFlags = D3DLOCK_DISCARD;
        }
        if (ReadWrite & this->vertexUsage)
        {
            d3dUsage = D3DUSAGE_DYNAMIC;
            d3dPool  = D3DPOOL_SYSTEMMEM;
        }
        if (RTPatch & this->vertexUsage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->vertexUsage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }

        // create buffer with software processing flag
        // if the NeedsVertexShader hint is enabled, and the d3d device
        // has been created with software or mixed vertex processing
        if (gfxServer->AreVertexShadersEmulated() && (NeedsVertexShader & this->vertexUsage))
        {
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
            d3dUsage &= ~D3DUSAGE_WRITEONLY;
            d3dPool = D3DPOOL_SYSTEMMEM;
        }

        // create the vertex buffer
        hr = gfxServer->d3d9Device->CreateVertexBuffer(
                this->vertexBufferByteSize,
                d3dUsage,
                0,
                d3dPool,
                &(this->vertexBuffer),
                NULL);
        n_dxtrace(hr, "CreateVertexBuffer() failed in nD3D9Mesh");
        n_assert(this->vertexBuffer);
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d index buffer and validate the index buffer member.

    - 27-Sep-04     floh    DX7 compatibility fix: software processing meshes
                            now created in system mem, and without the WRITEONLY flag
*/
void
nD3D9Mesh::CreateIndexBuffer()
{
    n_assert(this->indexBufferByteSize > 0);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->privIndexBuffer);
    n_assert(this->indexBufferByteSize > 0);

    if (ReadOnly & this->indexUsage)
    {
        this->privIndexBuffer = n_malloc(this->indexBufferByteSize);
        n_assert(this->privIndexBuffer);
    }
    else
    {
        nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
        n_assert(gfxServer->d3d9Device);
        HRESULT hr;

        DWORD d3dUsage       = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool      = D3DPOOL_MANAGED;
        this->d3dIBLockFlags = 0;
        if (WriteOnly & this->indexUsage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dIBLockFlags = D3DLOCK_DISCARD;
        }
        if (ReadWrite & this->indexUsage)
        {
            d3dUsage = D3DUSAGE_DYNAMIC;
            d3dPool  = D3DPOOL_SYSTEMMEM;
        }
        if (RTPatch & this->indexUsage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->indexUsage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }
        // create buffer with software processing flag
        // if the NeedsVertexShader hint is enabled, and the d3d device
        // has been created with software or mixed vertex processing
        if (gfxServer->AreVertexShadersEmulated() && (NeedsVertexShader & this->vertexUsage))
        {
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
            d3dUsage &= ~D3DUSAGE_WRITEONLY;
            d3dPool = D3DPOOL_SYSTEMMEM;
        }

        hr = gfxServer->d3d9Device->CreateIndexBuffer(
                this->indexBufferByteSize,
                d3dUsage,
                D3DFMT_INDEX16,
                d3dPool,
                &(this->indexBuffer),
                NULL);
        n_dxtrace(hr, "CreateIndexBuffer failed in nD3D9Mesh");
        n_assert(this->indexBuffer);
    }
}

//------------------------------------------------------------------------------
/**
    Create the d3d9 vertex declaration from the vertex component mask.
*/
void
nD3D9Mesh::CreateVertexDeclaration()
{
    n_assert(0 == this->vertexDeclaration);
    nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
    n_assert(gfxServer->d3d9Device);
    this->vertexDeclaration = gfxServer->NewVertexDeclaration(this->vertexComponentMask);
    n_assert(this->vertexDeclaration);
}

//------------------------------------------------------------------------------
/**
    Lock the d3d vertex buffer and return pointer to it.
*/
float*
nD3D9Mesh::LockVertices()
{
    this->LockMutex();
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    float* retval = 0;
    if (this->vertexBuffer)
    {
        VOID* ptr;
        n_dxverify2(
            this->vertexBuffer->Lock(0, 0, &ptr, this->d3dVBLockFlags),
            "Lock() on vertex buffer failed in nD3D9Mesh()");
        n_assert(ptr);
        retval = (float*) ptr;
    }
    else
    {
        retval = (float*) this->privVertexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d vertex buffer locked by nD3D9Mesh::LockVertices().
*/
void
nD3D9Mesh::UnlockVertices()
{
    this->LockMutex();
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    if (this->vertexBuffer)
    {
        HRESULT hr;
        hr = this->vertexBuffer->Unlock();
        n_dxtrace(hr, "Unlock() on vertex buffer failed in nD3D9Mesh");
    }
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Lock the d3d index buffer and return pointer to it.
*/
ushort*
nD3D9Mesh::LockIndices()
{
    this->LockMutex();
    n_assert(this->indexBuffer || this->privIndexBuffer);
    ushort* retval = 0;
    if (this->indexBuffer)
    {
        VOID* ptr;
        n_dxverify2(
            this->indexBuffer->Lock(0, 0, &ptr, this->d3dIBLockFlags),
            "Lock on index buffer failed in nD3D9Mesh" );
        n_assert(ptr);
        retval = (ushort*) ptr;
    }
    else
    {
        retval = (ushort*) this->privIndexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d index buffer locked by nD3D9Mesh::LockIndices().
*/
void
nD3D9Mesh::UnlockIndices()
{
    this->LockMutex();
    n_assert(this->indexBuffer || this->privIndexBuffer);
    if (this->indexBuffer)
    {
        n_dxverify2(
            this->indexBuffer->Unlock(),
            "Unlock() on index buffer failed in nD3D9Mesh" );
    }
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Compute the byte size of the mesh data
*/
int
nD3D9Mesh::GetByteSize()
{
    if (this->IsValid())
    {
        int vertexBufferSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        int indexBufferSize  = this->GetNumIndices() * sizeof(ushort);
        return vertexBufferSize + indexBufferSize + nMesh2::GetByteSize();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Mesh::TransformCoordArray(float* vb, int offsetComponent, const matrix44& matrix, int numVertices, bool normal)
{
    const int stride = this->GetVertexWidth();

#if 0 //FIXME- D3DXVec3TransformNormalArray fails in AMD architecture
    const int strideBytes = stride * sizeof(float);
    D3DXVECTOR3* vector = (D3DXVECTOR3*) (vb + offsetComponent);
    if (normal)
    {
        D3DXVec3TransformNormalArray(vector, strideBytes, vector, strideBytes, (D3DXMATRIX*) matrix.m, numVertices);
    }
    else
    {
        D3DXVec3TransformCoordArray(vector, strideBytes, vector, strideBytes, (D3DXMATRIX*) matrix.m, numVertices);
    }
#else
    vb += offsetComponent;
    if (normal)
    {
        for (int idx = 0; idx < numVertices; ++idx)
        {
            vector3* vec = (vector3*) vb;
            *vec = matrix.transform_norm(*vec);
            vb += stride;
        }
    }
    else
    {
        for (int idx = 0; idx < numVertices; ++idx)
        {
            vector3* vec = (vector3*) vb;
            *vec = matrix * (*vec);
            vb += stride;
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Mesh::TransformTexCoordArray(float* vb, int offsetComponent, const matrix33& matrix, int numVertices)
{
    const int stride = this->GetVertexWidth();

    vb += offsetComponent;
    for (int idx = 0; idx < numVertices; ++idx)
    {
        vector2* vec = (vector2*) vb;
        *vec = matrix * (*vec);
        vb += stride;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
nD3D9Mesh::Transform(float* vb, int idxBegin, int numVertices, const matrix44& matrix)
{
    n_assert(vb);
   
    int stride = this->GetVertexWidth();
    vb += (idxBegin*stride);

    if (this->vertexComponentMask & nMesh2::Coord)
    {
        int offset = this->GetVertexComponentOffset( nMesh2::Coord );
        this->TransformCoordArray( vb, offset, matrix, numVertices, false);
    }
    
    int hasNormal = this->vertexComponentMask & nMesh2::Normal;
    int hasTan    = this->vertexComponentMask & nMesh2::Tangent;
    int hasBin    = this->vertexComponentMask & nMesh2::Binormal;

    if ( hasNormal || hasTan || hasBin)
    {
        matrix44 nor(matrix);
        nor.invert_simple();
        nor.transpose();

        if (hasNormal)
        {
            int offset = this->GetVertexComponentOffset( nMesh2::Normal );
            this->TransformCoordArray( vb, offset, matrix, numVertices, true);
        }

        if (hasTan)
        {
            int offset = this->GetVertexComponentOffset( nMesh2::Tangent );
            this->TransformCoordArray( vb, offset, matrix, numVertices, true);
        }

        if (hasBin)
        {
            int offset = this->GetVertexComponentOffset( nMesh2::Binormal);
            this->TransformCoordArray( vb, offset, matrix, numVertices, true);
        }
    }
   
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nD3D9Mesh::TransformUv(int layer, float* vb, int idxBegin, int numVertices, const matrix33& matrix)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    uint uvMask[nGfxServer2::MaxTextureStages] = { nMesh2::Uv0, nMesh2::Uv1, nMesh2::Uv2, nMesh2::Uv3 };

    n_assert(vb);
    vb += idxBegin * this->GetVertexWidth();

    if (this->vertexComponentMask & uvMask[layer])
    {
        int offset = this->GetVertexComponentOffset( nMesh2::Uv0 );
        this->TransformTexCoordArray(vb, offset, matrix, numVertices);
    }
    
    return true;
}

//------------------------------------------------------------------------------
/**
    optimize faces to improve the post-tnl cache
*/
void 
nD3D9Mesh::OptimizeFaces(ushort* indices, int numFaces, int numVertices)
{
    DWORD* pdwRemap = n_new_array(DWORD, numFaces);
    D3DXOptimizeFaces(indices, numFaces, numVertices, FALSE, pdwRemap);

    ushort* dstIndices = n_new_array(ushort, numFaces * 3);
    n_assert(dstIndices);
    memcpy(dstIndices, indices, numFaces * 6); // = 3 * sizeof(ushort)

    for (int i = 0; i < numFaces; ++i)
    {
        int newFace = (int) pdwRemap[i];
        for (int j = 0; j < 3; ++j)
        {
            indices[newFace * 3 + j] = dstIndices[i * 3 + j];
        }
    }

    n_delete_array(dstIndices);
    n_delete_array(pdwRemap);
}

//------------------------------------------------------------------------------
/**
    optimize vertices to improve the pre-tnl cache
*/
void
nD3D9Mesh::OptimizeVertices(float* vertices, ushort* indices, int numVertices, int numFaces)
{
    DWORD* pdwRemap = n_new_array(DWORD, numVertices);

    D3DXOptimizeVertices(indices, numFaces, numVertices, FALSE, pdwRemap);

    // remap vertices
    float* dstVertices = n_new_array(float, numVertices * this->GetVertexWidth());
    n_assert(dstVertices);
    memcpy(dstVertices, vertices, numVertices * this->GetVertexWidth() * sizeof(float));

    for (int i = 0; i < numVertices; ++i)
    {
        float* src = dstVertices + (i * this->GetVertexWidth());
        float* dst = vertices + (pdwRemap[i] * this->GetVertexWidth());
        memcpy(dst, src, this->GetVertexWidth() * sizeof(float));
    }

    // remap triangles
    for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex)
    {
        for (int index = 0; index < 3; ++index)
        {
            indices[faceIndex * 3 + index] = (ushort) pdwRemap[indices[faceIndex * 3 + index]];
        }
    }

    n_delete_array(dstVertices);
    n_delete_array(pdwRemap);
}

//------------------------------------------------------------------------------
/**
    Optimizes the mesh indices and vertices

    - 27-Nov-06     mateu.batle     created
*/
bool 
nD3D9Mesh::OptimizeMesh(OptimizationFlag flags, float * vertices, int numVertices, ushort * indices, int numIndices)
{
    n_assert_return(vertices, false);
    n_assert_return(numVertices > 0, false);
    n_assert_return(indices, false);
    n_assert_return(numIndices > 0, false);

    // optimize faces to improve the post-TnL cache
    if (flags & nMesh2::Faces)
    {
        this->OptimizeFaces(indices, numIndices / 3, numVertices);
    }

    // optimize vertices to improve the pre-tnl cache
    if (flags & nMesh2::Vertices)
    {
        this->OptimizeVertices(vertices, indices, numVertices, numIndices / 3);
    }

    return true;
}
