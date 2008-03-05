//-----------------------------------------------------------------------------
//  nphypickableobj_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphypickableobj.h"

#ifndef NGAME //This class is only for a conjurer
//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyPickableObj)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------

/// object persistency
bool ncPhyPickableObj::SaveCmds(nPersistServer*)
{
    return true;
}

#else

//class nClassComponentObject * 
//n_init_ncPhyPickableObj(char const *,class nComponentObjectServer *)
//{
//    return 0;
//}

#endif 
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
