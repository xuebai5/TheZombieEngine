#ifndef N_LEVEL_H
#define N_LEVEL_H
//------------------------------------------------------------------------------
/**
    @class nLevel
    @ingroup NebulaLevelSystem

    @brief An abstract wrapper for loading a set of related assets and 
    game objects. Subclasses are supposed to implement logic for specific
    level types (single player, cooperative, etc.) and different types of
    resources and behaviors.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentityobjectserver.h"
#include "util/nstring.h"

class nEntityObject;

#ifndef NGAME
class nLayerManager;
class nLayer;
#endif

//------------------------------------------------------------------------------
class nLevel : public nRoot
{
public:
    /// constructor
    nLevel();
    /// destructor
    virtual ~nLevel();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);

    /// load resources for all registered entities
    void LoadEntityResources();
    /// load entities contained in registered space
    void LoadSpaceEntity(nEntityObject* entity);
    /// unload all entities
    void UnloadEntityResources();
    /// load resources for all registered entities
    void SaveEntityResources();
    /// load entity layers
    void LoadEntityLayers();
    #ifndef NGAME
    /// set level manager for entities
    void SetEntityLayerManager(nLayerManager *layerManager);
    #endif //NGAME

    // Script commands

    /// add space entity by id
    void SetSpaceEntity(nEntityObjectId);
    /// add global entity by id
    void SetGlobalEntity(nEntityObjectId);
    /// find whether an entity is global
    bool IsGlobalEntity(nEntityObjectId);
    /// set a name label for an entity
    void SetEntityName(nEntityObjectId, const char *);
    /// get entity name by id
    const char * GetEntityName(nEntityObjectId);
    /// get entity by name
    nEntityObjectId FindEntity(const char *);
    /// remove entity by name
    void RemoveEntityName(const char *);

    /// add an entity object
    void AddEntity(nObject*);
    /// set current layer key (persistence only)
    void SetWizardKey(int);
    /// add an entity layer
    void AddEntityLayer(int, const char *, bool, const char *);
    #ifndef NGAME
    /// create a new entity layer by name
    nRoot* GetEntityLayerManager();
    #endif //NGAME

private:
    /// construct for the list of entity names
    class EntityNameNode : public nStrNode
    {
    public:
        EntityNameNode() : entityId(0) { }
        nEntityObjectId entityId;
    };

    /// construct for the list of entity layers
    class EntityLayerNode : public nStrNode
    {
    public:
        EntityLayerNode() : layerId(0), locked(false), password(0), layerKey(nEntityObjectServer::IDINVALID) {}
        int layerId;
        bool locked;
        nString password;
        nEntityObjectId layerKey;
    };

    /// construct to group entities for persistence
    struct KeyGroup
    {
        nArray<nEntityObjectId> keyGlobals;     ///< global entities
        nArray<nEntityObjectId> keySpaces;      ///< space entities
        nArray<EntityNameNode*> keyNames;       ///< entity names
        #ifndef NGAME
        nArray<nLayer*> keyLayers;              ///< entity layers
        #endif
    };

    /// sort arrays in a given keygroup (for persistence)
    void SortKeyGroup(KeyGroup& keyGroup);
    /// a qsort() hook to sort by entity id
    static int __cdecl EntityIdSorter(const void* elm0, const void* elm1);
    /// a qsort() hook to sort by entity name node
    static int __cdecl EntityNameSorter(const void* elm0, const void* elm1);

#ifndef  NGAME
    /// a qsort() hook to sort by entity layer
    static int __cdecl EntityLayerSorter(const void* elm0, const void* elm1);
#endif NGAME

    nArray<nEntityObjectId> spaceObjects;       ///< space ids (for persisted entities)
    nArray<nEntityObjectId> globalObjects;      ///< global ids (for persisted entities)
    nStrList entityNameList;                    ///< entity names (for script access)

#ifndef NGAME
    nRef<nLayerManager> refEntityLayerManager;  ///< layer manager for level entities
#endif
    nStrList entityLayerList;                   ///< layer names and ids (persistence!)
    nEntityObjectId currentWizardKey;           ///< current key for loaded keys (persistence!)
};

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
void
nLevel::SetEntityLayerManager(nLayerManager *layerManager)
{
    this->refEntityLayerManager = layerManager;
}
#endif

//------------------------------------------------------------------------------
/**
*/
inline
void
nLevel::SetWizardKey(int wizardKey)
{
    this->currentWizardKey = wizardKey;
}

//------------------------------------------------------------------------------
#endif
