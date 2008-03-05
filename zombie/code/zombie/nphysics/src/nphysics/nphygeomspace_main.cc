//-----------------------------------------------------------------------------
//  nphygeomspace_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomSpace, "nphyspace");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 23-Sep-2004   David Reyes    created
*/
nPhyGeomSpace::nPhyGeomSpace()
{
    this->type = SimpleSpace;

    this->Create();
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 23-Sep-2004   David Reyes    created
*/
nPhyGeomSpace::~nPhyGeomSpace()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 23-Sep-2004   David Reyes    created
        - 01-Oct-2004   David Reyes    supressed name
*/
void nPhyGeomSpace::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempting to create an object already created." );

    this->geomID = phyCreateSimpleSpace();    

    n_assert2( this->Id() != NoValidID , "Hasn't been possible to create the space" );

    nPhySpace::Create();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
