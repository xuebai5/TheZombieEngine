#ifndef N_OBJECT_H
#define N_OBJECT_H
//------------------------------------------------------------------------------
/**
    @class nObject
    @ingroup Kernel

    Provides:
    - reference counting and tracking (through nReferenced)
    - Nebula RTTI, a class is identified by a string name
    
    Rules for subclasses:
    - only the default constructor is allowed
    - never use new/delete (or variants like n_new/n_delete) with nObject 
      objects
    - use nKernelServer::New() to create an object and
      the object's Release() method to destroy it

    See also @ref N2ScriptInterface_nobject

    (c) 2004 Vadim Macagon
    Refactored out of nRoot.
*/

#include "kernel/ntypes.h" 
#include "kernel/nclass.h"
#include "kernel/ncmd.h"
#include "kernel/nmutex.h"
#include "kernel/nreferenced.h"
#include "kernel/nref.h"
#include "signals/nsignalemitter.h"
#include "signals/nsignalnative.h"
#ifdef __ZOMBIE_EXPORTER__
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nCmd;
class nKernelServer;
class nPersistServer;
class nFile;
//------------------------------------------------------------------------------
class nObject : public nReferenced, public nSignalEmitter
{
public:

    enum InitInstanceMsg
    {
        // do not call InitInstance
        NoInit = 0,
        // called when created with New, no data load
        NewInstance,
        // called after loading all data for new instance
        LoadedInstance,
        // reload all data (already created)
        ReloadedInstance,
        // called after loading all cloned data for cloned instance
        ClonedInstance
    };

    /// constructor (DONT CALL DIRECTLY, USE nKernelServer::New() INSTEAD)
    nObject();

    /// INTERNAL, do not call directly (create object)
    virtual nObject * BeginNewObject(const char * objClass, const char * objName);
    /// INTERNAL, do not call directly (end object)
    virtual void EndObject();
    /// INTERNAL, do not call directly (user init instance code)
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// release object
    virtual bool Release(void);
    
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// get instance size
    virtual int GetInstanceSize() const;
        
    /// save object under different name
    virtual bool SaveAs(const char* name);
    /// save object under different name
    virtual bool SaveFile(nFile* file);
    /// create new object as clone of this object
    virtual nObject *Clone(const char *unused = 0, bool initInstance = true);
    /// save object state for later restoring on an already created object
    virtual bool SaveStateAs(const char * filename);
    /// load object state replacing the current state
    virtual bool LoadState(const char * filename);
    /// copy state from another compatible object
    virtual bool CopyStateFrom(nObject *);

    /// lock the object's main mutex
    void LockMutex();
    /// unlock the object's main mutex
    void UnlockMutex();
    
    /// get pointer to my class object
    nClass *GetClass() const;
    /// return true if part of class hierarchy
    bool IsA(const nClass *) const;
    /// return true if part of class herarchy
    bool IsA(const char *) const;
    /// return true instance of class
    bool IsInstanceOf(const nClass *) const; 

    /// invoke nCmd on object
    bool Dispatch(nCmd *);
    /// get cmd proto list from object
    void GetCmdProtos(nHashList *);
    /// call a scripting function from a variable list of arguments
    void Call( const char * functionname, int numoutargs, nArg * outargs, ... );
    /// call a scripting function from a variable list of arguments
    void CallArgs( const char * functionname, int numoutargs, nArg * outargs, va_list args );
    /// call a scripting function from an array of nArg arguments
    void CallArgs( const char * functionname, int numoutargs, nArg * outargs, int numargs, nArg * args );

    /// set a dependency with a nRoot object saved in a persistent file
    void SetDependency(const nString & , const nString & , const nString & );
    /// set a dependency with a nObject object saved in a persistent file
    void SetDependencyObject(const nString & , const nString & , const nString & );

    /// Look if the object has a specific command
    bool HasCommand(const char *);
    
    /// pointer to kernel server
    static nKernelServer* kernelServer;

    #ifdef __ZOMBIE_EXPORTER__
    /// Set comment for this Object
    void SetComment(const nString& comment);
    /// Set comment for this Object
    void AppendComment(const nString& comment);
    /// Set comment for this Object
    void __cdecl AppendCommentFormat(const char* fmtString, ...) __attribute__((format(printf,2,3)));
    /// Get Comment for this Object
    const nString& GetComment() const;
    #endif
    #ifndef NGAME
    /// Make the object not displayable in editor
    void HideInEditor();
    /// Make the object displayable in editor
    void UnHideInEditor();
    /// Check if the object must be displayed in editor
    bool IsHideInEditor();
    /// return if nobject is dirty
    bool IsObjectDirty() const;
    /// set if nobject is dirty
    void SetObjectDirty(bool dirty = true);
    /// @name Signals interface
    //@{
    /// Signal if object is modified
    NSIGNAL_DECLARE('COBM', void, ObjectModified, 1, (nObject*), 0, () );    
    /// Signal call the fist time object is modified
    NSIGNAL_DECLARE('COBD', void, ObjectDirty, 1, (nObject*), 0, () );
    /// Python server not allow parameters in functions, so this signal is added temporally
    NSIGNAL_DECLARE('FOBC', void, ObjectChanges, 0, (), 0, () );
    //@}
    /// SetDirty object
    void TriggerObjectDirty(nObject* emitter);
    #endif

protected:
    friend class nClass;
        
    /// destructor (DONT CALL DIRECTLY, USE Release() INSTEAD)
    virtual ~nObject();
    /// set pointer to my class object
    void SetClass(nClass *);

    nClass* instanceClass;
    nMutex mutex;
    #ifndef NGAME
    /// True if the object must be displayed in editor tools
    bool hidden;
    /// True if the object is dirty
    bool objectDirty;
    #endif
    #ifdef __ZOMBIE_EXPORTER__
    nString comment;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
inline
void
nObject::HideInEditor( void )
{
    this->hidden = true;
}
#endif
//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
inline
void
nObject::UnHideInEditor( void )
{
    this->hidden = false;
}
#endif
//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
inline
bool
nObject::IsHideInEditor( void )
{
    return this->hidden;
}
#endif
//------------------------------------------------------------------------------
/**
*/
inline
void 
nObject::SetClass(nClass* cl)
{
    if (this->instanceClass)
    {
        this->instanceClass->numInstances--;
    }
    this->instanceClass = cl;
    if (cl)
    {
        cl->numInstances++;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nClass*
nObject::GetClass() const
{
    return this->instanceClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nObject::IsA(const nClass *cl) const
{
    nClass *actClass = this->instanceClass;
    do 
    {
        if (actClass == cl) 
        {
            return true;
        }
    } while ( 0 != (actClass = actClass->GetSuperClass()) );
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nObject::IsInstanceOf(const nClass *cl) const
{
    return (cl == this->instanceClass);
}

//------------------------------------------------------------------------------
#endif // N_OBJECT_H
