#ifndef N_EXPLORERSTATE_H
#define N_EXPLORERSTATE_H
//------------------------------------------------------------------------------
/**
    @class nExploerState
    @ingroup NebulaConjurerEditor

    (C) 2010 The Zombie Team
*/
#include "conjurer/neditorstate.h"

//------------------------------------------------------------------------------
class nExplorerState : public nEditorState
{
public:
    /// constructor
    nExplorerState();
    /// destructor
    virtual ~nExplorerState();

protected:
    /// handle input
    virtual bool HandleInput(nTime frameTime);
};

//------------------------------------------------------------------------------
#endif    
