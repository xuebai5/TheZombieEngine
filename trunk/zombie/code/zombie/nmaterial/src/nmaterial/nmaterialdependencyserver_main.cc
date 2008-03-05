#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialdependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialdependencyserver.h"
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nmaterial.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nMaterialDependencyServer, "ndependencyserver");

//------------------------------------------------------------------------------
/**
*/
nMaterialDependencyServer::nMaterialDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialDependencyServer::~nMaterialDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nObject *
nMaterialDependencyServer::GetObject(const nString& filename, const nString& /*command*/)
{
    nMaterial *object = nMaterialServer::Instance()->LoadMaterialFromFile(filename.Get());
    return object;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialDependencyServer::GetObjectPath(nObject *const object, const nString& /*command*/, nString& path)
{
    n_assert(object->IsA("nmaterial"));
    if (nMaterialServer::Instance()->FindFileFromMaterial(static_cast<nMaterial*>(object), path))
    {
        return true;
    }

    // persist newly created materials in default path
    nRoot* material = (nRoot*) object;
    path.Clear();
    path.Append(nDependencyServer::parentPath); // "path_to_object/object/materials/"
    path.Append(material->GetName());           // "path_to_object/object/materials/materialname"
    path.Append(".n2");                         // "path_to_object/object/materials/materialname.n2"

    return true;
}
