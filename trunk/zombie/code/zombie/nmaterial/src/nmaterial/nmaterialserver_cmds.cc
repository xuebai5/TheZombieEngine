#include "precompiled/pchnmaterial.h"
//--------------------------------------------------
//  nmaterialserver_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//--------------------------------------------------
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nmaterial.h"
#include "kernel/npersistserver.h"
#include "file/nmemfile.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmaterialserver
    
    @cppclass
    nMaterialServer
    
    @superclass
    nroot
    
    @classinfo
    Central component for locating and building materials.
*/
NSCRIPT_INITCMDS_BEGIN(nMaterialServer)
    NSCRIPT_ADDCMD('SETB', void, SetBuilder, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GETB', const char *, GetBuilder, 0, (), 0, ());
    NSCRIPT_ADDCMD('STDB', void, SetDatabase, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GTDB', const char *, GetDatabase, 0, (), 0, ());
    NSCRIPT_ADDCMD('CRMT', nObject*, CreateMaterial, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('ISLD', bool, IsLibraryDirty, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLIB', void, SaveLibrary, 0, (), 0, ());
    NSCRIPT_ADDCMD('DLIB', void, DeleteLibrary, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSSR', void, SaveSurfaceReport, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MCSR', void, ClearSurfaceReport, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('SDME', void, SetDebugMaterialEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GDME', bool, GetDebugMaterialEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDMF', void, SetDebugMaterialFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GDMF', const char *, GetDebugMaterialFile, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDSE', void, SetDebugSurfaceEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GDSE', bool, GetDebugSurfaceEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDSF', void, SetDebugSurfaceFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GDSF', const char *, GetDebugSurfaceFile, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::IsLibraryDirty()
{
    return this->isLibraryDirty;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialServer::SaveLibrary()
{
    this->SaveMaterialLibrary();
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialServer::DeleteLibrary()
{
    if (this->DeleteMaterialLibrary())
    {
        n_message("The material library has been deleted from disk.\n\n"
                  "This will have effect next time you start the application.");
    }
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nMaterialServer::CreateMaterial(const char *materialName)
{
    n_assert(this->isLoadingLibrary);
    n_assert(materialName);
    nPersistServer* ps = kernelServer->GetPersistServer();

    // create a new material
    nString materialPath;
    materialPath.Format("/lib/materials/%s", materialName);
    nMaterial* obj = (nMaterial*) kernelServer->New("nmaterial", materialPath.Get());

    ps->BeginObjectLoad(obj, nObject::LoadedInstance);

    return obj;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::SaveMaterialLibrary()
{
    //save all materials in current library
    //save svn version
    nAutoRef<nRoot> refLibMaterials("/lib/materials");
    if (refLibMaterials.isvalid() && refLibMaterials->GetHead())
    {
        const char* fileName("wc:libs/shaders/_library.n2");

        nPersistServer *ps = kernelServer->GetPersistServer();
        n_assert(ps);

        nCmd* cmd = ps->GetCmd(this, 'THIS');
        if (ps->BeginObjectWithCmd(this, cmd, fileName))
        {
            this->isSavingLibrary = true;
            
            nPersistServer* ps = kernelServer->GetPersistServer();
            nRoot* curMaterial;
            for (curMaterial = refLibMaterials->GetHead();
                 curMaterial;
                 curMaterial = curMaterial->GetSucc())
            {
                // select materials with parameters only
                if (static_cast<nMaterial*>(curMaterial)->GetNumParams() == 0)
                {
                    continue;
                }
                
                //--- creatematerial ---
                cmd = ps->GetCmd(this, 'CRMT');
                cmd->In()->SetS(curMaterial->GetName());

                if (ps->BeginObjectWithCmd(this, cmd))
                {
                    if (!curMaterial->SaveCmds(ps))
                    {
                        this->isSavingLibrary = false;
                        return false;
                    }
                    ps->EndObject(false);
                }
            }
            ps->EndObject(true);

            this->isSavingLibrary = false;
        }
    }

    // prevent saving library again
    this->isLibraryDirty = false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::LoadMaterialLibrary()
{
    //do not load library if there are materials loaded already
    nAutoRef<nRoot> refLibMaterials("/lib/materials");
    if (refLibMaterials.isvalid() && refLibMaterials->GetHead())
    {
        return false;
    }

    //load the library from the file
    const char* fileName("wc:libs/shaders/_library.n2");
    if (kernelServer->GetFileServer()->FileExists(fileName))
    {
        this->isLoadingLibrary = true;

        kernelServer->PushCwd(this);
        kernelServer->Load(fileName, false);
        kernelServer->PopCwd();

        this->isLoadingLibrary = false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::DeleteMaterialLibrary()
{
    // delete the file holding the current library
    const char* fileName("wc:libs/shaders/_library.n2");
    nFileServer2* fileServer = kernelServer->GetFileServer();
    if (fileServer->FileExists(fileName))
    {
        return fileServer->DeleteFile(fileName);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    dump to specified file all data collected on surfaces.
*/
void
nMaterialServer::SaveSurfaceReport(const char *fileName)
{
    if (this->surfaceReportFile)
    {
        nFile *reportFile = kernelServer->GetFileServer()->NewFileObject();
        
        if (reportFile->Open(fileName, "w"))
        {
            reportFile->AppendFile(this->surfaceReportFile);
            reportFile->Close();
        }
        reportFile->Release();

        this->ClearSurfaceReport();
    }
}

//------------------------------------------------------------------------------
/**
    dump to specified file all data collected on surfaces.
*/
void
nMaterialServer::ClearSurfaceReport()
{
    if (this->surfaceReportFile)
    {
        this->surfaceReportFile->Close();
        n_delete(this->surfaceReportFile);
        this->surfaceReportFile = 0;
    }
}
