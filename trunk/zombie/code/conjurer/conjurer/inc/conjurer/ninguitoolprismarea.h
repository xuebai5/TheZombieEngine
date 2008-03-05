#ifndef N_INGUI_TOOL_PRISM_H
#define N_INGUI_TOOL_PRISM_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolprism.h
    @class nInguiToolPrismArea
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool to create polygonal trigger areas

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "kernel/ncmdprotonativecpp.h"
#include "tools/nlinedrawer.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nAppViewport;

//------------------------------------------------------------------------------
class nInguiToolPrismArea: public nInguiToolPhyPick
{
    public:

    /// constructor
    nInguiToolPrismArea();
    /// destructor
    virtual ~nInguiToolPrismArea();

    /// brief Handle input in a viewport.
    bool HandleInput( nAppViewport* vp );

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Finish line, create trigger
    void FinishLine();
	
    /// Cancel the trigger beign created
    void CancelLine();

    /// set state
    virtual void SetState( int s );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

	/// Called when tool is selected
	virtual void OnSelected();

    void SetHeight(float);
    float GetHeight ();

protected:

	/// Select a vertex
	int SelectVertex(vector3 & pos) const;

	/// Create trigger
	nEntityObject * CreateTrigger();

	/// Set trigger position
	void SetTriggerPosition();

    /// return true if I can be used to edit the object passed
    bool CanEditEntityObject(nEntityObject* anObject);

	/// Points created
    nArray<vector3> line;

    /// Line drawer
    nLineDrawer lineDrawer;

    /// Points size for editing
    nArray<float> vertexSizes;

    /// the ancestor class for any triggers created
    nString triggerClassTypeName;

    /// the class name for any triggers created
    nString triggerClassName;

private:

    /// Height of the prism
    float prismHeight;

    /// position of trigger
    bool triggerPosSet;
    vector3 triggerPos;

	/// selected vertex (always the last one if creating the area)
	int selectedVertex;

};

//------------------------------------------------------------------------------
#endif
