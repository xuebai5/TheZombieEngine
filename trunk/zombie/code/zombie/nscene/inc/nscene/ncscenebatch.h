#ifndef NC_SCENEBATCH_H
#define NC_SCENEBATCH_H
//------------------------------------------------------------------------------
/**
    @class ncSceneBatch
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for batched entities.

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/ncscene.h"

//------------------------------------------------------------------------------
class ncSceneBatch : public ncScene
{

    NCOMPONENT_DECLARE(ncSceneBatch,ncScene);

public:
    /// constructor
    ncSceneBatch();
    /// destructor
    virtual ~ncSceneBatch();
    /// component initialization
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// get valid
    virtual bool IsValid();
    /// load the graphic object
    virtual bool Load();
    /// unload the graphic object
    virtual void Unload();
    /// render the object
    virtual void Render(nSceneGraph *sceneGraph);

    /// set batch resource (to create batches from code)
    void SetBatchRoot(nSceneNode* sceneNode);

private:
    bool isBatchInitialized;

};

//------------------------------------------------------------------------------
#endif //NC_SCENEBATCH_H
