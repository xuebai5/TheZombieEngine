#ifndef SHOOTEMAPP_MODEL_H
#define SHOOTEMAPP_MODEL_H

#include "gfx2/ngfxserver2.h"

enum
{
    Model_Player = 0,
    Model_Enemy,
    Model_Tree,
    NumModels,
};

class Shape
{
public:
    nRef<nMesh2> refMesh;
    nRef<nShader2> refShader;
    nShaderParams shaderParams;
};

class Model
{
public:
    nArray<Shape> shapes;
    nArray<nRef<nTexture2> > textures;
};

#endif //SHOOTEMAPP_MODEL_H
