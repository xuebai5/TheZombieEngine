#ifndef N_COMPONENTINDEXER_H
#define N_COMPONENTINDEXER_H
//------------------------------------------------------------------------------
/**
    @class nComponentIdList
    @ingroup NebulaEntitySystem
    @brief Class used to keep track of the indices of the components
    @author Mateu Batle

    The nComponentIdList is normally shared between entity objects, at least
    the ones belonging to the same entity class (which have same structure,
    that is the same types and number of components). There is a process to
    declare the components making up the structure of the entity object of
    an specific entity class. During this process the status of the component
    indexer goes from empty -> declaring -> declared states.

    Anyway, as entity objects can evolve through time, by adding new components
    or removing existing components, each entity object might be able to have
    one of this structures itself. In these cases, the status change from 
    declared -> modified state. Before the change a copy of the component
    indexer is done, and modified only for that entity object.
    Normally most entity objects don't change their structure never.

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentitytypes.h"
#include "util/nkeyarray.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nComponentIdList
{
public:

    const static int InvalidIndex = -1;

    /// status of the component indexer
    enum nStatus
    {
        /// Nothing added yet
        Empty,
        /// In declaration state (after BeginComponents)
        Declaring,
        /// Declaration has finished (after EndComponents)
        Declared,
        /// Modified after initial declaration
        Modified
    };

    /// constructor
    nComponentIdList();
    /// copy constructor
    nComponentIdList(const nComponentIdList & src);
    /// destructor
    ~nComponentIdList();

    /// assignment operator 
    const nComponentIdList & operator =(const nComponentIdList & rhs);

    /// begin adding components
    void BeginComponents();
    /// end adding components
    void EndComponents();
    /// add a component object
    bool AddComponent(const nComponentId & id);
    /// add a parent component object (parent of last added component)
    bool AddParentComponent(const nComponentId & id);
    /// remove a component object
    bool RemoveComponent(const nComponentId & id);
    /// number of components in the entity
    int GetNumComponents() const;
    /// get index of the component  (zero based index)
    int GetComponentIndex(const nComponentId & id) const;
    /// get component by index (zero based index)
    const nComponentId & GetComponentIdByIndex(int index) const;
    /// has component
    bool HasComponent(const nComponentId & id) const;
    /// check status of the component
    nStatus GetStatus() const;

private:

    /// state of the component indexer
    nStatus state;
    /// number of components (without counting parent components)
    int numComponents;
    /// translates from from component id to index (usable for parent components)
    nKeyArray<int> comps;
    /// tree of inheritance component ids (components indexed by component index)
    nArray< nArray<nComponentId> > compIdTree;

};

//------------------------------------------------------------------------------
inline
nComponentIdList::nComponentIdList() : 
    numComponents(0),
    comps(8,8),
    state(Empty)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nComponentIdList::nComponentIdList(const nComponentIdList & src) :
    comps(8,8)
{
    *this = src;
}

//------------------------------------------------------------------------------
inline
nComponentIdList::~nComponentIdList()
{
    /// empty
}

//------------------------------------------------------------------------------
inline
const nComponentIdList & 
nComponentIdList::operator =(const nComponentIdList & rhs)
{
    this->numComponents = rhs.numComponents;
    this->comps = rhs.comps;
    this->compIdTree = rhs.compIdTree;
    this->state = rhs.state;

    return *this;
}

//------------------------------------------------------------------------------
inline
void 
nComponentIdList::BeginComponents()
{
    n_assert(this->state == Empty);
    this->state = Declaring;
}

//------------------------------------------------------------------------------
inline
void 
nComponentIdList::EndComponents()
{
    n_assert(this->state == Declaring);
    this->state = Declared;
}

//------------------------------------------------------------------------------
inline
bool 
nComponentIdList::AddComponent(const nComponentId & id)
{
    n_assert2(!this->HasComponent(id), "ComponentId already added");

    // add the component id to the key to index array
    this->comps.Add(id.AsKey(), this->numComponents);
    ++this->numComponents;

    // add the component id to a branch of the component id tree
    nArray<nComponentId> * idBranch = this->compIdTree.Reserve(1);
    n_assert(this->compIdTree.Size() == this->numComponents);
    idBranch->Append(id);

    // if added components past the end of components -> change state to modified
    if (this->state == Declared)
    {
        this->state = Modified;
    }

    return true;
}

//------------------------------------------------------------------------------
inline
bool 
nComponentIdList::AddParentComponent(const nComponentId & id)
{
    n_assert2(this->GetComponentIndex(id), "ComponentId already added");

    // add the component id to the key to index array
    this->comps.Add(id.AsKey(), this->numComponents - 1);

    // add the component id to a branch of the component id tree
    nArray<nComponentId> & idBranch = this->compIdTree[this->numComponents - 1];
    n_assert(this->compIdTree.Size() == this->numComponents);
    idBranch.Append(id);

    return true;
}

//------------------------------------------------------------------------------
inline
bool 
nComponentIdList::RemoveComponent(const nComponentId & id)
{
    int index = this->GetComponentIndex(id);

    if (index != InvalidIndex)
    {
        nArray<nComponentId> & idBranch = this->compIdTree[index];
        int i(0);
        while(i < idBranch.Size())
        {
            n_assert(this->HasComponent(idBranch[i]));
            this->comps.Rem(idBranch[i].AsKey());
            ++i;
        }

        this->compIdTree.EraseQuick(index);
        this->numComponents--;

        // if remove components past the end of components -> change state to modified
        if (this->state == Declared)
        {
            this->state = Modified;
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
inline
int 
nComponentIdList::GetNumComponents() const
{
    return this->numComponents;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nComponentIdList::GetComponentIndex(const nComponentId & id) const
{
    int index;
    if (this->comps.Find(id.AsKey(), index))
    {
        return index;
    }

    return InvalidIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nComponentId & 
nComponentIdList::GetComponentIdByIndex(int index) const
{
    if (index >= 0 && index < this->numComponents)
    {
        return this->compIdTree[index].At(0);
    }

    return compIdInvalid;
}

//------------------------------------------------------------------------------
inline
bool 
nComponentIdList::HasComponent(const nComponentId & id) const
{
    return this->comps.HasKey(id.AsKey());
}

//------------------------------------------------------------------------------
inline
nComponentIdList::nStatus 
nComponentIdList::GetStatus() const
{
    return this->state;
}

//------------------------------------------------------------------------------
#endif
