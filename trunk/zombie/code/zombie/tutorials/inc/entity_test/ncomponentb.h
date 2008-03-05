#ifndef N_COMPONENTB_H
#define N_COMPONENTB_H

// component B
#include "entity/nentity.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"

NCOMPONENT_OBJECT_BEGIN(nComponentObjectB)

public:
    void print()
    {
        n_printf("nComponentObjectB");
    }

    NCMDPROTONATIVECPP_DECLARE('PRIB',virtual,void,printb,0,(),0,())
    {
        n_printf("nComponentObjectB");
        this->Signalprintedb(this->GetEntityObject());
    }

    NSIGNAL_DECLARE('APRI', void, printedb, 0, (), 0, ());

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(printb)
        NCOMPONENT_ADDSIGNAL(printedb)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_OBJECT_END(nComponentObjectB)

NCOMPONENT_CLASS_BEGIN(nComponentClassB)

public:
    void print()
    {
        n_printf("nComponentClassB");
    }

    NCMDPROTONATIVECPP_DECLARE('PRIB',virtual,void,printb,0,(),0,())
    {
        n_printf("nComponentObjectB");
    }

    NSIGNAL_DECLARE('APRI', void, printedb, 0, (), 0, ());

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(printb)
        NCOMPONENT_ADDSIGNAL(printedb)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_CLASS_END(nComponentClassB)

#endif
 