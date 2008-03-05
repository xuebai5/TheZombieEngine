#ifndef N_COMPONENTOBJECT_H
#define N_COMPONENTOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nComponentObject
    @ingroup NebulaEntitySystem
    @brief Component object base class
    @author Mateu Batle

    Components are the constituent element of entities.

    (c) 2005 Conjurer Services, S.A.
*/ 
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "entity/nentitytypes.h"
#include "entity/nentityobject.h"
#include "entity/nentityclass.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nPersistServer;
class nEntityClass;

//------------------------------------------------------------------------------
class nComponentObject
{
public:
    /// constructor without argument
    nComponentObject();
    /// virtual destructor
    virtual ~nComponentObject();

    // save state of the component
    virtual bool SaveCmds(nPersistServer *);
    /// Initialize the component
    virtual void InitInstance(nObject::InitInstanceMsg /*initType*/);
    
    /// return a pointer to the entity object containing the component
    nEntityObject * GetEntityObject() const;
    /// Set the entity object
    void SetEntityObject(nEntityObject * entContext);

    /// return a pointer to the entity class containing the component
    nEntityClass * GetEntityClass() const;

    /// return component identifier (static class member)
    static const nComponentId & GetComponentIdStatic();
    /// return component identifier (virtual object member)
    virtual const nComponentId & GetComponentId() const;

    /// return component identifier of parent component 
    static const nComponentId & GetParentComponentIdStatic();
    /// return component identifier of parent component 
    virtual const nComponentId & GetParentComponentId() const;

    /// return a component (unsafe)
    nComponentObject * GetComponent(const nComponentId & id) const;
    /// return a component (unsafe)
    nComponentObject * GetComponentByIndex(int index) const;

    /// return a component (safe, cannot be null)
    nComponentObject * GetComponentSafe(const nComponentId & id) const;
    /// return a component (unsafe)
    nComponentObject * GetComponentSafeByIndex(int index) const;

    /// Get component object (unsafe)
    template <class T>
    T * GetComponent() const
    {
        return static_cast<T *> (this->GetComponent(T::GetComponentIdStatic()));
    }

    /// Get component object (safe)
    template <class T>
    T * GetComponentSafe() const
    {
        return static_cast<T *> (this->GetComponentSafe(T::GetComponentIdStatic()));
    }

    /// return a component (unsafe)
    nComponentClass * GetClassComponent(const nComponentId & id) const;

    /// return a component (safe, cannot be null)
    nComponentClass * GetClassComponentSafe(const nComponentId & id) const;

    /// Get a component of the class (unsafe)
    template <class T>
    T * GetClassComponent() const
    {
        return this->GetEntityClass()->GetComponent<T>();
    }

    /// Get a component of the class (safe)
    template <class T>
    T * GetClassComponentSafe() const
    {
        return this->GetEntityClass()->GetComponentSafe<T>();
    }
    
protected:

    friend class nEntityObject;

    /// attach the component to an entity object
    bool AttachToEntity(nEntityObject * eo);

    /// pointer to the entity object
    nEntityObject * entityObject;

};

//------------------------------------------------------------------------------
inline
nComponentObject::nComponentObject() :
    entityObject(0)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nComponentObject::~nComponentObject()
{
    /// empty
}

//------------------------------------------------------------------------------
inline
bool 
nComponentObject::SaveCmds(nPersistServer *)
{
    return true;
}

//------------------------------------------------------------------------------
inline
void 
nComponentObject::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentObject::GetComponentIdStatic()
{
    static const nComponentId nComponentObjectId("nComponentObject");
    return nComponentObjectId;
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentObject::GetComponentId() const
{
    return nComponentObject::GetComponentIdStatic();
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentObject::GetParentComponentIdStatic()
{
    static const nComponentId InvalidComponentId;
    return InvalidComponentId;
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentObject::GetParentComponentId() const
{
    return nComponentObject::GetComponentId();
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nComponentObject::GetComponent(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity");
    if (this->entityObject)
    {
        return this->entityObject->GetComponent(id);
    }
    return 0;
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nComponentObject::GetComponentByIndex(int index) const
{
    n_assert2(this, "Access to non-existent entity");
    if (this->entityObject)
    {
        return this->entityObject->GetComponentByIndex(index);
    }
    return 0;
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nComponentObject::GetComponentSafe(const nComponentId & id) const
{
    nComponentObject * comp = this->GetComponent(id);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nComponentObject::GetComponentSafeByIndex(int index) const
{
    nComponentObject * comp = this->GetComponentByIndex(index);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nComponentObject::GetClassComponent(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity");
    if (this->entityObject)
    {
        return this->GetEntityClass()->GetComponent(id);
    }
    return 0;
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nComponentObject::GetClassComponentSafe(const nComponentId & id) const
{
    nComponentClass * comp = this->GetClassComponent(id);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
inline
nEntityObject * 
nComponentObject::GetEntityObject() const
{
    return this->entityObject;
}

//------------------------------------------------------------------------------
inline
void 
nComponentObject::SetEntityObject(nEntityObject * entObject)
{
    this->entityObject = entObject;
}

//------------------------------------------------------------------------------
#endif // N_COMPONENTOBJECT_H
