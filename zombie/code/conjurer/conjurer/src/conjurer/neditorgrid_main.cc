#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
// neditorgrid_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorgrid.h"
#include "napplication/nappviewport.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nEditorGrid, "nroot");

//------------------------------------------------------------------------------
/**
*/
nEditorGrid::nEditorGrid():
    origin(vector3(0.0f, 0.0f, 0.0f)),
    spacing(1.0f),
    lightColor(vector4(0.6f, 0.6f, 0.6f, 1.0f)),
    darkColor(vector4(0.3f, 0.3f, 0.3f, 1.0f)),
    cameraRange( 100.0f ),
    numSub( 4 ),
    snapEnabled( true ),
    drawEnabled( true ),
    lineHandler(nGfxServer2::LineList, nMesh2::Coord | nMesh2::Color)
{
    this->lineHandler.SetShader("shaders:line_grid.fx");
}

//------------------------------------------------------------------------------
/**
*/
nEditorGrid::~nEditorGrid()
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
vector3
nEditorGrid::SnapPosition(vector3 pos)
{
    if ( this->snapEnabled )
    {
        return Snap( pos );
    }
    else
    {
        return pos;
    }
}
//------------------------------------------------------------------------------
/**
*/
vector3
nEditorGrid::Snap(vector3 pos, float sp)
{
    if ( sp == 0.0f )  
    {  
        sp = this->spacing;  
    } 

    vector3 v = pos - this->origin;

    v.x = v.x / sp;  
    v.y = v.y / sp;  
    v.z = v.z / sp;  
    v.x = int( v.x ) * sp;  
    v.y = int( v.y ) * sp;  
    v.z = int( v.z ) * sp;  

    return v + this->origin;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::Draw( nAppViewport* vp )
{
    if ( ! this->drawEnabled )
    {
        return;
    }

    // Normalized distance from center of grid, where fading of lines begins
    static const float fadeDistance = 0.8f;

    // Get viewport and camera info
    //nGfxServer2* refGfxServer = nGfxServer2::Instance();
    nCamera2 camera = vp->GetCamera();

    // Sets vp in gfxserver
    //FIXME
    //vp->SetInGfxServer();

    // Get camera pos
    matrix44 viewMatrix = vp->GetViewMatrix();
    vector3 camPos = viewMatrix.pos_component();

    // Calc. dynamic range (hardcoded)
    float curCameraRange = this->cameraRange * max( 0.3f, camPos.y / 50.0f);
    float curSpacing = this->spacing;
    int multi = 1;
    while ( curSpacing / curCameraRange < 0.005 )
    {
        curSpacing = curSpacing * this->numSub;
        multi *= this->numSub;
    }

    int numPointsSide = int( 2.0f * curCameraRange / curSpacing );

    if ( numPointsSide < 2 )
    {
        return;
    }

    // Calc. grid dimensions
    vector3 cornerNW = this->Snap(camPos - vector3( curCameraRange, this->origin.y, curCameraRange ), curSpacing );
    vector3 cornerSE = this->Snap(camPos + vector3( curCameraRange, this->origin.y, curCameraRange ), curSpacing );

    float temp = ( cornerNW.x - this->origin.x ) / curSpacing;
    int iX = int( temp );
    temp = ( cornerNW.z - this->origin.z ) / curSpacing;
    int iZ = int( temp );

    vector3 extrude = vector3(0.0f, 0.1f, 0.0f);// temp
    
    vector3 line[6];
    vector3 curPos = cornerNW;
    vector4 colors[6];
    matrix44 m;

    // (For setting a shader parameter in line handler..)
    //nShaderParams& shaderParams = this->lineHandler.GetShaderParams();
    //shaderParams.SetArg(nShaderState::DiffMap0, nShaderArg(tex));

    // Draw grid

    this->lineHandler.BeginLines(m);

    for (int i = 0; i < numPointsSide; i++)
    {

        float alpha = 1.0f;
        float da = abs(curPos.x - camPos.x) / curCameraRange - fadeDistance;
        if ( da > 0.0f )
        {
            alpha -= da / ( 1.0f - fadeDistance );
        }

        if ( iX % this->numSub == 0 )
        {
            colors[0] = this->darkColor;
        }
        else
        {
            colors[0] = this->lightColor;
        }
        colors[0].w = 0.0f;
        colors[1] = colors[0];
        colors[1].w = alpha;
        colors[2] = colors[1];
        colors[3] = colors[2];
        colors[4] = colors[3];
        colors[5] = colors[0];

        line[0] = vector3( curPos.x, origin.y, cornerNW.z);
        line[1] = vector3( curPos.x, origin.y, cornerNW.z + 0.5f * ( 1.0f - fadeDistance ) * ( cornerSE.z - cornerNW.z ));
        line[2] = line[1];
        line[3] = vector3( curPos.x, origin.y, cornerSE.z - 0.5f * ( 1.0f - fadeDistance ) * ( cornerSE.z - cornerNW.z ));
        line[4] = line[3];
        line[5] = vector3( curPos.x, origin.y, cornerSE.z);

        this->lineHandler.DrawLines3d(line,0, colors, 6 );

        curPos.x += curSpacing;
        iX++;
    }

    curPos = cornerNW;
    for (int i = 0; i < numPointsSide; i++)
    {
        float alpha = 1.0f;
        float da = abs(curPos.z - camPos.z) / curCameraRange - fadeDistance;
        if ( da > 0.0f )
        {
            alpha -= da / ( 1.0f - fadeDistance );
        }

        if ( iZ % this->numSub == 0 )
        {
            colors[0] = this->darkColor;
        }
        else
        {
            colors[0] = this->lightColor;
        }
        colors[0].w = 0.0f;
        colors[1] = colors[0];
        colors[1].w = alpha;
        colors[2] = colors[1];
        colors[3] = colors[2];
        colors[4] = colors[3];
        colors[5] = colors[0];

        line[0] = vector3( cornerNW.x, origin.y, curPos.z);
        line[1] = vector3( cornerNW.x + 0.5f * ( 1.0f - fadeDistance ) * ( cornerSE.x - cornerNW.x ), origin.y, curPos.z);
        line[2] = line[1];
        line[3] = vector3( cornerSE.x - 0.5f * ( 1.0f - fadeDistance ) * ( cornerSE.x - cornerNW.x ), origin.y, curPos.z);
        line[4] = line[3];
        line[5] = vector3( cornerSE.x, origin.y, curPos.z);

        this->lineHandler.DrawLines3d(line,0, colors, 6 );

        curPos.z += curSpacing;
        iZ++;
    }

    this->lineHandler.EndLines();

}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetLightColor(vector4 color)
{
    this->lightColor = color;
}
//------------------------------------------------------------------------------
/**
*/
vector4
nEditorGrid::GetLightColor()
{
    return this->lightColor;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetDarkColor(vector4 color)
{
    this->darkColor = color;
}
//------------------------------------------------------------------------------
/**
*/
vector4
nEditorGrid::GetDarkColor()
{
    return this->darkColor;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetSpacing(float sp)
{
    this->spacing = sp;
}
//------------------------------------------------------------------------------
/**
*/
float
nEditorGrid::GetSpacing()
{
    return this->spacing;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetCameraRange(float r)
{
    this->cameraRange = r;
}
//------------------------------------------------------------------------------
/**
*/
float
nEditorGrid::GetCameraRange()
{
    return this->cameraRange;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetNSubdivision(int n)
{
    this->numSub = n;
}
//------------------------------------------------------------------------------
/**
*/
int
nEditorGrid::GetNSubdivision()
{
    return this->numSub;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetSnapEnabled(bool en)
{
    this->snapEnabled = en;
}
//------------------------------------------------------------------------------
/**
*/
bool
nEditorGrid::GetSnapEnabled()
{
    return this->snapEnabled;
}
//------------------------------------------------------------------------------
/**
*/
void
nEditorGrid::SetDrawEnabled(bool en )
{
    this->drawEnabled = en;
}
//------------------------------------------------------------------------------
/**
*/
bool
nEditorGrid::GetDrawEnabled()
{
    return this->drawEnabled;
}
//------------------------------------------------------------------------------