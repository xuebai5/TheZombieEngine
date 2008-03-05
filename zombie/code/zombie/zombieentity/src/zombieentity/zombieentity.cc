#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 4100 )

//------------------------------------------------------------------------------

#include "entity/nentity.h"
#include "zombieentity/zombieentity.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all entity objects and classes
*/
#define N_DEFINE_NEBULA_ENTITY 
#include "zombieentitylist.cc"
#undef N_DEFINE_NEBULA_ENTITY 


#pragma warning( default : 4250 4100 )
