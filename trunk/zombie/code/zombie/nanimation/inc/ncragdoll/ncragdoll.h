#ifndef NC_RAGDOLL_H
#define NC_RAGDOLL_H

//-----------------------------------------------------------------------------
/**
    @class ncRagDoll
    @ingroup Scene
    @brief A rag doll communality interface.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Ragdoll Interface
    
    @cppclass ncRagDoll
    
    @superclass ncSkeleton

    @classinfo A rag doll communality interface.
*/

//-----------------------------------------------------------------------------

#include "animcomp/ncskeleton.h"

class nCharacter2;
class nRagdollCharAnimator;
//-----------------------------------------------------------------------------

class ncRagDoll : public ncSkeleton
{

    NCOMPONENT_DECLARE(ncRagDoll,ncSkeleton);

public:
    
    /// constructor
    ncRagDoll();

    /// destructor
    ~ncRagDoll();

    /// 
    void Limbo();
    /// 
    void UnLimbo();

    /// set parent object
    void SetParentObject(nEntityObject* eobj);
    /// get parent object
    nEntityObject* GetParentObject() const;

    virtual void SetJointData(int, vector3&, vector3&, const quaternion&, const vector3&);

    /// switches off the ragdoll
   virtual void SwitchOff();
    /// get physics data (updated data)
   virtual void GetJointData(int, vector3&, quaternion&, vector3&);
    /// get num rag joints
    int GetNumRagJoints();

    /// is valid
    bool IsValid();

    /// get character object
    nCharacter2 *GetCharacter();
    /// set character object
    void SetCharacter(nCharacter2*);

    ///create ragdoll
    virtual void CreateRagdoll();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

protected:
    nCharacter2* character;
    nRagdollCharAnimator* ragAnimator;
    bool active;

    nRef<nEntityObject> refEntityParentObj;

};

#endif