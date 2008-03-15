//------------------------------------------------------------------------------
//  nobjectinstancer_cmds.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nobjectinstancer.h"

//------------------------------------------------------------------------------
bool 
nObjectInstancer::SaveCmds(nPersistServer* ps)
{
    ps->Put(this, 'SEOT', this->entityObjectType);

    return nObjectArray::SaveCmds(ps);
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nObjectInstancer)
    NSCRIPT_ADDCMD('SEOT', void, SetEntityObjectType, 1, (nEntityObjectServer::nEntityObjectType), 0, ());
NSCRIPT_INITCMDS_END()
