#ifndef N_INGUI_TOOL_GENWAYPOINT_H
#define N_INGUI_TOOL_GENWAYPOINT_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolgenwaypoint.h
    @class nInguiToolGenWaypoint
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for placing generic waypoint paths (neWaypoint)

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolplacer.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;
class nObjectEditorState;

//------------------------------------------------------------------------------
class nInguiToolGenWaypoint : public nInguiToolPlacer
{
public:

    // constructor
    nInguiToolGenWaypoint();

    // destructor
    virtual ~nInguiToolGenWaypoint();

    /// Apply the tool
    virtual bool Apply( nTime dt );

    void SetPathName(const char*);
    const char* GetPathName ();

protected:
    nString pathName;

private:
};
//------------------------------------------------------------------------------
#endif
