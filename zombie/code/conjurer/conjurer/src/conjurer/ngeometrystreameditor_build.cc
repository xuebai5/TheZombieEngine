#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ngeometrystreameditor_build.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ngeometrystreameditor.h"

#include "zombieentity/nloaderserver.h"
#include "nscene/ncscenelodclass.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nshapenode.h"
#include "nscene/nstreamgeometrynode.h"

#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialnode.h"
#include "nmaterial/nmaterialserver.h"

int nGeometryStreamEditor::uniqueId = 0;

//------------------------------------------------------------------------------
/**
    load scene resources and find geometry by the material name.
    as a simple way to allow user identify materials, an nEnv node
    with a string value holding the surface path is created when the
    material is loaded in ncAssetClass::GetNumMaterials(nSceneNode*).
*/
bool
nGeometryStreamEditor::LoadSceneResources()
{
    int totalInstances = 0;

    for (int index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];

        totalInstances += geometry.frequency;

        // load class resources
        geometry.refEntityClass = nEntityClassServer::Instance()->GetEntityClass(geometry.className.Get());
        if (!geometry.refEntityClass.isvalid())
        {
            n_message("Entity class not found: '%s'", geometry.className.Get());
            return false;
        }

        if (!nLoaderServer::Instance()->LoadClassResources(geometry.refEntityClass))
        {
            n_message("Failed to load resources for class: '%s'", geometry.className.Get());
            return false;
        }

        // now check if the geometry reference is valid
        nSceneNode* levelRoot = 0;
        ncSceneLodClass* sceneLodClass = geometry.refEntityClass->GetComponentSafe<ncSceneLodClass>();
        if (sceneLodClass->GetNumLevels() > 0)
        {
            if (geometry.level < sceneLodClass->GetNumLevels())
            {
                //TODO- stream geometries for levels with more than a root node?
                levelRoot = sceneLodClass->GetLevelRoot(geometry.level, 0);
            }
        }
        else
        {
            levelRoot = sceneLodClass->GetRootNode();
        }

        // couldn't find a root node for the level specified
        if (!levelRoot)
        {
            n_message("No graphics for level: %d in class: '%s'", geometry.level, geometry.className.Get());
            return false;
        }

        // find the first geometry in the hierarchy that uses the material
        if (!geometry.materialName.IsEmpty())
        {
            nRoot* materials = kernelServer->Lookup("/usr/materials");
            if (materials)
            {
                nKernelServer::ks->PushCwd(materials);
                nEnv* env = static_cast<nEnv*>(nKernelServer::ks->Lookup(geometry.materialName.Get()));
                if (env)
                {
                    nGeometryNode* geometryNode = this->FindGeometryWithSurface(levelRoot, env->GetS());
                    if (geometryNode)
                    {
                        geometry.refGeometryNode = levelRoot->GetRelPath(geometryNode).Get();
                    }
                }
                nKernelServer::ks->PopCwd();
            }
        }
        
        nKernelServer::ks->PushCwd(levelRoot);
        bool success = geometry.refGeometryNode.isvalid();
        nKernelServer::ks->PopCwd();
        if (!success)
        {
            n_message("No geometry found for material: %s\nin level: %d in class: '%s'", 
                      geometry.materialName.Get(), geometry.level, geometry.className.Get());
            return false;
        }

        //save current stream to revert it later if changes are not committed
        if (!geometry.isPrevStreamValid)
        {
            geometry.prevStream = geometry.refGeometryNode->GetStream();
            geometry.prevStreamIndex = geometry.refGeometryNode->GetStreamIndex();
            geometry.refGeometryNode->SetStream(0);
            geometry.isPrevStreamValid = true;
        }
    }

    // check max number of slots
    if (totalInstances > nStreamGeometryNode::MaxStreamSlots)
    {
        n_message("Max number of slots excceded (72) for stream: %s", this->GetName());
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nGeometryNode*
nGeometryStreamEditor::FindGeometryWithSurface(nSceneNode* sceneNode, const char *surface) const
{
    n_assert(sceneNode);
    n_assert(surface);

    if (sceneNode->IsA("ngeometrynode"))
    {
        // check surface in current node
        nGeometryNode* geometryNode = static_cast<nGeometryNode*>(sceneNode);
        nSurfaceNode* surfaceNode = static_cast<nSurfaceNode*>(kernelServer->Lookup(geometryNode->GetSurface()));
        if (surfaceNode)
        {
            const char *diffMap = surfaceNode->GetTexture(nShaderState::diffMap);
            if (diffMap && !strcmp(diffMap, surface))
            {
                return static_cast<nGeometryNode*>(sceneNode);
            }
        }
    }

    nSceneNode* childNode;
    for (childNode = static_cast<nSceneNode*>(sceneNode->GetHead());
         childNode;
         childNode = static_cast<nSceneNode*>(childNode->GetSucc()))
    {
        // recursively search for the surface in child nodes
        nGeometryNode* geometryNode = this->FindGeometryWithSurface(childNode, surface);
        if (geometryNode)
        {
            return geometryNode;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGeometryStreamEditor::BuildCommonMaterial()
{
    // get rid of previous material, if any
    if (this->refMaterial.isvalid())
    {
        this->refMaterial->Release();
    }

    nString newMaterialPath;
    newMaterialPath.Format("/tmp/streambuilder/mat%04d", uniqueId++);

    // collect common material attributes from the streamed geometries
    int index;
    for (index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];

        const char *surface = geometry.refGeometryNode->GetSurface();
        nSurfaceNode* surfaceNode = static_cast<nSurfaceNode*>(kernelServer->Lookup(surface));
        n_assert(surfaceNode);

        //TODO- stream geometries with plain surfaces?
        n_assert(surfaceNode->IsA("nmaterialnode"));
        nMaterial* material = static_cast<nMaterialNode*>(surfaceNode)->GetMaterialObject();
        n_assert(material);
        if (!this->refMaterial.isvalid())
        {
            //start with an exact copy of the material
            this->refMaterial = static_cast<nMaterial*>(material->Clone(newMaterialPath.Get()));
        }
        else
        {
            //for the rest, remove from the material non-common attributes
            for (int i = 0; i < this->refMaterial->GetNumParams(); ++i)
            {
                const char* name;
                const char* value;
                this->refMaterial->GetParamAt(i, &name, &value);
                if (!material->HasParam(name, value))
                {
                    this->refMaterial->RemoveParam(name, value);
                }
            }
        }
    }

    //finally, add the "instanced" property
    if (this->refMaterial.isvalid())
    {
        if (!this->refMaterial->HasParam("deform", "instanced"))
        {
            this->refMaterial->AddParam("deform", "instanced");
        }

        return true;
    }

    n_message("Could not build a common material for this stream!");

    return false;
}

//------------------------------------------------------------------------------
/**
    This process builds a surface description that encompasses all the
    required textures and parameters for the described material.
    Also, it sets in every geometry what part of this surface is supposed
    to use. (stream index, uv transforms)
*/
bool
nGeometryStreamEditor::BuildCommonSurface()
{
    // get rid of previous surface, if any
    if (this->refSurface.isvalid())
    {
        this->refSurface->Release();
    }

    //build the surface with all the required parameters:
    nString newSurfacePath;
    newSurfacePath.Format("/tmp/streambuilder/sur%04d", uniqueId++);
    this->refSurface = static_cast<nMaterialNode*>(kernelServer->New("nmaterialnode", newSurfacePath.Get()));
    n_assert(this->refSurface.isvalid());
    static_cast<nMaterialNode*>(this->refSurface.get())->SetMaterial(this->refMaterial->GetFullName().Get());

    //get surface description from material
    const nMaterialDb& materialDb = nMaterialServer::Instance()->GetMaterialDb();
    static nArray<nShaderState::Param> shaderParams;
    materialDb.GetShaderParams(this->refMaterial, shaderParams);
    n_assert(shaderParams.Size() > 0);

    for (int index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];

        nGeometryNode* geometryNode = geometry.refGeometryNode.get();
        this->CollectTexturesFromNode(geometryNode);
        const char *surface = geometryNode->GetSurface();
        nSurfaceNode* surfaceNode = static_cast<nSurfaceNode*>(kernelServer->Lookup(surface));
        this->CollectTexturesFromNode(surfaceNode);
    }

    //for all required textures, get the value from the collected list
    static nArray<nString> texNames;
    for (int i = 0; i < shaderParams.Size(); ++i)
    {
        if (this->FindSharedTextures(shaderParams[i], texNames))
        {
            if (texNames.Size() == 1)
            {
                //easiest case: a single texture is already common
                this->refSurface->SetTexture(shaderParams[i], texNames[0].Get());
            }
            else
            {
                //tricky case: build a shared atlas texture
                //TEMP! just to have something visible there:
                switch (shaderParams[i])
                {
                case nShaderState::diffMap:
                case nShaderState::diffMap2:
                    this->refSurface->SetTexture(shaderParams[i], "wc:libs/system/textures/checker.dds");
                    break;
                case nShaderState::bumpMap:
                case nShaderState::BumpMap3:
                    this->refSurface->SetTexture(shaderParams[i], "wc:libs/system/textures/checker_bump.dds");
                    break;
                default:
                    this->refSurface->SetTexture(shaderParams[i], "wc:libs/system/textures/black.dds");
                }
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGeometryStreamEditor::BuildStreamGeometry()
{
    if (this->refStreamGeometry.isvalid())
    {
        //TODO- release from geometry nodes using it as well
        this->refStreamGeometry->Release();
    }
    
    //TODO- depending on each specific type of primitive geometry node,
    //build the stream one way or the other, for now all are single shapes
    
    static nArray<int> shapeNodes;
    shapeNodes.Reset();
    for (int index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];
        if (geometry.refGeometryNode->IsA("nshapenode"))
        {
            shapeNodes.Append(index);
        }
        //other nodes...
    }
    
    //create a new geometry node
    //TODO- for different node types?
    nString newGeometryPath;
    newGeometryPath.Format("/tmp/streambuilder/stream%04d", uniqueId++);
    this->refStreamGeometry = (nGeometryNode*) kernelServer->New("nstreamgeometrynode", newGeometryPath.Get());
    n_assert(this->refStreamGeometry.isvalid());

    if (shapeNodes.Size() > 0)
    {
        // clone all shape nodes to build the stream from them at load time.
        kernelServer->PushCwd(this->refStreamGeometry.get());
        nString shapeName;

        nStreamGeometryNode* stream = static_cast<nStreamGeometryNode*>(this->refStreamGeometry.get());
        stream->SetSurface(this->refSurface->GetFullName().Get());//common surface
        stream->BeginShapes(shapeNodes.Size());

        for (int i = 0; i < shapeNodes.Size(); ++i)
        {
            nGeometryNode* shapeNode = this->geometryArray[shapeNodes[i]].refGeometryNode.get();

            shapeName.Format("shape.%u", i);
            nGeometryNode* cloneShape = static_cast<nGeometryNode*>(shapeNode->Clone(shapeName.Get(), false));
            n_assert(cloneShape);

            // remove surface to prevent the node from drawing
            cloneShape->SetSurface(0);
            stream->SetShapeAt(i, stream->GetRelPath(cloneShape).Get());
            stream->SetFrequencyAt(i, this->geometryArray[shapeNodes[i]].frequency);

            // TODO- set uv transform depending on the atlas distribution
            // ...
            shapeNode->SetStream(this->refStreamGeometry->GetFullName().Get());
            shapeNode->SetStreamIndex(i); //TODO- not with shared materials!
        }
        stream->EndShapes();

        kernelServer->PopCwd();
    }
    //other node types...

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::CollectTexturesFromNode(nAbstractShaderNode* shaderNode)
{
    n_assert(this->refSurface.isvalid());

    // collect all numeric parameters into the target surface
    nShaderParams& params = shaderNode->GetShaderParams();
    for (int index = 0; index < params.GetNumValidParams(); ++index)
    {
        const nShaderArg& shaderArg = params.GetArgByIndex(index);
        if (shaderArg.GetType() != nShaderState::Texture)
        {
            //for numeric parameter, do not perform any merging
            this->refSurface->GetShaderParams().SetArg(params.GetParamByIndex(index), shaderArg);
        }
    }

    // collect textures from all nodes
    for (int tex = 0; tex < shaderNode->GetNumTextures(); ++tex)
    {
        this->AppendSharedTexture(shaderNode->GetTextureParamAt(tex), shaderNode->GetTextureAt(tex));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::AppendSharedTexture(nShaderState::Param texParam, const char* texName)
{
    for (int i = 0; i < this->textureArray.Size(); ++i)
    {
        //add to the array if the parameter is already there
        if (this->textureArray[i].shaderParam == texParam)
        {
            if (!this->textureArray[i].textures.Find(texName))
            {
                this->textureArray[i].textures.Append(texName);
            }
            return;
        }
    }

    // append new texture entry
    TextureEntry& newEntry = this->textureArray.At(this->textureArray.Size());
    newEntry.shaderParam = texParam;
    newEntry.textures.Append(texName);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGeometryStreamEditor::FindSharedTextures(nShaderState::Param texParam, nArray<nString>& texNames) const
{
    texNames.Reset();
    for (int i = 0; i < this->textureArray.Size(); ++i)
    {
        //add to the array if the parameter is already there
        if (this->textureArray[i].shaderParam == texParam)
        {
            texNames = this->textureArray[i].textures;
            return true;
        }
    }
    return false;
}
