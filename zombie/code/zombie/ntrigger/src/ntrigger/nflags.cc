//-----------------------------------------------------------------------------
//  nflags.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nflags.h"

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nFlags::nFlags()
    : flags(NULL)
{
#ifndef __NEBULA_NO_ASSERT__
    this->numFlags = 0;
#endif
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nFlags::~nFlags()
{
    if ( this->flags )
    {
        n_delete( this->flags );
    }
}

//-----------------------------------------------------------------------------
/**
    Init the class to hold at least numFlags flags

    All flags are set to false.
*/
void nFlags::Init( int numFlags )
{
    // Delete old flags
    if ( this->flags )
    {
        n_delete( this->flags );
    }

    // Create enough bitfields to contain all the flags
    int numBitfields( 1 + ((numFlags-1) >> BitsPerLocalIndex) );
    this->flags = n_new( Bitfield )[numBitfields];

    // Set to false all the flags
    for ( int i(0); i < numBitfields; ++i )
    {
        this->flags[i] = 0;
    }

#ifndef __NEBULA_NO_ASSERT__
    this->numFlags = numFlags;
#endif
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
