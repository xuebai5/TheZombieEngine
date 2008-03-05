#ifndef NC_SCENELIGHTENVCLASS_H
#define NC_SCENELIGHTENVCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLightEnvClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for lightmap environments.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncsceneclass.h"
#include "util/nsafekeyarray.h"
#include "util/narray2.h"

//------------------------------------------------------------------------------
class ncSceneLightEnvClass : public ncSceneClass
{

    NCOMPONENT_DECLARE(ncSceneLightEnvClass,ncSceneClass);

public:
    /// constructor
    ncSceneLightEnvClass();
    /// destructor
    virtual ~ncSceneLightEnvClass();

    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// reset associations of cell to node
    void Reset();

    /// get number of lights by cell index
    int GetClassNumLightCells();

    /// get access to internal node for an index
    nSceneNode* GetClassLightCellAt(int index, int& cellId);

    /// get access to internal node for a space cell
    nSceneNode* GetClassLightByCellId(int cellId);

    /// get access to internal node for a terrain block
    nSceneNode* GetClassLightByBlock(int bx, int bz);

    /// get cell id for a terrain block
    int GetCellIdByBlock(int bx, int bz) const;

    /// get path to light and shadow textures
    void GetLightmapTexturePath(int bx, int bz, nString& lightmapPath, nString& shadowmapPath) const;
    /// get path to light and shadow textures
    void GetLightmapTexturePath(int bx, int bz, nString& lightmapPath) const;
    /// get path to light and shadow textures
    void GetGlobalLightmapTexturePath(nString& lightmapPath) const;

    /// set node path by cell index
    void SetPathByCellId(int, const char *);
    /// get node path by cell index
    const char * GetPathByCellId(int);
    /// set block size
    void SetBlockSize(int, int);
    /// get block size
    void GetBlockSize(int&, int&);
    /// set block coordinates by cell id
    void SetBlockByCellId(int, int, int);
    /// get block coordinates by cell id
    void GetBlockByCellId(int, int&, int&);

private:
    struct PathByCell
    {
        PathByCell();
        PathByCell(int, const char*);
        int bx;
        int bz;
        int cellId;
        nDynAutoRef<nSceneNode> refCellNode;
    };
    /// cellId=scenePath
    nSafeKeyArray<PathByCell> pathByCellArray;
    /// bx,bx=cellId
    nArray2<int> pathIndexByBlock;
};

//------------------------------------------------------------------------------
/**
*/
inline
ncSceneLightEnvClass::PathByCell::PathByCell() :
    bx(-1),
    bz(-1),
    cellId(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSceneLightEnvClass::PathByCell::PathByCell(int id, const char* path) :
    bx(-1),
    bz(-1),
    cellId(id),
    refCellNode(path)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLightEnvClass::GetClassNumLightCells()
{
    return this->pathByCellArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLightEnvClass::GetCellIdByBlock(int bx, int bz) const
{
    if (this->pathIndexByBlock.ValidIndex(bx, bz))
    {
        return this->pathIndexByBlock.At(bx, bz);
    }
    return -1;
}

//------------------------------------------------------------------------------
#endif
