#ifndef N_NINGUI_TOOL_PLACER_H
#define N_NINGUI_TOOL_PLACER_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolplacer.h
    @class nInguiToolPlacer
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Tool for placing entities

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/ntimeserver.h"
#include "ngeomipmap/ncterraingmmclass.h"

//------------------------------------------------------------------------------
class nFloatMap;
class nAppViewport;

//------------------------------------------------------------------------------
class nInguiToolPlacer: public nInguiToolPhyPick
{
    public:

    // Constructor
    nInguiToolPlacer();

    // Destructor
    virtual ~nInguiToolPlacer();
    
    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    void SetInstanceClass(nString);
    nString GetInstanceClass ();
    void SetRandomRotation(bool);
    bool GetRandomRotation();
    void SetSizeVariation(float);
    float GetSizeVariation();

    /// Set outdoor object needed for collision heightmap
    void SetOutdoor( nEntityObject * object );

    /// Access to class names list
    void SetClassList( nStringList* );

    /// Access to entities id list created from first frame
    nArray<nEntityObjectId>* GetEntityList();

protected:

    /// List of entity classes to instance
    nRef<nStringList> classList;

    /// Array of entity instances placed, for undo
    nArray<nEntityObjectId> instanceArray;

    /// Flag for physic picking
    bool doPhysicPicking;

    /// Outdoor object
    nRef<nEntityObject> outdoor;
    /// Collision heightmap
    nRef<nFloatMap> refCollisionHeightmap;

    /// Object random rotation flag
    bool randomRotation;

    // Object size variation
    float sizeVariation;

private:

};

#endif
//------------------------------------------------------------------------------
