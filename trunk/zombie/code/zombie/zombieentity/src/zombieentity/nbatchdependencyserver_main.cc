#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nbatchdependencyserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nbatchdependencyserver.h"
#include "kernel/nkernelserver.h"
#include "nscene/ncscenebatch.h"
#include "entity/nentity.h"
#include "entity/nentityobjectserver.h"

nNebulaClass(nBatchDependencyServer, "ndependencyserver");

//------------------------------------------------------------------------------
/**
*/
nBatchDependencyServer::nBatchDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBatchDependencyServer::~nBatchDependencyServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nBatchDependencyServer::GetObjectPath(nObject *const /*object*/, const nString& /*command*/, nString& path)
{
    if (!nDependencyServer::parentObject)
    {
        return false;
    }

    n_assert(nDependencyServer::parentObject->IsA("nentityobject"));
    nEntityObject* entityObject = (nEntityObject*) nDependencyServer::parentObject;
    ncSceneBatch* batchComp = entityObject->GetComponent<ncSceneBatch>();

    //TODO- move batch resource to loader component
    if (batchComp && batchComp->GetRootNode())
    {
        nEntityObjectId id(entityObject->GetId());
        //filename uses full entity id, to ensure the name of the node when loaded
        path.Format("level:batches/%08X/%08X.n2", id & nEntityObjectServer::IDHIGHMASK, id);
        return true;
    }

    return false;
}
