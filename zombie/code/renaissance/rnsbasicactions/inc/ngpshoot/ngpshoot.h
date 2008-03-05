#ifndef N_GPSHOOT_H
#define N_GPSHOOT_H

//------------------------------------------------------------------------------
/**
    @class nGPShoot
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of a shot over a target
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncGPWeapon;
class ncGameplayLiving;
class ncLogicAnimator;

class nGPShoot : public nGPBasicAction
{
public:
    /// Constructor
    nGPShoot();

    /// Destructor
    ~nGPShoot();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    ncGPWeapon * weapon;
    ncGameplayLiving * gameplay;
    ncLogicAnimator * animator;
    ncLogicAnimator * targetAnimator;
    nTime initTime;
    nTime totalTime;

    /// calculate the direction of shoot
    void CalculateDirection( vector3 & pos, vector3 & dir );
    /// shoot using a direction and a position
    void ShootFrom( const vector3 & pos, const vector3 & dir );
};

#endif