#ifndef N_PHYSICSSERVER_H
#define N_PHYSICSSERVER_H
//-----------------------------------------------------------------------------
/**
    @class nPhysicsServer
    @ingroup NebulaPhysicsSystem
    @brief The physics server takes care of creating and managing the physics world

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Server
    
    @cppclass nPhysicsServer
    
    @superclass nRoot

    @classinfo The physics server takes care of creating and managing the physics world.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "kernel/nfileserver2.h"
#include "nphysics/nphysicsconfig.h"
#include "nphysics/nphycollide.h"
#include "entity/nentity.h"
#include "nphysics/nphysicsworld.h"
#include "nphysics/nphymaterial.h"

#ifndef NGAME
#include "kernel/nprofiler.h"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

class nPhysicsGeom;
class nScriptServer;
class nPhysicsWorld;
class nEntityObject;
class nPhyGeomRay;

#ifndef NGAME
    class ncPhyPickableObj;
    class nPhyPickObjManager;
    class nGfxServer2;
#endif  // !NGAME

//-----------------------------------------------------------------------------
class nPhysicsServer : public nRoot 
{
public:
    /// type of container
    typedef nKeyArray< nPhysicsWorld* > tContainerWorlds;

    /// type of call-back for impacts
    typedef void (*impactsReporting)(ncPhysicsObj*,ncPhysicsObj*,const nPhyCollide::nContact*);

    /// registers the impact call-back
    void RegisterImpactCallBack( impactsReporting report );

    /// returns the impact call-back function
    impactsReporting GetImpactCallBack() const;

    /// get instance pointer
    static nPhysicsServer* Instance();

    /// constructor
    nPhysicsServer();

    /// destructor
    ~nPhysicsServer();

    /// runs the physics server
    void Run( phyreal frames );

    /// returns where the physics objects exists
    nObject* GetPhysicsObjects() const;

    /// returns the file server
    nFileServer2* GetFileServer() const;
    
#ifndef NGAME
    /// returns if a geometry it's colliding with any of the others in any world
    int Collide( 
        const nPhysicsGeom *geom, 
        int numContacts, nPhyCollide::nContact* contact, 
        bool collideWithNoBodiesWorld = false );

    /// update objects when the layers activeness it's changed
    void LayersUpdate();
#else
    /// returns if a geometry it's colliding with any of the others in any world
    int Collide( 
        const nPhysicsGeom *geom, 
        int numContacts, nPhyCollide::nContact* contact );

    /// returns if an object it's colliding with any of the others in any world
    int Collide( 
        ncPhysicsObj *object,
        int numContacts, nPhyCollide::nContact* contact );
#endif
    /// removes a world from the container
    void Remove( nPhysicsWorld* world );

#ifndef NGAME
    /// adds a pickeable object
    void Add( ncPhyPickableObj* pickebleObj );

    /// returns if it's drawing alpha shapes
    bool& isDrawingAlphaShapes();

    /// returns the list of alpha shapes
    nArray<nPhysicsGeom*>& ListAlphaShapes();

    /// draw all the geometries
    void Draw( nGfxServer2* server );

    enum {
        phyAABB =       1 << 0,
        phyShapes =     1 << 1,
        phySpaces =     1 << 2,
        phyHeightMap =  1 << 3,
        phyRays =       1 << 4,
        phyTriMesh =    1 << 5,
        phyAreas =      1 << 6,
        phyJoints =     1 << 7,
        phyNormals =    1 << 8,
    };

    /// set what to draw
    void SetDraw( int draw );

    /// returns draw flags
    int GetDraw() const;

#endif // !NGAME

    // NOTE!!!!!!: This should uncomented b4 compiling the real last definitive game
    static nPhyMaterial::idmaterial DefaultMaterial;

    /// inserts a object in a space
    void InsertInSpace( ncPhysicsObj* obj );

    //////////////////////////////////////////////////////////////////////
    // Begin: Speed up functionality
    //////////////////////////////////////////////////////////////////////

    /// returns the world been processed
    static nPhysicsWorld* GetWorldBeenProcessed();

    //////////////////////////////////////////////////////////////////////
    // End: Speed up functionality
    //////////////////////////////////////////////////////////////////////

    /// makes a value proprotional to the frame rate and the engine pace
    static phyreal Proportion( const phyreal value );

    /// returns the speed of the physic server in frames
    static phyreal SpeedServerInFrames();

    /// returns the read-only world container
    const tContainerWorlds& GetWorldsContainer() const;

    /// returns the number of world to be processed
    int GetNumOfWorldsToBeProcessed() const;
    
    /// begin:scripting

    /// adds a world to the container
    void Add(nPhysicsWorld*);

#ifndef NGAME
    /// removes a physics material
    void RemoveMaterial(nPhyMaterial*);
#endif

    /// end:scripting

    /// returns the default world of this physics server
    nPhysicsWorld* GetDefaultWorld() const;

    /// resets the physics worlds
    void Reset();

    /// returns the default's world
    nPhysicsWorld* GetDefaultWorld();

    /// enumerates the physics server state
    typedef enum {
        Normal = 0,
        LoadingIndoor,
        NumStates
    } state;

    /// set physics server state
    void SetState( state newstate );

    /// returns the physics server state
    state GetState() const;

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// INTERNAL, do not call directly (user init instance code)
    void InitInstance(nObject::InitInstanceMsg initType);

    /// loads the materials
    void LoadMaterials();

#ifndef NGAME
    /// check a game material it's not longer used
    const bool ItsGameMaterialUsed( uint idgamematerial ) const;
#endif

    /// helper function to find collision with a ray
    static int Collide( const vector3& start, const vector3& direction, const phyreal lenght, 
        int numContacts, nPhyCollide::nContact* contact, int collidesWith = -1 );

private:
    /// stores the ray for collide helper function 
    static nPhyGeomRay* staticRay;

    /// stores the physics server state
    state psState;

    /// destroys server's data
    void Destroy();

    /// percentage correction for the step size
    static const phyreal stepCorrection;

    /// stores the proportion value
    static phyreal ProportionValue;

    //////////////////////////////////////////////////////////////////////
    // Begin: Containers
    //////////////////////////////////////////////////////////////////////

    /// number of initial worlds space
    static const int NumInitialWorlds = 5;

    /// growth pace
    static const int NumGrowthWorlds = 5;

    /// container of physic worlds
    tContainerWorlds containerWorlds;

    //////////////////////////////////////////////////////////////////////
    // End: Containers
    //////////////////////////////////////////////////////////////////////

    /// geometries space
    nRef<nObject> refPhysicsObjects;

    /// geometry to be used for object picking
    nPhysicsGeom* refGeom;

    /// reference to the fileserver
    nFileServer2* fileServer;

    /// call counter
    phyreal callCounter;

    /// inits the server
    void Init();

    /// sets the fileserver to be used
    void SetFileServer( nFileServer2* fileserver );

    /// stores the world been processed
    static nPhysicsWorld* worldBeenProcessed;

    /// min frames per second expected the server run
    static const phyreal framesPerSecond;

    /// pointer to the unique instance
    static nPhysicsServer* Singleton;
    
    /// stores the default world
    nPhysicsWorld* defaultWorld;

    /// stores the impact reporting call-back
    impactsReporting iReporting;

#ifndef NGAME
    nProfiler profPhysicsServer;
    nProfiler profPhysicsServerOneLoop;
    nProfiler profPhysicsServerCollide;

    nPhyPickObjManager *pickingObjectManager;

    // stores what to draw;
    int draw;

#endif  // !NGAME
};

//-----------------------------------------------------------------------------
/**
    Returns where the physics objects exists

    @return nObject object

    history:
        - 04-Oct-2004   David Reyes    created
        - 01-Jan-2005   David Reyes    inlined
*/
inline
nObject* nPhysicsServer::GetPhysicsObjects() const
{
    return this->refPhysicsObjects.get();
}

