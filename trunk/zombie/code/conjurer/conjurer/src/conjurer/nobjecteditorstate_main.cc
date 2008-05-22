#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nobjecteditorstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "conjurer/nobjecteditorstate.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/ninguitooltranslation.h"
#include "conjurer/ninguitoolrotation.h"
#include "conjurer/ninguitoolscale.h"
#include "conjurer/ninguitoolplacer.h"
#include "conjurer/ninguitoolwaypoint.h"
#include "conjurer/ninguitoolgenwaypoint.h"
#include "conjurer/ninguiterraintoolmultplacer.h"
#include "conjurer/neditorgrid.h"
#include "conjurer/ninguitoolspherearea.h"
#include "conjurer/ninguitoolprismarea.h"
#include "conjurer/ninguitoolsoundsourcespherearea.h"
#include "conjurer/ninguitoolsoundsourceprismarea.h"
#include "conjurer/ninguitoolmeasure.h"
#include "conjurer/nentitybatchbuilder.h"
#include "conjurer/nterrainbatchbuilder.h"
#include "conjurer/nterraineditorstate.h"

#include "napplication/nappviewport.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "ngeomipmap/nfloatmap.h"
#include "conjurer/objectplacingundocmd.h"
#include "conjurer/objecttransformundocmd.h"
#include "mathlib/transform44.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialbatch.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialindoor.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/ncsubentity.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncloader.h"
#include "ndebug/nceditor.h"

#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialbatch.h"

#include "nlayermanager/nlayermanager.h"
#include "nworldinterface/nworldinterface.h"
#include "entity/nobjectinstancer.h"

#include "nscene/ncscenelodclass.h"
#include "zombieentity/nloaderserver.h"
#include "zombieentity/nloadarea.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nObjectEditorState, "neditorstate");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nObjectEditorState, ToolSelected );
NSIGNAL_DEFINE( nObjectEditorState, LockedModeChanged );
NSIGNAL_DEFINE( nObjectEditorState, SelectionChanged );
NSIGNAL_DEFINE( nObjectEditorState, EntityModified );
NSIGNAL_DEFINE( nObjectEditorState, SingleEntityPlaced );

//------------------------------------------------------------------------------
const char* objectEditorPath = "/editor/object/";

