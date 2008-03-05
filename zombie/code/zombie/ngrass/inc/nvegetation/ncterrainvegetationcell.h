#ifndef N_NCTERRAINVEGETATIONCELL_H
#define N_NCTERRAINVEGETATIONCELL_H
//------------------------------------------------------------------------------
/**
    @class ncTerrainVegetation
    @ingroup ncTerrainVegetation

    Component for show and edit the terrain vegetation

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "kernel/nref.h"
#include "ngeomipmap/nbytemap.h"
#include "gfx2/ntexture2.h"

class nByteMap;
class nDynamicMesh;

//------------------------------------------------------------------------------
class ncTerrainVegetationCell : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTerrainVegetationCell,nComponentObject);

public:

    struct Growth
    {
        int  mesh;
        matrix44 matrix;
    };

    struct SubCell
    {
        nFixedArray<Growth> growth;
    };

    /// constructor
    ncTerrainVegetationCell();
    /// destructor
    ~ncTerrainVegetationCell();
    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Load the mesh in dynamic mesh
    void LoadMeshForSubCell(int idx_subcell ,nDynamicMesh* dynMesh);
    /// return the index of sucell
    void getSubCellIndex(float x, float z , int &indexX , int& indexZ);
    /// return the index of subcell
    int getSubGroupIdx(int x, int z);

    /// Create the material list called it when create the class first time
    /// @fixme cristobal.castillo see ticket #2645

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
        /// load resources
        bool LoadEditionResources(void);
        /// load resources
        bool CreateEditionResources(); 
        /// unload resources
        void UnloadEditionResources(void);
        /// are resources loaded & valid
        bool IsValidEditionResources();
        /// save edition resources
        bool SaveEditionResources(void);
        /// return the growthmap , null if it's no loaded
        nByteMap* GetGrowthMap();
        /// Rerturn the growthMap, Load it or create it if is necesary
        nByteMap* GetValidGrowthMap();

        /// Get undo growth map
        nByteMap* GetUndoGrowthMap();

        ///  copy the bytemap to texture before render;
        void UpdateTextureBeforeRender(bool val = true);
        /// return if is necesary update the texture
        bool GetUpdateTextureBeforeRender();

    #endif //#ifndef NGAME
    #endif //#ifndef __ZOMBIE_EXPORTER__

        #ifndef NGAME
        #ifndef __ZOMBIE_EXPORTER__
        #endif //#ifndef NGAME
        #endif //#ifndef __ZOMBIE_EXPORTER__

private:
    nArray<SubCell> subCell;

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
        nRef<nByteMap> refGrowthMap;
        nRef<nByteMap> refUndoGrowthMap;
        //nRef<nTexture2> refTexture;
        bool updateTexture;
        /// Create the growth map 
        void CreateRefGrowthMap();
    #endif //#ifndef NGAME
    #endif //#ifndef __ZOMBIE_EXPORTER__

};

//------------------------------------------------------------------------------
#endif//N_NCTERRAINVEGETATIONCell_H
