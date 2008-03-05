#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nscenesurfacedependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nscenesurfacedependencyserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nSceneSurfaceDependencyServer, "ndependencyserver");

//------------------------------------------------------------------------------
/**
*/
nSceneSurfaceDependencyServer::nSceneSurfaceDependencyServer() :
    counter(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSceneSurfaceDependencyServer::~nSceneSurfaceDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneSurfaceDependencyServer::Init()
{
    nDependencyServer::Init();
    this->counter = 0; // TODO restart for every saved object
}

//------------------------------------------------------------------------------
/**
*/
nObject *
nSceneSurfaceDependencyServer::GetObject(const nString& filename, const nString& command)
{
    nString parentName(filename.ExtractDirName());  // "path_to_object/object/materials/"
    parentName.StripTrailingSlash();                // "path_to_object/object/materials"
    parentName = parentName.ExtractToLastSlash();   // "path_to_object/object/"
    parentName.StripTrailingSlash();                // "path_to_object/object"
    parentName = parentName.ExtractFileName();      // "object"

    nString objectName(filename.ExtractFileName());
    objectName.StripExtension();

    nString objectPath("/usr/deps/");               // "/usr/deps/"
    objectPath.Append(command);                     // "/usr/deps/setsurface"
    objectPath.Append("/");                         // "/usr/deps/setsurface/"
    objectPath.Append(parentName.ExtractFileName());// "/usr/deps/setsurface/parent"
    objectPath.Append("/");                         // "/usr/deps/setsurface/parent/"
    objectPath.Append(objectName);                  // "/usr/deps/setsurface/parent/object"

    nRoot *object = kernelServer->Lookup(objectPath.Get());
    if (!object)
    {
        object = kernelServer->LoadAs(filename.Get(), objectPath.Get());
    }

    return object;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSceneSurfaceDependencyServer::GetObjectPath(nObject *const object, const nString& /*command*/, nString& path)
{
    const nRoot *surface = (const nRoot *) object;

    path.Clear();
    path.Append(nDependencyServer::parentPath); // "path_to_object/object/scene/"
    path = path.ExtractDirName();               // "path_to_object/object/"
    path.Append("materials/");                  // "path_to_object/object/materials/"
    path.Append(surface->GetName());            // "path_to_object/object/materials/surfacename"
    path.Append(".n2");                         // "path_to_object/object/materials/surfacename.n2"

    return true;
}
