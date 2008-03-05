#ifndef NC_HUMRAGDOLLCLASS_H
#define NC_HUMRAGDOLLCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncHumRagdollClass
    @ingroup Scene
    @author

    @brief Ragdoll component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "ncragdoll/ncragdollclass.h"

//------------------------------------------------------------------------------
class ncHumRagdollClass : public ncRagDollClass
{

    NCOMPONENT_DECLARE(ncHumRagdollClass,ncRagDollClass);

public:
    /// constructor
    ncHumRagdollClass();
    /// destructor
    ~ncHumRagdollClass();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

};

//------------------------------------------------------------------------------
#endif

