#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsanimator/nanimatorbuilderserver.h"
#include "n3dsanimator/ntexanimatorbuilder.h"
#include "kernel/nfileserver2.h"

nAnimatorBuilderServer::~nAnimatorBuilderServer()
{
    if ( this->node )
    {
        this->node->Release();
    }

    if ( this->rootNode )
    {
        this->rootNode->Release();
    }
    animatorHash.removeAll();
}

nString
nAnimatorBuilderServer::GetPath(nAnimatorBuilder* anim)
{
    n_assert(anim);
    n_assert(this->node);
    nStrPath* hashNode;

    hashNode =   (nStrPath*) (this->animatorHash.Find( anim->GetUniqueString() ));
    if (! hashNode)
    {
        hashNode = n_new(nStrPath);
        hashNode->path =  "animator_";
        hashNode->path.AppendInt(animatorCount++);
        hashNode->SetName(anim->GetUniqueString() );
        animatorHash.Add( hashNode );

        nKernelServer::ks->PushCwd( this->node);
        nAnimator* animator = (nAnimator* )nKernelServer::ks->New( anim->GetClass() , hashNode->path.Get() );
        nKernelServer::ks->PopCwd();

        anim->SetToAnimator( animator );
    }


    nString path = pathAnimatorOut + hashNode->path;
    return path;
}

void 
nAnimatorBuilderServer::Init(const nString name)
{
    pathAnimatorOut = "/usr/animators/";
    pathAnimatorOut+=name;
    pathAnimatorOut+="/";


    rootNode = nKernelServer::ks->New("ntransformnode",  "/usr/animators");
    n_assert(rootNode);
    nKernelServer::ks->PushCwd(rootNode);
    node = nKernelServer::ks->New("ntransformnode",  name.Get() );
    nKernelServer::ks->PopCwd();
}