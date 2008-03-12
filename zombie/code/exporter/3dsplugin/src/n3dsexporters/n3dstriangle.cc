#include "precompiled/pchn3dsmaxexport.h"
//------------------------------------------------------------------------------
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsexporters/n3dssystemcoordinates.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsanimationexport/n3dsskeleton.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
void n3dsTriangle::GetVertex(nMeshBuilder::Vertex  vertex[],int component,IGameNode *iNode, n3dsSkeleton* pSkeleton)
{
    n_assert(vertex);
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();
    //only 3 vertex of face.
    //clear all components;
    int i;
    for (i=0;i<3;i++)
        vertex[i].DelComponent((nMeshBuilder::Vertex::Component) vertex[i].GetComponentMask());

    FaceEx* face = this->mesh->GetFace(this->index);
    n_assert(face);

    Point3 vgeom; 
    //coordinates
    for (i=0; i<3; i++)
    {
        //int index = (int)face->vert[i];
        vgeom = mesh->GetVertex(face->vert[i]);
        vector3 pos= systemCoord->MaxtoNebulaVertex(vgeom);
        vertex[i].SetCoord(pos);
    }

    if (component&nMeshBuilder::Vertex::Component::NORMAL)
    {
        Point3 vnormal;
        vector3 normal;
        for (i=0; i<3; i++)
        {
            vnormal = this->mesh->GetNormal(face , i);
            normal = systemCoord->MaxtoNebulaNormal(vnormal);
            vertex[i].SetNormal(normal);
        }
    }

    //in the mesh any vertex has a vertex color 
    if (component&nMeshBuilder::Vertex::Component::COLOR)
    {
        int NumberOfColorVerts = mesh->GetNumberOfColorVerts();
        float alpha = 1.0;
        Point3 vcolor(1.0f,1.0f,1.0f);
        for (i=0; i<3; i++)
        {
            if ( ( face->color[i] < (DWORD)mesh->GetNumberOfColorVerts() ) )
            {
                vcolor = this->mesh->GetColorVertex(face->color[i]);
            } else
            { 
                // if mesh->GetNumberOfColorVerts() >0 then the mesh need a collapse
                vcolor=Point3(1.0f,1.0f,1.0f);
            }
            
            if ( !(face->alpha[i] < (DWORD)mesh->GetNumberOfAlphaVerts() &&
                  this->mesh->GetAlphaVertex(face->alpha[i] , alpha) ) )
            {
                alpha = 1.0f;
            }

            vertex[i].SetColor(vector4(vcolor.x, vcolor.y, vcolor.z, alpha));
        }
    }

    int layer;
    const int components[4]={nMeshBuilder::Vertex::Component::UV0,
                             nMeshBuilder::Vertex::Component::UV1,
                             nMeshBuilder::Vertex::Component::UV2,
                             nMeshBuilder::Vertex::Component::UV3};

    //n3dsSystemCoordinates* coord = n3dsExportServer::Instance()->GetSystemCoordinates();

    for (layer=0; layer<4; layer ++)
    {
        if (component & components[layer])
        {
            DWORD  v[3];
            n_verify( mesh->GetMapFaceIndex(layer+1, this->index,v) );
            Point3 uv3ds;
            for(i=0; i<3; i++)
            {
                uv3ds = this->mesh->GetMapVertex(layer+1, v[i]);
                vector2 uv = systemCoord->MaxToNebulaUV(uv3ds);
                vertex[i].SetUv(layer,uv);
            }
        } 
    }

    // fill weights and joint indices if skinned mesh
    if (component&nMeshBuilder::Vertex::Component::WEIGHTS && component&nMeshBuilder::Vertex::Component::JINDICES)
    {
        IGameObject *pGameObject = iNode->GetIGameObject();
        FaceEx *pFace = mesh->GetFace(this->index);
	    n_assert(pFace!=NULL);

        // find skin modifier
		int numMod = pGameObject->GetNumModifiers();
		for (int i = 0; i < numMod; i++)
		{
			IGameModifier *pGameModifier = pGameObject->GetIGameModifier(i);
			if (pGameModifier->IsSkin())
			{
				IGameSkin *pGameSkin = static_cast< IGameSkin * > ( pGameModifier );
                // determine weights and joint indices for each vertex of the triangle
                for (int idxVertex=0; idxVertex<3; idxVertex++)
                {
				    int iVertexIndex = pFace->vert[idxVertex];
				    int VertexType = pGameSkin->GetVertexType(iVertexIndex);
				    VertexInfluence vert_influence;
                    
				    switch (VertexType)
				    {
                        // influenced by one bone
					    case IGameSkin::IGAME_RIGID:
					    {
                            IGameNode *pBoneNode = pGameSkin->GetIGameBone(iVertexIndex, 0);
						    if (pBoneNode)
						    {
							    int iBoneIndex = pSkeleton->FindBoneIndexByNodeId( pBoneNode->GetNodeID() );
							    if (iBoneIndex >= 0)
							    {
								    vert_influence.AddBoneInfluence(iBoneIndex, 1.0f);
							    }
							    else
							    {
                                    n_printf("Bone influence - error");
							    }
						    }
						    else
						    {
                                n_printf("No bone influence for vertex");
						    }
					    }
					    break;

                        // influenced by more than one bone
					    case IGameSkin::IGAME_RIGID_BLENDED:
					    {
						    int iNumBones = pGameSkin->GetNumberOfBones(iVertexIndex);
						    for (int j = 0; j < iNumBones; j++)
						    {
							    IGameNode *pBoneNode = pGameSkin->GetIGameBone(iVertexIndex,j);
							    if (pBoneNode)
							    {
								    int iBoneIndex = pSkeleton->FindBoneIndexByNodeId( pBoneNode->GetNodeID() );
								    if (iBoneIndex >= 0)
								    {
								        float fWeight = pGameSkin->GetWeight(iVertexIndex, j);
									    vert_influence.AddBoneInfluence(iBoneIndex, fWeight);
								    }
								    else
								    {
                                        n_printf("Bone influence - error");
								    }
							    }
							    else
							    {
                                    n_printf("No bone influence for vertex");
							    }
						    }
					    }
					    break;
				    }

                    // remove bone influence if it's less than 0.0001f
				    vert_influence.AdjustBonesInfluence(0.0001f); 
                    // set indices and weights
                    vertex[idxVertex].SetJointIndices(vert_influence.GetIndicesVector());
				    vertex[idxVertex].SetWeights(vert_influence.GetWeightsVector());
                } // for (int idxVertex=0; idxVertex<3; idxVertex++)
			}  //if (pGameModifier->IsSkin())
		}  //for 
   }
 
}

