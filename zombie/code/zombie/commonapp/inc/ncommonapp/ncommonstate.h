#ifndef N_TRAGSTATE_H
#define N_TRAGSTATE_H
//------------------------------------------------------------------------------
/**
    @class nCommonState
    @ingroup Application

    @brief Zombie base nAppState. Implements gui loading and behavior by loading scripts
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "napplication/nappstate.h"
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nCommonState : public nAppState
{
public:
    /// constructor
    nCommonState();
    /// destructor
    virtual ~nCommonState();
    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called on state to perform state logic 
    virtual void OnFrame();

    /// configure on screen log
    void SetOnScreenLogUpdateInterval(float);

    /// update on screen log
    void UpdateOnScreenLog(const char *);

    /// test on screen log
    void TestLog(const char *);

protected:
    // State script functions names
    nString onEnterFunction;
    nString onLeaveFunction;
    nString onFrameFunction;
    nString onUpdateLogFunction;

    // State script functions protos
    nCmdProto* onEnterFunctionProto;
    nCmdProto* onLeaveFunctionProto;
    nCmdProto* onFrameFunctionProto;
    nCmdProto* onUpdateLogFunctionProto;

    /// On screen log update interval
    nTime oslUpdateInterval;
    
    /// Last on screen log update interval
    nTime oslLastUpdateInterval;

    /// loader server is enabled
    bool loadEnabled;
};

//------------------------------------------------------------------------------
#endif
