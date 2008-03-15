//------------------------------------------------------------------------------
//  nentityobject_main.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nentity.h"
#include "kernel/nkernelserver.h"
#include "kernel/nsystem.h"
#include "entity/nentityobjectserver.h"
#include "entity/ncpersisthelper.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

//------------------------------------------------------------------------------
nNebulaEntityObject(nEntityObject,"nobject","nentityclass");

//------------------------------------------------------------------------------
#ifndef NGAME
NSIGNAL_DEFINE( nEntityObject, EnterLimbo );
NSIGNAL_DEFINE( nEntityObject, ExitLimbo );
#endif

//------------------------------------------------------------------------------
/**
    Adds a component object to the entity object
*/
//bool 
//nEntityObject::AddComponent(const nComponentObject * comp)
//{
//    n_assert2(comp, "trying to add null component");
//    n_assert2(!comp->GetEntityObject(), "trying to add used component");
//    n_assert2(this->HasComponentById(comp->GetComponentId()), "trying to add a component with conflicting type");
//    n_assert2_always("Not implemented");
//
//    return false;
//}

//------------------------------------------------------------------------------
/**
    Remove a component object
*/
//bool 
//nEntityObject::RemoveComponent(const nComponentObject * comp)
//{
//    n_assert2(comp, "trying to remove null component");
//    n_assert2_always("Not implemented");
//
//    return false;
//}

//------------------------------------------------------------------------------
nEntityObject::~nEntityObject()
{
    n_assert3( !nEntityObjectServer::Instance()->FindEntityObject(this->GetId()), 
        ("Entity object 0x%x (class %s) was released instead of removed from entity object server", this->GetId(), this->GetClass()->GetName() ) );

    int i = this->comps.Size();
    while (i > 0)
    {
        nComponentObject * co = this->GetComponentByIndex(--i);
        n_assert(co);
        n_delete(co);
        this->comps.EraseQuick(i);
    }
}

//------------------------------------------------------------------------------
void 
nEntityObject::InitInstance(nObject::InitInstanceMsg initType)
{
    nObject::InitInstance(initType);

    int i(0);
    while (i < this->comps.Size())
    {
        nComponentObject * obj = this->GetComponentByIndex(i++);
        n_assert(obj);
        obj->InitInstance(initType);
    }
}

//------------------------------------------------------------------------------
/**
    Copy a component from another entity object. It basically executes the 
    commands in SaveCmds from the component on itself.
*/
bool 
nEntityObject::CopyComponentFrom(nComponentObject * comp)
{
    bool retval = false;

    if (comp)
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);

        nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
        ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
        nObject * oldClone = ps->GetClone();
        ps->SetClone(this);

        nKernelServer::Instance()->PushCwd(this);
        if (comp->SaveCmds(ps))
        {
            retval = true;
        }
        nKernelServer::Instance()->PopCwd();

        ps->SetClone(oldClone);
        ps->SetSaveMode(oldMode);
    }

    return retval;
}

//------------------------------------------------------------------------------
bool 
nEntityObject::CopyCommonComponentsFrom(nEntityObject * obj)
{
    n_assert(obj);

    bool ret = true;

    int i(0);
    while (ret && i < this->comps.Size())
    {
        const nComponentId & compId = this->GetComponentIdByIndex(i++);
        if ( obj->HasComponentById( compId ) )
        {
            nComponentObject * comp = obj->GetComponent(compId);
            ret &= this->CopyComponentFrom(comp);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
bool 
nEntityObject::HasComponent(const char * componentName) const
{
    if (!componentName)
    {
        return false;
    }

    return this->HasComponentById( nComponentId(componentName) );
}

#ifndef NGAME
//------------------------------------------------------------------------------
bool 
nEntityObject::IsInLimbo( void ) const
{
    return this->isInLimbo;
}
#endif //NGAME

#ifndef NGAME
//------------------------------------------------------------------------------
void nEntityObject::SendToLimbo( nEntityObject* object )
{   
    this->SignalEnterLimbo( object );
    this->isInLimbo = true;
}
#endif //NGAME

#ifndef NGAME
//------------------------------------------------------------------------------
void nEntityObject::ReturnFromLimbo( nEntityObject* object )
{
    this->SignalExitLimbo( object );
    this->isInLimbo = false;
}
#endif //NGAME 

//------------------------------------------------------------------------------
/**
    set component indexer
*/
bool
nEntityObject::AddComponentUnsafe(nComponentObject * comp)
{
    n_assert(comp);
    if (comp->AttachToEntity(this))
    {
        this->comps.Append(comp);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Return OID from peristence OID
*/
nEntityObjectId
nEntityObject::FromPersistenceOID( nEntityObjectId oid )
{
    ncPersistHelper * ph = this->GetComponent<ncPersistHelper>();
    if (ph)
    {
        return ph->FromPersistenceOID(oid);
    }
    return oid;
}

//------------------------------------------------------------------------------
/**
    @brief Return peristence OID from OID
*/
nEntityObjectId
nEntityObject::ToPersistenceOID( nEntityObjectId oid )
{
    ncPersistHelper * ph = this->GetComponent<ncPersistHelper>();
    if (ph)
    {
        return ph->ToPersistenceOID(oid);
    }
    return oid;
}

//------------------------------------------------------------------------------
