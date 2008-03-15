//------------------------------------------------------------------------------
//  ncomponentobject.cc
//  (c) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/ncomponentobject.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
nEntityClass* 
nComponentObject::GetEntityClass() const
{
    return this->entityObject->GetEntityClass();
}

//------------------------------------------------------------------------------
bool 
nComponentObject::AttachToEntity(nEntityObject * eo)
{
    this->entityObject = eo;
    return true;
}

//------------------------------------------------------------------------------
