#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "zombieentity/physicsentityexp.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all physic entity objects and classes
*/

#define N_DEFINE_NEBULA_ENTITY 
#include "physicsentitylistexp.cc"
#undef N_DEFINE_NEBULA_ENTITY

#pragma warning( default : 4250 )
