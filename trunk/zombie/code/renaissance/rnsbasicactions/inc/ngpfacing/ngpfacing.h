#ifndef N_GPFACING_H
#define N_GPFACING_H

//------------------------------------------------------------------------------
/**
    @class nGPFacing
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the facing of the an agent respect a 
           target or a direction

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"
#include "mathlib/quaternion.h"

class nGPFacing : public nGPBasicAction
{
public:
    /// Constructor
    nGPFacing();

    /// Destructor
    ~nGPFacing();

        /// Initial condition
        bool Init(nEntityObject*, const vector3&, bool);

    /// Stop condition
    bool IsDone() const;

private:
    /// Face to a point
    void FaceTo (const vector3& point);
    /// Move the head to a direction
    void GlanceTo (const quaternion& orientation);

    quaternion orientation;
};

#endif