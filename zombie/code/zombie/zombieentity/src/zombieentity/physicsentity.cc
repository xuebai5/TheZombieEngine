#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "zombieentity/physicsentity.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all physic entity objects and classes
*/

#define N_DEFINE_NEBULA_ENTITY 
#include "physicsentitylist.cc"
#undef N_DEFINE_NEBULA_ENTITY

#pragma warning( default : 4250 )
