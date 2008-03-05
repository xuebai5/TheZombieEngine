#ifndef NC_RAGDOLLCLASS_H
#define NC_RAGDOLLCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncRagDollClass
    @ingroup Scene
    @brief A rag doll class communality interface.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Ragdoll Interface
    
    @cppclass ncRagDollClass
    
    @superclass 

    @classinfo A rag doll communality interface.
*/

//-----------------------------------------------------------------------------
#include "animcomp/ncskeletonclass.h"

//-----------------------------------------------------------------------------

class ncRagDollClass : public ncSkeletonClass
{

    NCOMPONENT_DECLARE(ncRagDollClass,ncSkeletonClass);

public:
    enum RagDollType{
        Human = 0,
        Scavenger,
        Strider,

        NUMTYPES,
        INVALIDTYPE,
    };

    /// constructor
    ncRagDollClass();

    /// destructor
    ~ncRagDollClass();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// load resources
    bool LoadResources();
    ///// unload resources
    void UnloadResources();

    /// get ragdoll joints key array
    nKeyArray<int>& GetRagdollJointsArray();
    /// get num ragdoll joints
    int GetNumRagdollJoints() const;

    /// get ragdoll type
    RagDollType GetRagDollType() const;

#ifdef __ZOMBIE_EXPORTER__
    /// clean data
    bool CleanData();
#endif

    /// set ragdoll type
    void SetRagType(const char*);
    /// get ragdoll type
    const char * GetRagType() const;

    /// set ragdoll joint for the local joint
    void SetRagdollJoint(int, int);
    /// get ragdoll joint for the local joint
    int GetRagdollJoint(int);
    /// (lod, lodjoint, gfxragjoint)
    void SetJointCorrespondence(int, int, int);
    /// (lod, gfxragjoint)
    int GetGfxLodJointCorrespondence(int, int);
    /// (lod, lodragjoint)
    int GetGfxRagJointCorrespondence(int, int);
    /// (lod, gfxragjoint)
    int GetGfxLodJointCorrespondenceByIndex(int, int);
    /// (lod, lodragjoint)
    int GetGfxRagJointCorrespondenceByIndex(int, int);

    /// convert type string to enum
    static RagDollType StringToType(const char* str);
    /// convert type enum to string
    static const char* TypeToString(RagDollType t);

protected:
    ///
    class nRagCorrespondence
    {
    public:
        /// constructor
        nRagCorrespondence();
        /// destructor
        ~nRagCorrespondence();
        /// add correspondence
        void Add(int lodJoint, int ragJoint);
        /// get raggfxjoint index
        int GetRagCorrespondence(int lodJoint);
        /// get lodjoint index
        int GetLodCorrespondence(int ragJoint);
        /// get raggfxjoint index by index in keyarray
        int GetRagCorrespondenceByIndex(int index);
        /// get lodjoint index by index in keyarray
        int GetLodCorrespondenceByIndex(int index);
        /// get num correspondences
        int GetNumCorrespondences();

    private:
        nKeyArray<int> ragCorrespondence;
    };

    nKeyArray<int> ragdollJoints;
    nArray<nRagCorrespondence*> lodRagCorrespondence;

    RagDollType ragDollType;

};

#endif