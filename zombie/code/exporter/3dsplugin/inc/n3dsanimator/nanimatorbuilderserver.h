#ifndef  N_ANIMATOR_BUILDER_SERVER_H
#define  N_ANIMATOR_BUILDER_SERVER_H

#include "util/nstring.h"
#include "util/nhashtable.h"
//#include "ntexanimatorbuilder.h"

//------------------------------------------------------------------------------
/**
    @class nAnimatorBuilderServer
    @ingroup n3dsMaxAnimators
    @brief is a server, create a unique nAnimator by properties.
    return the path's animotor 
*/
class nRoot;
class nAnimatorBuilder;
class nAnimatorBuilderServer
{

public:
    nAnimatorBuilderServer();
    ~nAnimatorBuilderServer();
    /// in this path, the nAnimator were created 


    nString GetPath(nAnimatorBuilder* anim);
    /// init animator, pass the name of asset
    void Init(const nString path);
    /// Save the animators
    //void Save(const nString assetPath);

private:
    nString pathAnimatorOut;
    nString pathAnimatorIn;

    nHashTable animatorHash;
    int        animatorCount;
    nRoot*     rootNode;
    nRoot*     node;

    class nStrPath  : public nStrNode
    {
        public:
        nString path;
        int id;
    };

};

inline
nAnimatorBuilderServer::nAnimatorBuilderServer() : 
    animatorHash(29),
    animatorCount(0),
    rootNode(0),
    node(0)
{

}
#endif