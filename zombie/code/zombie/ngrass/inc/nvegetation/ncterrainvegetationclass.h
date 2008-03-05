#ifndef N_NCTERRAINVEGETATIONCLASS_H
#define N_NCTERRAINVEGETATIONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncTerrainVegetation
    @ingroup ncTerrainVegetation

    Component for show and edit the terrain vegetation

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "kernel/nref.h"
#include "nvegetation/ngrowthseedcacheentry.h"
#include "nvegetation/nvegetationmeshcacheentry.h"
#include "nvegetation/nvegetationnode.h"

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
class nVegetationBuilder;
#include "nvegetation/ngrowthtexturecacheentry.h"
#include "ngrassgrowth/ngrowthmaterial.h"
#endif //!__ZOMBIE_EXPORTER__
#endif //!NGAME

//------------------------------------------------------------------------------
class nMesh2;
class nGrowthSeeds;

//------------------------------------------------------------------------------
class ncTerrainVegetationClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncTerrainVegetationClass,nComponentClass);

public:

    struct SubCellId
    {
        union
        {
            nuint32 id;
            struct Cell
            {
                nint16 subCellx,subCellz;
            } cell;
        };
        nint16 desp;
        SubCellId();
        /// Create from unique id, the desp is the GetVegetationCellSubDivision of ncTerrainVegetationClass
        SubCellId( int id , int desp );
        /// Create from cell and relative subcell
        //SubCellId( int x, int z, int relx, int relz, int desp );
        /// Create from absolute subcell 
        SubCellId( int subCellx, int subCellz, int desp);
        /// return the cell index and relative subcell
        void Get( int& x, int& z , int& relx, int& relz ) const;
        /// return the cell index
        void GetCellIndex( int& x, int& z) const;
        /// return the subcell id of neighbour
        SubCellId GetNeighbour( int x , int z) const;
    };

    struct SubCellMesh
    {
        nMesh2* mesh;
        SubCellId id;
        nEntityObject *cell;
        int numIndices;
        SubCellMesh();
        ~SubCellMesh();
        void CalculateLod(nSceneGraph *sceneGraph, ncTerrainVegetationClass* vegClass); // Distance in meters
        static int __cdecl CompareByBlock(const void* elm0, const void* elm1);
    };

    /// constructor
    ncTerrainVegetationClass();
    /// destructor
    ~ncTerrainVegetationClass();
    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Nebula class persistence
    bool SaveCmds(nPersistServer * ps);
    /// load resources
    bool LoadResources();
    /// unload resources
    #if defined(NGAME) || defined(__ZOMBIE_EXPORTER__)
    void UnloadResources();
    #else
    void UnloadResources(bool exitGrowthEdition = true);
    #endif
    /// are resources loaded & valid
    bool IsValid();
    /// load resources
    bool LoadScene();
    /// unload resources
    void UnloadScene();
    /// return the nVegetation node
    nVegetationNode* GetVegetationNode() const;
    /// type of array meshes
    typedef nArray<SubCellMesh> MeshList;
    /// load the resource for scene
    bool LoadSceneResources();
    /// unload the resource for scene
    void UnloadSceneResources();
    /// Calculate meshes
    void Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject);
    /// Rerturn a list of visivble mesh , this method use cache
    const MeshList& GetMeshes() const;
    /// return  maxim index for subCell
    int  GetMaxSubCellIndexAbsolute() const;
    /// return  maxim index for subCell in cell
    int  GetMaxSubCellIndexRelative() const;
    /// return the size of subblock
    float GetSubBlockSideSizeScaled() const;
    /// Set The subdivision by cell
    void SetVegetationCellSubDivision (int);
    /// Get The subdivision by cell
    int GetVegetationCellSubDivision  () const;
    /// return array of nmesh2 this a mesh library
    const nFixedArray<nRef<nMesh2> >& GetMeshLibrary();
    /// set the size of mesh list
    void SetGrowthMeshLibrarySize(int);
    /// Get the size of mesh list 
    int GetGrowthMeshLibrarySize () const;
    /// set the mesh
    void SetGrowthMeshName(int, const char*);
    /// Get the mesh
    const char* GetGrowthMeshName(int) const;
    /// set the size of mesh list
    void SetNumSubBlockClip(int);
    /// Get the size of mesh list 
    int GetNumSubBlockClip () const;
    /// set the size of mesh list
    void SetGrasBeginFadeFactor(float);
    /// Get the size of mesh list 
    float GetGrasBeginFadeFactor () const;
    /// set the size of mesh list
    void SetGeometryFactorClipDistance(const vector2&);
    /// Get the size of mesh list 
    void GetGeometryFactorClipDistance (vector2&) const;

    /// return the name cell // "cell#%.2d#%.2d#seeds.ngs
    const nString GetCellName(int bx, int bz) const;
    /// return the resource filename for grwoth seed
    const nString GetGrowthSeedFileName(int bx, int bz) const;
    /// return the grothSeed
    nGrowthSeeds *GetGrowthSeeds(int bx, int bz); // not const because it use cache
    /// return the growth seed and the index
    nGrowthSeeds *GetGrowthIndex(const char* cmdString, int&  group); // not const because it use cache
    /// return the subGroup Id for a growthmesh
    int GetSubGroupIdx(int relx, int relz) const;
    /// save resources
    void SaveResources();
    /// return in  nebula units
    vector2 GetGeometryClipDistance() const;

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
        /// Nebula class persistence only for conjurer
        bool SaveConjurerCmds(nPersistServer * ps);
        /// Nebula class persistence only for conjurer
        bool SaveConjurer();
        /// return the path of growth map
        const nString GetGrowthMapFileName(int bx, int bz) const;
        /// return the texture growth map
        nTexture2* GetGrowthTexture(int bx, int bz); // not const because it use cache
        /// return the path of growth map
        static const nString GetGrowthMapFileName(nEntityObject* cell);
        /// return array of colors
        void GetTerrainGrassPalette(nArray<vector4>& colors);
        /// Set the mesh list
        void SetGrowthMeshLibrary( const nArray<nString>& meshNameList);
        /// Create the nvegatation node resource
        void CreateSceneResource();

        /// Create the material list called it when create the class first time
        nObject* CreateGrowthMaterialList();
        //void GreateGrowthMaterialList();
        /// Get the material list
        nRoot* GetGrowthMaterialList();
        /// return the id of the growth material with the given name if it exists, otherwise 0
        int GetIdOfGrowthMaterialWithName(const char* name);
        /// return true if there is a growth material with the given name, otherwise false
        bool HasGrowthMaterialWithName(const char* name);
        /// return the growth material
        nGrowthMaterial* GetGrowthMaterialById(int);
        /// create growth material and append it
        nGrowthMaterial* CreateGrowthMaterial(const char*) ;
        /// create a growth material from the given material and append it
        nGrowthMaterial* AddCopyOfGrowthMaterial(nGrowthMaterial * originalMaterial, const char* name);

        ///  Delete material
        void DeleteGrowthMaterial(int) ;
        /// return the number of materials
        int GetNumberOfGrowthMaterials();
        /// save terrain growth material 
        bool SaveTerrainVegetationResources();
        /// set the size of growthmap by cell
        void SetGrowthMapSizeByCell(int);
        /// Get the size of growthmap by cell
        int GetGrowthMapSizeByCell () const;
        /// Load necesary resource for edit
        bool BeginGrowthEditon ();
        /// return is groth edition state
        bool IsInGrowthEdition () const;
        /// Build seeds for a grass
        bool BuildGrowth ();
        /// Build seeds for a grass in only dirty growtmap
        bool BuildOnlyDirtyGrowth ();
        /// Buils seed for a selected cells
        bool BuildGrassOnlySelected ();
        /// return is groth edition state
        bool GrassUpdateMeshes ();
        /// return is groth edition state
        bool GrassUpdateHeight ();
        /// Unload resource for edition and create it
        bool EndGrowtEdition(bool);
        /// set alpha for render growth edition
        void SetGrowthEditionAlpha(float);
        /// get alpha for render growth edition
        float GetGrowthEditionAplha () const;

    #endif //#ifndef NGAME
    #endif //#ifndef __ZOMBIE_EXPORTER__

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
       // disable draw grass
        static bool debugDisableDrawGrass;
        static bool debugDisableDrawMesh;
        static bool debugInfo;
    #endif
    #endif

