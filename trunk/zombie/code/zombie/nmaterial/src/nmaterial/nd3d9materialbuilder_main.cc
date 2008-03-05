#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nd3d9materialbuild_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "file/nmemfile.h"
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nd3d9materialbuilder.h"
#include "nscene/nscenenode.h"
#include "nscene/nshadertree.h"
#include "kernel/nscriptserver.h"
#include "variable/nvariableserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nlogclass.h"

nNebulaScriptClass(nD3D9MaterialBuilder, "nmaterialbuilder");

//------------------------------------------------------------------------------
/**
*/
nD3D9MaterialBuilder::nD3D9MaterialBuilder() :
    refScriptServer("/sys/servers/script"),
    stackDepth(0),
    pathLen(0),
    buildState(None),
    shaderNeedsBuilding(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9MaterialBuilder::~nD3D9MaterialBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9MaterialBuilder::LoadMaterial(nMaterial *material)
{
    this->materialName = material->GetFullName();
    this->refMaterial = material;
    
    if (!this->LoadLevels(material))
    {
        return false;
    }
    n_assert(this->numLevels > 0);
    
    material->ClearLevels();
    for (int level = 0; level < this->numLevels; level++)
    {
        this->level = level;

        if (!this->LoadPasses(material))
        {
            return false;
        }
        
        material->BeginLevel();
        for (int i = 0; i < this->passes.Size(); i++)
        {
            nFourCC fourcc = this->passes[i];
            if (!material->GetShaderTree(level, fourcc))
            {
                nShaderTree* shaderTree = nMaterialServer::Instance()->NewShaderTree(material, level, fourcc);
                n_assert(shaderTree);
                
                if (!this->LoadShader(material, fourcc, shaderTree))
                {
                    return false;
                }

                material->SetShaderTree(level, fourcc, shaderTree);
            }
        }
        material->EndLevel();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Determine the detail levels needed in this implementation.
    Directly access the nMaterial to fill its array of levels.
*/
bool
nD3D9MaterialBuilder::LoadLevels(nMaterial * /*material*/)
{
    this->numLevels = 1;
    this->paramLevels.Clear();
    this->buildState = inBuildShaderLevels;

    if (!this->onBuildShaderLevelsFunction.IsEmpty())
    {
        nString result;
        bool val = this->refScriptServer->RunFunction(this->onBuildShaderLevelsFunction.Get(), result);
        n_assert3( val, ("Failed run function on onBuildShaderLevelsFunction\n%s", result.Get()));
        return val;
    }
    
    return true;
}

//------------------------------------------------------------------------------
/**
    Determine which passes are needed in this implementation.
    Directly access the nMaterial to fill its array of passes.
*/
bool
nD3D9MaterialBuilder::LoadPasses(nMaterial *material)
{
    this->materialName = material->GetFullName();
    this->refMaterial = material;
    this->passes.Clear();

    this->buildState = inBuildShaderPasses;
    nString result;
    bool val = this->refScriptServer->RunFunction(this->onBuildShaderPassesFunction.Get(), result);
    n_assert3(val, ("Failed run function on onBuildShaderPassesFunction\n%s", result.Get()));
    return val;
}

//------------------------------------------------------------------------------
/**
    Add a fixed shader attribute to the current shader description.
*/
void
nD3D9MaterialBuilder::AddAttribute(const char *name, const char *value)
{
    n_assert(this->buildState == inBuildShaderTree);

    ShaderAttr newAttr;
    newAttr.name = name;
    newAttr.value = value;
    this->shaderDesc.Append(newAttr);
}

//------------------------------------------------------------------------------
/**
    Get a fixed attribute from current shader description.
*/
const char *
nD3D9MaterialBuilder::GetAttribute(const char *name)
{
    n_assert(this->buildState == inBuildShader);

    ShaderAttr elm;
    elm.name = name;
    int i = shaderDesc.FindIndex(elm);
    if (i >= 0)
    {
        return shaderDesc[i].value.Get();    
    }
    return "";
}

//------------------------------------------------------------------------------
/**
    create a new case variable and push it into the case stack.
*/
void
nD3D9MaterialBuilder::BeginCaseVar(const char *name)
{
    n_assert(this->buildState == inBuildShaderTree);

    Node newNode;
    newNode.parent = (this->stackDepth > 0) ? this->nodeStack[this->stackDepth - 1] : 0;
    newNode.param = nVariableServer::Instance()->GetVariableHandleByName(name);
    nodeArray.Append(newNode);

    nodeStack[stackDepth] = &(this->nodeArray.Back());
    ++stackDepth;
}

//------------------------------------------------------------------------------
/**
    Add a new case value to the current case.
*/
void
nD3D9MaterialBuilder::AddCaseValue(const char *value)
{
    n_assert(this->buildState == inBuildShaderTree);
    n_assert(this->stackDepth > 0);

    nString newVal = value;
    this->nodeStack[stackDepth - 1]->values.Append(newVal);
}

//------------------------------------------------------------------------------
/**
    Pop a case variable from the stack.
*/
void
nD3D9MaterialBuilder::EndCaseVar()
{
    n_assert(this->buildState == inBuildShaderTree);
    n_assert(this->stackDepth > 0);
    this->stackDepth--;
}

//------------------------------------------------------------------------------
/**
    Get an attribute from current case in the shader tree.
    
    If the name appears more than once in the path to the current node
    (eg. LightType, ProjectedMap) it is retrieved in array form ("LightType[0]")
*/
const char *
nD3D9MaterialBuilder::GetCaseValue(const char *name)
{
    n_assert(this->buildState == inBuildShader);

    nString varName(name);
    int index = 0;
    int pos1 = varName.FindChar('(', 0);
    if (pos1 != -1)
    {
        int pos2 = varName.FindChar(')', pos1);
        if (pos2 != -1 && pos2 > pos1)
        {
            index = static_cast<ushort>(varName.ExtractRange(pos1 + 1, pos2 - pos1 - 1).AsInt());
            varName = varName.ExtractRange(0, pos1);
        }
    }
    nVariable::Handle param = nVariableServer::Instance()->GetVariableHandleByName(varName.Get());
    if (param != nVariable::InvalidHandle)
    {
        int currentIndex = 0;
        int i;
        for (i = 0; i < this->pathLen; i++)
        {
            if (casePath[i].param == param)
            {
                if (currentIndex == index)
                {
                    return casePath[i].value.Get();
                }
                currentIndex++;
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    build shader code for the current set of attributes and cases.
*/
bool
nD3D9MaterialBuilder::BuildShader()
{
    this->buildState = inBuildShader;
    this->technique = "";
    this->sequence = "";
    this->shaderNeedsBuilding = true;
    
    if (!this->onBuildShaderNameFunction.IsEmpty())
    {
        nString result;
        if (!this->refScriptServer->RunFunction(this->onBuildShaderNameFunction.Get(), result))
        {
            n_assert3_always( ("Failed run function on buildShaderName\n%s", result.Get()));
            return false;
        }
        n_assert(!this->shaderFileName.IsEmpty());
    }
    else
    {
        this->shaderFileName = "materials:";
        this->shaderFileName += this->shaderName;
        this->shaderFileName += ".fx";
    }

    // if the file exists, skip building the shader
    //if (!kernelServer->GetFileServer()->FileExists(this->shaderFileName.Get()))

    //build the shader code only if the shader file specified needs building
    if (this->shaderNeedsBuilding)
    {
        // if the shader is already loaded and valid, reuse it
        // @todo if the shader was already built and is not valid, return false-
        nShader2 *shd = nGfxServer2::Instance()->NewShader(this->shaderFileName.Get());
        n_assert(shd);
        if (!shd->IsValid())
        {
            this->file->Open(this->shaderFileName, "w");
            
            nString result;
            if (!this->refScriptServer->RunFunction(this->onBuildShaderFunction.Get(), result))
            {
                n_assert3_always( ("Failed run function on buildShader\n%s", result.Get()));
                file->Close();
                return false;
            }
            
            // create a log file, which by the way will be extremely useful sometime.       
            nFile *logFile = kernelServer->GetFileServer()->NewFileObject();
            
            if (logFile->Open(this->shaderFileName.Get(), "w"))
            {
                logFile->AppendFile(this->file);
                logFile->Close();
            }
            logFile->Release();
            
            // we don't load the shader directly because that creates an unwanted reference
            this->file->Seek(0, nFile::START);
            shd->SetLoadFlags(shd->GetLoadFlags() | nShader2::QuietLoadError);
            // set filename to allow the shader recover from a lost device
            shd->SetFilename(this->shaderFileName.Get());
            if (!shd->Load(this->file, 0, this->file->GetSize()))
            {
                // log the error so that the shader thus generated can be tracked.
                NLOG(resource, (NLOGUSER | 0, "nD3D9MaterialBuilder::BuildShader(): Failed to generate shader: '%s'!", this->GetShaderName()));
                this->ShowErrorForCurrentCasePath();
                shd->Release(); // important! otherwise, there will be a reference left
                this->file->Close();
                return false;
            }
            
            shd->SetLoadFlags(shd->GetLoadFlags() & ~nShader2::QuietLoadError);
            this->file->Close();
        }

        // set both filename and shader object to ensure that it doesn't try to load it again
        this->refShaderTree->SetShaderObject(shd);
    }
    
    this->refShaderTree->SetShader(this->shaderFileName.Get());
    this->refShaderTree->SetTechnique(this->technique.Get());
    this->refShaderTree->SetSequence(this->sequence.Get());
    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9MaterialBuilder::ShowErrorForCurrentCasePath()
{
    const char *paramName;
    const char *paramValue;

    int i;
    for (i = 0; i < this->refMaterial->GetNumParams(); i++)
    {
        this->refMaterial->GetParamAt(i, &paramName, &paramValue);
        NLOG(resource, (0, "%s=%s", paramName, paramValue))
    }

    for (i = 0; i < this->pathLen; i++)
    {
        const char* varName = nVariableServer::Instance()->GetVariableName(this->casePath[i].param);
        NLOG(resource, (0, "%s=%s", varName, this->casePath[i].value.Get()))
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9MaterialBuilder::Traverse()
{
    this->shaderName = this->refShaderTree->GetName();
    
    for (int i = 0; i < this->pathLen; i++)
    {
        this->shaderName += "_";
        this->shaderName += this->casePath[i].value;
    }
    
    for (int i = 0; i < pathLen; i++)
    {
        nVariable::Handle param = this->casePath[i].param;
        uint value = nVariableServer::StringToFourCC(this->casePath[i].value.Get());
        this->refShaderTree->BeginNode(param, value);
    }
    
    // if the shader could not be generated, invalidate current branch and return
    bool success = this->BuildShader();
    if (!success)
    {
        // @todo remove current branch from material tree
        this->refShaderTree->Remove();
    }
    
    for (int i = 0; i < pathLen; i++)
    {
        this->refShaderTree->EndNode();
    }

    if (!success)
    {
        return false;
    }
    
    // save current node
    Node *parentNode = this->shaderNode;
    
    // find all nodes with current as parent
    int n;
    int numNodes = nodeArray.Size();
    for (n = 0; n < numNodes; n++)
    {
        if (nodeArray[n].parent == parentNode)
        {
            this->shaderNode = &nodeArray[n];
            Case *newCase = &this->casePath[pathLen];
            newCase->param = this->shaderNode->param;
            ++pathLen;
            
            // find all cases for the current node
            int i;
            int numNodeCases = this->shaderNode->values.Size();
            for (i = 0; i < numNodeCases; i++)
            {
                newCase->value = this->shaderNode->values[i];
                this->Traverse();
            }
            pathLen--;
            
            // finally, traverse down the tree for the children nodes
            // this would generate a duplicate case node for the root?
            this->Traverse();
        }
    }
    
    this->shaderNode = parentNode;
    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9MaterialBuilder::LoadShader(nMaterial *material, const nFourCC fourcc, nShaderTree *shaderTree)
{
    n_assert(material);
    n_assert(shaderTree);
    
    this->materialName = material->GetFullName();
    this->refMaterial = material;
    this->refShaderTree = shaderTree;
    this->fourcc = fourcc;
    
    // 1. build shader description and case tree.
    this->shaderDesc.Clear(); 
    this->nodeArray.Clear();
    
    this->buildState = inBuildShaderTree;
    nString result;
    if (!this->refScriptServer->RunFunction(this->onBuildShaderTreeFunction.Get(), result))
    {
        n_assert3_always( ("Failed run function on BuildShaderTreeFunction\n%s", result.Get()));
        return false;
    }
    
    n_assert(this->stackDepth == 0);
    n_assert(this->shaderDesc.Size() > 0);
    
    this->file = n_new(nMemFile);
    
    // 2. traverse the tree and build the shaders
    this->pathLen = 0;
    this->shaderNode = 0;

    this->refShaderTree->BeginNode(nVariable::InvalidHandle, 0);
    bool success = this->Traverse();
    this->refShaderTree->EndNode();
    
    n_delete(this->file);
    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nD3D9MaterialBuilder::AppendFile(const char* fileName)
{
    nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    n_assert(fileServer);
    nFile* fileSrc = fileServer->NewFileObject();
    if (fileSrc->Open(fileName, "r"))
    {
        this->file->AppendFile(fileSrc);
        fileSrc->Close();
        fileSrc->Release();
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
int __cdecl 
nD3D9MaterialBuilder::shaderDescSorter(const ShaderAttr* elm0, const ShaderAttr* elm1)
{
    return strcmp(elm0->name.Get() , elm1->name.Get());
}
