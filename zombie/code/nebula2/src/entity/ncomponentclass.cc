//------------------------------------------------------------------------------
/**
    @file ncomponentclass.cc
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/ncomponentclass.h"

//------------------------------------------------------------------------------
bool 
nComponentClass::SaveCmds(nPersistServer *)
{
    return true;
}

//------------------------------------------------------------------------------
void 
nComponentClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    /// empty
}

//------------------------------------------------------------------------------
