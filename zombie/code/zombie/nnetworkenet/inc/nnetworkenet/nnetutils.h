#ifndef N_NETUTILS_H
#define N_NETUTILS_H
//------------------------------------------------------------------------------
/**
    @class nNetUtils
    @ingroup NNetworkEnetContribModule
    
    A static class with utils method for Network.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include <cstring>
#include "kernel/narg.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nNetUtils {
public:
    /// Type of send messages
    enum MessageType
    {
        /// message not guaranteed to arrive but arrives in order
        Unreliable = 0, 
        /// message guaranteed arrival but are not guaranteed to be in order
        Unordered,
        /// message guaranteed both arrival and to be in order
        Ordered
    };

    /// conexion channels configuration
    static const int UNRELIABLE_CHANNEL = 0;
    static const int ORDERED_CHANNEL = UNRELIABLE_CHANNEL + 1;
    static const int UNORDERED_CHANNELS = 3;
    static const int MIN_UNORDERED = ORDERED_CHANNEL + 1;
    static const int MAX_UNORDERED = MIN_UNORDERED + UNORDERED_CHANNELS - 1;
    static const int NUM_CHANNELS = 2 + UNORDERED_CHANNELS;

    /// Return the port number of a port name
    static int GetPort( const char *name );
    /// Set the minimun port retrieve for GetPort function
    static void SetMinPort( int port );
    /// Set the number of ports in range of GetPort function
    static void SetNumPorts( int num );
    /// put a nArg in a byte buffer
    static size_t PackTo( const char *msg, const nArg& value, const nArg * expected = 0 );
    /// put a unique string in a byte buffer
    static size_t PackUniqueTo( const char *msg, const char * string );
    /// put a value in a byte buffer
    template<typename T> static size_t PackTo( const char *msg, const T& value);
    /// put an array of values in a byte buffer
    template<typename T> static size_t PackArrayTo( const char *msg, const T* value, size_t size);
    /// get a nArg from a byte buffer
    static size_t UnpackFrom( const char *msg, nArg& value, const nArg * expected = 0 );
    /// get a unique string from a byte buffer
    static size_t UnpackUniqueFrom( const char *msg, const char * &string );
    /// get a value from a byte buffer
    template<typename T> static size_t UnpackFrom( const char *msg, T& value);
    /// get an array of values from a byte buffer
    template<typename T> static size_t UnpackArrayFrom( const char *msg, T* value, size_t size);

    /// insert a string in the unique string table
    static void InsertUniqueString( const char * string );

private:
    static int minPort;
    static unsigned int numPorts;

    static const unsigned char NO_UNIQUE_STRING = 0x80;
    static const unsigned int MAX_UNIQUE_STRING = 0x7FFF;

    static nArray< const char * > uniqueStringsArray;
    static nMapTableTypes< unsigned int >::NString uniqueStringsMap;

    /// get an unique string
    static const char * GetUniqueString( unsigned int strId );
    /// get the identifier for an unique string
    static bool GetUniqueStringId( const char * string, unsigned int & strId );
};

//------------------------------------------------------------------------------
/**
    @param port new minPort value
*/
inline
void
nNetUtils::SetMinPort( int port )
{
    nNetUtils::minPort = port;
}

//------------------------------------------------------------------------------
/**
    @param num new numPorts value
*/
inline
void
nNetUtils::SetNumPorts( int num )
{
    nNetUtils::numPorts = num;
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to insert
*/
template<typename T>
inline static 
size_t nNetUtils::PackTo( const char *msg, const T& value)
{
    memcpy( (void*)msg, &value, sizeof( T ) );
    return sizeof( T );
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to insert
    @param size number of elements in the array
*/
template<typename T>
inline static 
size_t nNetUtils::PackArrayTo( const char *msg, const T* value, size_t size)
{
    memcpy( (void*)msg, value, sizeof( T )*size );
    return ( sizeof( T )*size );
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to get
*/
template<typename T>
inline static 
size_t nNetUtils::UnpackFrom( const char *msg, T& value)
{
    memcpy( &value, msg, sizeof( T ));
    return sizeof( T );
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to get
    @param size number of elements in the array
*/
template<typename T>
inline static 
size_t nNetUtils::UnpackArrayFrom( const char *msg, T* value, size_t size)
{
    memcpy( value, msg, sizeof( T )*size);
    return ( sizeof( T )*size );
}

//------------------------------------------------------------------------------
#endif//N_NETUTILS_H
