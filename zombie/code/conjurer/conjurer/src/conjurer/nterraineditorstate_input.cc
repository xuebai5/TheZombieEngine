#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterraineditorstate_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/nconjurerapp.h"

#include "napplication/napplication.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"

#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "conjurer/ninguiterraintoolflatten.h"
#include "conjurer/ninguiterraintoolslope.h"
#include "conjurer/terrainpaintingundocmd.h"
#include "conjurer/terraingrasseditundocmd.h"
#include "nvegetation/ncterrainvegetationcell.h"
#include "nvegetation/ncterrainvegetationclass.h"

//------------------------------------------------------------------------------
/// Constant factor for terrain tool effect
const float nTerrainEditorState::TerrainToolIntensity = 3.0f;

//------------------------------------------------------------------------------
/**
    @brief Handle mouse and keyb inputs for editing the terrain in the current viewport
    @return True only if an input was processed
*/
bool
nTerrainEditorState::HandleInput(nTime frameTime)
{

    // Check  running physics
    if (!this->currentInguiTool->CanApplyWhenRunningPhysics() && this->GetApp()->IsPhysicsEnabled())
    {
        return nEditorState::HandleInput(frameTime);
    }

    // If there is no heightmap set, do nothing
    if ( ! this->heightMap.isvalid() )
    {
        return nEditorState::HandleInput(frameTime);
    }

    // Get input server reference
    nInputServer* inputServer = nInputServer::Instance();

    // Get current viewport
    nAppViewport* vp = this->refViewportUI->GetCurrentViewport();
    const rectangle& relSize = vp->GetRelSize();

    // Get mouse coordinates in viewport space [0..1], and center them in the viewport [-1..1]
    vector2 mousePos = nInputServer::Instance()->GetMousePos();
    mousePos = mousePos - relSize.v0;
    mousePos.x /= relSize.width();
    mousePos.y /= relSize.height();
    vector2 mp = mousePos * 2 - vector2(1, 1);
    mp.y = -mp.y;

    // Get mouse button state flags
    bool leftButtonPressed = inputServer->GetButton("buton0") || inputServer->GetButton("buton0_ctrl") || inputServer->GetButton("buton0_alt") || inputServer->GetButton("buton0_shift");
    bool leftButtonDown = inputServer->GetButton("buton0_down") || inputServer->GetButton("buton0_down_ctrl") || inputServer->GetButton("buton0_down_alt") || inputServer->GetButton("buton0_down_shift");

    // If the left button is pressed, we have a possible terrain edit action...
    if ( leftButtonPressed || leftButtonDown )
	{
        // Apply the tool
        bool applied = false;
        if ( this->currentInguiTool->IsFirstPick( vp ) )
        {
            // This is the first click
            this->previousViewport = vp;
            int undoRectSize = heightMap->GetSize();
            if ( this->currentInguiTool->IsA( terrainToolGeomClass ) )
            {
                undoRectSize = int( undoRectSize * static_cast<nInguiTerrainToolGeom*>(this->currentInguiTool)->GetDrawResolutionMultiplier() );
            }
            undoRectangle.Set( undoRectSize, undoRectSize, -1, -1);

            applied = this->ApplyTool(frameTime, vp, mp , true,  false);
        }
        else
        {
            // These are the next editing 'events'
            applied = this->ApplyTool(frameTime, vp, mp, false, false);
        }

        // Update cursor 3d drawing needed flag if the tool was applied
        this->currentInguiTool->SetDrawEnabled( applied );
 
        // Actualize the current height in the Height widget
        if ( applied )
        {
            if ( this->selectedTool == Flatten && ((nInguiTerrainToolFlatten*)this->inguiTerrainTool[ Flatten ])->GetAdaptiveIntensity() > 0.0f )
            {
                // Emit signal to make OUTGUI refresh the widget
                static_cast<nInguiTerrainToolFlatten*>(inguiTerrainTool[ selectedTool ])->SignalRefreshFlattenHeight( inguiTerrainTool[ selectedTool ] );
            }

            // Emit signal to make OUTGUI refresh generic info of the tool
            static_cast<nInguiTool*>(inguiTerrainTool[ selectedTool ])->SignalRefreshInfo( inguiTerrainTool[ selectedTool ] );
        }

        return applied;

    }
    else
    {
        // Left mouse button is not pressed.
        // If a tool was being used,
        if ( this->currentInguiTool->GetState() > nInguiTool::Inactive )
        {

            bool isLastPick = this->currentInguiTool->GetState() >= nInguiTool::Inactive;
            if ( isLastPick )
            {
                this->currentInguiTool->SetState( nInguiTool::Finishing );
            }

            // Last apply
            ApplyTool(frameTime, vp, mp, false, false);

            // Create the undo command corresponding to the tool and pass it to the undo server
            if ( this->currentInguiTool->IsA( this->terrainToolGeomClass ) &&
                 this->undoRectangle.x1 > this->undoRectangle.x0 &&
                 this->undoRectangle.z1 > this->undoRectangle.z0 )
            {

                // Make undo rectangle squared
                int sx = abs( undoRectangle.x1 - undoRectangle.x0 );
                int sz = abs( undoRectangle.z1 - undoRectangle.z0 );
                if ( sx != sz )
                {
                    if ( sx > sz )
                    {
                        undoRectangle.z1 = undoRectangle.z0 + sx;
                    }
                    else
                    {
                        undoRectangle.x1 = undoRectangle.x0 + sz;
                    }
                }
                float mult = static_cast<nInguiTerrainToolGeom*>(this->currentInguiTool)->GetDrawResolutionMultiplier();
                int undoRectSize = int( heightMap->GetSize() * mult );
                int curUndoRectSize = this->undoRectangle.x1 - this->undoRectangle.x0 + 1;

                if ( this->selectedTool < Paint )
                {
                    // Terrain geometry tool
                    TerrainUndoCmd* newCmd = n_new( TerrainGeometryUndoCmd( this->heightMapBuffer,
                                                            this->heightMap,
                                                            this->undoRectangle.x0,
                                                            this->undoRectangle.z0,
                                                            curUndoRectSize ) );
                    // Insert command into the undo server
                    if (newCmd)
                    {
                        nString name( inguiTerrainTool[ selectedTool ]->GetLabel() );
                        newCmd->SetLabel( name );
                        nUndoServer::Instance()->NewCommand( newCmd );
                    }
                }
                else if ( this->selectedTool <= Grass )
                {
                    // Paint tool and grass tool

                    nEntityObject * outdoor = this->GetOutdoorEntityObject();
                    n_assert( outdoor );

                    if ( this->selectedTool == Paint )
                    {
                        int cellRes = this->layerManager->GetAllWeightMapsSize();
                        int bx0 = min( this->undoRectangle.x0 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );
                        int bx1 = min( this->undoRectangle.x1 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );
                        int bz0 = min( this->undoRectangle.z0 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );
                        int bz1 = min( this->undoRectangle.z1 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );

                        // Create paint tool undo cmds
                        if ( this->layerManager->GetSelectedLayer() )
                        {
                            for ( int bx = bx0; bx <= bx1; bx++ )
                            {
                                for ( int bz = bz0; bz <= bz1; bz++ )
                                {
                                    nTerrainCellInfo* wmap = this->layerManager->GetTerrainCellInfo( bx, bz );
                                    int n = wmap->GetNumberOfLayers();
                                    for ( int i = 0; i < n; i++ )
                                    {
                                        nTerrainCellInfo::WeightMapLayerInfo& layerInfo = wmap->GetLayerInfo( i );

                                        TerrainPaintingUndoCmd* newCmd = n_new( TerrainPaintingUndoCmd( layerInfo.undoLayer.get_unsafe(),
                                                                                layerInfo.refLayer.get_unsafe(),
                                                                                bx,
                                                                                bz,
                                                                                layerInfo.layerHandle
                                                                                ) );
                                        if ( newCmd )
                                        {
                                            // these flags makes the undo server undo or redo several times ( chained )
                                            newCmd->chainUndo = i > 0 || ( bx > bx0 || bz > bz0 );
                                            newCmd->chainRedo = i < n-1 || ( bx < bx1 || bz < bz1 );

                                            // insert into the undo server the commands
                                            nString name( inguiTerrainTool[ selectedTool ]->GetLabel() );
                                            newCmd->SetLabel( name );
                                            nUndoServer::Instance()->NewCommand( newCmd );
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        ncTerrainVegetationClass * vegClass = outdoor->GetClassComponentSafe<ncTerrainVegetationClass>();
                        int cellRes = vegClass->GetGrowthMapSizeByCell();
                        int bx0 = n_min(this->undoRectangle.x0 / cellRes , this->layerManager->GetMaterialNumBlocks() - 1);
                        int bx1 = n_min(this->undoRectangle.x1 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );
                        int bz0 = n_min(this->undoRectangle.z0 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );
                        int bz1 = n_min(this->undoRectangle.z1 / cellRes, this->layerManager->GetMaterialNumBlocks() - 1 );

                        // Create grass tool undo cmds
                        for ( int bx = bx0; bx <= bx1; bx++ )
                        {
                            for ( int bz = bz0; bz <= bz1; bz++ )
                            {
                                nTerrainCellInfo* cellInfo = this->layerManager->GetTerrainCellInfo( bx, bz );
                                n_assert( cellInfo );

                                // Get cell entity's vegetation component
                                nEntityObject * cellEntity = cellInfo->GetTerrainCell();
                                n_assert(cellEntity);

                                ncTerrainVegetationCell * vegCell = cellEntity->GetComponentSafe<ncTerrainVegetationCell>();

                                TerrainGrassEditUndoCmd* newCmd = n_new( TerrainGrassEditUndoCmd( vegCell->GetUndoGrowthMap(),
                                                                         vegCell->GetValidGrowthMap(),
                                                                         bx,
                                                                         bz
                                                                         ) );
                                if ( newCmd )
                                {
                                    // these flags makes the undo server undo or redo several times ( chained )
                                    newCmd->chainUndo = ( bx > bx0 || bz > bz0 );
                                    newCmd->chainRedo = ( bx < bx1 || bz < bz1 );

                                    // insert into the undo server the commands
                                    nString name( inguiTerrainTool[ selectedTool ]->GetLabel() );
                                    newCmd->SetLabel( name );
                                    nUndoServer::Instance()->NewCommand( newCmd );
                                }
                            }
                        }
                    }

                }

                this->undoRectangle.Set( undoRectSize, undoRectSize, -1, -1);
            }
            else
            {
                // Create undo cmd for other tools (hole,...)
            }

            // The tool is not being applied, reset tool state to inactive
            this->currentInguiTool->SetState( nInguiTool::Inactive );

            return true;
        }
        else
        {

            // Refresh 3d mouse position for visual feedback, with a dummy call to ApplyTool. Also refreshes 3d cursor drawing needed flag
            bool drawFlag = this->ApplyTool(frameTime, vp, mp, false, true );
            this->currentInguiTool->SetDrawEnabled( drawFlag );

            if ( this->currentInguiTool->IsA( terrainToolGeomClass ) )
            {
                if ( inputServer->GetButton("wheel_down_ctrl") )
                {
                    this->MakePaintbrushSmaller();
                }   
                if ( inputServer->GetButton("wheel_up_ctrl") )
                {
                    this->MakePaintbrushBigger();
                }
            }
            if ( drawFlag )
            {
                // Emit signal to make OUTGUI refresh generic info of the tool
                inguiTerrainTool[ selectedTool ]->SignalRefreshInfo( inguiTerrainTool[ selectedTool ] );
            }
        }
    }

    return nEditorState::HandleInput(frameTime);
}

//------------------------------------------------------------------------------
/**
    @brief Apply the selected tool
    @param vp The nAppViewport where the user is editing
    @param mp The mouse position inside the viewport (range -1..1)
    @param firstTime Tells if this is the first click in the viewport
    @param dummy If true, this flag makes the function to actually don't do the action. Only 3d mouse position is computed, for visual feedback.
    @return True only if the heightmap was modified

*/
bool 
nTerrainEditorState::ApplyTool(nTime frameTime, nAppViewport *vp, vector2 mp, bool firstClick, bool dummy )
{  
    if ( currentInguiTool->GetState() < nInguiTool::Finishing )
    {
        // Alter tool depending on keyboard input (alt, control..)
        currentInguiTool->HandleInput( vp );
    }

    // Get heightmap reference
    if ( ! heightMap.isvalid())
    {
        return false;
    }
    nFloatMap* hmap = heightMap.get();

    // Select source paintbrush heightmap -- only if terrain geom tool selected
    nFloatMap* hmapbrush;

    // If the tool is a geometric one, set current terrain paintbrush
    if ( this->currentInguiTool->IsA( this->terrainToolGeomClass ) )
    {
        if (this->selectedPaintbrush < numPredefinedPaintbrushes)
        {
            hmapbrush = predefinedPaintbrush;
        }
        else
        {
            // get user paintbrush from the narray
            hmapbrush = userPaintbrushList[ selectedPaintbrush - numPredefinedPaintbrushes ];
        }
        (static_cast<nInguiTerrainToolGeom*>(this->currentInguiTool))->SetPaintbrush( hmapbrush );
    }
    else
    {
        hmapbrush = 0;
    }

    // Get world ray in mouse position
    line3 ray;
    nInguiTool::Mouse2Ray( vp, mp, ray);

    // Calculate the width of the ray segment so that it passes through the entire heightmap
    float mapSize = ( hmap->GetSize() - 1 ) * hmap->GetGridScale();
    float l = vector2(ray.b.x - 0.5f * mapSize, ray.b.z - 0.5f * mapSize).len();
    l = 4.0f * l + mapSize;
    ray.set(ray.b, ray.b + ray.m * l);

    // Call to tool's pick method
    float t;
    
    if ( !dummy || currentInguiTool->PickWhileIdle() )
    {
        t = currentInguiTool->Pick( vp, mp, ray );
    }
    else
    {
        // If tool doesn't need it, don't call Pick() while not pressing mouse button
        t = -1.0f;
    }
    bool intersect = ( t >= 0.0f );

    // This flag is true if the current tool is flatten and it's adaptive
    bool adaptiveFlatten = this->selectedTool == Flatten && ((nInguiTerrainToolFlatten*)this->inguiTerrainTool[ Flatten ])->GetAdaptiveIntensity() > 0.0f && ! dummy;

    // If there is intersection,
    if ( intersect )
    {
        // Get the new picked position
        currentInguiTool->GetLastPosition( this->lastPoint );
        currentInguiTool->GetLastTerrainCoords( this->currentXMousePos, this->currentZMousePos );

        // If this is the first action event (the first pick or click)
        if ( firstClick )
        {
            if ( this->selectedTool == Flatten && ! dummy )
            {
                // Get the first point reference
                this->firstPoint = this->lastPoint;

                // If this flag is true, get the height from first point picked
                if ( adaptiveFlatten )
                {
                    ((nInguiTerrainToolFlatten*)inguiTerrainTool[ Flatten ])->SetHeight( this->firstPoint.y );
                }
            }
        }
    }

    // If this is a dummy call, just return if the mouse pos lays inside the terrain
    if ( dummy )
    {
        return intersect;
    }

    // Apply the tool
    if ( intersect )
    {
        int xApply = this->currentXMousePos;
        int zApply = this->currentZMousePos;

        if ( hmapbrush != 0 )
        {
            xApply -= hmapbrush->GetSize()/2;
            zApply -= hmapbrush->GetSize()/2;
        }

        float oldH;
        vector3 normal;

        if ( ! hmap->GetHeightNormal( this->lastPoint.x, this->lastPoint.z, oldH, normal ) )
        {
            oldH = 0.0f;
            normal.set(0.0f, 1.0f, 0.0f);
        }

        if ( adaptiveFlatten && ! firstClick )
        {
            if ( this->currentXMousePos >= 0 && this->currentXMousePos < hmap->GetSize() &&
                 this->currentZMousePos >= 0 && this->currentZMousePos < hmap->GetSize() )
            {
                ( static_cast<nInguiTerrainToolFlatten*>
                    (this->inguiTerrainTool[ Flatten ]) )->AdaptHeight( oldH );
            }
        }

        // Applying time duration
        nTime dt = frameTime;
        if ( dt < nInguiTool::StdFrameRate )
        {
            dt = nInguiTool::StdFrameRate;
        }
        dt *= this->TerrainToolIntensity;

        // Set tool state to active, if it is inactive
        if ( this->currentInguiTool->GetState() <= nInguiTool::Inactive )
        {
            this->currentInguiTool->SetState( nInguiTool::Active );
        }

        // Apply the tool
        currentInguiTool->Apply( dt );

        // Check if it's needed to refresh the undo rectangle
        if ( this->currentInguiTool->IsA( this->terrainToolGeomClass ) )
        {
            int undoRectSize = heightMap->GetSize();
            float mul = static_cast<nInguiTerrainToolGeom*>(this->currentInguiTool)->GetDrawResolutionMultiplier();
            if ( mul != 1 )
            {
                // undo rectangle size is multiplied but first make it a power of 2
                undoRectSize--;
                undoRectSize = int( undoRectSize * mul );
            }
            
            int x0 = xApply;
            int z0 = zApply;
            int x1 = x0 + int( hmapbrush->GetSize() );
            int z1 = z0 + int( hmapbrush->GetSize() );

            if ( firstClick )
            {
                undoRectangle.Set( undoRectSize, undoRectSize, -1, -1);
            }
            if ( undoRectangle.x0 > x0 )
            {
                undoRectangle.x0 = max( 0, x0);
            }
            if ( undoRectangle.z0 > z0 )
            {
                undoRectangle.z0 = max( 0, z0);
            }
            if ( undoRectangle.x1 < x1 )
            {
                undoRectangle.x1 = min( undoRectSize - 1, x1);
            }
            if ( undoRectangle.z1 < z1 )
            {
                undoRectangle.z1 = min( undoRectSize - 1, z1);
            }
        }

        // This is testing:
        this->debugLine->RefreshHeights();

        return true;
    }
    return false;
}
