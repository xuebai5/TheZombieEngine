#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nobjecteditorstate_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "conjurer/nobjecteditorstate.h"
#include "conjurer/nconjurerapp.h"
#include "napplication/napplication.h"
#include "napplication/nappviewportui.h"
#include "input/ninputserver.h"
#include "kernel/ntimeserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialquadtreespacebuilder.h"
#include "conjurer/objecttransformundocmd.h"
#include "conjurer/objectplacingundocmd.h"
#include "mathlib/transform44.h"
#include "ndebug/nceditor.h"
#include "conjurer/ninguitooltransform.h"
#include "conjurer/ninguitoolselection.h"
#include "conjurer/ninguitoolplacer.h"
#include "conjurer/ninguiterraintoolmultplacer.h"
#include "conjurer/ninguitoolspherearea.h"
#include "conjurer/ninguitoolprismarea.h"
#include "conjurer/ninguitoolsoundsourcespherearea.h"
#include "conjurer/ninguitoolsoundsourceprismarea.h"
#include "conjurer/ninguitoolmeasure.h"
#include "conjurer/inguiobjecttransform.h"

//------------------------------------------------------------------------------
/**
    @brief Handle mouse and keyb inputs
    @return True only if an input was processed
*/
bool
nObjectEditorState::HandleInput(nTime frameTime)
{
    // Check  running physics
    if ( ! this->refSelectedTool.get()->CanApplyWhenRunningPhysics() && nConjurerApp::Instance()->IsPhysicsEnabled() )
    {
        return nEditorState::HandleInput(frameTime);
    }

    // Get current viewport
    nAppViewport* vp = this->refViewportUI->GetCurrentViewport();

    // Get input server reference
    nInputServer* inputServer = nInputServer::Instance();
 
    // Key input

    // Do key input if tool is not being used
    if ( this->refSelectedTool->GetState() < nInguiTool::Active )
    {

        // Cursor transform
        if ( this->refSelectedTool->IsA( this->transformToolClass ) )
        {
            // Calculate a different factor for the tool translation, rotation or scale
            float factor = vp->GetViewerVelocity() * float(frameTime);
            if ( this->GetSelectedToolIndex() == ToolTranslation )
            {
                factor *= 1.0f;
            }
            else if ( this->GetSelectedToolIndex() == ToolRotation )
            {
                factor *= 10.0f;
            }
            else if ( this->GetSelectedToolIndex() == ToolScale )
            {
                factor = factor * 0.2f + 1.0f;
            }

            // Check key presses to apply transform
            nInguiToolTransform::PickAxisType axisType = nInguiToolTransform::axisNone;
            bool keyTransf = false;

            if ( inputServer->GetButton("cursor_fwd") )
            {
                axisType = nInguiToolTransform::axisZ;
                keyTransf = true;
            }
            else if ( inputServer->GetButton("cursor_aft") )
            {
                factor = -factor;
                axisType = nInguiToolTransform::axisZ;
                keyTransf = true;
            }
            else if ( inputServer->GetButton("cursor_up") )
            {
                axisType = nInguiToolTransform::axisY;
                keyTransf = true;
            }
            else if ( inputServer->GetButton("cursor_down") )
            {
                factor = -factor;
                axisType = nInguiToolTransform::axisY;
                keyTransf = true;
            }
            else if ( inputServer->GetButton("cursor_left") )
            {
                factor = -factor;
                axisType = nInguiToolTransform::axisX;
                keyTransf = true;
            }
            else if ( inputServer->GetButton("cursor_right") )
            {
                axisType = nInguiToolTransform::axisX;
                keyTransf = true;
            }

            if ( keyTransf && ! cursorTransformFlag )
            {
                this->lastCursorTransformAxis = axisType;
            }

            if ( keyTransf && this->lastCursorTransformAxis == axisType )
            {
                // get selection object transform
                nInguiToolTransform* toolTransf = static_cast<nInguiToolTransform*>(this->refSelectedTool.get());
                InguiObjectTransform* objTransf = toolTransf->GetTransformObject();
                if ( objTransf )
                {
                    if ( ! this->cursorTransformFlag )
                    {
                        objTransf->Begin();
                        
                        this->cursorTransformAmount = 0.0f;
                    }

                    // Calculate transform
                    this->cursorTransformAmount = this->cursorTransformAmount + factor;

                    transform44 transform;
                    toolTransf->GetTransformMatrixForAxis( this->cursorTransformAmount, axisType, transform );

                    // Apply transform
                    objTransf->SetTransform( transform );
                    objTransf->End();

                    toolTransf->UpdateMatrices();

                    this->cursorTransformFlag = true;

                    return true;
                }
            }
            else
            {
                if ( this->cursorTransformFlag )
                {
                    // Create undo command
                    UndoCmd* newCmd = n_new( ObjectTransformUndoCmd( &this->selectionObjTransf ) );
                    if ( newCmd )
                    {
                        nString str = refSelectedTool->GetLabel();
                        newCmd->SetLabel( str );
                        nUndoServer::Instance()->NewCommand( newCmd );
                    }

this->AssignToolObjectTransform();

                    this->cursorTransformFlag = false;

                    return true;
                }

                // If a transform tool is selected and any cursor key is pressed, dont do any further input handling
                // because of holding cursor keys could bring two actions at once
                if ( inputServer->GetButton("pitch_up") ||
                     inputServer->GetButton("pitch_down") ||
                     inputServer->GetButton("turn_left") ||
                     inputServer->GetButton("turn_right") ||
                     inputServer->GetButton("strafe_up") ||
                     inputServer->GetButton("strafe_down") )
                {
                    return true;
                }
            }
        }

        // Clipboard ops
        if ( inputServer->GetButton("cut") )
        {
            this->CutEntities();
            return true;
        }
        if ( inputServer->GetButton("copy") )
        {
            this->CopyEntities();
            return true;
        }
        if ( inputServer->GetButton("paste") )
        {
            this->PasteEntities();
            return true;
        }
        if ( inputServer->GetButton("del") )
        {
            this->DelEntities();
            return true;
        }

        // Grid
        if ( inputServer->GetButton("grid") )
        {
            this->grid->SetDrawEnabled( ! this->grid->GetDrawEnabled() );
            return true;
        }

        // lock transform mode
        if ( inputServer->GetButton("togglelock") )
        {
            n_assert( this->refSelectionTool );
            this->refSelectionTool->SetLockedMode( ! this->refSelectionTool->GetLockedMode() );
            this->SignalLockedModeChanged( this );
            return true;
        }
    }

    // Get viewport screen relative size
    const rectangle& relSize = vp->GetRelSize();

    // Get mouse coordinates in viewport space [0..1], and center them in the viewport [-1..1]
    vector2 mp = nInputServer::Instance()->GetMousePos();
    mp = mp - relSize.v0;
    mp.x /= relSize.width();
    mp.y /= relSize.height();
    mp = mp * 2 - vector2(1, 1);
    mp.y = -mp.y;

    // Get mouse button state flags
    bool leftButtonPressed = inputServer->GetButton("buton0") || inputServer->GetButton("buton0_ctrl") || inputServer->GetButton("buton0_alt") || inputServer->GetButton("buton0_shift");
    bool leftButtonDown = inputServer->GetButton("buton0_down") || inputServer->GetButton("buton0_down_ctrl") || inputServer->GetButton("buton0_down_alt") || inputServer->GetButton("buton0_down_shift");

    // Get world ray in mouse position
    line3 ray;
    nInguiTool::Mouse2Ray( vp, mp, ray);
    //Max pick distance is more or less 10 Km
    ray.m *= 10000.0f;

    bool picked = false;

    // Applying time duration
    nTime dt = frameTime;
    if ( dt < nInguiTool::StdFrameRate )
    {
        dt = nInguiTool::StdFrameRate;
    }

    // If the tool was being used, it's the last pick
    bool isLastPick = !leftButtonPressed && !leftButtonDown && this->refSelectedTool->GetState() > nInguiTool::Inactive;
    if ( isLastPick )
    {
        // Set Finishing state to tool
        this->refSelectedTool->SetState( nInguiTool::Finishing );
    }

    // Alter tool depending on keyboard input (alt, control..)
    refSelectedTool->HandleInput( vp );

    // If the left button is pressed, we have a possible tool action...
    if ( leftButtonPressed || leftButtonDown )
	{

        // Begin object transform
        if ( this->refSelectedTool->IsFirstPick( vp ) )
        {
            if ( this->refSelectedTool->IsA( transformToolClass ) && this->selection.Size() > 0 )
            {
                InguiObjectTransform* ot = static_cast<nInguiToolTransform*>(this->refSelectedTool.get())->GetTransformObject();
                if ( ot )
                {
                    ot->Begin();
                }
            }
        }

        if ( this->refSelectedTool->Pick(vp, mp, ray) > 0.0f )
        {
            picked = true;
        }
        if ( picked )
        {
            // Apply the tool at every frame
            if ( this->refSelectedTool->Apply( dt ) )
            {
                if ( this->refSelectedTool == this->refSelectionTool )
                {
                    this->AssignToolObjectTransform();
                }

                this->refSelectedTool->SetState( nInguiTool::Active );

                // Update cursor 3d drawing needed flag
                this->refSelectedTool->SetDrawEnabled( true );

                // Emit signal to make OUTGUI refresh generic info of the tool
                refSelectedTool->SignalRefreshInfo( refSelectedTool );

            }
            else
            {
                this->refSelectedTool->SetState( nInguiTool::NotInited );
            }
        }

        return picked;
    }
    else
    {

        // Determine if pick must be called, wether because it's the last one or because tool always picks
        if ( this->refSelectedTool->PickWhileIdle() || isLastPick )
        {
            picked = this->refSelectedTool->Pick(vp, mp, ray) > 0.0f;
            
            if ( picked && isLastPick)
            {
                // Apply the tool at every frame
                if ( this->refSelectedTool->Apply( dt ) )
                {

                    if ( this->refSelectedTool == this->refSelectionTool )
                    {
                        this->AssignToolObjectTransform();
                    }

                    // Emit signal to make OUTGUI refresh generic info of the tool
                    refSelectedTool->SignalRefreshInfo( refSelectedTool );
                }
                else
                {
                    this->refSelectedTool->SetState( nInguiTool::Inactive );
                    isLastPick = false;
                }
            }
        }

        // The tool is not being applied, reset tool state to inactive
        if ( isLastPick )
        {

            // Create the undo command corresponding to the tool and pass it to the undo server
            // Only in normal mode (not subentity)
            if ( this->GetSelectionMode() == nObjectEditorState::ModeNormal )
            {
                UndoCmd* newCmd = 0;
                if ( this->refSelectedTool->IsA( this->transformToolClass ) )
                {
                    // Object transform tool
                    newCmd = n_new( ObjectTransformUndoCmd( &this->selectionObjTransf ) );
                }
                else if ( this->refSelectedTool == this->placeTool )
                {
                    // Object placing tool
                    newCmd = n_new( ObjectPlacingUndoCmd( this->placeTool->GetEntityList(), this->refSelectedTool->GetLabel().Get() ) );
                }
                else if ( this->refSelectedTool == this->multPlaceTool )
                {
                    // Multi object placing tool
                    newCmd = n_new( ObjectPlacingUndoCmd( this->multPlaceTool->GetEntityList(), this->refSelectedTool->GetLabel().Get() ) );
                }
                else if ( this->refSelectedTool == this->refSelectionTool )
                {
                    // When selection tool has been used, switch to previous tool if there was any
                    if ( this->previousTool >= 0 )
                    {
                        this->SetPreviousTool();
                    }
                }
                else if ( this->refSelectedTool == this->circleTriggerTool )
                {
                    // Sphere trigger
                    newCmd = n_new( ObjectPlacingUndoCmd( this->circleTriggerTool->GetEntityInstance(), this->refSelectedTool->GetLabel().Get() ) );
                }

                if ( newCmd )
                {
                    nString str = refSelectedTool->GetLabel();
                    newCmd->SetLabel( str );
                    nUndoServer::Instance()->NewCommand( newCmd );
                }
            }

            // Reset state
            this->refSelectedTool->SetState( nInguiTool::Inactive );
        }

        // If it's a transform tool, make it read again the transform from object to update the axis
        if ( this->refSelectedTool->IsA( this->transformToolClass ) )
        {
            nInguiToolTransform * tool = static_cast<nInguiToolTransform*>(this->refSelectedTool.get());
            if ( tool->GetTransformObject() )
            {
                tool->GetTransformObject()->Begin();
            }
            tool->UpdateMatrices();
        }
    }

    return nEditorState::HandleInput(frameTime);
}
