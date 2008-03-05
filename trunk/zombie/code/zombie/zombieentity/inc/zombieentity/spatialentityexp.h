#ifndef N_SPATIALENTITYEXP_H
#define N_SPATIALENTITYEXP_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

    List of all the spatial entities with its configuration.
    These entities are the ones used by the exporter.

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
/**
    Declaration of classes for all spatial entity objects and classes
*/
#include "entity/nentity.h"
#define N_DECLARE_NEBULA_ENTITY
#include "../../src/zombieentity/spatialentitylistexp.cc"
#undef N_DECLARE_NEBULA_ENTITY

#endif //N_SPATIALENTITYEXP_H
