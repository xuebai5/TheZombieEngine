//------------------------------------------------------------------------------
//  ntexture2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "gfx2/nsurface.h"
#include "gfx2/ntexture2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nTexture2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nTexture2::nTexture2() :
    type(TEXTURE_NOTYPE),
    format(NOFORMAT),
    width(0),
    height(0),
    depth(0),
    numMipMaps(0),
    usage(0),
    compoundFile(0),
    compoundFilePos(0)
{
    this->nResource::SetType(nResource::Texture);
}

//------------------------------------------------------------------------------
/**
*/
nTexture2::~nTexture2()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
    if (this->compoundFile)
    {
        this->compoundFile->Release();
        this->compoundFile = 0;
  }
}

//------------------------------------------------------------------------------
/**
    Locks a mipmap level of the texture, returns a pointer to the surface
    data and the surface pitch in the provided lockInfo structure.
*/
bool
nTexture2::Lock(LockType /*lockType*/, int /*level*/, LockInfo& /*lockInfo*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Locks a rectnagle in a mipmap level of the texture, returns a pointer to 
    the surface data and the surface pitch in the provided lockInfo structure.
*/
bool 
nTexture2::LockRect(LockType /*lockType*/, int /*level*/, int /*left*/, int /*top*/, int /*right*/, int /*bottom*/, LockInfo& /*lockInfo*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nTexture2::Unlock(int /*level*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Locks a single surface of a cube texture.
*/
bool 
nTexture2::LockCubeFace(LockType /*lockType*/, CubeFace /*face*/, int /*level*/, LockInfo& /*lockInfo*/)
{
    return false;
}


//------------------------------------------------------------------------------
/**
    Unlocks a single surface of a cube texture.
*/
void
nTexture2::UnlockCubeFace(CubeFace /*face*/, int /*level*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void
nTexture2::GetSurfaceLevel(const char* /*objName*/, uint /*level*/, nSurface** /*surface*/)
{
    // reimplemented in derived class's member function.
}

//------------------------------------------------------------------------------
/**
    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void
nTexture2::GenerateMipMaps()
{
    // reimplemented in derived class's member function.
}
//------------------------------------------------------------------------------
/**
    Copy to another compatible texture
    - May-05 Juanjo
*/
void
nTexture2::Copy( nTexture2*, const rectanglei&, const rectanglei&, Filter  )
{
    // reimplemented in derived class's member function.
}

//------------------------------------------------------------------------------
/**
    Save texture on a file with the format specified
*/
bool
nTexture2::SaveResourceAs(const char* /*name*/, nTexture2::Format /*format*/)
{
    // reimplemented in derived class's member function.
    return false;
}
