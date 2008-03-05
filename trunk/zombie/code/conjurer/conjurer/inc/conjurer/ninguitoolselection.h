#ifndef N_NINGUI_TOOL_SELECTION_H
#define N_NINGUI_TOOL_SELECTION_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolselection.h
    @class nInguiToolSelection
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class that implements selection of entities

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "conjurer/ninguitoolphypick.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gfx2/nlineserver.h"

class nPhyGeomTriMesh;
class nAppViewport;
class nInguiToolWaypoint;

//------------------------------------------------------------------------------
class nInguiToolSelection: public nInguiToolPhyPick
{
public:

    enum SelectionOp {
        NoSelect,
        Select,
        AddSelection,
        SubSelection
    };

    // Constructor
    nInguiToolSelection();

    // Destructor
    virtual ~nInguiToolSelection();

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the visual elements of the tool
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    /// Draw a box around an entity
    void DrawEntityBB(nEntityObject* entity, vector4 col);

    /// Draw the selection entities bounding boxes
    void DrawSelectionBB();

    /// Draw labels for selected items
    void DrawSelectionLabels();

    /// Set time to end displaying labels for selected items
    void SetTimeToStopDrawingLabels( nTime time );

    bool GetLockedMode ();
    void SetLockedMode(bool);

    /// Type of selection op: set, add or sub
    SelectionOp selectionOp;

    /// Get selected entity whose waypoints are being edited (only in waypoint mode)
    nEntityObject* GetEntityWPEdit();

    /// Return true if I can pick the given entity object
    virtual bool CanPickEntityObject(nEntityObject* entityObject);


protected:
private:

    // First ray coords
    vector3 firstRay;

    // Max distance for rectangle selection
    float rectRange;

    // First selection position in screen
    vector2 firstMousePos;

    // Geometry for rectangle selection
    nRef<nPhysicsGeom> refGeom;

    // Vertex buffer for the geomtrimesh (has one more vertex for line drawing)
    phyreal vertexBuf[6][3];

    // Camera used for selection
    nCamera2 selectionCamera;

    // Spatial camera used for selection
    nRef<nEntityObject> refSpatialCamera;

    // Flag for drawing selection rectangle
    bool drawRect;

    // Flag of locked mode ( transform tools won't switch to selection when picking outside of the axis)
    bool lockedMode;

    // Initial selection
    nArray<nRefEntityObject> initialSelection;

    // Line handler for drawing the grid
    nLineHandler lineHandler;
    
    // End time for drawing labels
    nTime timeToStopDrawingLabels;

};

//------------------------------------------------------------------------------
#endif

