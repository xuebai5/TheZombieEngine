#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  nterrainmaterial_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngeomipmap/nterrainmaterial.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nlogclass.h"
#include "tools/ntexturebuilder.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"
#include "gameplay/ngamematerialserver.h"
#include "gameplay/ngamematerial.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nTerrainMaterial, "nobject");

//------------------------------------------------------------------------------
/**
*/
nTerrainMaterial::nTerrainMaterial():
    textureFileName( 0 ),
    projectionType( ProjXZ ),
    label("Unnamed material"),
    scaleUV(1.0f, 1.0f),
    importMaterialState(false)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nTerrainMaterial::~nTerrainMaterial()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainMaterial::LoadResources(void)
{
    // Create material texture resource
    this->GetTexture();

    return true;
}

//------------------------------------------------------------------------------
/**
    @todo Method don't used, the material texture isn't modified.
*/
bool
nTerrainMaterial::SaveResources(void)
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainMaterial::UnloadResources(void)
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Unload();
        this->refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get texture pointer
*/
nTexture2*
nTerrainMaterial::GetTexture()
{
    n_assert( ! this->textureFileName.IsEmpty() );

    // Create texture
    if (!this->refTexture.isvalid())
    {
        this->refTexture = nGfxServer2::Instance()->NewTexture( this->textureFileName.Get() );
        n_assert(this->refTexture.isvalid());
    }

    // Load it
    if (this->refTexture.isvalid() && !this->refTexture->IsLoaded())
    {
        this->refTexture->SetFilename( this->textureFileName.Get() );
        if (!this->refTexture->Load() )
        {
            NLOG(resource, (NLOGUSER | 0, "Error loading texture '%s'", this->textureFileName.Get()));
            return 0;
        }
    }

    return this->refTexture.get();
}

