//------------------------------------------------------------------------------
//  ncNetworkVehicle.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsnetwork.h"

#include "rnsnetwork/ncNetworkVehicle.h"

#include "nphysics/ncphyvehicle.h"

#include "nphysics/ncphywheel.h"

#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNetworkVehicle,ncNetwork);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNetworkVehicle)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncNetworkVehicle::ncNetworkVehicle():
    vehicle( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncNetworkVehicle::~ncNetworkVehicle()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncNetworkVehicle::InitInstance(nObject::InitInstanceMsg initType)
{
    this->vehicle = this->GetComponentSafe<ncPhyVehicle>();

    ncNetwork::InitInstance( initType );
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be updated
*/
void
ncNetworkVehicle::UpdateNetwork( nstream & /*buffer*/ )
{
#if 0
    if( this->vehicle )
    {
        vector3 v3data;

        if( buffer.GetWrite() )
        {
            /*
            /// vehicle position
            this->vehicle->GetPosition( v3data );

            buffer.UpdateVector3( v3data );
            
            /// vehicle orientation
            matrix33 orientation;

            this->vehicle->GetOrientation( orientation );

            v3data = orientation.to_euler();

            buffer.UpdateVector3( v3data );

            /// vehicle linear speed
            this->vehicle->GetLinearVelocity(v3data);

            buffer.UpdateVector3( v3data );

            /// vehicle angular speed
            this->vehicle->GetAngularVelocity(v3data);

            buffer.UpdateVector3( v3data );
*/
            for( int index(0); index < 4; ++index )
            {
                ncPhyWheel* wheel((*this->vehicle)[ index ]->GetWheelObject());

/*                wheel->GetPosition( v3data );

                buffer.UpdateVector3( v3data );

                matrix33 orientation;

                wheel->GetOrientation( orientation );

                v3data = orientation.to_euler();

                buffer.UpdateVector3( v3data );
*/
                v3data = wheel->GetComponent<ncTransform>()->GetPosition();

                buffer.UpdateVector3 (v3data);

                v3data = wheel->GetComponent<ncTransform>()->GetEuler();
                
                buffer.UpdateVector3 (v3data);


                wheel->GetLinearVelocity(v3data);

                buffer.UpdateVector3( v3data );

                wheel->GetAngularVelocity(v3data);

                buffer.UpdateVector3( v3data );                
            }
        }
        else
        {
            /// vehicle position
/*
            buffer.UpdateVector3( v3data );

            this->vehicle->SetPosition( v3data );
            
            /// vehicle orientation
            buffer.UpdateVector3( v3data );

            this->vehicle->SetRotation( v3data.x, v3data.y, v3data.z );


            /// vehicle linear speed
            buffer.UpdateVector3( v3data );

            this->vehicle->SetLinearVelocity(v3data);

            /// vehicle angular speed
            buffer.UpdateVector3( v3data );
         
            this->vehicle->SetAngularVelocity(v3data);
*/
            for( int index(0); index < 4; ++index )
            {
                ncPhyWheel* wheel((*this->vehicle)[ index ]->GetWheelObject());

                /*
                buffer.UpdateVector3( v3data );

                wheel->SetPosition( v3data );

                buffer.UpdateVector3( v3data );

                wheel->SetRotation( v3data.x, v3data.y, v3data.z );
*/

                buffer.UpdateVector3 (v3data);

                wheel->GetComponent<ncTransform>()->SetPosition(v3data);

                buffer.UpdateVector3 (v3data);

                wheel->GetComponent<ncTransform>()->SetEuler(v3data);                





                buffer.UpdateVector3( v3data );

                buffer.UpdateVector3( v3data );
                
                wheel->SetLinearVelocity(v3data);

                buffer.UpdateVector3( v3data );

                wheel->SetAngularVelocity(v3data);
            }
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be initialized
*/
bool
ncNetworkVehicle::InitFromNetwork( nstream & buffer )
{
    this->UpdateNetwork( buffer );

    return true;
}

//------------------------------------------------------------------------------
