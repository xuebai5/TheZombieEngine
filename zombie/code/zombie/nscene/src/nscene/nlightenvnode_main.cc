#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  nlightenvnode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nlightenvnode.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncscenelightenvclass.h"
#include "nscene/ncscenelight.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nLightEnvNode, "nlightnode");

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nLightEnvNode)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nLightEnvNode::nLightEnvNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLightEnvNode::~nLightEnvNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLightEnvNode::EntityCreated(nEntityObject* entityObject)
{
    n_assert_return(entityObject,);
    ncSceneLight* sceneLight = entityObject->GetComponent<ncSceneLight>();
    sceneLight->SetCastLightmaps(true);
}

//------------------------------------------------------------------------------
/**
*/
void
nLightEnvNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    /// @todo ma.garcias -can't attach all lightmaps, use gmmcache
    // update a list of valid references to the loaded lightmaps
    this->activeCells.Clear();

    ncSceneLightEnvClass* lightEnvClass = entityObject->GetClassComponent<ncSceneLightEnvClass>();

    nEntityObject* spaceEntity = entityObject->GetComponent<ncSpatialLightEnv>()->GetSpaceEntity();
    if (spaceEntity)
    {
        ncTerrainMaterialClass* terrainMaterial = spaceEntity->GetClassComponent<ncTerrainMaterialClass>();
        if (terrainMaterial && terrainMaterial->GetLightMap())
        {
            int numLightmaps = terrainMaterial->GetNumLightmapTextures();
            for (int index = 0; index < numLightmaps; ++index)
            {
                int bx, bz;
                nTexture2* tex = terrainMaterial->GetLightmapTextureAt(index, bx, bz);
                if (tex)
                {
                    nAbstractShaderNode* cellNode = static_cast<nAbstractShaderNode*>(lightEnvClass->GetClassLightByBlock(bx, bz));
                    int cellId = lightEnvClass->GetCellIdByBlock(bx, bz);
                    if (cellNode && cellId != -1)
                    {
                        cellNode->SetTextureOverride(nShaderState::lightMap, tex);
                        this->activeCells.Append(cellId);
                    }
                }
            }
        }
    }

    // if there is any active cell, attach the light node as usual
    if (!this->activeCells.Empty())
    {
        nLightNode::Attach(sceneGraph, entityObject);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nLightEnvNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert_return(entityObject, false);
    if (this->activeCells.Empty())
    {
        return false;
    }

    ncSceneLightEnvClass* lightEnvClass = entityObject->GetClassComponent<ncSceneLightEnvClass>();

    // render the correct child node for the target entity
    nEntityObject* callingEntity = (nEntityObject*) sceneGraph->GetCallingEntity();
    ncSpatial* spatialComp = callingEntity->GetComponent<ncSpatial>();
    n_assert_return(spatialComp, false);
    ncSpatialCell *cell = spatialComp->GetCell();
    n_assert_return(cell, false);
    n_assert_return(cell->GetType() == ncSpatialCell::N_QUADTREE_CELL, false);
    ncSpatialQuadtreeCell *quadCell = static_cast<ncSpatialQuadtreeCell*>(cell);
    if (quadCell->IsLeaf())
    {
        if (this->activeCells.Find(quadCell->GetId()))
        {
            // render the parameters of the child node for the cell
            lightEnvClass->GetClassLightByCellId(quadCell->GetId())->Render(sceneGraph, entityObject);
            return true;
        }
    }
    else
    {
        ncSpatialQuadtree *quadtree = static_cast<ncSpatialQuadtree*>(quadCell->GetParentSpace());
        ncTransform* trComp = spatialComp->GetComponentSafe<ncTransform>();
        ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafContaining(quadCell, trComp->GetPosition());
        if (leafCell && this->activeCells.Find(leafCell->GetId()))
        {
            // render the parameters of the child node for the cell
            lightEnvClass->GetClassLightByCellId(leafCell->GetId())->Render(sceneGraph, entityObject);
            return true;
        }
    }

    return false;
}