//------------------------------------------------------------------------------
/**
*/
int
n3dsTriangle::GetMeshComponents(IGameMesh* mesh)
{
    int realComponent = nMeshBuilder::Vertex::Component::COORD |
                        nMeshBuilder::Vertex::Component::NORMAL;

    Tab<int> mapLayers = mesh->GetActiveMapChannelNum();
    const int mapCount = mapLayers.Count();

    for (int i=0;i<mapCount;i++)
    {
        int layer=mapLayers[i]-1;
        switch(layer)
        {
            case 0:realComponent |= nMeshBuilder::Vertex::Component::UV0;
                break;
            case 1:realComponent |= nMeshBuilder::Vertex::Component::UV1;
                break; 
            case 2:realComponent |= nMeshBuilder::Vertex::Component::UV2;
                break;
            case 3:realComponent |= nMeshBuilder::Vertex::Component::UV3;
                break;
            default:
                break;
        }
    }
    /// TODO check if this components if avaible
    if (mesh->IsObjectSkinned() )
    {
        realComponent |= nMeshBuilder::Vertex::Component::WEIGHTS;
        realComponent |= nMeshBuilder::Vertex::Component::JINDICES;
    }

    //if ( 0 < mesh->GetNumberOfColorVerts() )
    //{
    //    realComponent |= nMeshBuilder::Vertex::Component::COLOR;
    //}
    // if the mesh not has it always export
    realComponent |= nMeshBuilder::Vertex::Component::COLOR;
   
    return realComponent;
}
