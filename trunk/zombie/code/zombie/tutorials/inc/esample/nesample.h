#ifndef NE_SAMPLE_H
#define NE_SAMPLE_H
//------------------------------------------------------------------------------
/**
    @class neSample
    @ingroup Nebula2TutorialsEntities
    @brief Sample entity object
    @author Marc Ordinas i Llopis
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "esample/nccolor.h"
#include "esample/ncprinter.h"
#include "esample/nesampleclass.h"

class neSample : 
    public nEntityObject, 
    public ncColor, 
    public ncPrinter
{
public:

    neSample() :
        nEntityObject()
    {
        // only one is needed since this is virtual public
        this->ncColor::SetEntityObject(this);
        this->ncPrinter::SetEntityObject(this);
    }

    bool SaveCmds(nPersistServer * ps)
    {
        ncColor::SaveCmds(ps);
        ncPrinter::SaveCmds(ps);
        return true;
    }

    // Script commands
    NCMDPROTONATIVECPP_DECLARE_BEGIN(neSample)
    NCMDPROTONATIVECPP_DECLARE('SPRT',virtual,void,sampleprint,0,(),0,())
    {
        // Access all parts!
        // print instance
        n_printf("!!! Printing ncPrint instance component:\n");
        ncPrinter* pr = this->GetComponent(nComponentType<ncPrinter>());
        pr->instanceprint();
        // Print class
        n_printf("!!! Printing ncPrint class component:\n");
        ncPrinterClass* prc = this->GetClassComponent(nComponentClassType<ncPrinterClass>());
        prc->classprint();

        // Color
        n_printf("!!! Printing ncColor part, both class and instance:\n");
        ncColor* clr = this->GetComponent(nComponentType<ncColor>());
        clr->colorprint();
    }
    NCMDPROTONATIVECPP_DECLARE_END(neSample);

    static void InitClass(nClass * cl)
    {
        ncColor::InitClass<neSample>(cl);
        ncPrinter::InitClass<neSample>(cl);
        NCMDPROTONATIVECPP_ADDCMD(sampleprint);
    }

    NEOBJECT_COMPONENT(ncColor);
    NEOBJECT_COMPONENT(ncPrinter);
};

#endif
