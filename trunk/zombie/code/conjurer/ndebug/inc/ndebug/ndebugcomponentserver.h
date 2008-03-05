#ifndef NDEBUGCOMPONENTSERVER_H
#define NDEBUGCOMPONENTSERVER_H
//------------------------------------------------------------------------------
/**
   @file ndebugcomponentserver.h
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
#ifndef NGAME

#include "kernel/nroot.h"

//------------------------------------------------------------------------------
/**
    @class nDebugComponentServer
    @ingroup Entities

    @brief server that contains a list of Editor Component objects.
*/
class ncEditor;
class nGfxServer2;

class nDebugComponentServer : public nRoot
{
public:
    /// singleton instance
    static nDebugComponentServer * Instance();

    /// constructor
    nDebugComponentServer();
    /// destructor
    ~nDebugComponentServer();

    /// add an editor component object
    virtual void Add( ncEditor * const object );

    /// remove an editor component object
    virtual void Remove( ncEditor * const object );

#ifndef __ZOMBIE_EXPORTER__
    /// draw the components
    virtual void Draw( nGfxServer2 * const gfxServer );
#endif

    /// Set drawing flags
    void SetDrawingFlags( int flags );

    /// Get drawing flags
    int GetDrawingFlags();

private:
    static nDebugComponentServer* Singleton;

    nArray<ncEditor*> objects;

    unsigned int drawFlags;
};

//------------------------------------------------------------------------------
/**
    @returns the nDebugComponentServer instance
*/
inline
nDebugComponentServer *
nDebugComponentServer::Instance()
{
#ifndef __ZOMBIE_EXPORTER__
    n_assert(Singleton);
#endif // __ZOMBIE_EXPORTER__
    return Singleton;
}

#endif//!NGAME

#endif//NDEBUGCOMPONENTSERVER_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
