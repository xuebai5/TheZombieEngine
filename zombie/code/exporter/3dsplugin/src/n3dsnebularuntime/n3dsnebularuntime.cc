//------------------------------------------------------------------------------
//  n3dsNebulaRuntime.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "n3dsNebulaRuntime/n3dsNebulaRuntime.h"
#include "kernel/nscriptserver.h"
#include "nscene/nsceneserver.h"
#include "variable/nvariableserver.h"
#include "kernel/nwin32loghandler.h"
#include <windows.h>
#include "nphysics/nphysicsserver.h"
#include "resource/nresourceserver.h"
#include "entity/nentityobjectserver.h"
#include "entity/nentityclassserver.h"
#include "entity/ncomponentclassserver.h"
#include "entity/ncomponentobjectserver.h"
#include "misc/nprefserver.h"
#include "nspatial/nspatialserver.h"
#include "gameplay/ngamematerialserver.h"
#include "nmaterial/nmaterialserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nmaterial);
nNebulaUsePackage(nscene);
nNebulaUsePackage(nlua);
nNebulaUsePackage(nphysics);
nNebulaUsePackage(noctree);
nNebulaUsePackage(nentity);
nNebulaUsePackage(nspatial);
nNebulaUsePackage(framework);
nNebulaUsePackage(zombieentityexp);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ncshared);
nNebulaUsePackage(n3dsexporterdeps);
nNebulaUsePackage(nnavmesh);
nNebulaUsePackage(nanimation);

nAtomTable * n3dsNebulaRuntime::atomTable(0);
//------------------------------------------------------------------------------
/**
*/
n3dsNebulaRuntime::n3dsNebulaRuntime() :
    isOpen(false),
    kernelServer(0),
    scriptServer(0),
    varServer(0),
    sceneServer(0),
    compClassServer(0),
    compObjectServer(0)
{
    // empty
    logHandler = n_new(nWin32LogHandler("n3dsPluginlog"));
}

//------------------------------------------------------------------------------
/**
*/
n3dsNebulaRuntime::~n3dsNebulaRuntime()
{
    if (this->isOpen)
    {
        this->Close();
    }

    n_delete ( this->logHandler );
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsNebulaRuntime::Open( const nString& projectPath)
{
    n_assert(!this->isOpen);

    if( !this->atomTable )
    {
        this->atomTable = nAtomTable::Instance();
    }
    if( !this->kernelServer )
    {
        this->kernelServer = n_new(nKernelServer);
    }
    if( !this->compClassServer )
    {
        this->compClassServer = n_new(nComponentClassServer);
    }
    if( !this->compObjectServer )
    {
        this->compObjectServer = n_new(nComponentObjectServer);
    }

    kernelServer->New( "nprofilermanager", "/sys/servers/profilemanager" );     

    this->kernelServer->SetLogHandler( this->logHandler );
    
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(nentity);
    kernelServer->AddPackage(nscene);
    kernelServer->AddPackage(nmaterial);
    kernelServer->AddPackage(noctree);
    kernelServer->AddPackage(nspatial);
    kernelServer->AddPackage(nlua);
    kernelServer->AddPackage(nphysics);
    kernelServer->AddPackage(framework);
    kernelServer->AddPackage(ncshared);
    kernelServer->AddPackage(nnavmesh);
	kernelServer->AddPackage(nanimation);
    kernelServer->AddPackage(n3dsexporterdeps);
    kernelServer->AddPackage(zombieentityexp);

    char* s = getenv("NEBULA2_HOME");// temporal solution
    n_assert2( s , "The environment variable NEBULA2_HOME is not defined");

    this->kernelServer->GetFileServer()->SetAssign("home", s );
    this->scriptServer         = (nScriptServer*)   kernelServer->New("nluaserver", "/sys/servers/script");
    this->varServer            = (nVariableServer*) kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->resourceServer       = (nResourceServer*) kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->sceneServer          = (nSceneServer*)    kernelServer->New("nsceneserver", "/sys/servers/scene");
    this->physicsServer        = (nPhysicsServer*)  kernelServer->New("nphysicsserver", "/sys/servers/physic");
    this->gamematerialServer   = (nGameMaterialServer*)  kernelServer->New("ngamematerialserver", "/sys/servers/gamematerial");
    kernelServer->New( "nprefserver", "/sys/servers/prefserver" );
    kernelServer->New( "nentityobjectserver", "/sys/servers/entityobject" );
    kernelServer->New( "nentityclassserver", "/sys/servers/entityclass" );
    kernelServer->New( "nspatialserver", "/sys/servers/spatial" );
	kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->materialServer = (nMaterialServer*)kernelServer->New("nmaterialserver", "/sys/servers/material");
    
    nString projAssign = projectPath;

    if (!(projAssign=="")) 
        
    {
        projAssign+=nString("/");
        projAssign.ConvertBackslashes();
        this->kernelServer->GetFileServer()->SetAssign("proj", projAssign.Get());

    }
    // run the common Nebula initialization
    nString result;
    this->scriptServer->RunScript("home:data/scripts/startup.lua", result);
    this->scriptServer->RunFunction("OnStartup", result);

    //n_verify(this->materialServer->Open() );

    this->isOpen = true;
    return true;
}
//------------------------------------------------------------------------------
/**
*/
void
n3dsNebulaRuntime::Close()
{
    n_assert(this->isOpen);

    nEntityObjectServer::Instance()->UnloadAllEntityObjects();
    nSpatialServer::Instance()->Release();
    nEntityClassServer::Instance()->Release();
    nEntityObjectServer::Instance()->Release();
    nPrefServer::Instance()->Release();
    this->materialServer->Release();
    this->varServer->Release();
    this->scriptServer->Release();
    this->sceneServer->Release();
    this->gamematerialServer->Release();
    this->physicsServer->Release();
    this->resourceServer->Release();
    this->varServer = 0;
    this->scriptServer = 0;

    if (this->compObjectServer)
    {
        n_delete(this->compObjectServer);
        this->compObjectServer = 0;
    }

    if (this->compClassServer)
    {
        n_delete(this->compClassServer);
        this->compClassServer = 0;
    }

    if (this->kernelServer)
    {
        n_delete( this->kernelServer );
        this->kernelServer = 0;
    }

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsNebulaRuntime::WhenDetachProcess()
{
    if ( n3dsNebulaRuntime::atomTable)
    {
        n_delete(n3dsNebulaRuntime::atomTable);
        n3dsNebulaRuntime::atomTable = 0;
    }
}
//------------------------------------------------------------------------------
/**
*/
nKernelServer*
n3dsNebulaRuntime::GetKernelServer() const
{
    n_assert(this->kernelServer);
    return this->kernelServer;
}

//------------------------------------------------------------------------------
/**
*/
nFileServer2*
n3dsNebulaRuntime::GetFileServer() const
{
    n_assert(this->kernelServer);
    return this->kernelServer->GetFileServer();
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer*
n3dsNebulaRuntime::GetScriptServer() const
{
    n_assert(this->scriptServer);
    return this->scriptServer;
}

//------------------------------------------------------------------------------
/**
*/
nVariableServer*
n3dsNebulaRuntime::GetVariableServer() const
{
    n_assert(this->varServer);
    return this->varServer;
}


//------------------------------------------------------------------------------
