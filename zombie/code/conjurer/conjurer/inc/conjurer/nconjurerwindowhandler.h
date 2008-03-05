#ifndef N_CONJURERWINDOWHANDLER_H
#define N_CONJURERWINDOWHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nConjurerWindowHandler
    @ingroup NebulaConjurerEditor

    Window event handler for Conjurer.

    Same as nD3D9WindowHandler, but asking the user for confirmation when
    closing the window.

    (C) 2005 Conjurer Services, S.A.
*/
#include "gfx2/nd3d9windowhandler.h"

class nD3D9Server;

//------------------------------------------------------------------------------
class nConjurerWindowHandler : public nD3D9WindowHandler
{
public:
    /// constructor
    nConjurerWindowHandler(nD3D9Server* ptr);
    /// called when WM_CLOSE is received, return true if app should quit
    virtual bool OnClose();
};

#endif
