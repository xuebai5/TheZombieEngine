#ifndef N_VIDEOPLAYER_H
#define N_VIDEOPLAYER_H
//------------------------------------------------------------------------------
/**
    @class nVideoPlayer
    @ingroup Video

    An abstract player for videos
    
    (C) 2005 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "gfx2/ntexture2.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nVideoPlayer : public nRoot
{
public:
    enum LoopType
    {
        Clamp = 0,
        Repeat,
    };

    /// constructor
    nVideoPlayer();
    /// destructor
    virtual ~nVideoPlayer();
    /// open the video player
    virtual bool Open();
    /// close the video player
    virtual void Close();
    /// Rewinds the video
    virtual void Rewind();
    /// currently open?
    bool IsOpen() const;
    /// Set filename
    void SetFilename(const nString filename);
    /// Get filename
    nString GetFilename() const;
    /// Set texture to render to
    void SetTexture(nTexture2* texturePtr);
    /// Get texture to render to
    nTexture2* GetTexture() const;
    /// Enable/Disable updating of texture    
    void SetTextureUpdate(bool doUpdate);
    /// Get TextureUpdate Status
    bool GetTextureUpdate() const;
    /// Set Looping flag
    void SetLoopType(LoopType loopType);
    /// Get Looping flag
    LoopType GetLoopType() const;

    virtual void DecodeNextFrame();
    virtual void Decode(nTime deltaTime);
    virtual uint GetWidth() const;
    virtual uint GetHeight() const;
    virtual float GetFpS() const;
    virtual uint GetFrameCount() const;

protected:

    bool        isOpen;
    nString     filename;
    nTexture2*  texturePtr;
    
    uint        videoWidth;
    uint        videoHeight;
    float       videoFpS;
    uint        videoFrameCount;
    bool        doTextureUpdate;
    LoopType    loopType;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVideoPlayer::IsOpen() const
{
    return this->isOpen;
}


//------------------------------------------------------------------------------
/**
    Set Looping flag
*/
inline
void
nVideoPlayer::SetLoopType(LoopType loopType)
{
    this->loopType=loopType;
}

//------------------------------------------------------------------------------
/**
    Get Looping flag
*/
inline
nVideoPlayer::LoopType
nVideoPlayer::GetLoopType() const
{
    return this->loopType;
}

//------------------------------------------------------------------------------
#endif    
