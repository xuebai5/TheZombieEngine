#ifndef N_TEXTURESWITCHANIMATOR_H
#define N_TEXTURESWITCHANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTextureSwitchAnimator
    @ingroup SceneAnimators

    @brief nTextureSwitchAnimator selects between one of several textures,
    based on the value of some decision channel.

    See also @ref N2ScriptInterface_ntextureanimator

    (C) 2004 Conjurer Services, S.A.
*/

#include "nscene/ntextureanimator.h"

//------------------------------------------------------------------------------
class nTextureSwitchAnimator : public nTextureAnimator
{
public:
    /// constructor
    nTextureSwitchAnimator();
    /// destructor
    virtual ~nTextureSwitchAnimator();
    
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

private:
    ///...
};

//------------------------------------------------------------------------------
#endif
