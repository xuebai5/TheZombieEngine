#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatiallightenv.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "entity/nentityobjectserver.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"


#include "nscene/ncscenelight.h"
#include "nscene/nabstractshadernode.h"
#include "zombieentity/ncdictionary.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialLightEnv, ncSpatialLight);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialLightEnv)
    NSCRIPT_ADDCMD_COMPOBJECT('MSSI', void, SetSpaceEntityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSSE', void, SetSpaceEntity, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGSE', nEntityObject *, GetSpaceEntity, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSLI', void, SetLightEntityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSLE', void, SetLightEntity, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLE', nEntityObject *, GetLightEntity, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSpatialLightEnv::ncSpatialLightEnv() : 
    spaceEntityId(0),
    lightEntityId(0)
{
    this->SetAffectsOnlyACell(false);
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialLightEnv::~ncSpatialLightEnv()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLightEnv::InitInstance(nObject::InitInstanceMsg initType)
{
    ncSpatialLight::InitInstance(initType);

    if (initType != nObject::NewInstance)
    {
        // retrieve space and light references (assuming they're already loaded)
        this->refSpaceEntity = nEntityObjectServer::Instance()->GetEntityObject(this->spaceEntityId);
        this->refLightEntity = nEntityObjectServer::Instance()->GetEntityObject(this->lightEntityId);
    }
}

//------------------------------------------------------------------------------
/**
    This is to get access to the space and light, and associate to every block
    or cell in the target space the corresponding lightmap attributes.

    for ( all blocks/cells in the target space ) :
    - get the scene component
    - set the texture override for the terrain cell
*/
bool
ncSpatialLightEnv::Load()
{
    // @todo ma.garcias
    // this behavior could be moved to the scene component instead 
    // to set the texture overrides for every block in the terrain
    if (!this->refSpaceEntity.isvalid())
    {
        return false;
    }

    // HACK- set lightenv object as a local variable in the terrain entity
    // this allows terrain lightmap cache to dynamically assign textures to light nodes
    // and the grass renderer access the sunlight and terrain lightmaps.
    ncDictionary* varContext = this->refSpaceEntity->GetComponentSafe<ncDictionary>();
    nVariable::Handle lightEnvVarHandle = nVariableServer::Instance()->GetVariableHandleByName("outlight");
    varContext->AddLocalVar(nVariable(lightEnvVarHandle, this->GetEntityObject()));

#ifndef __ZOMBIE_EXPORTER__
    ncTerrainMaterialClass* terrainMtlClass = this->refSpaceEntity->GetClassComponentSafe<ncTerrainMaterialClass>();
    terrainMtlClass->SetLightMap( this->GetEntityObject() );
#endif

    #if 0
    // copy shader overrides from light nodes to terrain cells
    // except textures, that will be handled by the texture cache
    ncSceneLight* sceneLight = this->GetComponent<ncSceneLight>();
    if (sceneLight)
    {
        ncTerrainGMM* terrainGMM = this->refSpaceEntity->GetComponentSafe<ncTerrainGMM>();
        if (terrainGMM)
        {
            ncTerrainGMMCell* firstCell = terrainGMM->GetNorthWestCell();
            ncTerrainGMMClass *terrainClass = this->refSpaceEntity->GetClassComponentSafe<ncTerrainGMMClass>();
            
            int numBlocks = terrainClass->GetNumBlocks();
            for (int bz = 0; bz < numBlocks; bz++)
            {
                ncTerrainGMMCell *currentCell = firstCell;
                for (int bx = 0; bx < numBlocks; bx++)
                {
                    n_assert(currentCell);
                    ncScene* sceneComp = currentCell->GetComponent<ncScene>();
                    nShaderParams& shaderOverrides = sceneComp->GetShaderOverrides();
                    
                    // copy shader overrides from the cell node into the cell entity overrides
                    ncSpatialCell* spatialCell = currentCell->GetComponentSafe<ncSpatialCell>();
                    int cellId = spatialCell->GetId();
                    nSceneNode* lightNode = this->GetComponentSafe<ncSceneLight>()->GetLightByCellId(cellId);
                    if (lightNode)
                    {
                        n_assert(lightNode->IsA("nabstractshadernode"));
                        nShaderParams& from = static_cast<nAbstractShaderNode*>(lightNode)->GetShaderParams();
                        int index;
                        for (index = 0; index < from.GetNumValidParams(); ++index)
                        {
                            shaderOverrides.SetArg(from.GetParamByIndex(index), from.GetArgByIndex(index));
                        }
                        #ifndef NGAME
                        // turn off dynamic lighting for lightmapped terrains
                        shaderOverrides.SetArg(nShaderState::matDiffuse, nShaderArg(vector4(0.0f, 0.0f, 0.0f, 0.0f)));
                        currentCell->SetPassEnabled("lght", false);
                        #endif
                    }
                    
                    currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
                }
                firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
            }
        }
    }
    #endif

    ///@todo same for indoor lightmap sets?

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLightEnv::Unload()
{
    //clear all overriden shader parameters in terrain blocks
    if (this->refSpaceEntity.isvalid())
    {
#ifndef __ZOMBIE_EXPORTER__
        //HACK- invalidate all lightmaps when the outlight object is destroyed
        ncTerrainMaterialClass* terrainMtlClass = this->refSpaceEntity->GetClassComponentSafe<ncTerrainMaterialClass>();
        terrainMtlClass->SetLightMap(0);
#endif
        #if 0
        ncTerrainGMM* terrainGMM = this->refSpaceEntity->GetComponentSafe<ncTerrainGMM>();
        if (terrainGMM)
        {
            ncTerrainGMMCell* firstCell = terrainGMM->GetNorthWestCell();
            ncTerrainGMMClass *terrainClass = this->refSpaceEntity->GetClassComponentSafe<ncTerrainGMMClass>();
            
            int numBlocks = terrainClass->GetNumBlocks();
            for (int bz = 0; bz < numBlocks; bz++)
            {
                ncTerrainGMMCell *currentCell = firstCell;
                for (int bx = 0; bx < numBlocks; bx++)
                {
                    n_assert(currentCell);
                    ncScene* sceneComp = currentCell->GetComponent<ncScene>();
                    nShaderParams& shaderOverrides = sceneComp->GetShaderOverrides();
                    // clear shader overrides from the cell node into the cell entity overrides
                    ncSpatialCell* spatialCell = currentCell->GetComponentSafe<ncSpatialCell>();
                    int cellId = spatialCell->GetId();
                    nSceneNode* lightNode = this->GetComponentSafe<ncSceneLight>()->GetLightByCellId(cellId);
                    if (lightNode)
                    {
                        n_assert(lightNode->IsA("nabstractshadernode"));
                        nShaderParams& from = static_cast<nAbstractShaderNode*>(lightNode)->GetShaderParams();
                        int index;
                        for (index = 0; index < from.GetNumValidParams(); ++index)
                        {
                            shaderOverrides.ClearArg(from.GetParamByIndex(index));
                        }
                        // turn on dynamic lighting when releasing terrain lightmap
                        shaderOverrides.ClearArg(nShaderState::matDiffuse);
                        currentCell->SetPassEnabled("lght", true);
                    }
                    
                    currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
                }
                firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
            }
        }
        #endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLightEnv::Accept(nVisibleFrustumVisitor &visitor)
{
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialLightEnv::SaveCmds(nPersistServer * ps)
{
    if (ncSpatialLight::SaveCmds(ps))
    {
        // --- setspaceentity ---
        if (this->refSpaceEntity.isvalid())
        {
            ps->Put(this->GetEntityObject(), 'MSSI', this->refSpaceEntity->GetId());
        }

        // --- setlightentity ---
        if (this->refLightEntity.isvalid())
        {
            ps->Put(this->GetEntityObject(), 'MSLI', this->refLightEntity->GetId());
        }

        return true;
    }

    return false;
}
