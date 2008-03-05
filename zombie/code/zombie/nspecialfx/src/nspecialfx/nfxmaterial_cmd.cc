#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxmaterial_cmd.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxmaterial.h"
#include "nspecialfx/ncspecialfxclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nFxMaterial)
    NSCRIPT_ADDCMD('AAEF', void, BeginFx, 3, (const char *, const char *, int), 0, ());
    NSCRIPT_ADDCMD('AAEG', bool, AddFxClass, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD('AAEH', void, EndFx, 0, (), 0, ());
    NSCRIPT_ADDCMD('AAEK', void, BeginEvent, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD('AAEL', bool, AddEffect, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD('AAEM', void, EndEvent, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
*/
bool nFxMaterial::SaveMaterial(nPersistServer* ps)
{
    nCmd* cmd;

    //special fx lists
    for( int i=0; i< this->specialfx.Size(); i++)
    {
        nFxNode* fxNode = this->GetFxNodeByIndex(i);

        if( fxNode->fxlist.Size() != 0)
        {
            // --- BeginFx ---
            cmd = ps->GetCmd(this, 'AAEF');
            cmd->In()->SetS( fxNode->id.Get() );
            cmd->In()->SetS( ncSpecialFXClass::TypeToString(fxNode->GetType( 0) ) );
            cmd->In()->SetI( fxNode->fxlist.Size() );
            ps->PutCmd(cmd);

            for( int j=0; j< fxNode->fxlist.Size(); j++)
            {
                // --- AddFxClass ---
                cmd = ps->GetCmd(this, 'AAEG');
                cmd->In()->SetI( j );
                cmd->In()->SetS( fxNode->fxlist[j].Get());
                ps->PutCmd(cmd);

            }

            // --- EndFx ---
            cmd = ps->GetCmd(this, 'AAEH');
            ps->PutCmd(cmd);
        }
    }

    this->matEffects.Begin();
    nFxEffects* fxEffect;
    nString eventType;
    this->matEffects.Next(  eventType, fxEffect);

    while ( fxEffect)
    {
        if( fxEffect->effectsIndexList.Size() != 0)
        {
            // --- BeginEvent ---
            cmd = ps->GetCmd(this, 'AAEK');
            cmd->In()->SetS( fxEffect->eventType.Get()  );
            cmd->In()->SetI( fxEffect->effectsIndexList.Size() );
            ps->PutCmd(cmd);

            for( int j=0; j< fxEffect->effectsIndexList.Size(); j++)
            {
                // --- AddEffect ---
                cmd = ps->GetCmd(this, 'AAEL');
                cmd->In()->SetI( j );
                cmd->In()->SetS( specialfx[ fxEffect->effectsIndexList[j] ].id.Get() );
                ps->PutCmd(cmd);
            }

            // --- EndEvent ---
            cmd = ps->GetCmd(this, 'AAEM');
            ps->PutCmd(cmd);
        }
        this->matEffects.Next(  eventType, fxEffect);
    }
    return true;
}


#endif
