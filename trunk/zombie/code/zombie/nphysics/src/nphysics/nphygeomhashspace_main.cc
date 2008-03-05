//-----------------------------------------------------------------------------
//  nphygeomhashspace_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomHashSpace, "nphyspace");

//-----------------------------------------------------------------------------

const int nPhyGeomHashSpace::MaxLevelDefaultValue(3);

const int nPhyGeomHashSpace::MinLevelDefaultValue(1);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 08-Oct-2004   David Reyes    created
*/
nPhyGeomHashSpace::nPhyGeomHashSpace()
{

    this->type = HastTableBasedSpace;

    this->Create();
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 08-Oct-2004   David Reyes    created
*/
nPhyGeomHashSpace::~nPhyGeomHashSpace()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 08-Oct-2004   David Reyes    created
*/
void nPhyGeomHashSpace::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempting to create an object already created." );

    geomID = phyCreateHashSpace();    

    n_assert2( this->Id() != NoValidID , "Hasn't been possible to create the space" );

    // set default values
    this->SetLevels( MaxLevelDefaultValue, MinLevelDefaultValue );

    nPhySpace::Create();
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
