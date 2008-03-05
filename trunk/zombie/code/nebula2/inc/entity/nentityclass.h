#ifndef N_ENTITYCLASS_H
#define N_ENTITYCLASS_H
//------------------------------------------------------------------------------
/**
    @class nEntityClass
    @ingroup NebulaEntitySystem
    @brief Class used for entity classes
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentitytypes.h"
#include "entity/ncomponentidlist.h"
#include "entity/nclassentityobject.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nEntityClass;
class nComponentClass;
class nComponentIdList;

//------------------------------------------------------------------------------
class nEntityClass : public nClassEntityObject, public nRoot
{
public:

    /// The defalut IsA
    using nRoot::IsA;
    /// The defalut Release
    using nRoot::Release;
    /// The defalut AddRef
    using nRoot::AddRef;
    /// The deault GetRefCount
    using nRoot::GetRefCount;
    /// The deault GetName
    using nRoot::GetName;

    /// constructor
    nEntityClass();

    /// Initialize instance
    virtual void InitInstance(nObject::InitInstanceMsg);
    /// Persistence
    virtual bool SaveCmds(nPersistServer *);

    /// entity classes are persisted a bit different
    virtual bool SaveFile(nFile* file);

    /// add a component class (used on runtime)
    //bool AddComponent(const nComponentClass * comp);
    /// remove a component class (used on runtime)
    //bool RemoveComponent(const nComponentClass * comp);
    /// check if entity class has component of a given type
    bool HasComponentById(const nComponentId & id) const;
    /// number of components in the entity
    int GetNumComponents() const;
    /// get component by index (zero based index)
    nComponentClass * GetComponentByIndex(int index) const;
    /// get component id by index (zero based index)
    const nComponentId & GetComponentIdByIndex(int index) const;
    /// return index of the component specified
    int IndexOfComponent(const nComponentId & id) const;

    /// return a component (unsafe)
    nComponentClass * GetComponent(const nComponentId & id) const;

    /// Get component object (unsafe)
    template <class T>
    T * GetComponent() const
    {
        return static_cast<T *> (this->GetComponent(T::GetComponentIdStatic()));
    }

    /// return a component (safe)
    nComponentClass * GetComponentSafe(const nComponentId & id) const;

    /// Get component object (safe)
    template <class T>
    T * GetComponentSafe() const
    {
        return static_cast<T *> (this->GetComponentSafe(T::GetComponentIdStatic()));
    }

    /// Copy a component from a component of another entity object
    bool CopyComponentFrom(nComponentClass * comp);

    // used only for persistence
   virtual nEntityClass * BeginNewObjectEntityClass(const char *, const char *);
    /// Copy all components in common from another entity class
   virtual bool CopyCommonComponentsFrom(nEntityClass *);
    /// Check if the entity class has a component or not given by name (mainly done for usage from scripting)
   virtual  bool HasComponent(const char *) const;

    bool IsUserCreated() const;

protected:

    friend class nEntityClassServer;
    friend class nClassEntityClass;

    /// destructor
    virtual ~nEntityClass();
    /// Internal method to change the class of an object
    void ChangeClass(nClass* newClass);

    /// set component indexer for the entity class
    void SetComponentIdList(nComponentIdList *);
    /// Add a component unsafely (not called by user)
    bool AddComponentUnsafe(nComponentClass *);

    /// array with class components
    nArray<nComponentClass *> comps;
    /// index information about components of the entity class
    nComponentIdList * compIndexer;

};

//------------------------------------------------------------------------------
inline
nComponentClass * 
nEntityClass::GetComponent(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity class");
    return this->GetComponentByIndex( this->IndexOfComponent(id) );
}

//------------------------------------------------------------------------------
inline
nComponentClass * 
nEntityClass::GetComponentSafe(const nComponentId & id) const
{
    n_assert2(this, "Access to non-existent entity");
    nComponentClass * comp = this->GetComponent(id);
    n_assert(comp);
    return comp;
}

//------------------------------------------------------------------------------
/**
    Check if entity object has or a component object or not
*/
inline
bool 
nEntityClass::HasComponentById(const nComponentId & id) const
{
    return this->compIndexer->HasComponent(id);
}

//------------------------------------------------------------------------------
/**
    Returns the number of components in the entity
*/
inline
int 
nEntityClass::GetNumComponents() const
{
    return this->comps.Size();
}

//------------------------------------------------------------------------------
/**
    Get component by index (zero based index)
*/
inline
nComponentClass * 
nEntityClass::GetComponentByIndex(int index) const
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
nEntityClass::GetComponentIdByIndex(int index) const
{
    return this->compIndexer->GetComponentIdByIndex(index);
}

//------------------------------------------------------------------------------
/**
    Return index of the component specified
*/
inline
int 
nEntityClass::IndexOfComponent(const nComponentId & id) const
{
    return this->compIndexer->GetComponentIndex(id);
}

//------------------------------------------------------------------------------
#endif // N_ENTITYCLASS_H
