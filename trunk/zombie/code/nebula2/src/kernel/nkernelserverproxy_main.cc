#include "precompiled/pchnkernel.h"
/**
   @file nkernelserverproxy_main.cc
   @author Luis Jose Cabellos Gomez

   (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nkernelserverproxy.h"
#include "kernel/nkernelserver.h"

nNebulaScriptClass(nKernelServerProxy, "nroot");

nKernelServerProxy* nKernelServerProxy::singleton = 0;

//------------------------------------------------------------------------------
#ifndef NGAME
NSIGNAL_DEFINE( nKernelServerProxy, ObjectDeleted );
NSIGNAL_DEFINE( nKernelServerProxy, ClassDeleted );
#endif

//------------------------------------------------------------------------------
/**
*/
nKernelServerProxy::nKernelServerProxy()
{
    n_assert( !nKernelServerProxy::singleton );
    nKernelServerProxy::singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nKernelServerProxy::~nKernelServerProxy()
{
    n_assert( nKernelServerProxy::singleton );
    nKernelServerProxy::singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
nRoot *
nKernelServerProxy::LoadAs( const nString & path, const nString & name )
{
    nRoot * object = nKernelServer::Instance()->LoadAs( path.Get(), name.Get() );
    return object;
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::Lock()
{
    nKernelServer::Instance()->Lock();
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::Unlock()
{
    nKernelServer::Instance()->Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::RemClass(const nString & className)
{
    nClass* cl = nKernelServer::Instance()->FindClass(className.Get());
    nKernelServer::Instance()->RemClass(cl);
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::New(const char* className, const char* objectName)
{
    nRoot* object = nKernelServer::Instance()->New(className, objectName);
    return object;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nKernelServerProxy::CreateClass(const nString & name)
{
    nClass* cl = nKernelServer::Instance()->CreateClass(name.Get());
    if (cl)
    {
        return cl->GetName();
    }
    return "";
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::ReleaseClass(const nString & className)
{
    nClass* cl = nKernelServer::Instance()->FindClass(className.Get());
    nKernelServer::Instance()->ReleaseClass(cl);
}

//------------------------------------------------------------------------------
/**
*/
const char*
nKernelServerProxy::FindClass(const nString & name)
{
    nClass* cl = nKernelServer::Instance()->FindClass(name.Get());
    if (cl)
    {
        return cl->GetName();
    }
    return "";    
}


//------------------------------------------------------------------------------
/**
*/
bool
nKernelServerProxy::IsClassLoaded(const nString & name)
{
    nClass* cl = nKernelServer::Instance()->IsClassLoaded(name.Get());
    return cl != 0;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nKernelServerProxy::NewObject(const nString & className)
{
    nObject* object = nKernelServer::Instance()->New(className.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::NewNoFail(const nString & className, const nString & objectName)
{
    nRoot* object = nKernelServer::Instance()->NewNoFail(className.Get(), objectName.Get());
    return object;
}
//------------------------------------------------------------------------------
/**
*/
nObject*
nKernelServerProxy::NewObjectNoFail(const nString & className)
{
    nObject* object = nKernelServer::Instance()->NewNoFail(className.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nKernelServerProxy::Load(const nString & path)
{
    nObject* object = nKernelServer::Instance()->Load(path.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::Lookup(const nString & path)
{
    nRoot* object = nKernelServer::Instance()->Lookup(path.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::SetCwd(nObject * newCwd)
{
    nKernelServer::Instance()->SetCwd(newCwd);
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nKernelServerProxy::GetCwd()
{
    nObject* object = nKernelServer::Instance()->GetCwd();
    return object;
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::PushCwd(nObject* newCwd)
{
    nKernelServer::Instance()->PushCwd(newCwd);
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nKernelServerProxy::PopCwd()
{
    nObject* object = nKernelServer::Instance()->PopCwd();
    return object;
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::ReplaceFileServer(const nString & className)
{
    nKernelServer::Instance()->ReplaceFileServer(className.Get());
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::GetPersistServer()
{
    nRoot* server = (nRoot*) nKernelServer::Instance()->GetPersistServer();
    return server;
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::GetFileServer()
{
    nRoot* server = (nRoot*) nKernelServer::Instance()->GetFileServer();
    return server;    
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::GetRemoteServer()
{
    nRoot* server = (nRoot*) nKernelServer::Instance()->GetRemoteServer();
    return server;
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nKernelServerProxy::GetTimeServer()
{
    nRoot* server = (nRoot*) nKernelServer::Instance()->GetTimeServer();
    return server;
}

//------------------------------------------------------------------------------
/**
*/
void
nKernelServerProxy::Trigger(void)
{
    nKernelServer::Instance()->Trigger();
}

//------------------------------------------------------------------------------
/**
    Tell if a class is native.
*/
bool
nKernelServerProxy::IsClassNative(const char * className) const
{
    n_assert(className);

    nClass * clazz;

    clazz = nKernelServer::Instance()->FindClass(className);
    if (0 == clazz)
    {
        return false;
    }

    return (clazz->IsNative());
}

//------------------------------------------------------------------------------
/**
    Tell if a class has a specific command.
*/
bool
nKernelServerProxy::HasClassCommand(const char * className, const char * commandName) const
{
    n_assert(className);
    n_assert(commandName);

    nClass * clazz;

    clazz = nKernelServer::Instance()->FindClass(className);
    if (0 == clazz)
    {
        return false;
    }

    return (0 != clazz->FindCmdByName(commandName));
}

//------------------------------------------------------------------------------
/**
    Get the parent class of the given class
*/
const char*
nKernelServerProxy::GetSuperClass( const char* className ) const
{
    n_assert( className );

    // Get given class
    nClass* clazz( nKernelServer::Instance()->FindClass(className) );
    if ( !clazz )
    {
        return NULL;
    }

    // Get parent class
    clazz = clazz->GetSuperClass();
    if ( !clazz )
    {
        return NULL;
    }

    // Return name of parent class
    return clazz->GetName();
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
