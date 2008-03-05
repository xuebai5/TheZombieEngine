#ifndef N_GROWTHSEEDS_H
#define N_GROWTHSEEDS_H

/*-----------------------------------------------------------------------------
    @file ngrowthseeds.h
    @class nGrowthSeeds
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief This a resource for load and save 

    (C) 2004 Conjurer Services, S.A.

*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "resource/nresource.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Class nGrowthSeeds
*/
class nGrowthSeeds : public nResource
{
public:

    #pragma pack(push)
    #pragma pack(1)
    class Seed 
    {
    private: // Not change ther order because msvc not pack it
        unsigned int posx : 10;
        unsigned int posz : 10;
        unsigned int meshid : 8;
        unsigned int scale:  4;

        unsigned int posy : 10;
        unsigned int roty :  6;
        unsigned int norx : 8;
        unsigned int norz : 8;

        friend class nGrowthSeeds;
    };
    #pragma pack(pop)

    typedef nFixedArray<Seed> ListSeed;
    typedef nFixedArray<ListSeed> GroupListSeed;
    /// constructor
    nGrowthSeeds();
    ///destructor
    virtual ~nGrowthSeeds ();
    /// persistence of nebula
    virtual bool SaveCmds(nPersistServer* ps);
    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();
    /// check if asynchronous loading is allowed
    virtual bool CanLoadAsync() const;
    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

    const ListSeed& GetSeedsByGroup(int idx) const;
    const GroupListSeed& GetSeeds() const;

    const matrix44 GetMatrix(const Seed& seed) const;
    /// Set the position
    void SetPosition(Seed& seed, const vector3& pos) const;
    /// return the position
    vector3 GetPosition(const Seed& seed) const;
    /// Set the normal
    void SetNormal(Seed& seed, const vector3& pos) const;
    /// return the normal
    vector3 GetNormal(const Seed& seed) const;
    /// Set rotation
    void SetRotationY(Seed& seed,float rot) const;
    /// return the rotation
    float GetRotationY(const Seed& seed) const;
    /// Set rotation
    void SetScale(Seed& seed,float scalew) const;
    /// return the rotation
    float GetScale(const Seed& seed) const;
    /// Set the mesh id 
    void SetMeshId(Seed& seed , int meshId) const;
    /// return the mesh id
    int GetMeshId(const Seed& seed ) const;

private:
    bool LoadBinaryFile(nFile* file);
    /// The minimun position of cell
    vector3 min;
    /// The fator;
    vector3 factor;

    GroupListSeed allList;
    int size;

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    public:
         /// Type for create the seeds , not has a static list
        typedef nFixedArray< nArray<Seed> > BuildGroupListSeed;
        // Creation
        ///  CreateList 
        void CreateList(const BuildGroupListSeed& buildList);
        /// Set the cell size
        void SetCellSize( const vector3& min, const vector3& max);
        /// save a bytemap
        bool Save();
    private:
       bool SaveBinaryFile(nFile* file);

    #endif
    #endif

};

#endif //N_GROWTHSEEDS_H