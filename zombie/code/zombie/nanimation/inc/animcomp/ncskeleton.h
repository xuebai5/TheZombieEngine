#ifndef NC_SKELETON_H
#define NC_SKELETON_H
//------------------------------------------------------------------------------
/**
    @class ncSkeleton
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Skeleton component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncSkeleton : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSkeleton,nComponentObject);

public:
    /// constructor
    ncSkeleton();
    /// destructor
    ~ncSkeleton();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

private:

};

//------------------------------------------------------------------------------
#endif
