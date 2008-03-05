#ifndef N_PERSISTHELPERSUBENTITY_H
#define N_PERSISTHELPERSUBENTITY_H
//------------------------------------------------------------------------------
/**
    @class ncPersistHelperSubEntity 
    @author Mateu Batle i Sastre

    The persistence OID is the representation of the Object identifier which
    is saved on persistence. 
*/
//------------------------------------------------------------------------------
#include "entity/ncpersisthelper.h"

//------------------------------------------------------------------------------
class ncPersistHelperSubEntity : public ncPersistHelper
{

    NCOMPONENT_DECLARE(ncPersistHelper, nComponentObject);

public:
    /// Constructor
    ncPersistHelperSubEntity();
    /// Destructor
    ~ncPersistHelperSubEntity();

    /// return normal OID from persistence OID
    virtual nEntityObjectId FromPersistenceOID( nEntityObjectId ) const;
    /// return persistence OID from normal OID
    virtual nEntityObjectId ToPersistenceOID( nEntityObjectId ) const;

};

#endif // N_PERSISTHELPERSUBENTITY_H
