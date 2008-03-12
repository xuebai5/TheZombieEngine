#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsanimationexport/n3dsanimationexport.h"
#include "n3dsexporters/n3dsexportserver.h"


n3dsAnimationExport* n3dsAnimationExport::animationServer=0;
//------------------------------------------------------------------------------
/**
    default constructor
*/
n3dsAnimationExport::n3dsAnimationExport():
    skeletons(0,1),
    boundingBox(),
    perceptionOffset(),
    animationSettings(),
    jointGroupIndex(0),
    canExportAnim(false),
    canExportMotion(false)/*,
    morpher(0)*/
{
    //empty
}

//------------------------------------------------------------------------------
/**
    default destructor
*/
n3dsAnimationExport::~n3dsAnimationExport()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
n3dsAnimationExport*
n3dsAnimationExport::Instance()
{
    if(animationServer == 0)
    {
        animationServer =n_new(n3dsAnimationExport);
    }
    n_assert(animationServer);
    return animationServer;
}
//------------------------------------------------------------------------------
/**
    gets first skinned node pivot matrix
    only used to get Bip01 matrix
*/
Matrix3
n3dsAnimationExport::GetSkinPivotMatrix( )
{
    return this->GetSkinnedNode()->GetWorldTM().ExtractMatrix3();
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
    @return fist skinned node found in scene
*/
IGameNode*
n3dsAnimationExport::GetSkinnedNode()
{
    n_assert(n3dsExportServer::Instance()->GetIGameScene()->GetTotalNodeCount());

    IGameNode* skinnedNode = 0;
    for( int i=0; i< n3dsExportServer::Instance()->GetIGameScene()->GetTotalNodeCount(); i++)
    {
        skinnedNode = n3dsExportServer::Instance()->GetIGameScene()->GetTopLevelNode(i);
        IGameMesh* mesh = static_cast< IGameMesh* > ( skinnedNode->GetIGameObject() );
        if(mesh->IsObjectSkinned())
        {
            break;
        }
    }
    n_assert(skinnedNode);
    //initialize data
    skinnedNode->GetIGameObject()->InitializeData();
    return skinnedNode;
}

//------------------------------------------------------------------------------
/**
    get animation settings
*/
const n3dsAnimationSettings
n3dsAnimationExport::GetAnimationSettings()
{
    return this->animationSettings;
}
