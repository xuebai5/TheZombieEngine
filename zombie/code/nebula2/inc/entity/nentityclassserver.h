#ifndef N_ENTITYCLASSSERVER_H
#define N_ENTITYCLASSSERVER_H
//------------------------------------------------------------------------------
/**
    @class nEntityClassServer
    @ingroup NebulaEntitySystem
    @brief Entity common services
    @author Mateu Batle

    The entity class server has the responsability of loading & saving entity 
    classes

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "util/nmaptable.h"
#include "util/nstring.h"
#include "util/nsortedarray.h"
#include "entity/nentityobject.h"
#include "entity/nentityclassinfo.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nEntityClass;
class nObjectArray;

//------------------------------------------------------------------------------
class nEntityClassServer : public nRoot 
{
public:
    //------------------------------------------------------------------------------
    /**
        neccesary struct to sort key strings
    */
    static int StringSorter(nString* const* elm0, nString* const* elm1);
    typedef nArray< nSortedArray< nString*, StringSorter > > ArraySortedStrings;
    //------------------------------------------------------------------------------

    /// constructor
    nEntityClassServer(void);

    /// return singleton entity server
    static nEntityClassServer * Instance(void);

    /// set directory where to store class info
    void SetEntityClassDir(const char *);
    /// get directory where to store class info
    const char * GetEntityClassDir() const;
    /// Save entity classes (index and all dirty classes)
    bool SaveEntityClasses();
    /// Discard entity classes pending to be deleted
    bool DiscardDeletedClasses();
    /// INTERNAL FOR PERSISTENCE, add class 
    void AddClassToIndex(const char *);
    /// get entity class, loading it if not already loaded
    nEntityClass * GetEntityClass(const char *);
    /// return if entiyclass is in class index.
    bool ExistEntityClass(const char * className) const;
    /// remove class
    bool RemoveEntityClass(const char *);
    /// create new entity class
    nEntityClass * NewEntityClass(nEntityClass *, const char *);
    /// set entity class dirty
    void SetEntityClassDirty(nEntityClass *, bool);
    /// get entity class dirty
    bool GetEntityClassDirty(nEntityClass *);
    /// Change the version data
    void SetDirtyAllClasses ();
    /// new non-native plain nClass
    bool NewClass(const char *, const char *);
    /// remove a non-native plain nClass
    bool RemoveClass(const char *);
    /// check if class index has been modified
    bool GetDirty();

    /// get children subclass names
    void GetEntitySubClassNames(const char * name, nArray<nString> & subclasses) const;
    /// get loaded children subclasses
    void GetLoadedEntitySubClasses(nEntityClass * neclass, nArray<nEntityClass *> & subclasses) const;
    /// load all classes
    bool LoadAllEntityClasses();

    // check if class name is valid (same as valid class name but non-static)
    bool CheckClassName(const nString&);

    /// set the current class key
    void SetClassKey(int);

    /// set current class key to local HighId
    void SetLocalClassKey ();

    /// load all subclasses of a given class
    bool LoadEntitySubClasses(nEntityClass *);
    //bool LoadEntitySubClasses(nEntityClass *);

    /// check if is this string is a valid name
    static bool ValidClassName(const nString& name);

    /// save an entity class
   // bool SaveEntityClass(nEntityClass *) const;
    bool SaveEntityClass(nEntityClass *) const;

protected:

    //friend class nEntityObject;
    //friend class nObjectInstancer;
    friend class nEntityClass;
    //friend class nObject;
    //friend class nPersistServer;

#ifndef NGAME
    friend class ncEditorClass;
    friend class ncEditor;
    /// Set dirty class
    void TriggerClassDirty(nObject* emitter);
#endif//!NGAME

    /// destructor
    virtual ~nEntityClassServer(void);

    /// get entity class filename for a non-loaded class
    nString GetEntityClassFilename(const char * className) const;

    /// Save entity class index
    bool SaveEntityClassIndex(void);
    /// Load entity class index
    bool LoadEntityClassIndex(void);
    /// add class
    void AddClassToIndex(nEntityClass * neclass, bool dirty);
    // add native classes to the class index
    void AddNativeClasses(nEntityClass * cl);
    /// load an entity class
   // nEntityClass * LoadEntityClass(const char *);
    nEntityClass * LoadEntityClass(const char *);
    /// unload an entity class
   // bool UnloadEntityClass(nEntityClass *);
    bool UnloadEntityClass(nEntityClass *);
    /// find entity class from the classes loaded in memory
   // nEntityClass * FindEntityClass(const char *) const;
    nEntityClass * FindEntityClass(const char *) const; 
    /// internal function to create entity classes
    nEntityClass * CreateEntityClass(nEntityClass * neSuperClass, const char * name);

    /// unload all entity classes
   // bool UnloadAllEntityClasses();
    bool UnloadAllEntityClasses(void);
    /// unload all entity subclasses from a class
   // bool UnloadEntitySubClasses(nEntityClass *);
    bool UnloadEntitySubClasses(nEntityClass *);
    /// Create all non-native plain nClasses in class index
    void CreateNonNativePlainClasses();
    /// Create a subclass
    nClass * CreateSubClass(nClass *, const char *);

    /// remove entity class (effectively delete the file and entry)
    void RemoveEntityClassPost(const char * className);

    static nEntityClassServer * instance;

    /// directory where to persist entity classes
    nString entityClassDir;

    /// current key for loaded keys
    nEntityObjectId currentClassKey;

    // EntityClassInfo comparison function
    static int CmpEntityClassInfo( const nEntityClassInfo *, const nEntityClassInfo *);

    // index for entity classes
    nSortedArray<nEntityClassInfo, CmpEntityClassInfo> classIndex;

    // class index needs to be saved to disk
    bool dirty;

    static void __cdecl CallBackClassNotFound(const char* className);
};

//--------------------------------------------------------------------
#endif
