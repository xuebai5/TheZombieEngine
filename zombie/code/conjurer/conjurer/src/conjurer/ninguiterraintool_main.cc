#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintool_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintool.h"
#include "kernel/nkernelserver.h"
#include "nscene/nsurfacenode.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/ncphyterrain.h"
#include "nphysics/ncphyterraincell.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainTool, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainTool::nInguiTerrainTool():
    shapeSquared( true ),
    alwaysUsePicking( false ),
    diameter( 0.0f ),
    cursor3DLine(0)
{
    this->cursor3DLine = static_cast<nTerrainLine*>(nKernelServer::Instance()->New( "nterrainline" ));

    this->cursor3DLine->SetOffset( 0.0f );

    cursorColor = vector4(0.2f, 0.3f, 0.0f, 0.5f );

    pickWhileIdle = true;

    // Default shader for line drawer
    this->lineDrawer.SetShaderPath("shaders:defaulttool.fx");

    // Physic ray for terrain picking
    this->phyRay = static_cast<nPhyGeomRay*>( nKernelServer::Instance()->New("nphygeomray") );
    this->phyRay->SetCategories( nPhysicsGeom::Check );
    this->phyRay->SetCollidesWith( -1 );

    // Contact array
    this->contacts.SetFixedSize( 500 );

}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainTool::~nInguiTerrainTool()
{
    this->phyRay->Release();
    this->cursor3DLine->Release();
}
//------------------------------------------------------------------------------   
/**
*/
float
nInguiTerrainTool::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    if ( ! this->heightMap.isvalid() )
    {
        return -1.0f;
    }

    float t = -1.0f;

    // Get the intersection and then the x,z coordinates for applying the tool
    vector3 intersPoint;
    int xInt, zInt;
    bool intersect = false;
    if ( this->state <= Inactive || alwaysUsePicking )
    {
        ncPhyTerrain* phyT = this->outdoor->GetComponent<ncPhyTerrain>();
        n_assert( phyT );

        this->phyRay->SetPosition( ray.b );
        this->phyRay->SetDirection( ray.m );
        this->phyRay->SetLength( ray.m.len() );

        int i = 0;
        int n = phyT->GetNumberOfCells();
        while ( i < n )
        {
            ncPhyTerrainCell* cell = phyT->GetTerrainCell( i );
            int numContacts = cell->Collide( this->phyRay, contacts.Size(), &contacts[0]);
            for ( int j = 0; j < numContacts; j++ )
            {
                vector3 v;
                contacts[j].GetContactPosition( v );
                float t0 = ( v - ray.b ).len() / ray.m.len();
                if ( t < 0.0f || t0 < t )
                {
                    t = t0;
                    intersect = true;
                    intersPoint = v;
                }
            }
            i++;
        }

        if ( intersect )
        {
            this->firstPos = intersPoint;
        }

/*        if (heightMap->Intersect(ray, t, xInt, zInt))
        {
            intersect = true;
            intersPoint = ray.b + ray.m * t;
            this->firstPos = intersPoint;
        }
        else
        {
            t = -1.0f;
        }
*/
    }
    else 
    {
        plane thePlane( this->firstPos, this->firstPos + vector3(1.0f,0,0), this->firstPos + vector3(0,0,1.0f) );
        if ( thePlane.intersect(ray, t) )
        {
            intersect = true;
            intersPoint = ray.ipol(t);
        }
    }

    if ( intersect ) {
        // Select terrain point selected by the user.
        float xF = this->GetDrawResolutionMultiplier() * intersPoint.x / heightMap->GetGridScale();
        float zF = this->GetDrawResolutionMultiplier() * intersPoint.z / heightMap->GetGridScale();

        xInt = (int) xF;
        zInt = (int) zF;


        // Select closest terrain point to the picked point
        if ( ( xF - xInt ) > 0.5f )
        {
            xInt ++;
        }
        if ( ( zF - zInt ) > 0.5f )
        {
            zInt ++;
        }

        // Store the current 3d mouse position as a vector3 and as heightmap integer coordinates
        this->lastPos = intersPoint;
        this->lastXMousePos = xInt;
        this->lastZMousePos = zInt;

    }
    else
    {
        t = -1.0f;
    }

    nInguiTool::Pick(vp, mp, ray);

    return t;
}

