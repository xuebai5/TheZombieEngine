#ifndef N_INGUI_TOOL_WAYPOINT_H
#define N_INGUI_TOOL_WAYPOINT_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolwaypoint.h
    @class nInguiToolWaypoint
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for waypoint creation. It's derived from nInguiToolPlacer and the entity class to instance should be
    set to a conjurer waypoint class (neRnsWaypoint)

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolplacer.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;
class neRnsWaypoint;
class nObjectEditorState;

//------------------------------------------------------------------------------
class nInguiToolWaypoint : public nInguiToolPlacer
{
public:

    // constructor
    nInguiToolWaypoint();

    // destructor
    virtual ~nInguiToolWaypoint();

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

protected:
    bool manualLinking;

private:
};
//------------------------------------------------------------------------------
#endif
