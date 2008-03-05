#ifndef N_MATERIALBUILDER_H
#define N_MATERIALBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nMaterialBuilder
    @ingroup NebulaMaterialSystem

    @brief  Builder of custom shaders from material descriptions

    (C) 2004 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"

class nMaterial;
class nMaterialTree;
//------------------------------------------------------------------------------
class nMaterialBuilder : public nRoot
{
public:
    /// constructor
    nMaterialBuilder();
    /// destructor
    ~nMaterialBuilder();
    /// build passes and shader for a material
    virtual bool LoadMaterial(nMaterial *material);
};

//------------------------------------------------------------------------------
#endif
