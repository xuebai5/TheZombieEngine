#include "precompiled/pchnmaterial.h"
//--------------------------------------------------
//  nmaterialserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//--------------------------------------------------
#include "nscene/nscenenode.h"
#include "nscene/nshadertree.h"
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nmaterialbuilder.h"
#include "nmaterial/nmaterialnode.h"
#include "variable/nvariableserver.h"
#include "file/nmemfile.h"
#include "kernel/nlogclass.h"

nNebulaScriptClass(nMaterialServer, "nroot");

nMaterialServer* nMaterialServer::Singleton = 0;

int nMaterialServer::uniqueMaterialId = 0;
int nMaterialServer::uniqueSurfaceId = 0;

NCREATELOGLEVEL(material, "Material Database", false, 1)
// LOG levels:
// 0- simple log for loaded/reused/shared surfaces and materials
// 1- check for similar surfaces

//------------------------------------------------------------------------------
/**
*/
nMaterialServer::nMaterialServer() :
    isOpen(false),
    isLibraryDirty(false),
    isSavingLibrary(false),
    isLoadingLibrary(false),
    surfaceReportFile(0)
    #ifndef NGAME
   ,debugMaterialEnabled(false),
    debugSurfaceEnabled(false),
    debugMaterialFile("wc:libs/system/materials/debug_material.n2"),
    debugSurfaceFile("wc:libs/system/materials/debug_surface.n2")
    #endif
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->refShaders = kernelServer->New("nroot", "/sys/share/materials");
}

