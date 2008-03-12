#include "precompiled/pchn3dsmaxexport.h"
#pragma warning( push, 3 )
#include "IGAME/IGameObject.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
#include "n3dsexporters/n3dsblendshape.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsanimationexport/n3dsanimationexport.h"

//------------------------------------------------------------------------------
#include "nscene/nblendshapenode.h"
#include "nscene/nblendshapeanimator.h"
#include "nscene/ntransformnode.h"
#include "nscene/nshapenode.h"
#include "nasset/nentityassetbuilder.h"
#include "tools/nanimbuilder.h"
#include "kernel/nfileserver2.h"
//------------------------------------------------------------------------------
const int nameBufferSize = 255;
const int maxmorphtargets = 5;

//------------------------------------------------------------------------------
/**
    constructor
*/
n3dsBlendShape::n3dsBlendShape(IGameNode* Node, nEntityClass* entityClass)
{
    this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPath( entityClass );
    this->entityClass = entityClass;
    this->Init(Node); 
}

//------------------------------------------------------------------------------
/**
*/
n3dsBlendShape::n3dsBlendShape(IGameNode* Node, const n3dsAssetInfo& asset, nEntityClass* entityClass) 
{
    this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPathForAsset( asset.assetPath );
    this->entityClass = entityClass;
    this->Init(Node); 
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsBlendShape::Init(IGameNode* Node) 
{
    n_assert(Node);
    n_assert(Node->GetIGameObject()->GetIGameType()==IGameObject::ObjectTypes::IGAME_MESH);

    exportComponent=nMeshBuilder::Vertex::Component::COORD    |
                    nMeshBuilder::Vertex::Component::COLOR    |
                    nMeshBuilder::Vertex::Component::NORMAL   |
                    nMeshBuilder::Vertex::Component::UV0      |
                    nMeshBuilder::Vertex::Component::UV1      |
                    nMeshBuilder::Vertex::Component::UV2      |
                    nMeshBuilder::Vertex::Component::UV3      ;

    //get mesh and IGameNode
    this->targets.At(0)=Node;
    this->targetmesh.At(0)= static_cast<IGameMesh *>(this->targets.At(0)->GetIGameObject());
    this->Names.At(0).Format("mesh_%.8X", Node->GetNodeID() );

    //get the morpher modifier
    IGameObject* iObj = Node->GetIGameObject();

    bool foundmorph=false;
    for(int j=0;((iObj->GetNumModifiers())&&(!foundmorph));j++)
    {
        IGameModifier* iMod = this->targets.At(0)->GetIGameObject()->GetIGameModifier(j);
            
        if(iMod->IsMorpher())
        {
            morpher = static_cast< MorphR3* >(iMod->GetMaxModifier());
            foundmorph = !foundmorph;
        }
    }

    //get targets IGameNodes and names
    n3dsExportServer* Server  = n3dsExportServer::Instance();
    IGameScene* pIgame = Server->GetIGameScene();
    RefTargetHandle reference;
    for(int targetindex = 0;  targetindex< MR3_NUM_CHANNELS ; targetindex++)
    {
        if(this->morpher->GetReference(101+targetindex))
        {
            reference = morpher->GetReference(101+targetindex);
            INode *ignode = static_cast< INode * > ( reference );
            IGameNode* igametarget = pIgame->GetIGameNode( ignode);
            if( igametarget->GetIGameObject()->GetIGameType() == IGameObject::ObjectTypes::IGAME_MESH )
            {
                this->targets.Append( igametarget ); 
                this->targetmesh.Append( static_cast< IGameMesh *> ( igametarget->GetIGameObject() ) );
                this->targetmesh.Back()->InitializeData(); 

                TSTR targetname = this->morpher->chanBank[targetindex].mName;

                nString npath = (nString)targetname;
                npath.ConvertSpace();
                Names.Append(npath);
            }
        }
    }

    this->blendMesh = 0;
}

//------------------------------------------------------------------------------
/**
    creates animation
*/
bool
n3dsBlendShape::CreateAnimation()
{
    //n3dsExportServer* exportServer  = n3dsExportServer::Instance();
    float normalize = (float)0.01;

    //get animation
    nAnimBuilder AnimBuilder;
    nKernelServer* ks = nKernelServer::ks;

    nAnimBuilder::Group AnimGroup;
	AnimGroup.SetLoopType(nAnimBuilder::Group::REPEAT);//CLAMP);//

	nAnimBuilder::Curve AnimCurve;
    AnimCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);
	nAnimBuilder::Key AnimKey;
    Interval animinterval;
    int totalkeys = 0;
    Control* Controller;
    IKeyControl* Ikc;
    int KeyCount;
    IBezFloatKey FKey, FKey1;
    
    //how many keys each curve
    Interface* ip = GetCOREInterface();
    float animstart = (float)ip->GetAnimRange().Start();
    float animend = (float)ip->GetAnimRange().End();
    int ticks_per_frame = (int)animend/n3dsAnimationExport::Instance()->GetAnimationSettings().iLastFrame;
    int keyseachcurve = (int)(animend - animstart)/ticks_per_frame;
    keyseachcurve++;//the first one

    //calculate nebula keys

    //the fist mesh (neutral)
    for(int numkeys=0; numkeys<= keyseachcurve; numkeys++)
    {
        AnimKey.Set(vector4(0,0,0,0));
        AnimCurve.SetKey(numkeys,AnimKey);
    }
    AnimGroup.AddCurve(AnimCurve);
    //the targets
    int putkeys=0;
    for(int i=0; i<MR3_NUM_CHANNELS; i++)
    {
        if(morpher->chanBank[i].mActive)
        {
           putkeys=0;
           Controller = morpher->chanBank[i].cblock->GetController( 0 );
           Ikc = GetKeyControlInterface( Controller );
           KeyCount = Ikc->GetNumKeys();
           totalkeys += keyseachcurve; 

           int ki;
           for( ki=0; ki<(KeyCount-1); ki++)
           {
                Ikc->GetKey( ki, &FKey);
                Ikc->GetKey(ki+1, &FKey1);
                float incrT = (float) (FKey1.time - FKey.time)/ticks_per_frame;
                float incrX = (FKey1.val - FKey.val)*normalize/incrT;
                float lastkey = FKey.val*normalize;
                for(int numkeys=0; numkeys<= incrT; numkeys++)
                {
                    AnimKey.Set(vector4(lastkey + incrX*numkeys,0,0,0));
                    AnimCurve.SetKey(putkeys,AnimKey);
                    if((numkeys!=0)||(ki==0))
                        putkeys++;
                }
            }

           //mantain the animation till the end of the timeline
           if(FKey1.time!=animend)
           {
                for(int time=FKey1.time; time<= animend; time+=ticks_per_frame)
                {
                    AnimKey.Set(vector4(FKey1.val*normalize,0,0,0));
                    AnimCurve.SetKey(putkeys,AnimKey);
                    putkeys++;
                }

            }
            //there is no movement, static frame
            if(KeyCount == 1)
            {
                int keyspa= keyseachcurve-1;
                Ikc->GetKey( ki, &FKey);
                for(int numkeys=0; numkeys <= keyspa; numkeys++)
                {
                    AnimKey.Set(vector4(FKey.val*normalize,0,0,0));
                    AnimCurve.SetKey(numkeys,AnimKey);
                }
            }
            AnimGroup.AddCurve(AnimCurve);
        }
    }
    //keystride
  	/*n3dsAnimationExport::Instance()->GetAnimationSettings().fKeyDuration = 
            (float)n3dsAnimationExport::Instance()->GetAnimationSettings().iFramesPerSample / GetFrameRate();
	AnimGroup.SetKeyTime(n3dsAnimationExport::Instance()->GetAnimationSettings().fKeyDuration);*/
    float keyDuration = (float)n3dsAnimationExport::Instance()->GetAnimationSettings().iFramesPerSample / GetFrameRate();
    AnimGroup.SetKeyTime(keyDuration);

    AnimGroup.SetNumKeys(keyseachcurve);
	AnimBuilder.AddGroup(AnimGroup);
	AnimBuilder.Optimize(); 
	AnimBuilder.FixKeyOffsets();

    nString path = nEntityAssetBuilder::GetAnimsPath( entityClass );
    path.Append( "morphanimation.nanim2");

    nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() );

    if(AnimBuilder.Save(ks->GetFileServer(), path.Get()))
    {
        this->CreateAnimator();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    create i-th target
*/
void
n3dsBlendShape::CreateTarget(int i , int component)
{
    n3dsExportServer* Server  = n3dsExportServer::Instance();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    if(targetmesh[i]->GetNumberOfFaces() > 0)
    {  
        matrix44 myMatrix;
        const int numFaces = targetmesh.At(i)->GetNumberOfFaces();
        n3dsTriangle triangle;
        nMeshBuilder::Vertex vertex[3];
        int idxFace;

        blendMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
        n_assert(blendMesh);

        for (idxFace=0; idxFace<numFaces; idxFace++)
        {
            triangle.Set(targetmesh.At(i),idxFace);
            triangle.GetVertex(vertex, component, this->targets.At(i));
            systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
            blendMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
        }

        //move to the right place
        GMatrix g=this->targets.At(i)->GetWorldTM();
        transform44 transform = Server->GetSystemCoordinates()->MaxToNebulaTransform(g);
        targetTransMatrix = transform.getmatrix();
        targetTransMatrix.invert();

        targetTransMatrix = localTransMatrix * targetTransMatrix;

        this->blendMesh->Transform(targetTransMatrix);
    }
}

//------------------------------------------------------------------------------
/**
    create targets
*/
void
n3dsBlendShape::CreateTargets()
{
    TCHAR* neutralmesh = "NEUTRO";
    TSTR targetname;
    int stringcomparison;
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    IGameMaterial* mtl = this->targets.At(0)->GetNodeMaterial();
    int matId = mtlList->GetMaterialId(mtl);

    for(int i=1; i< targets.Size(); i++)
    {
        targetname = Names.At(i).Get();

        if((stringcomparison = strncmp(targetname,neutralmesh,6)) == 0)
        {
            this->CreateTarget(i , GetComponents(matId) );
            this->SetCurrentTargetName(0, matId);
            this->SaveTarget(0, matId);
            bbox3 localBox = this->blendMesh->GetGroupBBoxTestBillboard(0);
            this->box.extend(localBox);
            box = localBox;
            this->SaveSceneFile(matId);
        }
        else
        {
            this->CreateTarget(i , nMeshBuilder::Vertex::Component::COORD | nMeshBuilder::Vertex::Component::NORMAL);
            this->SetCurrentTargetName(i, matId);
            this->SaveTarget(i, matId);
        }
        n_delete(blendMesh);
    }
}

//------------------------------------------------------------------------------
/**
    create mesh
*/
bool
n3dsBlendShape::CreateMesh()
{
    nTransformNode*   trNode;

    n_assert(this->targets.At(0));

 
    n3dsExportServer* Server  = n3dsExportServer::Instance();
    nKernelServer*    ks      = Server->GetNebula()->GetKernelServer();

    if (targetmesh.At(0)->GetNumberOfFaces() > 0)
    {
        trNode = (nTransformNode* )ks->New("ntransformnode" , Names.At(0).Get());
        n_assert(trNode);
        ks->PushCwd(trNode);

        CreateTargets();

        trNode->SetLocalBox(box);
    
        this->box.begin_extend();
           
        nKernelServer::ks->PopCwd();
        return true;
    } else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
__cdecl
n3dsBlendShape::SortedByMatID(const void* elm0, const void* elm1)
{
    Map* i0 = (Map*)elm0;
    Map* i1 = (Map*)elm1;

    if (i0->material != i1->material)
    { 
        return i0->material - i1->material;
    } else
    {
        return i0->index - i1->index;
    }
}

//------------------------------------------------------------------------------
/**
    set current target name
*/
void
n3dsBlendShape::SetCurrentTargetName(int index, int material)
{
    char buf[nameBufferSize];
    sprintf(buf, ".%d", material);
    Names.At(index) += buf;
    Names.At(index) += index;

    bool binaryFile = n3dsExportServer::Instance()->GetSettings().binaryResource;
    if (binaryFile)
    {
        Names.At(index) += ".nvx2";
    } else
    {
       Names.At(index) += ".n3d2";
    }
}

//------------------------------------------------------------------------------
/**
    create animator
*/
void 
n3dsBlendShape::CreateAnimator()
{
    nKernelServer*  ks = nKernelServer::ks;

    //nString animPath = nEntityAssetBuilder::GetAnimsPath( entityClass );
    nBlendShapeAnimator *blendanim = static_cast< nBlendShapeAnimator * > ( ks->Lookup( /*animPath.Get()*/"/usr/animators/anims" ) );
    if (!blendanim)
    {
        blendanim = static_cast<nBlendShapeAnimator*>(ks->New("nscenenode", "/usr/animators/anims"));
        n_assert(blendanim);
    }
    ks->PushCwd( blendanim );
    nBlendShapeAnimator *pBlendAnimator = static_cast< nBlendShapeAnimator * > ( ks->NewNoFail("nblendshapeanimator","blendanimator") );
    if (pBlendAnimator)
	{
        nString animsName = nEntityAssetBuilder::GetAnimsPath( entityClass );
        animsName+= "morphanimation.nanim2";
        
        int numState = pBlendAnimator->GetNumStates();
        int numGroup = 0;
        //float fadeInTime = 0.0f;
        pBlendAnimator->AddState(numGroup/*, fadeInTime*/);
        pBlendAnimator->SetStateAnim( numState, animsName.Get() );
        pBlendAnimator->SetStateName( numState, animsName.ExtractFileName().Get());
    }
    ks->PopCwd();
}

//------------------------------------------------------------------------------
/**
    save target
*/
void
n3dsBlendShape::SaveTarget(int index, int material)
{
    //nKernelServer*  ks = nKernelServer::ks;

    nString meshName = this->assetPathMeshes;

    //nString meshName = nEntityAssetBuilder::GetMeshesPath( entityClass );
    meshName+=this->Names.At(index);

    nFileServer2::Instance()->MakePath( meshName.ExtractDirName().Get() );

    n3dsExportServer::Instance()->GetMtlList()->Preparen3dsMaterial(material); // Calculate the mesh components
    bool needTangentUV0 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV0();
    bool needTangentUV3 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV3();
    //bool optimizeMesh   = n3dsExportServer::Instance()->GetSettings().optimizeMesh;
    bool binaryFile = n3dsExportServer::Instance()->GetSettings().binaryResource;

    //if (optimizeMesh) 
    //{
    //    //this->myMesh->Cleanup(0);
    //}
    if (needTangentUV0)
    {
        blendMesh->BuildTriangleOnlyTangentBinormal();
    }
    
    if (needTangentUV3)
    {
        blendMesh->BuildTriangleOnlyTangentBinormal(3);
    }

    if (binaryFile)
    {
      this->blendMesh->SaveNvx2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    } else
    {
       this->blendMesh->SaveN3d2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    }
}

//------------------------------------------------------------------------------
/**
    save scene file
*/
void
n3dsBlendShape::SaveSceneFile(int material)
{
    nKernelServer*  ks = nKernelServer::ks;
    nMatTypePropBuilder matProp;
    nString meshName;
    meshName="meshes:";
    meshName+=Names.At(0).Get();

    char buf[255];
    sprintf(buf,"shape.%i",material);

    nShapeNode *currentShape;
    
    currentShape = (nShapeNode* )ks->New("nblendshapenode" , buf);
     
    n_assert(currentShape);

    bbox3 localBox = this->blendMesh->GetGroupBBoxTestBillboard(0);
    currentShape->SetLocalBox(localBox);
    this->box.extend(localBox);
    box = localBox;

    nBlendShapeNode* currentBlendShape = (nBlendShapeNode*) currentShape;

    //the targets
    if ( ( Names.Size() - 1 ) > maxmorphtargets )
    {
        n_message("More targets than the allowed, meshes will be saved correctly, animation won't");
    }

    for(int i = 0 ; (i < (Names.Size()-1))&&(i < maxmorphtargets); i++) //the last target is the neutral one
    {
        nString meshName = "meshes:";
        meshName+=Names.At(i).Get();

        currentBlendShape->SetMeshAt(i, meshName.Get());
        currentBlendShape->SetWeightAt(i, 0.00);
    }

    //add animator
    nString pathAnimator = nEntityAssetBuilder::GetAnimsPath( entityClass );
    pathAnimator += "blendanimator";

    currentBlendShape->AddAnimator( pathAnimator.Get() );
    currentBlendShape->SetGroupIndex(0);

    matProp.AddParam("deform", "morpher");
 
    // SET MATERIAL 
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();  
    mtlList->SetMaterial(currentShape, material,&matProp);

    ///@todo remove this warning when the invalid shader will be a invalid material
    N3DSWARNCOND( /*name*/  meshexport , 
            /*COND*/ mtlList->Getn3dsMaterial( material ).IsInValidShaderType() ,
            /*LOG*/( 0, "WARNING: \"%s\" has a invalid shader \"%s\"" , Names.At(0).Get() , mtlList->Getn3dsMaterial( material ).GetName() )
            );
    N3DSERRORCOND( /*name*/  meshexport , 
              /*COND*/ ! mtlList->Getn3dsMaterial( material ).IsValid() ,
              /*LOG*/( 0, "ERROR: \"%s\" has a incorrect material \"%s\"\n" , Names.At(0).Get() , mtlList->Getn3dsMaterial( material ).GetName() )
            );
}
 
//------------------------------------------------------------------------------
/**
*/
void 
n3dsBlendShape::TransformLocal(nMeshBuilder *localMesh)
{
    localMesh->Transform(localTransMatrix);
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsBlendShape::GetComponents(int matId)
{
    int matComponents = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(matId).GetMeshComponents();
    int components = matComponents & n3dsTriangle::GetMeshComponents( targetmesh.At(0) );
    return components;
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsBlendShape::IsBlendedNode(IGameNode* Node)
{
    bool isblending = false;
    IDerivedObject *pDerObj = NULL;
    Object* Obj =  Node->GetMaxNode()->GetObjectRef();

    if( Obj->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
	{
        IGameObject* iObj = Node->GetIGameObject();
		pDerObj = static_cast< IDerivedObject * >( Obj );
        for(int j=0;( ( j < iObj->GetNumModifiers() ) && ( !isblending ) );j++)
        {
            IGameModifier* iMod = iObj->GetIGameModifier(j);
            isblending = iMod->IsMorpher();
        }
    }
    return isblending;
}
