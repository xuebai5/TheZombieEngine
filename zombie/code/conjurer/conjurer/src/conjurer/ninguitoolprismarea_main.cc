#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolprismarea_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolprismarea.h"
#include "kernel/nkernelserver.h"
#include "ntrigger/nctriggershape.h"
#include "conjurer/objectplacingundocmd.h"
#include "conjurer/polygontriggerundocmd.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "ndebug/nceditor.h"
#include "zombieentity/nctransform.h"
#include "input/ninputserver.h"
#include "nlayermanager/nlayermanager.h"
#include "mathlib/polygon.h"
#include "nworldinterface/nworldinterface.h"
#include "zombieentity/ncsubentity.h"
#include "zombieentity/ncsuperentity.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiToolPrismArea, "ninguitool");

//------------------------------------------------------------------------------
const float screenVertexSize = 0.01f;
const float edgeRelativeSize = 0.01f;

//------------------------------------------------------------------------------
/**
*/
nInguiToolPrismArea::nInguiToolPrismArea():
    prismHeight(0.0f),
    triggerPosSet( false ),
    selectedVertex( -1 )
{
    // Default shader for line drawer
    this->lineDrawer.SetShaderPath("shaders:defaulttool.fx");

    this->label = "Place polygon area trigger";

    this->pickWhileIdle = true;

    this->triggerClassTypeName = "neareatrigger";

    this->triggerClassName = this->triggerClassTypeName;

}
//------------------------------------------------------------------------------
/**
*/
nInguiToolPrismArea::~nInguiToolPrismArea()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
*/
bool
nInguiToolPrismArea::HandleInput( nAppViewport* /*vp*/ )
{
    // End the line
    if ( nInputServer::Instance()->GetButton("return") )
    {
        this->FinishLine();
        return true;
    }

    // Cancel the line
    if ( nInputServer::Instance()->GetButton("cancel") )
    {
        this->CancelLine();
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------   
/**
*/
float
nInguiToolPrismArea::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    float t = 0.0f;

    if ( ! this->triggerPosSet )
    {
        t = nInguiToolPhyPick::Pick(vp, mp, ray);
        if ( t <= 0.0f )
        {
            return -1.0f;
        }
    }
    else
    {
        // Calculate position of new point to be added
        plane p( vector3(0.0f, this->triggerPos.y, 0.0f), vector3(1.0f, this->triggerPos.y, 0.0f), vector3(0.0f, this->triggerPos.y, 1.0f) );
        if ( p.intersect(ray, t) )
        {
            vector3 point = ray.ipol( t );

            // Can be used when drawing the current position before adding to the line
            this->lastPos = point;
        }
        else
        {
            t = -1.0f;
        }
    }

    // Try to select a vertex for visual feedback
    if ( this->GetState() <= nInguiTool::Inactive )
    {
        this->selectedVertex = this->SelectVertex( this->lastPos );
    }

    return t;
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolPrismArea::Apply( nTime /*dt*/ )
{

    if ( ! this->triggerPosSet )
    {
        this->triggerPos = this->lastPos;
        triggerPosSet = true;
    }
    else
    {
        if ( this->GetState() <= nInguiTool::Inactive )
        {
            // Try to select a vertex
            this->selectedVertex = this->SelectVertex( this->lastPos );

            // If didn't select any vertex..
            if ( this->selectedVertex < 0 )
            {
                // Try to select an edge: inserts point
                polygon pol( this->line );
                int segment;
                float d = pol.GetDistance2d( this->lastPos, segment );
                if ( segment >= 0 && abs( d ) > 0.0f && abs( d ) < 1.0f + pol.GetSegmentWidth( segment ) * edgeRelativeSize )
                {
                    // Insert new point and select it
                    int insertPos = ( segment + 1 ) % this->line.Size();
                    this->line.Insert( insertPos, this->lastPos );
                    this->selectedVertex = insertPos;

                    this->vertexSizes.Insert( insertPos, 1.0f );
                }
                else
                {
                    // Create new point and select it
                    this->line.Append( this->lastPos );
                    this->selectedVertex = this->line.Size() - 1;

                    this->vertexSizes.Append( 1.0f );
                }
            }
        }

        // Move selected point
        if ( this->selectedVertex >= 0 && this->selectedVertex < this->line.Size() )
        {
            this->line[ this->selectedVertex ] = this->lastPos;
        }

        // If line is not convex, remove selected point
        polygon pol( this->line );
        if ( this->selectedVertex >= 0 && this->line.Size() > 3 && ! pol.IsConvex() )
        {
            this->line.EraseQuick( this->selectedVertex );
            this->vertexSizes.EraseQuick( this->selectedVertex );
            this->selectedVertex = -1;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Set height of prism
    @param h Height. 0 means infinite (2D polygon test)
*/
void
nInguiToolPrismArea::SetHeight( float h )
{
    if ( h < 0.0f)
    {
        h = 0.0f;
    }
    this->prismHeight = h;
}

//------------------------------------------------------------------------------
/**
*/
float
nInguiToolPrismArea::GetHeight()
{
    return this->prismHeight;
}

//------------------------------------------------------------------------------
void
nInguiToolPrismArea::SetState( int s )
{
    nInguiTool::SetState( s );

    if ( s == nInguiTool::NotInited )
    {
        this->CancelLine();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Called when tool has been selected
*/
void
nInguiToolPrismArea::OnSelected()
{
    this->entityInstance = 0;

    // Check if selection is a polygon area trigger
    if ( 1 == this->refObjState->GetSelectionCount() )
    {
        nEntityObject * selectedEntityObject = this->refObjState->GetSelectedEntity( 0 );
        if ( this->CanEditEntityObject( selectedEntityObject ) )
        {
            // Select the trigger for editing instead of creating a new one
            this->entityInstance = selectedEntityObject;
    
            // Get polygon vertices
            ncTriggerShape* shape = selectedEntityObject->GetComponentSafe<ncTriggerShape>();
            polygon polygon1 = shape->GetPolygon();

			this->triggerPosSet = true;
			this->triggerPos = selectedEntityObject->GetComponentSafe<ncTransform>()->GetPosition();
			this->selectedVertex = polygon1.GetNumVertices() - 1;

            // Fill edit line
            this->line.Reset();
            this->vertexSizes.Reset();
            for ( int i = 0; i < polygon1.GetNumVertices(); i++ )
            {
                this->line.Append( polygon1.GetVertex( i ) + this->triggerPos );
                this->vertexSizes.Append( 1.0f );
            }

            // Set trigger entity dirty
            selectedEntityObject->SetObjectDirty( true );
    
        }
    }
}

//------------------------------------------------------------------------------
void
nInguiToolPrismArea::FinishLine()
{
    if ( this->line.Size() < 3 || ! this->triggerPosSet )
    {
        // Can't create with < 3 points
        this->CancelLine();
        return;
    }

    if ( this->line.Size() > 3 )
    {
        polygon pol( this->line );
        if ( ! pol.IsConvex() )
        {
            // Can't create this line (not convex)
            return;
        }        
    }

    // Create trigger if didn't exist
    bool triggerIsNew = false;
    if ( ! this->entityInstance )
    {
        this->entityInstance = this->CreateTrigger();
        triggerIsNew = true;
    }

    if ( ! this->entityInstance )
    {
        // Failure, trigger couldn't be created
        return;
    }

    // Set trigger position
    this->SetTriggerPosition();

	ncTriggerShape* shape = this->entityInstance->GetComponentSafe<ncTriggerShape>();
	n_assert( shape );

    // Store old vertices
    nArray<vector3> oldVertices;
    if ( ! triggerIsNew )
    {
        const polygon & oldPolygon = shape->GetPolygon();
        oldVertices = oldPolygon.GetVerticesReadOnly();
    }

    // Set trigger shape vertices
    shape->SetPolygonVertices( this->line );

    // Set prism height
    if ( triggerIsNew )
    {
        shape->SetHeight( this->prismHeight );
    }

    // Make undo command
    if ( ! this->entityInstance->GetComponent<ncSubentity>() )
    {
        UndoCmd* newCmd = 0;
        if ( triggerIsNew )
        {
            newCmd = n_new( ObjectPlacingUndoCmd( this->entityInstance, this->GetLabel().Get() ) );
            
            // Signal GUI that some entity could have been created or deleted
            this->refObjState->SetEntityPlaced( this->entityInstance );
        }
        else
        {
            newCmd = n_new( PolygonTriggerUndoCmd( this->entityInstance, &oldVertices, &this->line ) );
        }
        if ( newCmd )
        {
            nString str = this->GetLabel();
            newCmd->SetLabel( str );
            nUndoServer::Instance()->NewCommand( newCmd );
        }
    }

    // Update selection
    this->refObjState->ResetSelection();
    this->refObjState->AddEntityToSelection( this->entityInstance->GetId() );

    // Reset tool
    this->line.Reset();
    this->selectedVertex = -1;
    this->triggerPosSet = false;
    this->entityInstance = 0;
    this->vertexSizes.Reset();

    if ( !this->isSticky )
    {   
        this->refObjState->SelectToolAndSignalChange( nObjectEditorState::ToolSelection );
    }
}


//------------------------------------------------------------------------------
/**
    @brief Obtain class name and if it subentity and creates a trigger
    @returns Created trigger
*/
nEntityObject *
nInguiToolPrismArea::CreateTrigger()
{
    bool isSubentity = this->refObjState->GetSelectionMode() == nObjectEditorState::ModeSubentity;

    // Create trigger
    if ( isSubentity )
    {
        // Subentity
        nEntityObject* superEnt = this->refObjState->GetSelectionModeEntity();
        n_assert( superEnt );
        return nWorldInterface::Instance()->NewLocalEntity( this->triggerClassName.Get(), this->triggerPos, true, superEnt );
    }
    else
    {
        // Normal entity
        return nWorldInterface::Instance()->NewEntity( this->triggerClassName.Get(), this->triggerPos );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInguiToolPrismArea::SetTriggerPosition()
{
    n_assert( this->entityInstance );

    // Set trigger position
    this->entityInstance->GetComponentSafe<ncTransform>()->SetPosition( this->triggerPos );

    if ( this->entityInstance->GetComponent<ncSubentity>() )
    {
        this->entityInstance->GetComponentSafe<ncSubentity>()->UpdateRelativePosition();
    }

    // Make the line relative to first point
    for ( int v = 0; v < this->line.Size(); v++ )
    {
        this->line[ v ] = this->line[ v ] - this->triggerPos;
    }
}

//------------------------------------------------------------------------------
void
nInguiToolPrismArea::CancelLine()
{
    this->line.Reset();
    this->triggerPosSet = false;
    this->selectedVertex = -1;
}

//------------------------------------------------------------------------------
/**
    Get a vertex close to pos parameter
*/
int
nInguiToolPrismArea::SelectVertex(vector3 & pos) const
{
    for ( int i = 0; i < this->line.Size(); i++ )
    {
        if ( ( pos - this->line[i] ).len() <= this->vertexSizes[i] )
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
nInguiToolPrismArea::Draw( nAppViewport* vp, nCamera2* /*camera*/ )
{
    // Get gfx server pointer
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // Draw trigger pos
    if ( triggerPosSet )
    {
        gfxServer->BeginShapes();
        matrix44 m;
        m.scale( vector3( 1.0f, 1.0f, 1.0f ) * 0.15f );
        m.set_translation( this->triggerPos );
        gfxServer->DrawShape( nGfxServer2::Sphere, m, vector4(1.0f, 1.0f, 1.0f, 1.f) );
        gfxServer->EndShapes();
    }

    if ( this->line.Size() > 0 )
    {

        // Draw line
        vector4 col(0.15f, 0.45f, 0.6f, 0.3f);
        vector4 colSel(0.3f, 0.9f, 1.0f, 0.7f);
        vector3 extr(0.0f, max( 0.2f, this->prismHeight ), 0.0f);
        this->lineDrawer.DrawExtruded3DLine( &this->line[ 0 ], this->line.Size(), &col, 1, extr, true );

        gfxServer->BeginShapes();

        // Draw points
        for ( int i = 0; i < this->line.Size(); i++ )
        {
            vector4 c = col;
            if ( this->selectedVertex == i )
            {
                c = colSel;
            }
            
            this->vertexSizes[i] = this->Screen2WorldObjectSize( vp, this->line[ i ], screenVertexSize );
            matrix44 m;
            m.scale( vector3( 1.0f, 1.0f, 1.0f ) * this->vertexSizes[i] );
            m.set_translation( this->line[ i ] );
            gfxServer->DrawShape( nGfxServer2::Sphere, m, c );
        }
        gfxServer->EndShapes();
    }
}
//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolPrismArea::CanEditEntityObject( nEntityObject* anObject)
{
    if ( anObject->IsA( this->triggerClassTypeName.Get() ) )
    {
        ncTriggerShape* shape = anObject->GetComponentSafe<ncTriggerShape>();

        if ( strcmp(shape->GetShapeType(), "polygon") == 0 )
        {
            return true;
        }
     }

    return false;
}
