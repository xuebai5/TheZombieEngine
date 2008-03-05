//-----------------------------------------------------------------------------
//  ngamematerial_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchgameplay.h"

#include "gameplay/ngamematerial.h"
#include "gameplay/ngamematerialserver.h"

#include "nphysics/nphymaterial.h"

#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngamematerialserver

    @cppclass
    nGameMaterialServer

    @superclass
    nRoot

    @classinfo
    Represents a game material container and manager.
*/

NSCRIPT_INITCMDS_BEGIN(nGameMaterialServer);
    NSCRIPT_ADDCMD('DGMT', nGameMaterial*, GetMaterialByName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DCMT', nGameMaterial*, CreateMaterial, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DAMT', void, Add, 1, (nGameMaterial*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('DUMT', void, Update, 0, (), 0, ());
    NSCRIPT_ADDCMD('DREM', void, Remove, 1, (nGameMaterial*), 0, ());
#endif
NSCRIPT_INITCMDS_END();

/// object persistency

#ifdef NGAME
bool nGameMaterialServer::SaveCmds(nPersistServer* /*server*/)
{
#else
bool nGameMaterialServer::SaveCmds(nPersistServer* server)
{
    // persist physics materials
    nPhyMaterial::PersistMaterials(server);

    nString gameMaterialsPath("wc:libs/materials/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    if(!fileServer->DirectoryExists(gameMaterialsPath.Get()))
    {
        // if the waypoints directory doesn't exist yet, create it
        fileServer->MakePath(gameMaterialsPath.Get());
    }

    for( int index(0); index < this->listMaterials.Size(); ++index )
    {
        nGameMaterial* material(this->listMaterials.GetElementAt( index ));

        n_assert2( material, "Data corruption." );

        if( !material->IsDirty() )
        {
            continue;
        }

        if( material->GetToBeRemoved() )
        {
            this->RemoveMaterial( material );
            continue;
        }

        nString filename( gameMaterialsPath );

        filename += nString(material->GetName()) + ".n2";

        nCmd * cmd = server->GetCmd(material, 'THIS');
        
        if( server->BeginObjectWithCmd(material, cmd, filename.Get() ) )
        {
            material->SaveCmds( server );

            server->EndObject(true);
        }
    }
#endif

    return true;
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
bool nGameMaterialServer::Save()
{
    return this->SaveCmds( nKernelServer::Instance()->GetPersistServer() );
}
#endif // NGAME

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
