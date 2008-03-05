#ifndef NCNETWORKVEHICLE_H
#define NCNETWORKVEHICLE_H
//------------------------------------------------------------------------------
/**
    @class
    @ingroup

    Description of the porpouse of the class

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncnetwork/ncnetwork.h"

//------------------------------------------------------------------------------
class ncPhyVehicle;

//------------------------------------------------------------------------------
class ncNetworkVehicle : public ncNetwork
{

    NCOMPONENT_DECLARE(ncNetworkVehicle,ncNetwork);

public:
    /// constructor
    ncNetworkVehicle();
    /// destructor
    ~ncNetworkVehicle();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// tell the update data from / to the network
    virtual void UpdateNetwork( nstream & buffer );
    /// init an instance with network data
    virtual bool InitFromNetwork( nstream & buffer );

private:

    /// stores the vehicle
    ncPhyVehicle* vehicle;

};

#endif//NCNETWORKVEHICLE_H
