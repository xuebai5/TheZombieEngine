#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncharacteranimator_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncharacteranimator.h"

//------------------------------------------------------------------------------
/**
*/
nCharacterAnimator::nCharacterAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharacterAnimator::~nCharacterAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nCharacterAnimator::Animate(nCharacter2 * /*character*/, nVariableContext* /*renderContext*/)
{
    return false;
}
