#ifndef N_COMPONENTA_H
#define N_COMPONENTA_H

// component A
#include "entity/nentity.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"

NCOMPONENT_OBJECT_BEGIN(nComponentObjectA)

public:

    NSIGNAL_DECLARE('APRI', void, printeda, 0, (), 0, ())

    NCMDPROTONATIVECPP_DECLARE('PRIA',virtual,void,printa,0,(),0,())
    {
        n_printf("nComponentObjectA\n");
        this->Signalprinteda(this->GetEntityObject());
    }

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(printa)
        NCOMPONENT_ADDSIGNAL(printeda)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_OBJECT_END(nComponentObjectA)

NCOMPONENT_CLASS_BEGIN(nComponentClassA)

public:
    void print()
    {
        n_printf("nComponentClassA\n");
    }

    NSIGNAL_DECLARE('APRI', void, printeda, 0, (), 0, ());

    NCMDPROTONATIVECPP_DECLARE('PRIA',virtual,void,printa,0,(),0,())
    {
        n_printf("nComponentClassA\n");
    }

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(printa)
        NCOMPONENT_ADDSIGNAL(printeda)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_CLASS_END(nComponentClassA)

#endif