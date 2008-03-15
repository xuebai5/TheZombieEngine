#include "precompiled/pchrnsentity.h"
//------------------------------------------------------------------------------
//  gpentity.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "rnsentity/gpentity.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all gameplay entity objects and classes
*/
// disable warnings about inherits via dominance
#define N_DEFINE_NEBULA_ENTITY
#include "gpentitylist.cc"
#undef N_DEFINE_NEBULA_ENTITY 


#pragma warning( default : 4250 )
