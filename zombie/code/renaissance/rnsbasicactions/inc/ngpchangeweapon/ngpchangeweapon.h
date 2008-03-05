#ifndef N_GPCHANGEWEAPON_H
#define N_GPCHANGEWEAPON_H

//------------------------------------------------------------------------------
/**
    @class nAIChangeWeapon
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of changing the current weapon
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPChangeWeapon : public nGPBasicAction
{
public:
    /// Constructor
    nGPChangeWeapon();

    /// Destructor
    ~nGPChangeWeapon();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    nEntityObject* weapon;
};

#endif
