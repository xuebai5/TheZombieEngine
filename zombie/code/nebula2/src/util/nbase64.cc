//------------------------------------------------------------------------------
//  nbase64.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "util/nbase64.h"
#include <cstring>

//------------------------------------------------------------------------------
namespace
{
    /// Translation table as defined by RFC1113
    const char Base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char Base64Pad = '=';
}

//------------------------------------------------------------------------------
/**
    Get a base64 representation of given data (the result must be freed with n_free)
*/
void nBase64::Base256To64( int size, const char* source, char*& result )
{
    // If given an empty data buffer just return an empty string.
    if ( size == 0 )
    {
        result = (char*) n_malloc(1);
        result[0] = '\0';
        return;
    }

    n_assert( source );

    // Allocate space for base64 representation.
    // Each 24 bits group is stored in 4 human readable chars. Add padding if necessary.
    int size64 = ( size/3 + (size%3?1:0) ) * 4;
    result = (char*) n_malloc(size64 + 1);
    result[size64] = '\0';

    // Split 24 bits groups in 4 human readable ASCII codes.
    const unsigned char* data256 = reinterpret_cast<const unsigned char*>( source );
    char* data64 = result;
    for ( ; size >= 3; size -= 3, data256 += 3, data64 += 4 )
    {
        data64[0] = Base64Table[ data256[0] & 0x3f ];
        data64[1] = Base64Table[ ( (data256[0] & 0xc0) >> 2 ) | ( data256[1] & 0x0f ) ];
        data64[2] = Base64Table[ ( (data256[1] & 0xf0) >> 2 ) | ( data256[2] & 0x03 ) ];
        data64[3] = Base64Table[ (data256[2] & 0xfc) >> 2 ];
    }

    // Add padding if input bytes number isn't a multiple of 3.
    if ( size == 2 )
    {
        data64[0] = Base64Table[ data256[0] & 0x3f ];
        data64[1] = Base64Table[ ( (data256[0] & 0xc0) >> 2 ) | ( data256[1] & 0x0f ) ];
        data64[2] = Base64Table[ (data256[1] & 0xf0) >> 2 ];
        data64[3] = Base64Pad;
    }
    else if ( size == 1 )
    {
        data64[0] = Base64Table[ data256[0] & 0x3f ];
        data64[1] = Base64Table[ (data256[0] & 0xc0) >> 2 ];
        data64[2] = Base64Pad;
        data64[3] = Base64Pad;
    }
}

//------------------------------------------------------------------------------
/**
    Convert back data represented in base64 (the result must be freed with n_free)
*/
void nBase64::Base64To256( const char* source, int& size, char*& result )
{
    n_assert( source );

    // If given an empty string just return an empty data buffer.
    int size64( static_cast<int>( strlen(source) ) );
    if ( size64 == 0 )
    {
        size = 0;
        result = NULL;
        return;
    }

    n_assert( size64 % 4 == 0 );

    // Build decode table if not build yet
    static unsigned char base256Table[256];
    static bool base256TableIsInit = false;
    if ( !base256TableIsInit )
    {
        for ( unsigned char i(0); i < 64; ++i )
        {
            base256Table[ Base64Table[i] ] = i;
        }
        base256TableIsInit = true;
    }

    // Allocate space for restored data.
    // Each 4 6-bit bytes are packed into 24 bits groups. Remove padding if necessary.
    size = (size64/4) * 3 - ( source[size64-2]=='='? 2 : source[size64-1]=='='? 1 : 0 );
    result = (char*) n_malloc(size);

    // Decode and pack each 4 6-bit bytes into 24 bits groups.
    const char* data64 = source;
    unsigned char* data256 = reinterpret_cast<unsigned char*>( result );
    int size256;
    for ( size256 = size; size256 >= 3; size256 -= 3, data256 += 3, data64 += 4 )
    {
        // Decode bytes
        unsigned char group[4];
        for ( int i(0); i < 4; ++i )
        {
            group[i] = base256Table[ data64[i] ];
        }

        // Pack bytes
        data256[0] = group[0] | ( (group[1] & 0x30) << 2);
        data256[1] = (group[1] & 0x0f) | ( (group[2] & 0x3c) << 2 );
        data256[2] = (group[2] & 0x03) | ( (group[3] & 0x3f) << 2 );
    }

    // Convert back the last bytes discarding padding in the input string.
    if ( size256 > 0 )
    {
        // Decode bytes
        unsigned char group[3];
        for ( int i(0); i < 3; ++i )
        {
            group[i] = base256Table[ data64[i] ];
        }

        // Pack bytes
        data256[0] = group[0] | ( (group[1] & 0x30) << 2);
        if ( size256 == 2 )
        {
            data256[1] = (group[1] & 0x0f) | ( (group[2] & 0x3c) << 2 );
        }
    }
}
