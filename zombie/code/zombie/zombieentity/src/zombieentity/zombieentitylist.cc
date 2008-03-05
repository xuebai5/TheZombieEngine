#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    @ingroup ZombieEntity

    List of all the entities with its configuration.
    This must be a cc file, even although there is nothing really compiled in.
    It is a really a header defining some macros, but a cc is required by
    the build system right now.

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
    System entities
*/

nNebulaEntity(
    neSuperentity, nEntityObject,
    6,  (   ncDictionary, ncTransform, ncSubentity, ncLoader, ncSpatial, ncSuperentity),
    2,  (   ncPhyPickableObj, ncEditor),
    neSuperentityClass, nEntityClass,
    5,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass,
            ncPhysicsObjClass),
    1,  (   ncEditorClass)
);

//------------------------------------------------------------------------------
/**
    Sound entities
*/

nNebulaEntity(
    neSoundSource, nEntityObject,
    8,  (   ncDictionary, ncTransform, ncSubentity, ncLoader, ncSpatial, ncSound, 
            ncTriggerShape, ncAreaTrigger),
    2,  (   ncPhyPickableObj, ncEditor),
    neSoundSourceClass, nEntityClass,
    6,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncSoundClass,
            ncPhysicsObjClass),
    1,  (   ncEditorClass)
);

//------------------------------------------------------------------------------
/**
    Test entities
*/
nNebulaEntity(
    neWayPointPath, nEntityObject,
    1, (    ncWayPointPath),
    1, (    ncEditor),
    neWayPointPathClass, nEntityClass,
    0, (    ),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neGameCamera, nEntityObject,
    1, (    ncGameCamera),
    1, (    ncEditor),
    neGameCameraClass, nEntityClass,
    0, (    ),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neFakeEntity, nEntityObject,
    1, (    ncTransform),
    1, (    ncEditor),
    neFakeEntityClass, nEntityClass,
    0, (    ),
    1, (    ncEditorClass)
);
