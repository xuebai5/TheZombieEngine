#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  ncscenelightenv_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenelightenvclass.h"
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSceneLightEnvClass,ncSceneClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneLightEnvClass)
    NSCRIPT_ADDCMD_COMPCLASS('MSPC', void, SetPathByCellId, 2, (int, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGPC', const char *, GetPathByCellId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSBS', void, SetBlockSize, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBS', void, GetBlockSize, 0, (), 2, (int&, int&));
    NSCRIPT_ADDCMD_COMPCLASS('MSBC', void, SetBlockByCellId, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBC', void, GetBlockByCellId, 1, (int), 2, (int&, int&));
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSceneLightEnvClass::ncSceneLightEnvClass() :
    pathByCellArray(32, 32)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneLightEnvClass::~ncSceneLightEnvClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::Reset()
{
    this->pathByCellArray.Clear();
}

//------------------------------------------------------------------------------
/**
    create a subscene path for the cell index
*/
void
ncSceneLightEnvClass::SetPathByCellId(int cellId, const char* relPath)
{
    if (this->pathByCellArray.HasKey(cellId))
    {
        this->pathByCellArray.GetElement(cellId).refCellNode = relPath;
    }
    else
    {
        this->pathByCellArray.Add(cellId, PathByCell(cellId, relPath));
    }
}

//------------------------------------------------------------------------------
/**
    find the subscene path for the cell index
*/
const char *
ncSceneLightEnvClass::GetPathByCellId(int cellId)
{
    if (this->pathByCellArray.HasKey(cellId))
    {
        return this->pathByCellArray.GetElement(cellId).refCellNode.getname();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    TEMPORARY! this is to attach all cells for the light
*/
nSceneNode*
ncSceneLightEnvClass::GetClassLightCellAt(int index, int& cellId)
{
    if (index < this->pathByCellArray.Size())
    {
        nSceneNode* sceneRoot = this->GetComponentSafe<ncSceneClass>()->GetRootNode();
        nKernelServer::ks->PushCwd(sceneRoot);
        sceneRoot = this->pathByCellArray.GetElementAt(index).refCellNode.get();
        nKernelServer::ks->PopCwd();
        cellId = this->pathByCellArray.GetKeyAt(index);
        return sceneRoot;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    find the subscene node for the cell index
*/
nSceneNode*
ncSceneLightEnvClass::GetClassLightByCellId(int cellId)
{
    if (this->pathByCellArray.HasKey(cellId))
    {
        nSceneNode* sceneRoot = this->GetComponentSafe<ncSceneClass>()->GetRootNode();
        nKernelServer::ks->PushCwd(sceneRoot);
        sceneRoot = this->pathByCellArray.GetElement(cellId).refCellNode.get();
        nKernelServer::ks->PopCwd();
        return sceneRoot;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    expose access to internal nodes by block coordinates
    to allow the texture cache to set and clear lightmap textures
*/
nSceneNode*
ncSceneLightEnvClass::GetClassLightByBlock(int bx, int bz)
{
    if (this->pathIndexByBlock.ValidIndex(bx, bz))
    {
        return this->GetClassLightByCellId(this->pathIndexByBlock.At(bx, bz));
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::SetBlockSize(int width, int height)
{
    this->pathIndexByBlock.SetSize(width, height);
    for (int bx = 0; bx < width; ++bx)
    {
        for (int bz = 0; bz < height; ++bz)
        {
            this->pathIndexByBlock.Set(bx, bz, -1);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::GetBlockSize(int& bx, int& bz)
{
    bx = this->pathIndexByBlock.GetWidth();
    bz = this->pathIndexByBlock.GetHeight();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::SetBlockByCellId(int cellId, int bx, int bz)
{
    if (this->pathByCellArray.HasKey(cellId) && this->pathIndexByBlock.ValidIndex(bx, bz))
    {
        this->pathIndexByBlock.Set(bx, bz, cellId);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::GetBlockByCellId(int cellId, int& bx, int& bz)
{
    if (this->pathByCellArray.HasKey(cellId))
    {
        uint x, z;
        for (x = 0; x < this->pathIndexByBlock.GetWidth(); ++x)
        {
            for (z = 0; z < this->pathIndexByBlock.GetHeight(); ++z)
            {
                if (cellId == this->pathIndexByBlock.At(x, z))
                {
                    bx = x;
                    bz = z;
                    return;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @todo ma.garcias    - move this to a more proper terrainlightloader class
*/
void
ncSceneLightEnvClass::GetLightmapTexturePath(int bx, int bz, nString& lightmapPath, nString& shadowmapPath) const
{
    const ncLoaderClass* loader = this->GetComponentSafe<ncLoaderClass>();

    lightmapPath = loader->GetResourceFile();
    lightmapPath.StripExtension();
    lightmapPath.Append("/textures/");
    shadowmapPath = lightmapPath;

    lightmapPath.Append("lightmap");
    lightmapPath.AppendInt( bx );
    lightmapPath.Append("_");
    lightmapPath.AppendInt( bz );
    lightmapPath.Append(".dds");

    shadowmapPath.Append("shadowmap");
    shadowmapPath.AppendInt( bx );
    shadowmapPath.Append("_");
    shadowmapPath.AppendInt( bz );
    shadowmapPath.Append(".tga");
}

//------------------------------------------------------------------------------
/**
    @todo ma.garcias    - move this to a more proper terrainlightloader class
*/
void
ncSceneLightEnvClass::GetLightmapTexturePath(int bx, int bz, nString& lightmapPath) const
{
    const ncLoaderClass* loader = this->GetComponentSafe<ncLoaderClass>();

    lightmapPath = loader->GetResourceFile();
    lightmapPath.StripExtension();
    lightmapPath.Append("/textures/");

    lightmapPath.Append("lightmap");
    lightmapPath.AppendInt( bx );
    lightmapPath.Append("_");
    lightmapPath.AppendInt( bz );
    lightmapPath.Append(".dds");
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightEnvClass::GetGlobalLightmapTexturePath( nString& lightmapPath) const
{
    const ncLoaderClass* loader = this->GetComponentSafe<ncLoaderClass>();
    lightmapPath = loader->GetResourceFile();
    lightmapPath.StripExtension();
    lightmapPath.Append("/textures/lightmapglobal.dds");
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLightEnvClass::SaveCmds(nPersistServer* ps)
{
    if (ncSceneClass::SaveCmds(ps))
    {
        for (int i = 0; i < this->GetClassNumLightCells(); ++i)
        {
            //--- setpathbycellid ---
            int cellId = this->pathByCellArray.GetKeyAt(i);
            const char *path = this->pathByCellArray.GetElementAt(i).refCellNode.getname();
            ps->Put(this->GetEntityClass(), 'MSPC', cellId, path);
        }

        if (this->pathIndexByBlock.GetWidth() > 0 && this->pathIndexByBlock.GetHeight())
        {
            //--- setblocksize ---
            ps->Put(this->GetEntityClass(), 'MSBS', this->pathIndexByBlock.GetWidth(), this->pathIndexByBlock.GetHeight());
        }

        for (uint bz = 0; bz < this->pathIndexByBlock.GetHeight(); ++bz)
        {
            for (uint bx = 0; bx < this->pathIndexByBlock.GetWidth(); ++bx)
            {
                int cellId = this->pathIndexByBlock.At(bx, bz);
                if (cellId != -1)
                {
                    //--- setblockbycellid ---
                    ps->Put(this->GetEntityClass(), 'MSBC', cellId, bx, bz);
                }
            }
        }

        return true;
    }

    return false;
}
