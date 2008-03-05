#ifndef N_CONJURERGFXSERVER_H
#define N_CONJURERGFXSERVER_H
//------------------------------------------------------------------------------
/**
    @class nConjurerGfxServer
    @ingroup NebulaConjurerEditor

    Graphics server for Conjurer.

    Same as nD3D9Server, but using a window handler that asks the user for
    confirmation when closing the window.

    (C) 2005 Conjurer Services, S.A.
*/
#include "gfx2/nd3d9server.h"

//------------------------------------------------------------------------------
class nConjurerGfxServer : public nD3D9Server
{
public:
    /// constructor
    nConjurerGfxServer();

protected:
    /// create window handler
    virtual nD3D9WindowHandler *CreateWindowHandler();
};

#endif
