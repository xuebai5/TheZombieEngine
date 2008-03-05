#ifndef N_COMPONENTCLASS_H
#define N_COMPONENTCLASS_H
//------------------------------------------------------------------------------
/**
    @class nComponentClass
    @ingroup NebulaEntitySystem
    @brief Component class base class
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/ 
//------------------------------------------------------------------------------
#include "entity/nentitytypes.h"
#include "entity/nentityclass.h"

//------------------------------------------------------------------------------
class nEntityClass;
class nPersistServer;

//------------------------------------------------------------------------------
class nComponentClass
{
public:
    /// constructor without argument
    nComponentClass();
    /// destructor
    virtual ~nComponentClass();

    // save state of the component
    virtual bool SaveCmds(nPersistServer *);
    /// Initialize the component
    virtual void InitInstance(nObject::InitInstanceMsg /*initType*/);
    
    /// return a pointer to the entity containing the component
    nEntityClass * GetEntityClass() const;
    /// Set the entity class
    void SetEntityClass(nEntityClass * entClass);

    /// return component identifier (static class member)
    static const nComponentId & GetComponentIdStatic();
    /// return component identifier (virtual object member)
    virtual const nComponentId & GetComponentId() const;

    /// return component identifier of parent component 
    static const nComponentId & GetParentComponentIdStatic();
    /// return component identifier of parent component 
    virtual const nComponentId & GetParentComponentId() const;

    /// return a component (unsafe)
    nComponentClass * GetComponent(const nComponentId & id) const;

    /// return a component (safe, cannot be null)
    nComponentClass * GetComponentSafe(const nComponentId & id) const;

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
    
protected:

    friend class nEntityClass;

    /// attach the component to the entity class, return true on success
    bool AttachToEntity(nEntityClass * ec);
 
    /// pointer to the entity class
    nEntityClass * entityClass;

};

//------------------------------------------------------------------------------
inline
nComponentClass::nComponentClass() :
    entityClass(0)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nComponentClass::~nComponentClass()
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nEntityClass * 
nComponentClass::GetEntityClass() const
{
    return this->entityClass;
}

//------------------------------------------------------------------------------
inline
void 
nComponentClass::SetEntityClass(nEntityClass * entClass)
{
    this->entityClass = entClass;
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentClass::GetComponentIdStatic()
{
    static const nComponentId componentId("nComponentClass");
    return componentId;
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentClass::GetComponentId() const
{
    return nComponentClass::GetComponentIdStatic();
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentClass::GetParentComponentIdStatic()
{
    static const nComponentId InvalidComponentId;
    return InvalidComponentId;
}

//------------------------------------------------------------------------------
inline
const nComponentId & 
nComponentClass::GetParentComponentId() const
{
    return nComponentClass::GetParentComponentIdStatic();
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nComponentClass::GetComponent(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity");
    if (this->entityClass)
    {
        return this->entityClass->GetComponent(id);
    }
    return 0;
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nComponentClass::GetComponentSafe(const nComponentId & id) const
{
    nComponentClass * comp = this->GetComponent(id);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
inline
bool 
nComponentClass::AttachToEntity(nEntityClass * ec)
{
    this->entityClass = ec;
    return true;
}

//------------------------------------------------------------------------------
#endif // N_COMPONENTCLASS_H