private:
    ///
    int visibleSubCell;
    /// This factor indicate the start of fade
    float beginFadeFactor;
    /// Number of subidivisin for blocks 
    int vegetationCellSubDivision;
    /// List of brush mehses
    nFixedArray<nRef<nMesh2> > meshLibrary;
    /// The name of brush meshes
    nFixedArray<nString> meshNameLibrary;
    /// cache of growth seeds, ( one by block )
    nCache<nGrowthSeedCacheEntry> cacheGrowthSeed;
    /// cache for vegetation Mesh
    nCache<nVegetationMeshCacheEntry> cacheVegetationMesh;

    /// The scene node for draw a grass
    nRef<nVegetationNode> vegetationNode;
    /// temporal cache of mesh draw
    MeshList meshDrawList;
    /// The near a Far distance
    vector2 geometryFactorClipDistance;

    /// Return the vegetation mesh cache size
    int GetVegetationMeshCacheSize() const;
    /// Return the growth cache size
    int GetGrowthSeedCacheSize() const;
    /// return the fileName of ncenenode
    const nString GetSceneResourcePath() const;
    /// return the libscene root
    static nRoot* GetLibSceneRoot();
    /// return the subCell Id for this worl position
    SubCellId GetSubCellIndex( float bx , float bz) const;
    /// return if valid subCellIndex
    bool IsValidSubCellIndex(const SubCellId& id) const;
    /// return the mesh for subCell id
    nMesh2* GetMeshForSubCell(const SubCellId& id);
    /// Load all mesh in meshLibraryResources
    bool LoadMeshLibraryResources();
    /// Load all mesh in meshLibraryResources
    void UnloadMeshLibraryResources();
    /// breshenham algorithm for determiante the subcell for render
    void VisibilityTriangle( vector2 v1 , vector2 v2 , vector2 v3 );

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
        /// Cache of texture for render terrain
        nCache<nGrowthTextureCacheEntry>  cacheGrowthTexture;
        nRef<nRoot> refGrowthMaterialList;
        nRoot* LoadGrowthMaterialList();
        // return the parent node opf material list
        nRoot* GetRootNode();
        // size of growthmap by cell
        int growthMapSizeByCell;
        /// is in edit terrain vegetation
        bool isInGrowthEdition;
        /// the growth builder use in edit of growth 
        nVegetationBuilder* builder;
        /// alpha value for a render growth map
        float growthEditionAplha;
    #endif //#ifndef NGAME
    #endif //#ifndef __ZOMBIE_EXPORTER__

};

//------------------------------------------------------------------------------
#endif//N_NCTERRAINVEGETATIONCLASS_H