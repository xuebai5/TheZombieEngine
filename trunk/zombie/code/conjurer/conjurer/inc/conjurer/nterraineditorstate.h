#ifndef N_TERRAINEDITORSTATE_H
#define N_TERRAINEDITORSTATE_H
//------------------------------------------------------------------------------
/**
    @class nTerrainSceneState
    @ingroup NebulaConjurerEditor

    Editor state for conjurer terrain module.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "conjurer/neditorstate.h"
#include "kernel/ncmdprotonativecpp.h"
#include "conjurer/terraingeometryundocmd.h"
#include "ngeomipmap/nterrainmaterial.h"
#include "ngeomipmap/nfloatmapbuilder.h"
#include "ngeomipmap/nterrainline.h"
#include "tools/nlinedrawer.h"

//------------------------------------------------------------------------------
class nConjurerApp;
class nGuiWindow;
class nTexture2;
class nInguiTerrainTool;
class ncTerrainMaterialClass;
class nTerrainLightMapBuilder;

//------------------------------------------------------------------------------
// Path of terrain editor objects
extern const char* terrainEditorPath;
/// Path of paintbrushes nebula objects (nFloatMap)
extern const char* paintbrushesPath;
/// Physical path of paintbrushes texture directory
extern const char* paintbrushesDir;

// Type of functions for generating heightmaps
typedef float (*HeightmapFunc2D)(float, float);

//------------------------------------------------------------------------------
class nTerrainEditorState : public nEditorState
{
public:
    /// constructor
    nTerrainEditorState();
    /// destructor
    virtual ~nTerrainEditorState();
    /// called when state is becoming active
    virtual void OnStateEnter(const nString &prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// called on state to perform 3d rendering
    virtual void OnRender2D();

    // Type of terrain tool
    enum TerrainToolType
    {
        RaiseLow = 0,
        Flatten,
        Slope,
        Smooth,
        Noise,
        Paint,
        Grass,
        Hole,

        NumTerrainTools
    };

    /// Size of the icons of paintbrushes in the terrain application module
    static const int PaintbrushIconSize = 64;

    /// Initial Paintbrush size
    static const int InitialPaintbrushSize = 10;

    /// Constant factor for terrain tool effect
    static const float TerrainToolIntensity;

    /// Create a new terrain class
    bool CreateTerrainClass(const char * className, int heightMapSize, int blockSize, int vegetationCellsPerBlock, int vegetationCellSize, int weightMapSize, int globalTextureSize, float gridScale);

    /// Set terrain class parameters
    bool SetTerrainClassParams(nString, float, float, bool, float);

    /// Create a new terrain instance
    bool CreateTerrainInstance(nString);

    /// Set outdoor entity heightmap to be edited
    void SetOutdoor(nEntityObject *);

    /// Import the heightmap from a texture file
    bool ImportHeightMap(const char *);

    /// Export the heightmap to a texture file
    bool ExportHeightMap(const char *);

    /// Get number of paintbrushes
    int PaintbrushCount () const;

    /// Get file name of a paintbrush thumbnail
    nString GetPaintbrushThumbnail(int);

    /// Select a paintbrush
    void SelectPaintbrush(int);

    /// Get selected paintbrush
    int GetSelectedPaintbrush ();

    /// Tell if a paintbrush is affected by SetPaintBrushSize()
    bool IsSizeablePaintbrush(int);

    /// Sets the current size of the paintbrush
    void SetPaintbrushSize(int);

    /// Gets the current size of the paintbrush
    int GetPaintbrushSize ();

    /// Increases the size of the current paintbrush
    void MakePaintbrushBigger ();

    /// Decreases the size of the current paintbrush
    void MakePaintbrushSmaller ();

    /// Import paintbrushes from texture files in a directory
    int LoadPaintbrushes ();

    /// Select tool to use
    void SelectTool(int);

    /// Unselect the current tool
    void DeselectCurrentTool();

    /// Get the currently selected tool
    int GetSelectedTool ();

    /// Get the height and slope of terrain at a point
    bool GetHeightSlope(vector3, float&, float&);

    /// Generate the lightmaps
    void CreateTerrainLightMaps(const char *, int, int, int, nEntityObjectId, bool, const float, const vector2, const float);
    /// Generate the global lightmap for 
    bool CreateTerrainGlobalLightMap(const char*, int);

    /// Generate the global texture
    void CreateTerrainGlobalTexture();

    /// Generate the terrain weightmaps
    void CreateTerrainWeightmaps();

    /// Preprocess and save horizon information
    void PreprocessHorizon();

    /// Get the size of a user paintbrush
    int GetPaintbrushSizeByIndex(int) const;

    // Begin of non-scripted public methods

    /// Handle input in the current viewport
    bool HandleInput( void );
    
    /// Tell if a heightmap has been set
    bool IsHeightMapSet( void );

    /// Get the heightmap reference, and find it in the NOH if it hasn't been set
    nFloatMap* GetHeightMap();
    
    /// Gets a nTexture2 of a paintbrush
    nTexture2* GetPaintBrushImage( int paintbrush );

    /// looks up for an outdoor entity object and returns it
    nEntityObject * GetOutdoorEntityObject() const;

    // Draw debug visualization
    void DrawDebug( nAppViewport* vp );
    bool drawDebug;

    nTerrainLine* debugLine;   
    // Line drawer for testing
    nLineDrawer lineDrawer;

    // Signals
    NSIGNAL_DECLARE('ZBSC', void, PaintbrushSizeChanged, 1, (int), 0, ());

protected:
    /// Handle input in the current viewport
    virtual bool HandleInput(nTime frameTime);

    // INGUI tool objects
    nInguiTerrainTool* inguiTerrainTool[ NumTerrainTools ];

    friend class TerrainGeometryUndoCmd;

private:

    void InsertLayerUndoInfo( int i );

    // Type of predefined terrain paintbrushes
    enum TerrainPaintbrushType
    {
        paintbrushSquare = 0,
        paintbrushCircle,
        paintbrushHat,
        paintbrushCone,
        numPredefinedPaintbrushes
    };

    // Reference to the heightmap being edited
    nRef<nFloatMap> heightMap;

    // Heightmap buffer. It's a copy of the main one, for the undo/redo system
    nRef<nFloatMap> heightMapBuffer;

    // Array of floatmap buffers for painting undo/redo operations.
    struct PaintUndoBufferInfo {
        PaintUndoBufferInfo() 
        {
            layerHandle = -1;
            layerBuffer = 0;
        }

        ~PaintUndoBufferInfo() 
        {
        }

        nTerrainMaterial::LayerHandle layerHandle;
        nFloatMap* layerBuffer;
    };
    nArray<PaintUndoBufferInfo> paintUndoBufferInfo;

    // Floatmap buffer for paint filtering
    nRef<nFloatMap> layerPaintFilter;

    // Terrain layer manager
    nRef<nEntityObject> terrainObject;
    ncTerrainMaterialClass * layerManager;

    // Previous viewport
    nAppViewport* previousViewport;
    
    // List of user paintbrushes
    nArray<nFloatMap*> userPaintbrushList;

    // Heightmap paintbrush. It's the heightmap used for all predefined brushes
    nRef<nFloatMap> predefinedPaintbrush;

    // Temporary paintbrush for all operations
    nRef<nFloatMap> tempPaintbrush;

    // Second temporary paintbrush
    nRef<nFloatMap> tempPaintbrush2;

    // List of predefined paintbrushes textures (icons)
    nArray<nTexture2*> predefinedPaintbrushIconList;
    // List of user paintbrushes textures (icons)
    nArray<nTexture2*> userPaintbrushIconList;
    // List of paintbrushes names
    nArray<nString> thumbnailNames;

    // State variables
    int selectedPaintbrush;
    int paintbrushSize;
    int selectedTool;

    TerrainGeometryUndoCmd::IntRectangle undoRectangle;

    // Current INGUI tool
    nInguiTerrainTool* currentInguiTool;

    // First point of user action over the terrain
    vector3 firstPoint;
    // Last stored 3d mouse position
    vector3 lastPoint;
    int currentXMousePos, currentZMousePos;

    // Apply current tool
    bool ApplyTool( nTime frameTime, nAppViewport *vp, vector2 mp, bool firstClick, bool dummy );

    // Generate a predefined paintbrush in 'predefinedPaintbrush'
    void GeneratePredefinedPaintbrush( void );
    
    // Fill a floatmap with a plane
    void FillWithPlane(nFloatMap* hmap, vector3 normal, float h0);

    // Fill a heightmap with a 2D function
    void FillWithFunc2D( nFloatMap* hmap, HeightmapFunc2D func );

    // FLoatMap builder
    nFloatMapBuilder floatMapBuilder;

    // Tool class objects for quick reference
    nClass* terrainToolGeomClass;
    nClass* terrainToolClass;

    nTerrainLightMapBuilder* lightMapBuilder;
};

//------------------------------------------------------------------------------
// 2D function prototypes for filling heightmaps
float cubic(float x, float y);
float circle(float x, float y);
float distance(float x, float y);
float cosinus(float x, float y);
float noise(float x, float y);

//------------------------------------------------------------------------------

#endif
