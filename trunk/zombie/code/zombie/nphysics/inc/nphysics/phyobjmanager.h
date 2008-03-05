#ifndef PHYOBJMANAGER_H
#define PHYOBJMANAGER_H

//-----------------------------------------------------------------------------
/**
    @class phyObjManager
    @ingroup NebulaPhysicsSystem
    @brief Manages the objects in a physics world.

    (C) 2005 Conjurer Services, S.A.
*/
//-----------------------------------------------------------------------------

#include "nphysics/nphysicsconfig.h"

//-----------------------------------------------------------------------------
class nPhysicsWorld;
class neOutdoor;
class ncPhyIndoor;
class ncPhyTerrain;

//-----------------------------------------------------------------------------

class phyObjManager 
{
public:
    // constructor
    explicit phyObjManager( nPhysicsWorld* world );

    // destructor
    ~phyObjManager();

    // initializes the manager
    bool Init();

    // runs the object manager
    bool Run();

    // restores the world collision
    void Restore();

    // inserts an object to the static spaces
    void Insert( ncPhysicsObj* obj );

#ifndef NGAME
    // Draws a map of areas enabled/disabled.
    void DrawAreas( nGfxServer2* server );
#endif

    /// resets the state
    void Reset();

    /// sets the visible cell
    void SetVisibleCell( const unsigned index, const bool dynamic );

private:
    // default constructor (not allowed)
    phyObjManager();

    /// Manage physics areas
    void ManagePhysicsAreas();

    /// updates dynamic objects spaces
    void UpdateDynamicObjects();

    /// Processes a hot point
    void ProcessRadialHotPoint( const vector3& hotpoint );

    /// stores the world to be processed
    nPhysicsWorld* managedWorld;

    /// stores a reference to the only game's outdoor
    ncPhyTerrain* outdoor;

    /// precomputed cell-size
    phyreal cellSize;

    /// precomputed diagonal cell
    phyreal cellDiagonalSquared;

    /// precomputed cell per side
    int cellsPerSide;

    /// precomputed cell per side minus one
    int cellsPerSideMinusOne;

    /// cell's length in integer
    int cellsLengthInteger;

    /// distance between points
    phyreal pointDistance;

    /// shared by all managers
    static const vector2 direction[8];

    /// shared by all managers
    static const int directionIndexes[8][2];

    /// stores all the indoors in the level
    nArray<ncPhyIndoor*> indoors;

    /// number of initial worlds space
    static const int NumInitialIndoors = 1;

    /// growth pace
    static const int NumGrowthPhysicsObj = 1;

    /// distance to activate an indoor (should be maximun travel by an object from frame to frame)
    static const phyreal DistanceActivateIndoor;

#ifndef NGAME
    /// drawing areas pixel/ratio
    phyreal pixelRatio;
#endif
    enum {
        DynamicSpace = 1 << 0,
        StaticSpace = 1 << 1,
    };

    /// sectors bitmap
    char *bitmap;
    char *oldbitmap;

    /// updates the bitmap information
    void UpdateBitmapInformation();

    /// update bitmap cell
    void UpdateBitmapCell( const int cellx, const int celly );
};

#endif