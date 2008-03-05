#ifndef N_ENTITYOBJECTSERVER_H
#define N_ENTITYOBJECTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nEntityObjectServer
    @ingroup NebulaEntitySystem
    @brief Server managing entity objects
    @author Mateu Batle

    The entity object server has several functions:
    1) Generate identifiers for entity objects
    2) Persistence of entity objects

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "entity/nentityobjectinfo.h"
#include "util/nmaptable.h"
#include "util/nstring.h"
#include "util/nsortedarray.h"
#include "entity/nentityobject.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"
#include "util/nsortedarray.h"
#include "entity/nfileindexgroup.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nEntityClass;
class nObjectArray;
class nFileIndex;

//------------------------------------------------------------------------------
class nEntityObjectServer : public nRoot 
{
public:

    //------------------------------------------------------------------------------

    // Number of bits used for inter-machine ID (high part)
    static const int IDHIGHBITS = 8;
    // Number of bits used for intra-machine ID (low part)
    static const int IDLOWBITS = (32 - IDHIGHBITS);
    // Number of bits used for subentities (low part)
    static const int IDSUBENTITYBITS = IDLOWBITS - IDHIGHBITS;
    // Mask for low part
    static const nEntityObjectId IDLOWMASK  = ((1 << IDLOWBITS) - 1);
    // Mask for high part
    static const nEntityObjectId IDHIGHMASK = (~IDLOWMASK);
    // Mask for subentity objects
    static const nEntityObjectId IDSUBENTITYMASK = ( ( 1 << IDSUBENTITYBITS ) - 1 );
    // High ID for local objects
    static const nEntityObjectId IDHIGHLOCAL    = 0x00000000;
    // High ID for server objects
    static const nEntityObjectId IDHIGHSERVER   = 0xff000000;
    // Invalid entity object identifier
    static const nEntityObjectId IDINVALID      = 0;

    //------------------------------------------------------------------------------
    enum nEntityObjectType {
        /// Normal objects are persisted and available in all network machines with same ID
        Normal,
        /// Local objects are not persisted, private to each machine
        Local,
        /// Server objects are not persisted, created by the server and common for all clients
        Server
    };

    /// constructor
    nEntityObjectServer(void);

    /// return singleton entity object server
    static nEntityObjectServer * Instance(void);

    /// set config dir
    void SetConfigDir(const char *);
    /// get config dir
    const char * GetConfigDir() const;
    /// Save configuration
    bool SaveConfig();
    /// INTERNAL FOR PERSISTENCE, Set base object identifier
    void SetBaseEntityObjectId(nEntityObjectId);
    /// INTERNAL FOR PERSISTENCE, Set base object identifier
    void SetBaseEntityObjectBunchId(int);

    /// check if entity object server has something modified
   // bool GetDirty();

    /// set directory used to store class info - unload objects first
    void SetEntityObjectDir(const char *);
    /// get directory where to store class info
    const char * GetEntityObjectDir() const;
    /// save all entity objects dirty
    void SaveEntityObjects();
    /// Discard changes
    void DiscardDeletedObjects();
    /// delete entity object by object identifier
    bool RemoveEntityObjectById(nEntityObjectId);
    /// delete entity object 
    bool RemoveEntityObject(nEntityObject *);
    /// undelete entity object by object identifier
    bool UnremoveEntityObjectById(nEntityObjectId);
    /// check if an entity object is removed or not by object identifier
    bool CanBeUnremoved(nEntityObjectId);
    /// get entity object from the identifier, loads it if not loaded
    nEntityObject * GetEntityObject(nEntityObjectId);
    /// create entity object
    nEntityObject * NewEntityObject(const char *);
    /// create local entity object 
    nEntityObject * NewLocalEntityObject(const char *);
    /// create local entity object 
    nEntityObject * NewServerEntityObject(const char *);
    /// create local entity object 
    nEntityObject * NewServerEntityObjectWithId(const char *, nEntityObjectId);
    /// set entity object dirty
    void SetEntityObjectDirty(nEntityObject *, bool);
    /// unload entity object 
    void UnloadEntityObject(nEntityObject *);
    /// swap the identifier of the two entity objects provided
    void SwapEntityObjectIds(nEntityObject *, nEntityObject *);

    /// Unload all entity objects from a given class
    void UnloadEntityObjectsFromClass(nEntityClass *);
    /// Unload all entity objects
    void UnloadAllEntityObjects();
    /// Unload normal entity objects
    void UnloadNormalEntityObjects();

