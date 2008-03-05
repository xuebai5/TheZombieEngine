#ifndef N_SAVEMANAGER_H
#define N_SAVEMANAGER_H
//-----------------------------------------------------------------------------
/**
    @class nSaveManager
    @ingroup Framework
    @brief The save manager allows to save the state of a group of objects
    to a file, and restore it back later.

    The save manager can be used for two different things:
    a) Save the state of the editor before entering game mode and restore back
    b) Save a game at a given point

    At the moment only the functionality to save the the editor state has been
    implemented, but save game functionality will be possible too in near future.

    The way to use this save manager is as follows:

    Step 1 - save the state

    1) BeginSession()

    2) AddObjectState(obj,groupName,objectName) for objects to remember its state.

        obj: pointer to any nObject based, state can only be saved for nObjects
        objectName: optional object name given and persisted in the file
        groupName: optional group name given and persisted in the file

        The groupName and objectName are only used for easier recovery later,
        they are not interpreted in any way by the save manager.
        If not needed, just specify null strings.
        The state is not persisted yet in this call, it will be done later.

    3) AddObjectAvailable(obj) for objects to remember its existence, so later
        the system can restore the removed objects, and delete newly created objects.
        The objects added through AddObjectState are already handled, do not call
        it for them. State is not saved for these objects.

    4) EndSession()

    5) SaveSessionCmds(filename, saveType, file)

        Saves all the state in the filename specified. If a nFile file is provided
        then it is used directly. This can be used to allow saving state in a memory 
        file (nMemFile).
        The state of the object is the one provided by the SaveCmds.
        SaveType is set in the persist server, some objects might want to check
        the savetype in order to check which type of save is executing 
        (all save cmds for persistence, reload save, save game)

    Step 2 - restore the state

    1) RestoreSession(filename, sessionFile)

        Restores a session saved before. It loads all the state previously
        save in a session.

    2) AddObjectAvailableOnRestore(obj)
        
        Call it for all objects that are in the system, the ones that would get added
        through AddObjectState in the case we wanted to create a session.

    3) CheckRestoredObjects()

        Checks objects deleted after the save session have been recreated.
        Deletes objects that have been created after the session.

    Note: all the objects saved must guarantee that the commands they persist can
    be reexecuted on the already created object more than once without any kind
    of accumulation. It should behave the same as if it was just created.
    After loading all the state in an object, the InitInstance(Reloaded/Loaded) 
    is issued again, so this must also behave correctly without accumulation. 
    - Reload is used when the object previously existed.
    - Load when the object existed.

    Extension ideas:

    - signals on save manager for object reloaded after reload state, 
    before reload state, before delete, so the controller of the
    save manager can handle additional processes on these events.

    (C) 2006 Conjurer Services, S.A.
*/
//-----------------------------------------------------------------------------
/**
    @scriptclass Save Manager

    @cppclass nSaveManager

    @superclass nObject

    @classinfo The save manager takes care of saving and restore object state.
*/    
//-----------------------------------------------------------------------------
#include "kernel/nref.h"
#include "kernel/ndynautoref.h"
#include "kernel/nobject.h"
#include "kernel/nroot.h"
#include "entity/nrefentityobject.h"

//------------------------------------------------------------------------------
class nSaveManager : public nObject
{
public:

    /// types of obejcts to save
    enum nRefType
    {
        InvalidType,
        nObjectType,
        nRootType,
        nEntityObjectType
    };

    /// reference to objects in memory
    struct nRefVariant
    {
        /// override -> operator
        nObject* operator->();
        /// dereference operator
        nObject& operator*();
        /// set target object
        void set(nObject *obj);
        /// get target object (safe)
        nObject* get();
        /// get target object (unsafe, may return 0)
        nObject* get_unsafe();
        /// returns true if the object is valid
        bool isvalid();

        nRefType refType;
        nDynAutoRef<nRoot> refnRoot;
        nRef<nObject> refnObject;
        nRefEntityObject refnEntityObject;
    };

    /// named object
    struct nNamedObject
    {
        nString objectName;
        nRefVariant object;
    };

    /// named group of objects
    struct nObjectGroup
    {
        nString groupName;
        nArray<nNamedObject> objects;
    };

    /// constructor
    nSaveManager();
    /// destructor
    virtual ~nSaveManager();

    /// begin session definition
    void BeginSession();
    /// add a nObject state to the session
    bool AddObjectState(nObject * obj, const char * objectName = 0, const char * groupName = 0);
    /// call to remember which objects where available (does not save state)
    bool AddObjectAvailable(nObject * obj);
    /// end session definition
    void EndSession();
    /// save a session to a nFile
    bool SaveSession(const char * filename, nPersistServer::nSaveType type = nPersistServer::SAVETYPE_PERSIST);
    /// save a session to a nFile
    bool SaveSession(nFile * sessionFile, nPersistServer::nSaveType type = nPersistServer::SAVETYPE_PERSIST);
    /// reset the session cmd
    void ResetSession();

    /// restore a session
    bool RestoreSession(const char * filename);
    /// restore a session
    bool RestoreSession(nFile * sessionFile);

    /// check if an object was available or not
    bool AddObjectAvailableBeforeRestore(nObject * obj);
    /// add an object after the restore process
    bool AddObjectAvailableAfterRestore(nObject * obj);
    /// do a check of available objects, deletes new created objects
    void CheckRestoredObjects();

    /// find an object
    bool IsObjectAdded(nObject * obj, nString & groupName, nString & objectName);
    /// find an object group
    nObjectGroup * FindObjectGroup(const char * groupName) const;
    /// find an object by name of group and name of object
    nObject * FindObject(const char * groupName, const char * objectName) const;
    /// find the object with path NOH
    nObject * FindObjectByPathNOH(const char * pathNOH);
    /// find the object by entity object id
    nObject * FindObjectByEntityObjectId(nEntityObjectId oid);

    /// internal, used for persistence of nEntityObjects
    nObject * BeginStatenEntityObject(const char *, const char *, const char *, nEntityObjectId);
    /// internal, used for persistence of nRoot
    nObject * BeginStatenRoot(const char *, const char *, const char *, const char *);
    /// internal, used for persistence of plain nObjects
    nObject * BeginStatenObject(const char *, const char *, const char *);

private:

    /// check if an object is available or not
    bool IsObjectAvailable(nObject * obj);
    /// check if an object is available before restore
    bool IsObjectAvailableBeforeRestore(nObject * obj);
    /// check if an object is available after restore
    bool IsObjectAvailableAfterRestore(nObject * obj);
    /// Get object string identifier
    nString GetObjectStrId(nObject * obj);

    /// groups of objects whose state must be saved
    nArray<nObjectGroup> groups;
    /// to remember objects available at some point
    nArray<nRefVariant> availableObjects;
    /// objects available before restoring
    nArray<nRefVariant> availableObjectsBeforeRestore;
    /// objects available after restoring
    nArray<nRefVariant> availableObjectsAfterRestore;

    /// current object used when restoring session
    nObject * currentObject;

};

//------------------------------------------------------------------------------
#endif // N_SAVEMANAGER_H
