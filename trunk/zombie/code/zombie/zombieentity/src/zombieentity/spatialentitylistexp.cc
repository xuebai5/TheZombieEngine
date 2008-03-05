#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

    List of all the entities with its configuration.
    This must be a cc file, even although there is nothing really compiled in.
    It is a really a header defining some macros, but a cc is required by
    the build system right now.
    These entities are the ones used by the exporter.

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
    Spatial entities
*/

// IMPORTANT: Place ncNavMesh after ncSuperentity
nNebulaEntity(
    neIndoor, nEntityObject,
    6, (    ncPhyIndoor, ncSpatialIndoor, ncLoader, ncTransform, ncSuperentity,
            ncNavMesh),
    1, (    ncEditor),
    neIndoorClass, nEntityClass,
    4, (    ncPhyIndoorClass, ncSpatialIndoorClass, ncLoaderClass, ncTransformClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neIndoorBrush, nEntityObject,
    5, (    ncDictionary, ncLoader, ncTransform, ncSceneIndexed, ncSpatial),
    1, (    ncEditor),
    neIndoorBrushClass, nEntityClass,
    5, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    nePortal, nEntityObject,
    5, (    ncDictionary, ncLoader, ncTransform, ncSceneIndexed, ncSpatialPortal),
    1, (    ncEditor),
    nePortalClass, nEntityClass,
    5, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neOccluder, nEntityObject,
    2, (    ncSpatialOccluder, ncTransform ),
    1, (    ncPhyPickableObj),
    neOccluderClass, nEntityClass,
    2, (    ncTransformClass, ncPhysicsObjClass),
    0, (    )
);

// IMPORTANT: Place ncNavMesh after ncSuperentity
nNebulaEntity(
    neWalkableBrush, nEntityObject,
    9, (    ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial, 
            ncPhyCompositeObj, ncSubentity, ncSuperentity, ncNavMesh),
    1, (    ncEditor),
    neWalkableBrushClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass, 
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

//------------------------------------------------------------------------------
