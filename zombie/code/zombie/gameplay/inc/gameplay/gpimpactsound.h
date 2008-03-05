#ifndef GPIMPACTSOUND_H
#define GPIMPACTSOUND_H

#include "nphysics/ncphysicsobj.h"

//-----------------------------------------------------------------------------

/// processes the sound information for a given impact
void gpImpactSoundProcess( ncPhysicsObj* objA, ncPhysicsObj* objB, const nPhyCollide::nContact* contact );

//-----------------------------------------------------------------------------

#endif // GPIMPACTSOUND_H