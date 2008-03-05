#ifndef N_SCENEDEBUGMODULE_H
#define N_SCENEDEBUGMODULE_H
//------------------------------------------------------------------------------
/**
    @class nSceneDebugModule
    @ingroup NebulaConjurerEditor

*/
#include "ndebug/ndebugmodule.h"

//------------------------------------------------------------------------------
class nSceneDebugModule : public nDebugModule
{
public:
    /// constructor
    nSceneDebugModule();
    /// destructor
    virtual ~nSceneDebugModule();
    /// called when the module is created
    virtual void OnDebugModuleCreate();
    /// called when an option is assigned
    virtual void OnDebugOptionUpdated();
};

//------------------------------------------------------------------------------
#endif // N_SCENEDEBUGMODULE_H
