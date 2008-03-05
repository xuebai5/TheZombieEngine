#ifndef RNSMENUSTATE_H
#define RNSMENUSTATE_H
//------------------------------------------------------------------------------
/**
    @class RnsMenuState
    @ingroup RnsStates
    
    The Menu State class of Renaissance Game
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonstate.h"

#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nGameMaterial;

//------------------------------------------------------------------------------
class RnsMenuState : public nCommonState
{
public:
    /// Constructor
    RnsMenuState();
    /// Destructor
    virtual ~RnsMenuState();

    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// Called when state is becoming active
    virtual void OnStateEnter( const nString & prevState );
    /// Called on state to perform state logic 
    virtual void OnFrame();

    /// set the exit state before this
    void SetExitState( const char * stateName );

    /// Exit from Menu State
    void Exit();

private:
    nString exitStateName;
    bool useExitName;
};

//------------------------------------------------------------------------------
#endif//RNSMENUSTATE_H
