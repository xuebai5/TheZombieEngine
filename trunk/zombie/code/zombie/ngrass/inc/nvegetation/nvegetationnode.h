#ifndef N_VEGETATIONNODE_H
#define N_VEGETATIONNODE_H
//------------------------------------------------------------------------------
/**
    @file nvegetationnode.h
    @class nVegetationNode
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo
    @brief nVegetationNode is scene node used to render a vegetation in a terrain 
    (c) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "kernel/ncmdprotonativecpp.h"

class ncTerrainVegetationClass;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
class nVegetationNode : public nGeometryNode
{
public:
    /// constructor
    nVegetationNode();
    /// destructor
    virtual ~nVegetationNode();
    /// called on initialization
    void InitInstance(nObject::InitInstanceMsg);
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// Preload resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

/*
    /// call on new created entity
    void EntityCreated(nEntityObject* entityObject);

    /// set terrain component 
    void SetTerrainGMM(ncTerrainGMM *);
*/
    /// method called when node attached to the scene
    void Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject);
    /// Pre-instancing geometry call
    bool Apply(nSceneGraph* sceneGraph);    
    /// update internal state and render
    bool Render(nSceneGraph* sceneGraph, nEntityObject *entityObject);
    /// restore on lost device, restore resources
    void RestoreOnLostDevice(void);
    /// Set the vegetation class, this method use only by ncTerrainVegetationClass
    void SetVegetaionClass( ncTerrainVegetationClass* vegClass);

private:
    /// Set Params
    void SetShaderParams( nShader2* shader, nEntityObject* cell);
    nProfiler profRender;
    nProfiler profRenderDraw;

    ncTerrainVegetationClass* vegClass;
    /// Shader params
    nShaderParams shaderParams;

};

//------------------------------------------------------------------------------
#endif
