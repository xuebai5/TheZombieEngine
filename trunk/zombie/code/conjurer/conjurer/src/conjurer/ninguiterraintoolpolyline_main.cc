#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolpolyline_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolpolyline.h"
#include "kernel/nkernelserver.h"
#include "tools/nmeshbuilder.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiTerrainToolPolyLine, "ninguiterraintool");

#define HELPER_COLOR vector4(1.0f, 1.0f, 0.0f, 0.5f)
#define HELPER_CLOSE_COLOR vector4(1.0f, 0.0f, 0.0f, 0.5f)
#define HELPER_SCALE vector3(0.1f,0.1f,0.1f)

//------------------------------------------------------------------------------

/**
*/
nInguiTerrainToolPolyLine::nInguiTerrainToolPolyLine() :
    currentLine(0)
{
    this->currentLine = static_cast<nTerrainLine*>(nKernelServer::Instance()->New( "nterrainline" ));

    this->lineHandler.SetShader("shaders:line.fx");
    this->lineHandler.SetPrimitiveType( nGfxServer2::LineStrip );
    this->label.Set("Generate polyline");
    this->displayColor = vector4( 1.0f, 1.0f, 1.0f, 1.0f);
    this->mustClose = true;
    this->closePolyMargin = 0.1f;

    // use always picking for detect auto close poly
    this->alwaysUsePicking = true;

    const char* shaderPath = "shaders:line.fx";
    // Load shader
    this->sphereShader = nGfxServer2::Instance()->NewShader( shaderPath );
    this->sphereShader->SetFilename( shaderPath );
    this->sphereShader->Load();
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolPolyLine::~nInguiTerrainToolPolyLine()
{
    this->currentLine->Release();
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiTerrainToolPolyLine::Draw( nAppViewport* vp, nCamera2* /*camera*/ )
{
    n_assert( this->heightMap.isvalid() );
    int numVertices = this->currentLine->GetNumVertices();

    if ( numVertices > 0 )
    {        
        // helper size independent from camera distance
        float size = nInguiTool::Screen2WorldObjectSize( vp, this->currentLine->GetPointPosition( 0 ), 0.1f );
        // draw helper
        if ( !this->pickPointClosePoly )
        {                        
            this->DrawVertexHelper( 1, HELPER_COLOR, size );
        }
        else
        {
            this->DrawVertexHelper( 1, HELPER_CLOSE_COLOR, size );
        }

        // draw lines
        if ( numVertices > 1)
        {
            matrix44 m;
            this->lineHandler.BeginLines( m );
            this->lineHandler.DrawLines3d(this->currentLine->GetVertexBuffer(), 0, this->currentLine->GetNumVertices(), this->displayColor );
            this->lineHandler.EndLines();
        }        
    }
    else
    {
        // draw nothing if there aren't vertices
    }
}

//------------------------------------------------------------------------------
/**
    @brief Check if picked point is near the first point

    @param vp Viewport in witch the mouse pointer is located
    @param ray The ray to do the pick, in world space
    @return The ray parameter of picking position (for ordering intersections), or -1 if couldn't pick.
*/
float
nInguiTerrainToolPolyLine::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    float d = -1.0f;
    d = nInguiTerrainTool::Pick( vp, mp, ray);

    if ( d > -1 )
    {
        if ( this->PointClosePoly( this->lastPos ) )
        {
            this->pickPointClosePoly = true;
        }    
        else
        {
            this->pickPointClosePoly = false;
        }
    }

    return d;
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiTerrainToolPolyLine::Apply( nTime /*dt*/ )
{
    n_assert( this->heightMap );

    if ( this->addPoint )
    {
        if ( this->currentLine->GetHeightMap() == 0 )
        {
            this->currentLine->SetHeightMap( this->heightMap );
        }

        // close polygon if the point connects with the first one        
        if ( !this->pickPointClosePoly )
        {  
            this->currentLine->AddPoint( vector2(this->lastPos.x, this->lastPos.z ) );            
        }
        else
        {
            this->EndLine();
        }
        
        this->addPoint = false;

        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
*/
bool
nInguiTerrainToolPolyLine::HandleInput( nAppViewport* /*vp*/ )
{
    // Left mouse button adds a point
    if ( nInputServer::Instance()->GetButton("buton0_down") )
    {
        this->addPoint = true;
        return true;
    }

    // Find a key to end the line!...
    if ( nInputServer::Instance()->GetButton("return") )
    {
        this->EndLine();
        return true;
    }

    // With escape, the line is aborted
    if ( nInputServer::Instance()->GetButton("cancel") )
    {
        this->CancelLine();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/**
    @brief End the line and whatever you need
*/
void
nInguiTerrainToolPolyLine::EndLine()
{
    // This is a template for implementing in subclasses
    
    if ( this->mustClose )
    {
        if ( !this->currentLine->Close() )
        {
            return;
        }
    }

    // ... do whatever in subclasses of this tool
    
    this->currentLine->Clear();
    this->SetState( nInguiTool::Inactive );
}
//------------------------------------------------------------------------------
/**
    @brief Cancel operation, clear the line
*/
void
nInguiTerrainToolPolyLine::CancelLine()
{
    this->addPoint = false;
    this->currentLine->Clear();
    this->SetState( nInguiTool::Inactive );
}

//------------------------------------------------------------------------------
/**
    @brief Set reference to heightmap
    @param entityObject Outdoor entity
*/
void
nInguiTerrainToolPolyLine::SetOutdoor( nEntityObject * entityObject )
{
    this->CancelLine();
    nInguiTerrainTool::SetOutdoor( entityObject );
    if ( entityObject )
    {
        n_assert( this->heightMap );
        this->currentLine->SetHeightMap( this->heightMap );
    }
}
//------------------------------------------------------------------------------
/**
    @brief Check if a point close the polygon.
    @param firstPoint Outdoor entity
*/
bool
nInguiTerrainToolPolyLine::PointClosePoly( vector3& nextPoint )
{
    if ( this->currentLine->GetNumVertices() > 2 && this->mustClose )
    {        
        vector3 firstPoint = this->currentLine->GetPointPosition( 0 );
        float distance = (firstPoint - nextPoint).len();
        if ( distance < this->closePolyMargin )
        {            
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Draw a sphere around first vertex
*/
void
nInguiTerrainToolPolyLine::DrawVertexHelper( int numVertex, vector4 color, float size )
{    
    n_assert_return( this->currentLine->GetNumVertices() > 0 && numVertex <= this->currentLine->GetNumVertices(), );

    if ( this->currentLine->GetNumVertices() > 0 && numVertex <= this->currentLine->GetNumVertices() )
    {
        matrix44 m;
        // Get gfx server pointer
        nGfxServer2* gfxServer = nGfxServer2::Instance();            

        // Fit matrix transformation to first poly point            
        m.ident();
        m.scale( HELPER_SCALE * size );
        m.translate( this->currentLine->GetPointPosition( numVertex - 1 ) );  

        // Draw the sphere
        gfxServer->BeginShapes( this->sphereShader );                      
        gfxServer->DrawShape(nGfxServer2::Sphere, m, color);
        gfxServer->EndShapes();
    }
}

