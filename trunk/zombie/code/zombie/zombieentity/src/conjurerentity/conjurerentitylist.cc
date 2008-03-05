#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

    List of all the conjurer entities with its configuration.
    This must be a cc file, even although there is nothing really compiled in.
    It is a really a header defining some macros, but a cc is required by
    the build system right now.

    (C) 2005 Conjurer Services, S.A.

    NOTE: This entities only exists when conjurer is present.
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

//#ifndef NGAME

//------------------------------------------------------------------------------

/**
    @classinfo Conjurer specific waypoint with flags
*/
nNebulaEntity(
    neWayPoint, nEntityObject,
    3, (    ncTransform,ncScene,ncSpatial),
    2, (    ncEditor,ncWayPoint ),
    neWayPointClass, nEntityClass,
    4, (    ncTransformClass,ncPhysicsObjClass,ncSceneClass,ncSpatialClass),
    2, (    ncEditorClass,ncWayPointClass )
);

//#endif
