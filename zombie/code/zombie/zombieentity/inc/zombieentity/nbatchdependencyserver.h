#ifndef N_BATCHDEPENDENCYSERVER_H
#define N_BATCHDEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file nbatchdependencyserver.h
   @author ma.garcias <ma.garcias@yahoo.es>
   @brief Declaration of nBatchDependencyServer class
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ndependencyserver.h"

//------------------------------------------------------------------------------
class nBatchDependencyServer : public nDependencyServer
{
public:
    /// constructor
    nBatchDependencyServer();
    /// destructor
    virtual ~nBatchDependencyServer();

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath(nObject *const object, const nString& command, nString& path);

private:
    //...
};

//------------------------------------------------------------------------------
#endif /*N_BATCHDEPENDENCYSERVER_H*/
