#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncviewport.h"
#include "nscene/nscenegraph.h"
#ifndef NGAME
#include "zombieentity/ncdictionary.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncViewport,ncScene);

//------------------------------------------------------------------------------
/**
*/
ncViewport::ncViewport()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncViewport::~ncViewport()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::InitInstance(nObject::InitInstanceMsg initType)
{
    ncScene::InitInstance(initType);
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::SetPerspective(float aov, float aspect, float nearp, float farp)
{
    this->camera.SetPerspective(aov, aspect, nearp, farp);
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::SetOrthogonal(float w, float h, float nearp, float farp)
{
    this->camera.SetOrthogonal(w, h, nearp, farp);
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::SetPassEnabled(nFourCC fourcc, bool enabled)
{
    //nFourCC fourcc = nVariableServer::StringToFourCC(pass);
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    if (enabled)
    {
        this->passEnabledFlags |= (1<<passIndex);
    }
    else
    {
        this->passEnabledFlags &= ~(1<<passIndex);
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncViewport::SetMaterialProfile(int level)
{
    this->GetComponentSafe<ncDictionary>()->SetIntVariable("maxMaterialLevel", level);
}

//------------------------------------------------------------------------------
/**
*/
int
ncViewport::GetMaterialProfile()
{
    return this->GetComponentSafe<ncDictionary>()->GetIntVariable("maxMaterialLevel");
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::SetBackgroundColor(const vector3& bgColor)
{
    this->GetComponentSafe<ncDictionary>()->SetVectorVariable("clearcolor", vector4(bgColor));
}

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::GetBackgroundColor(float& r, float& g, float& b)
{
    ncDictionary* varContext = this->GetComponentSafe<ncDictionary>();
    nVariable::Handle colorHandle = nVariableServer::Instance()->GetVariableHandleByName("clearcolor");
    nVariable* colorVar = varContext->GetVariable(colorHandle);
    if (colorVar)
    {
        const vector4& bgColor = colorVar->GetVector4();
        r = bgColor.x; g = bgColor.y; b = bgColor.z;
    }
    else
    {
        r = 0.5f; g = 0.5f; b = 0.5f;
    }
}
#endif

//------------------------------------------------------------------------------
/**
*/
void
ncViewport::Render(nSceneGraph *sceneGraph)
{
    ncTransform *transformComp = this->GetComponent<ncTransform>();
    sceneGraph->SetViewTransform(transformComp->GetTransform());
    ncScene::Render(sceneGraph);
}
