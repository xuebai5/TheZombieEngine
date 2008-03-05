#ifndef NE_SAMPLECLASS_H
#define NE_SAMPLECLASS_H
//------------------------------------------------------------------------------
/**
    @class neSampleClass
    @ingroup Nebula2TutorialsEntities
    @brief Sample entity class
    @author Marc Ordinas i Llopis
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "esample/nccolor.h"
#include "esample/ncprinter.h"

class neSampleClass : 
    public nEntityClass, 
    public ncColorClass, 
    public ncPrinterClass
{
public:

    neSampleClass() :
        nEntityClass()
    {
        // only one is needed since this is virtual public
        this->ncColorClass::SetEntityClass(this);
        this->ncPrinterClass::SetEntityClass(this);
    }

    bool SaveCmds(nPersistServer * ps)
    {
        ncColorClass::SaveCmds(ps);
        ncPrinterClass::SaveCmds(ps);
        return true;
    }

    // Script commands
    NCMDPROTONATIVECPP_DECLARE_BEGIN(neSampleClass)
    NCMDPROTONATIVECPP_DECLARE('SCPR',virtual,void,sampleclassprint,0,(),0,())
    {
        // Access the ncPrinterClass part
        n_printf("!!! Printing ncPrintClass component:\n");
        ncPrinterClass* prc = this->GetComponent(nComponentClassType<ncPrinterClass>());
        prc->classprint();

        // Access the ncColorClass part
        n_printf("!!! Accessing ncColorClass component:\n");
        ncColorClass* clc = this->GetComponent(nComponentClassType<ncColorClass>());
        nString str("The color is: ");
        str += clc->getcolor();
        str += "\n";
        n_printf(str.Get());
    }
    NCMDPROTONATIVECPP_DECLARE_END(neSampleClass);


    static void InitClass(nClass * cl)
    {
        ncColorClass::InitClass<neSampleClass>(cl);
        ncPrinterClass::InitClass<neSampleClass>(cl);
        NCMDPROTONATIVECPP_ADDCMD(sampleclassprint);
    }

    NECLASS_COMPONENT(ncColorClass);
    NECLASS_COMPONENT(ncPrinterClass);
};

#endif
