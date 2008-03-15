#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  nterrainmaterial_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngeomipmap/nterrainmaterial.h"
#include "kernel/npersistserver.h"
#include "gameplay/ngamematerial.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nTerrainMaterial )
    NSCRIPT_ADDCMD('BLRE', bool, LoadResources, 0, (), 0, ());
    NSCRIPT_ADDCMD('STFN', void, SetTextureFileName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GTFN', const nString &, GetTextureFileName , 0, (), 0, ());
    NSCRIPT_ADDCMD('SPRJ', void, SetProjection, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GPRJ', int, GetProjection , 0, (), 0, ());
    NSCRIPT_ADDCMD('SUVS', void, SetUVScale, 1, (vector2), 0, ());
    NSCRIPT_ADDCMD('GUVS', vector2, GetUVScale , 0, (), 0, ());
    NSCRIPT_ADDCMD('GLHN', nTerrainMaterial::LayerHandle, GetLayerHandle, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLHN', void, SetLayerHandle, 1, (int), 0, ());
    NSCRIPT_ADDCMD('BSGM', void, SetGameMaterialByName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('BGMN', const char * , GetGameMaterialName, 0, (), 0, ());
    NSCRIPT_ADDCMD('BGGM', nGameMaterial *, GetGameMaterial, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTTP', nString, GetTextureThumbnail, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SCOL', void, SetMaskColor, 1, (vector4), 0, ());
    NSCRIPT_ADDCMD('GCOL', vector4, GetMaskColor , 0, (), 0, ());
    NSCRIPT_ADDCMD('SLAB', void, SetLabel, 1, (nString), 0, ());
    NSCRIPT_ADDCMD('GLAB', nString, GetLabel , 0, (), 0, ());
    NSCRIPT_ADDCMD('SLAP', void, SetLayerPath, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('GLAP', const nString &, GetLayerPath, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLUP', void, SetLayerUsePath, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('GLUP', const nString &, GetLayerUsePath, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nTerrainMaterial::SaveCmds(nPersistServer * ps)
{
    if (nObject::SaveCmds(ps))
    {
        nCmd * cmd;

        // --- SetLayerHandle
        cmd = ps->GetCmd(this, 'SLHN');
        cmd->In()->SetI( this->GetLayerHandle() );
        ps->PutCmd( cmd );

        // --- SetTextureFileName
        cmd = ps->GetCmd(this, 'STFN');
        cmd->In()->SetS( this->GetTextureFileName().Get() );
        ps->PutCmd( cmd );

        // --- SetProjection
        cmd = ps->GetCmd(this, 'SPRJ');
        cmd->In()->SetI( this->GetProjection() );
        ps->PutCmd( cmd );

        // --- SetUVScale
        cmd = ps->GetCmd(this, 'SUVS');
        cmd->In()->SetF( this->GetUVScale().x );
        cmd->In()->SetF( this->GetUVScale().y );
        ps->PutCmd( cmd );

        // --- SetMaskColor
        cmd = ps->GetCmd(this, 'SCOL');
        cmd->In()->SetF( this->GetMaskColor().x );
        cmd->In()->SetF( this->GetMaskColor().y );
        cmd->In()->SetF( this->GetMaskColor().z );
        cmd->In()->SetF( this->GetMaskColor().w );
        ps->PutCmd( cmd );

        // --- SetLabel
        cmd = ps->GetCmd(this, 'SLAB');
        cmd->In()->SetS( this->GetLabel().Get() );
        ps->PutCmd( cmd );

        /// Set game material name
        if (!this->gameMaterialName.IsEmpty())
        {
            ps->Put(this, 'BSGM', this->gameMaterialName.Get());
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
