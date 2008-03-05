#ifndef NCNETWORKPLAYER_H
#define NCNETWORKPLAYER_H
//------------------------------------------------------------------------------
/**
    @class
    @ingroup

    Description of the porpouse of the class

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsnetwork/ncnetworkliving.h"

//------------------------------------------------------------------------------
class ncGameplayPlayer;

//------------------------------------------------------------------------------
class ncNetworkPlayer : public ncNetworkLiving
{

    NCOMPONENT_DECLARE(ncNetworkPlayer,ncNetworkLiving);

public:
    /// constructor
    ncNetworkPlayer();
    /// destructor
    ~ncNetworkPlayer();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// tell the update data from / to the network
    virtual void UpdateNetwork( nstream & buffer );
    /// init an instance with network data
    virtual bool InitFromNetwork( nstream & buffer );

private:

    /// Get the data for duplicate the instance in network
    void CopyTo( nstream & data );
    /// Initialize the instance with a data buffer
    void InitializeWith( nstream & data );

    ncGameplayPlayer * playerComp;

};

#endif//NCNETWORKPLAYER_H
