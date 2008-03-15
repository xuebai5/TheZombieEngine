#include "precompiled/pchrnsentity.h"
//------------------------------------------------------------------------------
//  gpentitylist.cc
//  (C) 2005 Conjurer Services, S.A.
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
    AI entities
*/

nNebulaEntity(
	neSpawnPoint, nEntityObject,
	5, (	ncTransform, ncDictionary, ncSpawnPoint, ncGameplay, ncSpatial),
	3, (	ncScene, ncPhyPickableObj, ncEditor ),
	neSpawnPointClass, nEntityClass,
	4, (	ncTransformClass, ncDictionaryClass, ncSpatialClass, ncPhysicsObjClass),
	2, (	ncSceneClass, ncEditorClass )
);

nNebulaEntity(
	neSpawner, nEntityObject,
	5, (	ncTransform, ncDictionary, ncSpawner, ncGameplay, ncSpatial),
	3, (	ncScene, ncPhyPickableObj, ncEditor ),
	neSpawnerClass, nEntityClass,
	4, (	ncTransformClass, ncDictionaryClass, ncSpatialClass, ncPhysicsObjClass),
	2, (	ncSceneClass, ncEditorClass )
);

nNebulaEntity(
	nePopulatorArea, nEntityObject,
	2, (	ncDictionary, ncPopulatorArea),
	1, (	ncEditor ),
	nePopulatorAreaClass, nEntityClass,
	1, (	ncDictionaryClass),
	1, (	ncEditorClass ) 
);

nNebulaEntity(
    neAgentEntity, nEntityObject,
    19, (   ncTransform, ncDictionary, ncAIMovEngine, ncAIState, ncGameplayLiving, 
            ncGPHearing, ncGPFeeling, ncGPSight, ncSceneLod, ncPhyCharacterObj,
            ncLogicAnimator, ncLoader, ncSpatial, ncFSM, ncAgentMemory,
            ncAgentTrigger, ncAreaEvent, ncSoundLayer, ncNetworkLiving),
    1,  (   ncEditor ),
    neAgentEntityClass, nEntityClass,
    12,  (   ncDictionaryClass, ncGameplayLivingClass, ncPhysicsObjClass, ncCharacterClass, ncSceneLodClass,
            ncAssetClass, ncSpatialClass, ncFSMClass, ncTriggerClass, ncAreaEventClass,
            ncSoundClass, ncNetworkClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neAreaTrigger, nEntityObject,
    8,  (   ncTransform, ncSubentity, ncDictionary, ncTriggerShape, ncAreaTrigger, ncTriggerOutput,
            ncSpatial, ncGameplay),
    3,  (   ncScene, ncPhyPickableObj, ncEditor),
    neAreaTriggerClass, nEntityClass,
    5,  (   ncTransformClass, ncDictionaryClass, ncSpatialClass, ncGameplayClass, ncPhysicsObjClass),
    2,  (   ncSceneClass, ncEditorClass)
);

nNebulaEntity(
    neAreaEvent, nEntityObject,
    4,  (   ncTransform, ncDictionary, ncSpatial, ncAreaEvent),
    0,  (   ),
    neAreaEventClass, nEntityClass,
    3,  (   ncTransformClass, ncDictionaryClass, ncSpatialClass),
    0,  (   )
);

nNebulaEntity(
    nePlayer, nEntityObject,
    11,  (   ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial, 
            ncLogicAnimator, ncGameplayPlayer, ncAreaEvent, ncNetworkPlayer, ncSoundLayer, 
            ncPhyCharacterObj ),
    0,  (   ),
    nePlayerClass, nEntityClass,
    10,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncCharacterClass, ncGameplayPlayerClass, ncNetworkClass, ncSoundClass, ncPhysicsObjClass ),
    0,  (   )
);

nNebulaEntity(
    neGrenade, nEntityObject,
    8,  (   ncDictionary, ncPhyCompositeObj, ncTransform, ncLoader, ncScene,
            ncSpatial, ncGPGrenade, ncGPExplosion ),
    0,  (   ),
    neGrenadeClass, nEntityClass,
    8,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncSceneClass,
            ncPhysicsObjClass, ncGPGrenadeClass, ncGPExplosionClass ),
    0,  (   )
);

