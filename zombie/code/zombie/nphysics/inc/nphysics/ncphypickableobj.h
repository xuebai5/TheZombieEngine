#ifndef N_PHYPICKABLEOBJ_H
#define N_PHYPICKABLEOBJ_H

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    @class ncPhyPickableObj
    @ingroup NebulaPhysicsSystem
    @brief Object to wrap no physics objects so they are pickeable.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Pickable Object
    
    @cppclass ncPhyPickableObj
    
    @superclass ncPhyCompositeObj

    @classinfo Object to wrap no physics objects so they are pickeable.

*/

//-----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------
class ncPhyPickableObj : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyPickableObj,ncPhyCompositeObj);

public:
    /// default constructor
    ncPhyPickableObj();

    /// destructor
    ~ncPhyPickableObj();

    /// set the lengths of the box geometry for this object
    ///  by default are 1 unit per axis
    void SetLengths( const vector3& lengths );

    /// creates the object
    void Create( nPhysicsWorld* world );

    /// begin:scripting
    /// end:scripting

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    // Updates lenghts and position of box with spatial or scene AABB
    void UpdateWithSpatialOrScene();

private:

    /// stores the size of the lenghts
    vector3 lenghts;

};
#else

class ncPhyPickableObj {
    public:
        ncPhyPickableObj() {}
};

#endif // !NGAME

#endif 
