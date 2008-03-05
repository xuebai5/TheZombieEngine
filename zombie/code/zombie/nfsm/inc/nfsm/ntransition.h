#ifndef N_TRANSITION_H
#define N_TRANSITION_H

//------------------------------------------------------------------------------
/**
    @class nTransition
    @ingroup NebulaFSMSystem

    Transition to one or more target states.

    The transition just have target states, not the source state. The source
    state is implicity known by which state the transition belongs to.

    The transition has a condition attached, but also it's probabilistic.
    That is, it has multiple target states each one of them with an associated
    transition probability. So when the condition evaluates to true, each target
    has some probability to be the choosen one. In fact, it can be that no
    taget is selected if their accumulated probabilities doesn't sum 100%.

    The condition can be either an event or a script. If an event, the condition
    isn't intended to be evaluated periodically, but listening to somebody
    casting the event. Optionally, the event condition can also have attached a
    script, which is intented to be evaluated when the event triggers as a final
    condition validation step (it can be seen as an event filter). At the other
    side, script conditions are intended to be checked periodically.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nCondition;
class nState;

//------------------------------------------------------------------------------
class nTransition : public nRoot
{
public:
    /// Struct to hold info about a target state for the transition
    struct Target
    {
        /// Constructor
        Target( nState* state = NULL, int probability = 100 ) : state(state), probability(probability)
        {}
        ~Target()
        {}

        /// Target state to go when the transition's condition triggers
        nState* state;
        /// Probability to choose the target state when the transition's condition triggers (probability is supposed to be within [0,100] range)
        int probability;
    };

    /// Default constructor
    nTransition();
    /// Destructor (does NOT release the condition)
    ~nTransition();
    /// Get one of the targets at random, but using their probability to be chosen (return NULL if no target is chosen)
    nState* SelectTarget() const;

    /// Set the condition that triggers this transition (replaces the previous one, but does NOT release it)
    void SetCondition(nCondition*);
    /// Get the condition that triggers this transition, or NULL if no condition has beed set
    nCondition* GetCondition () const;
    /// Add a target state to this transition
    void AddTarget(nState*, int);
    /// Remove a target state from this transition
    void RemoveTarget(nState*);
    /// Change the probability to choose the given target state
    void SetTargetProbability(const nState*, int);

    /// Get number of target states
    int GetTargetsNumber () const;
    /// Get a target state by its index
    nState* GetTargetStateByIndex(int) const;
    /// Change a target state by its index
    void SetTargetStateByIndex(int, nState*);
    /// Get a target state probability by its index
    int GetTargetProbabilityByIndex(int) const;
    /// Change a target state probability by its index
    void SetTargetProbabilityByIndex(int, int);
    /// Remove a target state by its index
    void RemoveTargetByIndex(int);

    /// Set an event condition to trigger this transition, loading it if needed (used for peristence)
    void SetEventCondition(const char*);
    /// Set a script condition to trigger this transition, loading it if needed (used for peristence)
    void SetScriptCondition(const char*);
    /// Set an event condition with a filter to trigger this transition (used for peristence)
    void SetFilterCondition(const char*);
    /// Add a target state to this transition, creating the state if needed (used for persistence)
    void AddTargetByName(const char*, const char*, int, int);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Return the index for the given target state within the targets array, or -1 if the target isn't found
    int FindTargetIndex( const nState* state ) const;

    /// Condition that triggers this transition
    nCondition* condition;
    /// Target states where this transition can go to
    nArray<Target> targets;
};

//-----------------------------------------------------------------------------
/**
    Get the condition that triggers this transition, or NULL if no condition has beed set
*/
inline
nCondition* nTransition::GetCondition() const
{
    return this->condition;
}

//-----------------------------------------------------------------------------
/**
    Get number of target states
*/
inline
int nTransition::GetTargetsNumber() const
{
    return this->targets.Size();
}

//-----------------------------------------------------------------------------
/**
    Get a target state by its index
*/
inline
nState* nTransition::GetTargetStateByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->targets.Size() );
    return this->targets[index].state;
}

//-----------------------------------------------------------------------------
/**
    Get a target state probability by its index
*/
inline
int nTransition::GetTargetProbabilityByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->targets.Size() );
    return this->targets[index].probability;
}

#endif
