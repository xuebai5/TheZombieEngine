#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxserver_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxspawn.h"
#include "nspecialfx/nfxdecal.h"

#include "nspecialfx/nfxmaterial.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    Script interface
*/
NSCRIPT_INITCMDS_BEGIN(nFXServer)

    NSCRIPT_ADDCMD('MNSF', nObject*, NewSpawnEffectObject, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MNDF', nObject*, NewDecalEffectObject, 1, (const char *), 0, ());
    /*NSCRIPT_ADDCMD('MNTF', nObject*, NewTracerEffectObject, 1, (const char *), 0, ());*/
    NSCRIPT_ADDCMD('MPSF', nObject*, PullEffectObject, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('MKSF', void, KillEffectObject, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('MRSF', void, RemoveEffectObject, 1, (nObject*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('AAET', void, AddFxMaterial, 1, ( nString), 0, ());
    NSCRIPT_ADDCMD('AAEU', void, RemoveFxMaterial, 1, ( nString), 0, ());
#endif

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nObject*
nFXServer::NewSpawnEffectObject(const char *className)
{
    return this->NewSpawnEffect(className);
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nFXServer::NewDecalEffectObject(const char *className)
{
    return this->NewDecalEffect(className);
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nFXServer::PullEffectObject(nObject* fxObject)
{
    n_assert(fxObject->IsA("nfxobject"));
    return this->PullEffect(static_cast<nFXObject*>(fxObject)->GetKey());
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::KillEffectObject(nObject* fxObject)
{
    n_assert(fxObject->IsA("nfxobject"));
    this->KillEffect(static_cast<nFXObject*>(fxObject)->GetKey());
}

//------------------------------------------------------------------------------
/**
*/
void
nFXServer::RemoveEffectObject(nObject* fxObject)
{
    n_assert(fxObject->IsA("nfxobject"));
    this->RemoveEffect(static_cast<nFXObject*>(fxObject)->GetKey());
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
bool
nFXServer::SaveFxMaterials(nPersistServer* ps)
{
    nString FileFxMaterialsPath("wc:libs/materials/physics/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    if(!fileServer->DirectoryExists(FileFxMaterialsPath))
    {
        fileServer->MakePath(FileFxMaterialsPath);
    }

    for( int i=0; i< this->fxMaterialsPool.Size(); i++)
    {
        nFxMaterial* fxMat = this->fxMaterialsPool[i];

        nString filename( FileFxMaterialsPath );
        filename += fxMat->GetName();
        filename += ".n2";

        if( fxMat->GetToBeRemoved() )
        {
            if( fileServer->FileExists( filename ) )
            {
                // deleting the material file
                fileServer->DeleteFile( filename );
            }            
            continue;
        }

        nCmd * cmd = ps->GetCmd(fxMat, 'THIS');
        
        if( ps->BeginObjectWithCmd(fxMat, cmd, filename.Get() ) )
        {
            if( fxMat->SaveCmds( ps ) == false )
            {
                ps->EndObject(true);
                return false;
            }

            ps->EndObject(true);
        }
    }

    return true;
}
#endif