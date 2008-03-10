/** 
 @file  testing.h
 @brief ENet Testing
*/
#ifndef __ENET_TESTING_H__
#define __ENET_TESTING_H__

#include "enet/enet.h"

int  enet_testing_send(ENetHost * host, ENetPeer * peer );
void enet_testing_lagged( );

#endif /* __ENET_TESTING_H__ */