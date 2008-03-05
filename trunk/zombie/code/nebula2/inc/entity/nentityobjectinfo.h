#ifndef N_ENTITYOBJECTINFO_H
#define N_ENTITYOBJECTINFO_H
//------------------------------------------------------------------------------
/**
    @class nEntityObjectInfo
    @ingroup NebulaEntitySystem
    @brief Entity object information
    @author Mateu Batle

    Holds information for an entity object used by the entity object server.

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class nEntityObjectInfo
{
public:
    // constructor
    nEntityObjectInfo();
    /// copy constructor
    nEntityObjectInfo(const nEntityObjectInfo & obj);
    /// destructor
    ~nEntityObjectInfo();

    /// assignment operator 
    nEntityObjectInfo & operator=(const nEntityObjectInfo & rhs);
    /// set the entity object
    void SetEntityObject(nEntityObject * obj);
    /// get the entity object
    nEntityObject * GetEntityObject() const;

    /// get dirty state
    bool GetDirty() const;
    /// set dirty state
    void SetDirty(bool d);

    /// get delete status of entity object 
    bool IsDeleted() const;
    /// Delete object
    void Delete();
    /// Undelete object
    void Undelete();

private:
    // pointer to the entity object
    nRef<nEntityObject> refObject;
    // dirty flag used to know if save or not
    bool dirty;
    // deleted flag to know if delete or not
    bool deleted;
};

//--------------------------------------------------------------------
#endif //N_ENTITYOBJECTINFO_H
