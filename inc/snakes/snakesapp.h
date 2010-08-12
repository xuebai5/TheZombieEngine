#ifndef SNAKESAPP_H
#define SNAKESAPP_H

#include "kernel/nref.h"
#include "util/narray.h"

#include "demos/demoapp.h"

#include "snakes/snakes.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class SnakesApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    enum GameState
    {
        State_Menu = 0,
        State_Score,
        State_Game,
    };

    GameState gameState;

    SnakeBoard board;
	nArray<SnakeState> snakes;

    nRef<nMesh2> refBoardMesh;
    nRef<nTexture2> refBoardTexture;
    nRef<nShader2> refBoardShader;

    nRef<nMesh2> refSnakeMesh;
    nRef<nShader2> refSnakeShader;
};

#endif //SNAKESAPP_H