//------------------------------------------------------------------------------
/**
    @brief Save a texture thumbnail of this material and return its path
*/
nString
nTerrainMaterial::GetTextureThumbnail( int size )
{
    nString fileName("outgui:images/terrain/materialthumb");
    fileName.AppendInt(this->layerHnd);
    fileName.Append(".bmp");

    if ( nTextureBuilder::LoadResizeSaveImage(  this->GetTexture()->GetFilename(), fileName, size) )
    {
        return fileName;
    }
    else
    {
        return nString("");
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get texture filename
*/
const nString &
nTerrainMaterial::GetTextureFileName()
{
    return this->textureFileName;
}
//------------------------------------------------------------------------------
/**
    @brief Get layer handle
*/
int
nTerrainMaterial::GetLayerHandle()
{
    return layerHnd;
}
//------------------------------------------------------------------------------
/**
    @brief Get projection type
*/
int
nTerrainMaterial::GetProjection()
{
    return projectionType;
}
//------------------------------------------------------------------------------
/**
    @brief Get UV scale
*/
vector2
nTerrainMaterial::GetUVScale()
{
    return scaleUV;
}
//------------------------------------------------------------------------------
/**
    @brief Get color mask
*/
vector4
nTerrainMaterial::GetMaskColor()
{
    return maskColor;
}
//------------------------------------------------------------------------------
/**
    @brief Get label
*/
nString
nTerrainMaterial::GetLabel()
{
    return label;
}
//------------------------------------------------------------------------------
/**
    @brief Set texture filename
*/
void
nTerrainMaterial::SetTextureFileName( const char * fileName )
{
    if ( nFileServer2::Instance()->FileExists(fileName) )
    {
        this->textureFileName.Set( fileName );
        this->refTexture.invalidate();
    }
}
//------------------------------------------------------------------------------
/**
    @brief Set layer handle
*/
void
nTerrainMaterial::SetLayerHandle( int lh )
{
    // ignore this command when importing
    if (this->importMaterialState)
    {
        return;
    }

    this->layerHnd = lh;
}
//------------------------------------------------------------------------------
/**
    @brief Set projection type
*/
void
nTerrainMaterial::SetProjection( int proj )
{
    n_assert( proj >= ProjXZ && proj <= ProjZY );
    this->projectionType = static_cast<ProjectionType>(proj);
}
//------------------------------------------------------------------------------
/**
    @brief Set UV scale
*/
void
nTerrainMaterial::SetUVScale( vector2 sUV )
{
    this->scaleUV = sUV;
}
//------------------------------------------------------------------------------
/**
    @brief Set mask color
*/
void
nTerrainMaterial::SetMaskColor( vector4 col )
{
    this->maskColor = col;
}
//------------------------------------------------------------------------------
/**
    @brief Set label
*/
void
nTerrainMaterial::SetLabel( nString lab)
{
    this->label = lab;
}
//------------------------------------------------------------------------------
/**
    Set the terrain cell material transform for U texture coordinate so we can
    calculate the U texture coordinate by dot product:
    u = transformU.xyzw * pos.xyzw

    The cell position is required to work with coordinate relative to the block.
*/
void 
nTerrainMaterial::GetTerrainCellTransformU(const vector4 & cellpos, vector4 & transform) const
{
    switch(this->projectionType)
    {
    case ProjXZ:
        transform.x = this->scaleUV.x;
        transform.y = 0;
        transform.z = 0;
        transform.w = - cellpos.x;
        break;
    case ProjXY:
        transform.x = this->scaleUV.x;
        transform.y = 0;
        transform.z = 0;
        transform.w = - cellpos.x;
        break;
    case ProjZY:
        transform.x = 0;
        transform.y = 0;
        transform.z = this->scaleUV.x;
        transform.w = - cellpos.z;
        break;
    default:
        n_assert_always();
        break;
    }
}

//------------------------------------------------------------------------------
/**
    Set the terrain cell material transform for V texture coordinate so we can
    calculate the V texture coordinate by dot product:
    v = transformV.xyzw * pos.xyzw

    The cell position is required to work with coordinate relative to the block.
*/
void 
nTerrainMaterial::GetTerrainCellTransformV(const vector4 & cellpos, vector4 & transform) const
{
    switch(this->projectionType)
    {
    case ProjXZ:
        transform.x = 0;
        transform.y = 0;
        transform.z = this->scaleUV.y;
        transform.w = - cellpos.z;
        break;
    case ProjXY:
        transform.x = 0;
        transform.y = this->scaleUV.y;
        transform.z = 0;
        transform.w = 0;
        break;
    case ProjZY:
        transform.x = 0;
        transform.y = this->scaleUV.y;
        transform.z = 0;
        transform.w = 0;
        break;
    default:
        n_assert_always();
        break;
    }
}

//------------------------------------------------------------------------------
/**
    Set to change from normal state to import material state and viceversa.
    When the material is in import material state, simply ignores some commands,
    which correspond to data that must not be imported.
*/
void 
nTerrainMaterial::SetImportMaterialState(bool state)
{
    this->importMaterialState = state;
}

//------------------------------------------------------------------------------
/** 
    Set layer mask (floatmap)
    OBSOLETE
*/
void
nTerrainMaterial::SetLayerPath(const nString & /*fmPath*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Get layer mask (floatmap)
    OBSOLETE
*/
const nString &
nTerrainMaterial::GetLayerPath() const
{
    n_assert2_always("Obsolete function nTerrainMaterial::GetLayerPath called");
    //dummy return
    return this->textureFileName;
}

//------------------------------------------------------------------------------
/** 
    Set use layer mask (floatmap)
    OBSOLETE
*/
void
nTerrainMaterial::SetLayerUsePath(const nString & /*fmPath*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Get use layer mask (floatmap)
    OBSOLETE
*/
const nString &
nTerrainMaterial::GetLayerUsePath() const
{
    n_assert2_always("Obsolete function nTerrainMaterial::GetLayerUsePath called");
    // dummy return
    return this->textureFileName;
}


//------------------------------------------------------------------------------
/**
*/
void
nTerrainMaterial::SetGameMaterialByName(const char * strName)
{
    this->gameMaterialName = strName;
    this->gameMaterial = nGameMaterialServer::Instance()->GetMaterialByName(this->gameMaterialName);
}

//------------------------------------------------------------------------------
/**
*/
const char *
nTerrainMaterial::GetGameMaterialName()
{
    return this->gameMaterialName.Get();
}

//------------------------------------------------------------------------------
/**
*/
nGameMaterial *
nTerrainMaterial::GetGameMaterial()
{
    return this->gameMaterial.get_unsafe();
}

//------------------------------------------------------------------------------
