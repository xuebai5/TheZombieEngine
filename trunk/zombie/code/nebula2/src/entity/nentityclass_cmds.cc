//------------------------------------------------------------------------------
/**
    @file nentityclass_cmds.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nentityclass.h"
#include "kernel/nclass.h"

//------------------------------------------------------------------------------
bool 
nEntityClass::SaveCmds(nPersistServer * ps)
{
    bool ret = true;

    ret &= nRoot::SaveCmds(ps);
    int i(0);
    while (ret && i < this->comps.Size())
    {
        nComponentClass * cc = this->GetComponentByIndex(i++);
        n_assert(cc);
        ret &= cc->SaveCmds(ps);
    }

    return ret;
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN( nEntityClass )

    cl->BeginSignals(4);

    if (!strcmp(cl->GetName(), "nentityclass"))
    {
        NSCRIPT_ADDCMD('BNEC', nEntityClass *, BeginNewObjectEntityClass, 2, (const char *, const char *), 0, ());
        NSCRIPT_ADDCMD('BCCC', bool, CopyCommonComponentsFrom, 1, (nEntityClass *), 0, ());
        NSCRIPT_ADDCMD('BHCO', bool, HasComponent, 1, (const char *), 0, ());
        NSCRIPT_ADDCMD('BGNC', int, GetNumComponents, 0, (), 0, ());
        NSCRIPT_ADDCMD('BGCN', const nComponentId &, GetComponentIdByIndex, 1, (int), 0, ());
    }

    cl->EndSignals();

NSCRIPT_INITCMDS_END()
