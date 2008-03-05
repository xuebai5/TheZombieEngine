#ifndef N_VEGETATIONBUILDER_H
#define N_VEGETATIONBUILDER_H

/*-----------------------------------------------------------------------------
    @file nvegetationbuilder.h
    @class nVegetationBuilder
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief nVegetaionBuilder Class is for build the growthseeds for terrain.

    (C) 2004 Conjurer Services, S.A.

*/
#include "kernel/ntypes.h"
#include "util/narray2.h"
#include "util/narray.h"
#include "ngrassgrowth/ngrowthseeds.h"

class nEntityObject;
class nEntityClass;
class ncSpatialQuadtreeCell;
class nGrowthBrush;

class nVegetationBuilder
{
    public:
        typedef enum {
            All = 0,
            Dirty,
            Selected,
        } Mode;

        /// constructor
        nVegetationBuilder();
        ///destructor
        ~nVegetationBuilder();
        /// Set  the oputdoorClass
        bool Begin(nEntityClass* outdoorClass);
        /// Create vegetation and save it
        bool Build(Mode mode = nVegetationBuilder::All);
        /// Generate the vegetation
        bool End(bool save);
        /// Create meshes
        bool UpdateMeshes(nEntityClass* outdoorClass);
        /// Create meshes
        bool UpdateHeight(nEntityClass* outdoorClass);
        /// Get cellss
        //const nArray2<nEntityObject*> &GetCellsMatrix() const;
        const nArray<nEntityObject*> &GetCellsList() const;

    private:
        struct Index
        {
            int rand;
            nuint16 x;
            nuint16 y;
        };
        /// Create a list of cell
        void makeCellList();
        /// Get the cellList
        void GetCellsList(nArray<nEntityObject*>& buildCellList, Mode mode );
        /// Load the necesary resources
        bool LoadResources();
        ///
        void UnloadResources();
        /// Create growtseeds
        void BuildByCell(nEntityObject* currentCell, int size);
        /// return the num of blocks in terrain
        int getNumBlocks();
        /// Create meshes
        bool CreateMeshes(nEntityClass* outdoorClass,bool checkSameSize);
        /// Create a random access
        void MakeRandomAccess(int numdiv, nArray<Index>& list);
        /// compare to access index
        static int __cdecl IndexSorter(const void* elm0, const void* elm1);

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
        /// create a map with meshID
        void BuildListSeed(int size, nEntityObject* currentCell,  nGrowthSeeds::BuildGroupListSeed& listSeed );
        /// Append seed and append it
        void CreateSeed(nEntityObject* currentCell,nGrowthSeeds::BuildGroupListSeed& listSeed,nGrowthSeeds* growthSeed,nGrowthBrush* brush, int x, int z , int size);
#endif
#endif
        /// return if is podible put it
        bool TestBrush(nGrowthBrush* brush, int x, int z , nArray2<nuint8>& data);
        /// Paint invalid
        void MarkInvalids(nGrowthBrush* brush, int x, int z , nArray2<nuint8>& data);

        //nArray2<nEntityObject*> cells;
        nArray<nEntityObject*> cellList;
        nEntityClass* outdoorClass;
        nEntityObject* outdoor;
        nGrowthSeeds *growthSeed;
        /// precompute the random access 
        nArray<Index> randomAccess;

};

#endif //!N_VEGETATIONBUILDER_H