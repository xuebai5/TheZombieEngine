//------------------------------------------------------------------------------
//  ngfxserver2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nfont2.h"
#include "gfx2/nmesharray.h"

nNebulaScriptClass(nGfxServer2, "nroot");
nGfxServer2* nGfxServer2::Singleton = 0;

//------------------------------------------------------------------------------
static const char * groupNames[] = {
    "Shader Params",
    0
};

//------------------------------------------------------------------------------
NCREATELOGLEVELGROUP(gfx, "Graphics", false, 0, groupNames, NLOG_GROUP_MASK);

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nGfxServer2, RestoreOnLost);

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::nGfxServer2() :
    displayOpen(false),
    inBeginFrame(false),
    inBeginScene(false),
    inBeginLines(false),
    inBeginShapes(false),
    refResource("/sys/servers/resource"),
    vertexRangeFirst(0),
    vertexRangeNum(0),
    indexRangeFirst(0),
    indexRangeNum(0),
    featureSetOverride(InvalidFeatureSet),
    cursorVisibility(System),
    cursorDirty(true),
    isFocus( false ),
    inDialogBoxMode(false),
    gamma(1.0f),
    brightness(0.5f),
    contrast(0.5f),
    fontScale(1.0f),
    fontMinHeight(12),
    deviceIdentifier(GenericDevice),
    refRenderTargets(MaxRenderTargets),
    lightingType(Off),
    scissorRect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f)),
    hints(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    int i;
    for (i = 0; i < NumTransformTypes; i++)
    {
        this->transformTopOfStack[i] = 0;
        this->dirtyTransform[i] = false;
    }

    #if __NEBULA_STATS__
    this->profGUIBreakLines.Initialize("profGUI_BreakLines", true);
    this->profGUIGetTextExtent.Initialize("profGUI_GetTextExtent", true);
    this->profGUIDrawText.Initialize("profGUI_DrawText", true);
    #endif

    // initial render state
    this->renderStateStack.PushBack( RenderState() );
    this->SetDefaultRenderState();
}

//------------------------------------------------------------------------------
/**
*/
nGfxServer2::~nGfxServer2()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new static mesh object.

    @param  rsrcName    a resource name for resource sharing
    @return             a new nMesh2 object
