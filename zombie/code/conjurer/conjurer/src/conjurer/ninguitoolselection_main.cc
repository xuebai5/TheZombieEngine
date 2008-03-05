#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
// ninguitoolselection_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolselection.h"
#include "conjurer/ninguitooltransform.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
#include "mathlib/plane.h"
#include "zombieentity/nctransform.h"
#include "nphysics/nphygeomtrimesh.h"
#include "zombieentity/nctransform.h"
#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphycollide.h"
#include "nspatial/ncspatialcamera.h"
#include "conjurer/nconjurerapp.h"

nNebulaScriptClass(nInguiToolSelection, "nroot");
//------------------------------------------------------------------------------

// Indexes for the pyramidal geom for rectangle selection
int indBuf[6][3] = {{0,1,2},
                    {0,2,3},
                    {0,3,4},
                    {0,4,1},
                    {1,4,2},
                    {2,4,3}
                    };

//------------------------------------------------------------------------------
const int MOUSE_TOLERANCE = 15;
const vector4 COLOUR_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const float LABEL_OFFSET = 0.04f;
const int MAX_NUMBER_LABELS_TO_SHOW = 5;
const float LABEL_LIFETIME = 0.8f;

//------------------------------------------------------------------------------
/**
*/
nInguiToolSelection::nInguiToolSelection():
    selectionOp( Select ),
    drawRect( true ),
    rectRange( 1000.0f )
{
    label = "Select entity";

    this->refGeom = static_cast<nPhyGeomTriMesh*>( nKernelServer::Instance()->New("nphygeomtrimesh") );
    this->refGeom->SetCategories( nPhysicsGeom::Check );
    this->refGeom->SetCollidesWith( nPhysicsGeom::Check );

    matrix44 id;
    this->selectionCamera.SetCustomMatrix( id );
    this->refSpatialCamera = nSpatialServer::Instance()->CreateCamera( this->selectionCamera, id, false );
    this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->SetAttachGlobalEntities( false );
    this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->SetUseCameraCollector( true );

    firstRay = vector3( 0.0f, 0.0f, 0.0f );

    this->lineHandler.SetShader("shaders:line_no_z.fx");
    this->lineHandler.SetPrimitiveType( nGfxServer2::LineStrip );
    this->lineHandler.SetVertexComponents( nMesh2::Coord | nMesh2::Color );

    this->pickWhileIdle = false;
    this->lockedMode = false;

    // Do succesive picking for selection
    nInguiToolPhyPick::succesivePicking = true;

    nInguiTool::canApplyWhenRunningPhysics = true;
}

//------------------------------------------------------------------------------
/**
*/
nInguiToolSelection::~nInguiToolSelection()
{
    this->refGeom->Release();
    //@todo camera is destroyed before, but this ref is not invalidated! this->refSpatialCamera->Release();
}

