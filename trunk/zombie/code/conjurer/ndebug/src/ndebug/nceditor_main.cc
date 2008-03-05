//------------------------------------------------------------------------------
//  nceditor_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#ifndef NGAME

#include "ndebug/nceditor.h"
#include "ndebug/nceditorclass.h"
#include "entity/nentityobjectserver.h"

#include "ndebug/ndebugcomponentserver.h"

#include "zombieentity/nctransform.h"
#include "zombieentity/nloaderserver.h"

#include "nspatial/ncspatialindoor.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "nphysics/ncphysicsobj.h"

#ifndef __ZOMBIE_EXPORTER__
#include "ncrnswaypoint/ncrnswaypoint.h"

#include "rnsgameplay/ncgameplay.h"
#endif

#include "nscene/nscenegraph.h"
#include "nscene/ngeometrynode.h"
#include "gfx2/nshapeserver.h"

#include "nlayermanager/nlayermanager.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncEditor,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncEditor::ncEditor():
    drawFlags( None ),
    isDeleted( false )
    //lineHandler(nGfxServer2::LineStrip, nMesh2::Coord)
{
    this->lineHandler.SetShader("shaders:line_grid.fx");
    this->lineHandler.SetPrimitiveType( nGfxServer2::LineStrip );
}

//------------------------------------------------------------------------------
/**
*/
ncEditor::~ncEditor()
{
    // clear class data
    nVariable ** var;
    nString key;
    
    this->editorData.Begin();
    this->editorData.Next( key, var );
    while( var )
    {
        n_delete( *var );
        this->editorData.Next( key, var );
    }

    nDebugComponentServer * server = nDebugComponentServer::Instance();
    if( server )
    {
        server->Remove( this );
    }
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncEditor::InitInstance(nObject::InitInstanceMsg initType)
{
    nDebugComponentServer * server = nDebugComponentServer::Instance();
    if( server && initType != nObject::ReloadedInstance )
    {
        server->Add( this );
    }

    if( initType != nObject::NewInstance )
    {
        n_assert( this->GetEntityObject()->GetId() );
        if( this->GetEntityObject()->GetId() )
        {
            nFile* file = nEntityObjectServer::Instance()->GetChunkFile( this->GetEntityObject()->GetId() );
            // load resource
            nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
            n_assert(ps);
            if( ps && file )
            {
                nKernelServer::Instance()->PushCwd( this->GetEntityObject() );
                nKernelServer::Instance()->Load( file , false );
                nKernelServer::Instance()->PopCwd();
                file->Release();
            }
        }
    }

#ifndef __ZOMBIE_EXPORTER__
    // Set drawing flags
    int flags = 0;
    if (this->entityObject->GetComponent<ncRnsWaypoint>())
    {
        flags |= DrawWaypoint;
    }

    if (this->entityObject->GetComponent<ncScene>())
    {
        flags |= DrawNormals|DrawTangents|DrawBinormals;
    }

    this->SetDrawingFlags( flags );
#endif
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @retval true if key is found
*/
bool
ncEditor::IsSetEditorKey( const nString & key ) const
{
    return (this->editorData[ key ] != 0 );
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the data created
*/
nVariable *
ncEditor::CreateKeyData( const nString & key )
{   
    nVariable ** var = this->editorData[ key ];
    if( ! var )
    {
        nVariable * newVar = n_new( nVariable );
        n_assert( newVar );
        if( ! newVar )
        {
            return 0;
        }

        this->editorData.Add( key, &newVar );
        var = &newVar;
    }

    (*var)->Clear();

    return (*var);
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditor::SetEditorKeyString( const nString & key, const nString & value )
{
    nVariable * var = this->CreateKeyData( key );
    if( var )
    {
        var->SetType( nVariable::String );
        var->SetString( value.Get() );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
const char *
ncEditor::GetEditorKeyString( const nString & key ) const
{
    nVariable ** var = this->editorData[ key ];
    if( var )
    {
        if( nVariable::String == (*var)->GetType() )
        {
            return (*var)->GetString();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditor::SetEditorKeyInt( const nString & key, int value )
{
    nVariable * var = this->CreateKeyData( key );
    if( var )
    {
        var->SetType( nVariable::Int );
        var->SetInt( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
int
ncEditor::GetEditorKeyInt( const nString & key ) const
{
    nVariable ** var = this->editorData[ key ];
    if( var )
    {
        if( nVariable::Int == (*var)->GetType() )
        {
            return (*var)->GetInt();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditor::SetEditorKeyFloat( const nString & key, float value )
{
    nVariable * var = this->CreateKeyData( key );
    if( var )
    {
        var->SetType( nVariable::Float );
        var->SetFloat( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
float
ncEditor::GetEditorKeyFloat( const nString & key ) const
{
    nVariable ** var = this->editorData[ key ];
    if( var )
    {
        if( nVariable::Float == (*var)->GetType() )
        {
            return (*var)->GetFloat();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    set the layer id
*/
void
ncEditor::SetLayerId(int layerId)
{
    this->SetEditorKeyInt(nString("layerId"), layerId);

    ncSpatialIndoor *indoor = this->GetComponent<ncSpatialIndoor>();
    if (indoor)
    {
        indoor->SetLayerId(layerId);
    }    
}

//------------------------------------------------------------------------------
/**
    get the layer id
*/
int
ncEditor::GetLayerId() const
{
    if ( !this->editorData["layerId"] )
    {
        return -1;
    }
    return this->GetEditorKeyInt(nString("layerId"));
}

#ifndef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
    @param extended if draw completed of reduced information
*/
void
ncEditor::Draw( nGfxServer2 * const gfxServer )
{
    // the draw, put information in the screen so, if entity hasn't position ncEditor
    // component will draw nothing.
    ncTransform *transform = this->GetEntityObject()->GetComponent<ncTransform>();
    if( ! transform )
    {
        return;
    }

    // transform the position to screen coordinates
    vector3 pos = transform->GetPosition();

    matrix44 matV = gfxServer->GetTransform( nGfxServer2::View );
    matV *= gfxServer->GetTransform( nGfxServer2::Projection );
    vector4 pos4;
    pos4 = pos;
    pos4 = matV * pos4;

    float centerX = 0.0f;
    float centerY = 0.0f;
    bool visible = false;

    // check if it is visible
    if( pos4.w > 0 )
    {
        visible = true;
        centerX = pos4.x / pos4.w;
        centerY = -pos4.y / pos4.w;
    }

    if( visible )
    {
        const float TEXT_SIZE = 0.08f;
        const vector4 TEXT_LABEL_COLOR = vector4( 0, 1, 0, 0.7f ); 
        const vector4 TEXT_EDITOR_COLOR = vector4( 0, 1, 0, 0.5f ); 
        nString text;
        float offsetY = 0.0f;

        if( this->drawFlags & (DrawLabel|DrawLabelEx) )
        {
            // write entity class
            text =this->GetEntityClass()->GetProperName();
            gfxServer->Text( text.Get(), TEXT_LABEL_COLOR, centerX, centerY + offsetY );
            offsetY += TEXT_SIZE;

            // write gameplay name of entity
            text = "";
            ncGameplay * gameplay = this->GetComponent<ncGameplay>();
            if( gameplay )
            {
                text = gameplay->GetName();
            }

            if( 0 == text.Length() )
            {
                text.SetInt( this->GetEntityObject()->GetId() );
            }
            gfxServer->Text( text.Get(), TEXT_LABEL_COLOR, centerX, centerY + offsetY );
            offsetY += TEXT_SIZE;
        }

        if( this->drawFlags & DrawLabelEx )
        {
            // write editor string data
            nVariable ** var;
            nString key;

            this->editorData.Begin();
            this->editorData.Next( key, var );
            while( var )
            {
                text = key;
                text.Append( " : " );

                switch( (*var)->GetType() )
                {
                    case nVariable::String:
                        text.Append( (*var)->GetString() );
                        break;

                    case nVariable::Int:
                        text.AppendInt( (*var)->GetInt() );
                        break;

                    case nVariable::Float:
                        text.AppendFloat( (*var)->GetFloat() );
                        break;

                    default:
                        n_assert_always();
                }

                gfxServer->Text( text.Get(), TEXT_EDITOR_COLOR, centerX, centerY + offsetY );
                offsetY += TEXT_SIZE;
                this->editorData.Next( key, var );
            }
        }

        // Drawing of sound sources label
        if ( this->entityObject->IsA("nesoundsource") )
        {
            nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
            n_assert( level );
            nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );

            nString layerKey("layerId");
            int layerId = this->GetEditorKeyInt(layerKey);
            nLayer * layer = layerManager->SearchLayer(layerId);

            text = "Sound layer: ";
            text.Append( layer->GetLayerName() );
            gfxServer->Text( text.Get(), TEXT_LABEL_COLOR, centerX, centerY + offsetY );
            offsetY += TEXT_SIZE;
        }
    }

    // Waypoint component drawing
    ncRnsWaypoint *wpComp = this->entityObject->GetComponent<ncRnsWaypoint>();
    if ( ( this->drawFlags & DrawWaypoint ) && wpComp )
    {
        // Draw waypoint object
        const matrix44& m = transform->GetTransform();
        vector4 wpColor;
        if ( wpComp->GetLinksNumber() != wpComp->GetLocalLinksNumber() )
        {
            // Entry/exit connected nodes
            wpColor = vector4( 0.0f, 1.0f, 0.0f, 1.0f );
        }
        else if ( wpComp->GetLinksNumber() > 1 )
        {
            // Intermediate nodes
            wpColor = vector4( 0.0f, 0.5f, 1.0f, 1.0f );
        }
        else
        {
            // Entry/exit unconnected nodes
            wpColor = vector4( 1.0f, 0.0f, 0.0f, 1.0f );
        }
        gfxServer->BeginShapes();
        gfxServer->DrawShape( nGfxServer2::Box, m, wpColor );
        gfxServer->EndShapes();

        // Draw waypoint links
        nEntityClass* cl = this->GetEntityClass();
        ncEditorClass* edCompClass = static_cast<ncEditorClass*>( cl->GetComponent<ncEditorClass>() );
        if ( edCompClass )
        {
            vector4 localColor(0.0f, 0.5f, 1.0f, 1.0f);
            vector4 externalColor(0.0f, 1.0f, 0.0f, 1.0f);
            vector3 points[5];
            points[0] = pos;
            int n = wpComp->GetLinksNumber();
            int en = n - wpComp->GetLocalLinksNumber();
            for ( int i=0; i<n; i++ )
            {
                ncNavNode* targetNode = wpComp->GetLink( i );
                n_assert( targetNode );
                if ( targetNode )
                {
                    targetNode->GetMidpoint( points[1] );

                    matrix44 m;
                    this->lineHandler.BeginLines(m);
                    this->lineHandler.DrawLines3d( points, 0, 2, i < en ? externalColor : localColor );
                    this->lineHandler.EndLines();
                }
            }

/*            matrix44 m;
            vector4 col = vector4(0.0f, 1.0f, 0.2f, 1.0f);
            points[0] = vector3( 0.0f,0.0f,0.0f);
            points[1] = vector3( 1.0f,1.0f,0.0f);
            points[2] = vector3( 1.0f,2.0f,0.0f);
            points[3] = vector3( 1.0f,3.0f,0.0f);
            points[4] = vector3( 1.0f,4.0f,0.0f);
            lineh->BeginLines( m );
            lineh->DrawLines3d( points, 0, 5, col );
            lineh->EndLines();
*/
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw the debug information for the current node in the scene graph param.
    BeginShapes / EndShapes is assumed to be called from outside
    See use example in nViewportDebugModule::DrawNormals
*/
void
ncEditor::Draw( nSceneGraph* sceneGraph )
{
    nShapeServer* shapeServer = nShapeServer::Instance();
    if (this->drawFlags & (DrawNormals|DrawTangents|DrawBinormals))
    {
        static nClass* geometryNodeClass = nKernelServer::ks->FindClass("ngeometrynode");
        n_assert(sceneGraph->GetCurrentNode()->IsA(geometryNodeClass));
        const matrix44& model = sceneGraph->GetModelTransform();
        nGeometryNode* geomNode = (nGeometryNode*) sceneGraph->GetCurrentNode();
        nMesh2* debugMesh;
        if (this->drawFlags & DrawNormals)
        {
            debugMesh = geomNode->GetDebugMesh(sceneGraph, this->entityObject, "normal");
            if (debugMesh)
            {
                shapeServer->DrawShape(debugMesh, model, vector4(1.0f, 0.0f, 0.0f, 0.0f));
            }
        }
        if (this->drawFlags & DrawTangents)
        {
            debugMesh = geomNode->GetDebugMesh(sceneGraph, this->entityObject, "tangent");
            if (debugMesh)
            {
                shapeServer->DrawShape(debugMesh, model, vector4(0.0f, 1.0f, 0.0f, 0.0f));
            }
        }
        if (this->drawFlags & DrawBinormals)
        {
            debugMesh = geomNode->GetDebugMesh(sceneGraph, this->entityObject, "binormal");
            if (debugMesh)
            {
                shapeServer->DrawShape(debugMesh, model, vector4(0.0f, 0.0f, 1.0f, 0.0f));
            }
        }
    }
}
#endif
//------------------------------------------------------------------------------
/**
    @brief Insert entity in the current level and in the corresponding spatial and physic spaces
    @param The entity
*/
void
ncEditor::InsertEntityInLevel( nEntityObject* newEntity )
{
    // demand resource loading
    nLoaderServer::Instance()->EntityNeedsLoading(newEntity);

    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert( level );

    // Add entity to physic space
    ncPhysicsObj* phyComp = newEntity->GetComponent<ncPhysicsObj>();
    if ( phyComp )
    {
        phyComp->AutoInsertInSpace();
    }

    // Set current layer to the object
    nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
    n_assert( layerManager );
    ncEditor* editorComp = newEntity->GetComponent<ncEditor>();
    if ( editorComp )
    {
        editorComp->SetLayerId( layerManager->GetSelectedLayerId() );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set drawing flags
    @param flags Drawing flags
*/
void
ncEditor::SetDrawingFlags( int flags )
{
    this->drawFlags = static_cast<EditorDrawFlags>( flags );
}
//------------------------------------------------------------------------------
/**
    @brief Get drawing flags
    @return Drawing flags
*/
int 
ncEditor::GetDrawingFlags()
{
    return static_cast<int>( this->drawFlags );
}

//------------------------------------------------------------------------------
/**
    @brief Tells if entity has been deleted in the editor but has not been removed from entity server
*/
bool
ncEditor::IsDeleted()
{
    return this->isDeleted;
}

//------------------------------------------------------------------------------
/**
    @brief Sets deletion flag
*/
void
ncEditor::SetDeleted( bool deleted )
{
    this->isDeleted = deleted;
}

#endif//!NGAME

//------------------------------------------------------------------------------
