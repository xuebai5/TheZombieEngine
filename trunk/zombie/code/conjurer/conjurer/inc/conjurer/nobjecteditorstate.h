#ifndef N_OBJECTEDITORSTATE_H
#define N_OBJECTEDITORSTATE_H
//------------------------------------------------------------------------------
/**
    @class nObjectSceneState
    @ingroup NebulaConjurerEditor

    Editor state for object manipulation.

    (C) 2004 Conjurer Services, S.A.
*/
#include "conjurer/neditorstate.h"
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gfx2/ntexture2.h"
#include "gfx2/ngfxserver2.h"
#include "util/nstringlist.h"
#include "conjurer/inguiobjecttransform.h"
#include "conjurer/inguiobjecttransformgroup.h"
#include "conjurer/neditorgrid.h"
#include "nlayermanager/nlayermanager.h"
#include "entity/nrefentityobject.h"
#include "conjurer/ninguitooltransform.h"

class nConjurerApp;
class nAppViewport;
class nGuiWindow;
class nInguiTool;
class nInguiToolTransform;
class nInguiToolSphereArea;
class nInguiToolPrismArea;
class nInguiToolSoundSourceSphereArea;
class nInguiToolSoundSourcePrismArea;
class nInguiToolTransform;
class nInguiToolTranslation;
class nInguiToolRotation;
class nInguiToolScale;
class nInguiToolSelection;
class nInguiToolPlacer;
class nInguiTerrainToolMultPlacer;
class nInguiToolWaypoint;
class nInguiToolGenWaypoint;
class nInguiToolMeasure;
class ncSpatialQuadtreeCell;
class ncSpatialBatch;
class ncSpatialSpace;

//------------------------------------------------------------------------------
class nObjectEditorState : public nEditorState
{
public:

    /// Object tools
    enum ObjectTools {
        InvalidTool = -1,
        ToolTranslation,
        ToolRotation,
        ToolScale,
        ToolSelection,
        ToolPlace,
        ToolMultPlace,
        ToolWaypoint,
        ToolCircleTrigger,
        ToolPolygonTrigger,
        ToolSoundSourceCircleTrigger,
        ToolSoundSourcePolygonTrigger,
        ToolMeasure,
        ToolGenWaypoint,
        NumObjectTools
    };

    enum ClipBoardStatus {
        ClipNone,
        ClipCut,
        ClipCopied
    };

    enum SelectionMode {
        ModeNormal,
        ModeSubentity,
        ModeTerrainCell,
        LastSelectionMode
    };