//------------------------------------------------------------------------------
/**
*/
nObjectEditorState::nObjectEditorState():
    inWorldMode( false ),
    clipBoardStatus( ClipNone ),
    terrainCollisionType( InguiObjectTransform::colNever ),
    currentSelectionMode( ModeNormal ),
    entityPlaced( NULL ),
    cursorTransformFlag( false ),
    cursorTransformAmount( 0.0f ),
    lastCursorTransformAxis( nInguiToolTransform::axisNone ),
    batchRebuildResource(false),
    batchTestSubentities(true),
    batchMinDistance(100.0f)
{
    // Initialize tool class reference objects
    this->transformToolClass = nKernelServer::Instance()->FindClass("ninguitooltransform");
    this->indoorClass = nKernelServer::Instance()->FindClass("neindoor");
    this->editorStateClass = nKernelServer::Instance()->FindClass("neditorstate");
    this->terrainCellClass = nKernelServer::Instance()->FindClass("neoutdoorcell");

    this->ResetSelectionExceptions();

    this->selectedToolIndex = InvalidTool;
    this->previousTool = InvalidTool;

    this->refLayerManager.set( static_cast<nLayerManager*>( nKernelServer::Instance()->Lookup("/sys/servers/layermanager") ) );
}
//------------------------------------------------------------------------------
/**
*/
nObjectEditorState::~nObjectEditorState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::OnCreate(nApplication* application)
{
    nString name;

    nEditorState::OnCreate(application);

    // Create test grid
    name.Set( "/editor/grid" );
    grid = static_cast<nEditorGrid*> (nKernelServer::Instance()->New("neditorgrid", name.Get() ));
    grid->SetSnapEnabled( false );

    // Create tool objects
    name.Set( objectEditorPath );
    name.Append( "/translationTool");
    this->refTranslationTool = static_cast<nInguiToolTranslation*> (nKernelServer::Instance()->New("ninguitooltranslation", name.Get() ));
    this->refTranslationTool->SetGridPath("/editor/grid");
    
    name.Set( objectEditorPath );
    name.Append( "/rotationTool");
    this->refRotationTool = static_cast<nInguiToolRotation*> (nKernelServer::Instance()->New("ninguitoolrotation", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/scaleTool");
    this->refScaleTool = static_cast<nInguiToolScale*> (nKernelServer::Instance()->New("ninguitoolscale", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/selectionTool");
    this->refSelectionTool = static_cast<nInguiToolSelection*> (nKernelServer::Instance()->New("ninguitoolselection", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/placeTool");
    this->placeTool = static_cast<nInguiToolPlacer*> (nKernelServer::Instance()->New("ninguitoolplacer", name.Get() ));
    this->placeTool->SetGridPath("/editor/grid");
    // @todo MBS change this to get current level terrain or maybe change to terrain class
    //nKernelServer::Instance()->Lookup( "/usr/terrain/geomipmap/heightmap" );

    name.Set( objectEditorPath );
    name.Append( "/multiplePlacerTool");
    this->multPlaceTool = static_cast<nInguiTerrainToolMultPlacer*> (nKernelServer::Instance()->New("ninguiterraintoolmultplacer", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/waypointTool");
    this->waypointTool = static_cast<nInguiToolWaypoint*> (nKernelServer::Instance()->New("ninguitoolwaypoint", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/genWaypointTool");
    this->genWaypointTool = static_cast<nInguiToolGenWaypoint*>(nKernelServer::Instance()->New("ninguitoolgenwaypoint", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/circleTriggerTool");
    this->circleTriggerTool = static_cast<nInguiToolSphereArea*> (nKernelServer::Instance()->New("ninguitoolspherearea", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/polygonTriggerTool");
    this->polygonTriggerTool = static_cast<nInguiToolPrismArea*> (nKernelServer::Instance()->New("ninguitoolprismarea", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/circleSoundSourceTriggerTool");
    this->circleSoundSourceTriggerTool = static_cast<nInguiToolSoundSourceSphereArea*> (nKernelServer::Instance()->New("ninguitoolsoundsourcespherearea", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/polygonSoundSourceTriggerTool");
    this->polygonSoundSourceTriggerTool = static_cast<nInguiToolSoundSourcePrismArea*> (nKernelServer::Instance()->New("ninguitoolsoundsourceprismarea", name.Get() ));

    name.Set( objectEditorPath );
    name.Append( "/measureTool");
    this->measureTool = static_cast<nInguiToolMeasure*> (nKernelServer::Instance()->New("ninguitoolmeasure", name.Get() ));

    // Create selected class list
    name.Set( objectEditorPath );
    name.Append( "/classList");
    this->selectedClasses = static_cast<nStringList*> (nKernelServer::Instance()->New("nstringlist", name.Get() ));
    this->placeTool->SetClassList( this->selectedClasses.get() );
    this->multPlaceTool->SetClassList( this->selectedClasses.get() );

    // Set reference to selection tool in transform tools
    this->refTranslationTool->SetSelectionTool( this->refSelectionTool );
    this->refRotationTool->SetSelectionTool( this->refSelectionTool );
    this->refScaleTool->SetSelectionTool( this->refSelectionTool );

    this->SelectTool( ToolTranslation );

    // Bind signal of level loaded
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, this, &nObjectEditorState::OnLevelLoaded, 0);
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::OnLevelLoaded()
{
    this->OnStateEnter("");
}
//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::OnStateEnter(const nString &prevState)
{
    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    // get outdoor object
    nEntityObjectId oid = level->FindEntity("outdoor");
    if (oid)
    {
        // get entity object
        nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject(oid);
        if (object)
        {
            this->placeTool->SetOutdoor( object );
            this->multPlaceTool->SetOutdoor( object );
        }
    }

    nEditorState::OnStateEnter(prevState);
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::OnStateLeave(const nString& /*nextState*/)
{
    this->refSelectedTool->SetState( nInguiTool::NotInited );
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::OnRender2D()
{

    // Call parent's OnRender3D
    nEditorState::OnRender2D();
}

//------------------------------------------------------------------------------
/**
    Draw debug visualization
*/
void
nObjectEditorState::DrawDebug( nAppViewport* vp )
{
    n_assert( vp );
    
    // Get viewport and camera info
    nGfxServer2* refGfxServer = nGfxServer2::Instance();

    nCamera2 camera = vp->GetCamera();

    // Draw the current tool
    this->refSelectedTool->Draw( vp, &camera );

    refGfxServer->BeginShapes();

    // Draw selection
    this->refSelectionTool->DrawSelectionBB();

    // Draw labels for the selected items if appropriate
    if ( this->showInfoOnSelection )
    {
        this->refSelectionTool->DrawSelectionLabels();
    }

    // Draw superentity bounding box if subentity mode selected
    if ( this->GetSelectionMode() == nObjectEditorState::ModeSubentity && this->GetSelectionModeEntity() )
    {
        this->refSelectionTool->DrawEntityBB( this->GetSelectionModeEntity(), vector4(0.0f, 0.0f, 1.0f, 0.2f) );
    }
    refGfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    @brief Draw the grid
*/
void
nObjectEditorState::DrawGrid( nAppViewport* vp )
{
    this->grid->Draw( vp );
}

//------------------------------------------------------------------------------
/**
    @brief Select a tool by index
*/
void
nObjectEditorState::SelectTool( int tool )
{
    this->SelectToolWithParameter( tool, "" );
}

//------------------------------------------------------------------------------
/**
    @brief Select a tool by index, passing a single parameter
*/
void
nObjectEditorState::SelectToolWithParameter( int tool, const char* parameterOne )
{

    if ( tool < 0 )
    {
        tool = 0;
    }
    if ( tool >= NumObjectTools )
    {
        tool = NumObjectTools - 1;
    }

    if ( this->refSelectedTool.isvalid() )
    {
        this->refSelectedTool->SetState( nInguiTool::NotInited );
    }

    this->selectedToolIndex = tool;
    switch ( this->selectedToolIndex )
    {
        case ToolTranslation:
            this->refSelectedTool = this->refTranslationTool;
            break;
        case ToolRotation:
            this->refSelectedTool = this->refRotationTool;
            break;
        case ToolScale:
            this->refSelectedTool = this->refScaleTool;
            break;
        case ToolSelection:
            this->refSelectedTool = this->refSelectionTool;
            break;
        case ToolPlace:
            this->refSelectedTool = this->placeTool;
            break;
        case ToolMultPlace:
            this->refSelectedTool = this->multPlaceTool;
            break;
        case ToolWaypoint:
            this->refSelectedTool = this->waypointTool;
            break;
        case ToolGenWaypoint:
            this->refSelectedTool = this->genWaypointTool;
            break;
        case ToolCircleTrigger:
            this->refSelectedTool = this->circleTriggerTool;
            break;
        case ToolPolygonTrigger:
            this->refSelectedTool = this->polygonTriggerTool;
            break;
       case ToolSoundSourceCircleTrigger:
            this->circleSoundSourceTriggerTool->SetTriggerClassName(parameterOne);
            this->refSelectedTool = this->circleSoundSourceTriggerTool;
            break;
       case ToolSoundSourcePolygonTrigger:
            this->polygonSoundSourceTriggerTool->SetTriggerClassName(parameterOne);
            this->refSelectedTool = this->polygonSoundSourceTriggerTool;
            break;
        case ToolMeasure:
            this->refSelectedTool = this->measureTool;
            break;
    }

    this->AssignToolObjectTransform();
    
    if ( this->refSelectedTool->IsA( this->transformToolClass ) )
    {
        static_cast<nInguiToolTransform*>(this->refSelectedTool.get())->SetWorldMode( this->inWorldMode );
    }

    this->refSelectedTool->OnSelected();

    UpdateClassList();
}
//------------------------------------------------------------------------------
/**
    @brief Select a tool by index and signal that the selection has changed
*/
void
nObjectEditorState::SelectToolAndSignalChange( int tool )
{
    this->SelectTool( tool );
    this->SignalToolSelected( this );
}
//------------------------------------------------------------------------------
/**
    @brief Check if an entity is already in the objectTransform objects array
*/
bool
nObjectEditorState::CheckEntityIsInObjectTransform( nEntityObject* obj )
{
    for ( int i=0; i < this->selectionObjTransf.Size(); i++ )
    {
        if ( this->selectionObjTransf[ i ].GetEntity() == obj )
        {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
/**
    @brief If selected tool is a object transform, generate and assign to it the InguiObjectTransform objects
*/
void
nObjectEditorState::AssignToolObjectTransform()
{

    if ( this->refSelectedTool->IsA( this->transformToolClass ) )
    {
        this->selectionObjTransf.Reset();

        // get current level from the level manager
        nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
        n_assert(level);

        // get outdoor object
        nEntityObjectId oid = level->FindEntity("outdoor");
        nEntityObject * outdoor = 0;
        if (oid)
        {
            outdoor = nEntityObjectServer::Instance()->GetEntityObject(oid);
        }

        // loop through selected entities
        int n = this->selection.Size();
        int m = 0;
        for ( int i=0; i < n; i++)
        {

            // check if it's already in the object transform array
            if ( this->CheckEntityIsInObjectTransform( this->selection[ i ] ) )
            {
                continue;
            }

            // determine if selected entity is an indoor
            nEntityObject *potentialIndoor = this->selection[ i ];
            n_assert( potentialIndoor );
            ncSpatialSpace *spaceComp = 0;

            if ( ! potentialIndoor->GetClass()->IsA("nemirage") )
            {
                // Selected entity can be an indoor (i.e. have ncSpatialSpace)
                spaceComp = potentialIndoor->GetComponent<ncSpatialSpace>();
            }
            else
            {
                // Or be a facade
                ncSpatial *facadeSpatComp = potentialIndoor->GetComponentSafe<ncSpatial>();
                spaceComp = facadeSpatComp->GetIndoorSpace();
            }

            bool entityIsAnIndoor = spaceComp && (spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE);

            // Indoors can't be scaled
            if ( entityIsAnIndoor && this->refSelectedTool == this->refScaleTool )
            {
                continue;
            }

            InguiObjectTransform& ot = this->selectionObjTransf.At( m );

            // add the entity to the transform objects
            ot.SetEntity( this->selection[ i ] );
            ot.SetContainingIndoor( 0 );

            // Set collision heightmap to the object transform, depending if it is activated
            if ( outdoor && this->terrainCollisionType != InguiObjectTransform::colNever)
            {
                ot.SetOutdoor( outdoor, this->terrainCollisionType );
            }
            else
            {
                ot.SetOutdoor( 0, this->terrainCollisionType );
            }
            
            m++;

            // If the entity is an indoor space..
            if ( entityIsAnIndoor )
            {
                // Add the dynamic entities that it contains to the entities being transformed
                ncSpatialIndoor* indoor = static_cast<ncSpatialIndoor*>( spaceComp );
                this->indoorEntities.Reset();
                indoor->GetDynamicEntities( this->indoorEntities );

                int n2 = this->indoorEntities.Size();
                for (int j = 0; j < n2; j++)
                {
                    // check it's not already in the array
                    if ( this->CheckEntityIsInObjectTransform( this->indoorEntities[ j ] ) )
                    {
                        continue;
                    }

                    // set flag to refresh entity space
                    ncSpatial *spatComp = this->selection[ i ]->GetComponent<ncSpatial>();
                    if ( spatComp )
                    {
                        spatComp->SetDetermineSpaceFlag( ncSpatial::DS_TRUE );
                    }

                    // add entity to object transforms
                    InguiObjectTransform& ot = this->selectionObjTransf.At( m );

                    ot.SetEntity( this->indoorEntities[ j ] );

                    ot.SetContainingIndoor( this->selection[ i ] );

                    
                    if ( outdoor )
                    {
                        ot.SetOutdoor( outdoor, this->terrainCollisionType );
                    }
                    else
                    {
                        ot.SetOutdoor( 0, this->terrainCollisionType );
                    }

                    m++;
                }
            }
        }

        // Set transform object to tool
        n = this->selectionObjTransf.Size();
        nInguiToolTransform* tool = static_cast<nInguiToolTransform*>(this->refSelectedTool.get());
        if ( n == 0 )
        {
            tool->SetTransformObject( 0 );
        }
        else
        {
            if ( n == 1 )
            {
                tool->SetTransformObject( &this->selectionObjTransf[ 0 ] );
            }
            else
            {
                this->groupTransform.SetSelection( &this->selectionObjTransf );
                tool->SetTransformObject( &this->groupTransform );
            }
            tool->GetTransformObject()->Begin();
        }


        // Update matrices in the tool
        tool->UpdateMatrices();
    }
}
//------------------------------------------------------------------------------
/**
    @brief Get selected tool index
*/
int
nObjectEditorState::GetSelectedToolIndex( void )
{
    return this->selectedToolIndex;
}
//------------------------------------------------------------------------------
/**
    @brief Get selected tool pointer
*/
nInguiTool*
nObjectEditorState::GetSelectedTool( void )
{
    return this->refSelectedTool;
}
//------------------------------------------------------------------------------
/**
    @brief Get number of selected entities
*/
int
nObjectEditorState::GetSelectionCount( void )
{
    return this->selection.Size();
}
//------------------------------------------------------------------------------
/**
    @brief Check selection and remove invalid references
*/
void
nObjectEditorState::CheckSelection( void )
{
    int i = 0;
    while ( i < this->selection.Size())
    {
        if ( ! this->selection[i].isvalid() )
        {
            this->selection.Erase(i);
        }
        else
        {
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Reset selection, signalling the change if appropriate
*/
void
nObjectEditorState::ResetSelection( bool signalChanged)
{
    this->selection.Reset();

    this->AssignToolObjectTransform();
    if ( signalChanged )
    {
        this->SignalSelectionChanged( this );
    }

    // reset the selected entity in the spatial server
    nSpatialServer::Instance()->SetSelectedEntity(0);
    // reset the selected indoors array in the spatial server
    nSpatialServer::Instance()->ResetSelectedIndoors();
}
//------------------------------------------------------------------------------
/**
    @brief Reset selection and signal the change
*/
void
nObjectEditorState::ResetSelection()
{
    this->ResetSelection( true );
}

//------------------------------------------------------------------------------
/**
    @brief Reset selection without signalling the change
*/
void
nObjectEditorState::ResetSelectionSilently()
{
    this->ResetSelection( false );
}

//------------------------------------------------------------------------------
/**
    @brief Add entity to selection without signalling the selection change
*/
void
nObjectEditorState::AddEntityToSelectionSilently( nEntityObjectId id )
{
    this->AddEntityToSelection( id, false );
}
//------------------------------------------------------------------------------
/**
    @brief Add entity to selection and signal the selection change
*/
void
nObjectEditorState::AddEntityToSelection( nEntityObjectId id )
{
    this->AddEntityToSelection( id, true );
}
//------------------------------------------------------------------------------
/**
    @brief Add entity to selection
*/
void
nObjectEditorState::AddEntityToSelection( nEntityObjectId id, bool signalChange )
{
    if ( ! this->CanAddEntityToSelection( id ) )
    {
        return;
    }

    nEntityObject* obj = nEntityObjectServer::Instance()->GetEntityObject( id );

    n_assert( obj );

    // If entity is a terrain cell, check selection mode and select subcells if necessary
    bool dontSelectObj = false;
    if ( obj->IsA( this->terrainCellClass ) )
    {
        if ( ! ( this->currentSelectionMode == ModeTerrainCell ) )
        {
            return;
        }

        ncSpatialQuadtreeCell * cell = obj->GetComponentSafe<ncSpatialQuadtreeCell>();
        if ( ! cell->IsLeaf() )
        {
            this->SelectSubTerrainCells( cell->GetSubcells() );
            dontSelectObj = true;
        }
    }

    if ( ! dontSelectObj )
    {
        this->selection.Append( obj );
    }

    // select indoor if the entity is a facade
    ncSpatial* spatialComp = obj->GetComponent<ncSpatial>();
    if (spatialComp && spatialComp->GetIndoorSpace())
    {
        this->selection.Append( spatialComp->GetIndoorSpace()->GetEntityObject() );
        nSpatialServer::Instance()->AddSelectedIndoor(spatialComp->GetIndoorSpace()->GetEntityObject());
    }

    this->AssignToolObjectTransform();
    if ( signalChange )
    {
        this->SignalSelectionChanged( this );
    }

    if (obj->GetComponent<ncSpatialIndoor>())
    {
        nSpatialServer::Instance()->AddSelectedIndoor(obj);
    }
    else
    {
        // set the first entity in the selection array as the selected entity in the spatial server
        nSpatialServer::Instance()->SetSelectedEntity( this->selection.At(0) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Return true if entity can be added to the selection
*/
bool
nObjectEditorState::CanAddEntityToSelection( nEntityObjectId id )
{
    if ( !id )
    {
        return false;
    }

    nEntityObject* obj = nEntityObjectServer::Instance()->GetEntityObject( id );

    if ( !obj )
    {
        return false;
    }

    // Check that entity is editable
    ncEditor* edComp = obj->GetComponent<ncEditor>();
    if ( edComp && edComp->IsSetEditorKey("editable") && edComp->GetEditorKeyInt("editable") == 0 )
    {
        return false;
    }

    // Check if entity layer is locked
    n_assert( this->refLayerManager.isvalid() );
    if ( edComp )
    {
        nLayer *layer = this->refLayerManager->SearchLayer( edComp->GetLayerId() );
        if ( layer && strcmp("default",layer->GetName()) != 0 && layer->IsLocked() )
        {
            return false;
        }
    }

    if ( this->selection.FindIndex( obj ) != - 1 )
    {
        return false;
    }

    // Check that entity class is selectable
    for (int i = 0; i < this->selectionExceptions.Size(); i++)
    {
        if ( obj->IsA( this->selectionExceptions[i] ) )
        {
            return false;
        }
    }

    // If entity is a terrain cell, check selection mode
    if ( obj->IsA( this->terrainCellClass ) )
    {
        if ( ! ( this->currentSelectionMode == ModeTerrainCell ) )
        {
            return false;
        }
    }
    else
    {
        if ( this->currentSelectionMode == ModeTerrainCell )
        {
            return false;
        }
    }

    ncSubentity* seComp;
    switch ( this->currentSelectionMode )
    {
    case ModeNormal:
        seComp = obj->GetComponent<ncSubentity>();
        if ( seComp && seComp->GetSuperentity() != 0 )
        {
            // Subentities can not be selected in normal mode
            return false;
        }
        break;

    case ModeSubentity:
        seComp = obj->GetComponent<ncSubentity>();
        if ( !seComp || !refCurrentSelectionModeEntity.isvalid() || seComp->GetSuperentity() != refCurrentSelectionModeEntity )
        {
            // Only subentities attached to an entity can be selected in subentity mode
            return false;
        }
        break;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Select subcells of a terrain cell
*/
void
nObjectEditorState::SelectSubTerrainCells( ncSpatialQuadtreeCell** subCells )
{
    if ( subCells )
    {
        for ( int i =0; i < 4; i++ )
        {
            if ( subCells[i]->IsLeaf() )
            {
                nEntityObject * obj = subCells[i]->GetEntityObject();
                if ( this->selection.FindIndex( obj ) == - 1 )
                {
                    this->selection.Append( obj );
                }
            }
            else
            {
                this->SelectSubTerrainCells( subCells[i]->GetSubcells() );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Remove entity from selection silently
*/
void
nObjectEditorState::RemoveEntityFromSelectionSilently( nEntityObjectId id )
{
    this->RemoveEntityFromSelection( id, false );
}

//------------------------------------------------------------------------------
/**
    @brief Remove entity from selection and signal change
*/
void
nObjectEditorState::RemoveEntityFromSelection( nEntityObjectId id )
{
    this->RemoveEntityFromSelection( id, true );
}
//------------------------------------------------------------------------------
/**
    @brief Remove entity from selection
*/
void
nObjectEditorState::RemoveEntityFromSelection( nEntityObjectId id, bool signalChange )
{
    // Remove from current selection

    nEntityObject* ent = nEntityObjectServer::Instance()->GetEntityObject( id );
    if ( !ent ) 
    {
        return;
    }

    int j = this->selection.FindIndex( ent );
    if ( j != -1 )
    {
        this->selection.EraseQuick( j );
    }

    this->AssignToolObjectTransform();
    if ( signalChange )
    {
        this->SignalSelectionChanged( this );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get the current selection mode
*/
int
nObjectEditorState::GetSelectionMode()
{
    return this->currentSelectionMode;
}

//------------------------------------------------------------------------------
/**
    @brief Set the current selection mode
    @param saveChanges Specifies if subentity (or other) changes should be saved or cancelled.
*/
bool
nObjectEditorState::SetSelectionMode( int mode, bool saveChanges )
{
    n_assert( mode >= 0 && mode < LastSelectionMode );
    if ( mode < 0 || mode >= LastSelectionMode || this->currentSelectionMode == mode )
    {
        return false;
    }
    
    ncPhysicsObj* phyComp;

    // Switch current mode
    switch ( this->currentSelectionMode )
    {
    case ModeSubentity:
        // Exiting mode subentity, save subentities changes or cancel (reload them)
        ncSuperentity* supComp = this->refCurrentSelectionModeEntity->GetComponentSafe<ncSuperentity>();

        if ( saveChanges )
        {
            supComp->SaveSubentities();
            supComp->UpdateAllSuperentities();
        }
        else
        {
            supComp->LoadSubentities();
        }

		// Reset selection since selected subentities are no longer valid
	    this->ResetSelection();
	    this->clipboard.Clear();

		// Signal subentities changedthis->SignalSingleEntityPlaced( this );
		this->SignalEntityModified( this );

        break;
    }

    // Switch new mode
    switch ( mode )
    {
    case ModeNormal:
        if ( this->currentSelectionMode == ModeSubentity )
        {
            n_assert( this->refCurrentSelectionModeEntity.isvalid() );
            phyComp = this->refCurrentSelectionModeEntity->GetComponent<ncPhysicsObj>();
            if ( phyComp )
            {
                phyComp->Enable();
            }
        }
        break;

    case ModeSubentity:
        // Get reference to the superentity and disable its BB's physics
        if ( this->selection.Size() == 0 )
        {
            return false;
        }

        if ( this->selection[0]->GetComponent<ncSuperentity>() )
        {
            this->refCurrentSelectionModeEntity = this->selection[0];
        }
        else
        {
            if ( this->selection.Size() > 1 )
            {
                // Try to select other entity as superentity, perhaps the first is the fa
                if ( ! this->selection[1]->GetComponent<ncSuperentity>() )
                {
                    return false;
                }
                this->refCurrentSelectionModeEntity = this->selection[1];
            }
            else
            {
                // Selection is not an indoor
                return false;
            }
        }

        phyComp = this->refCurrentSelectionModeEntity->GetComponent<ncPhysicsObj>();
        if ( phyComp )
        {
            //HACK ma.garcias- this crashes the editor when editing indoor superentities
            //it has been preventively disabled, but needs checking
            //phyComp->Disable();
        }
        break;
    }

    // Reset selection and clipboard
    this->ResetSelection();
    this->clipboard.Clear();

    // Change the mode
    this->currentSelectionMode = mode;

    // Switch mode after having changed
    if ( mode == ModeNormal )
    {
        if ( this->refCurrentSelectionModeEntity.isvalid() )
        {
            // If new mode is Modenormal, set the selection to the superentity
            this->AddEntityToSelection( this->refCurrentSelectionModeEntity->GetId() );
        }
    }

    this->AssignToolObjectTransform();

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Get the current entity associated with the current selection mode
*/
nEntityObject*
nObjectEditorState::GetSelectionModeEntity()
{
    if ( this->currentSelectionMode != ModeNormal && this->refCurrentSelectionModeEntity.isvalid() )
    {
        return this->refCurrentSelectionModeEntity;
    }
    return 0;        
}

//------------------------------------------------------------------------------
/**
    @brief Get selected entity at index
*/
nEntityObject*
nObjectEditorState::GetSelectedEntity( int i )
{
    if ( i < 0 || i >= this->selection.Size() )
    {
        return 0;
    }
    return this->selection[i];
}

//------------------------------------------------------------------------------
/**
    @brief Get selection center point and radius
    @param radius Returnd selection radius
    @return Center of the selection
*/
vector3
nObjectEditorState::GetSelectionCenterPoint(float & radius)
{
    vector3 center(0.0f, 0.0f, 0.0f);
    int n = this->selection.Size();
    radius = 0.0f;

    if ( n == 0 )
    {
        return center;
    }
    for ( int i=0; i < n; i++)
    {
        nEntityObject* ent = this->selection[ i ];
        n_assert( ent );
        ncTransform* tr = ent->GetComponent<ncTransform>();
        n_assert( tr );
        center += tr->GetTransform().pos_component();
    }
    center = center / float( n );

    for ( int i=0; i < n; i++)
    {
        nEntityObject* ent = this->selection[ i ];
        n_assert( ent );
        ncTransform* tr = ent->GetComponent<ncTransform>();
        n_assert( tr );
        float objR = 0.0f;

        nEntityClass* entClass = ent->GetEntityClass();
        ncSpatialClass* spatialClass = entClass->GetComponent<ncSpatialClass>();
        if (spatialClass)
        {
            const bbox3& b = spatialClass->GetOriginalBBox();
            objR = b.diagonal_size();
        }
        radius = max( radius, ( center - tr->GetTransform().pos_component() ).len() + objR );
    }

    return center;
}

//------------------------------------------------------------------------------
/**
    @brief Save: dump to file the set of command to retrieve a selection.
*/
bool
nObjectEditorState::SaveSelectionToFile(const char *filename)
{
    if (this->selection.Size() == 0)
    {
        return false;
    }

    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    nCmd* cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, filename))
    {
        // --- setselectionmode ---
        cmd = ps->GetCmd(this, 'JSSM');
        cmd->In()->SetI(this->GetSelectionMode());
        cmd->In()->SetB(false);
        ps->PutCmd(cmd);

        // --- resetselection ---
        cmd = ps->GetCmd(this, 'JRSE');
        ps->PutCmd(cmd);

        for (int index = 0; index < this->selection.Size(); ++index)
        {
            // --- addentitytoselection ---
            cmd = ps->GetCmd(this, 'JAES');
            cmd->In()->SetI(this->selection[index]->GetId());
            ps->PutCmd(cmd);
        }

        ps->EndObject(true);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Load saved list of selected entities
*/
bool
nObjectEditorState::LoadSelectionFromFile(const char *filename)
{
    if (kernelServer->GetFileServer()->FileExists(filename))
    {
        kernelServer->PushCwd(this);
        kernelServer->Load(filename, false);
        kernelServer->PopCwd();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Get reference to the entity clipboard array
*/
nArray<nEntityObjectId>&
nObjectEditorState::GetClipboard( void )
{
    return this->clipboard;
}

//------------------------------------------------------------------------------
/**
    @brief Get reference to the entity selection array
*/
nArray<nRefEntityObject>&
nObjectEditorState::GetSelection( void )
{
    return this->selection;
}

//------------------------------------------------------------------------------
/**
    @brief Get selected classes
*/
nStringList*
nObjectEditorState::GetSelectedClasses() const
{
    return this->selectedClasses.get();
}

//------------------------------------------------------------------------------
/**
    @brief Get number of tools
*/
int
nObjectEditorState::GetToolCount( void )
{
    return NumObjectTools;
}
//------------------------------------------------------------------------------
/**
    @brief Set world mode on/off for the current tool (the operation is performed world axis-aligned) 
*/
void
nObjectEditorState::SetWorldMode( bool worldMode )
{
    this->inWorldMode = worldMode;
    
    if ( this->refSelectedTool && this->refSelectedTool->IsA( this->transformToolClass ) )
    {
        static_cast<nInguiToolTransform*>(this->refSelectedTool.get())->SetWorldMode( this->inWorldMode );
    }    
}

//------------------------------------------------------------------------------
/**
    @brief Set terrain collision mode: no collision, always collision, only if height is less than terrain
*/
void
nObjectEditorState::SetCanMoveThroughTerrain( int colType )
{
    this->terrainCollisionType = InguiObjectTransform::TerrainCollisionType( colType );

    this->AssignToolObjectTransform();
}

//------------------------------------------------------------------------------
/**
    @brief Update current class list in instancing tools
*/
void
nObjectEditorState::UpdateClassList()
{
    // Placer and multiplacer tools already have a reference to the class names list
}

//------------------------------------------------------------------------------

/**
    @brief Cut: Put selection into clipboard, removing the entities from the level
*/
void
nObjectEditorState::CutEntities()
{

    if ( this->selection.Size() == 0 )
    {
        return;
    }

    // If in mode waypoint, cannot delete or cut
    if ( this->GetSelectionMode() == ModeSubentity )
    {
        return;
    }

    this->clipboard.Clear();
    for (int i=0; i < selection.Size(); i++)
    {
        // Put selection entities into clipboard.
        clipboard.Append( selection[i]->GetId() );

        // Delete subentities
        ncSuperentity* supEntComp = selection[i]->GetComponent<ncSuperentity>();
        if ( supEntComp )
        {
            supEntComp->DeleteSubentities();
        }

    }
    // Make a delete undo command and execute it so the entities are removed
    ObjectPlacingUndoCmd* newCmd = n_new( ObjectPlacingUndoCmd( &this->clipboard, "Cut entities", true ) );
    nUndoServer::Instance()->NewCommand( newCmd );
    newCmd->Execute();

    // Clear selection
    selection.Clear();

    this->AssignToolObjectTransform();

    // Set last clipboard action
    this->clipBoardStatus = ClipCut;

    // Signal GUI that some entity could have been created or deleted
    this->SignalEntityModified( this );
}
//------------------------------------------------------------------------------

/**
    @brief Delete: Similar to cut without putting anything in the clipboard
*/
void
nObjectEditorState::DelEntities()
{
    if ( this->selection.Size() == 0 )
    {
        return;
    }

    // If in mode subentity, delete permanently
    if ( this->GetSelectionMode() == ModeSubentity )
    {
        for ( int i = 0; i < this->selection.Size(); i++)
        {
            ncSubentity* subComp = this->selection[i]->GetComponent<ncSubentity>();
            if ( subComp )
            {
                ncSuperentity* superComp = subComp->GetSuperentity()->GetComponentSafe<ncSuperentity>();
                nObjectInstancer* subEnts = superComp->GetSubentities();
                int index = subEnts->FindIndex( subComp->GetEntityObject() );
                if ( index != -1 )
                {
                    subEnts->Erase( index );
                }
                nWorldInterface::Instance()->DeleteEntityById( subComp->GetEntityObject()->GetId() );

                superComp->UpdateAABB();
            }
        }
    }
    else
    {
        //extract batched entities first
        /// @todo ma.garcias - UndoCmd should be able to re-batch entities when Unexecuted
        for (int i = 0; i < this->selection.Size(); ++i)
        {
            ncSpatialBatch* spatialBatch = this->selection[i]->GetComponent<ncSpatialBatch>();
            if (spatialBatch)
            {
                spatialBatch->ClearSubentities();
            }
        }

        // Make a delete undo command and execute it so the entities are removed
        ObjectPlacingUndoCmd* newCmd = n_new( ObjectPlacingUndoCmd( &this->selection, "Delete entities", true) );
        nUndoServer::Instance()->NewCommand( newCmd );
        newCmd->Execute();
    }

    // Clear selection
    selection.Clear();

    this->AssignToolObjectTransform();

    // Signal GUI that some entity could have been created or deleted
    this->SignalEntityModified( this );
}
//------------------------------------------------------------------------------

/**
    @brief Put selection into clipboard
*/
void
nObjectEditorState::CopyEntities()
{
    if ( this->selection.Size() == 0 )
    {
        return;
    }

    // Put selection entities into clipboard.
    clipboard.Clear();
    for (int i=0; i < selection.Size(); i++)
    {
        if (this->selection[i].isvalid())
        {
            clipboard.Append( selection[i]->GetId() );
        }
    }

    // Set last clipboard action
    this->clipBoardStatus = ClipCopied;
}
//------------------------------------------------------------------------------
/**
    @brief Instance new entities from the clipboard ( except if they were cutted)
*/
void
nObjectEditorState::PasteEntities()
{
    if ( this->clipboard.Size() == 0 )
    {
        return;
    }

    if ( this->clipBoardStatus == ClipCut )
    {
		// If in mode subentities, cannot do cut operations
		if ( this->GetSelectionMode() == ModeSubentity )
		{
			return;
		}

        // Make a 'insert' undo command and execute it so the entities are put again in the level
        ObjectPlacingUndoCmd* newCmd = n_new( ObjectPlacingUndoCmd( &this->clipboard, "Paste entities", false ));
        nUndoServer::Instance()->NewCommand( newCmd );
        newCmd->Execute();

        // Next paste will need cloning
        this->clipBoardStatus = ClipCopied;

        this->selection.Clear();
        for (int i=0; i < this->clipboard.Size(); i++)
        {
            nEntityObject* ent = nEntityObjectServer::Instance()->GetEntityObject( clipboard[i] );

            // Load waypoints of all pasted entities
            ncSuperentity* supEntComp = ent->GetComponent<ncSuperentity>();
            if ( supEntComp )
            {
                supEntComp->LoadSubentities();
            }

            // Set the selection to the cut objects again
            selection.Append( ent );
        }
    }
    else if ( this->clipBoardStatus == ClipCopied )
    {
        this->selection.Clear();

        // Clone entities
        for (int i =0; i < clipboard.Size(); i ++)
        {
            nEntityObjectId id = clipboard[i];
            nEntityObject* oldEntity = nEntityObjectServer::Instance()->GetEntityObject( id );
            nEntityObject* newEntity = static_cast<nEntityObject*>( oldEntity->Clone(0) );
            n_assert( newEntity );
            clipboard[i] = newEntity->GetId();

		    if ( this->GetSelectionMode() == ModeSubentity )
			{
				// Add cloned entity to old entity's superentity
				ncSubentity * subEntComp = oldEntity->GetComponentSafe<ncSubentity>();
				nEntityObject * superEntity = subEntComp->GetSuperentity();
				n_assert( superEntity );
				ncSuperentity* supEntComp = superEntity->GetComponent<ncSuperentity>();
				supEntComp->AddSubentity( newEntity );
				
			}
            else
			{
				// Insert cloned entity in level
				ncEditor::InsertEntityInLevel( newEntity );
			}

            ncTransform* ct = newEntity->GetComponentSafe<ncTransform>();
            ct->SetPosition( ct->GetPosition() );
            ct->SetQuat( ct->GetQuat() );
            ct->SetScale( ct->GetScale() );

            // Update subentities
			ncSuperentity* supEntComp = newEntity->GetComponent<ncSuperentity>();
            if ( supEntComp )
            {
                supEntComp->UpdateSubentities();
            }

            selection.Append( newEntity );
        }

		if ( this->GetSelectionMode() != ModeSubentity )
	    {
			// Make undo command for cloned entities
			ObjectPlacingUndoCmd* newCmd = n_new( ObjectPlacingUndoCmd( &this->clipboard, "Clone entities", false ));
			nUndoServer::Instance()->NewCommand( newCmd );        
		}
    }

    // Update object transform to the pasted entities
    this->AssignToolObjectTransform();

    // Signal GUI that some entity could have been created or deleted
    this->SignalEntityModified( this );
}

//------------------------------------------------------------------------------
/**
    @brief Automatically batch free entities from classes with BatchStatic=true
    grouped by the terrain cell they belong to.
*/
void
nObjectEditorState::CreateTerrainBatches()
{
    nTerrainEditorState* terrainEditor = static_cast<nTerrainEditorState*>(this->app->FindState("terrain"));
    nEntityObject* outdoor = terrainEditor->GetOutdoorEntityObject();
    if (!outdoor)
    {
        return;
    }

    // automatically build batches for the current terrain
    nTerrainBatchBuilder terrainBatchBuilder(outdoor);

    // only for the selected cells, if any
    terrainBatchBuilder.SetSelectedCells(this->GetSelection());
    terrainBatchBuilder.SetBuildSceneResource(this->GetBatchRebuildResource());
    terrainBatchBuilder.SetBatchTestSubentities(this->GetBatchTestSubentities());
    terrainBatchBuilder.SetDefaultMinDistance(this->GetBatchMinDistance());
    terrainBatchBuilder.Build();
}

//------------------------------------------------------------------------------
/**
    @brief Batch selected entities into static geometry batch.
    @todo ma.garcias - if selected entities
*/
void
nObjectEditorState::BatchEntities()
{
    if (this->selection.Size() == 0 || this->GetSelectionMode() == ModeSubentity)
    {
        return;
    }

    //if some of the entities are already batched, and are part of the same batch
    //add the rest of them to the batch, removing them from any other batch they may be in.
    //if any batch is emptied this way, remove it
    nEntityBatchBuilder batchBuilder;
    int i;
    for (i = 0; i < this->selection.Size(); ++i)
    {
        if (this->GetSelectionMode() == ModeTerrainCell)
        {
            ///@todo ma.garcias - batch all entities in the selected cells
            //this->BatchEntitiesInCell(selection[i]->GetComponent<ncSpatialQuadtreeCell>());
        }
        else
        {
            nEntityObject* entity = selection[i];
            ncSpatialBatch* spatialBatch = entity->GetComponent<ncSpatialBatch>();
            if (spatialBatch)
            {
                //if one the selected entity is a batch, set it as the target
                //this is to re-build the scene geometry in the batch
                batchBuilder.SetBatchEntity(entity);
            }
            else
            {
                //check that the entity is not batched already
                ncSpatial* spatialComp = entity->GetComponent<ncSpatial>();
                if (spatialComp && !spatialComp->IsBatched())
                {
                    //TEMP determine which level to use for the static batch?
                    ncSceneLodClass* lodClass = entity->GetClassComponent<ncSceneLodClass>();
                    int level = (lodClass && lodClass->GetNumLevels() > 0) ? lodClass->GetNumLevels() - 1 : 0;
                    batchBuilder.AddEntity(entity, level);//TEMP
                }
            }
        }
    }

    //set default properties for newly created batches
    if (!batchBuilder.GetBatchEntity())
    {
        batchBuilder.SetDefaultMinDistance(this->GetBatchMinDistance());
        batchBuilder.SetBatchTestSubentities(this->GetBatchTestSubentities());
    }

    //build the batch from scratch, or rebuild if there is some entity
    batchBuilder.SetBuildSceneResource(this->GetBatchRebuildResource());
    batchBuilder.Build();

    //signal that the batch has been placed / edited
    if (batchBuilder.GetBatchEntity())
    {
        this->SetEntityPlaced(batchBuilder.GetBatchEntity());
    }
}

//------------------------------------------------------------------------------
/**
    @brief Remove selected entities from the batch they're in.
    Ignore the selected entities if they are not in any batch.
    Destroy any batches that are left empty.
*/
void
nObjectEditorState::RemoveEntitiesFromBatch()
{
    if (this->selection.Size() == 0 || this->GetSelectionMode() == ModeSubentity)
    {
        return;
    }
    
    nEntityBatchBuilder batchBuilder;
    int i;
    for (i = 0; i < this->selection.Size(); ++i)
    {
        if (this->GetSelectionMode() == ModeTerrainCell)
        {
            ///@todo ma.garcias - batch all entities in the selected cells
            //this->BatchEntitiesInCell(selection[i]->GetComponent<ncSpatialQuadtreeCell>());
        }
        else
        {
            nEntityObject* entity = selection[i];
            ncSpatialBatch* spatialBatch = entity->GetComponent<ncSpatialBatch>();
            if (spatialBatch)
            {
                //if one the selected entity is a batch, remove all entities and destroy
                spatialBatch->ClearSubentities();
                nEntityObjectServer::Instance()->RemoveEntityObject(entity);
            }
            else
            {
                //check that the entity is not batched already
                ncSpatial* spatialComp = entity->GetComponent<ncSpatial>();
                if (spatialComp && spatialComp->IsBatched())
                {
                    //remove the entity from its batch
                    spatialComp->GetBatch()->RemoveSubentity(entity);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Force reloading of selected entities
*/
void
nObjectEditorState::ReloadEntities()
{
    if (this->selection.Size() == 0 || this->GetSelectionMode() == ModeSubentity)
    {
        return;
    }

    if (this->GetSelectionMode() == ModeTerrainCell)
    {
        //empty
    }
    else
    {
        int index;
        for (index = 0; index < this->selection.Size(); ++index)
        {
            ncLoader* loader = this->selection[index]->GetComponent<ncLoader>();
            if (loader && loader->AreComponentsValid())
            {
                loader->UnloadComponents();
                nLoaderServer::Instance()->EntityNeedsLoading(selection[index]);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load resources for selected entities.
*/
void
nObjectEditorState::LoadEntities()
{
    if (this->selection.Size() == 0 || this->GetSelectionMode() == ModeSubentity)
    {
        return;
    }

    int i;
    for (i = 0; i < this->selection.Size(); ++i)
    {
        if (this->GetSelectionMode() == ModeTerrainCell)
        {
            this->LoadEntitiesInCell(selection[i]->GetComponent<ncSpatialQuadtreeCell>());
        }
        else
        {
            nLoaderServer::Instance()->EntityNeedsLoading(selection[i]);

            //load entities in indoor cells and their resources
            ncSpatialSpace *indoorSpace = this->selection[i]->GetComponent<ncSpatialSpace>();
            if (indoorSpace)
            {
                this->LoadEntitiesInSpace(indoorSpace);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load resources for selected entities.
*/
void
nObjectEditorState::LoadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell)
{
    n_assert(quadtreeCell);
    ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(quadtreeCell->GetParentSpace());
    ncSpatialQuadtreeCell *currentCell = quadtreeCell;

    while (currentCell)
    {
        //load subentities in the cell first
        currentCell->LoadEntities();

        //load resources for entities in the cell
        const nArray<nEntityObject*> *categories = currentCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*> entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
                if (batchComp)
                {
                    this->LoadEntitiesInBatch(batchComp);
                }
                else
                {
                    if (!currentCell->IsLeaf())
                    {
                        //check the leaf the entity is in
                        ncTransform* transformComp = entities[i]->GetComponent<ncTransform>();
                        if (transformComp)
                        {
                            ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(transformComp->GetPosition());
                            if (!leafCell || (leafCell != quadtreeCell))
                            {
                                continue;
                            }
                        }
                    }
                    nLoaderServer::Instance()->EntityNeedsLoading(entities[i]);
                }
            }
        }
        currentCell = currentCell->GetParentCell();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load resources for selected entities.
*/
void
nObjectEditorState::LoadEntitiesInBatch(ncSpatialBatch* spatialBatch)
{
    n_assert(spatialBatch);

    spatialBatch->LoadSubentities();

    const nArray<nEntityObject*>& subentities = spatialBatch->GetSubentities();
    for (int index = 0; index < subentities.Size(); ++index)
    {
        n_assert( subentities[index] );
        ncSpatialBatch* batch = subentities[index]->GetComponent<ncSpatialBatch>();
        if (batch)
        {
            this->LoadEntitiesInBatch(batch);
        }
        else
        {
            nLoaderServer::Instance()->EntityNeedsLoading(subentities[index]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load resources for entities in the space
*/
void
nObjectEditorState::LoadEntitiesInSpace(ncSpatialSpace* spatialSpace)
{
    // load ids of contained entities
    nLoaderServer::Instance()->LoadSpace(spatialSpace->GetEntityObject());

    // load resources of contained entities
    const nArray<ncSpatialCell*>& cellsArray = spatialSpace->GetCellsArray();
    for (int cellIndex = 0; cellIndex < cellsArray.Size(); ++cellIndex)
    {
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
        {
            const nArray<nEntityObject*>& entities = cellsArray[cellIndex]->GetCategory(catIndex);
            for (int index = 0; index < entities.Size(); ++index)
            {
                nLoaderServer::Instance()->EntityNeedsLoading(entities[index]);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Batch selected entities into static geometry batch.
*/
void
nObjectEditorState::UnloadEntities()
{
    if (this->selection.Size() == 0 || this->GetSelectionMode() == ModeSubentity)
    {
        return;
    }

    int i;
    for (i = 0; i < this->selection.Size(); ++i)
    {
        if (this->GetSelectionMode() == ModeTerrainCell)
        {
            this->UnloadEntitiesInCell(selection[i]->GetComponent<ncSpatialQuadtreeCell>());
        }
        else
        {
            ncSpatialSpace* indoorSpace = selection[i]->GetComponent<ncSpatialSpace>();
            if (indoorSpace)
            {
                this->UnloadEntitiesInSpace(indoorSpace);
            }
            else
            {
                //unload resources for selected entities (not safe to unload them completely)
                nLoaderServer::Instance()->EntityNeedsUnloading(selection[i]);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load resources for selected entities.
*/
void
nObjectEditorState::UnloadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell)
{
    n_assert(quadtreeCell);
    ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(quadtreeCell->GetParentSpace());
    ncSpatialQuadtreeCell *currentCell = quadtreeCell;

    while (currentCell)
    {
        const nArray<nEntityObject*> *categories = currentCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*> entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
                if (batchComp)
                {
                    /// @todo ma.garcias - unload batched entities on demand
                }
                else
                {
                    if (!currentCell->IsLeaf())
                    {
                        //check the leaf the entity is in
                        ncTransform* transformComp = entities[i]->GetComponent<ncTransform>();
                        if (transformComp)
                        {
                            ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(transformComp->GetPosition());
                            if (!leafCell || (leafCell != quadtreeCell))
                            {
                                continue;
                            }
                        }
                    }

                    nLoaderServer::Instance()->UnloadEntitySafe(entities[i]);
                }
            }
        }
        currentCell = currentCell->GetParentCell();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Unload resources for entities in the space
*/
void
nObjectEditorState::UnloadEntitiesInSpace(ncSpatialSpace* spatialSpace)
{
    // unload resources of contained entities, and save their ids if not local
    const nArray<ncSpatialCell*>& cellsArray = spatialSpace->GetCellsArray();
    for (int cellIndex = 0; cellIndex < cellsArray.Size(); ++cellIndex)
    {
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
        {
            const nArray<nEntityObject*>& entities = cellsArray[cellIndex]->GetCategory(catIndex);
            for (int index = 0; index < entities.Size(); ++index)
            {
                nLoaderServer::Instance()->UnloadEntitySafe(entities[index]);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Add the selection to the given load area, depending on its type:
    indoor spaces to indoor areas, selection of terrain cells to outdoor areas.
    Both the type of the area and the selection mode are checked for that matter.
    @param  areaName    The name of a target area in the current level.
*/
void
nObjectEditorState::AddToLoadArea(const char *areaName)
{
    if (this->GetSelectionMode() == ModeSubentity || this->selection.Size() == 0)
    {
        return;
    }

    //check if there exists a load area with this name
    nLoadArea *loadArea = nLoaderServer::Instance()->FindArea(areaName);
    if (!loadArea)
    {
        return;
    }

    //check whether the selection mode is cells or objects
    if (this->GetSelectionMode() == ModeTerrainCell)
    {
        if (!loadArea->GetType() == nLoadArea::OutdoorArea)
        {
            return;
        }

        // find the selected cells in the area
        for (int i = 0; i < this->selection.Size(); ++i)
        {
            //check space id?
            ncSpatialCell *cell = selection[i]->GetComponentSafe<ncSpatialCell>();
            if (!loadArea->ContainsCell(cell->GetId()))
            {
                loadArea->SetSpaceId(cell->GetParentSpace()->GetEntityObject()->GetId());
                loadArea->AddCell(cell->GetId());
            }
        }
    }
    else if (this->GetSelectionMode() == ModeNormal)
    {
        if (!loadArea->GetType() == nLoadArea::IndoorArea)
        {
            return;
        }
        // check if the area is an interior
        for (int i = 0; i < this->selection.Size(); ++i)
        {
            ncSpatialSpace *space = selection[i]->GetComponent<ncSpatialSpace>();
            if (space && space->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
            {
                loadArea->SetSpaceId(selection[i]->GetId());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param  areaName    The name of a target area in the current level.
*/
void
nObjectEditorState::RemoveFromLoadArea(const char *areaName)
{
    //...same set of passes as in AddToLoadArea, only checking that the
    //selected cell/indoor is already in the area.

    if (this->GetSelectionMode() == ModeSubentity || this->selection.Size() == 0)
    {
        return;
    }

    //check if there exists a load area with this name
    nLoadArea *loadArea = nLoaderServer::Instance()->FindArea(areaName);
    if (!loadArea)
    {
        return;
    }

    //check whether the selection mode is cells or objects
    if (this->GetSelectionMode() == ModeTerrainCell)
    {
        if (!loadArea->GetType() == nLoadArea::OutdoorArea)
        {
            return;
        }

        // find the selected cells in the area
        for (int i = 0; i < this->selection.Size(); ++i)
        {
            ncSpatialCell *cell = selection[i]->GetComponentSafe<ncSpatialCell>();
            if (loadArea->ContainsCell(cell->GetId()))
            {
                loadArea->RemoveCell(cell->GetId());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Switch to selection tool, remembering the last tool used. 
    Will return to the previous tool after using the selection
*/
void
nObjectEditorState::SwitchToSelection( nAppViewport* vp, vector2 mp, line3 ray )
{
    this->previousTool = this->GetSelectedToolIndex();
    this->SelectTool( nObjectEditorState::ToolSelection );

    this->refSelectionTool->SetState( nInguiTool::Inactive );
    this->refSelectionTool->selectionOp = nInguiToolSelection::Select;
    this->refSelectionTool->HandleInput( vp );

    if ( this->refSelectionTool->Pick( vp, mp, ray ) > 0.0f )
    {
        this->refSelectionTool->SetState( nInguiTool::Active );
    }
}

//------------------------------------------------------------------------------
/**
    @brief SEt tool previous to selection tool
*/
void
nObjectEditorState::SetPreviousTool()
{
    if ( this->previousTool == InvalidTool )
    {
        return;
    }
    this->SelectTool( this->previousTool );
    this->previousTool = InvalidTool;
}

//------------------------------------------------------------------------------
/**
    @brief Handle input independent of current application state
    To be called from application input handling. This functions handles shortcut keypress for transform tools.
*/
bool
nObjectEditorState::HandleGlobalInput()
{
    nString curStateName = nConjurerApp::Instance()->GetCurrentState();
    nAppState* curState = nConjurerApp::Instance()->FindState( curStateName );
    if ( ! curState->IsA( editorStateClass ) )
    {
        // Don't handle shortcuts from game mode states
        return false;
    }

    nInputServer* inputServer = nInputServer::Instance();

    if ( inputServer->GetButton("selection") )
    {
        nConjurerApp::Instance()->SetState("object");
        this->SelectTool( nObjectEditorState::ToolSelection );
        this->SignalToolSelected( this );
        return true;
    }
    if ( inputServer->GetButton("translation") )
    {
        nConjurerApp::Instance()->SetState("object");
        this->SelectTool( nObjectEditorState::ToolTranslation );
        this->SignalToolSelected( this );
        return true;
    }
    if ( inputServer->GetButton("rotation") )
    {
        nConjurerApp::Instance()->SetState("object");
        this->SelectTool( nObjectEditorState::ToolRotation );
        this->SignalToolSelected( this );
        return true;
    }
    if ( inputServer->GetButton("scale") )
    {
        nConjurerApp::Instance()->SetState("object");
        this->SelectTool( nObjectEditorState::ToolScale );
        this->SignalToolSelected( this );
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Signal that an entity has been placed
*/
void
nObjectEditorState::SetEntityPlaced( nEntityObject* entity )
{
    this->entityPlaced = entity;
    this->SignalSingleEntityPlaced( this );
}

//------------------------------------------------------------------------------
/**
    Get the last entity placed (only valid just after the signal SingleEntityPlaced)
*/
nEntityObject*
nObjectEditorState::GetEntityPlaced() const
{
    return this->entityPlaced;
}

//------------------------------------------------------------------------------
/**
    Get the class name for the first selected entity that is an ancestor of the class passed in
*/
nString
nObjectEditorState::GetNameForFirstSelectedClassThatIsA(const char *parentClassName) const
{
    nString className;
    if ( this->GetSelectedClasses()->Size() > 0 )
    {
        nEntityClass * entClass = nEntityClassServer::Instance()->GetEntityClass( this->GetSelectedClasses()->GetString(0)->Get() );
        if ( entClass->IsA(parentClassName) )
        {
            nString * classNamePtr = this->GetSelectedClasses()->GetString(0);
            n_assert( classNamePtr );
            className = *classNamePtr;
        }
    }    
    return className;
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::ResetSelectionExceptions()
{
    this->selectionExceptions.Reset();

    this->AddSelectionException("neindoorbrush");
    
    this->AddSelectionException("neportal");
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::AddSelectionException(const char* className)
{
    n_assert(className);
    
    nClass *exception = nKernelServer::Instance()->FindClass(className);

    n_assert(exception);

    this->selectionExceptions.Append(exception);
}

//------------------------------------------------------------------------------
/**
*/
void
nObjectEditorState::SetShowInfoOnSelection(bool shouldShowInfo)
{
    this->showInfoOnSelection = shouldShowInfo;
}

//------------------------------------------------------------------------------
/**
*/
bool
nObjectEditorState::SnapSelectionToTerrain()
{
    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert_return2(level, false, "Unable to get current level");

    nEntityObjectId oid = level->FindEntity("outdoor");
    n_assert_return2(oid, false, "Unable to find outdoor");
    
    nEntityObject* outdoor = nEntityObjectServer::Instance()->GetEntityObject(oid);
    n_assert_return2(outdoor, false, "Unable to get outdoor instance");
 
    if (this->selection.Size() > 0)
    {
        // set up array to use for undo process
        nArray<InguiObjectTransform> transformObjects;

        // for each selected item, set up a transform object and apply it
        for (int i = 0; i < this->selection.Size(); i++)
        {
            InguiObjectTransform transformObject;
            transform44 transform;
            // this isn't a real move, so all values are set to zero
            vector3 v(0.0f, 0.0f, 0.0f);
            transform.settranslation(v);

            transformObject.SetEntity(this->selection[i]);
            // set the terrain collision type to always, so the entity will snap to the terrain
            transformObject.SetOutdoor(outdoor, InguiObjectTransform::colSetAlways);
            transformObject.Begin();
            transformObject.SetTransform(transform);
            transformObject.End();

            transformObjects.Append(transformObject);
        }
        
        // build an undo command for all the entities snapped to the terrain
        UndoCmd* newCmd = n_new(ObjectTransformUndoCmd(&transformObjects));
        if ( newCmd )
        {
            nString undoCmdLabel = "Snap to terrain";
            newCmd->SetLabel(undoCmdLabel);
            nUndoServer::Instance()->NewCommand(newCmd);
        }
    }
    return true;
}