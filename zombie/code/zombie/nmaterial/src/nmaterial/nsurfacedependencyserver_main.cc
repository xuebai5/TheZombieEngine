#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nsurfacedependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nsurfacedependencyserver.h"
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nmaterialnode.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nSurfaceDependencyServer, "nscenesurfacedependencyserver");

//------------------------------------------------------------------------------
/**
*/
nSurfaceDependencyServer::nSurfaceDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceDependencyServer::~nSurfaceDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nSurfaceDependencyServer::GetObject(const nString& filename, const nString& /*command*/)
{
    nSurfaceNode *object = nMaterialServer::Instance()->LoadSurfaceFromFile(filename.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSurfaceDependencyServer::GetObjectPath(nObject *const object, const nString& command, nString& path)
{
    n_assert(object->IsA("nsurfacenode"));
    if (nMaterialServer::Instance()->FindFileFromSurface(static_cast<nSurfaceNode*>(object), path))
    {
        return true;
    }

    return nSceneSurfaceDependencyServer::GetObjectPath(object, command, path);
}
