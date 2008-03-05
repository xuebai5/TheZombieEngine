#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  nvideoplayer_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "video/nvideoplayer.h"

nNebulaClass(nVideoPlayer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nVideoPlayer::nVideoPlayer() :
    isOpen(false),
    filename(""),
    texturePtr(0),
    loopType(Repeat),
    doTextureUpdate(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVideoPlayer::~nVideoPlayer()
{
    n_assert(!this->isOpen);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoPlayer::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoPlayer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoPlayer::SetFilename(const nString filename)
{
    this->filename = nFileServer2::Instance()->ManglePath(filename);
}

//------------------------------------------------------------------------------
/**
*/
nString
nVideoPlayer::GetFilename() const
{
    return(this->filename);
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoPlayer::SetTexture(nTexture2* texturePtr)
{
    this->texturePtr = texturePtr;
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
nVideoPlayer::GetTexture() const
{
    return(this->texturePtr);
}

//------------------------------------------------------------------------------
/**
*/
void 
nVideoPlayer::DecodeNextFrame()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nVideoPlayer::Decode(nTime /*time*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
uint 
nVideoPlayer::GetWidth() const
{
    return this->videoWidth;
}

//------------------------------------------------------------------------------
/**
*/
uint 
nVideoPlayer::GetHeight() const
{
    return this->videoHeight;
}

//------------------------------------------------------------------------------
/**
*/
float 
nVideoPlayer::GetFpS() const
{
    return this->videoFpS;
}

//------------------------------------------------------------------------------
/**
*/
uint 
nVideoPlayer::GetFrameCount() const
{
    return this->videoFrameCount;
}

//------------------------------------------------------------------------------
/**
*/
void 
nVideoPlayer::SetTextureUpdate(bool doUpdate)
{
    this->doTextureUpdate = doUpdate;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVideoPlayer::GetTextureUpdate() const
{
    return this->doTextureUpdate;
}

//------------------------------------------------------------------------------
/**
*/
void 
nVideoPlayer::Rewind()
{
    // empty
}
