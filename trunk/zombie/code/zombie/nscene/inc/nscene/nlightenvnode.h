#ifndef NC_LIGHTENVNODE_H
#define NC_LIGHTENVNODE_H
//------------------------------------------------------------------------------
/**
    @class nLightEnvNode
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief a light node that encapsulating a lightmap environment.
    it is the only node attached to the scene and at render time
    it selects the subnode that should be rendered for the entity
    being lit.

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/nlightnode.h"
#include "util/nsafekeyarray.h"
#include "util/narray2.h"

//------------------------------------------------------------------------------
class nLightEnvNode : public nLightNode
{
public:
    /// constructor
    nLightEnvNode();
    /// destructor
    virtual ~nLightEnvNode();

    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by nSceneGraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// render the light
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

private:
    nArray<int> activeCells;
};

//------------------------------------------------------------------------------
#endif
