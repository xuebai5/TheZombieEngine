#ifndef N_INGUI_TOOL_SPHERE_H
#define N_INGUI_TOOL_SPHERE_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolsphere.h
    @class nInguiToolSphereArea
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool to create polygonal trigger areas

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainline.h"
#include "tools/nlinedrawer.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nAppViewport;
class neAreaTrigger;

//------------------------------------------------------------------------------
class nInguiToolSphereArea: public nInguiToolPhyPick
{
    public:

    // constructor
    nInguiToolSphereArea();
    // destructor
    virtual ~nInguiToolSphereArea();

    /// Apply the tool
    virtual bool Apply( nTime dt );

protected:
    
    /// the class name for any triggers created
    nString triggerClassName;

private:

    // Area trigger being edited
    nRef<nEntityObject> sphereTrigger;
};

#endif
