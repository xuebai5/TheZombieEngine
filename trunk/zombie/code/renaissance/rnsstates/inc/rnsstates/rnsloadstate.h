#ifndef RNSLOADSTATE_H
#define RNSLOADSTATE_H
//------------------------------------------------------------------------------
/**
    @class RnsLoadState
    @ingroup RnsStates
    
    The Load State class of Renaissance Game
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonstate.h"

#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nGameMaterial;

//------------------------------------------------------------------------------
class RnsLoadState : public nCommonState
{
public:
    /// Constructor
    RnsLoadState();
    /// Destructor
    virtual ~RnsLoadState();

    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// Called when state is becoming active
    virtual void OnStateEnter( const nString & prevState );
    /// Called on state to perform state logic 
    virtual void OnFrame();

        /// set the next state before this
        void SetNextState( const char * stateName );
        /// insert a class to preload
        bool InsertPreloadClass( const char * name );
        /// get the percent of load
        float GetPercent()const;
private:
    /// get the list of effects from a game material
    void PreloadGameMaterial( nGameMaterial * const gameMaterial );

    static const char * PreloadFile;

    nString nextStateName;

    nArray<nString> preLoadClasses;

    int maxPreLoads;

    bool firstFrame;
};

//------------------------------------------------------------------------------
#endif//RNSLOADSTATE_H
