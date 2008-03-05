#ifndef NC_COLOR_H
#define NC_COLOR_H
//------------------------------------------------------------------------------
/**
    @class ncColorClass
    @ingroup Nebula2TutorialsEntities
    @brief Color component class
    @author Marc Ordinas i Llopis
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"

NCOMPONENT_CLASS_BEGIN(ncColorClass)

protected:
    nString color;
    
public:

    NCMDPROTONATIVECPP_DECLARE('GCLR',virtual,const char *,getcolor,0,(),0,())
    {
        return color.Get();
    }

    NCMDPROTONATIVECPP_DECLARE('SCLR',virtual,void,setcolor,1,(const char*),0,())
    {
        color = inarg1;
    }

    /// object persistency
    bool SaveCmds(nPersistServer *ps)
    {
        if( !nComponentClass::SaveCmds(ps) )
            return false;

        nCmd* cmd(ps->GetCmd( this->entityClass, 'SCLR' ));

        cmd->In()->SetS( this->color.Get() );
        ps->PutCmd(cmd);

        return true;
    }

    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(getcolor)
        NCOMPONENT_ADDCMD(setcolor)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_CLASS_END(ncColorClass)

//------------------------------------------------------------------------------
/**
    @class ncColor
    @ingroup Nebula2TutorialsEntities
    @brief Color component object
*/
NCOMPONENT_OBJECT_BEGIN(ncColor)

protected:
    nString shade;
    
public:

    NCMDPROTONATIVECPP_DECLARE('GSHD',virtual,const char *,getshade,0,(),0,())
    {
        return shade.Get();
    }

    NCMDPROTONATIVECPP_DECLARE('SSHD',virtual,void,setshade,1,(const char*),0,())
    {
        shade = inarg1;
    }

    NCMDPROTONATIVECPP_DECLARE('CPRT',virtual,void,colorprint,0,(),0,())
    {
        nString str("This is an instance with shade ");
        
        // Do it the long way to test
        nEntityObject* eobj = this->GetEntityObject();
        ncColor* clr = eobj->GetComponent(nComponentType<ncColor>());
        str += clr->shade;

        // Get the color class component
        str += " and color ";
        ncColorClass* ccl = eobj->GetClassComponent(nComponentClassType<ncColorClass>());
        str += ccl->getcolor();
        str += "\n";

        n_printf(str.Get());
    }


    // define here all the script commands
    NCOMPONENT_INITCMDS_BEGIN()
        NCOMPONENT_ADDCMD(getshade)
        NCOMPONENT_ADDCMD(setshade)
        NCOMPONENT_ADDCMD(colorprint)
    NCOMPONENT_INITCMDS_END()

NCOMPONENT_OBJECT_END(ncColor)

#endif
