#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

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
    Graphic entities
*/

nNebulaEntity(
    neScene, nEntityObject, 
    4, (    ncDictionary, ncTransform, ncLoader, ncSceneLod),
    1, (    ncEditor),
    neSceneClass, nEntityClass,
    4, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neStaticBatch, nEntityObject,
    5, (    ncDictionary, ncTransform, ncBatchLoader, ncSceneBatch, ncSpatialBatch ),
    2, (    ncPhyPickableObj, ncEditor),
    neStaticBatchClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass,
            ncPhysicsObjClass),
    1, (    ncEditorClass)

);

nNebulaEntity(
    neLight, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncSubentity, ncLoader, ncSceneLight, ncSpatialLight),
    2, (    ncPhyPickableObj, ncEditor),
    neLightClass, nEntityClass,
    6, (    ncTransformClass, ncDictionaryClass, ncAssetClass, ncSceneLightClass, ncSpatialClass,
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neOutLight, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncSubentity, ncLoader, ncSceneLight, ncSpatialLightEnv),
    2, (    ncPhyPickableObj, ncEditor),
    neOutLightClass, nEntityClass,
    6, (    ncTransformClass, ncDictionaryClass, ncAssetClass, ncSceneLightEnvClass, ncSpatialClass,
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neViewport, nEntityObject, 
    4, (    ncDictionary, ncTransform, ncLoader, ncViewport),
    0, (    ),
    neViewportClass, nEntityClass,
    4, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass),
    0, (    )
);

nNebulaEntity(
    neSpecialFX, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncLoader, ncScene, ncSpatial, ncSpecialFX),
    2, (    ncPhyPickableObj, ncEditor),
    neSpecialFXClass, nEntityClass,
    7, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass, 
            ncSpecialFXClass, ncPhysicsObjClass),
    1, (    ncEditorClass)
);

//------------------------------------------------------------------------------
