#ifndef N_3DS_SKELETON_H
#define N_3DS_SKELETON_H

//------------------------------------------------------------------------------
/**
    @class n3dsSkeleton
    @ingroup n3dsMaxAnimation
    @brief n3dsExporter skeleton data

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include <String.h>
#pragma warning( push, 3 )
#include "IGame/IGame.h"
#pragma warning( pop )

#include "util/narray.h"
#include "util/nstring.h"
#include "n3dsanimationexport/n3dsexportertypes.h"

//------------------------------------------------------------------------------
class n3dsSkeleton
{
public:
    /// data used to know which joint is the first one corresponding to a njointgroup
    class SeparationJoint
    {
    public:
        /// default constructor
        SeparationJoint(){;};
        /// constructor
        SeparationJoint(int jI, int gI){jointIndex=jI; groupIndex=gI;};
        /// destructor
        ~SeparationJoint(){;};
        /// set joint index
        void SetJointIndex(int jI){jointIndex = jI;};
        /// set joint group index
        void SetGroupIndex(int gI){groupIndex = gI;} ;
        /// get joint index
        int GetJointIndex(){return jointIndex;};
        /// get joint group index
        int GetGroupIndex(){return groupIndex;};
    private:
        int jointIndex;
        int groupIndex;
    };

    /// constructor
    n3dsSkeleton(int lodLevel = -1);
    /// destructor
    ~n3dsSkeleton();

    /// bones array
	nArray<n3dsBone> BonesArray;
    /// separation joints array
    nArray<SeparationJoint> SeparationArray;
    /// helpers array
    nArray<n3dsDynAttach> HelperArray;
    
    /// static-attachements array 
    nArray<n3dsAttachment> AttachmentArray;
    /// skeleton created bool value
    bool bSkeletonCreated;
    /// ragdoll bones array
    nArray<n3dsBone> RagBonesArray;
    /// correspondence between bones and ragdoll bones
    /// the first number is the index of the complet skeleton bone that corresponds with the ragbone 0, and so on
    nArray<int> RagCorresp;


    /// find bone index using 3dsMax node Id
    int FindBoneIndexByNodeId(int iNodeId);
    /// find bone index using name
    int FindBoneThatContains( const nString& jointName );

    /// check if an IGameNode is a bone
    static bool IsBoneNode(IGameNode *pGameNode);
    /// check if an IGameNode is a dummy
    bool IsDummyNode(IGameNode *pGameNode);

    /// creates the skeleton
    bool CreateSkeleton();
    /// fills bones array (recursive)
    void RecruseFillBonesArray(int iParentID, IGameNode *pGameNode, matrix44d parentMatrix);

    /// find jointseparations
    void FindJointSeparations(const nString& boneName);
    /// find human jointseparations
    void FindHumanJointSeparations(const nString& boneName);
    // find scavenger jointseparations
    void FindScavengerJointSeparations(const nString& boneName);
    /// find strider joint separations
    void FindStriderJointSeparations(const nString& boneName);


    /// is ragdoll bone
    int IsRagdollBone(const nString& boneName);
    /// fills ragdoll skeleton
    void FillRagSkeleton();
    /// finds ragdoll bones under the indicated one
    nArray<int> FindRagChildren(int parentindex);
    /// fins the ragdoll parent bone ( return the index of the ragskeleton (not of the full skeleton))
    int FindRagParent(n3dsBone child);
    /// calculates the ragdoll joint matrix
    void CalculateRagJointMatrix(int index, int parentid, matrix44d& localmatrix);

    /// is first joint of a joint group
    bool IsJointGroupLeader( const nString& boneName );

    /// get number of root bones
    int GetNumberOfRootBones() const;

private:
    /// skeleton lod level
    int lodLevel;

    /// bip01 matrices
    matrix44d rootLocalMatrix;
    matrix44d rootWorldMatrix;

    nArray<nString> humanRagBones;
    nArray<nString> scavengerRagBones;
    nArray<nString> striderRagBones;
};


//------------------------------------------------------------------------------
/**

*/
inline
int n3dsSkeleton::FindBoneIndexByNodeId(int iNodeId)
{
	for (int i = 0; i < BonesArray.Size(); i++)
	{
		if (BonesArray[i].iNodeId == iNodeId)
		{
			return i;
		}
	}

	return -1;
}

#endif