//------------------------------------------------------------------------------
/**
*/
nMaterialServer::~nMaterialServer()
{
    this->refShaders->Release();

    this->ClearSurfaceReport();

    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::Open()
{
    n_assert(!this->isOpen);

    // parse renderpath XML file
    if (this->materialDb.OpenXml())
    {
        nFileServer2* fileServer = nFileServer2::Instance();

        // initialize the shaders assign from the render path
        fileServer->SetAssign("materials", this->materialDb.GetShaderPath().Get());

        // initialize the render path object
        n_verify(this->materialDb.Open());

        // unload the XML doc
        this->materialDb.CloseXml();

        this->isOpen = true;
    }
    else
    {
        n_error("nMaterialServer could not open database file '%s'!", this->materialDb.GetFilename().Get());
    }

#ifndef __ZOMBIE_EXPORTER__ 
    // load material cache if it exists
    this->LoadMaterialLibrary();
#endif

    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the material server.
*/
void
nMaterialServer::Close()
{
    n_assert(this->isOpen);

    this->materialDb.Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Unload and release all owned material shaders.
*/
void
nMaterialServer::Clear()
{
    this->refShaders->Release();
    this->refShaders = kernelServer->New("nroot", "/sys/share/materials");
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialServer::SetBuilder(const char *path)
{
    n_assert(path);
    this->builderName = path;
    this->refMaterialBuilder = path;
}

//------------------------------------------------------------------------------
/**
*/
const char *
nMaterialServer::GetBuilder()
{
    return this->builderName.Get();
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialServer::SetDatabase(const char *filename)
{
    n_assert(filename);
    n_assert(!this->materialDb.IsOpen());
    this->materialDb.SetFilename(filename);
}

//------------------------------------------------------------------------------
/**
*/
const char *
nMaterialServer::GetDatabase()
{
    return this->materialDb.GetFilename().Get();
}

//------------------------------------------------------------------------------
/**
    Load material from file, compare with materials currently loaded.
    Keep an entry for the material and the parent object (asset root),
    to know which file to set as dependency if the material is used
    in an asset created from inside the editor.
    TODO- and it can be re-persisted in the same place it was loaded from.
*/
nMaterial*
nMaterialServer::LoadMaterialFromFile(const char *filename)
{
    //1- if there is an entry for the file, return the material directly
    MaterialEntry* materialEntry = filename ? this->materialNameMap[filename] : 0;
    if (materialEntry)
    {
        if (materialEntry->refMaterial.isvalid())
        {
            NLOG(material, (0, "shared material file: '%s' path: '%s'", filename, materialEntry->refMaterial->GetFullName()));
            materialEntry->refMaterial->AddRef();
            return materialEntry->refMaterial.get();
        }
    }

    //2- if there is no entry, create one, and load the material w/ unique path
    nString materialPath;
    do
    {
        //check for unique material entry
        materialPath.Format("/lib/materials/mat%04u", uniqueMaterialId++);
    }
    while (kernelServer->Lookup(materialPath.Get()) != 0);

    nMaterial* obj = (nMaterial*) kernelServer->LoadAs(filename, materialPath.Get());

    if (!obj)
    {
        NLOG(resource, (NLOGUSER | 0, "Could not load material from file: %s", filename))
        return 0;
    }

    #ifndef NGAME
    // override materials to debug performance cost of shader changes
    if (this->GetDebugMaterialEnabled() && (obj->GetNumParams() > 0) &&
        !obj->HasParam("mat", "shadowvolume") && !obj->HasParam("pro", "sky"))
    {
        bool skinned = obj->HasParam("deform", "skinned");
        bool hasalpha = obj->HasParam("hasalpha", "true");
        bool lightmapped = obj->HasParam("map", "lightmap");
        obj->Release();

        nMaterial* dbgMaterial = (nMaterial*) kernelServer->LoadAs(this->GetDebugMaterialFile(), materialPath.Get());
        if (skinned)
        {
            dbgMaterial->AddParam("deform", "skinned");
        }
        if (hasalpha)
        {
            dbgMaterial->AddParam("hasalpha", "true");
            dbgMaterial->RemoveParam("map", "clipmap");
        }
        if (lightmapped)
        {
            dbgMaterial->AddParam("map", "lightmap");
        }
        obj = dbgMaterial;
    }
    #endif

    NLOG(material, (0, "loaded material file: '%s' path: '%s'", filename, obj->GetFullName()));

    //3-now lookup for an identical material in the library ("/lib/materials").
    //if there is one, release the new, addref the existing, and return it.
    nMaterial* material = this->FindSameMaterial(obj);
    if (material)
    {
        NLOG(material, (0, "reused material file: '%s' path: '%s'", filename, material->GetFullName()));
        material->AddRef();
        obj->Release();
        obj = material;
    }

    //a new material has been loaded, mark library as dirty
    this->isLibraryDirty = true;

    //insert new resource or reuse slot if it was invalidated
    //eg. if all geometry nodes using the material are released.
    if (materialEntry)
    {
        materialEntry->refMaterial = obj;
    }
    else
    {
        MaterialEntry newEntry;
        newEntry.refMaterial = obj;
        this->materialNameMap.Add(filename, &newEntry);
    }

    return obj;
}

//------------------------------------------------------------------------------
/**
    todo- rewrite to handle editable assets, to re-persist materials
    in the same path they were originally loaded from.
*/
bool
nMaterialServer::FindFileFromMaterial(nMaterial* material, nString& filename)
{
    MaterialEntry* materialEntry;
    this->materialNameMap.Begin();
    this->materialNameMap.Next(filename, materialEntry);
    while (materialEntry)
    {
        if (materialEntry->refMaterial == material)
        {
            return true;
        }
        this->materialNameMap.Next(filename, materialEntry);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    todo- rewrite to handle editable assets, to re-persist materials
    in the same path they were originally loaded from.
*/
nMaterial*
nMaterialServer::FindSameMaterial(nMaterial* material)
{
    // compare materials by parameter set, not by material tree
    int numParams = material->GetNumParams();
    if (numParams == 0)
    {
        return 0;
    }

    const char* paramName;
    const char* paramValue;
    nAutoRef<nRoot> refMaterials("/lib/materials");
    if (refMaterials.isvalid())
    {
        nMaterial* curMaterial;
        for (curMaterial = (nMaterial*) refMaterials->GetHead();
             curMaterial;
             curMaterial = (nMaterial*) curMaterial->GetSucc())
        {
            if (curMaterial == material)
            {
                continue;
                //return 0; to use it as a sentinel
            }
            if (curMaterial->GetNumParams() == numParams)
            {
                bool found = true;
                int i;
                for (i = 0; i < numParams; ++i)
                {
                    material->GetParamAt(i, &paramName, &paramValue);
                    if (!curMaterial->HasParam(paramName, paramValue))
                    {
                        found = false;
                        break;
                    }
                }
                if (found)
                {
                    return curMaterial;
                }
            }

        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nMaterial*
nMaterialServer::NewMaterialFromMaterial(nMaterial* material)
{
    //create a new material by cloning another one
    nString materialPath;
    do
    {
        materialPath.Format("/lib/materials/mat%04u", uniqueMaterialId++);
    }
    while (kernelServer->Lookup(materialPath.Get()));

    nMaterial* obj = (nMaterial*) material->Clone(materialPath.Get());
    this->isLibraryDirty = true;
    return obj;
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode*
nMaterialServer::LoadSurfaceFromFile(const char *filename)
{
    //1- if there is an entry for the file, return the surface directly
    SurfaceEntry* surfaceEntry = filename ? this->surfaceNameMap[filename] : 0;
    if (surfaceEntry && surfaceEntry->refSurface.isvalid())
    {
        NLOG(material, (0, "shared surface file: '%s' path: '%s'", filename, surfaceEntry->refSurface->GetFullName().Get()));
        surfaceEntry->refSurface->AddRef();
        return surfaceEntry->refSurface.get();
    }

    //2- if there is no entry, create one, and load the surface w/ unique path
    nString surfacePath;
    surfacePath.Format("/lib/surfaces/sur%04u", uniqueSurfaceId++);
    nSurfaceNode* obj = (nSurfaceNode*) kernelServer->LoadAs(filename, surfacePath.Get());
    #ifndef NGAME
    // override materials to debug performance cost of shader changes
    if (this->GetDebugSurfaceEnabled())
    {
        surfacePath.Format("/lib/surfaces/sur%04u", uniqueSurfaceId++);
        nSurfaceNode *dbgSurface = (nSurfaceNode*) kernelServer->LoadAs(this->GetDebugSurfaceFile(), surfacePath.Get());
        static nClass* materialNodeClass = kernelServer->FindClass("nmaterialnode");
        if (dbgSurface && dbgSurface->IsA(materialNodeClass) && obj->IsA(materialNodeClass))
        {
            static_cast<nMaterialNode*>(dbgSurface)->SetMaterial(static_cast<nMaterialNode*>(obj)->GetMaterial());
            obj->Release();
            obj = dbgSurface;
        }
    }
    else
    #endif
    if (!obj)
    {
        NLOG(resource, (NLOGUSER | 0, "Could not load surface from file: %s", filename))
        return 0;
    }

    NLOG(material, (0, "loaded surface file: '%s' path: '%s'", filename, obj->GetFullName().Get()));

    //get rid of all exceeding parameters using the internal database
    this->CheckSurfaceParameters(obj);

    //3-now lookup for an identical surface in the library ("/lib/surface")
    //if there is one, release the new, addref the existing, and return it.
    this->currentSurfaceFile = filename;
    nSurfaceNode* surface = this->FindSameSurface(obj);
    if (surface)
    {
        N_IFDEF_NLOG(nString surfaceFile);
        NLOGCOND(material, this->FindFileFromSurface(surface, surfaceFile),
                 (0, "reused surface file: '%s' path: '%s' (file: %s)", filename, surface->GetFullName().Get(), surfaceFile.Get()));
        surface->AddRef();
        obj->Release();
        obj = surface;
    }

    //insert new resource or reuse slot if it was invalidated
    //eg. if all geometry nodes using the material are released.
    if (surfaceEntry)
    {
        surfaceEntry->refSurface = obj;
    }
    else
    {
        SurfaceEntry newEntry;
        newEntry.refSurface = obj;
        this->surfaceNameMap.Add(filename, &newEntry);
    }

    return obj;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialServer::FindFileFromSurface(nSurfaceNode* surface, nString& filename)
{
    SurfaceEntry* surfaceEntry;
    this->surfaceNameMap.Begin();
    this->surfaceNameMap.Next(filename, surfaceEntry);
    while (surfaceEntry)
    {
        if (surfaceEntry->refSurface == surface)
        {
            return true;
        }
        this->surfaceNameMap.Next(filename, surfaceEntry);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode*
nMaterialServer::FindSameSurface(nSurfaceNode* surface)
{
    static nClass* materialNodeClass = kernelServer->FindClass("nmaterialnode");
    nAutoRef<nRoot> refSurfaces("/lib/surfaces");
    if (!refSurfaces.isvalid())
    {
        return 0;
    }
    // reuse only materialnodes- 
    if ( (!surface) || surface->GetClass() != materialNodeClass)
    {
        return 0;
    }

    // compare by material
    const char *material = static_cast<nMaterialNode*>(surface)->GetMaterial();
    if (!material)
    {
        return 0;
    }
    // compare texture set only
    int numTextures = surface->GetNumTextures();
    if (numTextures == 0)
    {
        return 0;
    }

    nSurfaceNode* curSurface;
    for (curSurface = (nSurfaceNode*) refSurfaces->GetHead();
         curSurface;
         curSurface = (nSurfaceNode*) curSurface->GetSucc())
    {
        if (curSurface == surface)
        {
            continue;
            //return 0; to use it as a sentinel
        }
        if (curSurface->GetClass() != materialNodeClass)
        {
            continue;
        }
        //compare material nodes only-
        nMaterialNode* curMaterial = static_cast<nMaterialNode*>(curSurface);
        if (!curMaterial->GetMaterial() || strcmp(curMaterial->GetMaterial(), material))
        {
            //if there are some common textures, at least log the differences
            N_IFDEF_NLOG(nString filename;)
            NLOGCOND(material,
                     this->FindSharedTextures(surface, curSurface) && this->FindFileFromSurface(curSurface, filename),
                     (1, "Found similar surface, different shader: %s", filename.Get()))
            continue;
        }
        if (curSurface->GetNumTextures() == numTextures)
        {
            bool found = true;
            int tex;
            for (tex = 0; tex < numTextures; ++tex)
            {
                nShaderState::Param param = surface->GetTextureParamAt(tex);
                if (!curSurface->GetTexture(param) ||
                    strcmp(surface->GetTextureAt(tex), curSurface->GetTexture(param)))
                {
                    //if there are some common textures, at least log the differences
                    N_IFDEF_NLOG(nString filename;)
                    NLOGCOND(material,
                             this->FindSharedTextures(surface, curSurface) && this->FindFileFromSurface(curSurface, filename),
                             (1, "Found similar surface, same shader: %s", filename.Get()))
                    found = false;
                    break;//to next surface
                }
            }
            if (found)
            {
                //TODO- check any other parameters?
                return curSurface;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Check if two surfaces share any texture(s), if found return true
    and a user-friendly description of the case. Compare textures by path.
    This is used to find which surfaces the same number of textures have
    some textures in common, but can't be completely reused- this makes them
    candidates for creating atlas.
*/
bool
nMaterialServer::FindSharedTextures(nSurfaceNode* surface0, nSurfaceNode* surface1)
{
    bool found = false;

    int numTextures = surface0->GetNumTextures();
    for (int tex = 0; tex < numTextures; ++tex)
    {
        nShaderState::Param texParam = surface0->GetTextureParamAt(tex);
        if (surface1->GetTexture(texParam) &&
            (strcmp(surface0->GetTextureAt(tex), surface1->GetTexture(texParam)) == 0))
        {
            found = true;
            break;
        }
    }

    // save detailed material and surface attributes to memory file if there was a match
    if (found && surface0->GetTexture(nShaderState::diffMap) && surface1->GetTexture(nShaderState::diffMap) &&
        (strcmp(surface0->GetTexture(nShaderState::diffMap), surface1->GetTexture(nShaderState::diffMap)) == 0))
    {
        if (this->surfaceReportFile == 0)
        {
            this->surfaceReportFile = n_new(nMemFile);
            nString emptyFilename;
            this->surfaceReportFile->Open(emptyFilename, "w");
        }

        this->surfaceReportFile->PutS("----------------------------------------\n");
        this->surfaceReportFile->PutS("Comparing with surfaces in files:\n");

        this->surfaceReportFile->PutS(this->currentSurfaceFile.Get());
        this->surfaceReportFile->PutS("\n");

        nString filename;
        if (this->FindFileFromSurface(surface1, filename))
        {
            this->surfaceReportFile->PutS(filename.Get());
            this->surfaceReportFile->PutS("\n");
        }

        //if surfaces dont have the same material, log the different materials
        const char* mat0 = static_cast<nMaterialNode*>(surface0)->GetMaterial();
        const char* mat1 = static_cast<nMaterialNode*>(surface1)->GetMaterial();
        if (!mat0 || !mat1 || strcmp(mat0, mat1))
        {
            nMaterial* material0 = static_cast<nMaterialNode*>(surface0)->GetMaterialObject();
            nMaterial* material1 = static_cast<nMaterialNode*>(surface1)->GetMaterialObject();

            const char* name;
            const char* value;

            this->surfaceReportFile->PutS("Surfaces have different shaders.\n");
            this->surfaceReportFile->PutS("Properties for the first surface:\n");

            for (int i = 0; i < material0->GetNumParams(); ++i)
            {
                material0->GetParamAt(i, &name, &value);

                this->surfaceReportFile->PutS("  ");
                this->surfaceReportFile->PutS(name);
                this->surfaceReportFile->PutS("(");
                this->surfaceReportFile->PutS(value);
                this->surfaceReportFile->PutS(")\n");
            }

            this->surfaceReportFile->PutS("\n");
            this->surfaceReportFile->PutS("Properties for the second surface:\n");

            for (int i = 0; i < material1->GetNumParams(); ++i)
            {
                material1->GetParamAt(i, &name, &value);

                this->surfaceReportFile->PutS("  ");
                this->surfaceReportFile->PutS(name);
                this->surfaceReportFile->PutS("(");
                this->surfaceReportFile->PutS(value);
                this->surfaceReportFile->PutS(")\n");
            }

            this->surfaceReportFile->PutS("\n");
        }

        // save detailed information on both surfaces
        this->surfaceReportFile->PutS("Textures in the first surface:\n");
        for (int tex = 0; tex < surface0->GetNumTextures(); ++tex)
        {
            nShaderState::Param texParam = surface0->GetTextureParamAt(tex);

            this->surfaceReportFile->PutS("  ");
            this->surfaceReportFile->PutS(nShaderState::ParamToString(texParam));
            this->surfaceReportFile->PutS("=");
            this->surfaceReportFile->PutS(surface0->GetTexture(texParam));
            this->surfaceReportFile->PutS("\n");
        }

        this->surfaceReportFile->PutS("Textures in the second surface:\n");
        for (int tex = 0; tex < surface1->GetNumTextures(); ++tex)
        {
            nShaderState::Param texParam = surface1->GetTextureParamAt(tex);

            this->surfaceReportFile->PutS("  ");
            this->surfaceReportFile->PutS(nShaderState::ParamToString(texParam));
            this->surfaceReportFile->PutS("=");
            this->surfaceReportFile->PutS(surface1->GetTexture(texParam));
            this->surfaceReportFile->PutS("\n");
        }
    }

    return found;
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode*
nMaterialServer::NewSurfaceFromSurface(nSurfaceNode* surface)
{
    nString surfacePath;
    surfacePath.Format("/lib/surfaces/sur%04u", uniqueSurfaceId++);
    nSurfaceNode* obj = (nSurfaceNode*) surface->Clone(surfacePath.Get());
    return obj;
}

//------------------------------------------------------------------------------
/**
    Check that all surface parameters are actually required.
*/
void
nMaterialServer::CheckSurfaceParameters(nSurfaceNode* surfaceNode)
{
    n_assert(surfaceNode);
    n_assert(this->isOpen);

    if (surfaceNode->IsA("nmaterialnode"))
    {
        nMaterial* material = static_cast<nMaterialNode*>(surfaceNode)->GetMaterialObject();
        if (material)
        {
            if (material->GetNumParams() > 0)
            {
                const nMaterialDb& materialDb = this->GetMaterialDb();
                static nArray<nShaderState::Param> requiredParams;
                materialDb.GetShaderParams(material, requiredParams);
                if (requiredParams.Size() > 0)
                {
                    //remove any parameters that are not used by current material
                    nShaderParams& shaderParams = surfaceNode->GetShaderParams();
                    for (int index = 0; index < shaderParams.GetNumValidParams(); /*empty*/)
                    {
                        nShaderState::Param curParam = shaderParams.GetParamByIndex(index);
                        if (!requiredParams.Find(curParam))
                        {
                            NLOG(material, (0, "Removed unused parameter: %s", nShaderState::ParamToString(curParam)))
                            shaderParams.ClearParam(curParam);
                        }
                        else
                        {
                            ++index;
                        }
                    }
                    //same for textures
                    for (int tex = 0; tex < surfaceNode->GetNumTextures(); /*empty*/)
                    {
                        nShaderState::Param texParam = surfaceNode->GetTextureParamAt(tex);
                        if (!requiredParams.Find(texParam))
                        {
                            NLOG(material, (0, "Removed unused texture: %s", nShaderState::ParamToString(texParam)))
                            surfaceNode->ClearTexture(texParam);
                        }
                        else
                        {
                            ++tex;
                        }
                    }
                }

                #ifndef NGAME
                // additionally, set default values for any missing parameters:
                for (int index = 0; index < requiredParams.Size(); ++index)
                {
                    nMaterialDb::ShaderParam shaderParam;
                    if (this->materialDb.FindShaderParam(requiredParams[index], shaderParam))
                    {
                        switch (shaderParam.type)
                        {
                        case nShaderState::Texture:
                            if (!surfaceNode->GetTexture(shaderParam.param))
                            {
                                //fill w/ default texture
                                switch (shaderParam.param)
                                {
                                case nShaderState::clipMap:
                                case nShaderState::diffMap:
                                case nShaderState::diffMap2:
                                case nShaderState::controlMap:
                                    surfaceNode->SetTexture(shaderParam.param, "wc:libs/system/textures/white.dds");
                                    break;
                                case nShaderState::bumpMap:
                                case nShaderState::BumpMap3:
                                    surfaceNode->SetTexture(shaderParam.param, "wc:libs/system/textures/nobump.dds");
                                    break;
                                case nShaderState::CubeMap0:
                                    surfaceNode->SetTexture(shaderParam.param, "wc:libs/system/textures/defcube.dds");
                                    break;
                                default:
                                    surfaceNode->SetTexture(shaderParam.param, "wc:libs/system/textures/black.dds");
                                }
                            }
                            break;
                        }
                    }
                }
                #endif /*NGAME*/
            }
            else
            {
                //check for material parameters in all shaders
            }
        }
    }
    else
    {
        //TODO- find shader(s) in scene database to check for used parameters?
    }
}

//------------------------------------------------------------------------------
/**
    Get material by path, looking up starting at the root material.
*/
bool
nMaterialServer::LoadMaterial(nMaterial *material)
{
    return this->refMaterialBuilder->LoadMaterial(material);
}

//------------------------------------------------------------------------------
/**
    Get material shader for a render pass. If the shader already exists, it
    increments its reference counting. If not, it is created and assigned the
    current material builder. The material shader is not loaded, instead it is
    loaded only when the material node requires it.
*/
nShaderTree*
nMaterialServer::NewShaderTree(nMaterial *material, const int level, nFourCC fourcc)
{
    nString materialId;
    char buf[5];
    
    materialId += material->GetName();
    materialId += "_";
    materialId += nVariableServer::FourCCToString(fourcc, buf, sizeof(buf));

    if (level)
    {
        materialId += "_";
        materialId.AppendInt(level);
    }

    nShaderTree* shaderTree = static_cast<nShaderTree*>( kernelServer->New("nshadertree") );
    n_assert( shaderTree );
    shaderTree->SetName( materialId.Get() );

    return shaderTree;
}
