#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntity

    List of all the entities with its configuration.
    This must be a cc file, even although there is nothing really compiled in.
    It is a really a header defining some macros, but a cc is required by
    the build system right now.
    Graphic entities used by the exporter.

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
    neMirage, nEntityObject,
    6, (    ncDictionary, ncSubentity, ncTransform, ncLoader, ncSceneLod, ncSpatial),
    2, (    ncPhyPickableObj, ncEditor),
    neMirageClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neBrush, nEntityObject,
    7, (    ncDictionary, ncSubentity, ncTransform, ncLoader, ncSceneLod, ncSpatial, 
            ncPhyCompositeObj ),
    1, (    ncEditor),
    neBrushClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass, 
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neSimpleBrush, nEntityObject,
    6, (    ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncPhySimpleObj),
    1, (    ncEditor ),
    neSimpleBrushClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass, 
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neCharacter, nEntityObject, 
    7, (    ncDictionary, ncTransform, ncLoader, ncPhyCharacterObj, 
            ncSceneRagdoll, ncSpatial, ncCharacter),
    1, (    ncEditor ),
    neCharacterClass, nEntityClass,
    7, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncPhysicsObjClass, 
            ncSceneRagdollClass, ncCharacterClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neSkeleton, nEntityObject,
    7, (    ncDictionary, ncLoader, ncTransform, ncPhyCharacterObj, ncSpatial, 
            ncSceneRagdoll, ncCharacter),
    1, (    ncEditor),
    neSkeletonClass, nEntityClass,
    6, (    ncDictionaryClass, ncAssetClass, ncSkeletonClass, ncPhysicsObjClass, ncSpatialClass, 
            ncSceneRagdollClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neHumRagdoll, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncLoader, ncPhyHumRagDoll, ncHumRagdoll, ncScene),
    1, (    ncEditor),
    neHumRagdollClass, nEntityClass,
    5, (    ncDictionaryClass, ncAssetClass, ncPhyHumRagDollClass, ncHumRagdollClass, ncSceneClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neFourLeggedRagdoll, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncLoader, ncPhyFourleggedRagDoll, ncFourLeggedRagdoll, ncScene),
    1, (    ncEditor),
    neFourLeggedRagdollClass, nEntityClass,
    5, (    ncDictionaryClass, ncAssetClass, ncPhyFourleggedRagDollClass, ncFourLeggedRagdollClass, ncSceneClass),
    1, (    ncEditorClass)
);

//------------------------------------------------------------------------------
