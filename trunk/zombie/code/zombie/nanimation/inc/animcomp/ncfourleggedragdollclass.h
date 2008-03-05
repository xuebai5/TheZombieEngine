#ifndef NC_FOURLEGGEDRAGDOLLCLASS_H
#define NC_FOURLEGGEDRAGDOLLCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncFourLeggedRagdollClass
    @ingroup Scene
    @author

    @brief Ragdoll component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "ncragdoll/ncragdollclass.h"

//------------------------------------------------------------------------------
class ncFourLeggedRagdollClass : public ncRagDollClass
{

    NCOMPONENT_DECLARE(ncFourLeggedRagdollClass,ncRagDollClass);

public:
    /// constructor
    ncFourLeggedRagdollClass();
    /// destructor
    ~ncFourLeggedRagdollClass();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

private:

};

//------------------------------------------------------------------------------
#endif

