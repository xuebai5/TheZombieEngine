#pragma once
#pragma warning( push, 3 )
#include "Max.h"
#pragma warning( pop )
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
/**

*/
//struct n3dsTransformGeometrySettings
//{
//	n3dsTransformGeometrySettings()
//		: bEnableTransform(FALSE)
//		, Traslation(0, 0, 0)
//		, Rotation(0, 0, 0)
//		, Scale(1, 1, 1)
//	{
//	}
//
//	bool   bEnableTransform;
//	Point3 Traslation;
//	Point3 Rotation;
//	Point3 Scale;
//
//	Matrix3 GetMatrix()
//	{
//		Matrix3 tm;
//
//		tm.IdentityMatrix();
//		tm.Scale(Scale);
//		tm.RotateX(DegToRad(Rotation.x));
//		tm.RotateY(DegToRad(Rotation.y));
//		tm.RotateZ(DegToRad(Rotation.z));
//		tm.Translate(Traslation);
//
//		return tm;
//	};
//
//	Matrix3 GetScaleMatrix()
//	{
//		Matrix3 tm;
//
//		tm.IdentityMatrix();
//		tm.Scale(Scale);
//
//		return tm;
//	};
//
//	Matrix3 GetTransRotMatrix()
//	{
//		Matrix3 tm;
//
//		tm.IdentityMatrix();
//		tm.RotateX(DegToRad(Rotation.x));
//		tm.RotateY(DegToRad(Rotation.y));
//		tm.RotateZ(DegToRad(Rotation.z));
//		tm.Translate(Traslation);
//
//		return tm;
//	};
//
//	Matrix3 GetScaleRotMatrix()
//	{
//		Matrix3 tm;
//
//		tm.IdentityMatrix();
//		tm.Scale(Scale);
//		tm.RotateX(DegToRad(Rotation.x));
//		tm.RotateY(DegToRad(Rotation.y));
//		tm.RotateZ(DegToRad(Rotation.z));
//
//		return tm;
//	};
//};

//------------------------------------------------------------------------------
/**
    allows conversion between 3dsMax bone nodes and n3dsExporter bones

*/
struct n3dsBone
{
    /**
        default contructor
    */
	n3dsBone()
		: iParentBoneId(-1)
		, strBoneName(_T(""))
		, bIsDummy(FALSE)
		, iNodeId(-1)
	{
        //empty
	}

    //-----------------------------------
    /**
        _copy_ contructor using max matrices
    */
	n3dsBone( int iParentBoneId, const nString& boneName, const Matrix3& local_trans, const Matrix3& world_trans, int iNodeId, BOOL bIsDummy = FALSE)
		: iParentBoneId(iParentBoneId)
		, strBoneName(boneName)
		, LocalTransform(local_trans)
		, WorldTransform(world_trans)
		, bIsDummy(bIsDummy)
		, iNodeId(iNodeId)
	{
        //empty
	}

    /**
        _copy_ contructor using nebula matrices
    */
	n3dsBone(int iParentBoneId, const nString& boneName, const matrix44d& local_trans, const matrix44d& world_trans, int iNodeId,
		BOOL bIsDummy = FALSE)
		: iParentBoneId(iParentBoneId)
		, strBoneName(boneName)
		, localTr(local_trans)
		, worldTr(world_trans)
		, bIsDummy(bIsDummy)
		, iNodeId(iNodeId)
	{
        //empty
	}

    /**
        default destructor
    */
	~n3dsBone()
	{
        //empty
	}

    //-----------------------------------
    /** data
    */

    // order id
	int iParentBoneId;
	nString strBoneName;
	
    /// bone transformations with max matrices
	Matrix3 LocalTransform;
	Matrix3 WorldTransform;

    /// bone transformations with nebula matrices
    matrix44d localTr;
    matrix44d worldTr;

    //max node id
	int iNodeId;

	BOOL bIsDummy;
};


//------------------------------------------------------------------------------
/**
    struct to save attachments data

*/
struct n3dsAttachment
{
    //--------------------------------------------
    /**
        default constructor
    */
    n3dsAttachment()
		: iBoneID(-1)
		, iObject(0)
	{
        //empty
	}

    //--------------------------------------------
    /**
        default destructor
    */
    ~n3dsAttachment()
    {
        //empty
    }

