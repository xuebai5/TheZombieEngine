#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrenderpathnode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nrenderpathnode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nscenegraph.h"
#include "nscene/nsceneshader.h"
#include "nscene/nshadertree.h"
#include "nscene/ncscene.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ncscenelight.h"

#include "kernel/ntypes.h"
#include "kernel/nlogclass.h"
#include "gfx2/ngfxserver2.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nRenderPathNode, "nscenenode");

NCREATELOGLEVEL(renderpass, "Scene Passes", false, 3)
// log levels:
// 0 - high-level per-frame info: frameid, numpasses, numshapes
// 1 - detailed per-pass info: numshapes, 
// 2 - detailed per-bucket info: shader, technique
// 3 - detailed per-shape info: shader, shaderpass, surface, shape

#ifdef __NEBULA_STATS__
nArg *nRenderPathNode::arrayNumShaderChangesByPass = 0;
nArg *nRenderPathNode::arrayNumSurfaceChangesByPass = 0;
nArg *nRenderPathNode::arrayNumGeometryChangesByPass = 0;
#endif

nSceneGraph *nRenderPathNode::curSceneGraph = 0;
vector3 nRenderPathNode::viewerPos;
nRpPhase::SortingOrder nRenderPathNode::curSortingOrder;

//------------------------------------------------------------------------------
/**
*/
nRenderPathNode::nRenderPathNode() :
    viewPassIndex(-1),
    lghtPassIndex(-1),
    lightsEnabled(false),
    obeyLightLinks(true),
    maxMaterialLevel(0),
    errorShaderIndex(-1),
    lightArray(16, 16),
    sequenceShaderIndices(16, 16)
    #if __NEBULA_STATS__
   ,profSort("profRenderPathSort", true),
    profRender("profRenderPathRender", true),
    profBeginPass("profRenderPathBeginPass", true),
    profBeginPhase("profRenderPathBeginPhase", true),
    profShaderIndex("profRenderPathShaderIndex", true),
    profRenderLights("profRenderPathRenderLights", true),
    profRenderLightNodes("profRenderPathRenderLightNodes", true),
    profApplySurface("profRenderPathApplySurface", true),
    profApplyGeometry("profRenderPathApplyGeometry", true),
    profRenderSurface("profRenderPathRenderSurface", true),
    profRenderGeometry("profRenderPathRenderGeometry", true),
    dbgNumShaderChanges("sceneNumShaderChanges", nArg::Int),
    dbgNumSurfaceChanges("sceneNumSurfaceChanges", nArg::Int),
    dbgNumGeometryChanges("sceneNumGeometryChanges", nArg::Int),
    dbgNumShaderChangesByPass("sceneNumShaderChangesByPass", nArg::List),
    dbgNumSurfaceChangesByPass("sceneNumSurfaceChangesByPass", nArg::List),
    dbgNumGeometryChangesByPass("sceneNumGeometryChangesByPass", nArg::List),
    dbgGeometrySize("sceneGeometrySize", nArg::Int),
    dbgTextureSize("sceneTextureSize", nArg::Int),
    statsLevel(0),
    statsLevelVarHandle(nVariable::InvalidHandle)
    #endif
    #ifndef NGAME
   ,obeyLightLinksVarHandle(nVariable::InvalidHandle),
    maxMaterialLevelVarHandle(nVariable::InvalidHandle)
    #endif
{
    #if __NEBULA_STATS__
    if (arrayNumShaderChangesByPass == 0)
    {
        arrayNumShaderChangesByPass = n_new_array(nArg, MaxScenePasses);
        this->dbgNumShaderChangesByPass->SetL(arrayNumShaderChangesByPass, MaxScenePasses);
        arrayNumSurfaceChangesByPass = n_new_array(nArg, MaxScenePasses);
        this->dbgNumSurfaceChangesByPass->SetL(arrayNumSurfaceChangesByPass, MaxScenePasses);
        arrayNumGeometryChangesByPass = n_new_array(nArg, MaxScenePasses);
        this->dbgNumGeometryChangesByPass->SetL(arrayNumGeometryChangesByPass, MaxScenePasses);
    }
    #endif
}

