#ifndef N_RNSAPPVIEWPORT_H
#define N_RNSAPPVIEWPORT_H
//------------------------------------------------------------------------------
/**
    @class nCommonViewport
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A Conjurer-specific editor viewport. It implements rendering
    using a local viewport entity, and a camera entity for visibility
    determination. It is a full-featured viewport, with an embedded
    debug module, custom input handling, and some simple option to tweak
    the rendering and control behavior.

    (C) 2005 Conjurer Services, S.A.
*/
#include "ncommonapp/ncommonviewport.h"

#ifndef NGAME
#include "conjurer/neditorviewport.h"
#endif

//------------------------------------------------------------------------------
class nRnsAppViewport : 
#ifdef NGAME
    public nCommonViewport
#else
    public nEditorViewport
#endif
{
public:
    /// constructor
    nRnsAppViewport();
    /// destructor
    virtual ~nRnsAppViewport();
};
//------------------------------------------------------------------------------
#endif
