#ifndef N_ENTITYBATCHBUILDER_H
#define N_ENTITYBATCHBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nEntityBatchBuilder
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>
    @brief Batches a set of selected entities into a single batch entity.
    It creates the scene resource for it from the resources of these entities.

    (C) 2005 Conjurer Services, S.A.
*/

class nEntityObject;
class nSceneNode;
class nGeometryNode;
class nSurfaceNode;
//------------------------------------------------------------------------------
class nEntityBatchBuilder
{
public:
    /// constructor
    nEntityBatchBuilder();
    /// destructor
    ~nEntityBatchBuilder();

    /// clear all lists and structures
    void Clear();
    /// add an entity to the list of them to batch
    void AddEntity(nEntityObject*, int level);
    /// build the batch entity and scene resource
    bool Build();
    /// get number of entities added
    const int GetNumEntities();

    /// set a batch entity (re-build)
    void SetBatchEntity(nEntityObject*);
    /// get the batch entity 
    nEntityObject* GetBatchEntity();

    /// set if must rebuild the scene resource
    void SetBuildSceneResource(bool value);
    /// get if must rebuild the scene resource
    bool GetBuildSceneResource() const;
    /// set if the batch must test subentities
    void SetBatchTestSubentities(bool value);
    /// get if the batch must test subentities
    bool GetBatchTestSubentities() const;
    /// set if the batch must test subentities
    void SetDefaultMinDistance(float value);
    /// get if the batch must test subentities
    float GetDefaultMinDistance() const;

private:
    /// set current entity being batched
    void SetCurrentEntity(nEntityObject*, int level);
    /// collect shapes from an entity
    void CollectShapesFromEntity(nEntityObject*, int level);
    /// traverse down a scene hierarchy, group geometry nodes by material
    void CollectShapesByMaterial(nEntityObject*, nSceneNode* sceneRoot);
    /// build static batches for all collected materials
    nSceneNode* BatchShapesByMaterial();
    /// check the material and fix a new one if it is instanced
    nSurfaceNode* BuildBatchMaterialFrom(nSurfaceNode* surfaceNode);

    /// entities with levels
    struct EntityEntry
    {
        EntityEntry();
        EntityEntry(nEntityObject*);
        EntityEntry(nEntityObject*, int level);
        //FIXME this doesn't work with the nArray<>::Find() method:
        //bool operator==(const nEntityObject*);
        bool operator==(const EntityEntry&);

        nRef<nEntityObject> refEntity;
        int level;
    };

    nArray<EntityEntry> entities;
    nRef<nEntityObject> refBatchEntity;

    bool buildSceneResource;
    bool testSubentities;
    float minTestDistance;

    //materials (nodes)
    //shapesByMaterial (nodes)
    struct GeometryEntry
    {
        GeometryEntry();
        GeometryEntry(nEntityObject*, nGeometryNode*);
        nGeometryNode* geometry;
        nEntityObject* entity;
    };

    struct MaterialEntry
    {
        MaterialEntry();
        MaterialEntry(nSurfaceNode*);
        ~MaterialEntry();
        nSurfaceNode* material;
        nArray<GeometryEntry> shapes;//batched geometries
        //FIXME same problem here:
        //bool operator==(const nSurfaceNode*);
        bool operator==(const MaterialEntry&);
    };

    nArray<MaterialEntry> shapesByMaterial;
    nClass* geometryNodeClass;
    nClass* shapeNodeClass;
    nClass* impostorNodeClass;
    nClass* materialNodeClass;

    static int uniqueBatchId;
};

//------------------------------------------------------------------------------
/**
*/
inline
const int
nEntityBatchBuilder::GetNumEntities()
{
    return this->entities.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEntityBatchBuilder::SetBatchEntity(nEntityObject* entity)
{
    if (entity)
    {
        this->refBatchEntity = entity;
    }
    else
    {
        this->refBatchEntity.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nEntityBatchBuilder::GetBatchEntity()
{
    return this->refBatchEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEntityBatchBuilder::SetBuildSceneResource(bool value)
{
    this->buildSceneResource = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEntityBatchBuilder::GetBuildSceneResource() const 
{
    return this->buildSceneResource;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEntityBatchBuilder::SetBatchTestSubentities(bool value)
{
    this->testSubentities = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEntityBatchBuilder::GetBatchTestSubentities() const 
{
    return this->testSubentities;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEntityBatchBuilder::SetDefaultMinDistance(float value)
{
    this->minTestDistance = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEntityBatchBuilder::GetDefaultMinDistance() const 
{
    return this->minTestDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::EntityEntry::EntityEntry() :
    level(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::EntityEntry::EntityEntry(nEntityObject* entity) :
    refEntity(entity),
    level(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::EntityEntry::EntityEntry(nEntityObject* entity, int l) :
    refEntity(entity),
    level(l)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEntityBatchBuilder::EntityEntry::operator==(const nEntityBatchBuilder::EntityEntry& entry)
{
    return this->refEntity.isvalid() && (this->refEntity.get() == entry.refEntity.get());
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::GeometryEntry::GeometryEntry() :
    entity(0),
    geometry(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::GeometryEntry::GeometryEntry(nEntityObject* e, nGeometryNode* g) :
    entity(e),
    geometry(g)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::MaterialEntry::MaterialEntry() :
    material(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::MaterialEntry::MaterialEntry(nSurfaceNode* surface) :
    material(surface)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityBatchBuilder::MaterialEntry::~MaterialEntry()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEntityBatchBuilder::MaterialEntry::operator==(const nEntityBatchBuilder::MaterialEntry& entry)
{
    return this->material == entry.material;
}

//------------------------------------------------------------------------------
#endif /*N_ENTITYBATCHBUILDER_H*/
