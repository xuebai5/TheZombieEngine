#ifndef N_NINGUITOOLPHYPICK_H
#define N_NINGUITOOLPHYPICK_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolphypick.h
    @class nInguiToolPhyPick
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Abstract tool that does physics picking and can be grid snapped.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/ntimeserver.h"
#include "nphysics/nphygeomray.h"

class nFloatMap;
class nEditorGrid;
class nObjectEditorState;

//------------------------------------------------------------------------------
class nInguiToolPhyPick: public nInguiTool
{
    public:

    // Constructor
    nInguiToolPhyPick();

    // Destructor
    virtual ~nInguiToolPhyPick();

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    void SetGridPath(nString);

    /// Get picked entity
    const nEntityObject* GetPickedEntity();

    /// Get created entity instance (for tools that create entities)
    nEntityObject* GetEntityInstance();

    /// Set created entity instance
    void SetEntityInstance(nEntityObject* instance);

    // reference to object app state
    nRef<nObjectEditorState> refObjState;

    /// Return true if I can pick the given entity object
    virtual bool CanPickEntityObject(nEntityObject* entityObject);

protected:

    /// Picked entity
    nRef<nEntityObject> refLastPickedEntity;

    /// Grid for placing snap
    nDynAutoRef<nEditorGrid> refGrid;

    /// Normal at picking position
    vector3 pickingNormal;

    /// Geometry ray for picking
    nPhyGeomRay* phyRay;

    // Array of intersections to make picking with the physic server
    nArray<nPhyCollide::nContact> contacts;

    struct SortedContact
    {
        int index;
        float t;
        vector3 pos;
        nEntityObject* entity;
    };

    // Array of sorted collisions
    nArray<SortedContact> sortedContacts;

    /// Entity instance (for tools that create entities. for use in subsequent frames)
    nEntityObject* entityInstance;

    /// Tells if picking is done succesively, taking the next picked object to the previously picked
    bool succesivePicking;

    /// Sticky flag, decides if tool is deactivated after single action
    bool isSticky;

};
//------------------------------------------------------------------------------
#endif
