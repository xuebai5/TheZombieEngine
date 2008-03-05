#ifndef N_SCENESURFACEDEPENDENCYSERVER_H
#define N_SCENESURFACEDEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file nscenesurfacedependencyserver.h
   @author ma.garcias <ma.garcias@yahoo.es>
   @brief Declaration of nSceneSurfaceDependencyServer class
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ndependencyserver.h"

//------------------------------------------------------------------------------
class nSceneSurfaceDependencyServer : public nDependencyServer
{
public:
    /// constructor
    nSceneSurfaceDependencyServer();
    /// destructor
    virtual ~nSceneSurfaceDependencyServer();

    /// Init the dependency server
    virtual void Init();
    /// Get a object from a file if it isn't loaded
    virtual nObject *GetObject(const nString& filename, const nString& command);

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath(nObject *const object, const nString& command, nString& path);

private:
    uint counter;
};

//------------------------------------------------------------------------------
#endif // N_SCENESURFACEDEPENDENCYSERVER_H