//------------------------------------------------------------------------------
/**
    @brief This method draws the round or square shape of the tool in integer units (like snap) to terrain grid.
    Drawresolutionmultiplier multiplies the size of the grid, like in the pint tool.
*/
void
nInguiTerrainTool::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    n_assert( this->heightMap.isvalid() );

    if ( ! this->drawEnabled )
    {
        return;
    }

    this->cursor3DLine->Clear();

    float gs = this->heightMap->GetGridScale();
    int d = int( this->GetDiameter() );
    float mul = this->GetDrawResolutionMultiplier();
    float sc =  gs / mul;
    float scale = 0.5f * d * sc;
    vector2 brushPos(int(this->lastPos.x / sc) * sc, int(this->lastPos.z / sc) * sc);

    // Map size in m
    float mapSize = this->heightMap->GetSize() * sc;
    vector2 mapCenter( mapSize, mapSize);


    float offs = 0.0f;
    if ( ! ( d & 1 ) )
    {
        offs = 1.0;
    }
    brushPos += vector2(0.5f, 0.5f) * ( (1.0f + offs) * sc );

    if ( ! this->shapeSquared && d != 1 )
    {

        this->DrawRoundBrush( true );
    }
    else
    {
        
        float x0 =  max( 0.011f * scale, brushPos.x - scale - 0.011f * scale );
        float z0 =  max( 0.01f * scale, brushPos.y - scale + 0.01f * scale );
        float x1 =  min( gs * (this->heightMap->GetSize() - 1 ) - 0.015f * scale, brushPos.x + scale + 0.015f * scale);
        float z1 =  min( gs * (this->heightMap->GetSize() - 1 ) + 0.02f * scale, brushPos.y + scale - 0.02f * scale);

        this->cursor3DLine->AddPoint( vector2( x0, z0 ) );
        this->cursor3DLine->AddPoint( vector2( x1, z0 ) );
        this->cursor3DLine->AddPoint( vector2( x1, z1 ) );
        this->cursor3DLine->AddPoint( vector2( x0, z1 ) );

        this->cursor3DLine->Close();
        this->cursor3DLine->Wrap();

        // Draw brush line
        int np = this->cursor3DLine->GetNumVertices();
        vector3* lineVertices;
        vector3 singlePoint[5];

        lineVertices = this->cursor3DLine->GetVertexBuffer();
            
        vector3 extrude = vector3(0.0f, 1.0f, 0.0f) * max( scale * 0.05f, 0.1f);
        this->lineDrawer.DrawExtruded3DLine( lineVertices, np, &this->cursorColor, 1, extrude, true );    
    }

    this->drawEnabled = false;
}

//------------------------------------------------------------------------------
/**
    @brief Set reference to heightmap
    @param entityObject Outdoor entity
*/
void
nInguiTerrainTool::SetOutdoor( nEntityObject * entityObject )
{
    // set outdoor
    this->outdoor = entityObject;

    if ( entityObject )
    {
        // get terrain geomipmap class component
        ncTerrainGMMClass * comp = entityObject->GetClassComponent<ncTerrainGMMClass>();
        if (comp)
        {
            this->heightMap = comp->GetHeightMap();;
            n_assert2(this->heightMap, "Heightmap not available");
            this->cursor3DLine->SetHeightMap( this->heightMap );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get draw resolution multiplier
*/
float
nInguiTerrainTool::GetDrawResolutionMultiplier()
{
    return 1.0f;
}

//------------------------------------------------------------------------------
/**
    @brief Draw the round brush, clamped to the boundaries of terrain
    @param snapped If true, draws the shape snapped to terrain grid (multiplied by resolution multiplier)
*/
void
nInguiTerrainTool::DrawRoundBrush( bool snapped )
{
    float gs = this->heightMap->GetGridScale();
    float d = this->GetDiameter();
    float mul = this->GetDrawResolutionMultiplier();
    float sc =  gs / mul;
    float scale = 0.5f * d * sc;

    vector2 brushPos;
    if ( snapped )
    {
        brushPos = vector2(int(this->lastPos.x / sc) * sc, int(this->lastPos.z / sc) * sc);
    }
    else
    {
        brushPos = vector2( this->lastPos.x, this->lastPos.z );
    }

    vector3 extrude = vector3(0.0f, 1.0f, 0.0f) * max( scale * 0.05f, 0.1f);
    // Map size in m
    float mapSize = this->heightMap->GetSize() * sc;
    vector2 mapCenter( mapSize, mapSize);

    // Calculate brush size
    float offs = 0.0f;
    
    if ( snapped )
    {
        if ( snapped && ! ( (int(d)) & 1 ) )
        {
            offs = 1.0;
        }
        brushPos += vector2(0.5f, 0.5f) * ( (1.0f + offs) * sc );
    }

    int np = min( 32, int(d) * 4);
    float a = PI / 11.0f;
    float da = 2.0f*PI / ( np );


    bool addedPrevious = false;
    vector2 previousAddedPos;

    this->cursor3DLine->Clear();

    for (int i = 0; i <= np; i++)
    {
        vector2 pos = vector2( sin(a) * scale, cos(a) * scale ) + brushPos;

        float xy = pos.x + pos.y;
        float gs2 = sqrt( gs * gs );
        if ( ( pos.x / gs ) - int( pos.x / gs ) < TINY ||
             ( pos.y / gs ) - int( pos.y / gs ) < TINY ||
             ( xy / gs2 ) - int( xy / gs2 ) < TINY )
        {
            pos.x += 0.01f;
        }

        bool added = this->cursor3DLine->AddPoint( pos );

        if ( !added )
        {
            if ( addedPrevious )
            {
                this->DrawCursorLine( extrude, false );
                this->cursor3DLine->Clear();
            }
        }

        addedPrevious = added;
        a += da;
    }
    this->DrawCursorLine( extrude, false );

}

//------------------------------------------------------------------------------
/**
    @brief Draw current cursor line
    @param extrude extrude vector for the line
*/
void
nInguiTerrainTool::DrawCursorLine( vector3 extrude, bool closed )
{
    int np = this->cursor3DLine->GetNumVertices();
    vector3* lineVertices;
    vector3 singlePoint[5];

    lineVertices = this->cursor3DLine->GetVertexBuffer();
        
    this->lineDrawer.DrawExtruded3DLine( lineVertices, np, &this->cursorColor, 1, extrude, closed );    
}

//------------------------------------------------------------------------------
/**
    @brief Get diameter
    @return Current diameter value for this tool
*/
float
nInguiTerrainTool::GetDiameter( void )
{
    return this->diameter;
}

//------------------------------------------------------------------------------
/**
    @brief Set diameter
    @param @diam Current diameter value for this tool
*/
void
nInguiTerrainTool::SetDiameter( float diam )
{
    this->diameter = diam;
}

//------------------------------------------------------------------------------
