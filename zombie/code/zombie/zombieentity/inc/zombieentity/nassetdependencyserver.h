#ifndef N_ASSETDEPENDENCYSERVER_H
#define N_ASSETDEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file nassetdependencyserver.h
   @author ma.garcias <ma.garcias@yahoo.es>
   @brief Declaration of nAssetDependencyServer class
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ndependencyserver.h"

//------------------------------------------------------------------------------
class nAssetDependencyServer : public nDependencyServer
{
public:
    /// constructor
    nAssetDependencyServer();
    /// destructor
    virtual ~nAssetDependencyServer();

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath(nObject *const object, const nString& command, nString& path);

private:
    //...
};

//------------------------------------------------------------------------------
#endif // N_ASSETDEPENDENCYSERVER_H
