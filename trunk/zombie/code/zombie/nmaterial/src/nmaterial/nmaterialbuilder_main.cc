#include "precompiled/pchnmaterial.h"
//--------------------------------------------------
//  nmaterialbuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//--------------------------------------------------
#include "nmaterial/nmaterialbuilder.h"

nNebulaClass(nMaterialBuilder, "nroot");

//------------------------------------------------------------------------------
/**
*/
nMaterialBuilder::nMaterialBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialBuilder::~nMaterialBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Subclasses should build shader for materials.
*/
bool
nMaterialBuilder::LoadMaterial(nMaterial * /*material*/ )
{
    return false;
}
