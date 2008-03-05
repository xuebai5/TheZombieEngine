#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncbatchloader_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncbatchloader.h"
#include "nscene/ncscenebatch.h"
#include "nscene/nscenenode.h"
/** ZOMBIE REMOVE
#include "nasset/nasset.h"*/
#ifndef NGAME
#include "kernel/ndependencyserver.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncBatchLoader, ncLoader);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncBatchLoader)
    NSCRIPT_ADDCMD_COMPOBJECT('MSBR', void, SetBatchResource, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGBR', const char *, GetBatchResource, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncBatchLoader::ncBatchLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncBatchLoader::~ncBatchLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncBatchLoader::LoadComponents()
{
    // load graphic batch component
    ncSceneBatch *batchComp = this->entityObject->GetComponent<ncSceneBatch>();
    if (batchComp && this->GetBatchResource())
    {
        nAutoRef<nSceneNode> refSceneLib("/lib/scene");//TEMP!
        nSceneNode* rootNode = (nSceneNode*) ncLoaderClass::LoadResource(refSceneLib, this->GetBatchResource());
        if (rootNode)
        {
            batchComp->SetBatchRoot(rootNode);
        }
    }

    return ncLoader::LoadComponents();
}

//------------------------------------------------------------------------------
/**
*/
void
ncBatchLoader::UnloadComponents()
{
    ncLoader::UnloadComponents();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncBatchLoader::SaveCmds(nPersistServer* ps)
{
    if (ncLoader::SaveCmds(ps))
    {
        //DO NOT persist batched scene resource for this entity
        //the builder for static batches will be the one doing it
        nCmd* cmd;

        #ifndef NGAME
        // save batch resource to file
        nDependencyServer *depServer = ps->GetDependencyServer("ncloader", "setbatchresource");
        if (depServer && ps->GetSaveMode() == nPersistServer::SAVEMODE_FOLD)
        {
            ncSceneBatch* batchComp = this->GetComponent<ncSceneBatch>();
            if (batchComp && batchComp->GetRootNode())
            {
                nString batchPath;
                depServer->SaveObject(batchComp->GetRootNode(), "setbatchresource", batchPath);
                this->SetBatchResource(batchPath.Get());
            }
        }
        #endif

        if (this->GetBatchResource())
        {
            //--- setbatchresource ---
            cmd = ps->GetCmd(this->GetEntityObject(), 'MSBR');
            cmd->In()->SetS(this->GetBatchResource());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
