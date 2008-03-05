#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacterclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "entity/nentityclassserver.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "nragdollmanager/nragdollmanager.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncCharacterClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncCharacterClass::ncCharacterClass() :
    skeletonClass(0, 2),
    physicsSkeletonIndex(-1),
    ragDollManagerId(-1),
    firstPersonLevel(-1),
    activityDistance(50.f),
    activityDistanceSq(2500.f)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncCharacterClass::~ncCharacterClass()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacterClass::SetSkeletonClass(const char * className)
{
    this->skeletonClassName.Set(0, className);

    // find the entity class for this name
    nEntityClass *entityClass = nEntityClassServer::Instance()->GetEntityClass(className);
    n_assert(entityClass);

    this->skeletonClass.Set(0, entityClass->GetComponent<ncSkeletonClass>());
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncCharacterClass::GetSkeletonClass()
{
    if (this->skeletonClassName.Size() > 0)
    {
        return this->skeletonClassName[0].Get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncCharacterClass::GetSkeletonLevelClass(int index) const
{
    n_assert(index >= 0);
    if (index < this->skeletonClassName.Size())
    {
        return this->skeletonClassName[index].Get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacterClass::SetSkeletonLevelClass(int index, const char * className)
{
    this->skeletonClassName.Set(index, className);

    // find the entity class for this name
    nEntityClass *entityClass = nEntityClassServer::Instance()->GetEntityClass(className);
    n_assert(entityClass);

    this->skeletonClass.Set(index, entityClass->GetComponent<ncSkeletonClass>());
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncCharacterClass::GetPhysicsSkeletonClass() const
{
    if (this->physicsSkeletonIndex >= 0 &&
        this->physicsSkeletonIndex < this->skeletonClassName.Size())
    {
        return this->skeletonClassName[this->physicsSkeletonIndex].Get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacterClass::SetPhysicsSkeletonClass(const char * className)
{
    //search if skeleton has been already added
    this->physicsSkeletonIndex = this->skeletonClassName.FindIndex(className);

    if (this->physicsSkeletonIndex  == -1)
    {
        this->physicsSkeletonIndex = this->skeletonClassName.Size();
        this->skeletonClassName.Set(this->physicsSkeletonIndex, className);

        // find the entity class for this name
        nEntityClass *entityClass = nEntityClassServer::Instance()->GetEntityClass(className);
        n_assert(entityClass);

        this->skeletonClass.Set(this->physicsSkeletonIndex, entityClass->GetComponent<ncSkeletonClass>());
    }
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2&
ncCharacterClass::GetCharacter(int index) 
{
    n_assert(this->skeletonClass.Size() > index);
    return this->skeletonClass[index]->GetCharacter();
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacterClass::GetPhysicsSkeletonIndex() const
{
    if (this->skeletonClassName.Size() == 1)
    {
        return 0;
    }

    return this->physicsSkeletonIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacterClass::SetRagdollSkeletonClass(const char * className)
{
    this->ragSkeletonClass = className;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacterClass::GetRagdollSkeletonClass(nString& ragClassName) const
{
    if (!this->ragSkeletonClass.IsEmpty())
    {
        ragClassName = this->ragSkeletonClass;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Returns the ragdoll id manager

    @return id

    history:
        - 08-Nov-2005   David Reyes    created

*/
const int ncCharacterClass::GetRagDollManagerId() const
{
    return this->ragDollManagerId;
}

//------------------------------------------------------------------------------
/**
    User init instance code

    history:
        - 08-Nov-2005   David Reyes    created

*/
void ncCharacterClass::InitInstance(nObject::InitInstanceMsg initType)
{
#ifndef __ZOMBIE_EXPORTER__
    if (initType == nObject::LoadedInstance)
    {
        nString ragdollSkeletonClass;

        this->GetRagdollSkeletonClass(ragdollSkeletonClass);
        
        if (ragdollSkeletonClass.Length())
        {
            // registering the ragdoll
            this->ragDollManagerId = nRagDollManager::Instance()->Register(ragdollSkeletonClass, 10);
        }    
    }
#endif
}
