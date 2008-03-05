#ifndef NC_TRANSFORMCLASS_H
#define NC_TRANSFORMCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncTransformClass
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component class to hold the entity global transform.
    Also, it synchronizes the global transform across several components.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncTransformClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncTransformClass, nComponentClass);

public:
    /// constructor
    ncTransformClass();
    /// destructor
    ~ncTransformClass();

};

//------------------------------------------------------------------------------
#endif
