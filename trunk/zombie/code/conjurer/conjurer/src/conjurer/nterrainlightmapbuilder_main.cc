#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterrainlightmapbuilder_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nterrainlightmapbuilder.h"
//------------------------------------------------------------------------------
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmclass.h"
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/ncspatialbatch.h"
//------------------------------------------------------------------------------
#include "nscene/nlightnode.h"
#include "nscene/nshadowlightnode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/ncscenelightenvclass.h"
#include "nscene/nlightenvnode.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
//------------------------------------------------------------------------------
#include "tools/ntexturebuilder.h"
#include "resource/nresourceserver.h"
//------------------------------------------------------------------------------
#include "ndebug/nceditorclass.h"
//------------------------------------------------------------------------------
#include "kernel/nlogclass.h"
#include "mathlib/rectanglei.h"
//------------------------------------------------------------------------------

NCREATELOGLEVEL ( lightmap , "Lightmap Generation " , false , 2 )

//------------------------------------------------------------------------------
/**
*/
nTerrainLightMapBuilder::nTerrainLightMapBuilder():
    lightMapClass(0),
    lightMapClassName("LightMap_Test"),
    lightMapObject(0),
    libSceneRoot(0),
    shadowMapTextures(0,1),
    lightId(0),
    lightMapSize(512),
    shadowMapSize(512),
    globalLightMapSize(512),
    overwriteExistingFiles(true),
    maxDistance(0),
    offset( vector2(0.f,0.f) ),
    antiAliasingDiskSize( 1.f)
{
    //empty
}
//------------------------------------------------------------------------------
/**
*/
nTerrainLightMapBuilder::nTerrainLightMapBuilder(const char* lightClassName, int lightMapSize, int shadowMapSize, int globalLightMapsize, nEntityObjectId lightId, const float maxDistance , const vector2 & offset, const float diskSize):
    lightMapClass(0),
    lightMapClassName(lightClassName),
    lightMapObject(0),
    libSceneRoot(0),
    shadowMapTextures(0,1),
    lightId(lightId),
    lightMapSize(lightMapSize),
    shadowMapSize(shadowMapSize),
    globalLightMapSize(globalLightMapsize),
    overwriteExistingFiles(true),
    maxDistance(maxDistance),
    offset( offset ),
    antiAliasingDiskSize( diskSize)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nTerrainLightMapBuilder::~nTerrainLightMapBuilder()
{
    if ( this->refRenderTarget.isvalid())
    {
        this->refRenderTarget->Release();
    }

    if ( this->refShadowRenderTarget.isvalid())
    {
        this->refShadowRenderTarget->Release();
    }

    if ( this->refViewportObject.isvalid() )
    {
        this->refViewportObject->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::SceneShadowMapData(nEntityObject* currentCell, nString& texturefileName, const vector4& orthogonal, const vector3& euler, const vector3& position ,const vector4& deformation)
{
    bbox3 bbox = currentCell->GetComponentSafe<ncSpatialQuadtreeCell>()->GetBBox();
    vector3 min = bbox.corner_point(0); //min
    vector3 max = bbox.corner_point(4); //max

    nKernelServer*  ks = nKernelServer::ks;
    ks->PushCwd(this->libSceneRoot);

    nShadowLightNode* shadowLightNode;

    nString fileName = texturefileName.ExtractFileName();
    fileName.StripExtension();

    shadowLightNode = (nShadowLightNode*) ks->New("nshadowlightnode" , fileName.Get() );
    n_assert(shadowLightNode);

    shadowLightNode->SetVector(nShaderState::TerrainCellMaxMinPos, vector4(min.x, min.z, max.x, max.z)); 

    NLOG ( lightmap , ( 0 , "SceneShadowMapData: cell_center: %f %f %f", position.x, position.y, position.z) );
    NLOG ( lightmap , ( 0 , "SceneShadowMapData: euler: %f %f %f", euler.x, euler.y, euler.z) );
    NLOG ( lightmap , ( 0 , "SceneShadowMapData: orthogonal: %f %f %f %f", orthogonal.x, orthogonal.y, orthogonal.z, orthogonal.w) );
    NLOG ( lightmap, (0, "\n"));

    shadowLightNode->SetOrthogonal(orthogonal.x, orthogonal.y, orthogonal.z, orthogonal.w);
    shadowLightNode->SetEuler(euler);
    shadowLightNode->SetPosition(position);
    shadowLightNode->SetDeformation(deformation);

    ks->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachCurrentCellEntities( nSceneGraph* sceneGraph, nEntityObject* currentCell, bool isShadowMap)
{
    //attach current cell
    currentCell->GetComponent<ncScene>()->Render(sceneGraph);
    currentCell->GetComponent<ncScene>()->SetPassEnabled("sdep", false);

    // render elements visible for this camera
    ncSpatialQuadtreeCell *quadtreeCell = currentCell->GetComponent<ncSpatialQuadtreeCell>();
    //sceneGraph->Attach( currentCell);

    if( isShadowMap )
    {
        while( quadtreeCell )
        {
            const nArray<nEntityObject*> *categories = quadtreeCell->GetCategories();

            //generate static shadows for all brushes with the "ShadowStatic" property
            for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
            {
                this->AttachEntityArray(sceneGraph, categories[catIndex]);
            }

            quadtreeCell = quadtreeCell->GetParentCell();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachEntityArray(nSceneGraph* sceneGraph, const nArray<nRef<nEntityObject> >& entities)
{
    int i;
    for (i = 0; i < entities.Size(); ++i)
    {
        if (entities[i].isvalid())
        {
            this->AttachShadowEntity(sceneGraph, entities[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachEntityArray(nSceneGraph* sceneGraph, const nArray<nEntityObject*>& entities)
{
    int i;
    for (i = 0; i < entities.Size(); ++i)
    {
        this->AttachShadowEntity(sceneGraph, entities[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachShadowEntity(nSceneGraph* sceneGraph, nEntityObject* entity)
{
    ncSpatialBatch* batchComp = entity->GetComponent<ncSpatialBatch>();
    if (batchComp)
    {
        //attach batched entities (batches don't cast shadows)
        if (batchComp->LoadSubentities())
        {
            this->AttachEntityArray(sceneGraph, batchComp->GetSubentities());
        }
    }
    else
    {
        //attach shadow-static entities
        ncEditorClass* editorComp = entity->GetClassComponent<ncEditorClass>();
        if (editorComp && editorComp->IsSetClassKey("ShadowStatic") && 
            editorComp->GetClassKeyInt("ShadowStatic") == 1)
        {
            ncScene *sceneComponent = entity->GetComponent<ncScene>();
            if (sceneComponent)
            {
                // set lod index to the maximum
                sceneComponent->SetAttachIndex(0);
                sceneComponent->Render(sceneGraph);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachLightEntity(nSceneGraph* sceneGraph)
{
    ncScene *sceneComponent = this->refLightObject->GetComponent<ncScene>();
    if (sceneComponent)
    {
        sceneComponent->Render(sceneGraph);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::AttachViewport(nSceneGraph* sceneGraph)
{
    ncScene *sceneComponent = this->refViewportObject->GetComponent<ncScene>();
    if (sceneComponent)
    {
        sceneComponent->Render(sceneGraph);
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nTerrainLightMapBuilder::ComputeLightMapResolution()
{
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();

    return (terrainClass->GetHeightMapSize()-1)*terrainClass->GetPointSeparation()/terrainClass->GetNumBlocks();    
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
nTerrainLightMapBuilder::CopyTexture( nRef<nTexture2> from)
{
    //copy texture
    ushort width = from->GetWidth();
    ushort height = from->GetHeight();
    nTexture2* to = 0;
    to = 0;
    to = nGfxServer2::Instance()->NewTexture(0);
    n_assert(to);
    to->SetType( nTexture2::TEXTURE_2D );
    to->SetFormat( nTexture2::A8R8G8B8 );
    to->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic);
    to->SetWidth( width );
    to->SetHeight( height );
    to->Load();
    n_assert( to->IsLoaded() );

    rectanglei rect( 0, 0, width-1, height-1 );
    from->Copy( to,rect, rect, nTexture2::FILTER_POINT );
    return to;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::SaveLightMapTexture(int bx, int bz)
{
    nString lightTextureName = this->GetLightmapTextureName(bx, bz);
    nTextureBuilder texBuilder;
    texBuilder.SetTexture(refRenderTarget);
    bool success = texBuilder.Save(lightTextureName.Get()); // The texture builder always save in dxt5 format
    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::SaveShadowMapTexture(int bx, int bz)
{
    nString shadowTextureName(this->GetShadowTextureName(bx, bz));
    nTextureBuilder texBuilder;
    texBuilder.SetTexture(this->refShadowRenderTarget.get() );
    bool success = texBuilder.Save(shadowTextureName.Get()); // The texture builder always save in dxt5 format
    return success;
}

//------------------------------------------------------------------------------
/**
*/
nString
nTerrainLightMapBuilder::GetShadowTextureName(int bx, int bz)
{
    //nString shadowTextureName(this->shadowTexturePath + bx + "_" + bz + ".dds");
    nString shadowTextureName(this->shadowTexturePath + bx + "_" + bz + ".tga");
    return shadowTextureName;
    
}

//------------------------------------------------------------------------------
/**
*/
nString
nTerrainLightMapBuilder::GetLightmapTextureName(int bx, int bz)
{
    nString lightmapTextureName(this->lightTexturePath + bx + "_" + bz + ".dds");
    return lightmapTextureName;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::ConvertLightNameToValidClassName()
{
    // Check class name
    if ( !nEntityClassServer::Instance()->ValidClassName( this->lightMapClassName ) )
    {
        // Try to set the class name okay
        int n = this->lightMapClassName.Length();
        if ( n > 0 )
        {
            this->lightMapClassName[0] = (char) toupper( this->lightMapClassName[0] );
            for (int i=1; i<n; i++)
            {
                this->lightMapClassName[i] = (char) tolower( this->lightMapClassName[i] ); 
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::SaveGlobalShadowMapTexture()
{
    nString shadowTextureName = this->shadowTexturePath + "global" + ".dds";

    nTexture2* shadowMap = this->CopyTexture( this->refShadowRenderTarget );

    nTextureBuilder texBuilder;
    texBuilder.SetTexture( shadowMap );
    texBuilder.Save( shadowTextureName.Get() );
}

//------------------------------------------------------------------------------
/**
    copy all terrain blocks into the array if no selection
*/
void
nTerrainLightMapBuilder::GetCells(nArray<nEntityObject*>& terrainBlocks)
{
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    ncTerrainGMM* terrainGMM = this->refOutDoorObject->GetComponentSafe<ncTerrainGMM>();
    ncTerrainGMMCell* firstCell;
    terrainBlocks.Clear();
    if (! this->selectedCells.Empty())
    {
        terrainBlocks = this->selectedCells;

    } else
    {
        int numBlocks = terrainClass->GetNumBlocks();
        firstCell = terrainGMM->GetNorthWestCell();
        for (int bz = 0; bz < numBlocks; bz++)
        {
            ncTerrainGMMCell * currentCell = firstCell;
            for(int bx = 0; bx < numBlocks; bx++)
            {
                n_assert(currentCell);
                terrainBlocks.Append(currentCell->GetEntityObject());
                currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
            }
            firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/  
nString 
nTerrainLightMapBuilder::TerrainSetNewSurface(nFourCC passFourCC , const char* shaderFileName)
{
    ncSceneClass* sceneClass = this->refOutDoorObject->GetClassComponentSafe<ncSceneClass>();
    nGeometryNode* geometryNode = static_cast<nGeometryNode*>(sceneClass->GetRootNode());
    n_assert(geometryNode->IsA("ngeometrynode"));
    nSurfaceNode* surfaceNode = geometryNode->GetSurfaceNode();

    nString prevSurface(geometryNode->GetSurface());
    nKernelServer::ks->PushCwd(geometryNode);
    surfaceNode = static_cast<nSurfaceNode*>(nKernelServer::ks->New("nsurfacenode", "terrainLightmapSurface"));
    nKernelServer::ks->PopCwd();

    surfaceNode->SetShader( passFourCC , shaderFileName);
    geometryNode->SetSurface(surfaceNode->GetFullName().Get());
    geometryNode->GetSurfaceNode()->LoadResources();
    geometryNode->LoadResources();

    return prevSurface;
}

//------------------------------------------------------------------------------
/**
*/  
void 
nTerrainLightMapBuilder::TerrainRestoreSurface(const nString& prevSurface)
{
    ncSceneClass* sceneClass = this->refOutDoorObject->GetClassComponentSafe<ncSceneClass>();
    nGeometryNode* geometryNode = static_cast<nGeometryNode*>(sceneClass->GetRootNode());

    // restore geometry node original surface
    nSurfaceNode* surfaceNode = geometryNode->GetSurfaceNode();
    surfaceNode->Release();

    geometryNode->SetSurface(prevSurface.Get());
    geometryNode->GetSurfaceNode();
    geometryNode->LoadResources();
}


//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------

