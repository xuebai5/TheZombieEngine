//-----------------------------------------------------------------------------
//  nphysicsserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialindoor.h"

#ifndef NGAME
    #include "nphysics/nphypickobjmanager.h"
    #include "nphysics/ncphypickableobj.h"
#endif // !NGAME

//-----------------------------------------------------------------------------
// POLICIES
#define __CHANGE_SPACE_SIZE_POLICY__
//-----------------------------------------------------------------------------
namespace
{
    const char* MaterialsPath("/usr/physics/");
    const char* FileMaterialsPath("wc:libs/materials/physics");
    const char* DefaultMaterialName( "defaultmaterial" );
}

//-----------------------------------------------------------------------------

#include "kernel/nlogclass.h"

NCREATELOGLEVEL ( physicsLog, "Physics", false, 1 ) 
    
//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhysicsServer, "nroot");

//-----------------------------------------------------------------------------

nPhysicsServer* nPhysicsServer::Singleton(0);

const phyreal nPhysicsServer::framesPerSecond(phyreal(60));

nPhysicsWorld* nPhysicsServer::worldBeenProcessed(0);

phyreal nPhysicsServer::ProportionValue(0);

const phyreal nPhysicsServer::stepCorrection( phyreal(.25) );

nPhyGeomRay* nPhysicsServer::staticRay(0);

//#ifndef NGAME     // NOTE!!!!!!: This should uncomented b4 compiling the real last definitive game
nPhyMaterial::idmaterial nPhysicsServer::DefaultMaterial(0);
//#endif

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 23-Sep-2004   David Reyes    created
*/
nPhysicsServer::nPhysicsServer() :
#ifndef NGAME
    pickingObjectManager(0),
    draw(0),
    profPhysicsServer("profPhysicsServer", true),
    profPhysicsServerOneLoop("profPhysicsServerOneLoop", true),
    profPhysicsServerCollide("profPhysicsServerCollide", true),