//------------------------------------------------------------------------------
/**
*/
nRenderPathNode::~nRenderPathNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nRenderPathNode::LoadResources()
{
    if (!this->renderPath.IsOpen())
    {
        // parse renderpath XML file
        if (this->renderPath.OpenXml())
        {
            // initialize the render path object
            N_IFDEF_ASSERTS(bool renderPathOpened =) this->renderPath.Open();
            n_assert(renderPathOpened);

            // unload the XML doc
            this->renderPath.CloseXml();
        }
        else
        {
            n_error("nRenderPathNode could not open render path file '%s'!", this->renderPath.GetFilename().Get());
            return false;
        }
    }

    // initialize fixed array of sequence shaders
    this->shapeBucket.SetSize(this->renderPath.GetNumPhases());
    this->sequenceBucket.SetSize(this->renderPath.GetNumPhases());
    this->sequenceIndices.SetSize(this->renderPath.GetNumSequences());

    #ifdef NGAME
    // use silent (black) error shader for game mode
    this->errorShaderIndex = nSceneServer::Instance()->FindShader("default");
    if (this->errorShaderIndex == -1)
    {
        nShader2* defaultShader = nGfxServer2::Instance()->NewShader("shaders:default_dept.fx");
        n_assert(defaultShader);
        if (!defaultShader->IsLoaded())
        {
            defaultShader->SetFilename("shaders:default.fx");
            n_verify(defaultShader->Load());
        }

        nSceneShader newSceneShader(defaultShader);
        newSceneShader.SetShader("shaders:default_dept.fx");
        newSceneShader.SetShaderName("default");
        this->errorShaderIndex = nSceneServer::Instance()->AddShader(newSceneShader);
    }
    #else
    // load error shader for editor
    nSceneShader& errorShader = nSceneServer::Instance()->GetErrorShader();
    this->errorShaderIndex = errorShader.GetShaderIndex();
    #endif

    return nSceneNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nRenderPathNode::UnloadResources()
{
    nSceneNode::UnloadResources();
    if (this->renderPath.IsOpen())
    {
        this->renderPath.Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::EntityCreated(nEntityObject* entityObject)
{
    nSceneNode::EntityCreated(entityObject);

    // render path is required to have been loaded
    n_verify(this->LoadResources());

    // copy default variables to the entity dictionary
    ncDictionary* dictionary = entityObject->GetComponent<ncDictionary>();
    n_assert(dictionary);
    const nVariableContext& varContext = this->renderPath.GetVariableContext();
    int i;
    for (i = 0; i < varContext.GetNumVariables(); ++i)
    {
        // do not overwrite existing variables -they could have been persisted
        nVariable& var = varContext.GetVariableAt(i);
        if (!dictionary->GetVariable(var.GetHandle()))
        {
            dictionary->AddVariable(var);
        }
    }

    // empty the runtime structures
    this->sequenceShaderIndices.Clear();

    #ifdef __NEBULA_STATS__
    this->statsLevelVarHandle = nVariableServer::Instance()->GetVariableHandleByName("statsLevel");
    if (!dictionary->GetVariable(this->statsLevelVarHandle))
    {
        dictionary->AddVariable(nVariable(this->statsLevelVarHandle, this->statsLevel));
    }
    #endif

    #ifndef NGAME
    // create a variable to override the obeyLightLinks attribute
    this->obeyLightLinksVarHandle = nVariableServer::Instance()->GetVariableHandleByName("obeyLightLinks");
    if (!dictionary->GetVariable(this->obeyLightLinksVarHandle))
    {
        dictionary->AddVariable(nVariable(this->obeyLightLinksVarHandle, this->obeyLightLinks));
    }
    // create a variable to override the maxMaterialLevel attribute
    this->maxMaterialLevelVarHandle = nVariableServer::Instance()->GetVariableHandleByName("maxMaterialLevel");
    if (!dictionary->GetVariable(this->maxMaterialLevelVarHandle))
    {
        dictionary->AddVariable(nVariable(this->maxMaterialLevelVarHandle, this->maxMaterialLevel));
    }
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::EntityDestroyed(nEntityObject* entityObject)
{
    nSceneNode::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (this->lghtPassIndex == -1)
    {
        this->lghtPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('lght'));
    }

    #ifndef NGAME
    // UGLY HACK- per-viewport max material level is required at Attach of material nodes
    ncDictionary* varContext = entityObject->GetComponentSafe<ncDictionary>();
    this->maxMaterialLevel = varContext->GetVariable(this->maxMaterialLevelVarHandle)->GetInt();
    sceneGraph->SetMaxMaterialLevel(this->maxMaterialLevel);
    #endif

    nSceneNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
    This implements the complete render path scene rendering. A render
    path is made of a shader hierarchy of passes, phases and sequences, designed
    to eliminate redundant shader state switches as much as possible.

    @todo Allow overriding render path variables from render context.
*/
void
nRenderPathNode::DoRenderPath(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    ncScene* renderContext = entityObject->GetComponent<ncScene>();
    n_assert(renderContext);

    #ifndef NGAME
    // allow overriding the value of this member before resolving materials
    ncDictionary* varContext = entityObject->GetComponentSafe<ncDictionary>();
    this->obeyLightLinks = varContext->GetVariable(this->obeyLightLinksVarHandle)->GetBool();
    this->maxMaterialLevel = varContext->GetVariable(this->maxMaterialLevelVarHandle)->GetInt();

    #if __NEBULA_STATS__
    this->statsLevel = varContext->GetVariable(this->statsLevelVarHandle)->GetInt();
    
    // reset stats counters
    this->statsNumShaderChanges = 0;
    this->statsNumSurfaceChanges = 0;
    this->statsNumGeometryChanges = 0;
    this->statsGeometrySize = 0;
    this->statsTextureSize = 0;

    // reset all pass counters
    int i;
    nArg zeroArg;
    zeroArg.SetI(0);
    for (i = 0; i < MaxScenePasses; ++i) arrayNumShaderChangesByPass[i].SetI(0);
    for (i = 0; i < MaxScenePasses; ++i) arrayNumSurfaceChangesByPass[i].SetI(0);
    for (i = 0; i < MaxScenePasses; ++i) arrayNumGeometryChangesByPass[i].SetI(0);

    if (this->statsLevel)
    {
        // reset resource counters
        this->BeginResourceStats();
    }
    #endif
    #endif

    uint numPasses = this->renderPath.Begin();

    NLOG(renderpass, (0, "--------------------------------------------------------------------------------"))
    NLOG(renderpass, (0, "nRenderPathNode::DoRenderPath(frameid: %u, numpasses: %i)", renderContext->GetFrameId(), numPasses))

    N_IFDEF_NLOG(nString dbgstr);
    N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::DoRenderPath(frameid: %u, numpasses: %i)\n", renderContext->GetFrameId(), numPasses));
    N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));

    // sort the lights by priority
    this->SortLights(sceneGraph);

    uint passIndex;
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        // for each pass...
        nRpPass& curPass = this->renderPath.GetPass(passIndex);

        NLOG(renderpass, (0, "nRenderPathNode::BeginPass(pass: '%s')", curPass.GetName().Get()))

        N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::BeginPass(pass: '%s')\n", curPass.GetName().Get()));
        N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));

        #if __NEBULA_STATS__
        this->profBeginPass.Start();
        #endif
        uint numPhases = curPass.Begin(sceneGraph, entityObject);
        #if __NEBULA_STATS__
        this->profBeginPass.Stop();
        #endif

        uint phaseIndex;
        for (phaseIndex = 0; phaseIndex < numPhases; phaseIndex++)
        {
            // for each phase...
            nRpPhase& curPhase = curPass.GetPhase(phaseIndex);
            uint passIndex = curPhase.GetPassIndex();

            // @todo pass disabled from render context
            if (renderContext->GetPassEnabledFlags() & (1<<passIndex))
            {
                #if __NEBULA_STATS__
                this->profSort.StartAccum();
                #endif

                // sort scene graph pass for this phase
                this->SortPhase(curPhase, sceneGraph);

                #if __NEBULA_STATS__
                this->profSort.StopAccum();
                this->profRender.StartAccum();
                #endif

                // render current path phase
                if (sceneGraph->BeginRender(passIndex))
                {
                    this->RenderPhase(curPhase, sceneGraph, entityObject);

                    sceneGraph->EndRender();
                }
                
                #if __NEBULA_STATS__
                this->profRender.StopAccum();
                #endif
            }
        }

        curPass.End();

        NLOG(renderpass, (0, "nRenderPathNode::EndPass(pass: '%s')", curPass.GetName().Get()))

        N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::EndPass(pass: '%s')\n", curPass.GetName().Get()));
        N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));
    }

    N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::EndRenderPath()\n"));
    N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));

    this->renderPath.End();

    #ifndef NGAME
    #if __NEBULA_STATS__
    if (this->statsLevel)
    {
        this->EndResourceStats();

        this->dbgNumShaderChanges->SetI(this->statsNumShaderChanges);
        this->dbgNumSurfaceChanges->SetI(this->statsNumSurfaceChanges);
        this->dbgNumGeometryChanges->SetI(this->statsNumGeometryChanges);

        this->dbgGeometrySize->SetI(this->statsGeometrySize);
        this->dbgTextureSize->SetI(this->statsTextureSize);
    }
    #endif
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::SortLights(nSceneGraph* sceneGraph)
{
    // first sort the light groups by priority
    if (!this->obeyLightLinks)
    {
        curSceneGraph = sceneGraph;
        sceneGraph->SortPass(this->lghtPassIndex, PrioritySorter);
    }

    NLOG(renderpass, (3, "nRenderPathNode::SortLights( self: %s )", sceneGraph->GetName()));

    // then (re)build a map with light ids to light indices in the pass
    if (sceneGraph->BeginRender(this->lghtPassIndex))
    {
        do
        {
            N_IFDEF_NLOG(char buf[5]);
            NLOG(renderpass, (3, " - light at: %d, entity: 0x%x (%s), type: '%s'",
                 sceneGraph->GetCurrentIndex(), sceneGraph->GetCurrentEntity()->GetId(),
                 sceneGraph->GetCurrentEntity()->GetClass()->GetName(),
                 nVariableServer::Instance()->FourCCToString(
                    ((nAbstractShaderNode*)sceneGraph->GetCurrentNode())->GetSelectorType(), buf, sizeof(buf))))

            // assume each light entity has a single light node
            // if this wasn't the case, make the index point to the first one
            nEntityObjectId lightKey = sceneGraph->GetCurrentEntity()->GetId();
            if (this->lightArray.HasKey(lightKey))
            {
                this->lightArray.GetElement(lightKey) = sceneGraph->GetCurrentIndex();
            }
            else
            {
                this->lightArray.Add(lightKey, sceneGraph->GetCurrentIndex());
            }
        }
        while (sceneGraph->Next());

        sceneGraph->EndRender();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::SortPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph)
{
    curSceneGraph = sceneGraph;
    curSortingOrder = curPhase.GetSortingOrder();

    int passIndex = curPhase.GetPassIndex();

    switch (curPhase.GetSortingOrder())
    {
    case nRpPhase::BackToFront:
    case nRpPhase::FrontToBack:
        // both sorting types share the same sorting function
        viewerPos = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView).pos_component();
        sceneGraph->SortPass(passIndex, DistanceSorter);
        break;
        
    case nRpPhase::LocalBoxBackToFront:
    case nRpPhase::LocalBoxFrontToBack:
        // both sorting types share the same sorting function
        viewerPos = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView).pos_component();
        sceneGraph->SortPass(passIndex, LocalBoxDistanceSorter);
        break;
        
    case nRpPhase::None:
    default:
        sceneGraph->SortPass(passIndex, SurfaceSorter);
        break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPathNode::RenderPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    ncScene* renderContext = entityObject->GetComponentSafe<ncScene>();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    N_IFDEF_NLOG(char buf[5]);
    N_IFDEF_NLOG(const char* passStr = nVariableServer::FourCCToString(curPhase.GetFourCC(), buf, sizeof(buf));)

    N_IFDEF_NLOG(nString dbgstr);
    N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::BeginPhase(phase: '%s', fourcc: '%s')\n", curPhase.GetName().Get(), passStr));
    N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));

    #if __NEBULA_STATS__
    this->profBeginPhase.StartAccum();
    #endif
    N_IFDEF_NLOG(uint numBuckets =) this->BeginPhase(curPhase, sceneGraph, entityObject);
    #if __NEBULA_STATS__
    this->profBeginPhase.StopAccum();
    #endif

    curPhase.Begin();

    N_IFDEF_NLOG(int totalNumShapes = 0);
    N_IFDEF_NLOG(int numSurfaceChanges = 0);
    N_IFDEF_NLOG(int numGeometryChanges = 0);

    // get the bucket of shapes grouped by shader for this phase
    nShapeBucket& shapeBucket = this->shapeBucket[curPhase.GetShaderBucketIndex()];

    // get the bucket of shaders grouped by sequence for this phase
    nShapeBucket& sequenceBucket = this->sequenceBucket[curPhase.GetShaderBucketIndex()];

    // render the sequences in the phase
    int numSeqBuckets = sequenceBucket.Size();
    for (int seqIndex = 0; seqIndex < numSeqBuckets; ++seqIndex)
    {
        const nArray<int>& shaderBucket = sequenceBucket.GetShapeArrayAt(seqIndex);
        if (shaderBucket.Empty())
        {
            continue;
        }

        // begin sequence
        int sequenceIndex = this->sequenceIndices[sequenceBucket.GetShaderIndexAt(seqIndex)];
        nRpSequence& curSequence = curPhase.GetSequence(sequenceIndex);

        NLOG(renderpass, (2, "BeginSequence(pass: %s, sequence: %u (%s), numShaders: %u)",
             passStr, seqIndex, curSequence.GetShaderAlias(), shaderBucket.Size()))

        curSequence.Begin();
        
        // render sequence
        for (int index = 0; index < shaderBucket.Size(); ++index)
        {
            int bucketIndex = shaderBucket[index];
            int shaderIndex = shapeBucket.GetShaderIndexAt(bucketIndex);
            const nArray<int>& shapeArray = shapeBucket.GetShapeArrayAt(bucketIndex);

            int numShapes = shapeArray.Size();
            if (numShapes > 0)
            {
                N_IFDEF_NLOG(totalNumShapes += numShapes;)

                // set shader for current shape bucket
                n_assert(shaderIndex != -1);
                nSceneShader& sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
                if (!sceneShader.IsValid())
                {
                    // load shader first time it is needed
                    sceneShader.Validate();
                }
                nShader2* curShader = sceneShader.GetShaderObject();

                // check that the shader hasn't been invalidated
                if (!curShader)
                {
                    NLOG(resource, (NLOGUSER | 0, "nRenderPathNode::RenderPhase()- invalid bucket shader '%s'", sceneShader.GetShader()));
                    continue;
                }

                NLOG(renderpass, (2, "pass: %s, shader: '%s', technique: %s, numShapes: %i",
                    passStr, curShader->GetFilename().Get(), sceneShader.GetTechnique(), numShapes))
                NLOG(renderpass, (3, "{"))

                int numPasses = sceneShader.Begin();
                int curPass;
                for (curPass = 0; curPass < numPasses; curPass++)
                {
                    curShader->BeginPass(curPass);

                    #if __NEBULA_STATS__
                    this->profRender.StopAccum();
                    #endif

                    nGeometryNode* prevGeometry = 0;
                    nSurfaceNode* prevSurface = 0;
                    bool renderLights = true;

                    int shapeIndex;
                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                    {
                        sceneGraph->SetCurrentIndex(shapeArray[shapeIndex]);

                        // perform pre-instancing rendering
                        nGeometryNode *curGeometry = (nGeometryNode *) sceneGraph->GetCurrentNode();
                        nSurfaceNode *curSurface = curGeometry->GetSurfaceNode();
                        nEntityObject *curEntity = (nEntityObject *) sceneGraph->GetCurrentEntity();

                        if (curSurface != prevSurface)
                        {
                            // flush geometry stream when switching surfaces
                            if (prevGeometry)
                            {
                                NLOG(renderpass, (3, "  Flush geometry: %s", prevGeometry->GetFullName().Get()))

                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StartAccum();
                                #endif
                                prevGeometry->Flush(sceneGraph);
                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StopAccum();
                                #endif
                                prevGeometry = 0;
                            }

                            NLOG(renderpass, (3, "  Apply surface: %s, entity: %x, class: %s", curSurface->GetName(), curEntity->GetId(), curEntity->GetClass()->GetName()))

                            #if __NEBULA_STATS__
                            this->profApplySurface.StartAccum();
                            #endif
                            curSurface->Apply(sceneGraph); // == ApplyShader
                            #if __NEBULA_STATS__
                            this->profApplySurface.StopAccum();
                            this->CollectTextureStats(curSurface);
                            #endif

                            N_IFDEF_NLOG(++numSurfaceChanges);
                            prevSurface = curSurface;
                        }

                        if (curGeometry != prevGeometry)
                        {
                            // flush geometry stream when switching shapes
                            if (prevGeometry && prevGeometry->GetStreamId() != curGeometry->GetStreamId())
                            {
                                NLOG(renderpass, (3, "  Flush geometry: %s", prevGeometry->GetFullName().Get()))

                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StartAccum();
                                #endif
                                prevGeometry->Flush(sceneGraph);
                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StopAccum();
                                #endif

                                prevGeometry = 0;
                            }

                            NLOG(renderpass, (3, "  Apply geometry: %s, entity: %x, class: %s", curGeometry->GetFullName().Get(), curEntity->GetId(), curEntity->GetClass()->GetName()))

                            if (!prevGeometry || prevGeometry->GetStreamId() != curGeometry->GetStreamId())
                            {
                                #if __NEBULA_STATS__
                                this->profApplyGeometry.StartAccum();
                                #endif
                                curGeometry->Apply(sceneGraph); // == ApplyGeometry
                                #if __NEBULA_STATS__
                                this->profApplyGeometry.StopAccum();
                                this->CollectGeometryStats(curGeometry);
                                this->CollectTextureStats(curGeometry);
                                #endif
                                N_IFDEF_NLOG(++numGeometryChanges);
                            }

                            prevGeometry = curGeometry;
                            renderLights = true;
                        }
                        else if (curGeometry->GetWorldCoord())
                        {
                            // for geometry in world coordinates, render lights for the first instance only
                            renderLights = false;
                        }

                        // perform per-instance rendering
                        if (!curGeometry->GetWorldCoord())
                        {
                            if (curEntity->GetComponentSafe<ncTransform>()->GetLockViewer())
                            {
                                // handle lock to viewer
                                const matrix44& viewMatrix = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
                                matrix44 m = sceneGraph->GetModelTransform();
                                m.set_translation(viewMatrix.pos_component());
                                gfxServer->SetTransform(nGfxServer2::Model, m);
                            }
                            else
                            {
                                gfxServer->SetTransform(nGfxServer2::Model, sceneGraph->GetModelTransform());
                            }
                        }

                        // per-instance surface rendering
                        #if __NEBULA_STATS__
                        this->profRenderSurface.StartAccum();
                        #endif
                        curSurface->Render(sceneGraph, curEntity);
                        #if __NEBULA_STATS__
                        this->profRenderSurface.StopAccum();
                        #endif

                        // apply viewport overrides after lights
                        curShader->SetParams(renderContext->GetShaderOverrides());

                        if (curPhase.GetLightModeFlags() & nRpPhase::RenderOncePerLight)
                        {
                            // render geometry once for each linked light, if any
                            nArray<int>& lightArray = this->shapeLightArray.At(sceneGraph->GetCurrentIndex());
                            int lightIndex;
                            for (lightIndex = 0; lightIndex < lightArray.Size(); ++lightIndex)
                            {
                                if (sceneGraph->BeginRender(this->lghtPassIndex))
                                {
                                    sceneGraph->SetCurrentIndex(lightArray[lightIndex]);

                                    const nAbstractShaderNode* node = static_cast<const nAbstractShaderNode*>(sceneGraph->GetCurrentNode());
                                    if (node->GetPassEnabledFlags() & (1<<curPhase.GetPassIndex()))
                                    {
                                        #if __NEBULA_STATS__
                                        this->profRenderLightNodes.StartAccum();
                                        #endif
                                        sceneGraph->RenderCurrent();
                                        #if __NEBULA_STATS__
                                        this->profRenderLightNodes.StopAccum();
                                        #endif

                                        NLOG(renderpass, (3, "  - light: %x, cell: %i, class: %s", sceneGraph->GetCurrentEntity()->GetId(), sceneGraph->GetCurrentLod(), sceneGraph->GetCurrentEntity()->GetClass()->GetName()));
                                    }

                                    sceneGraph->EndRender();
                                }

                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StartAccum();
                                #endif
                                curGeometry->Render(sceneGraph, curEntity);
                                #if __NEBULA_STATS__
                                this->profRenderGeometry.StopAccum();
                                #endif

                                NLOG(renderpass, (3, "  - Render geometry: %s, entity: %x, class: %s", curGeometry->GetFullName().Get(), curEntity->GetId(), curEntity->GetClass()->GetName()))
                            }
                        }
                        else
                        {
                            // render geometry once for all lights
                            if (renderLights)
                            {
                                nArray<int>& lightArray = this->shapeLightArray.At(sceneGraph->GetCurrentIndex());
                                int numLights = lightArray.Size();
                                if (numLights > 0)
                                {
                                    if (sceneGraph->BeginRender(this->lghtPassIndex))
                                    {
                                        curShader->ResetParameterSlots();

                                        for (int index = 0; index < numLights; ++index)
                                        {
                                            sceneGraph->SetCurrentIndex(lightArray[index]);
                                            
                                            const nAbstractShaderNode* node = static_cast<const nAbstractShaderNode*>(sceneGraph->GetCurrentNode());
                                            if (node->GetPassEnabledFlags() & (1<<curPhase.GetPassIndex()))
                                            {
                                                #if __NEBULA_STATS__
                                                this->profRenderLightNodes.StartAccum();
                                                #endif

                                                if (curShader->BeginParameterSlot())
                                                {
                                                    sceneGraph->RenderCurrent();
                                                    curShader->EndParameterSlot();
                                                }

                                                #if __NEBULA_STATS__
                                                this->profRenderLightNodes.StopAccum();
                                                #endif

                                                NLOG(renderpass, (3, "  - light: %x, cell: %i, class: %s", sceneGraph->GetCurrentEntity()->GetId(), sceneGraph->GetCurrentLod(), sceneGraph->GetCurrentEntity()->GetClass()->GetName()));
                                            }
                                        }
                                        sceneGraph->EndRender();
                                    }
                                }
                            }

                            #if __NEBULA_STATS__
                            this->profRenderGeometry.StartAccum();
                            #endif
                            curGeometry->Render(sceneGraph, curEntity);
                            #if __NEBULA_STATS__
                            this->profRenderGeometry.StopAccum();
                            #endif

                            NLOG(renderpass, (3, "  - Render geometry: %s, entity: %x, class: %s", curGeometry->GetFullName().Get(), curEntity->GetId(), curEntity->GetClass()->GetName()))
                        }
                    }

                    if (prevGeometry)
                    {
                        #if __NEBULA_STATS__
                        this->profRenderGeometry.StartAccum();
                        #endif
                        prevGeometry->Flush(sceneGraph);
                        #if __NEBULA_STATS__
                        this->profRenderGeometry.StopAccum();
                        #endif

                        NLOG(renderpass, (3, "  Flush geometry: %s", prevGeometry->GetFullName().Get()))
                    }

                    sceneShader.GetShaderObject()->EndPass();

                    #if __NEBULA_STATS__
                    this->profRender.StartAccum();
                    #endif
                }

                NLOG(renderpass, (3, "}"))

                sceneShader.End();
            }
        }

        // end the current sequence
        curSequence.End();

        NLOG(renderpass, (2, "EndSequence(pass: %s, %u (%s))", passStr, seqIndex, curSequence.GetShaderAlias()))
            
    }

    curPhase.End();

    N_IFDEF_NLOG(dbgstr.Format("nRenderPathNode::EndPhase(phase: '%s')\n", curPhase.GetName().Get()));
    N_IFDEF_NLOG(N_OUTPUTDEBUGSTRING(dbgstr.Get()));

    NLOG(renderpass, (1, "numbuckets: %i, numshapes: %i, "
                         "numSurfaceChanges: %i, numGeometryChanges: %i",
                         numBuckets, totalNumShapes, numSurfaceChanges, numGeometryChanges));

    #ifdef __NEBULA_STATS__
    if (this->statsLevel)
    {
        arrayNumShaderChangesByPass[curPhase.GetPassIndex()].SetI(numBuckets);
        arrayNumSurfaceChangesByPass[curPhase.GetPassIndex()].SetI(numSurfaceChanges);
        arrayNumGeometryChangesByPass[curPhase.GetPassIndex()].SetI(numGeometryChanges);

        N_IFDEF_NLOG(this->statsNumShaderChanges += numBuckets);
        N_IFDEF_NLOG(this->statsNumSurfaceChanges += numSurfaceChanges);
        N_IFDEF_NLOG(this->statsNumGeometryChanges += numGeometryChanges);
    }
    #endif
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Render()
    Sort geometry nodes by its surface.