//-----------------------------------------------------------------------------
/**
    Returns the world been processed.

    @return world

    history:
        - 27-Oct-2004   David Reyes    created
        - 01-Jan-2005   David Reyes    inlined
*/
inline
nPhysicsWorld* nPhysicsServer::GetWorldBeenProcessed()
{
    return worldBeenProcessed;
}

//-----------------------------------------------------------------------------
/**
    Makes a value proprotional to the frame rate and the engine pace.

    @param value real value
    
    @return real value

    history:
        - 27-Oct-2004   David Reyes    created
        - 01-Jan-2005   David Reyes    inlined
*/
inline
phyreal nPhysicsServer::Proportion( const phyreal value )
{
    return value * ProportionValue;
}

//-----------------------------------------------------------------------------
/**
    Returns the speed of the physic server in frames.

    @return real value

    history:
        - 27-Oct-2004   David Reyes    created
        - 01-Jan-2005   David Reyes    inlined
*/
inline
phyreal nPhysicsServer::SpeedServerInFrames()
{
    return framesPerSecond;
}

//-----------------------------------------------------------------------------
/**
    Adds a world to the container

    @param world    the world to be added to the container

    history:
        - 23-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhysicsServer::Add( nPhysicsWorld* world )
{
    n_assert2( world , "Null pointer" );

    this->containerWorlds.Add( phyWorldIDToInt( world->Id() ), world );
}

//-----------------------------------------------------------------------------
/**
    Removes a world from the container

    @param world    the world to be removed from the container

    history:
        - 23-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhysicsServer::Remove( nPhysicsWorld* world )
{
    n_assert2( world , "Null pointer" );

    this->containerWorlds.Rem( phyWorldIDToInt( world->Id() ) );
}

//-----------------------------------------------------------------------------
/**
    Returns the read-only container.

    @return list of worlds

    history:
        - 10-Feb-2005   Ismael Noche    created
        - 11-May-2005   David Reyes    inlined
*/
inline
const nPhysicsServer::tContainerWorlds& nPhysicsServer::GetWorldsContainer() const
{
    return this->containerWorlds;
}

//-----------------------------------------------------------------------------
/**
    Set physics server state.

    @param newstat server's state

    history:
        - 31-May-2005   David Reyes    created
*/
inline
void nPhysicsServer::SetState( state newstate )
{
    this->psState = newstate;
}

//-----------------------------------------------------------------------------
/**
    Returns the physics server state.

    @return server's state

    history:
        - 31-May-2005   David Reyes    created
*/
inline
nPhysicsServer::state nPhysicsServer::GetState() const
{
    return this->psState;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Set what to draw.

    history:
        - 02-Jun-2005   David Reyes    created
*/
inline
void nPhysicsServer::SetDraw( int drawflags )
{
    this->draw = drawflags;
}

//-----------------------------------------------------------------------------
/**
    Returns draw flags.

    history:
        - 02-Jun-2005   David Reyes    created
*/
inline
int nPhysicsServer::GetDraw() const
{
    return this->draw;
}
#endif

#endif
