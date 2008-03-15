//------------------------------------------------------------------------------
//  nterrainglobaltexturebuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/nterrainglobaltexturebuilder.h"
#include "tools/ntexturebuilder.h"
#include "gfx2/ngfxserver2.h"

#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "nscene/ncsceneclass.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "mathlib/rectanglei.h"
//------------------------------------------------------------------------------
const int GlobalTextureGenerationResolution = 4;

//------------------------------------------------------------------------------
/**
*/
nTerrainGlobalTextureBuilder::nTerrainGlobalTextureBuilder()
{
    /// empty
}
//------------------------------------------------------------------------------
/**
*/
nTerrainGlobalTextureBuilder::~nTerrainGlobalTextureBuilder()
{
    /// empty
    if (this->weightmapTexture.isvalid())
    {
        this->weightmapTexture->Release();
        this->weightmapTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainGlobalTextureBuilder::SetTerrainMaterialComponent( ncTerrainMaterialClass* tmc )
{
    this->terrainMaterialComponent = tmc;
}

//------------------------------------------------------------------------------
/**
    Generate global texture from current weightmap and indexmap, given a set of materials
    @param x0 tile coordinate of top-left corner rectangle to generate
    @param z0 tile coordinate of top-left corner rectangle to generate
    @param x1 tile coordinate of bottom-right corner rectangle to generate
    @param z1 tile coordinate of bottom-right corner rectangle to generate
    @return true if the process was succesful, false otherwise.

*/
void
nTerrainGlobalTextureBuilder::GenerateGlobalTexture( int x0, int z0, int x1, int z1 )
{
    n_assert( terrainMaterialComponent );
    n_assert( this->terrainMaterialComponent->layerArray.Size() > 0 );
    n_assert( this->terrainMaterialComponent->globalTextureSize > 0 );

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // generate weightmap texture
    if (!this->weightmapTexture.isvalid())
    {
        this->weightmapTexture = this->terrainMaterialComponent->NewWeightMapTexture(0);
        n_assert( this->weightmapTexture.isvalid() );
        n_assert( this->weightmapTexture->IsLoaded() );
    }

    //// Create global texture
    //if ( ! this->terrainMaterialComponent->refGlobalTexture.isvalid() )
    //{
    //    this->terrainMaterialComponent->refGlobalTexture = nGfxServer2::Instance()->NewTexture(0);
    //    this->terrainMaterialComponent->refGlobalTexture->SetType( nTexture2::TEXTURE_2D );
    //    this->terrainMaterialComponent->refGlobalTexture->SetFormat( nTexture2::A8R8G8B8 );
    //    this->terrainMaterialComponent->refGlobalTexture->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic );
    //    this->terrainMaterialComponent->refGlobalTexture->SetWidth( this->terrainMaterialComponent->globalTextureSize );
    //    this->terrainMaterialComponent->refGlobalTexture->SetHeight( this->terrainMaterialComponent->globalTextureSize );
    //    this->terrainMaterialComponent->refGlobalTexture->Load();
    //    n_assert( this->terrainMaterialComponent->refGlobalTexture->IsEmpty() );
    //}

    // Get map size
    if ( ! this->terrainMaterialComponent->refHeightMap.isvalid() )
    {
        return;
    }
    int indexGlobalArea = this->terrainMaterialComponent->globalTextureSize / this->terrainMaterialComponent->GetMaterialNumBlocks();

    // Create Render target texture
    if ( ! this->refRenderTexture.isvalid() )
    {
        this->refRenderTexture = gfxServer->NewRenderTarget(0, 
            indexGlobalArea * GlobalTextureGenerationResolution, 
            indexGlobalArea * GlobalTextureGenerationResolution, 
            nTexture2::A8R8G8B8, 
            nTexture2::RenderTargetColor );
        if ( ! this->refRenderTexture->Load() )
        {
            return;
        }
    }

    // Load shader to paint it
    nShader2* shader = gfxServer->NewShader("shaders:gmmt_gen_glot.fx");
    shader->SetFilename("shaders:gmmt_gen_glot.fx");
    if ( ! shader->Load() )
    {
        return;
    }
    // And the plane mesh
    nMesh2* plane = gfxServer->NewMesh("wc:libs/system/meshes/plane.n3d2");
    plane->SetFilename("wc:libs/system/meshes/plane.n3d2");
    if ( !plane->Load() )
    {
        return;
    }

    // Get map size
    if ( ! this->terrainMaterialComponent->refHeightMap.isvalid() )
    {
        return;
    }

    // calculate side size in meters 
    float blockSize = ( this->terrainMaterialComponent->refHeightMap->GetSize() - 1 ) * this->terrainMaterialComponent->refHeightMap->GetGridScale() / this->terrainMaterialComponent->GetMaterialNumBlocks();

    // size of half texel
    float halfTexelSize = 1.0f / indexGlobalArea;

    // initialize shader and render target and matrices
    gfxServer->SetShader( shader );
    gfxServer->SetRenderTarget(0, this->refRenderTexture);
    gfxServer->PushTransform(nGfxServer2::Model, matrix44() );
    gfxServer->PushTransform(nGfxServer2::View, matrix44() );
    gfxServer->PushTransform(nGfxServer2::Projection, matrix44() );
    
    // get scene node from the class (passed through to all the leaf cells)
    n_assert( this->terrainMaterialComponent->GetComponent<ncSceneClass>() );

    // Loop through tiles
    for (int j = z0; j <= z1; j++)
    {
        for (int i = x0; i <= x1; i++)
        {
            // Begin render of tile
            gfxServer->BeginFrame();
            gfxServer->BeginScene();
            shader->Begin(true);

            shader->BeginPass( 0 );
            shader->ResetParameterSlots();
            shader->BeginParameterSlot();

            vector4 texgen;
            vector2 uv;

            // get weightmap weights texture
            nTerrainCellInfo * wmap = this->terrainMaterialComponent->GetTerrainCellInfo(i, j);            
            n_assert( wmap );

            // generate the weightmap texture
            nTexture2 * weightMapTexture = this->terrainMaterialComponent->GetWeightMapTexture(i, j, true);
            n_assert(weightMapTexture);

            // set mini weight texture
            shader->ClearParameter(nShaderState::TerrainCellWeightMap);
            shader->SetTexture(nShaderState::TerrainCellWeightMap, weightMapTexture);

            // Get weightmap indices
            const nuint8* idx = wmap->GetIndices();

            // set side size
            shader->SetFloat(nShaderState::MinDist, blockSize);

            // set half texel size in word coordinates
            shader->SetFloat(nShaderState::MaxDist, halfTexelSize);

            if (255 != idx[0])
            {
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( idx[ 0 ] );
                nTerrainMaterial* material = this->terrainMaterialComponent->GetLayerByHandle( layerHandle );
                n_assert( material );                
                shader->SetTexture( nShaderState::TerrainCellMaterial0, material->GetTexture() );
                uv = material->GetUVScale();
                texgen.z = uv.x;
                texgen.w = uv.y;
            }
            shader->SetVector4(nShaderState::TexGenS, texgen);
            texgen.set(0.0f, 0.0f, 0.0f, 0.0f);
            if (255 != idx[1])
            {
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( idx[ 1 ] );
                nTerrainMaterial* material = this->terrainMaterialComponent->GetLayerByHandle( layerHandle );
                n_assert( material );
                shader->SetTexture( nShaderState::TerrainCellMaterial1, material->GetTexture() );
                uv = material->GetUVScale();
                texgen.x = uv.x;
                texgen.y = uv.y;
            }
            if (255 != idx[2])
            {
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( idx[ 2 ] );
                nTerrainMaterial* material = this->terrainMaterialComponent->GetLayerByHandle( layerHandle );
                n_assert( material );
                shader->SetTexture( nShaderState::TerrainCellMaterial2, material->GetTexture() );
                uv = material->GetUVScale();
                texgen.z = uv.x;
                texgen.w = uv.y;
            }
            shader->SetVector4(nShaderState::TexGenT, texgen);
            texgen.set(0.0f, 0.0f, 0.0f, 0.0f);
            if (255 != idx[3])
            {
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( idx[ 3 ] );
                nTerrainMaterial* material = this->terrainMaterialComponent->GetLayerByHandle( layerHandle );
                n_assert( material );
                shader->SetTexture( nShaderState::TerrainCellMaterial3, material->GetTexture() );
                uv = material->GetUVScale();
                texgen.x = uv.x;
                texgen.y = uv.y;
            }
            if (255 != idx[4])
            {
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( idx[ 4 ] );
                nTerrainMaterial* material = this->terrainMaterialComponent->GetLayerByHandle( layerHandle );
                n_assert( material );
                shader->SetTexture( nShaderState::TerrainCellMaterial4, material->GetTexture() );
                uv = material->GetUVScale();
                texgen.z = uv.x;
                texgen.w = uv.y;
            }
            shader->SetVector4(nShaderState::TexGenR, texgen);
            texgen.set(0.0f, 0.0f, 0.0f, 0.0f);

            shader->EndParameterSlot();
            shader->CommitChanges();

            gfxServer->SetMesh(plane, plane);
            const nMeshGroup& curGroup = plane->Group(0);
            gfxServer->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
            gfxServer->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());

            gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
    
            shader->EndPass();

            shader->End();
            gfxServer->EndScene();
            gfxServer->EndFrame();

            vector2i position(i * indexGlobalArea, j * indexGlobalArea);
            rectanglei srcRect(0, 0,
                indexGlobalArea * GlobalTextureGenerationResolution - 1, 
                indexGlobalArea * GlobalTextureGenerationResolution - 1);
            rectanglei dstRect = rectanglei(0, 0, indexGlobalArea - 1, indexGlobalArea - 1) + position;

            // Copy render target texture to global texture
            this->refRenderTexture->Copy( 
                this->terrainMaterialComponent->refGlobalTexture,
                srcRect, 
                dstRect, 
                nTexture2::FILTER_TRIANGLE);
            #ifndef NGAME
            NLOG(conjurer, (0, "GenerateGlobalTexture(i=%d,j=%d)", i, j));
            #endif
        }
    }

    gfxServer->SetRenderTarget(0, 0);

    gfxServer->PopTransform(nGfxServer2::Model );
    gfxServer->PopTransform(nGfxServer2::View );
    gfxServer->PopTransform(nGfxServer2::Projection );
}

//------------------------------------------------------------------------------