//------------------------------------------------------------------------------
/**
*/
float
nInguiToolSelection::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    n_assert( this->refObjState );

    if ( this->selectionOp == NoSelect )
    {
        return -1.0f;
    }

    // put this here so keeps being shown if left mouse held down
    this->SetTimeToStopDrawingLabels(LABEL_LIFETIME);

    // Plane for calc. frustum vertexes
    const matrix44& vm = vp->GetViewMatrix();
    vector3 n = - vm.z_component();
    vector3 p = vm.pos_component() + n * this->rectRange;
    plane plane0(n.x, n.y, n.z, - n % p);
    float h = -1.0f;

    // vertices (NW, NE, SE, SW)
    vector3 v1, v2, v3, v4;

    // check if the mouse has moved
    const nDisplayMode2& dispMode( nGfxServer2::Instance()->GetDisplayMode() );
    int dx = int( dispMode.GetWidth() * abs( this->firstMousePos.x - mp.x ) );
    int dy = int( dispMode.GetHeight() * abs( this->firstMousePos.y - mp.y ) );

    /// build some tolerance in
    bool mouseMovedInXAndY = ( dx > MOUSE_TOLERANCE ) && ( dy > MOUSE_TOLERANCE );
    bool mouseMovedInXOrY = ( dx > MOUSE_TOLERANCE ) || ( dy > MOUSE_TOLERANCE );

    // Single object selection and NW corner of rectangle calc.
    if ( this->state <= Inactive )
    {
        // If op is not add or sub, start selection
        if ( this->selectionOp == Select )
        {
            this->refObjState->ResetSelection();
            this->initialSelection.Reset();
        }

        // Let it collide with terrain cells if the selection mode is that
        if ( this->refObjState->GetSelectionMode() == nObjectEditorState::ModeTerrainCell )
        {
            this->refGeom->SetCollidesWith( -1 );
        }
        else
        {
            this->refGeom->SetCollidesWith( nPhysicsGeom::Check );
        }

        /// if the mouse has moved, reset the last picked entity
        if ( mouseMovedInXOrY )
        {
            this->refLastPickedEntity = 0;
        }
        float rayPick = nInguiToolPhyPick::Pick(vp, mp, ray);


        if ( rayPick > 0.0f )
        {
            // Single selection
            
            // Add or remove object to the selection
            if ( this->selectionOp == SubSelection )
            {
                // Remove from current selection
                this->refObjState->RemoveEntityFromSelection( this->refLastPickedEntity->GetId() );
            }
            else
            {
                // Add to current selection
                this->refObjState->AddEntityToSelection( this->refLastPickedEntity->GetId() );
            }
        }
        else
        {
            // Restoring previous tool if selecting from a transform tool
            this->refObjState->AssignToolObjectTransform();
        }

        if ( !plane0.intersect( ray, h) )
        {
            return -1.0f;
        }
        v1 = ray.ipol(h);

        this->firstPos = v1;
        this->firstMousePos = mp;

        firstRay = ray.m;

        for (int i=1; i < 5; i++)
        {
            this->vertexBuf[i][0] = this->firstPos.x;
            this->vertexBuf[i][1] = this->firstPos.y;
            this->vertexBuf[i][2] = this->firstPos.z;
        }
        drawRect = true;

        return h;
    }

    // Check if mouse has moved. If so, do rect. selection

    if ( ! mouseMovedInXAndY )
    {
        return 1.0f;
    }

    // Rectangle selection

    // Last vertex (eye point)
    this->vertexBuf[0][0] = ray.b.x;
    this->vertexBuf[0][1] = ray.b.y;
    this->vertexBuf[0][2] = ray.b.z;
    vector3 v0( this->vertexBuf[0][0], this->vertexBuf[0][1], this->vertexBuf[0][2] );

    drawRect = true;

    // Use a camera to do the selection. Set off-center projection of selectionCamera

    // Get viewport camera dimensions
    float minx, maxx, miny, maxy, minz, maxz;
    const nCamera2& vpCamera = vp->GetCamera();
    vpCamera.GetViewVolume( minx, maxx, miny, maxy, minz, maxz );
    
    // Get screen selection rectangle NW and SE corners
    vector2 selNW = vector2( min( firstMousePos.x, mp.x ), min( firstMousePos.y, mp.y ) );
    vector2 selSE = vector2( max( firstMousePos.x, mp.x ), max( firstMousePos.y, mp.y ) );

    // Calc. selection camera dimensions
    float selMinx = selNW.x * ( maxx - minx ) * 0.5f;
    float selMaxx = selSE.x * ( maxx - minx ) * 0.5f;
    float selMiny = selNW.y * ( maxy - miny ) * 0.5f;
    float selMaxy = selSE.y * ( maxy - miny ) * 0.5f;

    matrix44 m;

    m.perspOffCenterRh( selMinx, selMaxx, selMiny, selMaxy, vpCamera.GetNearPlane(), vpCamera.GetFarPlane() );
    this->selectionCamera.SetCustomMatrix( m );

    n_assert( this->refSpatialCamera.isvalid() );

    ncSpatialCamera* spatComp = this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>();

    bool horizonEnabled = nSpatialServer::Instance()->IsDoingHorizonCulling();
    nSpatialServer::Instance()->SetDoHorizonCulling(false);
    spatComp->TurnOn();
    spatComp->Update( vp->Transform().getmatrix(), nConjurerApp::Instance()->GetFrameId() );
    spatComp->Update( this->selectionCamera );
    nSpatialServer::Instance()->DetermineVisibility(spatComp, 0);
    spatComp->TurnOff();
    nSpatialServer::Instance()->SetDoHorizonCulling( horizonEnabled );

    const nArray<nRef<nEntityObject> >& visibleEntities = spatComp->GetVisibleEntities();
        
    // Reset the selection
    this->refObjState->ResetSelectionSilently();

    // Restore initial selection
    for ( int i=0; i < this->initialSelection.Size(); i++)
    {
        this->refObjState->AddEntityToSelectionSilently( this->initialSelection[ i ]->GetId() );
    }

    // Iterate through new selection array
    for ( int i = 0; i < visibleEntities.Size(); i++ )
    {
        if (!visibleEntities[i].isvalid())
        {
            continue;
        }

        nEntityObject *obj = visibleEntities[i];
        nEntityObjectId objId = obj->GetId();

        // @todo JJ Selection exceptions, could be done through ncEditor, adding int key "editable", but would be costly.. or not
        if ( obj->IsA( "nescene") )
        {
            continue;
        }

        // Add or remove object to the selection
        if ( this->selectionOp == SubSelection )
        {
            // Remove from current selection
            this->refObjState->RemoveEntityFromSelectionSilently( objId );
        }
        else
        {
            // Add to current selection
            this->refObjState->AddEntityToSelectionSilently( objId );
        }

        // If finishing, add last selection to initial selection or subtract last selection from it
        if ( this->state == Finishing )
        {
            int objIndex = this->initialSelection.FindIndex( obj );

            if ( this->selectionOp == AddSelection && objIndex == -1 )
            {
                this->initialSelection.Append( obj );
            }
            else if ( this->selectionOp == SubSelection && objIndex != -1 )
            {
                this->initialSelection.Erase( objIndex );
            }
        }

    /// signal the change here, to minimize traffic
    this->refObjState->SignalSelectionChanged(this->refObjState);

    }

    // Update frustum vertex for rectangle drawing..@todo JJ change, can be done just in 2d
    this->selectionCamera.GetViewVolume( minx, maxx, miny, maxy, minz, maxz );

    matrix44 transf = vp->Transform().getmatrix();

    v1 = vector3( minx, miny,  -1.0f ) * this->rectRange;
    v2 = vector3( maxx, miny,  -1.0f ) * this->rectRange;
    v3 = vector3( maxx, maxy,  -1.0f ) * this->rectRange;
    v4 = vector3( minx, maxy,  -1.0f ) * this->rectRange;

    v1 = transf * v1;
    v2 = transf * v2;
    v3 = transf * v3;
    v4 = transf * v4;

    this->vertexBuf[1][0] = v1.x;
    this->vertexBuf[1][1] = v1.y;
    this->vertexBuf[1][2] = v1.z;

    this->vertexBuf[2][0] = v2.x;
    this->vertexBuf[2][1] = v2.y;
    this->vertexBuf[2][2] = v2.z;

    this->vertexBuf[3][0] = v3.x;
    this->vertexBuf[3][1] = v3.y;
    this->vertexBuf[3][2] = v3.z;

    this->vertexBuf[4][0] = v4.x;
    this->vertexBuf[4][1] = v4.y;
    this->vertexBuf[4][2] = v4.z;

    if ( this->state == Finishing )
    {
        // Restoring previous tool if selecting from a transform tool
        this->refObjState->SetPreviousTool();
        return -1.0f;
    }

    return 1.0f;
}

