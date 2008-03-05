//------------------------------------------------------------------------------
//  nnetutils.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkenet/nnetutils.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
int nNetUtils::minPort = 2301;
unsigned int nNetUtils::numPorts = 97; // prime near 100

nArray< const char * > nNetUtils::uniqueStringsArray;
nMapTableTypes< unsigned int >::NString nNetUtils::uniqueStringsMap;

//------------------------------------------------------------------------------
/**
    Use a hash algorithm for retrieve a number from the name string. The ports 
    numbers are assigned into range [minPort, minPort + numPorts) where minPort
    and numPorts are class variables. If no name is passed (name == NULL) the 
    returned port is equal minPort.
    @param name string with the name of the port
    @returns the integer value of the port
    @retval minPort if the name is NULL
    @retval (minport+value) with name
*/
int
nNetUtils::GetPort( const char *name )
{
    // if no name
    if( name == 0 )
    {
        return nNetUtils::minPort;
    }

    unsigned long value = 0;
    int c;

    while( 0 != (c = *name++) )
    {
        // value <- ( value + c ) * 33 MOD numPorts
        value = (( value + c )<< 5 ) + (value + c);
        if( value > nNetUtils::numPorts)
        {
            value %= nNetUtils::numPorts;
        }
    }

    return nNetUtils::minPort + (int)value;
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to insert
*/
size_t 
nNetUtils::PackTo( const char *msg, const nArg& value, const nArg * expected )
{
    const char * pointer( msg );

    char type = char( value.GetType() );

    if( expected )
    {
        n_assert( type == expected->GetType() );
    }
    else
    {
        pointer += nNetUtils::PackTo( pointer, unsigned char( type << 1 ) );
    }

    nObject * object = 0;

    switch( type )
    {
        case nArg::Void:
            break;

        case nArg::Int:
            pointer += nNetUtils::PackTo( pointer, value.GetI() );
            break;

        case nArg::Float:
            pointer += nNetUtils::PackTo( pointer, value.GetF() );
            break;

        case nArg::String:
            pointer += nNetUtils::PackUniqueTo( pointer, value.GetS() );
            break;

        case nArg::Bool:
            pointer +=  nNetUtils::PackTo( pointer, char( value.GetB() ) );
            break;

        case nArg::Char:
            pointer += nNetUtils::PackTo( pointer, value.GetC() );
            break;

        case nArg::Object:
            {
                nEntityObjectId entityId = nEntityObjectServer::IDINVALID;
                object = static_cast<nObject*>( value.GetO() );
                if( object )
                {
                    nNetworkManager * network = nNetworkManager::Instance();
                    bool valid;

                    valid = object->IsA( "nentityobject" );

                    n_assert2( valid, "We only can send nEntityObjects using network" );
                    if( valid && network )
                    {
                        nEntityObject * entity = static_cast<nEntityObject*>( object );

                        valid = network->IsRegisteredEntity( entity );
                        n_assert2( valid, "nEntityObject send using network is not Network entity" );
                        if( valid )
                        {
                            entityId = entity->GetId();
                        }
                    }
                }

                pointer += nNetUtils::PackTo( pointer, entityId );
            }
            break;

        case nArg::Pointer:
            n_assert2_always( "We can't send raw pointers using network" );
            break;

        case nArg::List:
            {
                nArg * args = 0;
                int num = value.GetL( args );
                n_assert( args );
                pointer += nNetUtils::PackTo( pointer, num );
                for( int i = 0 ; i < num ; ++i )
                {
                    pointer += nNetUtils::PackTo( pointer, args[ i ] );
                }
            }
            break;

        case nArg::Float3:
            pointer += nNetUtils::PackTo( pointer, value.GetF3().x );
            pointer += nNetUtils::PackTo( pointer, value.GetF3().y );
            pointer += nNetUtils::PackTo( pointer, value.GetF3().z );
            break;

        case nArg::Float4:
            pointer += nNetUtils::PackTo( pointer, value.GetF4().x );
            pointer += nNetUtils::PackTo( pointer, value.GetF4().y );
            pointer += nNetUtils::PackTo( pointer, value.GetF4().z );
            pointer += nNetUtils::PackTo( pointer, value.GetF4().w );
            break;
            
        case nArg::Matrix44:
            for( int j = 0 ; j < 4 ; ++j )
            {
                for( int i = 0 ; i < 4 ; ++i )
                {
                    pointer += nNetUtils::PackTo( pointer, value.GetM44().m[ j ][ i ] );
                }
            }
            break;

        default:
            n_assert_always();
    }

    return (pointer - msg);
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param value data to get
*/
size_t 
nNetUtils::UnpackFrom( const char *msg, nArg& value, const nArg * expected )
{
    const char * pointer( msg );
    int integer;
    float floatValue;
    char byte;
    nFloat3 vect3;
    nFloat4 vect4;
    matrix44 mat44;

    char type;

    if( expected )
    {
        type = char( expected->GetType() );
    }
    else
    {
        pointer += nNetUtils::UnpackFrom( pointer, type );
    }

    switch( type )
    {
        case nArg::Void:
            n_assert2_always( "Melkor say: How can do you send The Void over the network?" );
            break;

        case nArg::Int:
            pointer += nNetUtils::UnpackFrom( pointer, integer );
            value.SetI( integer );
            break;

        case nArg::Float:
            pointer += nNetUtils::UnpackFrom( pointer, floatValue );
            value.SetF( floatValue );
            break;

        case nArg::String:
            {
                const char * string;
                pointer += nNetUtils::UnpackUniqueFrom( pointer, string );
                value.SetS( string );
            }
            break;

        case nArg::Bool:
            pointer += nNetUtils::UnpackFrom( pointer, byte );
            value.SetB( byte != 0 );
            break;

        case nArg::Char:
            pointer += nNetUtils::UnpackFrom( pointer, byte );
            value.SetC( byte );
            break;

        case nArg::Object:
            {
                nEntityObjectId entityId;
                nObject * object = 0;

                pointer += nNetUtils::UnpackFrom( pointer, entityId );

                if( entityId != nEntityObjectServer::IDINVALID )
                {
                    nEntityObjectServer * entityServer = nEntityObjectServer::Instance();
                    nNetworkManager * network = nNetworkManager::Instance();

                    nEntityObject * entity = entityServer->GetEntityObject( entityId );

                    n_assert2( entity, "Invalid object send in the network" );
                    if( entity && network)
                    {
                        bool valid = network->IsRegisteredEntity( entity );
                        n_assert2( valid, "EntityObject is not a Network Entity" );
                        if( valid )
                        {
                            object = entity;
                        }
                    }
                }

                value.SetO( object );
            }
            break;

        case nArg::Pointer:
            n_assert2_always( "Gandalf say: You should not send pointers" );
            break;

        case nArg::List:
            {
                pointer += nNetUtils::UnpackFrom( pointer, integer );
                nArg * args = n_new_array( nArg, integer );
                n_assert( args );

                for( int i = 0 ; i < integer ; ++i )
                {
                    pointer += nNetUtils::UnpackFrom( pointer, args[ i ] );
                }

                value.SetL( args, integer );
            }
            break;

        case nArg::Float3:
            pointer += nNetUtils::UnpackFrom( pointer, vect3.x );
            pointer += nNetUtils::UnpackFrom( pointer, vect3.y );
            pointer += nNetUtils::UnpackFrom( pointer, vect3.z );
            value.SetF3( vect3 );
            break;

        case nArg::Float4:
            pointer += nNetUtils::UnpackFrom( pointer, vect4.x );
            pointer += nNetUtils::UnpackFrom( pointer, vect4.y );
            pointer += nNetUtils::UnpackFrom( pointer, vect4.z );
            pointer += nNetUtils::UnpackFrom( pointer, vect4.w );
            value.SetF4( vect4 );
            break;

        case nArg::Matrix44:
            for( int j = 0 ; j < 4 ; ++j )
            {
                for( int i = 0 ; i < 4 ; ++i )
                {
                    pointer += nNetUtils::UnpackFrom( pointer, mat44.m[ j ][ i ] );
                }
            }
            value.SetM44( mat44 );
            break;
    }
    return (pointer - msg);
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param string unique string to pack
    @returns the size of packed bytes
*/
size_t
nNetUtils::PackUniqueTo( const char *msg, const char * string )
{
    const char * pointer( msg );

    unsigned int strId;

    // if there is an Unique Identifier
    // [Byte 1 id][Byte 0 id]
    // if there isn't an Unique Identifier
    // [0x80][byte 0 string][byte 1 string] ... [byte n string][0x00]

    if( nNetUtils::GetUniqueStringId( string, strId ) )
    {
        // the unique string id is packed in Big Endian to allow the 
        // possibility of pack only one char more when the string is sended
        unsigned char car;
        car = char( ( strId >> 8 ) & 0x7f );
        pointer += nNetUtils::PackTo( pointer, car );
        car = char( strId & 0xff );
        pointer += nNetUtils::PackTo( pointer, car );
    }
    else
    {
        pointer += nNetUtils::PackTo( pointer, nNetUtils::NO_UNIQUE_STRING );
        pointer += nNetUtils::PackArrayTo( pointer, string, strlen( string ) + 1 );
    }

    return (pointer - msg);
}

//------------------------------------------------------------------------------
/**
    @param msg byte buffer
    @param string unique string to unpack
    @returns the size of unpacked bytes
*/
size_t
nNetUtils::UnpackUniqueFrom( const char * msg, const char * &string )
{
    const char * pointer( msg );

    if( ((*pointer) & nNetUtils::NO_UNIQUE_STRING) != 0 )
    {
        string = ++pointer;
        pointer += strlen( string ) + 1;
    }
    else
    {
        unsigned char car;
        unsigned int strId = 0;
        pointer += nNetUtils::UnpackFrom( pointer, car );
        strId |= unsigned int( car & 0x7f ) << 8;
        pointer += nNetUtils::UnpackFrom( pointer, car );
        strId |= unsigned int( car );

        string = nNetUtils::GetUniqueString( strId );
    }

    return (pointer - msg);
}

//------------------------------------------------------------------------------
/**
    @param strId id for the unique string
    @returns the unique string
*/
const char *
nNetUtils::GetUniqueString( unsigned int strId )
{
    if( ( strId >= 0 ) && ( int( strId ) < uniqueStringsArray.Size() ) )
    {
        return uniqueStringsArray[ strId ];
    }

    NLOG( network, ( 0, "NETWORK: ERROR: Invalid Unique Identifier" ) );
    return "";
}

//------------------------------------------------------------------------------
/**
    @param string string to check for unique id
    @param strId [OUT] identifier of 
    @returns true if there is an unique string
*/
bool
nNetUtils::GetUniqueStringId( const char * string, unsigned int & strId )
{
    unsigned int * ptrData = 0;

    ptrData = uniqueStringsMap[ nString( string ) ];

    if( ptrData )
    {
        strId = *ptrData;

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @param string string to insert in unique table
*/
void
nNetUtils::InsertUniqueString( const char * string )
{
    if( uniqueStringsArray.Size() < nNetUtils::MAX_UNIQUE_STRING )
    {
        unsigned int * ptrData = 0;

        nString unique( string );

        ptrData = uniqueStringsMap[ unique ];

        if( ! ptrData )
        {
            unsigned int index = uniqueStringsArray.Size();
            const nString * key = uniqueStringsMap.Add( unique, &index );
            n_assert( key );
            if( key )
            {
                uniqueStringsArray.Append( key->Get() );
            }
        }
    }
}

//------------------------------------------------------------------------------
