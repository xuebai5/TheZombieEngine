/** 
 @file  testing.c
 @brief ENet Testing functions to include lag and random errors
*/
#include "enet/testing.h"
#include "enet/enet.h"
#include "math.h"

static float enet_error    = 0.0;
static int   enet_lag_mean = 0;
static int   enet_lag_var  = 0;

typedef struct _LaggedNode
{
    ENetSocket  socket;
    ENetAddress address;
    size_t      bufferCount;
    ENetBuffer  buffers [ENET_BUFFER_MAXIMUM];
    int         lag;
}LaggedNode;

typedef struct _LaggedList
{
    LaggedNode              packet;
    struct _LaggedList *    next;
}LaggedList;

LaggedList *laggedBegin = 0;

void 
enet_testing_error( float val)
{
    enet_error = val;
}

void 
enet_testing_lag( int mean, int variance)
{
    enet_lag_mean = mean;
    enet_lag_var  = variance;
}

int
enet_testing_generate_lag()
{
    // Knuth, The Art of Computer Programming, Section 3.4.1
    float u1, u2, v1, v2, s, x1;
    int lag;
    for(;;)
    {
        u1 = ((float) rand()) / RAND_MAX;
        u2 = ((float) rand()) / RAND_MAX;
        v1 = 2*u1 - 1;
        v2 = 2*u2 - 1;
        s  = v1 * v1 + v2 * v2;
        if( s <= 1.0 )
        {
            break;
        }
    }
    x1 = v1 * (float) sqrt(-2.0 * log(s) / s);

    lag = (int)( enet_lag_mean + x1 * enet_lag_var );
    if(lag < 0 )
    {
        return 0;
    }
    return lag;
}

int 
enet_testing_send(ENetHost * host, ENetPeer * peer )
{
    if( ( ((float)rand()) / RAND_MAX ) > enet_error )
    {
        if( enet_lag_mean == 0 && enet_lag_var == 0 )
        {
            enet_socket_send (host -> socket, & peer -> address, host -> buffers, host -> bufferCount);
        }
        else
        {
            // create the Lagged Node
            unsigned int i;
            LaggedList *pLagged;
            LaggedList *pNext, *pBefore=0;
            pLagged = (LaggedList*)malloc(sizeof(LaggedList));

            // init LaggedNode packet vars
            pLagged->packet.socket = host->socket;
            pLagged->packet.address.host = peer->address.host;
            pLagged->packet.address.port = peer->address.port;
            pLagged->packet.bufferCount = host->bufferCount;
            for( i=0 ; i<host->bufferCount ; ++i )
            {
                pLagged->packet.buffers[i].dataLength = host->buffers[i].dataLength;
                pLagged->packet.buffers[i].data = malloc( host->buffers[i].dataLength );
                memcpy( 
                    pLagged->packet.buffers[i].data, 
                    host->buffers[i].data,
                    host->buffers[i].dataLength );
            }
            pLagged->packet.lag = enet_testing_generate_lag() ;

            // insert node in order
            pNext = laggedBegin;
            while( pNext && pNext->packet.lag < pLagged->packet.lag )
            {
                pBefore = pNext;
                pNext = pNext->next;
            }
            pLagged->next = pNext;
            if( pBefore )
            {
                pBefore->next = pLagged;
            }
            else
            {
                laggedBegin = pLagged;
            }
        }
    }
    return 1;
}

void
enet_testing_lagged( )
{
    static enet_uint32 time = 0;
    enet_uint32 time_inc;
    LaggedList *pNode;
    unsigned int i, ok;

    if( time == 0)
    {
        time = enet_time_get();
    }

    time_inc = enet_time_get() - time;
    time = enet_time_get();

    // send Lagged Packets with Lag <= 0
    ok = 1;
    while( laggedBegin && ok )
    {
        laggedBegin->packet.lag -= time_inc;
        if( laggedBegin->packet.lag <= 0 )
        {
            enet_socket_send (
                laggedBegin->packet.socket, 
                &laggedBegin->packet.address,
                laggedBegin->packet.buffers, 
                laggedBegin->packet.bufferCount);
            // destroy packet
            for( i=0 ; i < laggedBegin->packet.bufferCount ; ++i )
            {
                free( laggedBegin->packet.buffers[i].data );
            }
            // delete Node
            pNode = laggedBegin;
            laggedBegin = laggedBegin->next;
            free( pNode );
        }
        else
        {
            ok = 0;
        }
    }

    // if there are nodes
    if( laggedBegin )
    {
        // begin with the first node that haven't
        // update the lag
        pNode = laggedBegin->next;
    }
    else
    {
        return;
    }

    // decrement Lag to the rest nodes
    while( pNode )
    {
        pNode->packet.lag -= time_inc;
        pNode = pNode->next;
    }
}
