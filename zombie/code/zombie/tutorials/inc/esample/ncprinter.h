#ifndef NC_PRINTER_H
#define NC_PRINTER_H
//------------------------------------------------------------------------------
/**
    @class ncPrinter
    @ingroup Nebula2TutorialsEntities
    @brief Printer component object
    @author Marc Ordinas i Llopis
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"

NCOMPONENT_OBJECT_BEGIN(ncPrinter)

public:

    NCMDPROTONATIVECPP_DECLARE('IPRT',virtual,void,instanceprint,0,(),0,())
    {
        n_printf("Instance: This is an entity with a printer component!\n");
    }

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(instanceprint)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_OBJECT_END(ncPrinter)

//------------------------------------------------------------------------------
/**
    @class ncPrinterClass
    @ingroup Nebula2TutorialsEntities
    @brief Printer component class
*/
NCOMPONENT_CLASS_BEGIN(ncPrinterClass)

public:

    NCMDPROTONATIVECPP_DECLARE('CPRT',virtual,void,classprint,0,(),0,())
    {
        n_printf("Class: This is an entity class with a printer class component!\n");
    }

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(classprint)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_CLASS_END(ncPrinterClass)

#endif
