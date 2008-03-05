#ifndef NCWAYPOINTCLASS_H
#define NCWAYPOINTCLASS_H

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    @class ncWayPointClass
    @ingroup NebulaSystem
    @brief Representation of a waypoint class during developing time.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component ncwaypointclass
    
    @cppclass ncWayPointClass
    
    @superclass nComponentClass

    @classinfo Representation of a waypoint class during developing time.

*/

//-----------------------------------------------------------------------------
#include "entity/nentity.h"
//-----------------------------------------------------------------------------

class ncWayPointClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncWayPointClass,nComponentClass);

public:
    /// constructor
    ncWayPointClass();
    /// destructor
    ~ncWayPointClass();
   
    /// user init instance code.
    void InitInstance(nObject::InitInstanceMsg initType);

};

#else

class ncWayPointClass : public nComponentClass
{
public:
    ncWayPointClass() {}
};

#endif

#endif 