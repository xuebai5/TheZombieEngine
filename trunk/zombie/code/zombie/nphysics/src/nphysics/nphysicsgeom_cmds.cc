//-----------------------------------------------------------------------------
//  nphysicsgeom_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsgeom.h"
#include "kernel/npersistserver.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nphysicsgeom

    @cppclass
    nPhysicsGeom

    @superclass
    nObject

    @classinfo
    An abstract physics geometry.
*/
NSCRIPT_INITCMDS_BEGIN(nPhysicsGeom)
    NSCRIPT_ADDCMD('DSSP', bool, IsSpace, 0, (), 0, ());
    NSCRIPT_ADDCMD('DNBL', void, Enable, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSBL', void, Disable, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSEN', bool, IsEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('DPOS', void, SetPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('DGPO', void, GetPosition, 0, (), 1, (vector3&));
    NSCRIPT_ADDCMD('DROT', void, SetOrientation, 3, (phyreal,phyreal,phyreal), 0, ());
    NSCRIPT_ADDCMD('DGRO', void, GetOrientation, 0, (), 3, (phyreal&,phyreal&,phyreal&));
    NSCRIPT_ADDCMD('DSGM', void, SetGameMaterial, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DSGI', void, SetGameMaterialId, 1, (const uint), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('DSHP', void, DrawShape, 1, (bool), 0, ());
#endif
    NSCRIPT_ADDCMD('DCTB', void, SetCategories, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DCOB', void, SetCollidesWith, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DMAT', void, SetMaterial, 1, (nPhyMaterial*), 0, ());
    NSCRIPT_ADDCMD('DSAT', void, SetAttributes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DRAT', void, RemoveAttributes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DHAT', bool, HasAttributes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DAAT', void, AddAttributes, 1, (int), 0, ());
    NSCRIPT_ADDCMD('DRSC', void, Scale, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD('DGIT', int, GetIdTag, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSIT', void, SetIdTag, 1, (int), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('DGMN', const nString&, GetGameMaterialName, 0, (), 0, ());
#endif
    NSCRIPT_ADDCMD('DGMI', const uint, GetGameMaterialId, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
nPhysicsGeom::SaveCmds(nPersistServer* ps)
{
    if( !nObject::SaveCmds(ps) )
        return false;

    nCmd* cmd(0);

    if( this->IsEnabled() )
        cmd = ps->GetCmd( this, 'DNBL');
    else
        cmd = ps->GetCmd( this, 'DSBL');

    n_assert2( cmd, "Error command not found" );

    /// Setting if the object it's enabled
    ps->PutCmd(cmd);
            
    cmd = ps->GetCmd( this, 'DPOS');

    n_assert2( cmd, "Error command not found" );

    vector3 position;

    this->GetPosition(position);

    /// Setting the objects position
    cmd->In()->SetF( position.x );
    cmd->In()->SetF( position.y );
    cmd->In()->SetF( position.z );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DROT');

    n_assert2( cmd, "Error command not found" );

    vector3 euler;

    this->GetOrientation(euler.x, euler.y, euler.z);

    /// Setting the objects position
    cmd->In()->SetF( euler.x );
    cmd->In()->SetF( euler.y );
    cmd->In()->SetF( euler.z );

    ps->PutCmd(cmd);

    if( this->GetMaterial() != nPhyMaterial::None )
    {
        cmd = ps->GetCmd( this, 'DMAT');

        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetO( nPhyMaterial::GetMaterial( this->GetMaterial() ) );

        ps->PutCmd(cmd);
    }

    /// persisting attributes

    cmd = ps->GetCmd( this, 'DSAT');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetAttributes() );

    ps->PutCmd(cmd);
    
    /// persisting object material
#ifndef NGAME
    cmd = ps->GetCmd( this, 'DSGM');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetS( this->gameMaterial.GetText().Get() );

    ps->PutCmd(cmd);
#else
    cmd = ps->GetCmd( this, 'DSGI');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->gameMaterial.KeyMap() );

    ps->PutCmd(cmd);
#endif

    /// persisting id/tag
    if( this->GetIdTag() != NoValidID )
    {        
        cmd = ps->GetCmd( this, 'DSIT');

        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetI( this->GetIdTag() );

        ps->PutCmd(cmd);
    }

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
