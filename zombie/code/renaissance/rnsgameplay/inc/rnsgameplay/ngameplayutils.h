#ifndef NGAMEPLAYUTILS_H
#define NGAMEPLAYUTILS_H

#include "nphysics/nphysicsgeom.h"

namespace nGameplayUtils
{
    void Init();
    void Destroy();

    bool ShotRay( const vector3 & pos, const vector3 & dir, const float dist,
                    vector3 & shotPos, geomid * geom, ncPhysicsObj ** obj, vector3& shotNormal, nGameMaterial ** gmaterial );

    static const vector3 AngleBase( 0, 0, -1 );

#ifndef NGAME
    unsigned long GetNumShots();
    unsigned long GetNumCollisions();
    unsigned long GetMaxCollisions();
    unsigned long GetLastCollisions();
    unsigned long GetTotalCollisions();
    float GetMaxDistCollision();
    float GetLastDistCollision();
#endif// NGAME
}

//------------------------------------------------------------------------------
#endif//NGAMEPLAYUTILS_H
