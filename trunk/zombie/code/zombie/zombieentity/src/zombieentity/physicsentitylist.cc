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
    Physic entities
*/

nNebulaEntity(
    neSea, nEntityObject,
    6, (    ncDictionary, ncTransform, ncLoader, ncScene, ncSpatial,
            ncPhySea),
    0, (    ),
    neSeaClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass,
            ncPhysicsObjClass),
    0, (    )
);

nNebulaEntity(
    neWaterPool, nEntityObject,
    6, (    ncDictionary, ncTransform, ncLoader, ncScene, ncSpatial,
            ncPhyWaterPool),
    0, (    ),
    neWaterPoolClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass,
            ncPhysicsObjClass),
    0, (    )
);

nNebulaEntity(
    neCloth, nEntityObject,
    5, (    ncDictionary, ncTransform, ncLoader, ncSpatial, ncPhyCloth),
    0, (    ),
    neClothClass, nEntityClass,
    5, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncPhysicsObjClass),
    0, (    )
);

nNebulaEntity(
    neWheel, nEntityObject,
    8, (    ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncPhyWheel,ncSound,ncGameplayWheel),
    1, (    ncEditor),
    neWheelClass, nEntityClass,
    8, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncPhysicsObjClass,ncSoundClass,ncGameplayWheelClass),
    1, (    ncEditorClass)
);

nNebulaEntity(
    neJoint, nEntityObject,
    5, (    ncDictionary, ncTransform, ncLoader, ncSpatial, ncPhyJoint),
    0, (    ),
    neJointClass, nEntityClass,
    5, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncPhyJointClass),
    0, (    )
);

nNebulaEntity(
    neRagdollLimb, nEntityObject,
    2, (    ncTransform, ncPhyRagDollLimb),
    1, (    ncEditor),
    neRagdollLimbClass, nEntityClass,
    2, (    ncTransformClass, ncPhysicsObjClass),
    1, (    ncEditorClass)
);

nNebulaEntity(  
    nePhyCharacter, nEntityObject,  
    2, (    ncTransform, ncPhyCharacter),  
    0, (    ),  
    nePhyCharacterClass, nEntityClass,  
    1, (    ncPhysicsObjClass),  
    0, (    )  
);  

//------------------------------------------------------------------------------
