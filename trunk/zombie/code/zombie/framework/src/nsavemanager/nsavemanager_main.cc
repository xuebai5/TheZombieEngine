//-----------------------------------------------------------------------------
//  nsavemanager_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchframework.h"
#include "nsavemanager/nsavemanager.h"
#include "kernel/npersistserver.h"
#include "kernel/nlogclass.h"
#include "kernel/nlogclass.h"

//-----------------------------------------------------------------------------
NCREATELOGLEVEL_REL(savemanager, "Save Manager", false, 0);

//-----------------------------------------------------------------------------
nNebulaScriptClass(nSaveManager, "nobject");

//------------------------------------------------------------------------------
nSaveManager::nSaveManager() :
    currentObject(0)
{
    /// empty
}

//------------------------------------------------------------------------------
nSaveManager::~nSaveManager()
{
    /// empty
}

//------------------------------------------------------------------------------
void 
nSaveManager::BeginSession()
{
    this->ResetSession();
}

//------------------------------------------------------------------------------
bool
nSaveManager::AddObjectState(nObject * obj, const char * objectName, const char * groupName)
{
    n_assert(obj);

    // check the object is not already added (if it is return error)
    nString strGroupName;
    nString strObjectName;
    if (this->IsObjectAdded(obj, strGroupName, strObjectName))
    {
        return false;
    }

    if (groupName && objectName && this->FindObject(groupName, objectName))
    {
        return false;
    }

    if (!groupName)
    {
        groupName = "";
    }

    if (!objectName)
    {
        objectName = "";
    }

    // find the group where to add the object
    nObjectGroup * group = this->FindObjectGroup(groupName);
    if (!group)
    {
        group = this->groups.Reserve(1);
        group->groupName = groupName;
    }

    // append the object
    nNamedObject namedObject;
    namedObject.object.set(obj);
    namedObject.objectName = objectName;
    group->objects.Append(namedObject);

    // add object as available
    this->AddObjectAvailable(obj);

    NLOG(savemanager, (2, "AddObjectState: %s", this->GetObjectStrId(obj)));

    return true;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::AddObjectAvailable(nObject * obj)
{
    n_assert(obj);

    // check it has not been added before
    if (this->IsObjectAvailable(obj))
    {
        return false;
    }

    // add the object
    nRefVariant * refObj = this->availableObjects.Reserve(1);
    refObj->set(obj);

    NLOG(savemanager, (2, "AddObjectAvailable: %s", this->GetObjectStrId(obj)));

    return true;
}

//------------------------------------------------------------------------------
void 
nSaveManager::EndSession()
{
    /// empty
}

//------------------------------------------------------------------------------
bool 
nSaveManager::SaveSession(nFile * sessionFile, nPersistServer::nSaveType saveType)
{
    //n_assert(filename);

    bool ok = true;

    nPersistServer * ps = nKernelServer::Instance()->GetPersistServer();
    nPersistServer::nSaveType oldSaveType = ps->GetSaveType();
    ps->SetSaveType(saveType);

    nCmd * cmd = 0;
    cmd = ps->GetCmd(this, 'THIS');
    if (!ps->BeginObjectWithCmd(this, cmd, sessionFile)) 
    {
        ok = false;
    }

    // iterate all objects and save their state
    for(int i = 0;ok && i < this->groups.Size();i++)
    {
        for(int j = 0;ok && j < this->groups[i].objects.Size();j++)
        {
            if (this->groups[i].objects[j].object->IsA("nentityobject"))
            {
                nEntityObject * entityObject = static_cast<nEntityObject *> (this->groups[i].objects[j].object.get());
                cmd = ps->GetCmd(this, 'BBSE');
                cmd->In()->SetS(this->groups[i].objects[j].object->GetClass()->GetName());
                cmd->In()->SetS(this->groups[i].groupName.Get());
                cmd->In()->SetS(this->groups[i].objects[j].objectName.Get());
                cmd->In()->SetI(entityObject->GetId());
            }
            else if (this->groups[i].objects[j].object->IsA("nroot"))
            {
                nRoot * root = static_cast<nRoot *> (this->groups[i].objects[j].object.get());
                cmd = ps->GetCmd(this, 'BBSR');
                cmd->In()->SetS(this->groups[i].objects[j].object->GetClass()->GetName());
                cmd->In()->SetS(this->groups[i].groupName.Get());
                cmd->In()->SetS(this->groups[i].objects[j].objectName.Get());
                nString fullName = root->GetFullName();
                cmd->In()->SetS(fullName.Get());
            }
            else
            {
                cmd = ps->GetCmd(this, 'BBSO');
                cmd->In()->SetS(this->groups[i].objects[j].object->GetClass()->GetName());
                cmd->In()->SetS(this->groups[i].groupName.Get());
                cmd->In()->SetS(this->groups[i].objects[j].objectName.Get());
            }

            if (ps->BeginObjectWithCmd(this, cmd))
            {
                /// save all objects to disk
                this->groups[i].objects[j].object->SaveCmds(ps);
                ps->EndObject(true);
            }
            else
            {
                ok = false;
            }
        }
    }

    if (ok)
    {
        ps->EndObject(true);
    }

    ps->SetSaveType( oldSaveType );

    NLOG(savemanager, (2, "SaveSession"));

    return ok;
}

//------------------------------------------------------------------------------
bool
nSaveManager::SaveSession(const char * filename, nPersistServer::nSaveType saveType)
{
    n_assert(filename);
    bool retval = false;

    nFile* file = kernelServer->GetFileServer()->NewFileObject();
    if (file->Open(filename, "w+"))
    {
        retval = this->SaveSession(file , saveType);
        file->Close();
    }
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
void 
nSaveManager::ResetSession()
{
    NLOG(savemanager, (2, "ResetSession"));

    this->groups.Clear();
    this->availableObjects.Clear();
    this->availableObjectsBeforeRestore.Clear();
    this->availableObjectsAfterRestore.Clear();
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::BeginStatenEntityObject(const char * className, const char * groupName, const char * objectName, nEntityObjectId id)
{
    n_assert(className);
    n_assert(groupName);
    n_assert(objectName);
    n_assert(id != nEntityObjectServer::IDINVALID);

    bool ok = true;
    nObject::InitInstanceMsg initType = nObject::NoInit;

    // get class
    nClass * cl = nKernelServer::Instance()->FindClass(className);
    if (!cl)
    {
        NLOG(savemanager, (NLOGUSER | 0, "Save Manager: class not found %s", className));
        ok = false;
    }
    n_assert(cl->IsA("nentityobject"));

    // check if object is already created
    nObject * obj = 0;
    if (ok && !obj)
    {
        obj = this->FindObjectByEntityObjectId(id);
        initType = nObject::ReloadedInstance;
    }

    if (ok && !obj && groupName && objectName)
    {
        obj = this->FindObject(groupName, objectName);
        initType = nObject::ReloadedInstance;
    }

    if (ok && !obj && nEntityObjectServer::Instance()->CanBeUnremoved(id))
    {
        // it is possible that unremove does not work because it was not persisted before
        // in this case, the entity will be created afterwards with the info from the save maanger
        nEntityObjectServer::Instance()->UnremoveEntityObjectById(id);
    }

    if (ok && !obj)
    {
        obj = nEntityObjectServer::Instance()->GetEntityObject(id);
        initType = nObject::ReloadedInstance;
    }

    // check the proper class
    if (ok && obj && !obj->IsInstanceOf(cl))
    {
        NLOG(savemanager, (NLOGUSER | 0, "Save Manager: Object found with class %s not maching class %s", obj->GetClass()->GetName(), className));
        obj = 0;
    }
    
#if 0
    // if refcount is 1 -> delete it & recreate it (gives less problems)
    if (ok && obj && obj->GetRefCount() == 1)
    {
        nEntityObjectServer::Instance()->RemoveEntityObject(static_cast<nEntityObject *>(obj));
        obj = 0;
    }
#endif

    // create it if not
    NLOGCOND(savemanager, !ok, (2, "BeginStatenEntityObject: error %s", this->GetObjectStrId(obj)));
    NLOGCOND(savemanager, obj, (2, "BeginStatenEntityObject: reused %s", this->GetObjectStrId(obj)));
    if (ok && !obj)
    {
        obj = nEntityObjectServer::Instance()->NewEntityObjectFromType(
            className, 
            nEntityObjectServer::Instance()->GetEntityObjectType(id), 
            id, 
            false);
        initType = nObject::LoadedInstance;
        n_assert(obj);

        NLOG(savemanager, (2, "BeginStatenEntityObject: created %s", this->GetObjectStrId(obj)));
    }

    // report this to persist server
    if (ok && obj)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(obj, initType);
    }

    this->currentObject = obj;

    return obj;
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::BeginStatenRoot(const char * className, const char * groupName, const char * objectName, const char * pathNOH)
{
    n_assert(className);
    n_assert(groupName);
    n_assert(objectName);
    n_assert(pathNOH);

    bool ok = true;
    nObject::InitInstanceMsg initType = nObject::NoInit;

    // get class
    nClass * cl = nKernelServer::Instance()->FindClass(className);
    if (!cl)
    {
        NLOG(savemanager, (NLOGUSER | 0, "Save Manager: class not found %s", className));
        ok = false;
    }

    // check if object is already created
    nObject * obj = 0;
    if (ok && !obj)
    {
        obj = this->FindObjectByPathNOH(pathNOH);
        initType = nObject::ReloadedInstance;
    }

    if (ok && !obj && groupName && objectName)
    {
        obj = this->FindObject(groupName, objectName);
        initType = nObject::ReloadedInstance;
    }

    // check the proper class
    if (ok && obj && !obj->IsInstanceOf(cl))
    {
        NLOG(savemanager, (NLOGUSER | 0, "Save Manager: Object found with class %s not maching class %s", obj->GetClass()->GetName(), className));
        obj = 0;
    }

#if 0
    // if refcount is 1 -> delete it & recreate it (gives less problems)
    if (ok && obj && obj->GetRefCount() == 1)
    {
        obj->Release();
        obj = 0;
    }
#endif

    NLOGCOND(savemanager, !ok, (2, "BeginStatenRoot: error %s", this->GetObjectStrId(obj)));
    NLOGCOND(savemanager, obj, (2, "BeginStatenRoot: reused %s", this->GetObjectStrId(obj)));
    if (ok && !obj)
    {
        obj = nKernelServer::Instance()->New(className, pathNOH, false);
        initType = nObject::LoadedInstance;
        NLOG(savemanager, (2, "BeginStatenRoot: created %s", this->GetObjectStrId(obj)));
    }

    n_assert(obj);

    // report this to persist server
    if (obj)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(obj, initType);
    }

    this->currentObject = obj;

    return obj;
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::BeginStatenObject(const char * className, const char * groupName, const char * objectName)
{
    n_assert(className);
    n_assert(groupName);
    n_assert(objectName);

    bool ok = true;
    nObject::InitInstanceMsg initType = nObject::NoInit;

    // get class
    nClass * cl = nKernelServer::Instance()->FindClass(className);
    if (!cl)
    {
        NLOG(savemanager, (NLOGUSER | 0, "Save Manager: class not found %s", className));
        ok = false;
    }

    // check if object is already created
    nObject * obj = 0;
    if (!obj && groupName && objectName)
    {
        obj = this->FindObject(groupName, objectName);
        initType = nObject::ReloadedInstance;
    }

    // check the proper class
    if (ok && obj && !obj->IsInstanceOf(cl))
    {
        NLOG(savemanager, (0, "Save Manager: Object found with class %s not maching class %s", obj->GetClass()->GetName(), className));
        obj = 0;
    }

#if 0
    // if refcount is 1 -> delete it & recreate it (gives less problems)
    if (ok && obj && obj->GetRefCount() == 1)
    {
        obj->Release();
        obj = 0;
    }
#endif

    if (!obj)
    {
        obj = nKernelServer::Instance()->New(className, false);
        initType = nObject::LoadedInstance;
    }
    
    n_assert(obj);

    // report this to persist server
    NLOGCOND(savemanager, !ok, (2, "BeginStatenObject: error %s", this->GetObjectStrId(obj)));
    NLOGCOND(savemanager, obj, (2, "BeginStatenObject: reused %s", this->GetObjectStrId(obj)));
    if (obj)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(obj, initType);
        NLOG(savemanager, (2, "BeginStatenObject: created %s", this->GetObjectStrId(obj)));
    }

    this->currentObject = obj;

    return obj;
}

//------------------------------------------------------------------------------
bool
nSaveManager::RestoreSession(nFile * sessionFile)
{
    n_assert(sessionFile);

    NLOG(savemanager, (2, "RestoreSession"));

    kernelServer->PushCwd(this);
    nObject * obj = nKernelServer::Instance()->Load(sessionFile, false);
    kernelServer->PopCwd();

    return (obj != 0);
}

//------------------------------------------------------------------------------
bool
nSaveManager::RestoreSession(const char * filename)
{
    n_assert(filename);
    bool retval = false;

    nFile* file = kernelServer->GetFileServer()->NewFileObject();
    if (file->Open(filename, "r"))
    {
        retval = this->RestoreSession( file );
        file->Close();
    }
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
nSaveManager::nObjectGroup * 
nSaveManager::FindObjectGroup(const char * groupName) const
{
    if (!groupName)
    {
        groupName = "";
    }

    for(int i = 0;i < this->groups.Size();i++)
    {
        if (this->groups[i].groupName == groupName)
        {
            return &this->groups[i];
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::FindObject(const char * groupName, const char * objectName) const
{
    if ((!groupName || !groupName[0]) && (!objectName || !objectName[0]))
    {
        return 0;
    }

    if (!groupName)
    {
        groupName = "";
    }

    if (!objectName)
    {
        objectName = "";
    }

    for(int i = 0;i < this->groups.Size();i++)
    {
        if (this->groups[i].groupName == groupName)
        {
            for(int j = 0;j < this->groups[i].objects.Size();j++)
            {
                if (this->groups[i].objects[j].objectName == objectName)
                {
                    if (this->groups[i].objects[j].object.isvalid())
                    {
                        nObject * obj = this->groups[i].objects[j].object.get();
                        if (obj)
                        {
                            return obj;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::FindObjectByPathNOH(const char * pathNOH)
{
    n_assert(pathNOH);

    for(int i = 0;i < this->groups.Size();i++)
    {
        for(int j = 0;j < this->groups[i].objects.Size();j++)
        {
            if (this->groups[i].objects[j].object->IsA("nroot"))
            {
                if (this->groups[i].objects[j].object.isvalid())
                {
                    nRoot * root = static_cast<nRoot *> (this->groups[i].objects[j].object.get());
                    if (root && root->GetFullName() == pathNOH)
                    {
                        return root;
                    }
                }
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
nObject * 
nSaveManager::FindObjectByEntityObjectId(nEntityObjectId oid)
{
    for(int i = 0;i < this->groups.Size();i++)
    {
        for(int j = 0;j < this->groups[i].objects.Size();j++)
        {
            if (this->groups[i].objects[j].object.isvalid())
            {
                nObject * obj = this->groups[i].objects[j].object.get();
                if (obj && obj->IsA("nentityobject"))
                {
                    nEntityObject * obj = static_cast<nEntityObject *> (this->groups[i].objects[j].object.get());
                    if (obj && obj->GetId() == oid)
                    {
                        return obj;
                    }
                }
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::IsObjectAdded(nObject * obj, nString & groupName, nString & objectName)
{
    n_assert(obj);

    for(int i = 0;i < this->groups.Size();i++)
    {
        for(int j = 0;j < this->groups[i].objects.Size();j++)
        {
            if (this->groups[i].objects[j].object.isvalid())
            {
                if (this->groups[i].objects[j].object.get() == obj)
                {
                    groupName = this->groups[i].groupName;
                    objectName = this->groups[i].objects[j].objectName;
                    return true;
                }
            }
        }
    }

    return false;    
}

//------------------------------------------------------------------------------
bool 
nSaveManager::IsObjectAvailable(nObject * obj)
{
    n_assert(obj);

    for(int i = 0;i < this->availableObjects.Size();i++)
    {
        if (this->availableObjects[i].isvalid())
        {
            if (this->availableObjects[i].get() == obj)
            {
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::AddObjectAvailableBeforeRestore(nObject * obj)
{
    n_assert(obj);

    // check it has not been added before
    if (this->IsObjectAvailableBeforeRestore(obj))
    {
        return false;
    }

    // add the object
    nRefVariant * refObj = this->availableObjectsBeforeRestore.Reserve(1);
    refObj->set(obj);

    NLOG(savemanager, (2, "AddObjectAvailableBeforeRestore: %s", this->GetObjectStrId(obj)));

    return true;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::AddObjectAvailableAfterRestore(nObject * obj)
{
    n_assert(obj);

    // check it has not been added before
    if (this->IsObjectAvailableBeforeRestore(obj) || this->IsObjectAvailableAfterRestore(obj))
    {
        return false;
    }

    // add the object
    nRefVariant * refObj = this->availableObjectsAfterRestore.Reserve(1);
    refObj->set(obj);

    NLOG(savemanager, (2, "AddObjectAvailableAfterRestore: %s", this->GetObjectStrId(obj)));

    return true;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::IsObjectAvailableBeforeRestore(nObject * obj)
{
    n_assert(obj);

    for(int i = 0;i < this->availableObjectsBeforeRestore.Size();i++)
    {
        if (this->availableObjectsBeforeRestore[i].isvalid())
        {
            if (this->availableObjectsBeforeRestore[i].get() == obj)
            {
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool 
nSaveManager::IsObjectAvailableAfterRestore(nObject * obj)
{
    n_assert(obj);

    for(int i = 0;i < this->availableObjectsAfterRestore.Size();i++)
    {
        if (this->availableObjectsAfterRestore[i].isvalid())
        {
            if (this->availableObjectsAfterRestore[i].get() == obj)
            {
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
void 
nSaveManager::CheckRestoredObjects()
{
    // check objects available before not available now
    // in theory this is not needed, since they will be recreated
    // anyway, the check is done to ensure it works ok
    for(int i = 0;i < this->availableObjects.Size();i++)
    {
        nObject * obj = 0;
        if ( this->availableObjects[i].isvalid())
        {
            obj = this->availableObjects[i].get();
        }
        if (!obj || !(this->IsObjectAvailableBeforeRestore(obj) || this->IsObjectAvailableAfterRestore(obj)))
        {
            NLOG(savemanager, (2, "CheckRestoredObjects: object lost on restore %s", this->GetObjectStrId(obj)));
            n_message("warning: object lost on restore obj=0x%x", obj);
        }
    }

    // check objects available now not available before
    // they must be deleted
    for(int i = 0;i < this->availableObjectsBeforeRestore.Size();i++)
    {
        if ( this->availableObjectsBeforeRestore[i].isvalid() )
        {
            nObject * obj = this->availableObjectsBeforeRestore[i].get();
            if (! this->IsObjectAvailable(obj))
            {
                // object needs to be deleted
                if (obj->IsA("nentityobject"))
                {
                    nEntityObject * eo = static_cast<nEntityObject *> (obj);
                    NLOG(savemanager, (NLOGUSER | 1, "CheckRestoredObjects:: SaveManager deleting entity %s", this->GetObjectStrId(obj)));
                    nEntityObjectServer::Instance()->RemoveEntityObject(eo);
                }
                else
                {
                    NLOG(savemanager, (NLOGUSER | 1, "CheckRestoredObjects:: SaveManager deleting object %s", this->GetObjectStrId(obj)));
                    obj->Release();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
nObject* 
nSaveManager::nRefVariant::operator->()
{
    return this->get();
}

//-----------------------------------------------------------------------------
nObject& 
nSaveManager::nRefVariant::operator*()
{
    return *this->get();
}

//-----------------------------------------------------------------------------
void 
nSaveManager::nRefVariant::set(nObject *obj)
{
    if(obj->IsA("nentityobject"))
    {
        this->refType = nSaveManager::nEntityObjectType;
        this->refnEntityObject.set((static_cast<nEntityObject*> (obj)) ->GetId());
        this->refnEntityObject.get();
    }
    else if(obj->IsA("nroot"))
    {
        this->refType = nSaveManager::nRootType;
        this->refnRoot.set((static_cast<nRoot*> (obj))->GetFullName().Get());
        this->refnRoot.get();
    }
    else
    {
        this->refType = nSaveManager::nObjectType;
        this->refnObject.set(obj);
    }
}

//-----------------------------------------------------------------------------
nObject* 
nSaveManager::nRefVariant::get()
{
    switch(this->refType)
    {
    case InvalidType:
        n_assert_always();
        break;
    case nObjectType:
        return this->refnObject.get();
        break;
    case nRootType:
        return this->refnRoot.get();
        break;
    case nEntityObjectType:
        return this->refnEntityObject.get();
        break;
    default:
        n_assert_always();
        break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
nObject* 
nSaveManager::nRefVariant::get_unsafe()
{
    switch(this->refType)
    {
    case InvalidType:
        return 0;
        break;
    case nObjectType:
        return this->refnObject.get_unsafe();
        break;
    case nRootType:
        return this->refnRoot.get_unsafe();
        break;
    case nEntityObjectType:
        return this->refnEntityObject.get_unsafe();
        break;
    default:
        n_assert_always();
        break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
bool 
nSaveManager::nRefVariant::isvalid()
{
    switch(this->refType)
    {
    case InvalidType:
        return false;
        break;
    case nObjectType:
        return this->refnObject.isvalid();
        break;
    case nRootType:
        return this->refnRoot.isvalid();
        break;
    case nEntityObjectType:
        return this->refnEntityObject.isvalid();
        break;
    default:
        n_assert_always();
        break;
    }

    return false;
}

//-----------------------------------------------------------------------------
nString 
nSaveManager::GetObjectStrId(nObject * obj)
{
    nString id;
    if (obj)
    {
        if (obj->IsA("nentityobject"))
        {
            nEntityObject * eo = static_cast<nEntityObject *> (obj);
            id.Format("0x%x id=0x%x class=%s", eo, eo->GetId(), eo->GetClass()->GetName());
        }
        else 
        {
            id.Format("0x%x class=%s", obj, obj->GetClass()->GetName());
        }
    }
    else
    {
        id.Set("null");
    }

    return id;
}
