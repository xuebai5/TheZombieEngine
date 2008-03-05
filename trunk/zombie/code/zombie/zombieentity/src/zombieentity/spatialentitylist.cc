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
    Spatial entities
*/
nNebulaEntity(
    neCamera, nEntityObject,
    3, (    ncTransform, ncSpatialCamera, ncAreaEvent),
    0, (    ),
    neCameraClass, nEntityClass,
    2, (    ncTransformClass, ncSpatialClass),
    0, (    )
);

nNebulaEntity(
    neOctreeSpace, nEntityObject,
    2, (    ncTransform, ncSpatialOctree),
    0, (    ),
    neOctreeSpaceClass, nEntityClass,
    1, (    ncTransformClass),
    0, (    )
);

nNebulaEntity(
    neOutdoor, nEntityObject,
    8, (    ncDictionary, ncTransform, ncLoader, ncSpatialQuadtree, ncPhyTerrain, 
            ncTerrainGMM, ncNavMesh, ncOutdoorBuilder),
    0, (    ),
    neOutdoorClass, nEntityClass,
    10,(    ncTransformClass, ncDictionaryClass, ncAssetClass, ncPhysicsObjClass, ncSceneClass, 
            ncTerrainGMMClass, ncTerrainMaterialClass, ncTerrainVegetationClass, ncHorizonClass,
            ncSpatialQuadtreeClass),
    0, (    )
);

nNebulaEntity(
    neTerrainHole, nEntityObject,
    5, (    ncDictionary, ncTransform, ncLoader, ncSceneIndexed, ncSpatial),
    3, (    ncPhyPickableObj, ncEditor, ncEditorTerrainHole),
    neTerrainHoleClass, nEntityClass,
    6, (    ncDictionaryClass, ncTransformClass, ncAssetClass, ncSceneClass, ncSpatialClass,
            ncPhysicsObjClass),
    2, (    ncEditorClass, ncEditorTerrainHoleClass )
);

nNebulaEntity(
    neOutdoorCell, nEntityObject,
    7, (    ncTransform, ncLoader, ncPhyTerrainCell, ncSpatialQuadtreeCell, ncTerrainGMMCell, 
            ncTerrainVegetationCell, ncCellHorizon),
    0, (    ),
    neOutdoorCellClass, nEntityClass,
    4, (    ncTransformClass, ncDictionaryClass, ncPhysicsObjClass, ncSceneClass),
    0, (    )
);

//------------------------------------------------------------------------------
