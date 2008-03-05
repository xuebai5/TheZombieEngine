#ifndef N_OBJECTINSTANCER_H
#define N_OBJECTINSTANCER_H
//------------------------------------------------------------------------------
/**
    @class nObjectInstancer
    @ingroup NebulaEntitySystem
    @brief Same as object array, but assigning new ids for entity objects
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "misc/nobjectarray.h"
#include "entity/nentityobjectserver.h"
//------------------------------------------------------------------------------
class nObjectInstancer : public nObjectArray
{
public:
    /// constructor
    nObjectInstancer();
    /// destructor
    virtual ~nObjectInstancer();

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// set the entity object type of the instancer
    void SetEntityObjectType(nEntityObjectServer::nEntityObjectType);

    /// INTERNAL, DONT USE DIRECTLY, used for persistence (create object)
    virtual nObject * BeginNewObject(const char * objClass, const char * objName);

protected:
    nEntityObjectServer::nEntityObjectType entityObjectType;
};

//------------------------------------------------------------------------------
inline
nObjectInstancer::nObjectInstancer() :
    entityObjectType(nEntityObjectServer::Local)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nObjectInstancer::~nObjectInstancer()
{
    /// empty
}

//------------------------------------------------------------------------------
#endif N_OBJECTINSTANCER_H