    /// constructor
    nObjectEditorState();
    /// destructor
    virtual ~nObjectEditorState();
    /// called when state is becoming active
    virtual void OnStateEnter(const nString &prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// called on state to perform 3d rendering
    virtual void OnRender2D();

    /// Get tool count
    int GetToolCount ();
    /// Get index of select tool
    int GetSelectedToolIndex ();
    /// Select tool by index
    void SelectTool(int);
    /// Select tool by index passsing one string parameter
    void SelectToolWithParameter(int, const char*);
    // Select tool by index and signal the change
    void SelectToolAndSignalChange(int);
    /// Set world or local transform mode
    void SetWorldMode(bool);
    /// Set terrain collision type when editing objects
    void SetCanMoveThroughTerrain(int);
    /// Get number of selected entities
    int GetSelectionCount ();
    /// Reset selection (signalling change)
    void ResetSelection ();
    /// Reset selection without signalling the change
    void ResetSelectionSilently ();
    /// Reset selection - boolean indicates whether to signal change
    void ResetSelection (bool signalChange);
    /// Return true if the given object can be added to the selection
    bool CanAddEntityToSelection(nEntityObjectId);
    /// Add an entity to the selection silently
    void AddEntityToSelectionSilently(nEntityObjectId);
    /// Add an entity to the selection (signalling change)
    void AddEntityToSelection(nEntityObjectId);
    /// Add an entity to the selection
    void AddEntityToSelection(nEntityObjectId, bool signalChange);
    /// Remove an entity from the selection silently
    void RemoveEntityFromSelectionSilently(nEntityObjectId);
    /// Remove an entity from the selection (signalling change)
    void RemoveEntityFromSelection(nEntityObjectId);
    /// Remove an entity from the selection
    void RemoveEntityFromSelection(nEntityObjectId, bool signalChange);
    /// Get selected entity by entity id
    nEntityObject* GetSelectedEntity(int);
    /// Get center point of the selection
    vector3 GetSelectionCenterPoint(float&);
    /// Get selection mode index
    int GetSelectionMode();
    /// Set selection mode by index
    bool SetSelectionMode(int, bool);
    /// Get entity associated to current selection mode
    nEntityObject* GetSelectionModeEntity ();
    /// Save selection to file
    bool SaveSelectionToFile(const char *);
    /// Load selection from file
    bool LoadSelectionFromFile(const char *);
    /// Put selection into clipboard, removing the entities from the level
    void CutEntities();
    /// Put selection into clipboard
    void CopyEntities();
    /// Instance new entities or place cut entities from the clipboard
    void PasteEntities();
    /// Delete entities without touching the clipboard
    void DelEntities();
    /// Snap all selected entities to terrain
    bool SnapSelectionToTerrain();
    /// Get the last entity placed (only valid just after the signal SingleEntityPlaced)
    nEntityObject* GetEntityPlaced() const;
    /// Get name for first selected entity that is ancestor of the given parent
    nString GetNameForFirstSelectedClassThatIsA(const char *parentClassName) const;

    /// set rebuild batch resource
    void SetBatchRebuildResource(bool value);
    /// get rebuild batch resource
    bool GetBatchRebuildResource();
    /// set test batch subentities
    void SetBatchTestSubentities(bool value);
    /// get test batch subentities
    bool GetBatchTestSubentities();
    /// set default batch distance
    void SetBatchMinDistance(float value);
    /// get default batch distance
    float GetBatchMinDistance();

    /// Automatically batch entities per terrain block
    void CreateTerrainBatches();
    /// batch selected entities
    void BatchEntities();
    /// remove selected entities from their batches
    void RemoveEntitiesFromBatch();
    /// reload selected entities
    void ReloadEntities();
    /// Load entities
    void LoadEntities();
    /// Unload entities
    void UnloadEntities();
    /// Add selection to load area
    void AddToLoadArea(const char *);
    /// Remove selection from load area
    void RemoveFromLoadArea(const char *);
    /// Set showInfoOnSelection to the value passed
    void SetShowInfoOnSelection(bool shouldShowInfo);

    /// @name Signals interface
    //@{
    /// 
    NSIGNAL_DECLARE('JTSL', void, ToolSelected, 0, (), 0, ());
    NSIGNAL_DECLARE('JLMC', void, LockedModeChanged, 0, (), 0, ());
    NSIGNAL_DECLARE('JSCH', void, SelectionChanged, 0, (), 0, ());
    NSIGNAL_DECLARE('JENM', void, EntityModified, 0, (), 0, ());
    NSIGNAL_DECLARE('ESEP', void, SingleEntityPlaced, 0, (), 0, ());
    //@}

    // Begin of non-scripted public methods

    /// Get selected tool pointer
    nInguiTool* GetSelectedTool( void );

    /// Get reference to the entity clipboard array
    nArray<nEntityObjectId>& GetClipboard( void );

    /// Get reference to the entity selection array
    nArray<nRefEntityObject>& GetSelection();

    /// Check selection and remove invalid references
    void CheckSelection();

    /// Get selected classes
    nStringList* GetSelectedClasses() const;

    // If selected tool is a object transform, generate and assign to it the InguiObjectTransform objects
    void AssignToolObjectTransform();

    // Check if an entity is already in the objectTransform objects array
    bool CheckEntityIsInObjectTransform( nEntityObject* obj );

    // update current class list in instancing tools
    void UpdateClassList();

    // Draw debug visualization
    void DrawDebug( nAppViewport* vp );

    // Draw grid
    void DrawGrid( nAppViewport* vp );

    // Switch to selection tool, remembering the last tool used. Will return to the previous tool after using the selection
    void SwitchToSelection( nAppViewport* vp, vector2 mp, line3 ray );

    // Set previous tool used before selection
    void SetPreviousTool();

    // Handle shortcut keypress from any application state
    bool HandleGlobalInput();

    // Signal that an entity has been placed
    void SetEntityPlaced( nEntityObject* entity );

    // Receive level loaded notification
    void OnLevelLoaded();

    // Clear the list of exceptions
    void ResetSelectionExceptions();

    // Add the given class to the list of exceptions
    void AddSelectionException(const char* className);

protected:

    /// Handle input in the current viewport
    virtual bool HandleInput(nTime frameTime);

    /// Select subcells of a terrain cell
    void SelectSubTerrainCells( ncSpatialQuadtreeCell** subCells );

    /// Load entities contained in a terrain cell
    void LoadEntitiesInCell(ncSpatialQuadtreeCell* cell);

    /// Load entities contained in a terrain cell
    void UnloadEntitiesInCell(ncSpatialQuadtreeCell* cell);

    /// Load entities contained in a spatial batch
    void LoadEntitiesInBatch(ncSpatialBatch* batch);

    /// Load entities contained in an indoor space
    void LoadEntitiesInSpace(ncSpatialSpace* space);

    /// Load entities contained in an indoor space
    void UnloadEntitiesInSpace(ncSpatialSpace* space);

private:

    // Transform tools
    nRef<nInguiToolTranslation> refTranslationTool;
    nRef<nInguiToolRotation> refRotationTool;
    nRef<nInguiToolScale> refScaleTool;

    // Selection tool
    nRef<nInguiToolSelection> refSelectionTool;

    // Instancing (placer) tools
    nRef<nInguiToolPlacer> placeTool;
    nRef<nInguiTerrainToolMultPlacer> multPlaceTool;
    nRef<nInguiToolWaypoint> waypointTool;
    nRef<nInguiToolGenWaypoint> genWaypointTool;
    nRef<nInguiToolSphereArea> circleTriggerTool;
    nRef<nInguiToolPrismArea> polygonTriggerTool;
    nRef<nInguiToolSoundSourceSphereArea> circleSoundSourceTriggerTool;
    nRef<nInguiToolSoundSourcePrismArea> polygonSoundSourceTriggerTool;
    nRef<nInguiToolMeasure> measureTool;

    // Layer manager
    nRef<nLayerManager> refLayerManager;

    // Currently selected tool
    int selectedToolIndex;
    nRef<nInguiTool> refSelectedTool;

    // Current selection mode
    int currentSelectionMode;

    // Entity associated to the current selection mode
    nRefEntityObject refCurrentSelectionModeEntity;

    // Array of selected objects
    nArray<nRefEntityObject> selection;

    // Array of transform objects
    nArray<InguiObjectTransform> selectionObjTransf;

    // List of class names for instancing tools
    nRef<nStringList> selectedClasses;

    // Group object transform
    InguiObjectTransformGroup groupTransform;

    // Entity clipboard
    nArray<nEntityObjectId> clipboard;

    // Clipboard status
    ClipBoardStatus clipBoardStatus;

    // Global flag for world axis aligned operation
    bool inWorldMode;

    // Global flag for through-terrain entity movement
    InguiObjectTransform::TerrainCollisionType terrainCollisionType;

    // Tool that was selected previous to the selection tool, for quick tool switch (-1 = not valid )
    int previousTool;

    // Editor grid
    nEditorGrid* grid;

    // Temp array for assigning indoor entities to transform tool
    nArray<nEntityObject*> indoorEntities;

    // classes for quick test
    nClass* transformToolClass;
    nClass* indoorClass;
    nClass* editorStateClass;
    nClass* terrainCellClass;

    // array of not selectable classes
    nArray<nClass*> selectionExceptions;

    // Last entity placed (only valid just after the signal SingleEntityPlaced)
    nEntityObject* entityPlaced;

    // cursor object transform flag
    bool cursorTransformFlag;

    // cursor object transform amount
    float cursorTransformAmount;

    // last cursor object transform axis
    nInguiToolTransform::PickAxisType lastCursorTransformAxis;

    // properties for batch generation
    bool batchRebuildResource;
    bool batchTestSubentities;
    float batchMinDistance;

    // flag indicates whether to draw labels for selected items
    bool showInfoOnSelection;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nObjectEditorState::SetBatchRebuildResource(bool value)
{
    this->batchRebuildResource = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nObjectEditorState::GetBatchRebuildResource()
{
    return this->batchRebuildResource;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nObjectEditorState::SetBatchTestSubentities(bool value)
{
    this->batchTestSubentities = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nObjectEditorState::GetBatchTestSubentities()
{
    return this->batchTestSubentities;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nObjectEditorState::SetBatchMinDistance(float value)
{
    this->batchMinDistance = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nObjectEditorState::GetBatchMinDistance()
{
    return this->batchMinDistance;
}

#endif
