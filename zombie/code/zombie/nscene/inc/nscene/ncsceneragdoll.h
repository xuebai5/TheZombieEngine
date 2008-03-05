#ifndef NC_SCENERAGDOLL_H
#define NC_SCENERAGDOLL_H
//------------------------------------------------------------------------------
/**
    @class ncSceneRagdoll
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for character entities with ragdoll.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscenelod.h"

//------------------------------------------------------------------------------
class ncSceneRagdoll : public ncSceneLod
{

    NCOMPONENT_DECLARE(ncSceneRagdoll,ncSceneLod);

public:
    /// constructor
    ncSceneRagdoll();
    /// destructor
    virtual ~ncSceneRagdoll();
    /// component initialization
    virtual void InitInstance(nObject::InitInstanceMsg initType);
    /// load the graphic object
    virtual bool Load();
    /// unload the graphic object
    virtual void Unload();
    /// check if the graphic object is valid
    virtual bool IsValid();
    /// render the object
    virtual void Render(nSceneGraph *sceneGraph);

protected:
    enum
    {
        MaxRagdolls = 8,
    };
    /// correspondence between ragdoll levels and LOD levels
    nKeyArray<int> ragdollLevels;
    ncCharacter* characterComp;

};

//------------------------------------------------------------------------------
#endif