#endif // !NGAME
    containerWorlds( NumInitialWorlds, NumGrowthWorlds ),
    refGeom(0),
    fileServer(0),
    callCounter(0),
    defaultWorld(0),
    psState(Normal),
    iReporting(0)
{
    n_assert2( !Singleton , "Trying to instanciate a second instance of a singleton" );

    Singleton = this;

    this->Reset();

    if( !staticRay )
    {
        staticRay = static_cast<nPhyGeomRay*>(
            nKernelServer::Instance()->New( "nphygeomray" ));
        n_assert2( staticRay, "Failed to create the default ray for the physics server." );
        if ( staticRay )
        {
            staticRay->SetCategories( 0 );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 23-Sep-2004   David Reyes    created
*/
nPhysicsServer::~nPhysicsServer()
{
    // destroying data
    this->Destroy();

    // Has to be the last line.
    Singleton = 0;

    // destorying static ray
    if( staticRay )
    {
        staticRay->Release();
        staticRay = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    get instance pointer

    @return pointer to the only instance of this object

    history:
        - 23-Sep-2004   David Reyes    created
*/
nPhysicsServer* nPhysicsServer::Instance()
{
    n_assert2( Singleton , "Accessing to the physics server without instance." );

    return Singleton;
}


//-----------------------------------------------------------------------------
/**
    Draw all the geometries

    @param nGfxServer2  drawing server

    history:
        - 23-Sep-2004   David Reyes    created
*/
#ifndef NGAME
void nPhysicsServer::Draw( nGfxServer2* server )
{
    if( !server )
    {
        // Nothing to do
        return;
    }

    matrix44 transformmatrix;

    server->SetTransform( nGfxServer2::Model, transformmatrix );

    isDrawingAlphaShapes() = false;

    for( int index(0); index < this->containerWorlds.Size(); ++index )
    {
        this->containerWorlds.GetElementAt(index)->Draw( server ); 
    }
    
    isDrawingAlphaShapes() = true;

    for(nArray<nPhysicsGeom*>::iterator shape(ListAlphaShapes().Begin());
        shape != ListAlphaShapes().End(); ++shape )
    {
        (*shape)->Draw( server );
    }

    ListAlphaShapes().Reset();
    
#ifndef NGAME
    char buffer[4096];
    
    sprintf( buffer, "Contacts: %03d ", nPhysicsWorld::DEBUG_NumberOfContacts );
    
    server->Text(buffer, vector4(1,0,0,1), .7f, -0.85f);

    sprintf( buffer, "Bodies: %03d ", nPhysicsWorld::DEBUG_NumberOfBodies );
    
    server->Text(buffer, vector4(1,0,0,1), .7f, -0.80f);

#endif // _DEBUG

}

//-----------------------------------------------------------------------------
/**
    Update objects when the layers activeness it's changed.

    history:
        - 23-Sep-2004   David Reyes    created
*/
void nPhysicsServer::LayersUpdate()
{
    for( int index(0); index < this->containerWorlds.Size(); ++index )
    {
        this->containerWorlds.GetElementAt(index)->EnableByLayers();
    }
}

#endif // !NGAME

//-----------------------------------------------------------------------------
/**
    Runs the physics server

    @param server   graphics server

    history:
        - 04-Oct-2004   David Reyes    created
        - 27-Oct-2004   David Reyes    Added support to simulate more than one world
*/
void nPhysicsServer::Run( phyreal frames )
{
#ifndef NGAME
    this->profPhysicsServer.StartAccum();
#endif

    if( frames == phyreal(0) )
    {
#ifndef NGAME
        this->profPhysicsServer.StopAccum();
#endif
        return; // Paused Simulation
    }

    int numWorlds(this->GetNumOfWorldsToBeProcessed());

    if( !numWorlds )
    {
#ifndef NGAME
        this->profPhysicsServer.StopAccum();
#endif
        return;
    }

    /// counting time
    callCounter += frames;

    ProportionValue = frames / (phyreal(1) / framesPerSecond);

    if( callCounter < phyreal(1) / framesPerSecond )
    {
#ifndef NGAME
        this->profPhysicsServer.StopAccum();
#endif
        return;
    }

    int loops(int(callCounter / (phyreal(1) / framesPerSecond)));

#if !NGAME && _DEBUG
    // looses a frame every full frame (1 second)    
    loops %= int(framesPerSecond);
#endif

    ProportionValue /= phyreal(loops);

    for( int l(0); l < loops; ++l )
    {

#ifndef NGAME
        if( !l )
        {
            this->profPhysicsServerOneLoop.StartAccum();
        }
#endif
        callCounter -= phyreal(1) / framesPerSecond; 
        
        for( int index(0); index < this->containerWorlds.Size(); ++index )
        {

            nPhysicsWorld* world(this->containerWorlds.GetElementAt(index));

            n_assert2( world, "Data corruption" );

            worldBeenProcessed = world;

            if( world->IsEnabled() == false )
            {
                continue;
            }

            /// Run simulation
            world->RunWorld( phyreal(1) / framesPerSecond );
        }
#ifndef NGAME
        if( !l)
        {
            this->profPhysicsServerOneLoop.StopAccum();
        }
#endif
    }

    for( int index(0); index < this->containerWorlds.Size(); ++index )
    {
        nPhysicsWorld* world(this->containerWorlds.GetElementAt(index));

        n_assert2( world, "Data corruption" );

        worldBeenProcessed = world;

        if( world->IsEnabled() == false )
        {
            continue;
        }

        world->GetObjectManager()->Reset();
    }


    worldBeenProcessed = 0;

#ifndef NGAME
    this->profPhysicsServer.StopAccum();
#endif
}


//-----------------------------------------------------------------------------
/**
    Returns the file server.

    @return file server

    history:
        - 10-Oct-2004   David Reyes    created
*/
nFileServer2* nPhysicsServer::GetFileServer() const
{
    return fileServer;
}

//-----------------------------------------------------------------------------
/**
    Sets the file server.

    @param fileserver  a file server

    history:
        - 10-Oct-2004   David Reyes    created
*/
void nPhysicsServer::SetFileServer( nFileServer2* fileserver )
{
    n_assert2( fileserver , "Null pointer" );

    this->fileServer = fileserver;
}

//-----------------------------------------------------------------------------
/**
    Returns if a geometry it's colliding with any of the others.

    @param geom         geom to be cheked
    @param numContacts  how many contacts should be reported if any
    @param contact      list of contacts

    @return num of contacts reported

    history:
        - 11-Oct-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    new implememtation
*/
int nPhysicsServer::Collide( 
    const nPhysicsGeom *geom,
    int numContacts,
    nPhyCollide::nContact* contact
#ifndef NGAME
    ,bool collideWithNoBodiesWorld 
#endif
    )
{
#ifndef NGAME
    profPhysicsServerCollide.StartAccum();
#endif

    n_assert2( geom , "Null pointer" );

    n_assert2( numContacts , "No contacts required" );

    n_assert2( contact , "Null pointer" );

    int foundContacts(0);

    for( int index(0); index < containerWorlds.Size();++index )
    {
        // for each world check collisions
        nPhysicsWorld* world( containerWorlds.GetElementAt( index ) );

        if( !world->IsEnabled() )
        {
            continue;
        }

#ifndef NGAME
        if( collideWithNoBodiesWorld == false )
        {
            if( world->HasBodies() == false )
            {
                continue;
            }
        }
#endif


        int found(world->Collide( geom, numContacts - foundContacts, &contact[foundContacts] ));

        foundContacts += found;

        if( foundContacts >= numContacts )
        {
            break;
        }
    }
    
#ifndef NGAME
    profPhysicsServerCollide.StopAccum();
#endif
    return foundContacts;
}

//-----------------------------------------------------------------------------
/**
    Inits the server.

    history:
        - 27-Oct-2004   David Reyes    created
*/
void nPhysicsServer::Init()
{
    // Initing Nebula stuff
    this->refPhysicsObjects = kernelServer->New("nroot", "/usr/physics");

    SetFileServer(nFileServer2::Instance());

#ifndef NGAME
    this->pickingObjectManager = 
        static_cast<nPhyPickObjManager*>(nKernelServer::Instance()->New( "nphypickobjmanager" ));

    n_assert2( this->pickingObjectManager, "Failed to create the picking object manager." );

    this->pickingObjectManager->CreateWorld();
#endif // !NGAME

    // creating default's world and world's space
    this->defaultWorld = 
        static_cast<nPhysicsWorld*>(nKernelServer::Instance()->New( "nphysicsworld", "/usr/physics/world" ));

    n_assert2( this->defaultWorld, "Failed to create the defaults world" );

    // createing the world's space
    nPhySpace* worldSpace( static_cast<nPhySpace*>( 
        nKernelServer::Instance()->New( "nphygeomquadtreespace" )));

    n_assert2( worldSpace, "Failed to create the defaults world space" );

    /// setting space name
    worldSpace->SetName( "WorldSpace" );

    // setting world's space
    this->defaultWorld->SetSpace( worldSpace );
}


//-----------------------------------------------------------------------------
/**
    Returns the number of world to be processed.

    @return number of the worlds

    History:
        - 10-Feb-2005   David Reyes    created
*/
int nPhysicsServer::GetNumOfWorldsToBeProcessed() const
{
    // TODO: Once we got the game only compute only once per level loaded
    //  -IDEA: Do it with an static flag that changes the first time, or
    //  when a world it's added or removed from the server

    int numWorlds( this->containerWorlds.Size() );

    for( int index(0); index < numWorlds; ++index )
    {
        nPhysicsWorld* world(this->containerWorlds.GetElementAt(index));

        if( !world->IsEnabled() )
        {
            --numWorlds;
            continue;
        }

#ifndef NGAME
        if( !world->HasBodies() )
        {
            --numWorlds;
        }
#endif
    }

    return numWorlds;
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Adds a pickeable object.

    @param pickableObj a pickable object

    history:
        - 07-Apr-2005   David Reyes    created
*/
void nPhysicsServer::Add( ncPhyPickableObj* pickableObj )
{
    n_assert2( pickableObj, "Null pointer." );

    this->pickingObjectManager->GetWorld()->Add(pickableObj->GetEntityObject());
}

//-----------------------------------------------------------------------------
/**
    Returns if it's drawing alpha shapes.    

    history:
        - 07-Apr-2005   David Reyes    created
*/
bool& nPhysicsServer::isDrawingAlphaShapes()
{
    static bool isalpha(false);

    return isalpha;
}

//-----------------------------------------------------------------------------
/**
    Returns the list of alpha shapes.

    history:
        - 07-Apr-2005   David Reyes    created
*/
nArray<nPhysicsGeom*>& nPhysicsServer::ListAlphaShapes()
{
    static nArray<nPhysicsGeom*> alphaShapes(1,1);

    return alphaShapes;
}

#endif // !NGAME

//-----------------------------------------------------------------------------
/**
    Returns the default world of this physics servers.

    @return world

    history:
        - 07-Apr-2005   David Reyes    created
*/
nPhysicsWorld* nPhysicsServer::GetDefaultWorld() const
{
    tContainerWorlds const& container(this->GetWorldsContainer()); 
#ifndef NGAME
    for( int index(0); index < container.Size(); ++index )
    {
        if( container.GetElementAt(index)->HasBodies() )
        {
            return container.GetElementAt(index);
        }
    }
#else
    if( container.Size() )
    {
        return container.GetElementAt(0);
    }
#endif
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Resets the physics worlds.

    history:
        - 30-May-2005   David Reyes    created
*/
void nPhysicsServer::Reset()
{
    this->Destroy();
    this->Init();
}

//-----------------------------------------------------------------------------
/**
    Destroys server's data.

    history:
        - 30-May-2005   David Reyes    created
*/
void nPhysicsServer::Destroy()
{
#ifndef NGAME
    if( this->pickingObjectManager )
    {
        this->pickingObjectManager->Release();
        this->pickingObjectManager = 0;
    }
#endif

    for( ;this->containerWorlds.Size(); )
    {
        this->containerWorlds.GetElementAt(0)->Release();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the default's world.

    @return world

    history:
        - 30-May-2005   David Reyes    created
*/
nPhysicsWorld* nPhysicsServer::GetDefaultWorld()
{
    return this->defaultWorld;
}

//-----------------------------------------------------------------------------
/**
    Inserts a object in a space.

    @param obj physics object

    history:
        - 01-Jun-2005   David Reyes    created
*/
void nPhysicsServer::InsertInSpace( ncPhysicsObj* obj )
{
    // @todo speed up

    n_assert2( obj, "Null pointer." );

#ifndef NGAME
    /// check if it's an autoinsertable object
    if( obj->GetType() == ncPhysicsObj::Pickeable )
        return; // do nothing
#endif

    if( !obj->GetWorld() ) 
    {
        /// If the object it isn't in the world add to the world
        nPhysicsServer::Instance()->GetDefaultWorld()->Add( obj->GetEntityObject() );
    }

    /// check if it's an autoinsertable object
    if( obj->GetType() == ncPhysicsObj::Indoor )
        return; // do nothing.

    /// check if it's an autoinsertable object
    if( obj->GetType() == ncPhysicsObj::Terrain )
        return; // do nothing.

    const bool IsDynamic( obj->GetCategories() & nPhysicsGeom::Dynamic ? true : false );

    // checking if it's an static object
    const bool IsStatic( obj->GetCategories() & nPhysicsGeom::Static );

    nPhysicsAABB objAABB;

    // retrieving the bounding box
    obj->GetAABB( objAABB );

    // get indoors
    const nArray<ncSpatialIndoor*>& indoors(nSpatialServer::Instance()->GetIndoorSpaces());

    // move it out
    obj->Alienate();

    // stores how many points has in a given space
    int pointsAtIt(0);

    // stores the best guess space
    nPhySpace* bestGuess(0);

    // stores if it's indoor space
    ncPhyIndoor* indoorSpace(0);

    if( !IsDynamic )
    {
        // check if it's inside an indoor
        for( int index(0); index < indoors.Size(); ++index ) 
        {
            ncPhyIndoor* phyIndoor(indoors[index]->GetComponent<ncPhyIndoor>());

            nPhySpace* space(phyIndoor->GetIndoorSpace());

            if( !space )
            {
                continue;
            }

            nPhysicsAABB spaceAABB;

            space->GetAABB( spaceAABB );

            int IndoorHits(0);

            for( int i(0); i < 6; ++i )
            {
                vector3 vertex;
                
                objAABB.GetVertex( i, vertex );

                if( spaceAABB.IsInside( vertex.x, vertex.y, vertex.z ) )
                {
                    if( IsStatic )
                    {
                        // find out if there is more sub-spaces
                        const ncPhyIndoor::tSpaces& spaceContainer(phyIndoor->GetSubSpaces());

                        for( int iSpace(0); iSpace < spaceContainer.Size(); ++iSpace )
                        {
                            nPhySpace* subSpace( spaceContainer.GetElementAt( iSpace ) );

                            n_assert2( subSpace, "Corrupt data." );

                            nPhysicsAABB subspaceAABB;

                            subSpace->GetAABB( subspaceAABB );

                            int SubIndoorHits(0);

                            for( int i(0); i < 6; ++i ) 
                            {
                                vector3 vertex;
                                
                                objAABB.GetVertex( i, vertex );

                                if( subspaceAABB.IsInside( vertex.x, vertex.y, vertex.z ) )
                                {
                                    ++SubIndoorHits;
                                }
                            }                       
                            if( SubIndoorHits > pointsAtIt )
                            {
                                pointsAtIt = SubIndoorHits;
                                bestGuess = subSpace;
                                indoorSpace = phyIndoor;
                                if( IndoorHits == 6 )
                                {
                                    break;
                                }
                            }
                        }
                    }

                    ++IndoorHits;
                }            
            }

            if( IndoorHits > pointsAtIt )
            {
                pointsAtIt = IndoorHits;
                bestGuess = space;
                indoorSpace = phyIndoor;
                if( IndoorHits == 6 )
                {
                    break;
                }
            }
        }
    }

    if( bestGuess )
    {
        obj->MoveToSpace( bestGuess );
        return;
    }

    // getting outdoor entity
    nEntityObject* outdoorEntity = nSpatialServer::Instance()->GetOutdoorEntity();

    if( outdoorEntity )
    {
        ncPhyTerrain* terrain = outdoorEntity->GetComponent<ncPhyTerrain>();

        n_assert2( terrain, "Data corruption." );

        nPhySpace* outdoorSpace(terrain->GetTerrainSpace());

        int cellsPerSide(int(sqrt(phyreal(terrain->GetComponentSafe<ncPhyTerrain>()->GetNumberOfCells()))));
        
        int cellsLengthInteger(int(terrain->GetLenghtCell()*terrain->GetHeightMapInfo()->GetGridScale()));

        if( outdoorSpace )
        {     
            vector3 position;

            obj->GetPosition( position );

            int cellx(int(position.x) / cellsLengthInteger);

            int celly(int(position.z) / cellsLengthInteger);

            int cellIndex(celly * cellsPerSide + cellx);
            
            if ( (0 <= cellIndex) && (cellIndex < terrain->GetNumberOfCells()) )
            {
                if( IsDynamic )
                {
                    obj->MoveToSpace(terrain->GetTerrainCell( cellIndex )->GetDynamicCellSpace());
                }
                else
                {
                    obj->MoveToSpace(terrain->GetTerrainCell( cellIndex )->GetCellSpace());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    INTERNAL, do not call directly (user init instance code).

    @param ignored

    history:
        - 02-Nov-2005   David Reyes    created
*/
void nPhysicsServer::InitInstance(nObject::InitInstanceMsg)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Loads the physics materials.

    history:
        - 02-Nov-2005   David Reyes    created
*/
void nPhysicsServer::LoadMaterials() 
{
    nFileServer2* fserver(nKernelServer::Instance()->GetFileServer());

    n_assert2( fserver, "File server required." );

    nArray<nString> files(fserver->ListFiles( FileMaterialsPath ));

    for( int index(0); index < files.Size(); ++index )
    {
        nString NOHPathName( MaterialsPath );

        NOHPathName += strrchr( files[ index ].Get(), '//' ) + 1;

        NOHPathName.StripExtension();
        
        // create an object first
        nPhyMaterial* gmaterial( static_cast<nPhyMaterial*>(
            nKernelServer::Instance()->New( "nphymaterial", NOHPathName.Get() )));

        n_assert2( gmaterial, "Failed to createa a Game Material." );

        // load scripting info
        nKernelServer::Instance()->PushCwd( gmaterial );
        nKernelServer::Instance()->Load( files[ index ].Get(), false );
        nKernelServer::Instance()->PopCwd();

//#ifndef NGAME // NOTE!!!!!!: This should uncomented b4 compiling the real last definitive game
        if( !strcmp( gmaterial->GetName(),DefaultMaterialName ) )
        {
            DefaultMaterial = gmaterial->Id();
        }
//#endif //NGAME

    }

    
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Removes a physics material.

    @param material a physics material

    history:
        - 02-Nov-2005   David Reyes    created
*/
void nPhysicsServer::RemoveMaterial( nPhyMaterial* material )
{
    n_assert2( material, "Missing physics material." );

    if( !strcmp( material->GetName(),DefaultMaterialName ) )
    {
        n_assert2_always( "The default material can't be removed." );
        return;
    }

    material->SetToBeRemoved( true );
}

//-----------------------------------------------------------------------------
/**
    Check a game material it's not longer used.

    @param idgamematerial game id material

    @return true/false

    history:
        - 03-Nov-2005   David Reyes    created
*/
const bool nPhysicsServer::ItsGameMaterialUsed( uint idgamematerial ) const
{
    for( int index(0); index < this->GetWorldsContainer().Size(); ++index )
    {
        nPhysicsWorld* world( this->GetWorldsContainer().GetElementAt( index ) );

        n_assert2( world, "Data corruption." );

        for( int indexObject(0); indexObject < world->GetNumPhysicObjs(); ++indexObject )
        {
            ncPhysicsObj* object( world->GetPhysicObj( indexObject ) );

            n_assert2( object, "Data corruption." );

            for( int indexGeometry(0); indexGeometry < object->GetNumGeometries(); ++indexGeometry )
            {
                nPhysicsGeom* geometry(object->GetGeometry( indexGeometry ));

                if( geometry->GetGameMaterialId() == idgamematerial )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

#endif

//-----------------------------------------------------------------------------
/**
    Helper function to find collision with a ray.

    @param start ray's start point
    @param direction ray's direction
    @param ray's lenght
    @param numContacts max desired contacts reported
    @param contact contact information report if any
    @param collidesWith flags to specify with with objects collides

    @return how many contacts reported.

    history:
        - 03-Nov-2005   David Reyes    created
*/
int nPhysicsServer::Collide( const vector3& start, const vector3& direction, const phyreal lenght, 
    int numContacts, nPhyCollide::nContact* contact, int collidesWith )
{
    staticRay->SetPosition( start );
    staticRay->SetDirection( direction );
    staticRay->SetLength( lenght );
    staticRay->SetCollidesWith( collidesWith );
    return nPhysicsServer::Instance()->Collide( staticRay, numContacts, contact );
}


//-----------------------------------------------------------------------------
/**
    Registers the impact call-back.

    @param report call-back function

    history:
        - 14-Feb-2006   David Reyes    created
*/
void nPhysicsServer::RegisterImpactCallBack( impactsReporting report )
{
    this->iReporting = report;
}

//-----------------------------------------------------------------------------
/**
    Returns the impact call-back function.

    @return call-back function

    history:
        - 14-Feb-2006   David Reyes    created
*/
nPhysicsServer::impactsReporting nPhysicsServer::GetImpactCallBack() const
{
    return this->iReporting;
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
