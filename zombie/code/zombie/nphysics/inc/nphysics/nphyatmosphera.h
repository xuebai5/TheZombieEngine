#ifndef N_PHYATMOSPHERA_H
#define N_PHYATMOSPHERA_H

//-----------------------------------------------------------------------------
/**
    @class nPhyAtmosphera
    @ingroup NebulaPhysicsSystem
    @brief An abstract representation of an atmosphera

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "nphysics/nphysicsconfig.h"
#include "util/nkeyarray.h"
//-----------------------------------------------------------------------------
class nPhyRigidBody;
//-----------------------------------------------------------------------------

class nPhyAtmosphera
{
public:
    /// default constructor
    nPhyAtmosphera();

    /// destructor
    ~nPhyAtmosphera();

    /// sets the density of the atmosphere 1 by default
    void SetDensity( phyreal density );

    /// sets the wind
    void SetWind( const vector3& wind );

    /// gets the density
    phyreal GetDensity() const;

    /// gets the wind
    void GetWind( vector3& wind ) const;

    /// set wind origin
    void SetWindOrigin( const vector3& windorigin );

    /// get wind origin
    void GetWindOrigin( vector3& windorigin ) const;

    /// process the atmosphera effects
    void Process( const nKeyArray< nPhyRigidBody* >& bodies );

private:
    
    /// stores the wind
    vector3 windAtmosphera;

    /// stores the wind origin
    vector3 windAtmoespheraOrigin;

    /// density
    phyreal densityAtmosphera;

    /// stores the air resistance constant
    static const phyreal airResistance;
};

#endif 
