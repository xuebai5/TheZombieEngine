#ifndef NC_SCENEINDEXED_H
#define NC_SCENEINDEXED_H
//------------------------------------------------------------------------------
/**
    @class ncScene
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscene.h"

//------------------------------------------------------------------------------
class ncSceneIndexed : public ncScene
{

    NCOMPONENT_DECLARE(ncSceneIndexed, ncScene);

public:
    /// constructor
    ncSceneIndexed();
    /// destructor
    ~ncSceneIndexed();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);

    /// load
    virtual bool Load();

    /// set subscene path
    void SetSubscenePath(const char *);
    /// get subscene path
    const char * GetSubscenePath();

private:
    /// subscene's path
    nDynAutoRef<nSceneNode> refSubScene;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneIndexed::SetSubscenePath(const char * subScenePath)
{
    this->refSubScene = subScenePath;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
ncSceneIndexed::GetSubscenePath()
{
    return this->refSubScene.getname();
}

//------------------------------------------------------------------------------
#endif