    /// Load configuration
    bool LoadConfig();

    /// Get the HighId (local Id)
    nEntityObjectId GetHighId ();
    /// 
    bool IsValidEntityObjectId(nEntityObjectId);

    /// Get the fisrt entity object
    nEntityObject*  GetFirstEntityObject();
    /// Get next entity object
    nEntityObject*  GetNextEntityObject();

    /// return true if entity object is dirty
    bool GetEntityObjectDirty(nEntityObjectId);

    /// set the version data
    void SetDataVersion(int);
    /// get the version data
    int GetDataVersion();
    /// Change the version data
    bool ChangeDataVersion(int);
    /// Change the version data
    void SetDirtyAllObjects ();
    /// Get the entity type (normal, local or server)
    nEntityObjectType GetEntityObjectType(nEntityObjectId) const;

    /// set entity object id
    void SetEntityId(nEntityObject *entity, nEntityObjectId id);

    /// change entity object class
    bool ChangeEntityClass(nEntityObject*, const char *);

    /// @name Signals interface
    //@{
    NSIGNAL_DECLARE( 'LEND', void, EntityDeleted, 1, (int), 0, () );
    //@}

#ifndef NGAME
    /// Create new nFile a return the chunk of object
    nFile* GetChunkFile(nEntityObjectId id);
#endif

protected:

    friend class nEntityObject;
    friend class nObjectInstancer;
    //friend class nEntityClass;
    friend class nObject;
    friend class nPersistServer;
    friend class nSaveManager;

#ifndef NGAME
    friend class ncEditorClass;
    friend class ncEditor;
    void TriggerObjectDirty(nObject* emitter);
#endif//!NGAME

    /// destructor
    virtual ~nEntityObjectServer(void);

    /// unload all entity classes
    /// New entity object with info about type and identifier
    nEntityObject * NewEntityObjectFromType(const char * className, nEntityObjectType otype, nEntityObjectId id = 0, bool initInstance = true);
    /// load entity object
   // nEntityObject * LoadEntityObject(nEntityObjectId);
    nEntityObject * LoadEntityObject(nEntityObjectId);
    /// save entity object 
   // bool SaveEntityObject(nEntityObject *) const;
    bool SaveEntityObjectV0(nEntityObject *) const;
    /// save all entity objects dirty with version 0
    void SaveEntityObjectsV0();
    /// save all entity objects dirty with version 1
    void SaveEntityObjectsV1();

    /// find entity object in memory
    nEntityObject * FindEntityObject(nEntityObjectId) const;
    /// remove entity object postprocessing (file, ...)
    bool RemoveEntityObjectPost(nEntityObjectId id);

    /// get configuration filename 
    nString GetConfigFilename() const;
    /// get configuration filename, common all users
    nString GetCommonConfigFilename() const;

    /// get entity object filename 
    nString GetEntityObjectFilename(nEntityObjectId) const;

    /// Get block of object identifiers
    nEntityObjectId GetEntityObjectId(nEntityObjectType otype = Normal, int count = 1);

    static nEntityObjectServer * instance;

    /// configuration directory
    nString configDir;
    /// directory where to persist entity objects
    nString entityObjectDir;

    /// base object id for new entity objects
    nEntityObjectId newEntityObjectId;
    nEntityObjectId newLocalEntityObjectId;
    nEntityObjectId newServerEntityObjectId;

    /// high part of every identifier generated by this server
    nEntityObjectId highId;

    // hash with entity objects loaded in memory
    typedef nMapTable<nEntityObjectInfo, nEntityObjectId> EntityObjectTable;
    EntityObjectTable * entityObjects;
    // class index needs to be saved to disk
    bool dirty;
    // The mode files is saved
    int dataVersion;

    struct EntryIdAndInfo
    {
        nEntityObjectId id;
        nEntityObjectInfo info;
        nFileIndex* fileIndex;
        ///destructor
        ~EntryIdAndInfo();
        /// Compare function
        static int Cmp( const void *, const void *);
    };
    /// File index of entitiess
    nFileIndexGroup  FileIndexGroupEntity;
#ifndef NGAME
    /// File index of chunk
    nFileIndexGroup  FileIndexGroupChunk;
#endif
};

//--------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(nEntityObjectServer::nEntityObjectType, "i", (value = (nEntityObjectServer::nEntityObjectType) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );

//--------------------------------------------------------------------
#endif
