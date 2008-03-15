#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolsoundsourceprismarea_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolsoundsourceprismarea.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiToolSoundSourcePrismArea, "ninguitool");


//------------------------------------------------------------------------------
/**
*/
nInguiToolSoundSourcePrismArea::nInguiToolSoundSourcePrismArea()
{
    label = "Place polygonal sound source area trigger";

    this->isSticky = false;

    this->triggerClassTypeName = "nesoundsource";
    
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolSoundSourcePrismArea::~nInguiToolSoundSourcePrismArea()
{
    //empty
    
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolSoundSourcePrismArea::SetTriggerClassName(const char* className)
{   
    this->triggerClassName = className;
}
