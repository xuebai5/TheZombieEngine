#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nvideoanimator_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nvideoanimator.h"
#include "nscene/nabstractshadernode.h"

#include "video/nvideoserver.h"
#include "video/nvideoplayer.h"

#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nVideoAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nVideoAnimator::nVideoAnimator() :
    stateVarHandle(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVideoAnimator::~nVideoAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoAnimator::SetStateChannel(const char *varName)
{
    n_assert(varName);
    this->stateVarHandle = nVariableServer::Instance()->GetVariableHandleByName(varName);
}

//------------------------------------------------------------------------------
/**
*/
const char *
nVideoAnimator::GetStateChannel() const
{
    if (this->stateVarHandle == nVariable::InvalidHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->stateVarHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoAnimator::LoadResources()
{
    if (!this->refVideoPlayer.isvalid() && !this->videoFile.IsEmpty())
    {
        nVideoPlayer* videoPlayer = nVideoServer::Instance()->NewVideoPlayer(this->videoFile.Get());
        n_assert(videoPlayer);

        if (!videoPlayer->IsOpen())
        {
            switch (this->GetLoopType())
            {
            case nAnimLoopType::Clamp:
                videoPlayer->SetLoopType(nVideoPlayer::Clamp);
                break;
            case nAnimLoopType::Loop:
            default:
                videoPlayer->SetLoopType(nVideoPlayer::Repeat);
                break;
            }
        }

        this->refVideoPlayer = videoPlayer;
        this->refVideoPlayer->Open();
        n_assert(this->refVideoPlayer->IsOpen());
        n_assert(this->refVideoPlayer->GetWidth() > 0);
        n_assert(this->refVideoPlayer->GetHeight() > 0);

        if (!this->refVideoTexture.isvalid())
        {
            nString texName(this->videoFile);
            texName.Append("_tex");
            nTexture2* tex = nGfxServer2::Instance()->NewTexture(texName.Get());
            n_assert(tex);
            if (!tex->IsLoaded())
            {
                tex->SetFormat(nTexture2::A8R8G8B8);
                tex->SetType(nTexture2::TEXTURE_2D);
                tex->SetWidth(ushort(this->refVideoPlayer->GetWidth()));
                tex->SetHeight(ushort(this->refVideoPlayer->GetHeight()));
                tex->SetUsage(nTexture2::CreateEmpty|nTexture2::Dynamic);
                tex->Load();
                n_assert(tex->IsLoaded());
            }
            this->refVideoTexture = tex;
            this->refVideoPlayer->SetTexture(tex);
        }

        return nSceneNode::LoadResources();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoAnimator::UnloadResources()
{
    if (this->refVideoPlayer.isvalid())
    {
        nVideoServer::Instance()->DeleteVideoPlayer(this->refVideoPlayer);
        this->refVideoPlayer.invalidate();
    }
    if (this->refVideoTexture.isvalid())
    {
        this->refVideoTexture->Release();
        this->refVideoTexture.invalidate();
    }

    nShaderAnimator::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    if (!this->refVideoPlayer.isvalid() || !this->refVideoTexture.isvalid())
    {
        return;
    }

    ncDictionary* varContext = entityObject->GetComponent<ncDictionary>();
    if (varContext)
    {
        //get state variable to open or shut video
        nVariable* var = varContext->GetVariable(this->stateVarHandle);
        if (var && var->GetBool())
        {
            if (!this->refVideoPlayer->IsOpen())
            {
                this->refVideoPlayer->Open();
                this->refVideoPlayer->SetTextureUpdate(true);
            }
            else
            {
                //TODO- get rewind variable
            }
            //set the texture only if the video is on
            nAbstractShaderNode* targetNode = static_cast<nAbstractShaderNode*>(sceneNode);
            targetNode->SetTextureOverride(this->param, this->refVideoTexture);
        }
        else if (this->refVideoPlayer->IsOpen())
        {
            //stop the video
            this->refVideoPlayer->Close();
            this->refVideoPlayer->SetTextureUpdate(false);
        }
    }
}
