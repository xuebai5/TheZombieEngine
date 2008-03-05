#ifndef N_NTEXTUREBUILDER_H
#define N_NTEXTUREBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nTextureBuilder
    @ingroup NebulaGraphicsSystem

    Clsss containing code to make operations with textures like save, resize, 
    fill, etc.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nTexture2;

//------------------------------------------------------------------------------
class nTextureBuilder
{
public:
    /// constructor
    nTextureBuilder();
    /// destructor
    ~nTextureBuilder();
    
    /// set the texture to work with
    void SetTexture(nTexture2 * texture);
    /// load texture by filename
    void LoadTexture(const char * fileName);

    /// fill the texture with color channels rgba
    bool Fill(vector4 & rgba);

    /// resize the texture
    //void Resize(int width, int height);

    /// Save a nTexture2 object
    bool Save( const char * fileName, int newSize = 0 );

    /// @todo refactor this
    /// Load an image file, resize it and save it as a different file
    static bool LoadResizeSaveImage( nString sourceFileName, nString fileName, int newSize = 0 );

private:

    /// reference to work texture
    nRef<nTexture2> refTexture;

};
//------------------------------------------------------------------------------
#endif
