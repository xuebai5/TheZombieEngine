#ifndef N_FLOATANIMATOR_H
#define N_FLOATANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nFloatAnimator
    @ingroup Scene

    @brief Animates a float attribute of a nAbstractShaderNode.

    (C) 2005 Radon Labs GmbH
*/
#include "nscene/nshaderanimator.h"
#include "util/nanimkeyarray.h"

//------------------------------------------------------------------------------
class nFloatAnimator : public nShaderAnimator
{
public:
    /// constructor
    nFloatAnimator();
    /// destructor
    virtual ~nFloatAnimator();
    /// save object to persistency stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

    /// reset all keys
    virtual void ResetKeys();
    /// add a key 
    void AddKey(float time, float key);
    /// get number of keys
    int GetNumKeys() const;
    /// get key at
    void GetKeyAt(int index, float& time, float& key) const;

private:
    nAnimKeyArray<nAnimKey<float> > keyArray;
};
//------------------------------------------------------------------------------
#endif
