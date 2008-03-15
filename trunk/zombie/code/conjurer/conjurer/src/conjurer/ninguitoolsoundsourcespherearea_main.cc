#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolsoundsourcespherearea_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolsoundsourcespherearea.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiToolSoundSourceSphereArea, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiToolSoundSourceSphereArea::nInguiToolSoundSourceSphereArea()
{
    label = "Place spherical sound source area trigger";
    this->isSticky = false;
    
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolSoundSourceSphereArea::~nInguiToolSoundSourceSphereArea()
{
    //empty
    
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolSoundSourceSphereArea::SetTriggerClassName(const char* className)
{   
    this->triggerClassName = className;
}