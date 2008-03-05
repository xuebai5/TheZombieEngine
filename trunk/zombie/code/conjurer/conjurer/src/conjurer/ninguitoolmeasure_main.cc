#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nInguiToolSphereArea.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolmeasure.h"
#include "kernel/nkernelserver.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nterraineditorstate.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiToolMeasure, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiToolMeasure::nInguiToolMeasure() :
    terrainLine(0)
{
    this->terrainLine = static_cast<nTerrainLine*>(nKernelServer::Instance()->New( "nterrainline" ));

    label = "Place spherical area trigger";

    this->terrainLine->SetOffset( 0.0f );

    // Default shader for line drawer
    this->lineDrawer.SetShaderPath("shaders:defaulttool.fx");
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolMeasure::~nInguiToolMeasure()
{
    this->terrainLine->Release();
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolMeasure::Apply( nTime /*dt*/ )
{
    if ( this->state >= Inactive )
    {
        vector3 r = this->lastPos - this->firstPos;

        this->terrainLine->Clear();
        this->terrainLine->SetHeightMap( static_cast<nTerrainEditorState*>( nConjurerApp::Instance()->FindState("terrain") )->GetHeightMap() );
        this->terrainLine->AddPoint( vector2( this->firstPos.x, this->firstPos.z ) );
        this->terrainLine->AddPoint( vector2( this->lastPos.x, this->lastPos.z ) );
        this->terrainLine->Wrap();
        this->terrainDistance = 0.0f;

        if ( this->terrainLine->GetNumVertices() > 0 )
        {
            vector3 point0pos = this->terrainLine->GetPointPosition( 0 );
            for (int point = 1; point < this->terrainLine->GetNumVertices(); point++ )
            {
                vector3 point1pos = this->terrainLine->GetPointPosition( point );
                vector3 dist = ( point1pos - point0pos );
                this->terrainDistance += dist.len();
                point0pos = point1pos;
            }
        }

        vector3 vecXZ = vector3( this->pickingNormal.x, 0.0f, this->pickingNormal.z );

        float vecXZlen = vecXZ.len();
        if ( vecXZlen < TINY )
        {
            this->slopeAngle = 0.0f;
        }
        else
        {
            this->slopeAngle = 90.0f - n_rad2deg( acos( ( this->pickingNormal.dot( vecXZ ) ) / ( this->pickingNormal.len() * vecXZ.len() ) ) );
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
float
nInguiToolMeasure::GetMeasureInfo( float& slopeAngle )
{
    if ( this->GetState() <= nInguiTool::Inactive )
    {
        slopeAngle = 0.0f;
        return 0.0f;
    }

    slopeAngle = this->slopeAngle;

    return this->terrainDistance;
}

//------------------------------------------------------------------------------
/**
*/
void
nInguiToolMeasure::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    if ( ! this->drawEnabled )
    {
        return;
    }

    // Get a reference to the graphics server
    //nGfxServer2* refGfxServer = nGfxServer2::Instance();

    // Draw terrain line
    int np = this->terrainLine->GetNumVertices();
    vector3* lineVertices = lineVertices = this->terrainLine->GetVertexBuffer();

    vector4 col(0.2f, 0.8f, 0.2f, 1.0f );
    vector3 extrude = vector3(0.0f, 0.1f, 0.0f);
    this->lineDrawer.DrawExtruded3DLine( lineVertices, np, &col, 1, extrude, false );

    this->drawEnabled = false;
}
//------------------------------------------------------------------------------
