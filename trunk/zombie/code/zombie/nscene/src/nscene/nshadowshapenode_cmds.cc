#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshadowshapenode_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nshadowshapenode.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
*/
bool
nShadowShapeNode::SaveCmds(nPersistServer* ps)
{
    return nSceneNode::SaveCmds(ps);
}
