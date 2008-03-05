#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//  nlinedrawer_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "tools/nlinedrawer.h"
#include "kernel/nkernelserver.h"
#include "gfx2/ngfxutils.h"

//------------------------------------------------------------------------------
/**
*/
nLineDrawer::nLineDrawer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLineDrawer::~nLineDrawer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set path of shader used to paint the lines
    @param shPath Path of the shader
*/
void
nLineDrawer::SetShaderPath( const nString& shPath )
{
    this->shaderPath = shPath;
}
//------------------------------------------------------------------------------
/**
    Set path of shader used to paint the lines
    @return shPath Path of the shader
*/
const nString&
nLineDrawer::GetShaderPath( void )
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
    Draw a line 3d extruded. Each point is repeated by adding the vector 'extrude'. The line is drawn as a
    triangle strip.

    @param points buffer of vector3
    @param numPoints Number of line points
    @param colors color array. Line points are drawn cyclically with this colors
    @param numColors Number of colors in the 'colors' array
    @param extrude vector wich is added to each line point to generate the extrusion
*/
void
nLineDrawer::DrawExtruded3DLine(vector3 *points, int numPoints, vector4 *colors, int numColors, vector3 extrude, bool closed)
{
    if ( numPoints <= 1 )
    {
        return;
    }

    n_assert( points );
    n_assert( colors && numColors >= 1);

    matrix44 m;
    nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, m);

    if (!this->refShader.isvalid())
    {
        n_assert(!this->shaderPath.IsEmpty());
        this->refShader = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
        this->refShader->SetFilename(this->shaderPath);
        n_verify(this->refShader->Load());
    }

    n_assert(this->refShader.isvalid());
    nGfxServer2::Instance()->SetShader(this->refShader);

    if (!this->dynMesh.IsValid())
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleStrip,
            nMesh2::Coord | /*nMesh2::Uv0*/ nMesh2::Color,
            nMesh2::WriteOnly /*| nMesh2::NeedsVertexShader*/, false);
        n_assert(this->dynMesh.IsValid());
    }

    int maxVertices;
    float* dstVertices = 0;
    this->dynMesh.Begin( dstVertices, maxVertices );
    n_assert( dstVertices );
    
    int validPoints = 0;   
    float* currBufPointer = dstVertices;
    int currColor = 0;


    this->refShader->Begin(true);
    this->refShader->BeginPass( 0 );

    if ( closed )
    {
        numPoints++;
    }
    vector3 *curPoint = points;

    for (int i= 0; i < numPoints; i++ )
    {

        // -- First point --

        vector3 pos = *curPoint;

        *currBufPointer++ = pos.x;
        *currBufPointer++ = pos.y;
        *currBufPointer++ = pos.z;
        curPoint++;

        // ( insert here U and V if needed )

        *currBufPointer++ = colors[ currColor ].x;
        *currBufPointer++ = colors[ currColor ].y;
        *currBufPointer++ = colors[ currColor ].z;
        *currBufPointer++ = colors[ currColor ].w;

        // -- Extruded point --

        pos += extrude;

        *currBufPointer++ = pos.x;
        *currBufPointer++ = pos.y;
        *currBufPointer++ = pos.z;

        // ( insert here U and V if needed )

        *currBufPointer++ = colors[ currColor ].x;
        *currBufPointer++ = colors[ currColor ].y;
        *currBufPointer++ = colors[ currColor ].z;
        *currBufPointer++ = colors[ currColor ].w;


        // Change color from color array
        if ( numColors > 1)
        {
            currColor++;
            if ( currColor >= numColors )
            {
                currColor = 0;
            }
        }

        validPoints+= 2;

        // Check buffer completion
        if ( validPoints > maxVertices - 2 )
        {
            this->dynMesh.Swap( validPoints, dstVertices );

            validPoints = 0;
            currBufPointer = dstVertices;
        }

        if ( closed && i == numPoints - 2 )
        {
            curPoint = points;
        }
    }

    this->dynMesh.End( validPoints );

    this->refShader->EndPass();
    this->refShader->End();

    nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);

}

//------------------------------------------------------------------------------
/**
*/
void
nLineDrawer::DrawExtruded3DLineOnPlane(vector3 * /*points*/,  int /*np*/, vector3 /*planeNormal*/, float /*size*/)
{
    // implement me
}

//------------------------------------------------------------------------------
/**
*/
void
nLineDrawer::DrawRectangle2D(const rectangle& rect, const vector4& color)
{
    static vector2 vertexList[5];

    vertexList[0].set(rect.v0.x, rect.v0.y);
    vertexList[1].set(rect.v1.x, rect.v0.y);
    vertexList[2].set(rect.v1.x, rect.v1.y);
    vertexList[3].set(rect.v0.x, rect.v1.y);
    vertexList[4].set(rect.v0.x, rect.v0.y);

    nGfxServer2::Instance()->DrawLines2d(vertexList, 5, color);
}

//------------------------------------------------------------------------------
/**
    Draw the given rectangle with a single line.

    @param  rect        rectangle in relative coordinates
    @param  color       color of the rectangle (rgba)
*/
void
nLineDrawer::DrawRectangle2DRel(const rectangle& rect, const vector4& color)
{
    static vector2 vertexList[5];

    float x0 = static_cast<float>(nGfxUtils::absx(rect.v0.x));
    float y0 = static_cast<float>(nGfxUtils::absy(rect.v0.y));
    float x1 = static_cast<float>(nGfxUtils::absx(rect.v1.x));
    float y1 = static_cast<float>(nGfxUtils::absy(rect.v1.y));

    vertexList[0].set(x0, y0);
    vertexList[1].set(x1, y0);
    vertexList[2].set(x1, y1);
    vertexList[3].set(x0, y1);
    vertexList[4].set(x0, y0);

    nGfxServer2::Instance()->DrawLines2d(vertexList, 5, color);
}

//------------------------------------------------------------------------------
/**
    Draw a border around the given rectangle. Must be used between a pair
    of nGfxServer2::BeginLines() / nGfxServer2::EndLines() calls.

    @param  rect        rectangle in relative coordinates
    @param  innerWidth  inner width of the border in pixels
    @param  outerWidth  outer width of the border in pixels
    @param  color       color of the rectangle (rgba)
*/
void
nLineDrawer::DrawRectangle2DRelBorder(const rectangle& rect, int innerWidth, int outerWidth, const vector4& color)
{
    static vector2 vertexList[5];

    float x0 = static_cast<float>(nGfxUtils::absx(rect.v0.x));
    float y0 = static_cast<float>(nGfxUtils::absy(rect.v0.y));
    float x1 = static_cast<float>(nGfxUtils::absx(rect.v1.x));
    float y1 = static_cast<float>(nGfxUtils::absy(rect.v1.y));

    for (int i = innerWidth; i <= outerWidth; i++)
    {
        float border = 1.0f * i;

        vertexList[0].set(x0 - border, y0 - border);
        vertexList[1].set(x1 + border, y0 - border);
        vertexList[2].set(x1 + border, y1 + border);
        vertexList[3].set(x0 - border, y1 + border);
        vertexList[4].set(x0 - border, y0 - border);

        nGfxServer2::Instance()->DrawLines2d(vertexList, 5, color);
    }
}
