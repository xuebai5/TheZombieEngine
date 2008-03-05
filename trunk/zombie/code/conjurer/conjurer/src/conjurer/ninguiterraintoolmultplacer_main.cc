#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolmultplacer_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolmultplacer.h"
#include "kernel/nkernelserver.h"
#include "stdlib.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "util/nstringlist.h"
#include "nphysics/nphycollide.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/ncspatialclass.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/nloaderserver.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "zombieentity/ncsuperentity.h"
#include "nspatial/nspatialserver.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#include "ndebug/nceditorclass.h"
#include "nlayermanager/nlayermanager.h"
#include "nspatial/ncspatial.h"
#endif

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiTerrainToolMultPlacer, "ninguiterraintool");
//------------------------------------------------------------------------------

/**
*/
nInguiTerrainToolMultPlacer::nInguiTerrainToolMultPlacer():
    randomRotation( false ),
    sizeVariation( 0.0f ),
    areaCoverage( 0.1f )
{
    this->label.Set("Multiple object placing");

    this->refGeomCyl = static_cast<nPhyGeomCylinder*>( nKernelServer::Instance()->New("nphygeomcylinder") );

    this->refGeomCyl->SetCategories( -1 );
    this->refGeomCyl->SetCollidesWith( -1 );

    cursorColor = vector4(1.0f, 0.0f, 0.0f, 0.5 );
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolMultPlacer::~nInguiTerrainToolMultPlacer()
{
    this->refGeomCyl->Release();
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiTerrainToolMultPlacer::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    if ( ! this->heightMap.isvalid() || ! this->drawEnabled )
    {
        return;
    }

    this->DrawRoundBrush( false );

#if 0
    // Get a reference to the graphics server
    //nGfxServer2* refGfxServer = nGfxServer2::Instance();

    cursor3DLine.Clear();

    float d = this->GetDiameter() * 0.5f;
    vector2 brushPos(this->lastPos.x, this->lastPos.z );
    
    int np = int( min( 32, d * 4) );
    float a = PI / 4.0f, da = 2.0f*PI / ( np );
    for (int i = 0; i < np; i++)
    {
        cursor3DLine.AddPoint( vector2( sin(a) * d, cos(a) * d ) + brushPos );
        a += da;
    }

    // Draw brush line
    vector3* lineVertices;
    vector3 singlePoint[5];

    np = cursor3DLine.GetNumVertices();
    lineVertices = cursor3DLine.GetVertexBuffer();
        
    vector3 extrude = vector3(0.0f, 1.0f, 0.0f) * max( d * 0.05f, 0.5f);
    this->lineDrawer.DrawExtruded3DLine( lineVertices, np, &this->cursorColor, 1, extrude, true );

    this->drawEnabled = false;

#endif

    if ( this->refGeomCyl )
    {
        this->refGeomCyl->Draw(nGfxServer2::Instance());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiTerrainToolMultPlacer::Apply( nTime /*dt*/ )
{
    n_assert( this->heightMap );

    nObjectEditorState* objState = static_cast<nObjectEditorState*>( nConjurerApp::Instance()->FindState( "object" ) );
    n_assert( objState );

    // Reset instance array if first frame
    if ( this->state <= nInguiTool::Inactive )
    {
        instanceArray.Clear();
        objState->ResetSelection();
    }

    if ( this->classList->Size() == 0 )
    {
        // No class was selected
        return false;
    }

    for (int i = 0; i < this->classList->Size(); i++ )
    {
        nEntityClass* instanceClass = nEntityClassServer::Instance()->GetEntityClass( this->classList->GetString( i )->Get() );
        n_assert( instanceClass );
        if ( instanceClass->IsA( "neindoorclass" ) )
        {
            // Don't allow inddor placing with this tool
            return false;
        }
    }

    float radius = this->GetDiameter() * 0.5f;
    float area = PI * radius * radius * this->areaCoverage;
    vector2 brushPos(this->lastPos.x, this->lastPos.z );

    // Leave the terrain from the 'static' category in physics
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);

    nEntityObjectId tid = level->FindEntity("outdoor");
    ncPhysicsObj* theTerrainPhysicComponent = 0;
    if (tid)
    {
        nEntityObject* theTerrainEntity = nEntityObjectServer::Instance()->GetEntityObject(tid);
        if (theTerrainEntity)
        {
            theTerrainPhysicComponent = theTerrainEntity->GetComponent<ncPhysicsObj>();
            n_assert( theTerrainPhysicComponent );
            theTerrainPhysicComponent->SetCategories( theTerrainPhysicComponent->GetCategories() & !nPhysicsGeom::Static );
        }
    }

    matrix33 cylOrientMatrix;
    cylOrientMatrix.rotate_x( n_deg2rad( 90.0f ) );

    int tries = 0;
    while ( area > 0.0f && tries < 1)
    {
        // Select class to instantiate
        int selected = rand() % this->classList->Size();

        // get class info (get bb and then the radius)
        float objRadius = 1.0f;
        vector3 bbLengths( 1.0f, 1.0f, 1.0f );
        nEntityClass* entClass = nEntityClassServer::Instance()->GetEntityClass( this->classList->GetString( selected )->Get() );
        if ( entClass )
        {
            ncSpatialClass* spatialClass = entClass->GetComponent<ncSpatialClass>();
            if (spatialClass)
            {
                const bbox3& b = spatialClass->GetOriginalBBox();
                vector3 ext = b.extents();
                objRadius = vector3(ext.x, 0.0f, ext.z).len() * 0.5f;
                bbLengths = b.extents() * 2.0f;
            }
        }

        // Select a random point inside the area
        float a = n_rand() * ( 2.0f * PI );
        float r1 = n_rand() * ( 2 * ( radius - objRadius ) );
        vector2 instancePos = brushPos + vector2(sin(a) * r1, cos(a) * r1 );

        #define SIDE_MARGIN 0.01f
        float mapExtent = this->heightMap->GetExtent();
        instancePos.x = min( max( instancePos.x, SIDE_MARGIN ), mapExtent - SIDE_MARGIN );
        instancePos.y = min( max( instancePos.y, SIDE_MARGIN ), mapExtent - SIDE_MARGIN );

        float h;
        vector3 n;
        if ( ! this->heightMap->GetHeightNormal(instancePos.x, instancePos.y, h, n) )
        {
            h = 0.0f;
        }
        vector3 finalPos( instancePos.x, h , instancePos.y);

        bool canPlaceHere = true;

        this->refGeomCyl->SetPosition( finalPos );

        // Scale cylinder to simulate density variation. The larger the cylinder, the less the probability that the entity is placed
        float r = vector2( bbLengths.x, bbLengths.z).len() * 0.5f * sqrt(2.0f);
        r *= pow( 2.0f - this->areaCoverage, 3.0f );

        ((nPhyGeomCylinder*)this->refGeomCyl.get())->SetRadius( r );
        ((nPhyGeomCylinder*)this->refGeomCyl.get())->SetLength( bbLengths.y );
        ((nPhyGeomCylinder*)this->refGeomCyl.get())->SetOrientation( cylOrientMatrix );

        nPhysicsServer* phyServer = nPhysicsServer::Instance();
        nPhyCollide::nContact contact;
        if ( phyServer->Collide( this->refGeomCyl, 1, &contact ) > 0 )
        {
            canPlaceHere = false;
        }

        if ( radius <= objRadius )
        {
            canPlaceHere = false;
        }

        // The position is inside a cell?
        nSpatialServer* spatSrv = nSpatialServer::Instance();
        n_assert( spatSrv );
        if ( spatSrv->SearchCell( this->lastPos ) == 0 )
        {
            canPlaceHere = false;
        }

        if ( canPlaceHere )
        {
            // Instatiate the class
            nEntityObject* placedInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewEntityObject( this->classList->GetString( selected )->Get() ) );
            n_assert( placedInstance );

            // Is indoor flag
            bool isIndoorClass = instanceClass->IsA( "neindoorclass" );

            // demand immediate loading for placed entities
            nLoaderServer::Instance()->EntityNeedsLoading(placedInstance);

#ifndef NGAME
            // Insert the entity in the current layer
            nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
            n_assert( layerManager );
            ncEditor* editorComp = placedInstance->GetComponent<ncEditor>();
            if ( editorComp )
            {
                int selectedLayerId = layerManager->GetSelectedLayerId();
                editorComp->SetLayerId( selectedLayerId );
                if (!layerManager->SearchLayer( selectedLayerId )->IsActive())
                {
                    ncSpatial* spatialComp = placedInstance->GetComponent<ncSpatial>();
                    if (spatialComp)
                    {
                        spatialComp->RemoveTemporary();
                    }
                }
            }
#endif

            // Set entity position
            ncTransform* trComp = placedInstance->GetComponent<ncTransform>();
            trComp->SetPosition( finalPos );

            // Set random orientation, if the option is enabled and not an indoor
            if ( ! isIndoorClass )
            {
                if ( this->randomRotation )
                {
                    quaternion q;
                    q.set_rotate_y( n_rand() * (2*PI) );
                    trComp->SetQuat( q );
                }

                // Set random size, if the option is enabled
                if ( this->sizeVariation != 0.0f )
                {
                    vector3 v(1.0f,1.0f,1.0f);
                    v *= max( 0.1f, 1.0f + ( n_rand() * ( 1.0f ) - 0.5f ) * this->sizeVariation );
                    trComp->SetScale( v );
                }
            }

            // update subentities from this one
            ncSuperentity* supEnt = placedInstance->GetComponent<ncSuperentity>();
            if ( supEnt )
            {
                supEnt->UpdateSubentities();
            }

            // Log entity id in instanceArray
            instanceArray.Append( placedInstance->GetId() );

            // Add entity to physic space
            ncPhysicsObj* phyComp = placedInstance->GetComponent<ncPhysicsObj>();
            if ( phyComp )
            {
                phyComp->AutoInsertInSpace();
            }

            area -= objRadius * objRadius * PI;
            tries = 0;

            // Add entity to selection
            objState->AddEntityToSelection( placedInstance->GetId() );
        }
        tries++;
    }

    // Return again the terrain to the static category
    if ( theTerrainPhysicComponent )
    {
        theTerrainPhysicComponent->SetCategories( theTerrainPhysicComponent->GetCategories() | nPhysicsGeom::Static );
    }

    // Signal GUI that some entity could have been created or deleted
    objState->SignalEntityModified( objState );

    return true;
}
//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
*/
bool
nInguiTerrainToolMultPlacer::HandleInput( nAppViewport* /*vp*/ )
{

    this->drawEnabled = true;

    return false;
}
//------------------------------------------------------------------------------
/**
    @brief Reset list of classes to instantiate
*/
void
nInguiTerrainToolMultPlacer::ResetClassList()
{
    this->classList->Clear();
}
//------------------------------------------------------------------------------
/**
    @brief Add a class
*/
void
nInguiTerrainToolMultPlacer::AddClass( nString name )
{
    nEntityClass* cl = nEntityClassServer::Instance()->GetEntityClass( name.Get() );
    cl->GetComponentSafe<ncTransformClass>();

    this->classList->AppendString( name );
}
//------------------------------------------------------------------------------

