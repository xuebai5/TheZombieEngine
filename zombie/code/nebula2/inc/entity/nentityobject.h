#ifndef N_ENTITYOBJECT_H
#define N_ENTITYOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nEntityObject
    @ingroup NebulaEntitySystem
    @brief Class used for entity instances
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"
#include "entity/nentityclass.h"
#include "entity/ncomponentidlist.h"

//------------------------------------------------------------------------------
class nEntityClass;
class nEntityObject;
class nPersistServer;
class nComponentObject;

//------------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(nEntityObjectId, "i", (value = (nEntityObjectId) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );

//------------------------------------------------------------------------------
class nEntityObject : public nObject
{
public:

    /// constructor
    nEntityObject();

    /// Initialize the entity instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Persistence
    bool SaveCmds(nPersistServer * ps);

    /// add a component object (used on runtime)
    //bool AddComponent(const nComponentObject * comp);
    /// remove a component object (used on runtime)
    //bool RemoveComponent(const nComponentObject * comp);
    /// has component
    bool HasComponentById(const nComponentId & id) const;
    /// number of components in the entity
    int GetNumComponents() const;
    /// get component by index (zero based index)
    nComponentObject * GetComponentByIndex(int index) const;
    /// get component id by index (zero based index)
    const nComponentId & GetComponentIdByIndex(int index) const;
    /// return index of the component specified
    int IndexOfComponent(const nComponentId & id) const;

    /// return a component (unsafe)
    nComponentObject * GetComponent(const nComponentId & id) const;

    /// Get component object (unsafe)
    template <class T>
    T * GetComponent() const
    {
        return static_cast<T *> (this->GetComponent(T::GetComponentIdStatic()));
    }

    /// return a component (unsafe)
    nComponentObject * GetComponentSafe(const nComponentId & id) const;

    /// Get component object (safe)
    template <class T>
    T * GetComponentSafe() const
    {
        return static_cast<T *> (this->GetComponentSafe(T::GetComponentIdStatic()));
    }

    /// Get a component of the class (unsafe)
    nComponentClass * GetClassComponent(const nComponentId & id) const;

    /// Get a component of the class (unsafe)
    template <class T>
    T * GetClassComponent() const
    {
        return static_cast<T *> (this->GetEntityClass()->GetComponent<T>());
    }

    /// Get a component of the class (safe)
    template <class T>
    T * GetClassComponentSafe() const
    {
        return static_cast<T *> (this->GetEntityClass()->GetComponentSafe<T>());
    }

    /// Copy a component from a component of another entity object
    bool CopyComponentFrom(nComponentObject *);

    /// Get entity class
    nEntityClass * GetEntityClass() const;
    //const nEntityClass * GetEntityClass() const; 
    /// INTERNAL FOR PERSISTENCE, normally not used directly, set object identifier
    void SetId(nEntityObjectId);
    /// get object identifier
    nEntityObjectId GetId() const;
    /// Copy all components in common from another entity object
    bool CopyCommonComponentsFrom(nEntityObject *);
    /// Check if the entity has a component or not given by name (mainly done for usage from scripting)
    bool HasComponent(const char *) const;
#ifndef NGAME
    /// Check if the object is in the limbo
    bool IsInLimbo() const;
    /// Send the object to the limbo list
    void SendToLimbo(nEntityObject* );
    /// Remove object from limbo list
    void ReturnFromLimbo(nEntityObject* );
#endif
    
    /// return OID from persistence OID
    virtual nEntityObjectId FromPersistenceOID( nEntityObjectId );

    /// return persistence OID from OID
    virtual nEntityObjectId ToPersistenceOID( nEntityObjectId );

#ifndef NGAME
    /// @name Signals interface
    //@{
    /// 
    NSIGNAL_DECLARE('JENL', void, EnterLimbo, 0, (), 0, ());
    NSIGNAL_DECLARE('JEXL', void, ExitLimbo, 0, (), 0, ());
    //@}
#endif

protected:

    friend class nEntityClass;
    friend class nClassEntityObject;

    /// destructor
    virtual ~nEntityObject();

    /// set component indexer
    void SetComponentIdList(nComponentIdList *);
    /// Add a component unsafely (not called by user)
    bool AddComponentUnsafe(nComponentObject * obj);

    /// entity object identifier
    nEntityObjectId eoid;

#ifndef NGAME
    /// Limbo checker
    bool isInLimbo;
#endif

    /// array with components
    nArray<nComponentObject *> comps;
    /// index information about components
    nComponentIdList * compIndexer;

};

//------------------------------------------------------------------------------
inline
nEntityObject::nEntityObject() :
    eoid(0),
    compIndexer(0)
#ifndef NGAME
    ,isInLimbo(false)
#endif
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nEntityClass*
nEntityObject::GetEntityClass() const
{
    nEntityClass* cl = static_cast<nEntityClass *> (this->GetClass());
    return cl;
}

//------------------------------------------------------------------------------
inline
nEntityObjectId
nEntityObject::GetId() const
{
    return this->eoid;
}

//------------------------------------------------------------------------------
inline
void
nEntityObject::SetId(nEntityObjectId id)
{
    this->eoid = id;
}

//------------------------------------------------------------------------------
/**
    Check if entity object has or a component object or not
*/
inline
bool 
nEntityObject::HasComponentById(const nComponentId & id) const
{
    return this->compIndexer->HasComponent(id);
}

//------------------------------------------------------------------------------
/**
    Returns the number of components in the entity
*/
inline
int 
nEntityObject::GetNumComponents() const
{
    return this->comps.Size();
}

//------------------------------------------------------------------------------
/**
    Get component by index (zero based index)
*/
inline
nComponentObject * 
nEntityObject::GetComponentByIndex(int index) const
{
    if (index >= 0 && index < this->comps.Size())
    {
        return this->comps[index];
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Get component id by index (zero based index)
*/
inline
const nComponentId &
nEntityObject::GetComponentIdByIndex(int index) const
{
    return this->compIndexer->GetComponentIdByIndex(index);
}

//------------------------------------------------------------------------------
/**
    Return index of the component specified
*/
inline
int 
nEntityObject::IndexOfComponent(const nComponentId & id) const
{
    return this->compIndexer->GetComponentIndex(id);
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nEntityObject::GetComponent(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity");
    return this->GetComponentByIndex( this->IndexOfComponent(id) );
}

//------------------------------------------------------------------------------
inline
nComponentObject * 
nEntityObject::GetComponentSafe(const nComponentId & id) const
{
    nComponentObject * comp = this->GetComponent(id);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nEntityObject::GetClassComponent(const nComponentId & id) const
{
    return this->GetEntityClass()->GetComponent(id);
}

//------------------------------------------------------------------------------
/**
    set component indexer
*/
inline
void 
nEntityObject::SetComponentIdList(nComponentIdList * ind)
{
    n_assert2(ind && ind->GetStatus() >= nComponentIdList::Declared, "ComponentIndexer not ready to use");
    n_assert2(!this->compIndexer || this->compIndexer->GetStatus() != nComponentIdList::Modified, "Old component indexer will be lost");
    this->compIndexer = ind;
}

//------------------------------------------------------------------------------
#endif // N_ENTITYOBJECT_H
