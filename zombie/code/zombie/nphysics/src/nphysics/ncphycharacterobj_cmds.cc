//-----------------------------------------------------------------------------
//  ncphycharacterobj_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphycharacterobj.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyCharacterObj)
    NSCRIPT_ADDCMD_COMPOBJECT('AACK', void, SetGeometryOffset, 2, (int, vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACL', void, GetGeometryOffset, 2, (int, vector3&), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
*/
bool
ncPhyCharacterObj::SaveCmds(nPersistServer *ps)
{
    if (ncPhyCompositeObj::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Object chunk persistency.
*/
bool 
ncPhyCharacterObj::SaveChunk(nPersistServer *ps)
{

    if( ncPhyCompositeObj::SaveChunk(ps) )
    {

#ifdef __ZOMBIE_EXPORTER__
        nCmd* cmd(0);

        /// storing mass and density
        for( int index=0; index< this->geomOffset.Size(); index++)
        {
            cmd = ps->GetCmd( this->entityObject, 'AACK');

            n_assert2( cmd, "Error command not found" );

            int jointIndex = this->geomOffset.GetKeyAt(index);
            vector3 offset = this->geomOffset.GetElementAt(index);

            cmd->In()->SetI( jointIndex );
            cmd->In()->SetF( offset.x );
            cmd->In()->SetF( offset.y );
            cmd->In()->SetF( offset.z );

            ps->PutCmd(cmd);
        }
#endif 

        return true;
    }

    return true;
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
