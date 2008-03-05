#ifndef N_EDITORDEBUGMODULE_H
#define N_EDITORDEBUGMODULE_H
//------------------------------------------------------------------------------
/**
    @class nEditorDebugModule
    @ingroup NebulaConjurerEditor

*/
#include "ndebug/ndebugmodule.h"

//------------------------------------------------------------------------------
class nEditorDebugModule : public nDebugModule
{
public:
    /// constructor
    nEditorDebugModule();
    /// destructor
    virtual ~nEditorDebugModule();
    /// called when the module is created
    virtual void OnDebugModuleCreate();
    /// called when an option is assigned
    virtual void OnDebugOptionUpdated();

private:
    /// set if terrain lightmaps are on
    void TerrainLightmapsEnabled(bool enabled);
    /// intercept when a level is loaded
    void OnLevelLoaded();

    nRef<nEntityObject> refPrevLightmap;
};

//------------------------------------------------------------------------------
#endif // N_EDITORDEBUGMODULE_H
