#ifndef NCNETWORKCLASS_H
#define NCNETWORKCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncNetworkClass
    @ingroup Entities

    Component Class that control the network behaviour.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncNetworkClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncNetworkClass,nComponentClass);

public:
    /// constructor
    ncNetworkClass();
    /// destructor
    ~ncNetworkClass();
private:

};

//------------------------------------------------------------------------------
#endif//NCNETWORKCLASS_H
