#ifndef N_DOTLASER_H
#define N_DOTLASER_H
#include "nscene/nspotlightnode.h"
//------------------------------------------------------------------------------
/**
    @class nDotLaserNode
    @ingroup SceneNodes
    @author Cristobal Castillo

    @brief A laser node with a projection matrix describing a pyramidal

    (C) 2006 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class nDotLaserNode : public nLightNode
{
public:
    /// constructor
    nDotLaserNode();
    /// destructor
    virtual ~nDotLaserNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform pre-instancing rendering of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// render geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

private:

};




#endif //!N_DOT_LASER