#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncrnswaypoint_cmds.cc
//------------------------------------------------------------------------------
#include "ncrnswaypoint/ncrnswaypoint.h"
#include "entity/nentityobjectserver.h"
#include "entity/nobjectinstancer.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncRnsWaypoint,ncNavNode);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncRnsWaypoint)
        NSCRIPT_ADDCMD_COMPOBJECT('EALI', void, AddLocalLinkById, 1, (nEntityObjectId), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('I_SA', void, SetAttributes, 1, (unsigned int), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('I_GA', unsigned int, GetAttributes, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISWA', void, SetWalkAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIWA', bool, IsWalkAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISCA', void, SetCrouchAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IICA', bool, IsCrouchAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISPA', void, SetProneAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIPA', bool, IsProneAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISRA', void, SetRunAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIRA', bool, IsRunAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISJA', void, SetJumpAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIJA', bool, IsJumpAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISJO', void, SetJumpoverAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIJO', bool, IsJumpoverAttribute, 0, (), 0, ());
/** ZOMBIE REMOVE
        NSCRIPT_ADDCMD_COMPOBJECT('ISHA', void, SetHopAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIHA', bool, IsHopAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISLP', void, SetLeapAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IILP', bool, IsLeapAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISLI', void, SetLeapInAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IILI', bool, IsLeapInAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISLO', void, SetLeapOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IILO', bool, IsLeapOutAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISL2', void, SetLeapInOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISBA', void, SetBurrowAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIBA', bool, IsBurrowAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISBI', void, SetBurrowInAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIBI', bool, IsBurrowInAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISBO', void, SetBurrowOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIBO', bool, IsBurrowOutAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISB2', void, SetBurrowInOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISRL', void, SetRollAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIRL', bool, IsRollAttribute, 0, (), 0, ());*/
        NSCRIPT_ADDCMD_COMPOBJECT('ISCU', void, SetClimbupAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IICU', bool, IsClimbupAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISCD', void, SetClimbdownAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IICD', bool, IsClimbdownAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISSA', void, SetSwimAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IISA', bool, IsSwimAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISSI', void, SetSwimInAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IISI', bool, IsSwimInAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISSO', void, SetSwimOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IISO', bool, IsSwimOutAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISS2', void, SetSwimInOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISFA', void, SetFlyAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIFA', bool, IsFlyAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISFI', void, SetFlyInAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIFI', bool, IsFlyInAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISFO', void, SetFlyOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIFO', bool, IsFlyOutAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIF2', void, SetFlyInOutAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISWL', void, SetWallAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IIWL', bool, IsWallAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISCL', void, SetCoverLowAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IICL', bool, IsCoverLowAttribute, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ISCH', void, SetCoverHighAttribute, 1, (bool), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('IICH', bool, IsCoverHighAttribute, 0, (), 0, ());
#ifndef NGAME
        NSCRIPT_ADDCMD_COMPOBJECT('EGNA', int, GetNumAttributes, 0, (), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('EGAL', const char*, GetAttributeLabel, 1, (int), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('EGAS', bool, GetAttributeState, 1, (int), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('ESAS', void, SetAttributeState, 2, (int, bool), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncRnsWaypoint::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- attributes
        ps->Put (this->entityObject, 'I_SA', this->attributes);

        // -- links
        int links = this->GetLocalLinksNumber();

        for ( int i=0; i<links; i++ )
        {
            ncNavNode* targetNode = this->GetLocalLink(i);
            n_assert( targetNode );
            if ( !targetNode )
            {
                continue;
            }
            nEntityObject* entObj = targetNode->GetEntityObject();
            // Convert object reference id to persist id
            nEntityObjectId id = entObj->GetId();
            id = entObj->ToPersistenceOID( id );

            ps->Put (this->entityObject, 'EALI', id);
        }
    }

    return true;
}