#ifndef NCNETWORKTRANSFORM_H
#define NCNETWORKTRANSFORM_H
//------------------------------------------------------------------------------
/**
    @class ncNetworkTransform
    @ingroup Entities
    
    Component Object that control the network update when only update its 
    transform component.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncnetwork/ncnetwork.h"

//------------------------------------------------------------------------------
class ncTransform;

//------------------------------------------------------------------------------
class ncNetworkTransform : public ncNetwork
{

    NCOMPONENT_DECLARE(ncNetworkTransform,ncNetwork);

public:
    /// constructor
    ncNetworkTransform();
    /// destructor
    ~ncNetworkTransform();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// tell the update data from / to the network
    virtual void UpdateNetwork( nstream & buffer );

private:

    ncTransform * transformComp;

};

//------------------------------------------------------------------------------
#endif//NCNETWORKTRANSFORM_H
