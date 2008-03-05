#ifndef NC_PHYRAGDOLLLIMB_H
#define NC_PHYRAGDOLLLIMB_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyRagDollLimb
    @ingroup NebulaPhysicsSystem
    @brief Base behaviour for rag-doll limbs.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Rag-doll limb.
    
    @cppclass ncPhyRagDollLimb
    
    @superclass ncPhyCompositeObj

    @classinfo Base behaviour for rag-doll limbs.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------

class ncPhyRagDoll;

//-----------------------------------------------------------------------------
class ncPhyRagDollLimb : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyRagDollLimb,ncPhyCompositeObj);

public:
    /// rag-doll limb unique identifier (can be anything like a tag)
    typedef int rdId;

    /// definition of a none valid id
    static const rdId rdNoValidID;

    /// constructor
    ncPhyRagDollLimb();

    /// destructor
    ~ncPhyRagDollLimb();

    /// sets what is the rag-doll
    void SetRagDoll( ncPhyRagDoll* ragdoll );

    /// gets the owner of this limb
    ncPhyRagDoll* GetRagDoll() const;

    /// returns if the limb it's free from the body
    const bool IsFree() const;

    /// function to be procesed during the collision check
    bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// returns the rag-doll Id
    const rdId GetLimbId() const;

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// begin:scripting
    /// end:scripting

    /// creates the object
    virtual void Create( nPhysicsWorld* world );

private:
    /// rag-doll owner
    ncPhyRagDoll* ragDoll;

    /// rag-doll limb id
    rdId ragdollId;

};

//-----------------------------------------------------------------------------
/**
    Returns the rag-doll Id.

    @return limb id

    history:
        - 14-Mar-2005   David Reyes    created
*/
inline
const ncPhyRagDollLimb::rdId ncPhyRagDollLimb::GetLimbId() const
{
    return int(reinterpret_cast<size_t>(this));
}

#endif 
