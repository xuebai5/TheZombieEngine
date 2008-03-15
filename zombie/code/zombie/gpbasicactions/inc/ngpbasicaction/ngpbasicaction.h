#ifndef N_GPBASICACTION_H
#define N_GPBASICACTION_H

//------------------------------------------------------------------------------
/**
    @class nGPBasicAction
    @ingroup NebulaGameplayBasicActions

    @brief Superclass for ai actions. All ai basic actions will derive of this class.
    Defines how a action must be coded

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nobject.h"
#include "kernel/ncmdprotonativecpp.h"

#include "ngpactionmanager/ngpactionmanager.h"

class nEntityObject;

class nGPBasicAction : public nObject
{
public:
    /// Constructor
    nGPBasicAction();

    /// Destructor
    virtual ~nGPBasicAction();

        /// Says if the action is initialized
        bool IsInit() const;
        /// Stop condition
       virtual bool IsDone() const;
        /// Main loop
       virtual bool Run();
        /// Post proccess of the action when necessary
       virtual void End();
        /// Set the entity that's executing this action
        void SetOwnerEntity(nEntityObject*);
        /// Get the entity that's executing this action
        nEntityObject* GetOwnerEntity() const;
    
protected:
    /// Says if the action if initialized
    bool init;

    /// Current state of the action
    int state;

    /// Who is executing the action?
    nEntityObject* entity;
};

//------------------------------------------------------------------------------
/**
    IsInit
*/
inline
bool
nGPBasicAction::IsInit() const
{
    return this->init;
}

//------------------------------------------------------------------------------
/**
    IsDone

    @brief Says if the action has ended
*/
inline
bool
nGPBasicAction::IsDone() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    SetOwnerEntity
*/
inline
void
nGPBasicAction::SetOwnerEntity( nEntityObject* entity )
{
    this->entity = entity;
}

//------------------------------------------------------------------------------
/**
    GetOwnerEntity
*/
inline
nEntityObject*
nGPBasicAction::GetOwnerEntity() const
{
    return this->entity;
}

#endif