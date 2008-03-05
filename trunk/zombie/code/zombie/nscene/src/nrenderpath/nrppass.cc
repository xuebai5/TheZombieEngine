#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrppass.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nrenderpath/nrppass.h"
#include "nrenderpath/nrprendertarget.h"
#include "nrenderpath/nrenderpath2.h"
#include "gfx2/ngfxserver2.h"
#include "misc/nconserver.h"
//#include "shadow/nshadowserver.h"
#include "zombieentity/ncdictionary.h"
#include "nscene/ncviewport.h"

//------------------------------------------------------------------------------
/**
*/
nRpPass::nRpPass() :
    inBegin(false),
    clearFlags(0),
    clearColor(0.0f, 0.0f, 0.0f, 1.0f),
    clearDepth(1.0f),
    clearStencil(0),
    shaderFourCC(0),
    renderPath(0),
    drawFullscreenQuad(false),
    drawShadowVolumes(false),
    drawGui(false),
    shadowEnabledCondition(false),
    restoreState(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpPass::~nRpPass()
{
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
    if (this->refQuadMesh.isvalid())
    {
        this->refQuadMesh->Release();
        this->refQuadMesh.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Validate the pass object. This will validate the pass shader and
    also invoke Validate() on all owned phase objects.
*/
void
nRpPass::Validate()
{
    // invoke validate on phases
    int i;
    int num = this->phases.Size();
    for (i = 0; i < num; i++)
    {
        this->phases[i].SetRenderPath(this->renderPath);
        this->phases[i].Validate();
    }

    // validate shader
    if (!this->shaderPath.IsEmpty())
    {
        nShader2* shd = 0;
        if (!this->refShader.isvalid())
        {
            n_assert(!this->shaderPath.IsEmpty());
            shd = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
            this->refShader = shd;
        }
        else
        {
            shd = this->refShader;
        }

        n_assert(shd);
        if (!shd->IsLoaded())
        {
            shd->SetFilename(this->shaderPath);
            if (!shd->Load())
            {
                shd->Release();
                n_error("nRpPass: could not load shader '%s'!", this->shaderPath.Get());
            }
        }
    }

    // validate quad mesh
    nMesh2* mesh = 0;
    if (!this->refQuadMesh.isvalid())
    {
        mesh = nGfxServer2::Instance()->NewMesh("_rpmesh");
        if (!mesh->IsLoaded())
        {
            mesh->SetUsage(nMesh2::WriteOnly);
            mesh->SetNumVertices(4);
            mesh->SetNumIndices(6);
            mesh->SetVertexComponents(nMesh2::Coord | nMesh2::Uv0);
            mesh->Load();
        }
        this->refQuadMesh = mesh;
    }
    else
    {
        mesh = this->refQuadMesh;
    }
}

//------------------------------------------------------------------------------
/**
    Update the mesh coordinates. This takes several things into account:
    - 3d coordinates are created for direct mapping between texels and pixels
    - uv coordinates are take the render targets, or source texture's
      border color into account (hmm, tricky...)
*/
void
nRpPass::UpdateMeshCoords()
{
    // compute half pixel size for current render target
    nTexture2* renderTarget = nGfxServer2::Instance()->GetRenderTarget(0);
    int w, h;
    if (renderTarget)
    {
        w = renderTarget->GetWidth();
        h = renderTarget->GetHeight();
    }
    else
    {
        const nDisplayMode2& mode = nGfxServer2::Instance()->GetDisplayMode();
        w = mode.GetWidth();
        h = mode.GetHeight();
    }
    vector2 pixelSize(1.0f / float(w), 1.0f / float(h));
    vector2 halfPixelSize = pixelSize * 0.5f;

    float x0 = -1.0f;
    float x1 = +1.0f - pixelSize.x;
    float y0 = -1.0f + pixelSize.y;
    float y1 = +1.0f;

    float u0 = 0.0f + halfPixelSize.x;
    float u1 = 1.0f - halfPixelSize.x;
    float v0 = 0.0f + halfPixelSize.y;
    float v1 = 1.0f - halfPixelSize.y;

    nMesh2* mesh = this->refQuadMesh;
    float* vPtr = mesh->LockVertices();
    n_assert(vPtr);    
    *vPtr++ = x0; *vPtr++ = y1; *vPtr++ = 0.0f; *vPtr++ = u0; *vPtr++ = v0;
    *vPtr++ = x0; *vPtr++ = y0; *vPtr++ = 0.0f; *vPtr++ = u0; *vPtr++ = v1;
    *vPtr++ = x1; *vPtr++ = y1; *vPtr++ = 0.0f; *vPtr++ = u1; *vPtr++ = v0;
    *vPtr++ = x1; *vPtr++ = y0; *vPtr++ = 0.0f; *vPtr++ = u1; *vPtr++ = v1;
    mesh->UnlockVertices();

    ushort* iPtr = mesh->LockIndices();
    n_assert(iPtr);
    *iPtr++ = 0; *iPtr++ = 1; *iPtr++ = 2;
    *iPtr++ = 1; *iPtr++ = 3; *iPtr++ = 2;
    mesh->UnlockIndices();
}

//------------------------------------------------------------------------------
/**
    Begin a scene pass. This will set the render target, activate the pass
    shader and set any shader parameters.
*/
int
nRpPass::Begin(nSceneGraph* /*sceneGraph*/, nEntityObject* entityObject)
{
    n_assert(!this->inBegin);

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // only render this pass if shadowing is enabled?
/*    if (this->shadowEnabledCondition && 
        ((gfxServer->GetNumStencilBits() == 0) || (!nShadowServer::Instance()->GetShowShadows())))
    {
        return 0;
    }*/

    // set render target
    if (this->renderTargetName.IsEmpty())
    {
        // set default render target
        gfxServer->SetRenderTarget(0, 0);
    }
    else
    {
        // set custom render target
        nRpRenderTarget* rpRenderTarget = this->renderPath->FindRenderTarget(this->renderTargetName);
        if (0 == rpRenderTarget)
        {
            n_error("nRpPass: invalid render target name: %s!", this->renderTargetName.Get());
        }
        gfxServer->SetRenderTarget(0, rpRenderTarget->GetTexture());
    }

    // invoke begin scene
    if (!gfxServer->BeginScene())
    {
        return 0;
    }

    // set viewport before clearing the frame buffer and drawing anything else
    vector4 clearColor(this->clearColor);
    if (this->GetRenderTargetName().IsEmpty())
    {
        ncViewport *viewContext = entityObject->GetComponentSafe<ncViewport>();
        // FIXME ATI and nVidia Cards use a different default viewport
        // temporarily we just copy the rectangle component of the viewport.
        nViewport& curViewport = gfxServer->GetViewport();
        nCamera2 camera = viewContext->GetCamera();
        viewContext->GetViewport(curViewport.x, curViewport.y, curViewport.width, curViewport.height);
        gfxServer->SetCamera(camera);
        gfxServer->SetViewport(curViewport);

        // allow overriding the clear color for the frame buffer from the entity 
        // variable name is fixed to "clearcolor"
        ncDictionary* varContext = entityObject->GetComponentSafe<ncDictionary>();
        static nVariable::Handle colorHandle = nVariableServer::Instance()->GetVariableHandleByName("clearcolor");
        nVariable* colorVar = varContext->GetVariable(colorHandle);
        if (colorVar)
        {
            clearColor.set(colorVar->GetVector4());
        }
    }

    // clear render target?
    if (this->clearFlags != 0)
    {
        gfxServer->Clear(this->clearFlags, 
                         clearColor.x, 
                         clearColor.y, 
                         clearColor.z, 
                         clearColor.w, 
                         this->clearDepth, 
                         this->clearStencil);
    }

    // apply shader (note: save/restore all shader state for pass shaders!)
    if (this->refShader.isvalid())
    {
        this->UpdateVariableShaderParams(entityObject);
        nShader2* shd = this->refShader;
        if (!this->technique.IsEmpty())
        {
            shd->SetTechnique(this->technique.Get());
        }
        shd->SetParams(this->shaderParams);
        gfxServer->SetShader(shd);
        N_IFDEF_ASSERTS(int numPasses =) shd->Begin(false);
        N_IFDEF_ASSERTS(int numExpectedPasses = this->GetRestoreState() ? 2 : 1);
        n_assert(numPasses == numExpectedPasses);
        shd->BeginPass(0);
    }

    // draw the fullscreen quad?
    if (this->drawFullscreenQuad)
    {
        // update the mesh coordinates
        this->UpdateMeshCoords();

        // draw the quad
        const matrix44 ident;
        gfxServer->PushTransform(nGfxServer2::Model, ident);
        gfxServer->PushTransform(nGfxServer2::View, ident);
        gfxServer->PushTransform(nGfxServer2::Projection, ident);
        gfxServer->SetMesh(this->refQuadMesh, this->refQuadMesh);
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
        gfxServer->SetMesh(0, 0);  // FIXME FLOH: hmm, why is this necessary??? otherwise mesh data will be broken...
        gfxServer->PopTransform(nGfxServer2::Projection);
        gfxServer->PopTransform(nGfxServer2::View);
        gfxServer->PopTransform(nGfxServer2::Model);
    }

    this->inBegin = true;
    return this->phases.Size();
}

//------------------------------------------------------------------------------
/**
    Finish a scene pass.
*/
void
nRpPass::End()
{
    if (!this->inBegin)
    {
        return;
    }

    if (this->refShader.isvalid())
    {
        nShader2* shd = this->refShader;
        shd->EndPass();

        // apply restore pass
        if (this->GetRestoreState())
        {
            shd->BeginPass(1);
            shd->EndPass();
        }

        shd->End();
    }

    nGfxServer2::Instance()->EndScene();

    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
    This gathers the current global variable values from the render path
    object and updates the shader parameter block with the new values.
*/
void
nRpPass::UpdateVariableShaderParams(nEntityObject* entityObject)
{
    // for each variable shader parameter...
    int varIndex;
    int numVars = this->varContext.GetNumVariables();
    for (varIndex = 0; varIndex < numVars; varIndex++)
    {
        const nVariable& paramVar = this->varContext.GetVariableAt(varIndex);
        
        // get shader state from variable
        nShaderState::Param shaderParam = (nShaderState::Param) paramVar.GetInt();

        // get the current value
        ncDictionary* dictionary = entityObject->GetComponent<ncDictionary>();
        n_assert(dictionary);
        const nVariable* valueVar = dictionary->GetVariable(paramVar.GetHandle());
        n_assert(valueVar);

        nShaderArg shaderArg;
        switch (valueVar->GetType())
        {
            case nVariable::Int:
                shaderArg.SetInt(valueVar->GetInt());
                break;

            case nVariable::Float:
                shaderArg.SetFloat(valueVar->GetFloat());
                break;

            case nVariable::Float4:
                shaderArg.SetFloat4(valueVar->GetFloat4());
                break;

            case nVariable::Object:
                shaderArg.SetTexture((nTexture2*) valueVar->GetObj());
                break;

            case nVariable::Matrix:
                shaderArg.SetMatrix44(&valueVar->GetMatrix());
                break;

            case nVariable::Vector4:
                shaderArg.SetVector4(valueVar->GetVector4());
                break;

            default:
                n_error("nRpPass: Invalid shader arg datatype!");
                break;
        }
        
        // update the shader parameter
        this->shaderParams.SetArg(shaderParam, shaderArg);
    }
}
