#ifndef NC_PHYINDOORCLASS_H
#define NC_PHYINDOORCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyIndoorClass
    @ingroup NebulaPhysicsSystem
    @brief Represents an indoor.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "nphysics/ncphysicsobjclass.h"
//-----------------------------------------------------------------------------
class ncPhyIndoorClass : public ncPhysicsObjClass
{

    NCOMPONENT_DECLARE(ncPhyIndoorClass,ncPhysicsObjClass);

public:

    // returns the indoor physics path
    void GetPath( nString& path ) const;

};

#endif