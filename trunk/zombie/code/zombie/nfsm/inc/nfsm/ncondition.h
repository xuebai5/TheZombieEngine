#ifndef N_CONDITION_H
#define N_CONDITION_H

//------------------------------------------------------------------------------
/**
    @class nCondition
    @ingroup NebulaFSMSystem

    Interface for all conditions that triggers a state transition.
*/

#include "kernel/nroot.h"

class nEntityObject;

//------------------------------------------------------------------------------
class nCondition : public nRoot
{
public:
    /// Enum to represent the condition type id
    enum TypeId
    {
        Event,
        Script
    };

    /// Constructor
    nCondition( TypeId conditionType );
    /// Destructor
    virtual ~nCondition();
    /// Return the type id of this condition
    TypeId GetConditionType() const;
    /// Tell if the condition rule evaluates to true for the given entity
    virtual bool Evaluate( nEntityObject* entity ) const = 0;

private:
    /*const*/ TypeId conditionType; // Non const to avoid "assignment operator could not be generated" warning
};

//-----------------------------------------------------------------------------
/**
     Return the type id of this condition
*/
inline
nCondition::TypeId nCondition::GetConditionType() const
{
    return this->conditionType;
}

#endif
