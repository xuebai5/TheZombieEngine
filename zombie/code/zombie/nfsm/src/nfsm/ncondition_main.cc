//-----------------------------------------------------------------------------
//  ncondition_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/ncondition.h"

//-----------------------------------------------------------------------------
/**
    Constructor
*/
nCondition::nCondition( TypeId conditionType )
    : conditionType(conditionType)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nCondition::~nCondition()
{
    /// Empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
