//------------------------------------------------------------------------------
/**
    @file nentityserver_object.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
//------------------------------------------------------------------------------
#include "entity/nentityobjectserver.h"
#include "entity/nentityobject.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndebug.h"
#include "misc/nprefserver.h"
#include "kernel/nsocketdefs.h"
#include "kernel/nlogclass.h"
#include "entity/nfileindex.h"
#include "file/nmemfile.h"
#include "kernel/nscriptserver.h"
#include "kernel/nlogclass.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaScriptClass(nEntityObjectServer, "nroot");

//------------------------------------------------------------------------------
nEntityObjectServer * nEntityObjectServer::instance = 0;

//------------------------------------------------------------------------------
static const char * RegistryKeyHighID = "HighID";

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nEntityObjectServer, EntityDeleted );

//------------------------------------------------------------------------------
NCREATELOGLEVEL ( entityObjectServer , "Entity Object Server " , true, 3 )

//------------------------------------------------------------------------------
nEntityObjectServer::nEntityObjectServer(void) :
    newEntityObjectId(0),
    newLocalEntityObjectId(IDHIGHLOCAL | 1),
    newServerEntityObjectId(IDHIGHSERVER | 1),
    highId(0),
    entityObjects(0),
    dirty(false),
    dataVersion(1)
{
    // initialize instance pointer
    if (!nEntityObjectServer::instance)
    {
        nEntityObjectServer::instance = this;
    }

    // create the map table for entity objects
    const int size = 24571;
    this->entityObjects = n_new(EntityObjectTable)(size);
    n_assert(this->entityObjects);
    #ifndef NDEBUG
    this->entityObjects->CalcDistribution(); // Trick for link this method
    #endif

    // check if configuration file available to get highid
    this->highId = nPrefServer::Instance()->ReadInt(RegistryKeyHighID);

    // check if the high id obtained is valid as high id for normal objects
    if (GetEntityObjectType(this->highId) == Normal)
    {
        this->newEntityObjectId = this->highId | 1;
        return;
    }

    // if highid not available then autodetect
#if defined(WIN32)
    struct WSAData wsaData;
    WSAStartup(0x101, &wsaData);
#endif
    /// get highId
    char hostname[128];
    int err = gethostname(hostname, sizeof(hostname));
    if (err)
    {
        return;
    }
    // get IP address
    struct hostent * hostinfo = gethostbyname(hostname);
    if (hostinfo &&
        hostinfo->h_addr_list[0] &&
        AF_INET == hostinfo->h_addrtype &&
        4 == hostinfo->h_length)
    {
        // solution for Zombie (GUID cheap & dirty)
        // use the lower part of the IP 192.168.0.XXX as highId
        this->highId = (hostinfo->h_addr_list[0][3] << 24);
    }
    else
    {
        this->highId = 0;
    }
#if defined(WIN32)
    WSACleanup();
#endif

    if (this->GetEntityObjectType(this->highId) != Normal)
    {
        this->highId = (1 << IDLOWBITS);
    }

    // write high id to registry
    nPrefServer::Instance()->WriteInt(RegistryKeyHighID, this->highId);

    // initialize ids
    this->newEntityObjectId = this->highId | 1;
}

//------------------------------------------------------------------------------
nEntityObjectServer::~nEntityObjectServer(void)
{
    // unload all objects
    if (this->entityObjects)
    {
        this->UnloadAllEntityObjects();
        n_delete(this->entityObjects);
    }

    this->FileIndexGroupEntity.Clear();

#ifndef NGAME
    this->FileIndexGroupChunk.Clear();
#endif NGAME

    if (nEntityObjectServer::instance == this)
    {
        nEntityObjectServer::instance = 0;
    }
}

//------------------------------------------------------------------------------
nEntityObjectServer * 
nEntityObjectServer::Instance(void)
{
    n_assert( nEntityObjectServer::instance );
    return nEntityObjectServer::instance;
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetConfigDir(const char * dir)
{
    n_assert(dir);
    this->configDir = dir;
    this->configDir.StripTrailingSlash();
    this->configDir += "/";
}

//------------------------------------------------------------------------------
const char * 
nEntityObjectServer::GetConfigDir() const
{
    return this->configDir.Get();
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::SaveConfig(void)
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // construct filename of the config file
    nString filename = this->GetConfigFilename();
    // create all directories in the path
    nFileServer2::Instance()->MakePath(filename.ExtractDirName().Get());

    // save configuration with cmd BeginNewObjectConfig cmd
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, filename.Get())) 
    {
        ps->Put(this, 'SOID', this->newEntityObjectId);
        ps->EndObject(true);
        retval = true;
    }


    // construct filename of the common config file
    filename = this->GetCommonConfigFilename();
    // create all directories in the path
    nFileServer2::Instance()->MakePath(filename.ExtractDirName().Get());

    cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, filename.Get())) 
    {
        ps->Put(this, 'CSDV', this->GetDataVersion() );

        ps->EndObject(true);
        retval = retval && true;
    }
    return retval;
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::LoadConfig(void)
{
    // by default use data version 1
    this->SetDataVersion(1);

    kernelServer->PushCwd(this);
    nKernelServer::Instance()->Load(this->GetCommonConfigFilename().Get(), false);
    nObject * obj = nKernelServer::Instance()->Load(this->GetConfigFilename().Get(), false);
    if (!obj)
    {
        this->newEntityObjectId = this->highId | 1;
    }

    kernelServer->PopCwd();

    return (obj != 0);
}

//------------------------------------------------------------------------------
nString 
nEntityObjectServer::GetConfigFilename() const
{
    nString filename(this->configDir);
    char buffer[16];
    sprintf(buffer, "%08X", this->highId);
    filename.Append(buffer);
    filename.Append(".n2");
    return filename;
}

//------------------------------------------------------------------------------
nString 
nEntityObjectServer::GetCommonConfigFilename() const
{
    return this->configDir + "commonconfig.n2";
}


//------------------------------------------------------------------------------
nEntityObjectId
nEntityObjectServer::GetHighId()
{
    return this->highId;
}

//------------------------------------------------------------------------------
nEntityObjectId 
nEntityObjectServer::GetEntityObjectId(nEntityObjectType otype, int count)
{
    n_assert(count > 0);
    nEntityObjectId retId = 0;

    switch(otype)
    {
    case Normal:
        // check id does not wrap around
        if ( ((this->newEntityObjectId + count) & IDHIGHMASK) != this->highId)
        {
            return 0;
        }

        retId = this->newEntityObjectId;
        this->newEntityObjectId += count;
        break;
    case Server:
        // check id does not wrap around
        if ( ((this->newServerEntityObjectId + count) & IDHIGHMASK) != IDHIGHSERVER)
        {
            return 0;
        }

        retId = this->newServerEntityObjectId;
        this->newServerEntityObjectId += count;
        break;
    case Local:
        // check id does not wrap around
        if ( ((this->newLocalEntityObjectId + count) & IDHIGHMASK) != IDHIGHLOCAL)
        {
            return 0;
        }

        retId = this->newLocalEntityObjectId;
        this->newLocalEntityObjectId += count;
        break;
    default:
        n_assert2_always("Unknown entity object type");
    }

    return retId;
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetBaseEntityObjectId(nEntityObjectId id)
{
    n_assert(id > 0);
    n_assert((id & IDHIGHMASK) == this->highId);

    this->newEntityObjectId = id;
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetEntityObjectDir(const char * dir)
{
    n_assert(dir);
    n_assert(this->entityObjects);

    this->entityObjectDir = dir;
    this->entityObjectDir.StripTrailingSlash();
    this->entityObjectDir += "/";

    // Set new path and release file index of previous configdir
    nString fileIndexPath( this->entityObjectDir + "objects/");
    if (!nFileServer2::Instance()->MakePath(fileIndexPath))
    {
        NLOG(persistence, (0, "Could not create path %s", fileIndexPath.Get()));
    }
    this->FileIndexGroupEntity.SetPath( fileIndexPath.Get() );

#ifndef NGAME
    fileIndexPath.Append("ncEditor_");
    this->FileIndexGroupChunk.SetPath( fileIndexPath.Get() );
#endif
}

//------------------------------------------------------------------------------
const char * 
nEntityObjectServer::GetEntityObjectDir(void) const
{
    return this->entityObjectDir.Get();
}

//------------------------------------------------------------------------------
nString 
nEntityObjectServer::GetEntityObjectFilename(nEntityObjectId id) const
{
    n_assert(id);

    // build filename of the entity object
    nString path;
    path = this->entityObjectDir;
    char buffer[32];
    sprintf(buffer, "objects/%08X/%08X.n2", id & IDHIGHMASK, id & IDLOWMASK);
    path.Append(buffer);
    return path;
}

//------------------------------------------------------------------------------
void
nEntityObjectServer::SaveEntityObjects()
{
    switch ( this->GetDataVersion() )
    {
    case 0:
        this->SaveEntityObjectsV0();
        break;
    case 1:
        this->SaveEntityObjectsV1();
        break;
    case 2:
        n_assert2_always( "Invalid data version in current level" );
        this->SaveEntityObjectsV0();
        break;
    }
}

//------------------------------------------------------------------------------
void
nEntityObjectServer::SaveEntityObjectsV0()
{
    // get a filename to get the directory part
    nString path = this->GetEntityObjectFilename(this->newEntityObjectId);

    // create all directories in the path
    nFileServer2::Instance()->MakePath(path.ExtractDirName().Get());

    // save all dirty objects
    // & remove files for all entity objects pending to be deleted
    nEntityObjectId key;
    nEntityObjectInfo * info;
    this->entityObjects->Begin();
    this->entityObjects->Next(key, info);
    while(0 != info)
    {
        if (info->IsDeleted())
        {
            n_assert(this->GetEntityObjectType(key) == Normal);
            this->RemoveEntityObjectPost(key);
        } 
        else
        {
            if (info->GetDirty())
            {
                nEntityObject * obj = info->GetEntityObject();
                if (obj)
                {
                    if (this->GetEntityObjectType(obj->GetId()) == Normal)
                    {
                        this->SaveEntityObjectV0(info->GetEntityObject());
                        info->SetDirty(false);
                    }
#ifndef NGAME // This method only defined in conjurer
                    obj->SetObjectDirty( false );
#endif
                }
            }
        }

        this->entityObjects->Next(key, info);
    }
}


//----------------------------------------------------------------------------
/**
    1 Step . Sort All entities by id
    2 Step.  Save by group

*/
void
nEntityObjectServer::SaveEntityObjectsV1()
{
    nArray<EntryIdAndInfo> sortedEntities(this->entityObjects->GetCount(),1024); // Initial size
    int totalDelete=0;
    int totalDirty=0;
    int totalSave=0;
    bool previousHeaderFormat(false);
    nRef<nScriptServer> scriptServer = nKernelServer::Instance()->GetPersistServer()->GetScriptServer();
    if ( scriptServer.isvalid() )
    {
        previousHeaderFormat = scriptServer->GetUseShortHeader();
        scriptServer->SetUseShortHeader(true);
    }
   

    // save all dirty objects
    // & remove files for all entity objects pending to be deleted
    nEntityObjectId key;
    nEntityObjectInfo * info;
    this->entityObjects->Begin();
    this->entityObjects->Next(key, info);
    while(0 != info)
    {
        EntryIdAndInfo  idAndInfo;
        idAndInfo.id = key;
        idAndInfo.info = *info;

        if (this->GetEntityObjectType( key ) == Normal) 
        {
            sortedEntities.Append( idAndInfo);
        }
        this->entityObjects->Next(key, info);
    }
    sortedEntities.QSort(nEntityObjectServer::EntryIdAndInfo::Cmp); // Sort by id

    int index = 0;
    while ( index < sortedEntities.Size() )
    {
        int countDelete=0;
        int countDirty=0;
        int countSave=0;
        nEntityObjectId  currentGroup = sortedEntities[index].id & IDHIGHMASK ;
        nFileIndex* fileIndex  = this->FileIndexGroupEntity.LookUpFileIndex(currentGroup);
        n_assert2( fileIndex, "Problem for save level" );
        n_verify2( fileIndex->BeginBuild()  , "Problem for save level" );
 
#ifndef NGAME 
        nFileIndex* fileIndexChunk = this->FileIndexGroupChunk.LookUpFileIndex(currentGroup);
        n_assert2( fileIndexChunk , "Problem for save level" );
        n_verify2( fileIndexChunk->BeginBuild() , "Problem for save level" );
#endif

        // The same group
        for ( ; index < sortedEntities.Size() && ( currentGroup == (sortedEntities[index].id & IDHIGHMASK) ) 
              ; ++index
            ) 
        {
            if (sortedEntities[index].info.IsDeleted())
            {
                n_assert(this->GetEntityObjectType(sortedEntities[index].id) == Normal);
                fileIndex->Remove(sortedEntities[index].id);
                #ifndef NGAME
                fileIndexChunk->Remove(sortedEntities[index].id);
                #endif
                this->RemoveEntityObjectPost(sortedEntities[index].id);
                ++countDelete;
            }
            else if (sortedEntities[index].info.GetDirty())
            {
                countDirty++;
                nEntityObject * obj = sortedEntities[index].info.GetEntityObject();
                n_assert2(obj , "The object is dirty but is not loaded , problems for save it" )
                if (obj)
                {
                    nMemFile* file = n_new(nMemFile);
                    n_assert(file);
                    nString emptyFilename;
                    file->Open(emptyFilename, "r");
                    obj->SaveFile(file);
                    fileIndex->AppendOrReplace( file, obj->GetId() );
                    file->Release();
#ifndef NGAME // This method only defined in conjurer
                    obj->SetObjectDirty( false );

                    ncEditor * editor = obj->GetComponent<ncEditor>();
                    if( editor )
                    {
                        file = n_new(nMemFile);
                        n_assert(file);
                        file->Open(emptyFilename, "r");
                        editor->SaveChunkFile(file);
                        fileIndexChunk->AppendOrReplace( file, obj->GetId() );
                        file->Release();
                    }
#endif
                } 
            }
        } // The same group

        fileIndex->EndBuild();
#ifndef NGAME 
        fileIndexChunk->EndBuild();
#endif

        countSave =  fileIndex->GetCountSave();
        n_assert2( countSave > 0 , (nString( "talk to the programmers , The current group is emtpy ") + nString(int(currentGroup)) ).Get() );
        NLOG(entityObjectServer , ( 0 | NLOGUSER , "in %08X: Dirty %d , delete %d , save %d" , currentGroup , countDirty, countDelete, countSave)  );
        totalSave += countSave;
        totalDirty += countDirty;
        totalDelete += countDelete;
    }

    NLOG(entityObjectServer , ( 0 | NLOGUSER , "Total: Dirty %d , delete %d , save %d" ,  totalDirty, totalDelete, totalSave)  );

    // Mark as not dirty the save classe
    // in sortedEntities the info is a copy.
    this->entityObjects->Begin(); 
    this->entityObjects->Next(key, info);
    while(0 != info)
    {
        info->SetDirty(false);
        this->entityObjects->Next(key, info);
    }

    if ( scriptServer.isvalid() )
    {
        scriptServer->SetUseShortHeader(previousHeaderFormat);
    }
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @return the fiel of chunk, 0 if its not exists
*/
nFile* 
nEntityObjectServer::GetChunkFile(nEntityObjectId id)
{
    nFile* file(0);
    switch ( this->GetDataVersion() )
    {
    case 1:
        {
            nFileIndex* fileIndex = this->FileIndexGroupChunk.LookUpFileIndex( id & IDHIGHMASK );
            if ( fileIndex)
            {
                file = fileIndex->GetNewFile(id);
            }
        }

        break;
    default:
        {
            // create filename of debug chunk
            nString filename = this->GetEntityObjectFilename( id);
            nString path = filename.ExtractDirName();
            path.Append( "debug/" );
            filename.StripExtension();
            path.Append( filename.ExtractFileName() );
            path.Append( ".n2" );
            if( nFileServer2::Instance()->FileExists( path.Get() ) )
            {
                file = nKernelServer::Instance()->GetFileServer()->NewFileObject();
                if (!file->Open(path.Get(), "r"))
                {
                    n_assert2_always( (nString("Can't load chunk for entity , ") + path  ).Get() );
                    file->Release();
                    file = 0;
                }
            }   
        }
    }
    return file;
}
#endif  //NGAME

//------------------------------------------------------------------------------
/**
    Scans through the list of entity objects, and all objects in the hash 
    signaled as pending to be deleted (from persistence) are removed.
*/
void
nEntityObjectServer::DiscardDeletedObjects()
{
    // discard all deleted objects
    nEntityObjectId key;
    nEntityObjectInfo * info;
    this->entityObjects->Begin();
    this->entityObjects->Next(key, info);
    while(0 != info)
    {
        if (info->IsDeleted())
        {
            n_assert(this->GetEntityObjectType(key) == Normal);
            // remove the id from the list of entity objects
            this->entityObjects->Remove(key);
        } 

        this->entityObjects->Next(key, info);
    }
}

//------------------------------------------------------------------------------
nEntityObject * 
nEntityObjectServer::LoadEntityObject(nEntityObjectId id)
{
    n_assert(id);
    n_assert(!this->FindEntityObject(id));
    n_assert(this->GetEntityObjectType(id) == nEntityObjectServer::Normal);
 
    // if signaled for deletion, just return null object
    nEntityObjectInfo * info = (*this->entityObjects)[id];
    if (info && info->IsDeleted())
    {
        return 0;
    }

    nEntityObject * neobj(0);
    switch ( this->GetDataVersion() )
    {
    case 0:
        {
            // build filename of the entity object
            nString path = this->GetEntityObjectFilename(id);
            // load entity object 
            neobj = static_cast<nEntityObject *> (kernelServer->Load(path.Get(), false));
        }
        break;
    case 1:
        {
            int group = id & IDHIGHMASK;
            nFileIndex* fileIndex = this->FileIndexGroupEntity.LookUpFileIndex(group);
            nFile* file = fileIndex->GetNewFile(id);
            NLOGCOND(persistence, !file, (NLOGUSER | 0, "Could not load entity 0x%x (%d)", id, id));
            if ( file)
            {
                // load entity object 
                neobj = static_cast<nEntityObject *> (kernelServer->Load( file , false));
                file->Close();
                file->Release();
            }
        }
        break;
    default:
        {
            n_assert2_always("Invalid data version in entityObjectServer");
            nString path = this->GetEntityObjectFilename(id);
            // load entity object 
            neobj = static_cast<nEntityObject *> (kernelServer->Load(path.Get(), false));
        }
        break;  
    }

    if (neobj)
    {
        // check and set identifier
        n_assert(neobj->IsA("nentityobject"));
        neobj->SetId(id);

        // call init instance now, after setting the id
        neobj->InitInstance(nObject::LoadedInstance);
        
        if (!info)
        {
            // add object to the map
            nEntityObjectInfo info2;
            info2.SetEntityObject(neobj);
            this->entityObjects->Add(id, &info2);
        }
        else
        {
            // the info slot already existed in the map
            info->SetEntityObject(neobj);
        }
        neobj->Release();

        #ifndef NGAME
        neobj->BindSignal( nObject::SignalObjectDirty, this, &nEntityObjectServer::TriggerObjectDirty,10 );
        #endif

    }
    //n_assert(!neobj || neobj->GetRefCount() == 1);

    return neobj;
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::SaveEntityObjectV0(nEntityObject * obj) const
{
    n_assert(obj);
    n_assert(obj->GetId());
    n_assert(this->GetEntityObjectType(obj->GetId()) == Normal);

    // build filename of the entity object
    nString path = this->GetEntityObjectFilename(obj->GetId());

    // load entity object
    bool valid = obj->SaveAs(path.Get());
#ifndef NGAME
    if( valid )
    {
        ncEditor * editor = obj->GetComponent<ncEditor>();
        if( editor )
        {
            editor->SaveChunk();
        }
    }
#endif//!NGAME
    return valid;
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::UnloadEntityObject(nEntityObject * obj)
{
    n_assert(obj);

    nEntityObjectId id = obj->GetId();
    n_assert((*this->entityObjects)[id]);
    //n_assert(!((*this->entityObjects)[id])->dirty);
    
    obj->AddRef();

    // first is signaled that is deleted
    this->SignalEntityDeleted( this, obj->GetId() );

    // second is removed from list of entity objects (only when not normal or not deletd)
    // normal objects are persisted and can be deleted, in this case they are kept in 
    // the table to remove their files when user wants to save changes
    if (this->GetEntityObjectType(id) != Normal || !(*this->entityObjects)[id]->IsDeleted())
    {
        this->entityObjects->Remove(id);

        // the only reference at this point must be the one kept by the entity server
        n_assert2( obj->GetRefCount() == 1, "Trying to unload an entity from memory with pending references" );
    }
    else
    {
        // the only reference at this point must be the one kept by the entity server
        // if is deleted, we are being called from RemoveEntityObject which still holds a reference
        n_assert2( obj->GetRefCount() == 2, "Trying to unload an entity from memory with pending references" );
    }

    // then release object
    obj->Release();
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::RemoveEntityObject(nEntityObject * obj)
{
    n_assert(obj);
    
    return this->RemoveEntityObjectById(obj->GetId());
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::RemoveEntityObjectById(nEntityObjectId id)
{
    n_assert(id);

    // Normal objects are handled different for deletion since they are persisted
    // This means we will have to remove the file too when the user requests to 
    // save changes
    nEntityObjectInfo * info = 0;
    if (this->GetEntityObjectType(id) == Normal)
    {
        // check if id in the table, if not add it to keep the deleted state
        info = (*this->entityObjects)[id];
        if (!info)
        {
            nEntityObjectInfo infoNew;
            this->entityObjects->Add(id, &infoNew);
            info = (*this->entityObjects)[id];
            n_assert(info);
        }

        // mark the entry in the table as deleted
        // the object pointer will be reseted after calling UnloadEntityObject
        info->Delete();
    }

    // unload if object is in memory
    nEntityObject * obj = this->FindEntityObject(id);
    if (obj)
    {
        this->UnloadEntityObject(obj);
    }

    // if normal entity to be deleted -> then set entity object pointer to 0
    // the hash entry is not removed, and the deleted state is remembered 
    // (due to previous Delete call)
    if (info)
    {
        info->SetEntityObject(0);
    }

    return true;
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::UnremoveEntityObjectById(nEntityObjectId id)
{
    n_assert(id);

    if (!this->CanBeUnremoved(id))
    {
        return false;
    }

    // remove the id from the list of entity objects
    this->entityObjects->Remove(id);

    n_assert(!this->FindEntityObject(id));
    nEntityObject * obj = this->LoadEntityObject(id);
    if (!obj)
    {
        // if the object cannot be loaded, then delete it definitely
        this->RemoveEntityObjectPost(id);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool 
nEntityObjectServer::CanBeUnremoved(nEntityObjectId id)
{
    n_assert(id);

    // Normal objects are handled different for deletion since they are persisted
    // This means we will have to remove the file too when the user requests to 
    // save changes
    nEntityObjectInfo * info = 0;
    if (this->GetEntityObjectType(id) != Normal)
    {
        return false;
    }

    // check if id in the table, if not add it to keep the deleted state
    info = (*this->entityObjects)[id];
    if (!info)
    {
        return false;
    }

    return info->IsDeleted();
}

//------------------------------------------------------------------------------
/**
    Completes the remove entity object operation. Previously must have been
    issued with remove entity object. This is only applicable to normal 
    objects.
*/
bool
nEntityObjectServer::RemoveEntityObjectPost(nEntityObjectId id)
{
    n_assert(id);

    if (this->GetEntityObjectType(id) == Normal)
    {
        if ( this->GetDataVersion() == 0 )
        {
            // remove the file
            nString path = this->GetEntityObjectFilename(id);
            kernelServer->GetFileServer()->DeleteFile(path.Get());
        }

        // remove the id from the list of entity objects
        this->entityObjects->Remove(id);
    }

    return true;
}

//------------------------------------------------------------------------------
nEntityObject * 
nEntityObjectServer::FindEntityObject(nEntityObjectId id) const
{
    n_assert(id);
    nEntityObjectInfo * info = (*this->entityObjects)[id];
    if (info)
    {
        return info->GetEntityObject();
    }
    return 0;
}

//------------------------------------------------------------------------------
nEntityObject * 
nEntityObjectServer::GetEntityObject(nEntityObjectId id)
{
    n_assert(id);

    nEntityObject * obj = this->FindEntityObject(id);
    if (!obj)
    {
        if (this->GetEntityObjectType(id) == Normal)
        {
            obj = this->LoadEntityObject(id);
        }
    }

    return obj;
}

//------------------------------------------------------------------------------
nEntityObject *
nEntityObjectServer::NewEntityObjectFromType(const char * className, nEntityObjectType otype, nEntityObjectId id, bool initInstance)
{
    n_assert(className);
    n_assert(this->entityObjects);

    nEntityObject * obj = static_cast<nEntityObject *> (nKernelServer::Instance()->New(className, false));
    if (obj)
    {
        if (!id)
        {
            id = this->GetEntityObjectId(otype);
        }
        obj->SetId( id );

        if (initInstance)
        {
            obj->InitInstance(nObject::NewInstance);
        }

        // add object to the map
        nEntityObjectInfo * infoPtr = (*this->entityObjects)[id];
        n_assert2(!infoPtr, "A new entity has been created with an id of an existing entity");
        if (!infoPtr)
        {
            nEntityObjectInfo infoNew;
            this->entityObjects->Add(id, &infoNew);
            infoPtr = (*this->entityObjects)[id];
            n_assert(infoPtr);
        }

        if(infoPtr->IsDeleted())
        {
            infoPtr->Undelete();
        }
        infoPtr->SetEntityObject(obj);
        infoPtr->SetDirty(true);

        obj->Release();

        #ifndef NGAME
        obj->BindSignal( nObject::SignalObjectDirty, this, &nEntityObjectServer::TriggerObjectDirty,10 );
        #endif
    }
    //n_assert(!obj || obj->GetRefCount() == 1);
    return obj;
}

#ifndef NGAME
//------------------------------------------------------------------------------
void
nEntityObjectServer::TriggerObjectDirty(nObject* emitter)
{
    nEntityObject * obj = static_cast<nEntityObject*>(emitter);
    this->SetEntityObjectDirty( obj, true);
}
#endif

//------------------------------------------------------------------------------
nEntityObject *
nEntityObjectServer::NewEntityObject(const char * className)
{
    return NewEntityObjectFromType(className, Normal);
}

//------------------------------------------------------------------------------
nEntityObject *
nEntityObjectServer::NewLocalEntityObject(const char * className)
{
    return NewEntityObjectFromType(className, Local);
}

//------------------------------------------------------------------------------
nEntityObject *
nEntityObjectServer::NewServerEntityObject(const char * className)
{
    return NewEntityObjectFromType(className, Server);
}

//------------------------------------------------------------------------------
nEntityObject *
nEntityObjectServer::NewServerEntityObjectWithId(const char * className, nEntityObjectId id)
{
    return NewEntityObjectFromType(className, Server, id);
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetEntityObjectDirty(nEntityObject * obj, bool dirty)
{
    n_assert(obj);
    n_assert(obj->GetId());
    nEntityObjectInfo * info = (*this->entityObjects)[obj->GetId()];
    if (info)
    {
        info->SetDirty(dirty);
    }
}

//------------------------------------------------------------------------------
void
nEntityObjectServer::SwapEntityObjectIds(nEntityObject * objA, nEntityObject * objB)
{
    n_assert(objA);
    n_assert(objB);
    nEntityObjectId idA = objA->GetId();
    nEntityObjectId idB = objB->GetId();
    
    nEntityObjectInfo oldInfoA;
    oldInfoA.SetEntityObject(objA);
    oldInfoA.SetDirty(true);

    nEntityObjectInfo oldInfoB;
    oldInfoB.SetEntityObject(objB);
    oldInfoB.SetDirty(true);

    this->entityObjects->Remove( idA );
    this->entityObjects->Remove( idB );
    this->entityObjects->Add(idB, &oldInfoA);
    this->entityObjects->Add(idA, &oldInfoB);
    objA->SetId(idB);
    objB->SetId(idA);
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetDirtyAllObjects()
{
    nEntityObjectId key;
    nEntityObjectInfo * info;
    this->entityObjects->Begin();
    this->entityObjects->Next(key, info);
    while(0 != info)
    {
        if ( !info->IsDeleted() )
        {
#ifndef NGAME
            info->GetEntityObject()->SetObjectDirty( true );
#endif
            info->SetDirty(true);
        }
        this->entityObjects->Next(key, info);
    }
    this->dirty = true;
}


//------------------------------------------------------------------------------
void 
nEntityObjectServer::UnloadEntityObjectsFromClass(nEntityClass * neclass)
{
    n_assert(neclass);

    nEntityObjectInfo * info;
    this->entityObjects->Begin();
    info = this->entityObjects->Next();
    while(0 != info)
    {
        if (info->GetEntityObject()->GetClass() == neclass)
        {
            nEntityObject * obj = info->GetEntityObject();
            this->UnloadEntityObject(obj);
        }
        info = this->entityObjects->Next();
    }
}

//------------------------------------------------------------------------------
/**
    UnloadAllEntityObjects unloads from memory all types of objects (normals,
    locals and server). The order of unloading is done by order of reference
    count. This is done in several passes (as many as needed), in each pass
    only the objects with reference count equal to one (the reference stored 
    internally by the entity server) are really unloaded. These objects
    may perform releases on other objects referenced internally, so in next
    pass some will become in candidates for unloading.

    The circular references are not solved now (but it could be done somehow
    with dirty hacks), but detected to avoid getting into an infinite loop.
    Once is detected, mandatory deallocation is tried in an arbitrary order,
    which might not always work.
*/
void 
nEntityObjectServer::UnloadAllEntityObjects()
{
    bool finish = false;
    nEntityObjectInfo * info;

    while (!finish)
    {
        // initialize counters
        int numObjectsUnloaded = 0;
        int numRefCountsGreaterThanOne = 0;

        // do one pass over all objects trying to unload those with refcount 1
        this->entityObjects->Begin();
        info = this->entityObjects->Next();
        while(0 != info)
        {
            nEntityObject * obj = info->GetEntityObject();
            if (obj)
            {
                if (obj->GetRefCount() == 1)
                {
                    this->UnloadEntityObject(obj);
                    numObjectsUnloaded++;
                }
                else
                {
                    n_assert(obj->GetRefCount() > 1);
                    numRefCountsGreaterThanOne++;
                }
            }
            info = this->entityObjects->Next();
        }

        // if no more objects pending then just exist
        if (!numRefCountsGreaterThanOne)
        {
            finish = true;
        }
        else
        {
            // if no unload object in last pass, then give assert warning
            // otherwise try another pass
            if (numObjectsUnloaded == 0)
            {
                n_assert2_always("Entity objects with circular references may exist, trying to release anyway");

                this->entityObjects->Begin();
                info = this->entityObjects->Next();
                while(0 != info)
                {
                    nEntityObject * obj = info->GetEntityObject();
                    if (obj)
                    {
                        this->UnloadEntityObject(obj);
                    }
                    info = this->entityObjects->Next();
                }
                finish = true;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    It is exactly the same algorithm used by UnloadAllEntityObjects, but only
    applied to normal objects.
*/
void 
nEntityObjectServer::UnloadNormalEntityObjects()
{
    bool finish = false;

    while (!finish)
    {
        // initialize counters
        int numObjectsUnloaded = 0;
        int numRefCountsGreaterThanOne = 0;

        // do one pass over all objects trying to unload those with refcount 1
        nEntityObjectInfo * info;
        this->entityObjects->Begin();
        info = this->entityObjects->Next();
        while(0 != info)
        {
            nEntityObject * obj = info->GetEntityObject();
            if (obj && this->GetEntityObjectType(obj->GetId()) == Normal)
            {
                if (obj->GetRefCount() == 1)
                {
                    this->UnloadEntityObject(obj);
                    numObjectsUnloaded++;
                }
                else
                {
                    n_assert(obj->GetRefCount() > 1);
                    numRefCountsGreaterThanOne++;
                }
            }
            info = this->entityObjects->Next();
        }

        // if no more objects pending then just exist
        if (!numRefCountsGreaterThanOne)
        {
            finish = true;
        }
        else
        {
            // if no unload object in last pass, then give assert warning
            // otherwise try another pass
            if (numObjectsUnloaded == 0)
            {
                n_assert2_always("Entity objects with circular references may exist, trying to release anyway");

                this->entityObjects->Begin();
                info = this->entityObjects->Next();
                while(0 != info)
                {
                    nEntityObject * obj = info->GetEntityObject();
                    if (obj && this->GetEntityObjectType(obj->GetId()) == Normal)
                    {
                        n_assert3_always( ("Forcing unload normal object 0x%x (%d) class=%s", obj->GetId(), obj->GetId(), obj->GetClass()->GetName()) );
                        this->UnloadEntityObject(obj);
                    }
                    info = this->entityObjects->Next();
                }
                finish = true;

            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @deprecated not used anymore
*/
void 
nEntityObjectServer::SetBaseEntityObjectBunchId(int)
{
    /// empty
}

//------------------------------------------------------------------------------
nEntityObject*
nEntityObjectServer::GetFirstEntityObject()
{
    this->entityObjects->Begin();

    return this->GetNextEntityObject();
}

//------------------------------------------------------------------------------
nEntityObject*
nEntityObjectServer::GetNextEntityObject()
{
    nEntityObjectInfo * info;

    // skip deleted entity object entries
    do
    {
        info = this->entityObjects->Next();
    } while(info && info->IsDeleted());

    if (0 != info)
    {
        return info->GetEntityObject();            
    }
    
    return 0;
}

//------------------------------------------------------------------------------
bool
nEntityObjectServer::GetEntityObjectDirty(nEntityObjectId id)
{
    nEntityObjectInfo * info;

    info = (*this->entityObjects)[id];

    if (0 != info)
    {
        return info->GetDirty();
    }

    return 0;    
}

//------------------------------------------------------------------------------
nEntityObjectServer::nEntityObjectType 
nEntityObjectServer::GetEntityObjectType(nEntityObjectId id) const
{
    if ((id & IDHIGHMASK) == IDHIGHSERVER)
    {
        return nEntityObjectServer::Server;
    }
    else if ((id & IDHIGHMASK) == IDHIGHLOCAL)
    {
        return nEntityObjectServer::Local;
    }

    return nEntityObjectServer::Normal;
}

//--------------------------------------------------------------------
bool
nEntityObjectServer::IsValidEntityObjectId(nEntityObjectId id)
{
    return ((id & IDLOWMASK) != 0);
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetDataVersion( int version )
{
    this->dataVersion = version;
}

//------------------------------------------------------------------------------
int
nEntityObjectServer::GetDataVersion()
{
    return this->dataVersion;
}

//------------------------------------------------------------------------------
bool
nEntityObjectServer::ChangeDataVersion( int version )
{
    if ( version <= this->GetDataVersion() )
    {
        return false;
    }
    this->SetDataVersion(version);
    this->SetDirtyAllObjects();
    this->SaveEntityObjects();
    this->SaveConfig();
    return true;
}

//------------------------------------------------------------------------------
void 
nEntityObjectServer::SetEntityId(nEntityObject *entity, nEntityObjectId id)
{
    n_assert( 0 == (*this->entityObjects)[id] )
    nEntityObjectInfo * oldInfo = (*this->entityObjects)[entity->GetId()];
    n_assert( oldInfo);
    
    this->entityObjects->Add( id , oldInfo);
    this->entityObjects->Remove( entity->GetId() );

    entity->SetId(id);

}

//------------------------------------------------------------------------------
int
nEntityObjectServer::EntryIdAndInfo::Cmp(const void *elm0, const void *elm1)
{
    const EntryIdAndInfo* info0(static_cast<const EntryIdAndInfo*>(elm0));
    const EntryIdAndInfo* info1(static_cast<const EntryIdAndInfo*>(elm1));


    return info0->id < info1->id ? -1 :  (info0->id == info1->id ? 0 : 1 );

}

//------------------------------------------------------------------------------
nEntityObjectServer::EntryIdAndInfo::~EntryIdAndInfo()
{
    /// Empty , this is needed by nArray
}

//------------------------------------------------------------------------------
bool
nEntityObjectServer::ChangeEntityClass(nEntityObject * entity, const char *newClassName)
{
    nEntityObject * newEntity = NewEntityObjectFromType(newClassName, Normal, 0, false);

    newEntity->CopyCommonComponentsFrom( entity );

    nEntityObjectId oid = entity->GetId();

    // @todo copy all signal bindings

#ifndef NGAME
    entity->SendToLimbo( entity );
#endif
    
    this->RemoveEntityObject( entity );

    this->RemoveEntityObjectPost( oid );

    this->SetEntityId(newEntity, oid);

    newEntity->InitInstance( nObject::NewInstance );

    return false;
}

//------------------------------------------------------------------------------
