#ifndef NC_CHARACTERCLASS_H
#define NC_CHARACTERCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncCharacterClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Character Animation component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class nCharacter2;
class ncSkeletonClass;

//------------------------------------------------------------------------------
class ncCharacterClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncCharacterClass,nComponentClass);

public:
    /// constructor
    ncCharacterClass();
    /// destructor
    virtual ~ncCharacterClass();
    /// save state of the component
    virtual bool SaveCmds(nPersistServer *);
    /// user init instance code
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// get character
    nCharacter2& GetCharacter(int skelIndex);
    /// get skeleton class pointer
    ncSkeletonClass * GetSkeletonClassPointer(int skelIndex = 0) const;

    /// get physics skeleton index
    int GetPhysicsSkeletonIndex() const;

    /// set skeleton class
    void SetSkeletonClass(const char *);
    /// get skeleton class
    const char * GetSkeletonClass();
    /// set skeleton lod class
    void SetSkeletonLevelClass(int, const char *);
    /// get skeleton lod class
    const char * GetSkeletonLevelClass(int) const;
    /// set skeleton lod class
    void SetPhysicsSkeletonClass(const char *);
    /// get skeleton lod class
    const char * GetPhysicsSkeletonClass() const;
    /// get number lod levels
    int GetNumberLevelSkeletons() const;
    /// set ragdoll skeleton class
    void SetRagdollSkeletonClass(const char *);
    /// get ragdoll skeleton class
    bool GetRagdollSkeletonClass(nString&) const;
    /// set first person class
    void SetFirstPersonLevel(int);
    /// get first person class
    int GetFirstPersonLevel() const;
    /// set activity distance
    void SetActivityDistance(float);
    /// get activity distance
    float GetActivityDistance() const;
    /// get activity distance squared
    float GetActivityDistanceSq() const;

    /// returns the ragdoll id manager
    const int GetRagDollManagerId() const;

private:
    nArray<nString> skeletonClassName;
    nArray<ncSkeletonClass*> skeletonClass;

    /// index of physics skeleton
    int physicsSkeletonIndex;

    /// ragdoll skeleton class
    nString ragSkeletonClass;

    /// is first person
    int firstPersonLevel;

    /// ragdoll manager register id
    int ragDollManagerId;

    /// activity distance for collision boxes
    float activityDistance;
    float activityDistanceSq;
};

//------------------------------------------------------------------------------
/**
*/
inline
ncSkeletonClass *
ncCharacterClass::GetSkeletonClassPointer(int skelIndex) const
{
    if (this->skeletonClass.Size() > skelIndex && 0 <= skelIndex)
    {
        return this->skeletonClass[skelIndex];
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacterClass::GetNumberLevelSkeletons() const
{
    return this->skeletonClass.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacterClass::SetFirstPersonLevel(int fperson)
{
    this->firstPersonLevel = fperson;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacterClass::GetFirstPersonLevel() const
{
    return this->firstPersonLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacterClass::SetActivityDistance(float dist)
{
    this->activityDistance = dist;
    this->activityDistanceSq = dist * dist;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ncCharacterClass::GetActivityDistance() const
{
    return this->activityDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ncCharacterClass::GetActivityDistanceSq() const
{
    return this->activityDistanceSq;
}

//------------------------------------------------------------------------------
#endif
