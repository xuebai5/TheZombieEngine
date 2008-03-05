//-----------------------------------------------------------------------------
//  ncphyvehicle_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyvehicle.h"
#include "nphysics/ncphyvehicleclass.h"
#include "nphysics/ncphywheel.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyVehicle)
    NSCRIPT_ADDCMD_COMPOBJECT('DNFL', void, TryUnflip, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DASS', void, Assemble, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSPI', void, SetPieceImpact, 2, (int,phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSLC', void, SetLoadOrClone, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('DGNG', int, GetNumberPieces, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DMGO', void, MarkPiece, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DUGO', void, UnMarkPiece, 1, (int), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Save state of the component.

    history:
        - 06-May-2005   David Reyes    created
*/
bool ncPhyVehicle::SaveCmds(nPersistServer *server)
{    
    if( server->GetSaveType() != nPersistServer::SAVETYPE_PERSIST )
    {
        return true;
    }

    nCmd* cmd(0);

    cmd = server->GetCmd( this->entityObject, 'DSLC' );

    n_assert2( cmd, "Error command not found" );

    server->PutCmd(cmd);        

    if( !ncPhyCompositeObj::SaveCmds( server ) )
    {
        return false;
    }

    for( int index(0); index < this->vehicleParts.Size(); ++index )
    {
        cmd = server->GetCmd( this->entityObject, 'DSPI' );

        n_assert2( cmd, "Error command not found" );

        int iindex(-1);

        for( int inner(0); inner < this->GetNumGeometries(); ++inner )
        {
            if( static_cast<nPhyGeomTrans*>(this->GetGeometry( index ))->GetGeometry() == this->vehicleParts[ index ].where )
            {
                iindex = inner;
                break;
            }
        }

        n_assert2( iindex != -1, "Data corruption." );

        cmd->In()->SetI( iindex );
        cmd->In()->SetF( this->vehicleParts[ index ].tresholdForce );

        server->PutCmd(cmd);        
    }

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
