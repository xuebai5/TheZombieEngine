//------------------------------------------------------------------------------
//  nfxeventtrigger.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/nfxeventtrigger.h"

//------------------------------------------------------------------------------
#include "gameplay/ngamematerialserver.h"
#include "gameplay/ngamematerial.h"

//------------------------------------------------------------------------------
#include "nspecialfx/nfxmaterial.h"
//------------------------------------------------------------------------------
#include "nscene/ncscene.h"
#include "nscene/ntransformnode.h"
//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nFxEventTrigger)
    NSCRIPT_ADDCMD('AAEQ', void, BeginGroup, 2, ( nString&, int), 0, ());
    NSCRIPT_ADDCMD('AAER', bool, AddFxSpecialClass, 2, (nString&, nString&), 0, ());
    NSCRIPT_ADDCMD('AAFG', bool, AddFxClassToGroup, 2, (int, nString&), 0, ());
    NSCRIPT_ADDCMD('AAES', void, EndGroup, 0, (), 0, ());
    NSCRIPT_ADDCMD('AAFC', void, Splash, 1, (nString), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nFxEventTrigger::SaveSpecialFx(nPersistServer* ps)
{
    if( this->fxMatIndependent.Size() != 0)
    {
        nString FileFxMaterialsPath("wc:libs/fx/");

        nFileServer2 *fileServer(nFileServer2::Instance());

        if(!fileServer->DirectoryExists(FileFxMaterialsPath.Get()))
        {
            fileServer->MakePath(FileFxMaterialsPath.Get());
        }

        nCmd * cmd = ps->GetCmd(this, 'THIS');

        nString filename( FileFxMaterialsPath );
        filename += nString("fx_general.n2");

        if( ps->BeginObjectWithCmd(this, cmd, filename.Get() ) )
        {
            //special fx lists
            for( int i=0; i< this->fxMatIndependent.Size(); i++)
            {
                nFxSpecial* fxNode = &this->fxMatIndependent[i];

                if( fxNode->fxlist.Size() > 1)
                {
                    // --- BeginGroup ---
                    cmd = ps->GetCmd(this, 'AAEQ');
                    cmd->In()->SetS( nFxEventTrigger::TypeToString(fxNode->fxsubtype) );
                    cmd->In()->SetI( fxNode->fxlist.Size());
                    ps->PutCmd(cmd);

                    for( int j=0; j< fxNode->fxlist.Size(); j++)
                    {
                        // --- AddFxClassToGroup ---
                        cmd = ps->GetCmd(this, 'AAFG');
                        cmd->In()->SetI( j );
                        cmd->In()->SetS( fxNode->fxlist[j].fxclass.Get() );
                        ps->PutCmd(cmd);
                    }

                    // --- EndGroup ---
                    cmd = ps->GetCmd(this, 'AAES');
                    ps->PutCmd(cmd);
                }
                else //has only one element
                {   
                    // --- AddFxSpecialClass ---
                    cmd = ps->GetCmd(this, 'AAER');
                    cmd->In()->SetS( fxNode->fxlist[0].fxclass.Get() );
                    cmd->In()->SetS( nFxEventTrigger::TypeToString(fxNode->fxsubtype));
                    ps->PutCmd(cmd);
                }
            }
        }

        return true;
    }
    return false;
}


//------------------------------------------------------------------------------

