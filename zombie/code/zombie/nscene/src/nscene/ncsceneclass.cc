#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncsceneclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncsceneclass.h"
//N_GAME
//#include "nscene/ninstscenebuilder.h"

//NSIGNAL_DEFINE(ncSceneClass, ...);

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSceneClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneClass)
    NSCRIPT_ADDCMD_COMPCLASS('BSRN', void, SetRootNode, 1, (nSceneNode*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGRN', nSceneNode*, GetRootNode, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAE', void, BeginPlugs, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAF', void, AddPlug, 4, (int, char*, vector3, quaternion), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAAG', void, EndPlugs, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSPE', void, SetPassEnabled, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGPE', bool, GetPassEnabled, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSMP', void, SetMaxMaterialLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGMP', int, GetMaxMaterialLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLG', void, SetLightmapStatic, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLG', bool, GetLightmapStatic, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLH', void, SetLightmapReceiver, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLH', bool, GetLightmapReceiver, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSWT', void, SetAlwaysOnTop, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGWT', bool, GetAlwaysOnTop, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SVEC', void, SetVectorOverride, 2, (const char *, vector4), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GVEC', vector4, GetVectorOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SFLT', void, SetFloatOverride, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GFLT', float, GetFloatOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SINT', void, SetIntOverride, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GINT', int, GetIntOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('STEX', void, SetTextureOverride, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GTEX', const char *, GetTextureOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSIE', void, SetInstanceEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGIE', bool, GetInstanceEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MBIS', void, BeginInstanceStreamDecl, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MAIC', void, AddInstanceStreamComp, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MEIS', void, EndInstanceStreamDecl, 0, (), 0, ());
    #ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('CSPN', void, SetParticleNodeDummy, 1, (nObject *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGRN', nObject *, GetParticleNodeDummy, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSFD', void, SetFadeDistances, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGFD', void, GetFadeDistances, 0, (), 2, (float&, float&));
    #endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSceneClass::ncSceneClass() :
    passEnabledFlags(0xffffffff),
    maxMaterialLevel(0),
    geometryFlags(0),
    instanceEnabled(false),
    inBeginInstanceStreamDecl(false)
    #ifndef NGAME
   ,lastEditedTime(0)
    #endif //NGAME
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneClass::~ncSceneClass()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    for instanced classes, build an instanced scene resource
    using a nInstSceneBuilder and replace the root node.

    history:
        - 13-Mar-2006   ma.garcias  Added preloading of class resources
*/
bool
ncSceneClass::LoadResources()
{
    // preload class resources
    if (this->refRootNode.isvalid())
    {
        this->refRootNode->PreloadResources();
    }

    if (this->GetInstanceEnabled())
    {
        n_assert(this->refRootNode.isvalid());
        //const char *streamName = this->refRootNode->GetName();//TEMP!

        // load instance stream
        /*this->refInstanceStream = nGfxServer2::Instance()->NewInstanceStream(streamName);
        n_assert(this->refInstanceStream.isvalid());
        this->refInstanceStream->SetDeclaration(this->decl);
        if (!this->refInstanceStream->Load())
        {
            this->refInstanceStream->Release();
            this->refInstanceStream.invalidate();
        }*/
    }

    // load texture overrides
    if (this->textureArray.LoadResources())
    {
        this->shaderOverrides.SetParams(this->textureArray.GetShaderParams());
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::UnloadResources()
{
    // unload texture overrides
    this->textureArray.UnloadResources();

/*    if (this->refInstanceStream.isvalid())
    {
        this->refInstanceStream->Release();
        this->refInstanceStream.invalidate();
    }*/
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetMaxMaterialLevel(int level)
{
    this->maxMaterialLevel = level;
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneClass::GetMaxMaterialLevel() const
{
    return this->maxMaterialLevel;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetPassEnabled(const char *pass, bool enabled)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(pass);
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

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneClass::GetPassEnabled(const char *pass)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(pass);
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    return (this->passEnabledFlags & (1<<passIndex)) != 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetVectorOverride(const char *name, vector4 value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
vector4
ncSceneClass::GetVectorOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        const nFloat4& val = this->shaderOverrides.GetArg(param).GetFloat4();
        return vector4(val.x, val.y, val.z, val.w);
    }
    return vector4();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetFloatOverride(const char *name, float value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
float
ncSceneClass::GetFloatOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        return this->shaderOverrides.GetArg(param).GetFloat();
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetIntOverride(const char *name, int value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneClass::GetIntOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        return this->shaderOverrides.GetArg(param).GetInt();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetInstanceEnabled(bool enabled)
{
    this->instanceEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneClass::GetInstanceEnabled()
{
    return this->instanceEnabled;
}

//------------------------------------------------------------------------------
/**
    begin instance stream declaration
*/
void
ncSceneClass::BeginInstanceStreamDecl()
{
    n_assert(!this->inBeginInstanceStreamDecl);
    this->inBeginInstanceStreamDecl = true;
    this->decl.Reset();
}

//------------------------------------------------------------------------------
/**
    add a component to the instance stream declaration
*/
void
ncSceneClass::AddInstanceStreamComp(const char* paramName, const char* paramType)
{
    n_assert(this->inBeginInstanceStreamDecl);

    nShaderState::Param param = nShaderState::StringToParam(paramName);
    n_assert(param != nShaderState::InvalidParameter);

    nShaderState::Type type = nShaderState::StringToType(paramType);
    if (nShaderState::Void == type)
    {
        return;
    }

    // append instance stream component (if not exists yet)
    int i;
    bool paramExists = false;
    for (i = 0; i < decl.Size(); i++)
    {
        if (decl[i].GetParam() == param)
        {
            paramExists = true;
            break;
        }
    }
    if (!paramExists)
    {
        nInstanceStream::Component streamComponent(type, param);
        this->decl.Append(streamComponent);
    }
}

//------------------------------------------------------------------------------
/**
    end instance stream declaration
*/
void
ncSceneClass::EndInstanceStreamDecl()
{
    n_assert(this->inBeginInstanceStreamDecl);
    this->inBeginInstanceStreamDecl = false;
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetLastEditedTime(nTime time)
{
    this->lastEditedTime = this->IsValid() ? time : 0;
}

//------------------------------------------------------------------------------
/**
*/
nTime
ncSceneClass::GetLastEditedTime()
{
    return this->IsValid() ? this->lastEditedTime : 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetParticleNodeDummy(nObject *)
{
    // empty
}

nObject *
ncSceneClass::GetParticleNodeDummy() const
{
    nSceneNode* rootNode = this->GetRootNode();
    if (rootNode)
    {
        nSceneNode* node = static_cast<nSceneNode*>(rootNode->GetHead());
        if (node && (node->IsA("nparticleshapenode") || node->IsA("nparticleshapenode2")))
        {
            return node;
        }
    }
 
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::SetFadeDistances(float minDist, float maxDist)
{
    if (minDist == 0.0f && maxDist == 0.0f)
    {
        this->shaderOverrides.ClearArg(nShaderState::MinDist);
        this->shaderOverrides.ClearArg(nShaderState::MaxDist);
    }
    else
    {
        this->shaderOverrides.SetArg(nShaderState::MinDist, minDist);
        this->shaderOverrides.SetArg(nShaderState::MaxDist, maxDist);
    }
    this->SetLastEditedTime(nTimeServer::Instance()->GetFrameTime());
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneClass::GetFadeDistances(float& minDist, float& maxDist)
{
    if (this->shaderOverrides.IsParameterValid(nShaderState::MinDist) &&
        this->shaderOverrides.IsParameterValid(nShaderState::MaxDist))
    {
        minDist = this->shaderOverrides.GetArg(nShaderState::MinDist).GetFloat();
        maxDist = this->shaderOverrides.GetArg(nShaderState::MaxDist).GetFloat();
    }
}

#endif

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneClass::GetPlugData( const nString& plugName, vector3& position, quaternion& rotation)
{
    for(int i=0; i< this->plugArray.Size(); i++)
    {
        if ( this->plugArray[i].GetName() == plugName )
        {
            position = this->plugArray[i].GetTranslation();
            rotation = this->plugArray[i].GetRotation();
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneClass::SaveCmds(nPersistServer *ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        nCmd* cmd;

        nShaderParams& shaderOverrides = this->GetShaderOverrides();
        int paramIndex;
        int numParams = shaderOverrides.GetNumValidParams();
        for (paramIndex = 0; paramIndex < numParams; paramIndex++)
        {
            nCmd *cmd;

            nShaderState::Param param = shaderOverrides.GetParamByIndex(paramIndex);
            const nShaderArg& arg = shaderOverrides.GetArg(param);

            switch (arg.GetType())
            {
            case nShaderState::Float4:
                // --- setvectoroverride ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SVEC');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetF(arg.GetFloat4().x);
                cmd->In()->SetF(arg.GetFloat4().y);
                cmd->In()->SetF(arg.GetFloat4().z);
                cmd->In()->SetF(arg.GetFloat4().w);
                ps->PutCmd(cmd);
                break;

            case nShaderState::Float:
                // --- setfloatoverride ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SFLT');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetF(arg.GetFloat());
                ps->PutCmd(cmd);
                break;

            case nShaderState::Int:
                // --- setintoverride ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SINT');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetI(arg.GetInt());
                ps->PutCmd(cmd);
                break;
            }
        }

        // --- settextureoverride ---
        int tex;
        for (tex = 0; tex < this->textureArray.GetNumTextures(); ++tex)
        {
            nShaderState::Param param = this->textureArray.GetTextureParamAt(tex);
            cmd = ps->GetCmd(this->GetEntityClass(), 'STEX');
            cmd->In()->SetS(nShaderState::ParamToString(param));
            cmd->In()->SetS(this->textureArray.GetTextureAt(tex));
            ps->PutCmd(cmd);
        }

        // --- setpassenabled ---
        nSceneServer* sceneServer = nSceneServer::Instance();
        int numPasses = sceneServer->GetNumPasses();
        int passIndex;
        for (passIndex = 0; passIndex < numPasses; ++passIndex)
        {
            // passes are enabled by default, persist disabled only
            if ((this->passEnabledFlags & (1<<passIndex)) == 0)
            {
                char buf[5];
                cmd = ps->GetCmd(this->GetEntityClass(), 'MSPE');
                cmd->In()->SetS(nVariableServer::FourCCToString(sceneServer->GetPassAt(passIndex), buf, sizeof(buf)));
                cmd->In()->SetB(false);
                ps->PutCmd(cmd);
            }
        }

        // --- setinstanceenabled ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'MSIE');
        cmd->In()->SetB(this->GetInstanceEnabled());
        ps->PutCmd(cmd);

        // --- setmaxmateriallevel ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'MSMP');
        cmd->In()->SetI(this->GetMaxMaterialLevel());
        ps->PutCmd(cmd);

        // --- setlightmapstatic ---
        if (this->GetLightmapStatic())
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'MSLG');
            cmd->In()->SetB(this->GetLightmapStatic());
            ps->PutCmd(cmd);
        }

        // --- setlightmapreceiver ---
        if (this->GetLightmapReceiver())
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'MSLH');
            cmd->In()->SetB(this->GetLightmapReceiver());
            ps->PutCmd(cmd);
        }

        // --- setalwaysontop ---
        if (this->GetAlwaysOnTop())
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'MSWT');
            cmd->In()->SetB(this->GetAlwaysOnTop());
            ps->PutCmd(cmd);
        }

        // --- beginplugs ---
        int numPlugs = this->plugArray.Size();
        if (numPlugs != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAE');
            cmd->In()->SetI( numPlugs);
            ps->PutCmd(cmd);
        }

        // --- addplug ---
        for (int i=0; i<numPlugs; i++)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAF');
            cmd->In()->SetI(i);
            cmd->In()->SetS(this->plugArray[i].GetName().Get());

            vector3 pos = this->plugArray[i].GetTranslation();
            cmd->In()->SetF(pos.x);
            cmd->In()->SetF(pos.y);
            cmd->In()->SetF(pos.z);

            quaternion quat = this->plugArray[i].GetRotation();
            cmd->In()->SetF(quat.x);
            cmd->In()->SetF(quat.y);
            cmd->In()->SetF(quat.z);
            cmd->In()->SetF(quat.w);

            ps->PutCmd(cmd);
        }

        // --- endplugs ---
        if (numPlugs != 0)
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAAG');
            ps->PutCmd(cmd);
        }

        //... instance stream declaration

        return true;
    }
    return false;
}