    //--------------------------------------------
    /**
        constructor
    */	
    n3dsAttachment(int iboneID, IGameNode* igObject)
		: iBoneID(iboneID)
		, iObject(igObject)
	{
        //empty
	}

    //--------------------------------------------
    /**
        data
    */

    int iBoneID;
    IGameNode* iObject;
};

//-------------------------------------------------------------------------
/**
    struct to convert 3dsMax vertex data to nebula data
    one vertex can be influenced by 4 different bones
*/
struct VertexInfluence
{
    //-------------------------------------------
    /**
        one bone influence
    */
	struct BoneInfluence
	{
		int iBone;
		float fWeight;
        ~BoneInfluence(){/*Empty*/}; // need a destructor for nArray
	};
    
    //-------------------------------------------
    /// data    
	nArray<BoneInfluence> InfluencesList;

    //-------------------------------------------
    /**
    */
	int GetNumInfuences()
	{
		return InfluencesList.Size();
	}

    //-------------------------------------------
    /**
    */
	vector4 GetWeightsVector()
	{
		vector4 vec(0,0,0,0);
		for(int i=0;i<min(InfluencesList.Size(),4);i++)
		{
			BoneInfluence& influence = InfluencesList[i];
			switch(i)
			{
			case 0:
				vec.x = influence.fWeight;
				break;
			case 1:
				vec.y = influence.fWeight;
				break;
			case 2:
				vec.z = influence.fWeight;
				break;
			case 3:
				vec.w = influence.fWeight;
				break;
			}
		}

		return vec;
	}

    //-------------------------------------------
    /**
    */
	vector4 GetIndicesVector()
	{
		vector4 vec(0,0,0,0);
		for(int i=0;i<min(InfluencesList.Size(),4);i++)
		{
			BoneInfluence& influence = InfluencesList[i];
			switch(i)
			{
			case 0:
				vec.x = (float)influence.iBone;
				break;
			case 1:
				vec.y = (float)influence.iBone;
				break;
			case 2:
				vec.z = (float)influence.iBone;
				break;
			case 3:
				vec.w = (float)influence.iBone;
				break;
			}
		}

		return vec;
	}

    //-------------------------------------------
    /**
    */
	void AddBoneInfluence(int iBone, float fWeight)
	{
		BOOL bAddFlag = TRUE;

		for(int i=0;i<InfluencesList.Size();i++)
		{
			BoneInfluence& influence = InfluencesList[i];
			if (influence.iBone == iBone)
			{
				influence.fWeight += fWeight;
				bAddFlag = FALSE;
				break;
			}
		}

		if (bAddFlag)
		{
			BoneInfluence Infliuence;
			Infliuence.iBone = iBone;
			Infliuence.fWeight = fWeight;

			InfluencesList.Append(Infliuence);
		}
	}

    //-------------------------------------------
    /**
        removes tiny influences
    */
	void AdjustBonesInfluence(float fWeightTreshold)
	{
		int i;

		for( i=0; i < InfluencesList.Size(); i++ )
		{
			BoneInfluence& influence = InfluencesList[i];

			if (influence.fWeight < fWeightTreshold)
			{
				InfluencesList.Erase(i);
			}
		}

		while (InfluencesList.Size() > 4)
		{
			// find min influence
			float fMinInfluence = 1.0f;
			int MinInfluencePos = -1; //out of bounds

			for( i=0; i < InfluencesList.Size(); i++)
			{
				BoneInfluence& influence = InfluencesList[i];

				if (influence.fWeight < fMinInfluence)
				{
					fMinInfluence = influence.fWeight;
					MinInfluencePos = i;
				}
			}

            if( MinInfluencePos >= 0 ) 
            {
    			InfluencesList.Erase(MinInfluencePos);
            }
		}

		// normalize infuences
		float fTotalWeight = 0.0f;
		for(i=0;i<InfluencesList.Size();i++)
		{
			BoneInfluence& influence = InfluencesList[i];
			fTotalWeight += influence.fWeight;
		}

		if ((fTotalWeight < TINY) && (InfluencesList.Size() > 0))
		{
			fTotalWeight = 1.0f / (float) InfluencesList.Size();
		}

		for(i=0;i<InfluencesList.Size();i++)
		{
			BoneInfluence& influence = InfluencesList[i];
			influence.fWeight /= fTotalWeight;
		}
	}
};