//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @return true if any input (keyboard, mouse or other) was handled
*/
bool
nInguiToolSelection::HandleInput( nAppViewport* /*vp*/ )
{

    if ( this->state == Finishing )
    {
        return true;
    }

    this->selectionOp = Select;
    if ( nInputServer::Instance()->GetButton("buton0_ctrl") || nInputServer::Instance()->GetButton("buton0_down_ctrl"))
    {
        this->selectionOp = AddSelection;
        if ( this->state <= Inactive )
        {
            this->initialSelection = refObjState->GetSelection();
        }
    }
    else if ( nInputServer::Instance()->GetButton("buton0_alt") || nInputServer::Instance()->GetButton("buton0_down_alt"))
    {
        this->selectionOp = SubSelection;
        if ( this->state <= Inactive )
        {
            this->initialSelection = refObjState->GetSelection();
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Apply the tool, given a world ray
    @param ray The ray
    
*/
bool
nInguiToolSelection::Apply( nTime /*dt*/ )
{
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Draw the helper, given a viewport and a camera
    @param vp The viewport
    @param camera The camera
*/
void
nInguiToolSelection::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    n_assert( this->refObjState );
    nArray<nRefEntityObject>& selection = this->refObjState->GetSelection();

    nGfxServer2* refGfxServer = nGfxServer2::Instance();
    refGfxServer->SetTransform( nGfxServer2::Model, matrix44());

    vector4 col( 1.0f, 0.0f, 0.0f, 0.5f );

    for (int i=0;i < selection.Size(); i++)
    {
        ncTransform* tc = selection[i]->GetComponent<ncTransform>();
        n_assert( tc );
        matrix44 m( tc->GetQuat() );
        vector3 scale(tc->GetScale());
        float scaleAv = ( scale.x + scale.y + scale.z ) / 3;
        m.scale( vector3(scaleAv, scaleAv, scaleAv ) );
        m.set_translation( tc->GetPosition() );
        nInguiToolTransform::DrawAxis(m, 1.0f);
    }

    // Draw rectangle selection in 2d
    if ( this->drawRect )
    {
        matrix44 ident;
        vector4 col(1.0f,1.0f,1.0f,1.0f);
        this->vertexBuf[5][0] = this->vertexBuf[1][0];
        this->vertexBuf[5][1] = this->vertexBuf[1][1];
        this->vertexBuf[5][2] = this->vertexBuf[1][2];

        this->lineHandler.BeginLines(ident);
        this->lineHandler.DrawLines3d((vector3*)( this->vertexBuf + 1), 0, 5, col);
        this->lineHandler.EndLines();

        this->drawRect = false;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Draw a box around an entity
    @param col Color to paint it

    BeginShapes and EndShapes must be called prior and after to this called
*/
void
nInguiToolSelection::DrawEntityBB(nEntityObject* entity, vector4 col)
{
    #define margin 0.01f

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    ncPhysicsObj* pc = entity->GetComponent<ncPhysicsObj>();
    // If entity doesn't have physic BB, don't draw selection box.. anyway it can't be selected directly.
    if ( pc && pc->GetNumGeometries() > 0 )
    {
        nPhysicsAABB bbox;
        pc->GetAABB( bbox );
        bbox3 box( vector3( (bbox.minx + bbox.maxx) / 2.0f, (bbox.miny + bbox.maxy) / 2.0f, (bbox.minz + bbox.maxz) / 2.0f ),
                    vector3( (bbox.maxx - bbox.minx) / 2.0f + margin, (bbox.maxy - bbox.miny) / 2.0f + margin, (bbox.maxz - bbox.minz) / 2.0f + margin ) );

        matrix44 m = box.to_matrix44();
        gfxServer->DrawShape( nGfxServer2::Box, m, col );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Draw the bounding box of each selected entity

    BeginShapes and EndShapes must be called externally
*/
void
nInguiToolSelection::DrawSelectionBB()
{
    n_assert( this->refObjState );
    nArray<nRefEntityObject>& selection = this->refObjState->GetSelection();

    if ( selection.Size() == 0 )
    {
        return;
    }

    // Draw selection boxes for selected entities
    for (int i=0;i < selection.Size(); i++)
    {
        if ( selection[ i ].isvalid() )
        {
            this->DrawEntityBB( selection[ i ], vector4(1.0f, 0.0f, 0.0f, 0.2f) );
        }
    }
}
//------------------------------------------------------------------------------
/**
    @brief Draw labels for the selected entities
*/
void
nInguiToolSelection::DrawSelectionLabels()
{
    if ( this->timeToStopDrawingLabels > nApplication::Instance()->GetTime() )
    {
        n_assert( this->refObjState );
        nArray<nRefEntityObject>& selection = this->refObjState->GetSelection();

        int numberOfSelectedItems = selection.Size();
        if ( numberOfSelectedItems == 0 )
        {
            return;
        }

        vector2 textPos;
        textPos.x = this->firstMousePos.x + LABEL_OFFSET;
        textPos.y = -(this->firstMousePos.y);
        
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        if ( numberOfSelectedItems > MAX_NUMBER_LABELS_TO_SHOW ) 
        {
            nString numberItemsSelectedLabel;
            
            numberItemsSelectedLabel.Format("* %d items selected", numberOfSelectedItems);

            gfxServer->Text(numberItemsSelectedLabel.Get(), COLOUR_WHITE, textPos.x, textPos.y);

            textPos.y += ( gfxServer->GetTextExtent( numberItemsSelectedLabel.Get() ).y ) * 2;

            nString maxNumberItemsLabel;
            maxNumberItemsLabel.Format("Info can be shown for a maximum of %d items", MAX_NUMBER_LABELS_TO_SHOW);

            gfxServer->Text(maxNumberItemsLabel.Get(), COLOUR_WHITE, textPos.x, textPos.y);
        }
        else
        {
            for (int i=0; i < numberOfSelectedItems; i++)
            {
                if ( !selection[ i ].isvalid() )
                {
                    continue;
                }
                
                nEntityObject* entity = selection[i];
                n_assert_return(entity, );
    
                nEntityObjectId entityId = entity->GetId();
                n_assert_return(entityId, );
    
                nClass* entityClass = entity->GetClass();
                n_assert_return(entityClass, );

                const char* entityClassName = entityClass->GetName();
                n_assert_return(entityClassName, );

                nString labelString;
                labelString.Format("%d (%s)", entityId, entityClassName);

                gfxServer->Text(labelString.Get(), COLOUR_WHITE, textPos.x, textPos.y);

                textPos.y += ( gfxServer->GetTextExtent( labelString.Get() ).y ) * 2;
            }
        }
    }
}
//------------------------------------------------------------------------------
/**
    @brief Gets locked mode. If in locked mode, transform tools won't switch to selection tool
*/
bool
nInguiToolSelection::GetLockedMode()
{
    return lockedMode;
}

//------------------------------------------------------------------------------
/**
    @brief Sets locked mode.
*/
void
nInguiToolSelection::SetLockedMode( bool locked )
{
    lockedMode = locked;
}
//------------------------------------------------------------------------------
/**
    @brief Return true if I can pick the given entity object
*/
bool
nInguiToolSelection::CanPickEntityObject( nEntityObject* entityObject )
{
    n_assert( entityObject );

    n_assert( this->refObjState );
    
    return this->refObjState->CanAddEntityToSelection( entityObject->GetId() );
}

//------------------------------------------------------------------------------
/**
    @brief Set time to stop drawing labels for the selected items
    @param time The time that the label should be shown
*/
void 
nInguiToolSelection::SetTimeToStopDrawingLabels( nTime time )
{
    n_assert( time );

    this->timeToStopDrawingLabels = max( this->timeToStopDrawingLabels, nTimeServer::Instance()->GetFrameTime() + time );
}
