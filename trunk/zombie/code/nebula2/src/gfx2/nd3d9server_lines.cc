#include "precompiled/pchndirect3d9.h"
//------------------------------------------------------------------------------
//  nd3d9server_lines.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9shader.h"
#include "gfx2/nmesh2.h"
#include "mathlib/plane.h"

//------------------------------------------------------------------------------
/**
    Begin rendering lines.
*/
void
nD3D9Server::BeginLines()
{
    N_OUTPUTDEBUGSTRING("nD3D9Shader::BeginLines()\n");

    this->SetRenderState();

    n_assert(this->d3dxLine);
    nGfxServer2::BeginLines();
    n_dxverify2(
        this->d3dxLine->Begin(),
        "ID3DXLine::Begin() failed!" );
}

//------------------------------------------------------------------------------
/**
    Render a 3d line strip using the current transformations.
    
    - 27.01.2005    ma.garcias      clip vertices to near plane manually.
*/
void
nD3D9Server::DrawLines3d(const vector3* vertexList, int numVertices, const vector4& color, bool clip)
{
    #ifndef NGAME
    if (!this->drawLines)
    {
        return;
    }
    #endif//NGAME

    n_assert(vertexList);
    n_assert(numVertices > 0);
    n_assert(this->inBeginLines);
    n_assert(this->d3dxLine);
    D3DCOLOR d3dColor = N_COLORVALUE(color.x, color.y, color.z, color.w);
    const matrix44& modelViewProj = this->GetTransform(nGfxServer2::ModelViewProjection);
    
    if (!clip)
    {
        n_dxverify2(
            this->d3dxLine->DrawTransform((CONST D3DXVECTOR3*) vertexList, numVertices, (CONST D3DXMATRIX*) &modelViewProj, d3dColor),
            "ID3DXLine::DrawTransform() failed!" );
    }
    else
    {
        // group vertices in pairs and draw them one by one
        // checking first if both vertices are within the near plane first
        const matrix44& modelView = this->GetTransform(nGfxServer2::ModelView);
        const matrix44& projection = this->GetTransform(nGfxServer2::Projection);
        for (int i = 0; i < numVertices - 1; i++)
        {
            vector3 v0, v1, vertexPair[2];
            memcpy(vertexPair, &vertexList[i], sizeof(vertexPair));
            v0 = modelView * vertexList[i];
            v1 = modelView * vertexList[i+1];
            if (v0.z > 0 || v1.z > 0)
            {
                if (v0.z > 0 && v1.z > 0)
                    continue;

                // TODO: find intersection point between v0 and v1
                float l;
                plane p0(0, 0, -1, 0);
                if (v0.z > 0) // TODO: cull v0
                {
                    line3 l0(v0, v1);
                    p0.intersect(l0, l);
                    l += (float) TINY;
                    vertexPair[0] = l0.ipol(l);
                    vertexPair[1] = v1;
                }
                else // TODO: cull v1;
                {
                    line3 l0(v1, v0);
                    p0.intersect(l0, l);
                    l += (float) TINY;
                    vertexPair[0] = v0;
                    vertexPair[1] = l0.ipol(l);
                }

                n_dxverify2(
                    this->d3dxLine->DrawTransform((CONST D3DXVECTOR3*) vertexPair, 2, (CONST D3DXMATRIX*) &projection, d3dColor),
                    "ID3DXLine::DrawTransform() failed!" );
                continue;
            }

            n_dxverify2(
                this->d3dxLine->DrawTransform((CONST D3DXVECTOR3*) vertexPair, 2, (CONST D3DXMATRIX*) &modelViewProj, d3dColor),
                "ID3DXLine::DrawTransform() failed!" );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render a 3d line strip using the current transformations. Careful:
    Clipping doesn't work correctly!
*/
void
nD3D9Server::DrawLines2d(const vector2* vertexList, int numVertices, const vector4& color)
{
    #ifndef NGAME
    if (!this->drawLines)
    {
        return;
    }
    #endif //NGAME

    n_assert(vertexList);
    n_assert(numVertices > 0);
    n_assert(this->inBeginLines);
    n_assert(this->d3dxLine);
    D3DCOLOR d3dColor = N_COLORVALUE(color.x, color.y, color.z, color.w);

    n_dxverify2(this->d3dxLine->Draw((CONST D3DXVECTOR2*) vertexList, numVertices, d3dColor),
                "ID3DXLine::Draw() failed!");

    // convert to d3d screen space
/*    vector2 rtSize = this->GetCurrentRenderTargetSize();
    D3DXVECTOR2* d3dxVectors = n_new_array(D3DXVECTOR2, numVertices);
    int i;
    for (i = 0; i < numVertices; i++)
    {
        d3dxVectors[i].x = vertexList[i].x * rtSize.x;
        d3dxVectors[i].y = vertexList[i].y * rtSize.y;
    }

    n_dxverify2(this->d3dxLine->Draw(d3dxVectors, numVertices, d3dColor),
                "ID3DXLine::Draw() failed!");
    n_delete(d3dxVectors);*/
}

//------------------------------------------------------------------------------
/**
    Finish rendering lines.
*/
void
nD3D9Server::EndLines()
{
    N_OUTPUTDEBUGSTRING("nD3D9Shader::EndLines()\n");

    n_assert(this->d3dxLine);
    nGfxServer2::EndLines();
    n_dxverify2(
        this->d3dxLine->End(),
        "ID3DXLine::End() failed!" );
}
