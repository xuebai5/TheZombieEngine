#ifndef N_INGUI_TOOL_SPHERE_SOUND_SOURCE_H
#define N_INGUI_TOOL_SPHERE_SOUND_SOURCE_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolsoundsourcesphere.h
    @class nInguiToolSoundSourceSphereArea
    @ingroup NebulaConjurerEditor

    @author Dominic Ashby

    @brief nInguiTool to create spherical trigger areas for sound sources

    (C) 2006 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolspherearea.h"

//------------------------------------------------------------------------------
class nInguiToolSoundSourceSphereArea: public nInguiToolSphereArea
{
    public:

    // constructor
    nInguiToolSoundSourceSphereArea();
    // destructor
    virtual ~nInguiToolSoundSourceSphereArea();
    
    // set the trigger class name with the string passed 
    void SetTriggerClassName(const char*);

};

#endif
