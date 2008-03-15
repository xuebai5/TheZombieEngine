#ifndef GPIMPACTSOUND_H
#define GPIMPACTSOUND_H

/**
    (C) 2006 Conjurer Services, S.A.
*/

#include "nphysics/ncphysicsobj.h"

//-----------------------------------------------------------------------------

/// processes the sound information for a given impact
void gpImpactSoundProcess( ncPhysicsObj* objA, ncPhysicsObj* objB, const nPhyCollide::nContact* contact );

//-----------------------------------------------------------------------------

#endif // GPIMPACTSOUND_H