#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nanimatordependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nanimatordependencyserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nAnimatorDependencyServer, "ndependencyserver");

//------------------------------------------------------------------------------
/**
*/
nAnimatorDependencyServer::nAnimatorDependencyServer() :
    counter(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimatorDependencyServer::~nAnimatorDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimatorDependencyServer::Init()
{
    nDependencyServer::Init();
    this->counter = 0; // TODO restart for every saved object
}

//------------------------------------------------------------------------------
/**
*/
nObject *
nAnimatorDependencyServer::GetObject(const nString& filename, const nString& command)
{
    nString parentName(filename.ExtractDirName());  // "path_to_object/object/anims/"
    parentName.StripTrailingSlash();                // "path_to_object/object/anims"
    parentName = parentName.ExtractToLastSlash();   // "path_to_object/object/"
    parentName.StripTrailingSlash();                // "path_to_object/object"
    parentName = parentName.ExtractFileName();      // "object"

    nString objectName(filename.ExtractFileName());
    objectName.StripExtension();

    nString objectPath("/usr/deps/");               // "/usr/deps/"
    objectPath.Append(command);                     // "/usr/deps/addanimator"
    objectPath.Append("/");                         // "/usr/deps/addanimator/"
    objectPath.Append(parentName.ExtractFileName());// "/usr/deps/addanimator/parent"
    objectPath.Append("/");                         // "/usr/deps/addanimator/parent/"
    objectPath.Append(objectName);                  // "/usr/deps/addanimator/parent/object"

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
nAnimatorDependencyServer::GetObjectPath(nObject *const object, const nString& /*command*/, nString& path)
{
    const nRoot *animator = (const nRoot *) object;

    path.Clear();
    path.Append(nDependencyServer::parentPath); // "path_to_object/object/scene/"
    path = path.ExtractDirName();               // "path_to_object/object/"
    path.Append("anims/");                      // "path_to_object/object/anims/"
    path.Append(animator->GetName());           // "path_to_object/object/anims/animatorname"
    path.Append(".n2");                         // "path_to_object/object/anims/animatorname.n2"

    return true;
}
