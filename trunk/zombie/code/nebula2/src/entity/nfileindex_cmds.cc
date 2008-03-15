//------------------------------------------------------------------------------
//  nfileindex_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nfileindex.h"

//------------------------------------------------------------------------------
/**
*/

NSCRIPT_INITCMDS_BEGIN( nFileIndex )
    NSCRIPT_ADDCMD('CGSL', void, GetStartAndLength, 1, (int), 2, (int&, int&));
    NSCRIPT_ADDCMD('CSSL', void, SetStartAndLength, 3, (int,int, int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::SaveCmds(nPersistServer * ps)
{
    if (nObject::SaveCmds(ps))
    {
        return true;
    }
    return false;
}