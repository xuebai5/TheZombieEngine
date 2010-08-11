#ifndef N_SUMMONERSTATE_H
#define N_SUMMONERSTATE_H
//------------------------------------------------------------------------------
/**
    @class nSummonerState
    @ingroup NebulaConjurerEditor

    (C) 2010 The Zombie Team
*/
#include "conjurer/neditorstate.h"

//------------------------------------------------------------------------------
class nSummonerState : public nEditorState
{
public:
    /// constructor
    nSummonerState();
    /// destructor
    virtual ~nSummonerState();

protected:
    /// handle input
    virtual bool HandleInput(nTime frameTime);
};

//------------------------------------------------------------------------------
#endif    
