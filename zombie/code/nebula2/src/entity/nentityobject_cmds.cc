//------------------------------------------------------------------------------
/**
    @file nentityobject_main.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nentityobject.h"
#include "kernel/nclass.h"

//------------------------------------------------------------------------------
bool 
nEntityObject::SaveCmds(nPersistServer * ps)
{
    bool ret = true;

    ret &= nObject::SaveCmds(ps);
    int i(0);
    while (ret && i < this->comps.Size())
    {
        nComponentObject * obj = this->GetComponentByIndex(i++);
        n_assert(obj);
        ret &= obj->SaveCmds(ps);
    }

    return ret;
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN( nEntityObject )

    cl->BeginSignals(4);

    // This is inherited by subclasses, so cmds must only be added for nEntityObject
    if (!strcmp(cl->GetName(), "nentityobject"))
    {
        NSCRIPT_ADDCMD('BGEC', nEntityClass *, GetEntityClass, 0, (), 0, ());
        NSCRIPT_ADDCMD('BSID', void, SetId, 1, (nEntityObjectId), 0, ());
        NSCRIPT_ADDCMD('BGID', nEntityObjectId, GetId, 0, (), 0, ());
        NSCRIPT_ADDCMD('BCCC', bool, CopyCommonComponentsFrom, 1, (nEntityObject *), 0, ());
        NSCRIPT_ADDCMD('BHCO', bool, HasComponent, 1, (const char *), 0, ());
        NSCRIPT_ADDCMD('BGNC', int, GetNumComponents, 0, (), 0, ());
        NSCRIPT_ADDCMD('BGCN', const nComponentId &, GetComponentIdByIndex, 1, (int), 0, ());
#ifndef NGAME
        NSCRIPT_ADDCMD('FIIL', bool, IsInLimbo, 0, (), 0, ());
        NSCRIPT_ADDCMD('FSTL', void, SendToLimbo, 1, (nEntityObject* ), 0, ());
        NSCRIPT_ADDCMD('FRFL', void, ReturnFromLimbo, 1, (nEntityObject* ), 0, ());
#endif

#ifndef NGAME
        N_INITCMDS_ADDSIGNAL( EnterLimbo );
        N_INITCMDS_ADDSIGNAL( ExitLimbo );
#endif
    }

    cl->EndSignals();

NSCRIPT_INITCMDS_END()
