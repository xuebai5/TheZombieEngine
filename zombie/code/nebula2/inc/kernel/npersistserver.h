#ifndef N_PERSISTSERVER_H
#define N_PERSISTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nPersistServer
    @ingroup Kernel

    Interface to the file system for serializing objects.

    See also @ref N2ScriptInterface_npersistserver

    <h2>Nebula Persistent Object File Format</h2>

    The actual format of serialized objects is not specified.
    Instead script server objects are used to translate nCmd objects
    emitted by objects into actual script statements that will
    be saved to files. However, since the file server needs to
    know which script server to use for reading serialized object
    files, there has to be some meta information in the file
    itself which names the class of the script server which 
    understands it.

    Within the first 128 Bytes of a serialized object file the 
    following string must be embedded:

    @verbatim
    $parser:classname$
    @endverbatim

    Where 'classname' is the script server's class which has
    to be used to read that file. 

    <h2>Folded and unfolded objects</h2>

    @note
    UNFOLDED OBJECTS ARE NO LONGER SUPPORTED FOR
    WRITING, AND READING IS ONLY SUPPORTED FOR "FLAT" FILE
    HIERARCHIES (i.e. a "nKernelServer::Load("bla.n") is allowed
    if bla.n is a directory, and contains a _main file).

    Serialized objects can be saved in 2 ways, folded and unfolded.
    Folded means, the object itself and all of it's children will
    be saved into a single file. Unfolded means that for each
    object, a directory will be create in the host filesystem,
    so that a saved object hierarchy will result in a
    corresponding directory hierarchy.

    <h2>Code</h2>

    The following code demonstrates how to specify <tt>nluaserver</tt> for
    persist server:
    @code
    nPersistServer* persisitServer;

    persisitServer = nKernelServer::Instance()->GetPersistServer();
    persisitServer->SetSaverClass("nluaserver");
    @endcode

    (C) 2002 A.Weissflog
*/
#include "kernel/ntypes.h"
#include "kernel/nroot.h"
#include "util/nstack.h"
#include "kernel/nautoref.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nDependencyServer;
class nFileServer2;
class nFile;
class nPersistServer : public nRoot 
{
public:
    /// Savemodes
    enum nSaveMode 
    {
        SAVEMODE_FOLD,  /// default mode
        SAVEMODE_CLONE,  
    };

    /// type of information to save 
    enum nSaveType
    {
        /// normal SaveCmds behavior
        SAVETYPE_PERSIST = 0,
        /// save only cmds to reload
        SAVETYPE_RELOAD,
        /// save state of the game
        SAVETYPE_GAMESAVE
    };

    /// constructor
    nPersistServer();
    /// destructor
    virtual ~nPersistServer();

    /// set the save mode
    void SetSaveMode(nSaveMode);
    /// get the save mode
    nSaveMode GetSaveMode(void);
    /// set the save level
    void SetSaveLevel(int);
    /// get the save level
    int GetSaveLevel(void);
    /// set the save type
    void SetSaveType(nSaveType);
    /// get the save type
    nSaveType GetSaveType(void);
    /// set the script server class which should be used for saving
    bool SetSaverClass(const nString& saverClass);
    /// get the script server class which is currently used for saving
    nString GetSaverClass();
    /// return the current script server used by the persist server
    nScriptServer * GetScriptServer();
    /// begin a persistent object<
    bool BeginObject(nObject *, const char *, bool);
    /// begin a persistent object<
    bool BeginObject(nObject *, nFile*, bool);
    /// begin a persistent object with constructor command
    bool BeginObjectWithCmd(nObject *, nCmd *, const char *);
    /// begin a persistent object with constructor command
    bool BeginObjectWithCmd(nObject *, nCmd *, nFile* file=0);
    /// create a nCmd object
    nCmd *GetCmd(nObject *, nFourCC id);
    /// put a cmd into persistency stream
    bool PutCmd(nCmd *);
    /// put a cmd into persistency stream if savelevel permits
    bool PutCmd(int, nCmd *);
    /// easier put into persistency stream
    bool Put( nObject *o, nFourCC fourcc, ... );
    /// easier put into persistency stream if savelevel permits
    bool PutLevel( nObject *o, int, nFourCC fourcc, ... );
    /// finish a persistent object
    bool EndObject(bool);
    /// return cloned object
    nObject *GetClone(void);
    /// set cloned object
    void SetClone(nObject * obj);
    /// load a object from persistency stream
    nObject *LoadObject(nFile *file, const char* objName);
    /// say if nPersistSever is saving an object now
    bool IsSaving()const;
    /// set dependency server class for a class and command
    void SetDependencyClass(nString objectClass, nString command, nString depClass);
    /// get persistency server class for a class and command
    nString GetDependencyClass(nString objectClass, nString command);
    /// get dependency server for a class command
    nDependencyServer *GetDependencyServer(nString objectClass, nString command);

    /// called by object loading on begin object
    void BeginObjectLoad(nObject * obj, nObject::InitInstanceMsg initType);
    /// called by object loading on end object
    void EndObjectLoad(nObject * obj);

    /// Check if class has info about cmd for new operation
    bool IsNewCmd(const char * objClass) const;
    /// Parse the new cmd and return tokens for cmd name and arguments
    bool ParseNewCmd(const char * objClass, nArray<nString> & newCmdTokens) const;
    /// Returns a string with the encoded cmd
    nString MakeNewCmdString(nCmd * cmd) const;
#ifdef __ZOMBIE_EXPORTER__
    /// Write comment
    bool PutComment(const char* comment);
#endif

private:   
    /// internal
    bool BeginFoldedObject(nObject *obj, nCmd *, nFile* file, bool sel_only, 
                           bool isObjNamed);
    /// internal
    bool BeginCloneObject(nObject *obj, nCmd * cmd, const char *name, bool isObjNamed);
    /// internal
    bool EndFoldedObject(bool);
    /// internal
    bool EndCloneObject(bool);
    /// internal
    nObject* LoadFoldedObject(nFile * file, const char *objName,
                              const char *parser, const char *objClass);
    /// internal
    char *ReadEmbeddedString(nFile * file, const char *keyword, char *buf, int buf_size);
    /// internal
    bool PutFoldedObjectHeader(nScriptServer *saver, nFile *file, nObject* obj, nCmd * cmd);
    /// get a valid loader script server from class name
    nScriptServer* GetLoader(const char* loaderClass);

    struct nDepServerEntry
    {
        nString objClass;
        nString command;
        nRef<nDependencyServer> refDepServer;
    };
    nArray<nDepServerEntry> cmdDepServers;
    /// get dependency server for a command
    nDepServerEntry *FindDepServerEntry(nString objClass, nString command);
    /// get entry for a command
    nDepServerEntry *GetDepServerEntry(nString objClass, nString command);

    nDynAutoRef<nScriptServer> refSaver;
    nFile * file;
    nStack<nObject*> objectStack;
    nStack<nObject::InitInstanceMsg> initInstanceMsgStack;
    nSaveMode saveMode;     
    
    nObject *cloneTarget;
    // store original current working directory on clone operation
    nObject *origCwd;

    int saveLevel;
    nSaveType saveType;
    /// release file when endObject
    bool releaseFile;
};

//--------------------------------------------------------------------
/**
    @retval true if nPersistServer is still saving an object
*/
inline
bool
nPersistServer::IsSaving()const
{
    return ( ! this->objectStack.IsEmpty() );
}

//--------------------------------------------------------------------
#endif
