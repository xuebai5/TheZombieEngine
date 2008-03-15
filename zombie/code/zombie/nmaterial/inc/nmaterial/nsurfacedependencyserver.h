#ifndef N_SURFACEDEPENDENCYSERVER_H
#define N_SURFACEDEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file nmatsurfacedependencyserver.h
   @author ma.garcias <ma.garcias@yahoo.es>
   @brief Declaration of nMatSurfaceDependencyServer class
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nscenesurfacedependencyserver.h"

//------------------------------------------------------------------------------
class nSurfaceDependencyServer : public nSceneSurfaceDependencyServer
{
public:
    /// constructor
    nSurfaceDependencyServer();
    /// destructor
    virtual ~nSurfaceDependencyServer();

    /// Get a object from a file if it isn't loaded
    virtual nObject *GetObject(const nString& filename, const nString& command);

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath(nObject *const object, const nString& command, nString& path);

private:
    /// find if identical surface already loaded
    nRoot* FindSameSurface(nRoot* surface, const nString& command);
};

//------------------------------------------------------------------------------
#endif // N_SURFACEDEPENDENCYSERVER_H
