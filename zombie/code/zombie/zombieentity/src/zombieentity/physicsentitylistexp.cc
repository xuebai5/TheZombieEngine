#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

    List of all the entities with its configuration.
    This must be a cc file, even although there is nothing really compiled in.
    It is a really a header defining some macros, but a cc is required by
    the build system right now.
    Entities used in the exporter.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

// include for  define nNebulaEntity
#include "entity/ndefinenebulaentity.h"

//------------------------------------------------------------------------------
/**
*/
#ifndef nNebulaEntity
#define nNebulaEntity(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)
#endif

//------------------------------------------------------------------------------
/**
    Physic entities
*/

nNebulaEntity(
    nePhyComposite, nEntityObject,
    3, (    ncSpatial, ncTransform, ncPhyCompositeObj),
    0, (    ),
    nePhyCompositeClass, nEntityClass,
    3, (    ncTransformClass, ncSpatialClass, ncPhysicsObjClass),
    0, (    )
);

nNebulaEntity(
    nePhySimple, nEntityObject,
    3, (    ncSpatial, ncTransform, ncPhySimpleObj),
    0, (    ),
    nePhySimpleClass, nEntityClass,
    3, (    ncTransformClass, ncSpatialClass, ncPhysicsObjClass),
    0, (    )
);

//------------------------------------------------------------------------------
