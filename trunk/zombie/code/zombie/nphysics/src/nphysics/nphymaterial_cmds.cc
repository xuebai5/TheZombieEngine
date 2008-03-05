//-----------------------------------------------------------------------------
//  nphymaterial_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphymaterial.h"
#include "kernel/npersistserver.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nphymaterial

    @cppclass
    nPhyMaterial

    @superclass
    nObject

    @classinfo
    Represents a physics material.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyMaterial)
    NSCRIPT_ADDCMD('DSMN', void, SetMaterialName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DGMN', const nString&, GetMaterialName, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSNF', void, SetCoeficientNormalFriction, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGNF', phyreal, GetCoeficientNormalFriction, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSCB', void, SetCoeficientBounce, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGCB', phyreal, GetCoeficientBounce, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSSE', void, SetCoeficientSoftnessERP, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGSE', phyreal, GetCoeficientSoftnessERP, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSSC', void, SetCoeficientSoftnessCFM, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGSC', phyreal, GetCoeficientSoftnessCFM, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSLI', void, SetCoeficientSlip, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGLI', phyreal, GetCoeficientSlip, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSPF', void, SetCoeficientPyramidFriction, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGPF', phyreal, GetCoeficientPyramidFriction, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSMB', void, SetMinVelocityBounce, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGMB', phyreal, GetMinVelocityBounce, 0, (), 0, ());
    NSCRIPT_ADDCMD('DEDF', void, EnableFrictionDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD('DDFM', void, DisableFrictionDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD('DUPT', void, Update, 0, (), 0, ());
    NSCRIPT_ADDCMD('DNFI', void, SetCoeficientNormalFrictionInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DBOI', void, SetCoeficientBounceInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DERI', void, SetCoeficientSoftnessERPInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DCFI', void, SetCoeficientSoftnessCFMInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DSII', void, SetCoeficientSlipInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DPFI', void, SetCoeficientPyramidFrictionInf, 0, (), 0, ());
    NSCRIPT_ADDCMD('DBVI', void, SetMinVelocityBounceInf, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyMaterial::SaveCmds(nPersistServer* ps)
{
    if( !nObject::SaveCmds(ps) )
    {
        return false;
    }

    nCmd* cmd(0);

    if( IsType(NormalFriction) )
    {
        phyreal coeficient(this->GetCoeficientNormalFriction());

        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DNFI');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSNF');
            n_assert2( cmd, "Error command not found" );
            cmd->In()->SetF( coeficient );
        }

        ps->PutCmd(cmd);        
    }

    if( IsType(Bounce) )
    {
        phyreal coeficient(this->GetCoeficientBounce() );

        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DBOI');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSCB');
            n_assert2( cmd, "Error command not found" );
            cmd->In()->SetF( coeficient );
        }

        ps->PutCmd(cmd);
    }

    if( IsType(SoftnessERP) )
    {
        phyreal coeficient(this->GetCoeficientSoftnessERP() );

    
        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DERI');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSSE');

            n_assert2( cmd, "Error command not found" );

            cmd->In()->SetF( coeficient );
        }

        ps->PutCmd(cmd);
    }

    if( IsType(SoftnessCFM) )
    {
        phyreal coeficient(this->GetCoeficientSoftnessCFM() );

        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DCFI');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSSC');

            n_assert2( cmd, "Error command not found" );

            cmd->In()->SetF( coeficient );
        }
        ps->PutCmd(cmd);
    }

    if( IsType(Slip) )
    {
        phyreal coeficient(this->GetCoeficientSlip() );
        
        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DSII');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSLI');

            n_assert2( cmd, "Error command not found" );

            cmd->In()->SetF( coeficient );
        }

        ps->PutCmd(cmd);
    }

    if( IsType(PyramidFriction) )
    {
        phyreal coeficient(this->GetCoeficientPyramidFriction() );
        
        if( coeficient == phyInfinity )
        {
            cmd = ps->GetCmd( this, 'DPFI');
            n_assert2( cmd, "Error command not found" );
        }
        else
        {
            cmd = ps->GetCmd( this, 'DSPF');

            n_assert2( cmd, "Error command not found" );

            cmd->In()->SetF( coeficient );
        }

        ps->PutCmd(cmd);
    }

    cmd = ps->GetCmd( this, 'DSMB');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMinVelocityBounce() );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DSMN');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetS( this->nameMaterial.Get() );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DUPT');

    n_assert2( cmd, "Error command not found" );

    ps->PutCmd(cmd);

#ifndef NGAME
    // it's not longer dirty
    this->SetObjectDirty(false);
#endif

    return true;
}

//------------------------------------------------------------------------------
/**
    Objects persistency.
*/
const bool nPhyMaterial::PersistMaterials( nPersistServer* N_IFNDEF_NGAME(server) )
{
    // persists physics materials information
#ifndef NGAME
    nString gameMaterialsPath("wc:libs/materials/physics/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    if(!fileServer->DirectoryExists(gameMaterialsPath.Get()))
    {
        // if the waypoints directory doesn't exist yet, create it
        fileServer->MakePath(gameMaterialsPath.Get());
    }

    // starting at position one, the 0 is the default material
    for( int index(0); index < listMaterials.Size(); ++index )
    {
        nPhyMaterial* material(listMaterials.GetElementAt( index ));

        n_assert2( material, "Data corruption." );

        if( !material->IsObjectDirty() )
        {
            continue;
        }

        nString filename( gameMaterialsPath );

        filename += nString(material->GetName()) + ".n2";

        if( material->GetToBeRemoved() )
        {
            if( fileServer->FileExists( filename ) )
            {
                // deleting the material file
                fileServer->DeleteFile( filename );
            }            
            continue;
        }

        nCmd * cmd = server->GetCmd(material, 'THIS');
        
        if( server->BeginObjectWithCmd(material, cmd, filename.Get() ) )
        {
            if( material->SaveCmds( server ) == false )
            {
                server->EndObject(true);
                return false;
            }

            server->EndObject(true);
        }
    }
#endif    
    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
