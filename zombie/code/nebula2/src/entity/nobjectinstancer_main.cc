//------------------------------------------------------------------------------
//  nobjectinstancer_main.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nobjectinstancer.h"
#include "entity/nentityobjectserver.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nObjectInstancer, "nobjectarray");

//------------------------------------------------------------------------------
void 
nObjectInstancer::SetEntityObjectType (nEntityObjectServer::nEntityObjectType objType) 
{
    this->entityObjectType = objType;
}

//------------------------------------------------------------------------------
nObject * 
nObjectInstancer::BeginNewObject(const char * objClass, const char * objName)
{
    // autoinitialize if it was not done before
    if (!this->objs)
    {
        this->Init(64, 64);
    }

    // get entity class
    nClass * cl = nKernelServer::Instance()->FindClass(objClass);
    nObject * obj = 0;

    if ( !cl)
    {
        obj = 0;
    }
    else if ( ! cl->IsA("nentityobject") )
    {
        // not an entity object
        // check every new object created and set the identifier
        obj = nObject::BeginNewObject(objClass, objName);
    }
    else
    {
        // case when the it is an entity object (a new local id will be assigned)
        obj = nEntityObjectServer::Instance()->NewEntityObjectFromType(objClass, this->entityObjectType, 0, false);
        if (obj)
        {
            // report this to persist server
            nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(obj, nObject::LoadedInstance);
        }
    }

    if (obj)
    {
        this->AppendWithName(obj, objName);        
    }
    else
    {
        n_error("The object of class '%s' could not be created in the object instancer (name='%s')!", objClass, objName);
    }

    return obj;
}