nNebulaEntity(
    neRocket, nEntityObject,
    9,  (   ncDictionary, ncPhyCompositeObj, ncTransform, ncLoader, ncScene, 
            ncSpatial, ncGPRocket, ncGPExplosion, ncSoundLayer ),
    0,  (   ),
    neRocketClass, nEntityClass,
    9,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSpatialClass, ncSceneClass, 
            ncPhysicsObjClass, ncGPRocketClass, ncGPExplosionClass, ncSoundClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neRnsWaypoint, nEntityObject,
    5,  (   ncTransform, ncSubentity, ncDictionary, ncRnsWaypoint, ncSpatial),
    2,  (   ncPhyPickableObj, ncEditor),
    neRnsWaypointClass, nEntityClass,
    4,  (   ncTransformClass, ncDictionaryClass, ncSpatialClass, ncPhysicsObjClass),
    1,  (   ncEditorClass)
);

nNebulaEntity(
    neWeapon, nEntityObject,
    9,  (   ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncPhySimpleObj, ncGPWeapon, ncGPWeaponMelee, ncSoundLayer ),
    1,  (   ncEditor ),
    neWeaponClass, nEntityClass,
    9,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncPhysicsObjClass, ncGPWeaponClass, ncGPWeaponMeleeClass, ncSoundClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neWeaponMelee, nEntityObject,
    8,  (   ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncPhySimpleObj, ncGameplay, ncGPWeaponMelee ),
    1,  (   ncEditor ),
    neWeaponMeleeClass, nEntityClass,
    8,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncPhysicsObjClass, ncGameplayClass, ncGPWeaponMeleeClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neWeaponNatural, nEntityObject,
    3,  (   ncDictionary, ncGameplay, ncGPWeaponMelee ),
    1,  (   ncEditor ),
    neWeaponNaturalClass, nEntityClass,
    3,  (   ncDictionaryClass, ncGameplayClass, ncGPWeaponMeleeClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neWeaponAddon, nEntityObject,
    6,  (   ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncGPWeaponAddon ),
    1,  (   ncEditor ),
    neWeaponAddonClass, nEntityClass,
    6,  (   ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncGPWeaponAddonClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neAmmunition, nEntityObject,
    0,  (   ),
    1,  (   ncEditor ),
    neAmmunitionClass, nEntityClass,
    1,  (   ncGPAmmunitionClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neEquipment, nEntityObject,
    1,  (   ncDictionary ),
    1,  (   ncEditor ),
    neEquipmentClass, nEntityClass,
    1,  (   ncDictionaryClass ),
    1,  (   ncEditorClass )
);

nNebulaEntity(
    neNavMeshNode, nEntityObject,
    2, (    ncNavMeshNode, ncSpatialMC),
    1, (    ncScene),
    neNavMeshNodeClass, nEntityClass,
    1, (    ncSpatialClass),
    1, (    ncSceneClass)
);

nNebulaEntity(
    neNavMeshObstacle, nEntityObject,
    2, (    ncNavMeshObstacle, ncSpatialMC),
    0, (    ),
    neNavMeshObstacleClass, nEntityClass,
    1, (    ncSpatialClass),
    0, (    )
);

nNebulaEntity(
    neGamePlayDoor, nEntityObject,
    4, (   ncGamePlayDoor, ncSuperentity, ncTransform, ncSpatial),
    2, (   ncPhyPickableObj, ncEditor),
    neGamePlayDoorClass, nEntityClass,
    4, (   ncGamePlayDoorClass, ncTransformClass, ncSpatialClass, ncPhysicsObjClass),
    1, (   ncEditorClass)
);

nNebulaEntity(
    neVehicle, nEntityObject,
    10, (    ncDictionary, ncTransform, ncLoader, ncSceneLod, ncSpatial,
            ncPhyVehicle, ncGameplayVehicle, ncNetworkVehicle,ncSound, ncAreaEvent),
    0, (    ),
    neVehicleClass, nEntityClass,
    10, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneLodClass, ncSpatialClass,
            ncPhyVehicleClass,ncGameplayVehicleClass, ncNetworkClass, ncSoundClass, ncAreaEventClass),
    0, (    )
);

nNebulaEntity(
    neVehicleSeat, nEntityObject,
    1, (    ncGameplayVehicleSeat),
    0, (    ),
    neVehicleSeatClass, nEntityClass,
    1, (    ncGameplayVehicleSeatClass),
    0, (    )
);

// Important , call InitIntance ncSpatialLight before InitIntance ncSpatialLight
nNebulaEntity(
    neDotLaser, nEntityObject, 
    6, (    ncDictionary, ncTransform, ncSubentity, ncLoader, ncSpatialLight, ncSceneDotLaser),
    2, (    ncPhyPickableObj, ncEditor),
    neDotLaserClass, nEntityClass,
    6, (    ncTransformClass, ncDictionaryClass, ncAssetClass, ncSceneDotLaserClass, ncSpatialClass,
            ncPhysicsObjClass),
    1, (    ncEditorClass)
);
//------------------------------------------------------------------------------
