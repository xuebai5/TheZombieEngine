#include "precompiled/pchgameplay.h"

#include "ncgamecamera/ncgamecamera.h"

#include "napplication/nappviewport.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameCamera)
    NSCRIPT_ADDCMD_COMPOBJECT('DNBL', void, Enable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DDBL', void, Disable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DIND', const bool, IsEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSCT', void, SetCameraType, 1, (const ncGameCamera::type), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGCT', const ncGameCamera::type, GetCameraType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSAC', void, SetAttributes, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGAC', const int, GetAttributes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DAAC', void, AddAttributes, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DRAC', void, RemoveAttributes, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSAP', void, SetAnchorPoint, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGAP', nEntityObject*, GetAnchorPoint, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSCO', void, SetCameraOffset, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGCO', const vector3&, GetCameraOffset, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSHH', void, SetHeight, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGHH', const float, GetHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDD', void, SetDistance, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGDD', const float, GetDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSRR', void, SetRoute, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGRR', const int, GetRoute, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DUCC', void, Update, 1, (const nTime&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSVP', void, SetViewPort, 1, (nAppViewport*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGVP', nAppViewport*, GetViewPort, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DLAT', void, LookAt, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DBLD', void, Build, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSST', void, SetStep, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGST', const float, GetStep, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTX', void, SetTranspositionXType, 1, (const ncGameCamera::transition), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTX', const ncGameCamera::transition, GetTranspositionXType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTY', void, SetTranspositionYType, 1, (const ncGameCamera::transition), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTY', const ncGameCamera::transition, GetTranspositionYType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTZ', void, SetTranspositionZType, 1, (const ncGameCamera::transition), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTZ', const ncGameCamera::transition, GetTranspositionZType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDA', void, SetDampeningPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGDA', const vector3&, GetDampeningPosition, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDO', void, SetDampeningOrientation, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGDO', const float, GetDampeningOrientation, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSMX', void, SetMaxDistance, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGMX', const float, GetMaxDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSMN', void, SetMinDistance, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGMN', const float, GetMinDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSOT', void, SetTranspositionOrientationType, 1, (const ncGameCamera::transition), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGOT', const ncGameCamera::transition, GetTranspositionOrientationType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSLO', void, SetLookAtOffset, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGLO', const float, GetLookAtOffset, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    object persistency
*/
bool ncGameCamera::SaveCmds(nPersistServer *ps)
{
    if( !nComponentObject::SaveCmds(ps) )
    {
        return false;
    }

    nCmd* cmd(0);

    // set camera type
    cmd = ps->GetCmd( this->entityObject, 'DSCT');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( int(this->GetCameraType()) );

    ps->PutCmd(cmd);        

    // set attributes
    cmd = ps->GetCmd( this->entityObject, 'DSAC');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetAttributes() );

    ps->PutCmd(cmd);        

    // set camera offset
    cmd = ps->GetCmd( this->entityObject, 'DSCO');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetCameraOffset().x );
    cmd->In()->SetF( this->GetCameraOffset().y );
    cmd->In()->SetF( this->GetCameraOffset().z );

    ps->PutCmd(cmd);        

    // set step
    cmd = ps->GetCmd( this->entityObject, 'DSTP');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetStep() );

    ps->PutCmd(cmd);        

    // set kind of dampening in X
    cmd = ps->GetCmd( this->entityObject, 'DSTX');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetTranspositionXType() );

    ps->PutCmd(cmd);        

    // set kind of dampening in Y
    cmd = ps->GetCmd( this->entityObject, 'DSTY');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetTranspositionYType() );

    ps->PutCmd(cmd);        

    // set kind of dampening in Z
    cmd = ps->GetCmd( this->entityObject, 'DSTZ');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetTranspositionZType() );

    ps->PutCmd(cmd);        

    // set dampening position
    cmd = ps->GetCmd( this->entityObject, 'DSDA');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetDampeningPosition().x );
    cmd->In()->SetF( this->GetDampeningPosition().y );
    cmd->In()->SetF( this->GetDampeningPosition().z );

    ps->PutCmd(cmd);        

    // sets the dampening for the orientation
    cmd = ps->GetCmd( this->entityObject, 'DSDO');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetDampeningOrientation() );

    ps->PutCmd(cmd);        

    // sets the max distance of the camera
    cmd = ps->GetCmd( this->entityObject, 'DSMX');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMaxDistance() );

    ps->PutCmd(cmd);        

    // sets the min distance of the camera
    cmd = ps->GetCmd( this->entityObject, 'DSMN');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMinDistance() );

    ps->PutCmd(cmd);        

    // sets the orientation transition type
    cmd = ps->GetCmd( this->entityObject, 'DSOT');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetTranspositionOrientationType() );

    ps->PutCmd(cmd);        

    // sets the lookat offset in y axis
    cmd = ps->GetCmd( this->entityObject, 'DSLO');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetLookAtOffset() );

    ps->PutCmd(cmd);        

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
