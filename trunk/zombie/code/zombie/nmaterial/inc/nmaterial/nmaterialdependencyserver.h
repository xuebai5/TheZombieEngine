#ifndef N_MATERIALDEPENDENCYSERVER_H
#define N_MATERIALDEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file nmaterialdependencyserver.h
   @author ma.garcias <ma.garcias@yahoo.es>
   @class nMaterialDependencyServer
   @ingroup NebulaMaterialSystem

   @brief Declaration of nMaterialDependencyServer class
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ndependencyserver.h"

//------------------------------------------------------------------------------
class nMaterialDependencyServer : public nDependencyServer
{
public:
    /// constructor
    nMaterialDependencyServer();
    /// destructor
    virtual ~nMaterialDependencyServer();

    /// Get a object from a file if it isn't loaded
    virtual nObject *GetObject(const nString& filename, const nString& command);

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath(nObject *const object, const nString& command, nString& path);
};

//------------------------------------------------------------------------------
#endif // N_MATERIALDEPENDENCYSERVER_H
