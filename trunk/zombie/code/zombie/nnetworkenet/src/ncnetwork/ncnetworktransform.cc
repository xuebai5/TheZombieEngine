//------------------------------------------------------------------------------
//  ncnetworktransform.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "ncnetwork/ncnetworktransform.h"

#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNetworkTransform,ncNetwork);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNetworkTransform)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncNetworkTransform::ncNetworkTransform():
    transformComp( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncNetworkTransform::~ncNetworkTransform()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncNetworkTransform::InitInstance( nObject::InitInstanceMsg /*initType*/ )
{
    this->transformComp = this->GetComponent<ncTransform>();
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be updated
*/
void
ncNetworkTransform::UpdateNetwork( nstream & buffer )
{
    if( this->transformComp )
    {
        vector3 position;
        vector3 euler;

        if( buffer.GetWrite() )
        {
            position = this->transformComp->GetPosition();
            euler = this->transformComp->GetEuler();
        }

        buffer.UpdateVector3( position );
        buffer.UpdateVector3( euler );

        if( ! buffer.GetWrite() )
        {
            this->transformComp->SetPosition( position );
            this->transformComp->SetEuler( euler );
        }
    }
}

//------------------------------------------------------------------------------
