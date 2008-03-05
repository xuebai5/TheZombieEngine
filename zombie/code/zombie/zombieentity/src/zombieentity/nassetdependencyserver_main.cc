#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nassetdependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nassetdependencyserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nAssetDependencyServer, "ndependencyserver");

//------------------------------------------------------------------------------
/**
*/
nAssetDependencyServer::nAssetDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAssetDependencyServer::~nAssetDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nAssetDependencyServer::GetObjectPath(nObject *const /*object*/, const nString& /*command*/, nString& path)
{
    if (!nDependencyServer::parentObject)
    {
        return false;
    }

    const nEntityClass* entityClass = (const nEntityClass*) nDependencyServer::parentObject;
    const ncAssetClass* assetClass = entityClass->GetComponent<ncAssetClass>();

    //get resource file from class if not provided as parameter
    if (path.IsEmpty())
    {
        if (assetClass && assetClass->GetResourceFile())
        {
            path.Set(assetClass->GetResourceFile()); // "path_to_asset"
        }
        else
        {
            return false;
        }
    }

    //add /scene/(objectname) to the path
    const char* ext = path.GetExtension();
    if (!ext || strcmp(ext, "n2"))  // extension != "n2"
    {
        nString assetName(path);
        assetName.StripTrailingSlash();
        assetName = assetName.ExtractFileName();

        path.Append("/scene/");         // "path_to_asset/scene/"
        path.Append(assetName.Get());   // "path_to_asset/scene/(assetName)"
        path.Append(".n2");             // "path_to_asset/scene/(assetName).n2"
    }
    return true;
}
