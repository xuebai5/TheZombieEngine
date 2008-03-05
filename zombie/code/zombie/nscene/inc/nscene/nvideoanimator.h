#ifndef N_VIDEOANIMATOR_H
#define N_VIDEOANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nVideoAnimator
    @ingroup SceneAnimators
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Animates a texture from a video player
    
    (C) 2006 Conjurer Services, S.A.
*/

#include "nscene/nshaderanimator.h"

class nVideoPlayer;
//------------------------------------------------------------------------------
class nVideoAnimator : public nShaderAnimator
{
public:
    /// constructor
    nVideoAnimator();
    /// destructor
    virtual ~nVideoAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

    /// set state variable name
    void SetStateChannel(const char *varName);
    /// get state variable name
    const char *GetStateChannel() const;
    /// set video filename
    void SetVideoFile(const char *file);
    /// get video filename
    const char *GetVideoFile() const;

private:
    nString videoFile;
    nRef<nVideoPlayer> refVideoPlayer;
    nRef<nTexture2> refVideoTexture;
    nVariable::Handle stateVarHandle;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nVideoAnimator::SetVideoFile(const char *filename)
{
    this->videoFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nVideoAnimator::GetVideoFile() const
{
    return this->videoFile.Get();
}

//------------------------------------------------------------------------------
#endif /*N_VIDEOANIMATOR_H*/
