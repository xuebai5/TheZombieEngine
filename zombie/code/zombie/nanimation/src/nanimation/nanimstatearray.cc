#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nanimstatearray.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nanimation/nanimstatearray.h"

//------------------------------------------------------------------------------
#ifndef __ZOMBIE_EXPORTER__
static
int __cdecl
n_AnimStateArraySort(const void * a, const void * b)
{
    if (a && b)
    {
        const nAnimState * animA = static_cast<const nAnimState*>(a);
        const nAnimState * animB = static_cast<const nAnimState*>(b);

        return (strcmp(animA->GetAnimFile(), animB->GetAnimFile()));
    }

    return 0;
}
#endif
//------------------------------------------------------------------------------
/**
*/
void
nAnimStateArray::End()
{
#ifndef __ZOMBIE_EXPORTER__
    this->stateArray.QSort(n_AnimStateArraySort);
#endif
}

//------------------------------------------------------------------------------