//------------------------------------------------------------------------------
/**
    mesh fragment data struct
*/
struct n3dsMeshFragment
{
	int iGroupMappingIndex;
	nArray<int> BonePaletteArray;
};

//------------------------------------------------------------------------------
/**
*/
class n3dsAnimationStateDesc
{
public:
    //-------------------------------------------
    /**
        default constructor
    */
	n3dsAnimationStateDesc()
		: fFadeInTime(0.0f)
		, iFirstFrame(0)
		, bAnimationRepeat(true)
		, AnimClipsArray(0,10)
	{
        //empty
	}

    //-------------------------------------------
    /**
        constructor
    */
    n3dsAnimationStateDesc(int pariFirstFrame, float parfFadeInTime, bool parbAnimationRepeat = true)
		: fFadeInTime(parfFadeInTime)
		, iFirstFrame(pariFirstFrame)
		, bAnimationRepeat(parbAnimationRepeat)
		, AnimClipsArray(0,10)
	{
		AddClip("one");
	}

    //-------------------------------------------
    /**
       default destructor
    */
    ~n3dsAnimationStateDesc()
    {
        //empty
    }

	nArray<nString> AnimClipsArray;
 
    void AddClip(const char *pszChannelName)
	{
        nString chanName(pszChannelName);
	    AnimClipsArray.Append(chanName);
    }

	bool IsHasClips()
	{
		return AnimClipsArray.Size()>0;
	}

	float fFadeInTime;
	int iFirstFrame;
	bool bAnimationRepeat;
};

//------------------------------------------------------------------------------
/**
*/
class n3dsAnimationSettings
{
public:
    //-------------------------------------------
    /**
        default constructor
    */
	n3dsAnimationSettings()
		: iFirstFrame(0)
		, iLastFrame(30)
		, strStateChannelName(_T("charState"))
        , strAnimationFile(_T(""))
		, iFramesPerSample(1)
		, bSaveAsNax2(false)
        , bAddToExist(false)
        , bExportAnim(false)
        , bExportRagdoll(false)
	{
	    AddState(0, 0.0f, true);
    }

    //-------------------------------------------
    /**
        default destructor
    */
    ~n3dsAnimationSettings()
    {
        //empty
    }

    //-------------------------------------------
    /**
        OBSOLETE
    */
    void AddState(int pariFirstFrame, float parfFadeInTime, bool parbAnimationRepeat)
	{
		AnimStatesArray.Append(n3dsAnimationStateDesc(pariFirstFrame, parfFadeInTime, parbAnimationRepeat));
	}

    //-------------------------------------------
    /**
    */
	int GetNumFrames()
	{
        return (iLastFrame + 1) -iFirstFrame; 
	}

    nArray<n3dsAnimationStateDesc> AnimStatesArray; //OBSOLETE

	int iFirstFrame;
	int iLastFrame;
	int iFramesPerSample;

	float fKeyDuration;

	nString strStateChannelName;
	BOOL bSaveAsNax2;
    nString strAnimationFile;
    BOOL bAddToExist;
    BOOL bExportAnim;
    BOOL bExportRagdoll;
};

//------------------------------------------------------------------------------
/**
    dallows conversion between dynamicattachment helpers and n3dsExporter attachments
*/
class n3dsDynAttach
{
public:
    /// default constructor
    n3dsDynAttach():
      jointIndex(-1),
      name(_T("")),
      localTM(),
      iGNodeId(-1)
    {
        ;
    };

    /// constructor
    n3dsDynAttach( const char * attName, int jtIndex, Matrix3 matrix, int nodeId) : 
        jointIndex(jtIndex), 
        name(attName), 
        iGNodeId(nodeId), 
        localTM(matrix)
    {
        //
    };

    /// get joint index
    int GetJointIndex()
    {
        return this->jointIndex;
    };

    /// get helper name
    nString  GetAtName()
    {
        return this->name;
    };

    /// get transformation matrix
    Matrix3 GetTransformation() const 
    {
        return this->localTM;
    };

private:
    int jointIndex;
    nString name;
    Matrix3 localTM;
    int iGNodeId;
};
//------------------------------------------------------------------------------
/**
*/