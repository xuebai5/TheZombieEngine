#ifndef N_CONJURERDEBUGCOMPSERVER_H
#define N_CONJURERDEBUGCOMPSERVER_H
//------------------------------------------------------------------------------
/**
   @class nConjurerDebugCompServer
   @ingroup Conjurer
   @author MA. Garcias <ma.garcias@yahoo.es>

   @brief Custom server for editor components in Conjurer.

   (C) 2006 Conjurer Services, S.A.
*/
#include "ndebug/ndebugcomponentserver.h"

//------------------------------------------------------------------------------
enum AIFlag
{
    AINavMeshOutdoor        = 1<<0,
    AINavMeshLinks          = 1<<1,
    AIBehavior              = 1<<2,
    AISight                 = 1<<3,
    AIHearing               = 1<<4,
    AIFeeling               = 1<<5,
    AINearCulling           = 1<<6,
    AIMemory                = 1<<7,
    AITarget                = 1<<8,
    AITriggerShapes         = 1<<9,
    AISightRay              = 1<<10,
    AISpawners              = 1<<11,
    AIMotion                = 1<<12,
    AIObstaclesAvoid        = 1<<13,
    AIStaticObstaclesAvoid  = 1<<14,
    AINavMeshIndoors        = 1<<15,
    AINavMeshWireframe      = 1<<16,
    AINavMeshOverlay        = 1<<17,
    AINavMeshOutdoorObstacles = 1<<18,

    AINavMesh = AINavMeshOutdoor | AINavMeshIndoors,
    AIAgents = AIBehavior | AISight | AIHearing | AIFeeling | AINearCulling |
        AIMemory | AITarget | AISightRay | AIMotion | AIObstaclesAvoid |
        AIStaticObstaclesAvoid,
    AIEntities = AIAgents | AITriggerShapes | AISpawners
};

//------------------------------------------------------------------------------
class nConjurerDebugCompServer : public nDebugComponentServer
{
public:
    /// constructor
    nConjurerDebugCompServer();
    /// destructor
    virtual ~nConjurerDebugCompServer();

    /// add an editor component object
    virtual void Add(ncEditor * const object);

    /// remove an editor component object
    virtual void Remove(ncEditor * const object);

    /// draw a trigger shape with default color
    void DrawTriggerShape( nEntityObject* entity );

    /// draw sound sources
    void DrawSoundSources(bool drawRadius);

private:
    /// find and remove from array
    void RemoveFromArray(nArray<nRef<nEntityObject> >& array, nEntityObject* entity);
    /// check if an entity in an is valid and visible
    bool IsValid(const nRef<nEntityObject> *refEntityObject);

    typedef nArray<nRef<nEntityObject> > nRefEntityArray;

    /// draw trigger shape
    void DrawTriggerShapes(nRefEntityArray& array, vector4 color, bool onlySelection);
    /// draw a trigger shape
    void DrawTriggerShape( nEntityObject* entity, vector4 color );
    /// draw sound sources
    void DrawSoundSources(nRefEntityArray& array);

    /// keep separate lists for different debug options:
    nRefEntityArray soundSourcesArray;     //sound sources

    // Object for drawing extruded lines
    nLineDrawer* extrudedLineDrawer;

    /// viewport where this is being drawn
    nAppViewport* appViewport;
};

//------------------------------------------------------------------------------
#endif //N_CONJURERDEBUGCOMPSERVER_H
