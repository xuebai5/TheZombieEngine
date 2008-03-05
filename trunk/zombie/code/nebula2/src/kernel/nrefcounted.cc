//------------------------------------------------------------------------------
//  nrefcounted.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nrefcounted.h"

//------------------------------------------------------------------------------
/**
*/
nRefCounted::~nRefCounted()
{
    n_assert(0 == this->refCount);
}
