#ifndef N_INGUI_TOOL_SOUND_SOURCE_PRISM_H
#define N_INGUI_TOOL_SOUND_SOURCE_PRISM_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolsoundsourceprism.h
    @class nInguiToolSoundSourcePrismArea
    @ingroup NebulaConjurerEditor

    @author Dominic Ashby

    @brief nInguiTool to create prism trigger areas for sound sources

    (C) 2006 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolprismarea.h"

//------------------------------------------------------------------------------
class nInguiToolSoundSourcePrismArea: public nInguiToolPrismArea
{
    public:

    // constructor
    nInguiToolSoundSourcePrismArea();
    // destructor
    virtual ~nInguiToolSoundSourcePrismArea();

    /// Set the trigger class name with the string passed
    void SetTriggerClassName(const char *);

};

#endif