*/
nMesh2*
nGfxServer2::NewMesh(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new static mesh array object.

    @return             a new nMeshArray object
*/
nMeshArray*
nGfxServer2::NewMeshArray(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared texture object.

    @param  rsrcName    a resource name for resource sharing
    @return             a new nTexture2 object
*/
nTexture2*
nGfxServer2::NewTexture(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared shader object

    @param  rsrcName    a resource name for resource sharing
    @return             a new nShader2 object
*/
nShader2*
nGfxServer2::NewShader(const char* /*rsrcName*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared font object.

    @param  rsrcName    a resource name for resource sharing
    @param  fontDesc    a valid font description object
    @return             a nFont2 object
*/
nFont2*
nGfxServer2::NewFont(const char* /*rsrcName*/, const nFontDesc& /*fontDesc*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shared instance stream object.

    @param  rsrcName    a resource name for resource sharing (0 if no sharing)
    @return             pointer to a new nInstanceStream object
*/
nInstanceStream*
nGfxServer2::NewInstanceStream(const char* rsrcName)
{
    return (nInstanceStream*) this->refResource->NewResource("ninstancestream", rsrcName, nResource::Other);
}

//------------------------------------------------------------------------------
/**
    Create a new occlusion query object.

    @return     pointer to a new nOcclusionQuery object
*/
nOcclusionQuery*
nGfxServer2::NewOcclusionQuery()
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new render target object.

    @param  rsrcName    a resource name for resource sharing
    @param  width       width of render target
    @param  height      height of render target
    @param  format      pixel format of render target
    @param  usageFlags  a combination of nTexture2::Usage flags (RenderTargetXXX only)
*/
nTexture2*
nGfxServer2::NewRenderTarget(const char* /*rsrcName*/,
                             int /*width*/,
                             int /*height*/,
                             nTexture2::Format /*format*/,
                             int /*usageFlags*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Set the prefered display mode. This must be called outside
    OpenDisplay()/CloseDisplay().
*/
void
nGfxServer2::SetDisplayMode(const nDisplayMode2& /*mode*/)
{
    n_error("nGfxServer2: Pure virtual function called!");
}

//------------------------------------------------------------------------------
/**
*/
const nDisplayMode2&
nGfxServer2::GetDisplayMode() const
{
    n_error("nGfxServer2: Pure virtual function called!");
    static nDisplayMode2 dummy;
    return dummy;
}

//------------------------------------------------------------------------------
/**
    Set the window title.

    @param title        the new window title
*/
void
nGfxServer2::SetWindowTitle(const char* /*title*/)
{
    n_error("nGfxServer2: Pure virtual function called!");
}

//------------------------------------------------------------------------------
/**
    Set the current camera. Subclasses should adjust their projection matrix
    accordingly when this method is called.

    @param  camera      a camera object with valid parameters
*/
void
nGfxServer2::SetCamera(nCamera2& camera)
{
    this->camera = camera;
}

//------------------------------------------------------------------------------
/**
    Set the current viewport. Subclasses should adjust the device accordingly.
*/
void
nGfxServer2::SetViewport(nViewport& vp)
{
    this->viewport = vp;
}

//------------------------------------------------------------------------------
/**
    Open the display.

    @return     true if display successfully opened
*/
bool
nGfxServer2::OpenDisplay()
{
    n_assert(!this->displayOpen);
    this->displayOpen = true;

    // Adjust new display to user-defined gamma (if any)
    this->AdjustGamma();

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the display.

    23-Aug-04    kims    added calling RestoreGamma().
*/
void
nGfxServer2::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->displayOpen = false;

    // restore to original.
    this->RestoreGamma();
}

//------------------------------------------------------------------------------
/**
    @return     true if display is opened.
*/
bool
nGfxServer2::IsDisplayOpen()
{
    return this->displayOpen;
}

//------------------------------------------------------------------------------
/**
    Trigger the windows system message pump. Return false if application
    should be closed on request of the windows system.

    @return     false if window system requests application to quit
*/
bool
nGfxServer2::Trigger()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Reset the light array. This will happen automatically in BeginScene().
*/
void
nGfxServer2::ClearLights()
{
    this->lightArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Reset the light array. This will happen automatically in BeginScene().
*/
void
nGfxServer2::ClearPointLights()
{
    int index;
    for(index = 0; index < this->lightArray.Size(); index++)
    {
        if(nLight::Point == this->lightArray[index].GetType())
        {
            this->ClearLight(index);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGfxServer2::ClearLight(int index)
{
    n_assert(index >= 0);
    this->lightArray.Erase(index);
}

//------------------------------------------------------------------------------
/**
    Add a light to the light array. Return new number of lights.
*/
int
nGfxServer2::AddLight(const nLight& light)
{
    this->lightArray.Append(light);
    return this->lightArray.Size();
}

//------------------------------------------------------------------------------
/**
    Begin rendering the current frame. This is guaranteed to be called
    exactly once per frame.
*/
bool
nGfxServer2::BeginFrame()
{
    n_assert(!this->inBeginFrame);
    n_assert(!this->inBeginScene);
    this->inBeginFrame = true;

    #if __NEBULA_STATS__
    //this->profGUIBreakLines.ResetAccum();
    //this->profGUIDrawText.ResetAccum();
    //this->profGUIGetTextExtent.ResetAccum();
    #endif

    return true;
}

//------------------------------------------------------------------------------
/**
    Finish rendering the current frame. This is guaranteed to be called
    exactly once per frame after PresentScene() has happened.
*/
void
nGfxServer2::EndFrame()
{
    n_assert(this->inBeginFrame);
    n_assert(!this->inBeginScene);
    this->inBeginFrame = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering to the current render target. This may get called
    several times per frame.

    @return     false on error, do not call EndScene() or Present() in this case
*/
bool
nGfxServer2::BeginScene()
{
    n_assert(this->inBeginFrame);
    n_assert(!this->inBeginScene);
    if (this->displayOpen)
    {
        this->inBeginScene = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Finish rendering to the current render target.

    @return     false on error, do not call Present() in this case.
*/
void
nGfxServer2::EndScene()
{
    n_assert(this->inBeginFrame);
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    Present the contents of the back buffer. This must be called
    outside BeginScene()/EndScene().
*/
void
nGfxServer2::PresentScene()
{
    n_assert(!this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
    Clear buffers.
*/
void
nGfxServer2::Clear(int /*bufferTypes*/, float /*red*/, float /*green*/, float /*blue*/, float /*alpha*/, float /*z*/, int /*stencil*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the current render target at a given index (for simultaneous render targets). 
    This method must be called outside BeginScene()/EndScene(). The method will 
    increment the refcount of the render target object and decrement the refcount of the
    previous render target.

    @param  index   render target index
    @param  t       the new render target, or 0 to render to the frame buffer
*/
void
nGfxServer2::SetRenderTarget(int index, nTexture2* t)
{
    n_assert(!this->inBeginScene);
    if (t)
    {
        t->AddRef();
    }
    if (this->refRenderTargets[index].isvalid())
    {
        nObject* tmp = this->refRenderTargets[index].get(); // Use a temporal variable because the textures's destructor calls this method
        this->refRenderTargets[index].invalidate(); 
        tmp->Release();
    }
    this->refRenderTargets[index] = t;
}

//------------------------------------------------------------------------------
/**
    Set the current mesh object for rendering.

    @param  vbMesh  mesh which delivers the vertex buffer
    @param  ibMesh  mesh which delivers the index buffer
*/
void
nGfxServer2::SetMesh(nMesh2* vbMesh, nMesh2* ibMesh)
{
    this->refVbMesh = vbMesh;
    this->refIbMesh = ibMesh;
}

//------------------------------------------------------------------------------
/**
    Set the current mesh array object for rendering.

    @param  meshArray   pointer to a nMeshArray object
*/
void
nGfxServer2::SetMeshArray(nMeshArray* meshArray)
{
    this->refMeshArray = meshArray;
}

//------------------------------------------------------------------------------
/**
    Notify a mesh is being unloaded

    @param  nMesh       pointer to a nMesh2 object
*/
void
nGfxServer2::OnUnloadMesh(nMesh2 * mesh)
{
    n_assert_return(mesh, );
    if (this->GetVerticesMesh() == mesh || this->GetIndicesMesh() == mesh)
    {
        this->SetMesh(0, 0);
    }

    nMeshArray * meshArray = this->GetMeshArray();
    if (meshArray)
    {
        bool meshFound = false;
        for (int i = 0;!meshFound && i < nGfxServer2::MaxVertexStreams; i++)
        {
            meshFound = (meshArray->GetMeshAt(i) == mesh);
        }

        if (meshFound)
        {
            this->SetMeshArray(0);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set the current shader object for rendering.

    @param  shd     pointer to a nShader2 object
*/
void
nGfxServer2::SetShader(nShader2* shd)
{
    this->refShader = shd;
}

//------------------------------------------------------------------------------
/**
    Set the current font object for rendering.

    @param  font        pointer to a nFont2 object
*/
void
nGfxServer2::SetFont(nFont2* font)
{
    this->refFont = font;
}

//------------------------------------------------------------------------------
/**
    Set the current instance stream object for rendering.

    @param  stream      pointer to nInstanceStream object
*/
void
nGfxServer2::SetInstanceStream(nInstanceStream* stream)
{
    this->refInstanceStream = stream;
}

//------------------------------------------------------------------------------
/**
    Set the current mouse cursor.

    @param  cursor      a valid mouse cursor object
*/
void
nGfxServer2::SetMouseCursor(const nMouseCursor& cursor)
{
    this->curMouseCursor = cursor;
    this->cursorDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get the current mouse cursor.
*/
const nMouseCursor&
nGfxServer2::GetMouseCursor() const
{
    return this->curMouseCursor;
}

//------------------------------------------------------------------------------
/**
    Set transformation matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::SetTransform(TransformType type, const matrix44& matrix)
{
    n_assert(type < NumTransformTypes);
    switch (type)
    {
        case Model:
            this->transform[Model] = matrix;
            this->dirtyTransform[InvModel] = true;
            this->dirtyTransform[ModelView] = true;
            this->dirtyTransform[InvModelView] = true;
            this->dirtyTransform[ModelLight] = true;
            this->dirtyTransform[InvModelLight] = true;
            this->dirtyTransform[ModelViewProjection] = true;
            break;

        case View:
            this->transform[View] = matrix;
            this->dirtyTransform[InvView] = true;
            this->dirtyTransform[ModelView] = true;
            this->dirtyTransform[InvModelView] = true;
            this->dirtyTransform[ViewProjection] = true;
            break;

        case Projection:
            this->transform[Projection] = matrix;
            this->dirtyTransform[ViewProjection] = true;
            this->dirtyTransform[ModelViewProjection] = true;
            break;

        case ShadowProjection:
            this->transform[ShadowProjection] = matrix;

        case Texture0:
        case Texture1:
        case Texture2:
        case Texture3:
            this->transform[type] = matrix;
            break;

        case Light:
            this->transform[type] = matrix;
            this->dirtyTransform[ModelLight] = true;
            this->dirtyTransform[ModelLightProjection] = true;
            break;

        case LightProjection:
            this->transform[type] = matrix;
            this->dirtyTransform[ModelLightProjection] = true;
            break;

        default:
            n_error("nGfxServer2::SetTransform() Trying to set read-only transform type!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Set transformation matrix.

    @param  type    transform type
    @return         the 4x4 matrix
*/
const matrix44&
nGfxServer2::GetTransform(TransformType type)
{
    n_assert(type < NumTransformTypes);
    if (this->dirtyTransform[type])
    {
        switch (type)
        {
        case InvModel:
            this->transform[InvModel] = this->transform[Model];
            this->transform[InvModel].invert_simple();
            break;

        case InvView:
            this->transform[InvView] = this->transform[View];
            this->transform[InvView].invert_simple();
            break;

        case ModelView:
            this->transform[ModelView] = this->transform[Model] * this->transform[View];
            break;

        case InvModelView:
            this->transform[InvModelView] = this->GetTransform(InvView) * this->GetTransform(InvModel);
            break;

        case ModelLight:
            this->transform[ModelLight] = this->transform[Model] * this->transform[Light];
            break;

        case InvModelLight:
            this->transform[InvModelLight] = this->transform[Light] * this->GetTransform(InvModel);
            break;

        case ViewProjection:
            this->transform[ViewProjection] = this->transform[View] * this->transform[Projection];
            break;

        case ModelViewProjection:
            this->transform[ModelViewProjection] = this->GetTransform(ModelView) * this->transform[Projection];
            break;

        case ModelLightProjection:
            this->transform[ModelLightProjection] = this->GetTransform(ModelLight) * this->GetTransform(LightProjection);
            break;
        }

        this->dirtyTransform[type] = false;
    }
    return this->transform[type];
}

//------------------------------------------------------------------------------
/**
    Push current transformation on stack and set new matrix.

    @param  type        transform type
    @param  matrix      the 4x4 matrix
*/
void
nGfxServer2::PushTransform(TransformType type, const matrix44& matrix)
{
    n_assert(type < NumTransformTypes);
    n_assert(this->transformTopOfStack[type] < MaxTransformStackDepth);
    this->transformStack[type][this->transformTopOfStack[type]++] = this->transform[type];
    this->SetTransform(type, matrix);
}

//------------------------------------------------------------------------------
/**
    Pop transformation from stack and make it the current transform.
*/
const matrix44&
nGfxServer2::PopTransform(TransformType type)
{
    n_assert(type < NumTransformTypes);
    this->SetTransform(type, this->transformStack[type][--this->transformTopOfStack[type]]);
    return this->transform[type];
}

//------------------------------------------------------------------------------
/**
    Draw current mesh with indexed primitives.
*/
void
nGfxServer2::DrawIndexed(PrimitiveType /*primType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw current mesh with non-indexed primitives.
*/
void
nGfxServer2::Draw(PrimitiveType /*primType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render indexed primitives without applying shader state. You have
    to call nShader2::Begin(), nShader2::Pass() and nShader2::End() 
    yourself as needed.
*/
void
nGfxServer2::DrawIndexedNS(PrimitiveType /*primType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render non-indexed primitives without applying shader state. You have
    to call nShader2::Begin(), nShader2::Pass() and nShader2::End() 
    yourself as needed.
*/
void
nGfxServer2::DrawNS(PrimitiveType /*primType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add text to the text buffer (OBSOLETE)
*/
void
nGfxServer2::Text(const char* /*text*/, const vector4& /*color*/, float /*x*/, float /*y*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the text in the text buffer (OBSOLETE)
*/
void
nGfxServer2::DrawTextBuffer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw text.

    @param  text    the text to draw
    @param  color   the text color
    @param  rect    screen space rectangle in which to draw the text
    @param  flags   combination of nFont2::RenderFlags
*/
void
nGfxServer2::DrawText(const char* /*text*/, const vector4& /*color*/, const rectangle& /*rect*/, uint /*flags*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return the text extents of the given text using the current font in
    screen space coordinates.
*/
vector2
nGfxServer2::GetTextExtent(const char* /*text*/)
{
    return vector2(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
/**
    Return the supported feature set.
*/
nGfxServer2::FeatureSet
nGfxServer2::GetFeatureSet()
{
    return InvalidFeatureSet;
}

//------------------------------------------------------------------------------
/**
    Save a screenshot.
*/
bool
nGfxServer2::SaveScreenshot(const char* /*filename*/)
{
    // implement me in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Set cursor visibility.
*/
void
nGfxServer2::SetCursorVisibility(CursorVisibility v)
{
    this->cursorVisibility = v;
    this->cursorDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get the mouse cursor visibility status.
*/
nGfxServer2::CursorVisibility
nGfxServer2::GetCursorVisibility() const
{
    return this->cursorVisibility;
}

//------------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nGfxServer2::EnterDialogBoxMode()
{
    n_assert(!this->inDialogBoxMode);
    this->inDialogBoxMode = true;
}

//------------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nGfxServer2::LeaveDialogBoxMode()
{
    n_assert(this->inDialogBoxMode);
    this->inDialogBoxMode = false;
}

//------------------------------------------------------------------------------
/**
    Return true if currently in dialog box mode.
*/
bool
nGfxServer2::InDialogBoxMode() const
{
    return this->inDialogBoxMode;
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available stencil bits 
    (override in subclass).
*/
int
nGfxServer2::GetNumStencilBits() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    This method should return the number of currently available depth bits
    (override in subclass).
*/
int
nGfxServer2::GetNumDepthBits() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Begin rendering lines. Override this method in a subclass.
*/
void
nGfxServer2::BeginLines()
{
    n_assert(!this->inBeginLines);
    this->inBeginLines = true;
}

//------------------------------------------------------------------------------
/**
    Draw a 3d line strip using the current transforms. 

    The following draws a unit cube with red lines.

    @code
    vector3 v[8];
    v[0].set(0.5f, -0.5f, 0.5f);
    v[1].set(0.5f, -0.5f, -0.5f);
    v[2].set(-0.5f, -0.5f, -0.5f);
    v[3].set(-0.5f, -0.5f, 0.5f);
    v[4].set(0.5f, 0.5f, 0.5f);
    v[5].set(0.5f, 0.5f, -0.5f);
    v[6].set(-0.5f, 0.5f, -0.5f);
    v[7].set(-0.5f, 0.5f, 0.5f);

    vector3 cube[16] = {
        v[1], v[0], v[4], v[5],
        v[1], v[2], v[6], v[5],
        v[3], v[2], v[6], v[7],
        v[3], v[0], v[4], v[7]
    };

    nGfxServer2::Instance()->BeginLines();
    nGfxServer2::Instance()->DrawLines3d(cube,   8, vector4(1.0f, 0.0f, 0.0f, 0.5f));
    nGfxServer2::Instance()->DrawLines3d(cube+8, 8, vector4(1.0f, 0.0f, 0.0f, 0.5f));
    nGfxServer2::Instance()->EndLines();
    @endcode
*/
void
nGfxServer2::DrawLines3d(const vector3* /*vertexList*/, int /*numVertices*/, const vector4& /*color*/, bool /*clip*/)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
    Draw a 2d lines in screen space.
*/
void
nGfxServer2::DrawLines2d(const vector2* /*vertexList*/, int /*numVertices*/, const vector4& /*color*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Finish rendering lines. Override this method in a subclass.
*/
void
nGfxServer2::EndLines()
{
    n_assert(this->inBeginLines);
    this->inBeginLines = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering shapes.
    @param  altShapeShader  (optional)  
    Allows providing an alternative shader for shape rendering. 
    Subclasses are free to interpret or ignore this parameter at will.
    The original shader used for shape rendering should be restored at
    EndShapes()
*/
void
nGfxServer2::BeginShapes(nShader2* /*altShader*/)
{
    n_assert(!this->inBeginShapes);
    this->inBeginShapes = true;
}

//------------------------------------------------------------------------------
/**
    Render a shape.
*/
void
nGfxServer2::DrawShape(ShapeType /*type*/, const matrix44& /*model*/, const vector4& /*color*/)
{
    n_assert(this->inBeginShapes);
}

//------------------------------------------------------------------------------
/**
    Render a shape without shader management.
*/
void
nGfxServer2::DrawShapeNS(ShapeType /*type*/, const matrix44& /*model*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw prmitives with the given model matrix with given color.
*/
void 
nGfxServer2::DrawShapePrimitives(PrimitiveType /*type*/, int /*numPrimitives*/, const vector3* /*vertexList*/, int /*vertexWidth*/, const matrix44& /*model*/, const vector4& /*color*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Draw indexed prmitives with the given model matrix with given color.
*/
void
nGfxServer2::DrawShapeIndexedPrimitives(PrimitiveType /*type*/, int /*numPrimitives*/, const vector3* /*vertexList*/, int /*numVertices*/, int /*vertexWidth*/, void* /*indices*/, IndexType /*indexType*/, const matrix44& /*model*/, const vector4& /*color*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Finish shape drawing.

    Subclasses should restore the shader used for shape rendering.
*/
void
nGfxServer2::EndShapes()
{
    n_assert(this->inBeginShapes);
    this->inBeginShapes = false;
}

//------------------------------------------------------------------------------
/**
    Returns true when vertex shaders run in emulation. This is needed by
    mesh handling classes when running in the Fixed Function pipeline code
    path. The method must be overwritten in subclasses.
*/
bool
nGfxServer2::AreVertexShadersEmulated()
{
    n_error("nGfxServer2::AreVertexShadersEmulated() called!");
    return false;
}

//------------------------------------------------------------------------------
/**
    - 23-Aug-04    kims    created
*/
void
nGfxServer2::AdjustGamma()
{
    // empty.
}

//------------------------------------------------------------------------------
/**
    - 23-Aug-04    kims    created
*/
void
nGfxServer2::RestoreGamma()
{
    // empty.
}
//------------------------------------------------------------------------------
/**
    Break a string into lines by replacing spaces with newlines.
    Word break in D3DX doesn't work for umlauts, that's why this fix is
    needed. 

    FIXME: this should be removed once D3DX has been fixed!

    - 20-Sep-04     floh    fixed the implementation to use nString instead of
                            insecure char* 
*/
void
nGfxServer2::BreakLines(const nString& inText, const rectangle& rect, nString& outString)
{
#if __NEBULA_STATS__
    this->profGUIBreakLines.StartAccum();
#endif

    n_assert(!inText.IsEmpty());
    n_assert(this->refFont.isvalid());
    
    // text lenght
    const int textLength = inText.Length();
    n_assert(textLength > 0);
    
    // allocate memory
    outString = inText;

    // do the line break
    bool finished = false;
    int lastLineBegin = 0;
    int previousLineEndTestMark = -1;
    int lineEndTestMark = 0;
    
    while (!finished)
    {
        // search the next white space for test newline test
        while (!finished && (outString[lineEndTestMark] != ' '))
        {
            if (lineEndTestMark >= textLength)
            {
                // end of text - stop
                finished = true;
            }
            else
            {
                lineEndTestMark++;
            }
        }

        if (lineEndTestMark > 0) // skip a possible leading white space
        {
            // set a text end mark at the current test postion
            outString[lineEndTestMark] = 0;

            // get the text extend
            const vector2& textExtend = this->GetTextExtent(outString.Get());

            // restore the original text (but only if this isn't the last char, that must be \0)
            if (lineEndTestMark < textLength)
            {
                outString[lineEndTestMark] = ' ';
            }

            if (textExtend.x >= rect.width())
            {
                // the test line is greater then the rect, insert a newline at the previous tested position
                if ((previousLineEndTestMark > 0) && (lastLineBegin != previousLineEndTestMark))
                {
                    lastLineBegin = previousLineEndTestMark;
                    outString[lastLineBegin] = '\n';
                }
                else
                {
                    // there was no valid previous tested position (reason, no white space in the fisrt line, or since the
                    // last inserted new line
                    // insert the new line at the current test positon
                    lastLineBegin = lineEndTestMark;
                    previousLineEndTestMark = lastLineBegin;
                    outString[lastLineBegin] = '\n';
                    
                    // debug
                    n_printf("nGfxServer2::BreakLines(): found a part in the text that don't fit into one line. Please insert a new line manual!\n\
                             Text: '%s'\n", outString.Get());
                }
            }
            else
            {
                // line fits in screen, remember this test position and continue at next char
                previousLineEndTestMark = lineEndTestMark;
                lineEndTestMark++;
            }
        }
    }
#if __NEBULA_STATS__
    this->profGUIBreakLines.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Utility function which computes a ray in world space between the eye
    and the current mouse position on the near plane.
*/
line3
nGfxServer2::ComputeWorldMouseRay(const vector2& mousePos, float length)
{
    // get the current Nebula camera description and view matrix
    nCamera2& nebCamera = this->GetCamera();
    const matrix44& viewMatrix = this->GetTransform(nGfxServer2::InvView);

    // Compute mouse position in world coordinates.
    vector3 screenCoord3D((mousePos.x - 0.5f) * 2.0f, (mousePos.y - 0.5f) * 2.0f, 1.0f);
    vector3 viewCoord = nebCamera.GetInvProjection() * screenCoord3D;
    vector3 localMousePos = viewCoord * nebCamera.GetNearPlane() * 1.1f;
    localMousePos.y = -localMousePos.y;
    vector3 worldMousePos = viewMatrix * localMousePos;
    vector3 worldMouseDir = worldMousePos - viewMatrix.pos_component();
    worldMouseDir.norm();
    worldMouseDir *= length;

    return line3(worldMousePos, worldMousePos + worldMouseDir);
}

//------------------------------------------------------------------------------
/**
    - 24-Nov-04    kims    created
*/
void
nGfxServer2::SetSkipMsgLoop(bool /*skip*/)
{
    // empty.
}

//------------------------------------------------------------------------------
/**
    Set the current scissor rectangle in virtual screen space coordinate
    (top-left is (0.0f, 0.0f), bottom-right is (1.0f, 1.0f)). This
    method doesn't enable or disable the scissor rectangle, this must be
    done externally in the shader. The default scissor rectangle
    is ((0.0f, 0.0f), (1.0f, 1.0f)).
*/
void
nGfxServer2::SetScissorRect(const rectangle& r)
{
    this->scissorRect = r;
}

//------------------------------------------------------------------------------
/**
    Set user defined clip planes in clip space. Clip space is where
    outgoing vertex shader vertices live in. Up to 6 clip planes
    can be defined. Provide an empty array to clear all clip planes.
*/
void
nGfxServer2::SetClipPlanes(const nArray<plane>& planes)
{
    this->clipPlanes = planes;
}

//------------------------------------------------------------------------------
/**
    Set the value of a boolean render option
*/
void
nGfxServer2::SetBoolRenderOption( RenderOption option, bool value )
{
    RenderState& state( this->renderStateStack.Back() );
    state.boolRenderOptions[ option ] = value;
}

//------------------------------------------------------------------------------
/**
    Get the value of a boolean render option
*/
bool
nGfxServer2::GetBoolRenderOption( RenderOption option ) const
{
    RenderState& state( this->renderStateStack.Back() );
    return state.boolRenderOptions[ option ];
}

//------------------------------------------------------------------------------
/**
    Set the value of a float render option
*/
void
nGfxServer2::SetFloatRenderOption( RenderOption option, float value )
{
    RenderState& state( this->renderStateStack.Back() );
    state.floatRenderOptions[ option ] = value;
}

//------------------------------------------------------------------------------
/**
    Get the value of a float render option
*/
float
nGfxServer2::GetFloatRenderOption( RenderOption option ) const
{
    RenderState& state( this->renderStateStack.Back() );
    return state.floatRenderOptions[ option ];
}

//------------------------------------------------------------------------------
/**
    Set the value of an integer render option
*/
void
nGfxServer2::SetIntRenderOption( RenderOption option, int value )
{
    RenderState& state( this->renderStateStack.Back() );
    state.intRenderOptions[ option ] = value;
}

//------------------------------------------------------------------------------
/**
    Get the value of an integer render option
*/
int
nGfxServer2::GetIntRenderOption( RenderOption option ) const
{
    RenderState& state( this->renderStateStack.Back() );
    return state.intRenderOptions[ option ];
}

//------------------------------------------------------------------------------
/**
    Push current render state
*/
void
nGfxServer2::PushRenderState()
{
    RenderState& state( this->renderStateStack.Back() );
    this->renderStateStack.PushBack( state );
}

//------------------------------------------------------------------------------
/**
    Pop current render state
*/
void
nGfxServer2::PopRenderState()
{
    // Make sure that there's always at least one render state on the stack
    if ( this->renderStateStack.Size() > 1 )
    {
        this->renderStateStack.Erase( this->renderStateStack.Size() - 1 );
    }
}

//------------------------------------------------------------------------------
/**
    Set the render state default values
*/
void
nGfxServer2::SetDefaultRenderState()
{
    RenderState& state( this->renderStateStack.Back() );
    state.boolRenderOptions[ ZTestEnable ] = true;
    state.boolRenderOptions[ ZWriteEnable ] = true;
    state.floatRenderOptions[ LineWidth ] = 1.0f;
    state.intRenderOptions[ ZTestFunc ] = LessEqual;
}
