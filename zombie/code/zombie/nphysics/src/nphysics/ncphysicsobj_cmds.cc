//-----------------------------------------------------------------------------
//  ncphysicsobj_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "nphysics/nphysicsserver.h"
#include "precompiled/pchnphysics.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncPhysicsObj,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhysicsObj)
    NSCRIPT_ADDCMD_COMPOBJECT('DNBL', void, Enable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSBL', void, Disable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSEN', const bool, IsEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DPOM', void, SetMaterial, 1, (const nPhyMaterial*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBSM', void, SetMass, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBGM', phyreal, GetMass, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBSD', void, SetDensity, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBGD', phyreal, GetDensity, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DOSP', void, SetSpace, 1, (nPhySpace*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBJF', void, SetForce, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DTSP', void, MoveToSpace, 1, (nPhySpace*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DFCE', void, AddForce, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DEOR', void, SetOrientation, 3, (phyreal,phyreal,phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTF', void, Resets, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DRSC', void, Scale, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DRSV', void, SetLastScaledFactor, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSCO', void, SetCategories, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGCO', int, GetCategories, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSCW', void, SetCollidesWith, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGCW', int, GetCollidesWith, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSPN', void, SetSpaceByName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DITW', void, InsertInTheWorld, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DPPO', void, SetPositionPhyObj, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTO', void, SetTweakedOffset, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DOPP', void, PreparePhyObject, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
ncPhysicsObj::SaveCmds(nPersistServer* ps)
{

    if( !nComponentObject::SaveCmds(ps) )
        return false;

    if( ps->GetSaveType() != nPersistServer::SAVETYPE_PERSIST )
    {
        return true;
    }

    nCmd* cmd(0);

    cmd = ps->GetCmd( this->entityObject, 'DOPP');

    n_assert2( cmd, "Error command not found" );

    ps->PutCmd(cmd);        

    /// storing mass and density
    cmd = ps->GetCmd( this->entityObject, 'DBSM');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMass() );
    
    ps->PutCmd(cmd);        

    cmd = ps->GetCmd( this->entityObject, 'DBSD');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetDensity() );
    
    ps->PutCmd(cmd);

    ncSubentity * subentity = this->GetComponent<ncSubentity>();
    const bool isSubentity(subentity && subentity->IsSubentity());

    if (!isSubentity) //Subentity get the physic position from ncTransform
    {
        /// exporting object position
        cmd = ps->GetCmd( this->entityObject, 'DPPO');
        
        n_assert2( cmd, "Error command not found" );

        vector3 position;

        this->GetPosition( position );


        if( this->GetBody() )
        {
            if( this->GetBody()->GetTweaked() )
            {
                position -= this->GetBody()->GetTweakedOffset();
            }
        }

        cmd->In()->SetF( position.x );
        cmd->In()->SetF( position.y );
        cmd->In()->SetF( position.z );

        ps->PutCmd(cmd);        

        /// exporting object orientation
        cmd = ps->GetCmd( this->entityObject, 'DEOR');
        
        n_assert2( cmd, "Error command not found" );

        matrix33 orientation;

        this->GetOrientation( orientation );    

        cmd->In()->SetF( orientation.to_euler().x );
        cmd->In()->SetF( orientation.to_euler().y );
        cmd->In()->SetF( orientation.to_euler().z );

        ps->PutCmd(cmd);


#ifndef __ZOMBIE_EXPORTER__
        /// exporting auto-insert (always the last)
        cmd = ps->GetCmd( this->entityObject, 'DITW');
        
        n_assert2( cmd, "Error command not found" );

        ps->PutCmd(cmd);
#endif
    } // !isSubentity

    return true;
}

//------------------------------------------------------------------------------
/**
    Object chunk persistency.
*/
bool ncPhysicsObj::SaveChunk(nPersistServer *ps)
{
    nCmd* cmd(0);

    if( this->material != nPhyMaterial::None )
    {
        // persist the object material
        cmd = ps->GetCmd( this->entityObject, 'DPOM' );

        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetO( nPhyMaterial::GetMaterial( this->material ) );

        ps->PutCmd(cmd);        
    }

    /// storing mass and density
    cmd = ps->GetCmd( this->entityObject, 'DBSM');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMass() );
    
    ps->PutCmd(cmd);        

    cmd = ps->GetCmd( this->entityObject, 'DBSD');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetDensity() );
    
    ps->PutCmd(cmd);        

#ifdef __ZOMBIE_EXPORTER__
    if( !this->GetSpaceName().IsEmpty() )
    {
        /// exporting space by name
        cmd = ps->GetCmd( this->entityObject, 'DSPN');
        
        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetS( this->GetSpaceName().Get() );

        ps->PutCmd(cmd);
    }
#endif 

    if( this->GetLastScaledFactor() != phyreal(1) )
    {
        /// exporting space by name
        cmd = ps->GetCmd( this->entityObject, 'DRSV');
        
        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetF( this->GetLastScaledFactor() );

        ps->PutCmd(cmd);        
    }

    /// exporting object position
    cmd = ps->GetCmd( this->entityObject, 'DPPO');
    
    n_assert2( cmd, "Error command not found" );

    vector3 position;

    this->GetPosition( position );

    if( this->GetBody() )
    {
        if( this->GetBody()->GetTweaked() )
        {
            position -= this->GetBody()->GetTweakedOffset();
        }
    }

    cmd->In()->SetF( position.x );
    cmd->In()->SetF( position.y );
    cmd->In()->SetF( position.z );

    ps->PutCmd(cmd);        

    /// exporting object orientation
    cmd = ps->GetCmd( this->entityObject, 'DEOR');
    
    n_assert2( cmd, "Error command not found" );

    matrix33 orientation;

    this->GetOrientation( orientation );    

    cmd->In()->SetF( orientation.to_euler().x );
    cmd->In()->SetF( orientation.to_euler().y );
    cmd->In()->SetF( orientation.to_euler().z );

    ps->PutCmd(cmd);        

    /// exporting categories
    cmd = ps->GetCmd( this->entityObject, 'DSCO');
    
    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetCategories() );

    ps->PutCmd(cmd);        
    
    /// exporting collideswith
    cmd = ps->GetCmd( this->entityObject, 'DSCW');
    
    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetCollidesWith() );

    ps->PutCmd(cmd);        

#if 0
    if( this->GetBody() )
    {
        if( this->GetBody()->GetTweaked() )
        {
            /// exporting object orientation
            cmd = ps->GetCmd( this->entityObject, 'DSTO');
            
            n_assert2( cmd, "Error command not found" );

            vector3 offset(this->GetBody()->GetTweakedOffset());

            cmd->In()->SetF( offset.x );
            cmd->In()->SetF( offset.y );
            cmd->In()->SetF( offset.z );

            ps->PutCmd(cmd);        
        }
    }
#endif
    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
