#ifndef MODEL_H
#define MODEL_H

#include "kernel/nref.h"
#include "util/narray.h"
#include "gfx2/ngfxserver2.h"

class Shape
{
public:
    nRef<nMesh2> refMesh;
    nRef<nShader2> refShader;
    //nRef<nTexture2> refDiffMap;
    //nRef<nTexture2> refSpecMap;
    nShaderParams shaderParams;
};

class Model
{
public:
    nArray<Shape> shapes;
    nArray<nRef<nTexture2> > textures;
};

#endif
