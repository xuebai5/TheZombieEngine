#ifndef NCNETWORKLIVING_H
#define NCNETWORKLIVING_H
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
class ncGameplayLiving;
class ncAIMovEngine;
class ncTransform;
class ncLogicAnimator;
class ncGameplayPlayer;

//------------------------------------------------------------------------------
class ncNetworkLiving : public ncNetwork
{

    NCOMPONENT_DECLARE(ncNetworkLiving,ncNetwork);

public:
    /// constructor
    ncNetworkLiving();
    /// destructor
    ~ncNetworkLiving();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// frame update of component
    virtual void Update( float frameTime );
    /// tell the update data from / to the network
    virtual void UpdateNetwork( nstream & buffer );
    /// init an instance with network data
    virtual bool InitFromNetwork( nstream & buffer );

private:

    /// Get the data for duplicate the instance in network
    void CopyTo( nstream & data );
    /// Initialize the instance with a data buffer
    void InitializeWith( nstream & data );

    ncGameplayLiving * livingComp;
    ncAIMovEngine * movEngine;
    ncTransform * transform;
    ncLogicAnimator * animator;
    ncGameplayPlayer * playerComp;

    vector3 finalPosition;
    quaternion finalRotation;
    bool updatePosition;
    bool updateRotation;

    static const float minimumDistance;
    static const float maximumDistance;
    static const float minimumAngle;
    static const float maximumAngle;
    static const float lerpFactor;

};

#endif//NCNETWORKLIVING_H
