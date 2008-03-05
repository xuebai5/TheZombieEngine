#ifndef N_NAVUTILS_H
#define N_NAVUTILS_H

//------------------------------------------------------------------------------
/**
    @ingroup NebulaNavmeshSystem

    Utility functions related to navigation
*/

#include "mathlib/vector.h"

class ncGameplayClass;
class nFloatMap;
class nPhysicsGeom;

namespace nNavUtils
{
    /// Get the height of the closest ground below the give point
    bool GetGroundHeight( const vector3& point, float& groundHeight );

    /// Get the height of the closest floor physics geometry below the given point
    bool GetGroundLevelHeight( const vector3& point, float& floorHeight );

    /// Check if there's a clear LoS between two points
    bool IsLineOfSight( const vector3& source, const vector3& target );

    /// Check if an entity can walk between two points (unidirectional check)
    bool IsWalkable( const vector3& source, const vector3& target, const ncGameplayClass* entityConfig );

    /// Get the terrain height map
    nFloatMap* GetHeightMap();
};

//------------------------------------------------------------------------------
#endif // N_NAVUTILS_H
