#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nentitybatchbuilder_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nentitybatchbuilder.h"
#include "entity/nentityobjectserver.h"
#include "nscene/ncscenelodclass.h"
#include "nscene/ncscenebatch.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nshapenode.h"
#include "nscene/nstaticbatchnode.h"
#include "nmaterial/nmaterialnode.h"
#include "nmaterial/nmaterialserver.h"
#include "nspatial/ncspatialbatch.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/nloaderserver.h"

#include "ndebug/nceditorclass.h"
#include "ndebug/nceditor.h"

int nEntityBatchBuilder::uniqueBatchId = 0;

//------------------------------------------------------------------------------
/**
*/
nEntityBatchBuilder::nEntityBatchBuilder() :
    shapesByMaterial(16, 16),
    buildSceneResource(false),
    testSubentities(true),
    minTestDistance(100.0f)
{
    this->geometryNodeClass = nKernelServer::ks->FindClass("ngeometrynode");
    this->shapeNodeClass = nKernelServer::ks->FindClass("nshapenode");
    this->impostorNodeClass = nKernelServer::ks->FindClass("nimpostornode");
    this->materialNodeClass = nKernelServer::ks->FindClass("nmaterialnode");
}

//------------------------------------------------------------------------------
/**
*/
nEntityBatchBuilder::~nEntityBatchBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nEntityBatchBuilder::Clear()
{
    this->entities.Reset();
    this->refBatchEntity.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
void
nEntityBatchBuilder::AddEntity(nEntityObject* entity, int level)
{
    if (!this->entities.Find(entity))
    {
        this->entities.Append(EntityEntry(entity, level));
    }
}

//------------------------------------------------------------------------------
/**
    If there is no current batch, create one with all entities added.
    If there is one, first load subentities, then add all new ones.
    Do not batch a single entities, remove the batch if it is made empty.
*/
bool
nEntityBatchBuilder::Build()
{
    bool rebuildBatch = false;
    ncSpatialBatch* spatialBatch = 0;
    if (this->refBatchEntity.isvalid())
    {
        //allow an empty array of entities to rebuild the scene resource
        spatialBatch = this->refBatchEntity->GetComponentSafe<ncSpatialBatch>();
        spatialBatch->LoadSubentities();
        if (spatialBatch->GetSubentities().Empty() && this->entities.Empty())
        {
            return false;
        }
        rebuildBatch = true;
    }
    else
    {
        //check that there is more than 1 entity to batch
        if (this->entities.Size() < 2)
        {
            return false;
        }
    }

    //create a static batch entity: nestaticbatch
    if (!this->refBatchEntity.isvalid())
    {
        this->refBatchEntity = nEntityObjectServer::Instance()->NewEntityObject("nestaticbatch");
        n_assert(this->refBatchEntity.isvalid());
    }

    //add all source entities to the batch, this will add up all bboxes
    //lock the position and everything for the batch (do not allow transform updates)
    spatialBatch = this->refBatchEntity->GetComponentSafe<ncSpatialBatch>();
    spatialBatch->BeginBatch();
    for (int i = 0; i < this->entities.Size(); ++i)
    {
        //skip entities with no spatial component
        ncSpatial* spatialComp = this->entities[i].refEntity->GetComponent<ncSpatial>();
        if (spatialComp)
        {
            spatialBatch->AddSubentity(this->entities[i].refEntity.get());
        }
    }

    spatialBatch->EndBatch();
    spatialBatch->SetOriginalBBox(spatialBatch->GetBBox());

    spatialBatch->SetMinDistance(this->minTestDistance);
    spatialBatch->SetTestSubentities(this->testSubentities);

    spatialBatch->CalculateLightLinks();

    // build custom scene resource for the batch
    if (this->GetBuildSceneResource())
    {
        //1- statically collect shapes from all nodes, group by material
        for (int i = 0; i < this->entities.Size(); ++i)
        {
            ncEditorClass* editorClass = this->entities[i].refEntity->GetClassComponent<ncEditorClass>();
            if (editorClass && editorClass->IsSetClassKey("BatchLevel"))
            {
                int level = editorClass->GetClassKeyInt("BatchLevel");
                if (level != -1)
                {
                    this->CollectShapesFromEntity(this->entities[i].refEntity, level);
                }
            }
        }

        //2- if rebuilding a batch, collect shapes from entities already in the batch as well
        if (rebuildBatch)
        {
            const nArray<nEntityObject*>& subentities = spatialBatch->GetSubentities();
            for (int i = 0; i < subentities.Size(); ++i)
            { 
                nEntityObject* entity = subentities[i];
                ncEditorClass* editorClass = entity->GetClassComponent<ncEditorClass>();
                if (editorClass && editorClass->IsSetClassKey("BatchLevel"))
                {
                    int level = editorClass->GetClassKeyInt("BatchLevel");
                    if (level != -1)
                    {
                        this->CollectShapesFromEntity(entity, level);
                    }
                }
            }
        }

        //3- create a scene resource for the batch
        nSceneNode* batchSceneRoot = this->BatchShapesByMaterial();
        if (!batchSceneRoot)
        {
            //destroy the entity object if there is no resource to build
            nEntityObjectServer::Instance()->RemoveEntityObject(this->refBatchEntity.get());
            return false;
        }

        //4- replace existing resource with the one just created
        ncSceneBatch* sceneBatch = this->refBatchEntity->GetComponentSafe<ncSceneBatch>();
        if (sceneBatch->IsValid())
        {
            sceneBatch->Unload();
        }

        //the component gets its own ref:
        sceneBatch->SetBatchRoot(batchSceneRoot);
        //it will be loaded somewhere else by the batch entity:
        batchSceneRoot->Release();
        if (rebuildBatch)
        {
            //force reloading the scene component directly
            sceneBatch->Load();
        }
        else
        {
            //load the newly created entity
            nLoaderServer::Instance()->EntityNeedsLoading(this->refBatchEntity);
        }
    }

    //set batch as dirty
    nEntityObjectServer::Instance()->SetEntityObjectDirty(this->refBatchEntity.get(), true);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nEntityBatchBuilder::CollectShapesFromEntity(nEntityObject* entity, int level)
{
    // get the scene root to retrieve the information
    ncSceneLodClass* sceneLodComp = entity->GetClassComponent<ncSceneLodClass>();
    nSceneNode* sceneRoot(0);
    if (sceneLodComp && (level < sceneLodComp->GetNumLevels()))
    {
        sceneRoot = sceneLodComp->GetLevelRoot(level, 0); //FIXME get first root (second parameter added for inserts)
    }
    else
    {
        ncSceneClass* sceneComp = entity->GetClassComponent<ncSceneClass>();
        if (sceneComp)
        {
            sceneRoot = sceneComp->GetRootNode();
        }
    }
    // collect the geometry information from it
    if (sceneRoot)
    {
        this->CollectShapesByMaterial(entity, sceneRoot);
    }
}

//------------------------------------------------------------------------------
/**
    recursively traverse down the scene hierarchy and for every geometry node 
    with an assigned material:
    find the entry for the material, create if not found
    add the geometry node to the list of shapes for the material
*/
void
nEntityBatchBuilder::CollectShapesByMaterial(nEntityObject* entity, nSceneNode* sceneNode)
{
    if (sceneNode->IsA(this->geometryNodeClass))
    {
        nGeometryNode* geometry = static_cast<nGeometryNode*>(sceneNode);
        nSurfaceNode* material = static_cast<nGeometryNode*>(sceneNode)->GetSurfaceNode();
        MaterialEntry* materialEntry = this->shapesByMaterial.Find(material);
        if (materialEntry)
        {
            materialEntry->shapes.Append(GeometryEntry(entity, geometry));
        }
        else
        {
            MaterialEntry& newEntry = this->shapesByMaterial.PushBack(MaterialEntry(material));
            newEntry.shapes.Append(GeometryEntry(entity, geometry));
        }
    }

    // recursively do the same for children
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) sceneNode->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        this->CollectShapesByMaterial(entity, curChild);
    }
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode*
nEntityBatchBuilder::BuildBatchMaterialFrom(nSurfaceNode* surfaceNode)
{
    if (surfaceNode->IsA(materialNodeClass))
    {
        nMaterial* material = static_cast<nMaterialNode*>(surfaceNode)->GetMaterialObject();
        n_assert(material);
        if (!material->HasParam("deform", "instanced"))
        {
            return surfaceNode;
        }

        //if surfaceNode has an instanced material, get non-instanced version
        //TEMP- find non instanced version by name
        nString materialPath(surfaceNode->GetFullName());
        materialPath.StripExtension();//UGLY- treat the ".instanced" as an extension (!)
        //TODO- build and reuse a surface with a material without instancing
        nMaterialNode* materialNode = static_cast<nMaterialNode*>(nKernelServer::ks->Lookup(materialPath.Get()));
        n_assert(materialNode);
        return materialNode;
    }
    return surfaceNode;
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
nEntityBatchBuilder::BatchShapesByMaterial()
{
    nString batchScenePath;
    batchScenePath.Format("/tmp/entitybatchbuilder/batch%04u", uniqueBatchId++);
    nSceneNode* sceneNode = static_cast<nSceneNode*>(nKernelServer::ks->New("ntransformnode", batchScenePath.Get()));
    n_assert(sceneNode);

    //for every material: create a batch
    MaterialEntry* materialEntry = this->shapesByMaterial.Begin();
    while (materialEntry != this->shapesByMaterial.End())
    {
        //1- collect different types of geometry nodes
        nArray<int> staticShapeIndices;
        nArray<int> impostorIndices;
        int i;
        for (i = 0; i < materialEntry->shapes.Size(); ++i)
        {
            //switch (geometryNode->GetClass())
            GeometryEntry& geometryEntry = materialEntry->shapes.At(i);
            //case "nshapenode":
            if (geometryEntry.geometry->IsA(this->shapeNodeClass))
            {
                staticShapeIndices.Append(i);
            }
            //case "nimpostornode":
            else if (geometryEntry.geometry->IsA(this->impostorNodeClass))
            {
                impostorIndices.Append(i);
            }
        }

        //create an entry for each kind of geometry batched:
        //2- batch static shapes
        if (!staticShapeIndices.Empty())
        {
            //nGeometryNode* staticBatchNode = nKernelServer::New("nstaticbatchnode")
            nString batchStaticPath(batchScenePath);
            batchStaticPath.Append("/static.");
            nSurfaceNode* batchMaterial = this->BuildBatchMaterialFrom(materialEntry->material);
            batchStaticPath.Append(batchMaterial->GetName());
            nStaticBatchNode* staticBatchNode = static_cast<nStaticBatchNode*>(nKernelServer::ks->New("nstaticbatchnode", batchStaticPath.Get()));
            //set surface, removing the instancing parameter from the material
            staticBatchNode->SetSurface(batchMaterial->GetFullName().Get());
            //get meshes, add to list of static shapes, w/ world transform
            staticBatchNode->BeginMeshes(staticShapeIndices.Size());
            int i;
            for (i = 0; i < staticShapeIndices.Size(); ++i)
            {
                int index = staticShapeIndices.At(i);
                //set mesh at index
                nShapeNode* shapeNode = static_cast<nShapeNode*>(materialEntry->shapes.At(index).geometry);
                staticBatchNode->SetMeshAt(i, shapeNode->GetMesh());
                //set transform at index
                nEntityObject* entity = materialEntry->shapes.At(index).entity;
                ncTransform* transform = entity->GetComponentSafe<ncTransform>();
                staticBatchNode->SetPositionAt(i, transform->GetPosition());
                staticBatchNode->SetQuatAt(i, transform->GetQuat());
                staticBatchNode->SetScaleAt(i, transform->GetScale().x);
            }
            staticBatchNode->EndMeshes();
        }

        //3- batch impostors
        if (!impostorIndices.Empty())
        {
            nString batchImpostorPath(batchScenePath);
            batchImpostorPath.Append("/impostor.");
            batchImpostorPath.Append(materialEntry->material->GetName());
            nShapeNode* impostorBatchNode = static_cast<nShapeNode*>(nKernelServer::ks->New("nshapenode", batchImpostorPath.Get()));
            n_assert(impostorBatchNode);

            //for every node in the list
            //build a shapenode with all collected impostors, set created mesh
            nMeshBuilder builder;
            const vector2 spriteCorners[4] = { vector2(-1.0,-1.0),
                                               vector2(-1.0, 1.0),
                                               vector2(1.0,  1.0),
                                               vector2(1.0, -1.0) };

            int i;
            for (i = 0; i < impostorIndices.Size(); ++i)
            {
                int index = impostorIndices.At(i);
                //get transform at index
                nEntityObject* entity = materialEntry->shapes.At(index).entity;
                ncTransform* transform = entity->GetComponentSafe<ncTransform>();
                //get impostor at index
                nGeometryNode* impostorNode = static_cast<nGeometryNode*>(materialEntry->shapes.At(index).geometry);
                const bbox3& localBox = impostorNode->GetLocalBox();
                vector3 position = transform->GetTransform() * localBox.center();
                float scale = localBox.extents().y;//FIXME- valid only if impostor is higher than wider
                //build a static mesh to hold all impostors, save as level:batches/mesh####.nvx2
                nMeshBuilder::Vertex v0;
                v0.SetCoord(position);
                v0.SetUv(0, vector2(0.0f, 1.0f));
                v0.SetUv(1, spriteCorners[0] * scale);
                nMeshBuilder::Vertex v1;
                v1.SetCoord(position);
                v1.SetUv(0, vector2(0.0f, 0.0f));
                v1.SetUv(1, spriteCorners[1] * scale);
                nMeshBuilder::Vertex v2;
                v2.SetCoord(position);
                v2.SetUv(0, vector2(1.0f, 0.0f));
                v2.SetUv(1, spriteCorners[2] * scale);
                nMeshBuilder::Vertex v3;
                v3.SetCoord(position);
                v3.SetUv(0, vector2(1.0f, 1.0f));
                v3.SetUv(1, spriteCorners[3] * scale);
                //TODO- use this method to transform coord and normals when there are these
                //const matrix44& m44 = transform->GetTransform();
                //matrix33 m33(m44.x_component(), m44.y_component(), m44.z_component());
                //v0.Transform(m44, m33);
                /*nMeshBuilder::Triangle& t0 = */builder.AddTriangle(v0, v1, v2);
                /*nMeshBuilder::Triangle& t1 = */builder.AddTriangle(v0, v2, v3);
            }
            //save mesh to file
            //TODO- instead of saving immediately, enqueue for saving with the batch entity
            nEntityObjectId id(this->refBatchEntity->GetId());
            nString path;
            //save one mesh for every different material in the impostor batch
            path.Format("level:batches/%08X/impostor.%s.%08X.n3d2", id & nEntityObjectServer::IDHIGHMASK, 
                        materialEntry->material->GetName(), id);
            nKernelServer::ks->GetFileServer()->MakePath(path.ExtractDirName().Get());
            builder.SaveN3d2(nKernelServer::ks->GetFileServer(), path.Get());
            //builder.SaveNvx2(nKernelServer::ks->GetFileServer(), "level:batches/mesh####.nvx2");
            impostorBatchNode->SetFloat(nShaderState::Scale, 1.0f);
            impostorBatchNode->SetMesh(path.Get());
            impostorBatchNode->SetSurface(materialEntry->material->GetFullName().Get());
        }
        ++materialEntry;
    }

    return sceneNode;
}
