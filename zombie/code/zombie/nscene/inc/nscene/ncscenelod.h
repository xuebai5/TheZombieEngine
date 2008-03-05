#ifndef NC_SCENELOD_H
#define NC_SCENELOD_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLod
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscene.h"

//------------------------------------------------------------------------------
class ncSceneLod : public ncScene
{

    NCOMPONENT_DECLARE(ncSceneLod,ncScene);

public:
    /// constructor
    ncSceneLod();
    /// destructor
    virtual ~ncSceneLod();
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

    /// set locked level
    void SetLockedLevel(bool,int);
    /// get lockedlevel 
    int GetLockedLevel() const;

protected:
    enum
    {
        MaxLevels = 8,
    };

    /// get number of detail levels initialized
    int GetNumLevels();

    nKeyArray< nArray<nSceneNode*> > levelRootNodes;

    /// actual number of levels used for LOD issues
    int numDetailLevels;
    /// locked level of detail
    int lockedLod;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLod::GetNumLevels()
{
    return this->levelRootNodes.Size();
}

//------------------------------------------------------------------------------
#endif //NC_SCENELOD_H
