#include "precompiled/pchconjurerexp.h"
//-----------------------------------------------------------------------------
//  nlayer.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nlayermanager/nlayer.h"
#include "kernel/nkernelserver.h"
#include "entity/nentityobject.h"
#include "entity/nentityobjectserver.h"

nNebulaScriptClass(nLayer, "nroot");

//-----------------------------------------------------------------------------
/**
    Constructor 1

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer::nLayer():
m_locked(false),
m_active(true),
m_id(-1),
layerKey(nEntityObjectServer::IDINVALID)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Constructor 2

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer::nLayer(const nString &name):
m_locked(false),
m_active(true),
m_id(-1),
m_password(0),
layerKey(nEntityObjectServer::IDINVALID)
{
    this->SetLayerName(name);
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer::~nLayer()
{
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

