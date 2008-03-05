//------------------------------------------------------------------------------
//  npersistserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nscriptserver.h"
#include "kernel/npersistserver.h"
#include "kernel/ndependencyserver.h"
#include "kernel/nlogclass.h"
#include "kernel/nfile.h"

nNebulaScriptClass(nPersistServer, "nroot");

//------------------------------------------------------------------------------
NCREATELOGLEVEL_REL(persistence, "Persistence", true, 0);

//------------------------------------------------------------------------------
/**
*/
nPersistServer::nPersistServer() :
    refSaver("/sys/servers/script"),
    file(0),
    saveMode(SAVEMODE_FOLD),
    cloneTarget(0),
    origCwd(0),
    saveLevel(0),
    cmdDepServers(16, 16),
    saveType(SAVETYPE_PERSIST),
    releaseFile(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nPersistServer::~nPersistServer()
{
    n_assert(0 == this->file);
}

//------------------------------------------------------------------------------
/**
*/
void 
nPersistServer::SetSaveLevel(int l)
{
    this->saveLevel = l;
}

//------------------------------------------------------------------------------
/**
*/
int 
nPersistServer::GetSaveLevel(void)
{
    return this->saveLevel;
}


//------------------------------------------------------------------------------
/**
    Set class of script server which should be used to save objects.
    Creates a local script server object if not exists.

    @param  saverClass      name of a nScriptServer derived class
    @return                 currently always true, breaks if object could not be created
*/
bool
nPersistServer::SetSaverClass(const nString& saverClass)
{
    n_assert(!saverClass.IsEmpty());

    // first check if the default script server matches this class
    nScriptServer* defaultServer = (nScriptServer*) kernelServer->Lookup("/sys/servers/script");
    if (defaultServer && (strcmp(saverClass.Get(), defaultServer->GetClass()->GetName()) == 0))
    {
        this->refSaver.set(0);
        this->refSaver = defaultServer;
    }
    else
    {
        // otherwise check if a matching local script server already exists
        nScriptServer* localServer = (nScriptServer*) this->Find(saverClass.Get());
        if (!localServer)
        {
            kernelServer->PushCwd(this);
            localServer = (nScriptServer*) kernelServer->New(saverClass.Get(), saverClass.Get());
            kernelServer->PopCwd();
        }
        n_assert(localServer);
        this->refSaver.set(0);
        this->refSaver = localServer;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Get name of class which is currently used to save objects.
*/
nString
nPersistServer::GetSaverClass()
{
    return this->refSaver->GetClass()->GetName();
}

//------------------------------------------------------------------------------
/**
    return the current script server used by the persist server
*/
nScriptServer * 
nPersistServer::GetScriptServer()
{
    return this->refSaver.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Get a loader script server.
*/
nScriptServer*
nPersistServer::GetLoader(const char* loaderClass)
{
    n_assert(loaderClass);
    nScriptServer* loaderServer = 0;

    // first check if the default script server matches this class
    loaderServer = (nScriptServer*) kernelServer->Lookup("/sys/servers/script");
    if (loaderServer && (strcmp(loaderClass, loaderServer->GetClass()->GetName()) == 0))
    {
        // the default server is the right one
    }
    else
    {
        // otherwise check if a matching local script server already exists
        loaderServer = (nScriptServer*) this->Find(loaderClass);
        if (!loaderServer)
        {
            // if not, create
            kernelServer->PushCwd(this);
            loaderServer = (nScriptServer*) kernelServer->New(loaderClass, loaderClass);
            kernelServer->PopCwd();
        }
        n_assert(loaderServer);
    }

    return loaderServer;
}

//------------------------------------------------------------------------------
/**
     - 04-Nov-98   floh    created
     - 05-Nov-98   floh    autonome Objects gekillt
     - 18-Dec-98   floh    + SAVEMODE_CLONE
     - 09-Nov-04   enlight + nObject support
*/
bool 
nPersistServer::BeginObject(nObject *o, const char *name, bool isObjNamed)
{
    bool retval = false;
    nFile* file;
    switch (this->saveMode) 
    {
        case SAVEMODE_FOLD:
            file = kernelServer->GetFileServer()->NewFileObject();
            if (file->Open(name, "w"))
            {
                this->releaseFile = true;
                retval = this->BeginFoldedObject(o, NULL, file, false, isObjNamed);
            } else
            {
                file->Close();
                file->Release();
            }
            break;
        case SAVEMODE_CLONE:
            retval = this->BeginCloneObject(o, 0, name, isObjNamed);
            break;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nPersistServer::BeginObject(nObject *o, nFile* file, bool isObjNamed )
{
    n_assert(file);
    n_assert(this->saveMode == SAVEMODE_FOLD );
    return this->BeginFoldedObject(o, NULL, file, false, isObjNamed);
}


//------------------------------------------------------------------------------
/**
    Start writing object, with constructor cmd instead of a
    generic "new [class] [name]". If cmd pointer is NULL,
    no constructor statement will be written, just the select
    statement.

     - 29-Feb-00   floh    created
*/
bool 
nPersistServer::BeginObjectWithCmd(nObject *o, nCmd *cmd, const char *name)
{
    bool retval = false;
    bool sel_only = (cmd==NULL) ? true : false;
    nFile* file;
    switch (this->saveMode) 
    {
        case SAVEMODE_FOLD:
            file = kernelServer->GetFileServer()->NewFileObject();
            if (file->Open(name, "w"))
            {
                this->releaseFile = true;
                retval = this->BeginFoldedObject(o, cmd, file, sel_only, true);
            } else
            {
                file->Release();
            }
            break;
        case SAVEMODE_CLONE:
            if ( o->IsA("nroot") )
            {
                retval = this->BeginCloneObject(o, cmd, name, true);
            }
            else
            {
                retval = this->BeginCloneObject(o, cmd, name, false);
            }
            break;
    }
    if (cmd) 
    {
        cmd->GetProto()->RelCmd(cmd);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nPersistServer::BeginObjectWithCmd(nObject *o, nCmd *cmd, nFile* file)
{
    n_assert( file || this->file);
    n_assert(this->saveMode == SAVEMODE_FOLD);

    bool retval = false;
    bool sel_only = (cmd==NULL) ? true : false;  
    retval = this->BeginFoldedObject(o, cmd, file, sel_only, true);
    if (cmd) 
    {
        cmd->GetProto()->RelCmd(cmd);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
     - 04-Nov-98   floh    created
     - 05-Nov-98   floh    autonome Objects gekillt
     - 18-Dec-98   floh    + SAVEMODE_CLONE
     - 09-Nov-04   enlight + nObject support
*/
bool 
nPersistServer::EndObject(bool isObjNamed)
{
    bool retval = false;
    switch (this->saveMode) 
    {
        case SAVEMODE_FOLD:
            retval = this->EndFoldedObject(isObjNamed);
            break;
        case SAVEMODE_CLONE:
            retval = this->EndCloneObject(isObjNamed);
            break;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
     - 04-Nov-98   floh    created
     - 19-Dec-98   floh    neues Arg, id
                           + jetzt doch etwas komplexer, sucht das
                             Command selbst raus, indem es die
                             Klassen des Objekts durchsucht
     - 08-Aug-99   floh    + benutzt nicht mehr nCmdProto::GetCmd(),
                             sondern nCmdProto::NewCmd()
*/
nCmd*
nPersistServer::GetCmd(nObject *o, nFourCC id)
{
    n_assert(o);
    nCmdProto *cp = o->GetClass()->FindCmdById(id);
    if (!cp)
    {
        if (o->IsA("nroot"))
        {
            nString fullname = ((nRoot *) o)->GetFullName();
            NLOG(persistence, (NLOGUSER | 0, "nPersistServer::GetCmd(): unknown command '%s' for object '%s' of class %s\n",
                    n_fourcctostr(id), fullname.Get(), o->GetClass()->GetName()));
        }
        else
        {
            NLOG(persistence, (NLOGUSER | 0, "nPersistServer::GetCmd(): unknown command '%s' for object of class %s\n",
                    n_fourcctostr(id), o->GetClass()->GetName()));
        }
        return 0;
    }
    return cp->NewCmd();
}

//------------------------------------------------------------------------------
/**
    Ignore if level is less or equal the internal save level
    defined by 'SetSaveLevel'.
    
     - 16-Jun-00   floh    created
*/
bool 
nPersistServer::PutCmd(int level, nCmd *cmd)
{
    bool success = false;
    if (this->saveLevel <= level) 
    {
        n_assert(cmd);
        if (SAVEMODE_CLONE == this->saveMode) 
        {
            // in clone mode, send cmd directly to object
            n_assert(this->cloneTarget);

            // OLD CODE:
            //success = this->cloneTarget->Dispatch(cmd);

            // OLD CODE is not valid since we added automatic wrappers & 
            // entities with multiple inheritance
            nCmdProto * cloneCmdProto = this->cloneTarget->GetClass()->FindCmdById( cmd->GetId() );
            n_assert( cloneCmdProto );
            nCmd * cloneCmd = cloneCmdProto->NewCmd();
            n_assert( cloneCmd );
            cloneCmd->CopyInArgsFrom( cmd );
            success = this->cloneTarget->Dispatch(cloneCmd);
            cloneCmdProto->RelCmd( cloneCmd );
            // ignore cmd output arguments (not check by anybody)
        } 
        else 
        {
            success = this->refSaver->WriteCmd(this->file, cmd);
        }
    }
    cmd->GetProto()->RelCmd(cmd);
    return success;
}

//------------------------------------------------------------------------------
/**
    This is the same as a PutCmd(0,cmd).

     - 04-Nov-98   floh    created
     - 08-Aug-99   floh    nCmd wird nicht mehr per delete(), sondern
                           per Cmd-Proto freigegeben
     - 20-Jan-00   floh    rewritten to ref_ss
*/
bool 
nPersistServer::PutCmd(nCmd *cmd)
{
    return this->PutCmd(0, cmd);
}

//------------------------------------------------------------------------------
/**
    @param o nObject with the command
    @param level level of save
    @param fourcc command to save
    @param ... parameters (if any) of the command
    Ignore if level is less or equal the internal save level
    defined by 'SetSaveLevel'.
*/
bool 
nPersistServer::PutLevel( nObject *o, int level, nFourCC fourcc, ... )
{
    bool success = false;
    if (this->saveLevel <= level) 
    {
        nCmd * cmd = this->GetCmd( o, fourcc );
        n_assert2( cmd, "Persist server do not found command" );

        if( cmd ){
            va_list marker;

            va_start( marker, fourcc );
            cmd->CopyInArgsFrom( marker );
            va_end( marker );

            if (SAVEMODE_CLONE == this->saveMode) 
            {
                // in clone mode, send cmd directly to object
                n_assert(this->cloneTarget);
                success = this->cloneTarget->Dispatch(cmd);
            } 
            else 
            {
                success = this->refSaver->WriteCmd(this->file, cmd);
            }
            cmd->GetProto()->RelCmd(cmd);
        }
    }
    return success;
}

//------------------------------------------------------------------------------
/**
    @param o nObject with the command
    @param fourcc command to save
    @param ... parameters (if any) of the command
    This is the same as a PutCmd(0,cmd) but creating before the cmd.
*/
bool 
nPersistServer::Put( nObject *o, nFourCC fourcc, ... )
{
    nCmd * cmd = this->GetCmd( o, fourcc );
    n_assert2( cmd, "Persist server do not found command" );

    if( cmd ){
        va_list marker;

        va_start( marker, fourcc );
        cmd->CopyInArgsFrom( marker );
        va_end( marker );

        return this->PutCmd( 0, cmd );
    }
    return false;
}

//------------------------------------------------------------------------------
/**
     - 05-Nov-98   floh    created
*/
void 
nPersistServer::SetSaveMode(nPersistServer::nSaveMode sm)
{
    this->saveMode = sm;
}

//------------------------------------------------------------------------------
/**
     - 05-Nov-98   floh    created
*/
nPersistServer::nSaveMode 
nPersistServer::GetSaveMode()
{
    return this->saveMode;
}

//--------------------------------------------------------------------
/**
*/
nPersistServer::nDepServerEntry *
nPersistServer::FindDepServerEntry(nString objClass, nString command)
{
    int i;
    for (i = 0; i < this->cmdDepServers.Size(); i++)
    {
        if (cmdDepServers[i].objClass == objClass &&
            cmdDepServers[i].command == command)
        {
            return &cmdDepServers[i];
        }
    }
    return 0;
}

//--------------------------------------------------------------------
/**
*/
nPersistServer::nDepServerEntry *
nPersistServer::GetDepServerEntry(nString objClass, nString command)
{
    nDepServerEntry *entry = FindDepServerEntry(objClass, command);
    if (entry)
    {
        return entry;
    }

    nDepServerEntry newEntry;
    newEntry.objClass = objClass;
    newEntry.command = command;
    this->cmdDepServers.Append(newEntry);
    return &this->cmdDepServers[this->cmdDepServers.Size() - 1];
}

//------------------------------------------------------------------------------
/**
*/
void
nPersistServer::SetDependencyClass(nString objectClass, nString command, nString depClass)
{
    n_assert(!objectClass.IsEmpty());

    n_assert(!command.IsEmpty());
    nRoot *depRoot = kernelServer->Lookup("/sys/servers/dependency");
    if (!depRoot)
    {
        depRoot = kernelServer->New("nroot", "/sys/servers/dependency");
    }

    nDepServerEntry *entry = this->GetDepServerEntry(objectClass, command);
    n_assert(entry);
    nDependencyServer *depServer = (nDependencyServer *) kernelServer->Lookup("/sys/servers/dependency/basic");
    if (depServer && (strcmp(depServer->GetClass()->GetName(), depClass.Get()) == 0))
    {
        // assign depServer to object class and command
        entry->refDepServer = depServer;
    }
    else
    {
        nDependencyServer *cmdServer = (nDependencyServer *) depRoot->Find(depClass.Get());
        if (!cmdServer)
        {
            kernelServer->PushCwd(depRoot);
            cmdServer = (nDependencyServer *) kernelServer->New(depClass.Get(), depClass.Get());
            kernelServer->PopCwd();
        }
        // assign cmdserver to object class and command
        entry->refDepServer = cmdServer;
    }
}

//------------------------------------------------------------------------------
/**
*/
nString
nPersistServer::GetDependencyClass(nString objClass, nString command)
{
    nDepServerEntry *entry = this->FindDepServerEntry(objClass, command);
    if (entry && entry->refDepServer.isvalid())
    {
        return entry->refDepServer->GetClass()->GetName();
    }
    return "ndependencyserver";
}

//------------------------------------------------------------------------------
/**
*/
nDependencyServer *
nPersistServer::GetDependencyServer(nString objClass, nString command)
{
    nDepServerEntry *entry = this->FindDepServerEntry(objClass, command);
    if (entry && entry->refDepServer.isvalid())
    {
        return entry->refDepServer.get();
    }
    return 0;
}


//------------------------------------------------------------------------------
/**
*/
#ifdef __ZOMBIE_EXPORTER__
bool 
nPersistServer::PutComment(const char* comment)
{
    bool success;
    if ( 0 == comment  )
    {
        return true;
    }
    if (SAVEMODE_CLONE == this->saveMode) 
    {
        // in clone mode, send cmd directly to object
        n_assert(this->cloneTarget);
        success = true;
    } 
    else 
    {
        success = this->refSaver->WriteComment(this->file, comment);
    }
    return success;
}
#endif

//------------------------------------------------------------------------------
/**
    set the save type
*/
void 
nPersistServer::SetSaveType(nPersistServer::nSaveType saveType)
{
    this->saveType = saveType;
}

//------------------------------------------------------------------------------
/**
    get the save type
*/
nPersistServer::nSaveType 
nPersistServer::GetSaveType(void)
{
    return this->saveType;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
