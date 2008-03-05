#ifndef N_NCPERSISTHELPER_H
#define N_NCPERSISTHELPER_H
//------------------------------------------------------------------------------
/**
    @class ncPersistHelper
    @ingroup NebulaEntitySystem
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/ncomponentobject.h"

//------------------------------------------------------------------------------
class ncPersistHelper : public nComponentObject
{

    NCOMPONENT_DECLARE(ncPersistHelper, nComponentObject);

public:
    /// Constructor
    ncPersistHelper();
    /// Destructor
    ~ncPersistHelper();

    /// return normal OID from persistence OID
    virtual nEntityObjectId FromPersistenceOID( nEntityObjectId ) const;
    /// return persistence OID from normal OID
    virtual nEntityObjectId ToPersistenceOID( nEntityObjectId ) const;

};

#endif
