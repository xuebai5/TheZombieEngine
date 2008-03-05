#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncloaderclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nloaderserver.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncLoaderClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncLoaderClass)
    NSCRIPT_ADDCMD_COMPCLASS('SRSF', void, SetResourceFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GRSF', const char *, GetResourceFile, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SASE', void, SetAsyncEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GASE', bool, GetAsyncEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MINV', void, Invalidate, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ZRFR', void, Refresh, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncLoaderClass::ncLoaderClass() :
    asyncEnabled(false),
    numLoadedEntities(0),
    retainResources(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncLoaderClass::~ncLoaderClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    load resources for this entity.
    override in subclasses to implement specific load functionality.
*/
bool
ncLoaderClass::LoadResources()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    unload resources for this entity.
    override in subclasses to implement specific unload functionality.
*/
void
ncLoaderClass::UnloadResources()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    check for valid resources for this entity.
    override in subclasses to implement specific check functionality.
*/
bool
ncLoaderClass::AreResourcesValid()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    reload class resources and trigger reloading entities of this class.
    override in subclasses to implement specific reload functionality.
*/
void
ncLoaderClass::ReloadResources()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    force reinitialization, doesn't unload current resources
*/
void
ncLoaderClass::Refresh()
{
    this->ReloadResources();
}

//------------------------------------------------------------------------------
/**
    force reloading of class resources
*/
void
ncLoaderClass::Invalidate()
{
    this->ReloadResources();

    if ( this->AreResourcesValid() )
    {
        this->UnloadResources();
    }
}

//------------------------------------------------------------------------------
/**
    called by entities of this class when per-entity resources are loaded.
    override in subclass to implement specific pre-load funcionality.
*/
void
ncLoaderClass::EntityLoaded(nEntityObject* /*entityObject*/)
{
    ++this->numLoadedEntities;
}

//------------------------------------------------------------------------------
/**
    called by entities of this class when per-entity resources are unloaded.
    override in subclass to implement specific unload funcionality.
*/
void
ncLoaderClass::EntityUnloaded(nEntityObject* /*entityObject*/)
{
    --this->numLoadedEntities;
}

//------------------------------------------------------------------------------
/**
*/
nRoot *
ncLoaderClass::LoadResource(nRoot *root, const nString& resourceFile)
{
    nKernelServer* kernelServer = nKernelServer::Instance();

    nString objName(resourceFile);
    objName.ConvertBackslashes();
    objName = objName.ExtractFileName();
    objName.StripExtension();

    nRoot *obj = root->Find(objName.Get());
    if (obj)
    {
        obj->AddRef();
    }
    else
    {
        kernelServer->PushCwd(root);
        obj = (nRoot *) kernelServer->Load(resourceFile.Get());
        kernelServer->PopCwd();
    }

    return obj;
}

//------------------------------------------------------------------------------
/**
*/
nRoot *
ncLoaderClass::LoadResourceAs(nRoot *root, const nString& resourceFile, const nString& objName)
{
    nKernelServer* kernelServer = nKernelServer::Instance();

    nRoot *obj = root->Find(objName.Get());
    if (obj)
    {
        obj->AddRef();
    }
    else
    {
        kernelServer->PushCwd(root);
        obj = (nRoot *) kernelServer->LoadAs(resourceFile.Get(), objName.Get());
        kernelServer->PopCwd();
    }

    return obj;
}

// commands

//------------------------------------------------------------------------------
/**
*/
bool
ncLoaderClass::SaveCmds(nPersistServer * ps)
{
    if (this->GetResourceFile())
    {
        // --- setresourcefile ---
        nCmd *cmd = ps->GetCmd(this->GetEntityClass(), 'SRSF');
        cmd->In()->SetS(this->GetResourceFile());
        ps->PutCmd(cmd);
    }

    if (this->GetAsyncEnabled())
    {
        // --- setasyncenabled ---
        nCmd *cmd = ps->GetCmd(this->GetEntityClass(), 'SASE');
        cmd->In()->SetB(this->GetAsyncEnabled());
        ps->PutCmd(cmd);
    }

    return true;
}