*/
int
__cdecl
nRenderPathNode::PrioritySorter(const int *i1, const int *i2)
{
    nSceneNode *g1 = (nSceneNode *) curSceneGraph->GetGroupNodeByIndex(*i1);
    nSceneNode *g2 = (nSceneNode *) curSceneGraph->GetGroupNodeByIndex(*i2);

    int diff = g1->GetRenderPri() - g2->GetRenderPri();
    if (!diff)
    {
        // keep sorted by entity id
        nEntityObject* entity1 = (nEntityObject*) curSceneGraph->GetGroupContextByIndex(*i1);
        nEntityObject* entity2 = (nEntityObject*) curSceneGraph->GetGroupContextByIndex(*i2);
        diff = (int) (entity1 - entity2);
    }
    return diff;
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Render()
    Sort geometry nodes by its surface.
*/
int
__cdecl
nRenderPathNode::SurfaceSorter(const int *i1, const int *i2)
{
    nGeometryNode *g1 = (nGeometryNode *) curSceneGraph->GetGroupNodeByIndex(*i1);
    nGeometryNode *g2 = (nGeometryNode *) curSceneGraph->GetGroupNodeByIndex(*i2);

    nptrdiff diff = g1->GetSurfaceNode() - g2->GetSurfaceNode();
    if (!diff)
    {
        // if using same surface, sort by identical geometry
        diff = g1->GetStreamId() - g2->GetStreamId();
    }
    return int(diff);
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Render()
    Sort nodes by distance to viewer.
*/
int
__cdecl
nRenderPathNode::DistanceSorter(const int *i1, const int *i2)
{
    const matrix44& modelTransform1 = curSceneGraph->GetGroupModelTransformByIndex(*i1);
    const matrix44& modelTransform2 = curSceneGraph->GetGroupModelTransformByIndex(*i2);
    
    // distance to viewer (closest first)
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - modelTransform1.M41,
              viewerPos.y - modelTransform1.M42,
              viewerPos.z - modelTransform1.M43);
    dist2.set(viewerPos.x - modelTransform2.M41,
              viewerPos.y - modelTransform2.M42,
              viewerPos.z - modelTransform2.M43);
    
    float diff = dist1.lensquared() - dist2.lensquared();
    if (curSortingOrder == nRpPhase::FrontToBack)
    {
        if (diff < 0.001f)
        {
            return -1;
        }
        else if (diff > 0.001f)
        {
            return +1;
        }
    }
    else if (curSortingOrder == nRpPhase::BackToFront)
    {
        if (diff < 0.001f)
        {
            return +1;
        }
        else if (diff > 0.001f)
        {
            return -1;
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Render()
    
    Sort nodes by distance to viewer, using the centroid of local boxes.
    More precise than normal distance sorter when scenes use world coordinates
    but much slower as well.
*/
int
__cdecl
nRenderPathNode::LocalBoxDistanceSorter(const int *i1, const int *i2)
{
    const matrix44& modelTransform1 = curSceneGraph->GetGroupModelTransformByIndex(*i1);
    const matrix44& modelTransform2 = curSceneGraph->GetGroupModelTransformByIndex(*i2);
    const bbox3& modelBox1 = curSceneGraph->GetGroupNodeByIndex(*i1)->GetLocalBox();
    const bbox3& modelBox2 = curSceneGraph->GetGroupNodeByIndex(*i2)->GetLocalBox();
    static vector3 center1 = modelTransform1 * modelBox1.center();
    static vector3 center2 = modelTransform2 * modelBox2.center();
    
    // distance to viewer (closest first)
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - center1.x,
              viewerPos.y - center1.y,
              viewerPos.z - center1.z);
    dist2.set(viewerPos.x - center2.x,
              viewerPos.y - center2.y,
              viewerPos.z - center2.z);
    
    float diff = dist1.lensquared() - dist2.lensquared();
    if (curSortingOrder == nRpPhase::LocalBoxFrontToBack)
    {
        if (diff < 0.001f)
        {
            return -1;
        }
        else if (diff > 0.001f)
        {
            return +1;
        }
    }
    else if (curSortingOrder == nRpPhase::LocalBoxBackToFront)
    {
        if (diff < 0.001f)
        {
            return +1;
        }
        else if (diff > 0.001f)
        {
            return -1;
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------
/**
    for all geometry nodes within the current phase, determine its shader index
    using the available lights for the shape
    TODO- if the lights affecting each entity didn't change since last frame,
    skip this completely and use the shader index for the group from last frame.
*/
int
nRenderPathNode::BeginPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    N_IFDEF_NLOG(char buf[5];)
    N_IFDEF_NLOG(const char* passStr = nVariableServer::FourCCToString(curPhase.GetFourCC(), buf, sizeof(buf));)

    NLOG(scenephase, (0, "- - - - - - - - - - - - - - - - - - - - "))
    NLOG(scenephase, (0, "nRenderPathNode::BeginPhase(pass: %s)", passStr))

    // reset all lists of lights per shape
    this->shapeLightArray.Reset();

    // get shape and sequence bucket for the current phase
    int bucketIndex = curPhase.GetShaderBucketIndex();
    nShapeBucket& shapeBucket = this->shapeBucket[bucketIndex];
    nShapeBucket& sequenceBucket = this->sequenceBucket[bucketIndex];

    if (curPhase.GetSortingOrder() == nRpPhase::None || curPhase.GetSortingOrder() == nRpPhase::Surface)
    {
        // reset sequence and shape bucket for the phase
        sequenceBucket.Begin(true);
        shapeBucket.Begin(true);

        do
        {
            if (this->CheckPhaseFlags(curPhase, sceneGraph))
            {
                // here we can optimize (remove if not changed) GetShaderIndex;
                int shaderIndex = this->GetShaderIndex(curPhase, sceneGraph);
                if (shaderIndex != -1)
                {
                    // get shape array for the current shader
                    nArray<int>& shapeArray = shapeBucket.GetShapeArray(shaderIndex);

                    // add shader index to the sequence if this is its first shape
                    if (shapeArray.Empty())
                    {
                        int sequenceIndex = this->GetSequenceIndex(curPhase, shaderIndex);
                        bucketIndex = shapeBucket.Size() - 1;
                        nArray<int>& shaderArray = sequenceBucket.GetShapeArray(sequenceIndex);
                        shaderArray.Append(bucketIndex);
                        NLOG(scenephase, (1, "shaderIndex: %u, bucketIndex: %u, sequenceIndex: %u", shaderIndex, bucketIndex, sequenceIndex))
                    }

                    // add shape index to the shape array
                    shapeArray.Append(sceneGraph->GetCurrentIndex());
                    NLOG(scenephase, (1, "shaderIndex: %u, shapeIndex: %u", shaderIndex, sceneGraph->GetCurrentIndex()))
                }
            }
        }
        while (sceneGraph->Next());
    }
    else
    {
        // keep sorted the array, try to reuse shaders.
        // reset the shader array, keeping the list of shape arrays
        sequenceBucket.Begin(false);
        shapeBucket.Begin(false);

        int prevSequenceIndex = -1;
        int prevShaderIndex = -1;
        int bucketIndex = 0;
        int sequenceBucketIndex = 0;

        do
        {
            if (this->CheckPhaseFlags(curPhase, sceneGraph))
            {
                int curShaderIndex = this->GetShaderIndex(curPhase, sceneGraph);
                if (curShaderIndex != -1)
                {
                    if (curShaderIndex != prevShaderIndex)
                    {
                        bucketIndex = shapeBucket.Append(curShaderIndex);
                        prevShaderIndex = curShaderIndex;

                        // store index into the shape bucket for the sequence
                        int curSequenceIndex = this->GetSequenceIndex(curPhase, curShaderIndex);
                        if (curSequenceIndex != prevSequenceIndex)
                        {
                            sequenceBucketIndex = sequenceBucket.Append(curSequenceIndex);
                            prevSequenceIndex = curSequenceIndex;
                        }

                        nArray<int>& shaderArray = sequenceBucket.GetShapeArrayAt(sequenceBucketIndex);
                        shaderArray.Append(bucketIndex);
                        NLOG(scenephase, (1, "shaderIndex: %u, bucketIndex: %u, sequenceIndex: %u", curShaderIndex, bucketIndex, curSequenceIndex))
                    }

                    shapeBucket.GetShapeArrayAt(bucketIndex).Append(sceneGraph->GetCurrentIndex());
                    NLOG(scenephase, (1, "shaderIndex: %u, bucketIndex: %u, shapeIndex: %u", curShaderIndex, bucketIndex, sceneGraph->GetCurrentIndex()))
                }
            }
        }
        while (sceneGraph->Next());
    }

    // apply shader overrides from viewport or camera entity
    if (curPhase.GetShader())
    {
        ncScene* renderContext = entityObject->GetComponentSafe<ncScene>();
        curPhase.GetShader()->SetParams(renderContext->GetShaderOverrides());
    }

    NLOG(scenephase, (0, "numShapeBuckets: %i", shapeBucket.Size()))

    NLOG(renderpass, (1, "pass: %s, numShapeBuckets: %i", passStr, shapeBucket.Size()))

    return shapeBucket.Size();
}

//------------------------------------------------------------------------------
/**
*/
bool
nRenderPathNode::CheckPhaseFlags(nRpPhase& curPhase, nSceneGraph* sceneGraph)
{
    // check visible flags
    int curFlags = sceneGraph->GetCurrentFlags();

    if (!this->obeyLightLinks)
    {
        //HACK- force shadows to be visible when not using light visibility
        if (curPhase.GetLightModeFlags() & nRpPhase::CheckLightEntity)
        {
            return false;
        }

        if (curFlags & ncScene::ShapeVisible)
        {
            // HACK- prevent AlwaysOnTop geometry from being drawn twice in different passes
            //return (curPhase.GetVisibleFlags() & ncScene::AlwaysOnTop) ?
            //       (curFlags & ncScene::AlwaysOnTop) != 0 :
            //       (curFlags & ncScene::AlwaysOnTop) == 0;
            return true;
        }

        return false;
    }

    if (curFlags & curPhase.GetVisibleFlags())
    {
        if (curPhase.GetLightModeFlags() & nRpPhase::CheckLightEntity)
        {
            ncScene* curContext = (ncScene*) sceneGraph->GetCurrentEntity()->GetComponent<ncScene>();
            for (int linkIndex = 0; linkIndex < curContext->GetNumLinks(); ++linkIndex)
            {
                ncSceneLight* curLightLink = curContext->GetLinkAt(linkIndex)->GetComponent<ncSceneLight>();
                if (curLightLink)
                {
                    //check conditional rendering for all attached lights
                    nEntityObject* curEntity = (nEntityObject*) sceneGraph->GetCurrentEntity();
                    if (curLightLink->AffectsEntity(curEntity))
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else
        {
            // HACK- prevent AlwaysOnTop geometry from being drawn twice in different passes
            //return (curPhase.GetVisibleFlags() & ncScene::AlwaysOnTop) ?
            //       (curFlags & ncScene::AlwaysOnTop) != 0 :
            //       (curFlags & ncScene::AlwaysOnTop) == 0;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Resolve shader for the scene graph in its current state.
*/
int
nRenderPathNode::GetShaderIndex(nRpPhase& curPhase, nSceneGraph* sceneGraph)
{
    #if __NEBULA_STATS__
    this->profShaderIndex.StartAccum();
    #endif

    nGeometryNode *curGeometry = (nGeometryNode*) sceneGraph->GetCurrentNode();
    nEntityObject *curEntity = (nEntityObject*) sceneGraph->GetCurrentEntity();
    n_assert(curGeometry);
    n_assert(curEntity);

    nSurfaceNode* curSurface = curGeometry->GetSurfaceNode();
    n_assert(curSurface);

    ncScene* curContext = curEntity->GetComponentSafe<ncScene>();
    int level = n_max(curContext->GetMaxMaterialLevel(), sceneGraph->GetMaxMaterialLevel());
    int passIndex = sceneGraph->GetCurrentPassIndex();
    level = n_min(level, curSurface->GetNumLevels() - 1);
    nShaderTree* shaderTree = curSurface->GetShaderTree(level, passIndex);

    if (!shaderTree)
    {
        // if there is no shader tree, go with a default error shader
        return this->errorShaderIndex;
    }

    bool lightsEnabled = curPhase.GetLightsEnabled() &&
        (curContext->GetPassEnabledFlags() & curGeometry->GetPassEnabledFlags() & (1<<this->lghtPassIndex));

    // reset the array of lights, automatically grow the array if necessary
    nArray<int>& lightArray = this->shapeLightArray.At(sceneGraph->GetCurrentIndex());
    lightArray.Reset();

    if (this->obeyLightLinks)
    {
        if (lightsEnabled)
        {
            // HACK- use lights linked to parent entities
            while (curContext->GetParentEntity())
            {
                curContext = curContext->GetParentEntity()->GetComponent<ncScene>();
            }

            // traverse linked light entities
            int numLights = curContext->GetNumLinks();
            if (numLights > 0)
            {
                NLOG(scenephase, (2, "nRenderPathNode::GetShaderIndex( entity: 0x%x (%s), geometry: %s, surface: %s, numLinks: %d )",
                     curEntity->GetId(), curEntity->GetClass()->GetName(), curGeometry->GetFullName().Get(), curSurface->GetName(), numLights))

                ncSceneClass* sceneClass = curEntity->GetClassComponent<ncSceneClass>();

                // sort light links
                curContext->SortLinks();

                int lightIndex;
                for (lightIndex = 0; lightIndex < curContext->GetNumLinks(); ++lightIndex)
                {
                    nEntityObject* linkedLight = curContext->GetLinkAt(lightIndex);
                    if (linkedLight && this->lightArray.HasKey(linkedLight->GetId()))
                    {
                        ncSceneLight* sceneLight = linkedLight->GetComponent<ncSceneLight>();
                        if (sceneLight)
                        {
                            bool shapeInfluencedByLight(false);

                            // check if light entity influences the shape:
                            if (curPhase.GetVisibleFlags() & ncScene::ShadowVisible)
                            {
                                shapeInfluencedByLight = (sceneLight->GetAttachFlags() & ncScene::CastShadows) != 0;
                                NLOGCOND(scenephase, !shapeInfluencedByLight, (2, " - REJECTED light: 0x%x (%s), priority: %d (light is not casting shadows)",
                                         linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority()))
                            }
                            else if (sceneLight->GetCastLightmaps())
                            {
                                shapeInfluencedByLight = sceneClass->GetLightmapReceiver();
                                NLOGCOND(scenephase, !shapeInfluencedByLight, (2, " - REJECTED light: 0x%x (%s), priority: %d (entity is not lightmap receiver)",
                                         linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority()))
                            }
                            else
                            {
                                shapeInfluencedByLight = sceneClass->GetLightmapStatic() ? sceneLight->GetLightStatic() : sceneLight->GetLightDynamic();
                                NLOGCOND(scenephase, !shapeInfluencedByLight, (2, " - REJECTED light: 0x%x (%s), priority: %d (light static/dynamic flags)",
                                         linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority()))
                            }
                            
                            if (shapeInfluencedByLight)
                            {
                                // add the light index to the list of lights for the current shape
                                shapeInfluencedByLight = sceneLight->GetFrameId() == curContext->GetFrameId();
                                NLOGCOND(scenephase, !shapeInfluencedByLight, (2, " - ERROR light: 0x%x (%s), priority: %d (lightFrameId: %d != frameId: %d)",
                                         linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority(),
                                         sceneLight->GetFrameId(), curContext->GetFrameId()))
                            }

                            // if there are valid decision nodes in the tree
                            // get the selector types collected in the linked lights
                            // and set them in the shader tree, or reject the light if they don't fit
                            if (shapeInfluencedByLight && shaderTree->GetNumNodes() > 1 &&
                                (curPhase.GetLightModeFlags() & nRpPhase::RenderOncePerLight) == 0)
                            {
                                // find an empty slot for the type in the current tree
                                int numSelectorTypes = sceneLight->GetNumLightTypes();
                                for (int index = 0; index < numSelectorTypes; ++index)
                                {
                                    nFourCC fourcc = sceneLight->GetLightTypeAt(index);
                                    if (!shaderTree->SetSelectorType(fourcc))
                                    {
                                        NLOG(scenephase, (2, " - REJECTED light: 0x%x (%s), priority: %d (type not accepted in shader tree)",
                                             linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority()))
                                        shapeInfluencedByLight = false;
                                        break;
                                    }
                                }
                            }

                            if (shapeInfluencedByLight)
                            {
                                NLOG(scenephase, (2, " - ACCEPTED light: 0x%x (%s), priority: %d",
                                     linkedLight->GetId(), linkedLight->GetClass()->GetName(), sceneLight->GetLightPriority()))

                                int lightIndex = this->lightArray.GetElement(linkedLight->GetId());
                                lightArray.Append(lightIndex);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // per scene lighting
        if (lightsEnabled)
        {
            // collect light types directly from nodes
            if (sceneGraph->BeginRender(this->lghtPassIndex))
            {
                do
                {
                    nAbstractShaderNode* node = (nAbstractShaderNode*) sceneGraph->GetCurrentNode();
                    shaderTree->SetSelectorType(node->GetSelectorType());
                    lightArray.Append(sceneGraph->GetCurrentIndex());
                }
                while (sceneGraph->Next());
                sceneGraph->EndRender();
            }
        }
    }

    // geometry defaults to cached index if not invalidated at BeginPhaseLights
    int shaderIndex = shaderTree->GetCurrentShaderIndex();
    if (shaderIndex == -1)
    {
        shaderIndex = this->errorShaderIndex;
    }

    // try validating current shader when selected
    nSceneShader& sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
    while (!sceneShader.IsValid())
    {
        sceneShader.Validate();

        // if the shader thus selected is not valid, go up the decision tree until
        // a valid shader is found
        if (!sceneShader.IsValid())
        {
            shaderTree->InvalidateCurrent();

            //shaderIndex = shaderTree->GetParentShaderIndex();
            shaderIndex = shaderTree->GetCurrentShaderIndex();

            if (shaderIndex == -1)
            {
                shaderIndex = this->errorShaderIndex;
            }

            sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
        }
    }

    shaderTree->ResetCurrentNode();
/*
    /// @todo ma.garcias - try integrating the previous process into SelectShaderIndex()
    int shaderIndex = shaderTree->SelectShaderIndex();
    if (shaderIndex == -1)
    {
        shaderIndex = this->errorShaderIndex;
    }
*/
    sceneGraph->SetShaderIndex(shaderIndex);

    #if __NEBULA_STATS__
    this->profShaderIndex.StopAccum();
    #endif

    return shaderIndex;
}

//------------------------------------------------------------------------------
/**
    Resolve the sequence for a shader within the current phase.
    If the sequence is not found, a default sequence is returned.
*/
int
nRenderPathNode::GetSequenceIndex(nRpPhase& curPhase, int shaderIndex)
{
    if (!this->sequenceShaderIndices.HasKey(shaderIndex))
    {
        nSceneShader& sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
        int sequenceIndex = curPhase.FindSequenceIndex(sceneShader.GetSequence());
        nRpSequence& sequence = curPhase.GetSequence(sequenceIndex);
        // get global index of sequence in the render path
        int bucketIndex = sequence.GetShaderBucketIndex();
        // index global sequence index into sequence index (in phase)
        this->sequenceIndices[bucketIndex] = sequenceIndex;
        // register shader as belonging to the specific global sequence
        this->sequenceShaderIndices.Add(shaderIndex, bucketIndex);
    }

    return this->sequenceShaderIndices.GetElement(shaderIndex);
}

//------------------------------------------------------------------------------
/**
    Invokes rendering of the rest of scene into a selected viewport.
    The set of passes it renders can be customized through the render passes.
*/
bool
nRenderPathNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    nGfxServer2 *gfxServer = nGfxServer2::Instance();

    matrix44 viewMatrix = sceneGraph->GetModelTransform();
    viewMatrix.invert_simple();
    gfxServer->SetTransform(nGfxServer2::View, viewMatrix);

    this->DoRenderPath(sceneGraph, entityObject);

    return true;
}
