#ifndef NC_SKELETONCLASS_H
#define NC_SKELETONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSkeletonClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Skeleton component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "character2/ncharacter2.h"
#include "animcomp/ndynamicattachment.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class ncSkeletonClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSkeletonClass,nComponentClass);

public:
    enum
    {
        InvalidIndex = 0xffffffff
    };

    /// constructor
    ncSkeletonClass();
    /// destructor
    ~ncSkeletonClass();
    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// get access to internal character
    nCharacter2& GetCharacter();

    /// load resources
    virtual bool LoadResources();
    ///// unload resources
    virtual void UnloadResources();

    /// load upper level resources to use in that level
    bool LoadUpperLevelResources();

    /// get state attributes
    const nAnimState& GetStateAt(int stateIndex) const;
    /// add state
    void AddState(int animGroupIndex, int jointGroupIndex, float fadeInTime);

    /// get attachments array
    nArray<nDynamicAttachment> GetDynAttachments();
    /// get attachment
    const nDynamicAttachment& GetDynAttachment(int index);

#ifdef __ZOMBIE_EXPORTER__
    /// clean data
    bool CleanData();
#endif

    /// set animation file for state (animation-driven)
    void SetStateAnim(int, const char *);
    /// get animation file for state (animation-driven)
    const char * GetStateAnim(int) const;
    /// set motion file for state (animation-driven)
    void SetStateMotion(int, const char *);
    /// get motion file for state (animation-driven)
    const char * GetStateMotion(int) const;
    /// begin configuring the character skeleton
    void BeginJoints(int);
    /// add a joint to the skeleton (not persisted, kept for backwards compatibility)
    void SetJoint(int, int&, const vector3&, const quaternion&, const vector3&);
    /// add a joint to the skeleton
    void SetNamedJoint(int, int&, const vector3&, const quaternion&, const vector3&, const nString&);
    /// finish configuring the joint skeleton
    void EndJoints();
    /// get number of joints in skeleton
    int GetNumJoints();
    /// add a joint name
    void AddJointName(int, const char *);
    /// get joint name
    const nString GetJointName(int);
    /// get joint attributes
    void GetJoint(int, int&, vector3&, quaternion&, vector3&);
    /// begin anim state definition
    void BeginStates(int);
    /// add an animation state
    void SetState(int, int, float);
    /// add animation state and jointgroup
    void SetStateWithJointGroup(int, int, int, float);
    /// set optional state name
    void SetStateName(int, const char *);
    /// get state name
    const char * GetStateName(int);
    /// finish adding states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// begin adding clips to a state
    void BeginClips(int, int);
    /// add an animation clip to a state (uses full skeleton)
    void SetClip(int, int, const char*);
    /// set clip with a given number of curves
    void SetClipAndCurves(int, int, int , const char*);
    /// finish adding clips to a state
    void EndClips(int);
    /// get number of animations in a state
    int GetNumClips(int);
    /// set perceptionoffset
    void SetPerceptionOffset(int, vector3);
    /// get perceptionoffset
    vector3 GetPerceptionOffset(int) const;

    /// get a joint index by name
    unsigned int GetJointByName(const char *);
    /// get animation attributes
    const char * GetClipAt(int, int);
    /// set channel name for external animation agent
    void SetStateAnimChannel(int, const char*);
    /// set state bounding box
    void SetStateBBox(int, vector3&, vector3&);
    /// get state bounding box
    void GetStateBBox(int, vector3&, vector3&);

    /// set fade in time to an state
    void SetFadeInTime(int, float);
    /// get fade in time to an state
    float GetFadeInTime(int);

    /// begin joint groups
    void BeginJointGroups(int);
    /// set group number of joints
    void SetGroupNumberJoints(int, int);
    /// set joint group
    void SetGroup(int, int, int, int, int);
    /// end joint groups
    void EndJointGroups();
    /// assign group to state
    void AssignGroupToState(int, int);

    /// begin tweakable joints
    void BeginTweakableJoints(int);
    /// set tweakable joint
    void SetTweakableJoint(int, int, int);
    /// end tweakable joints
    void EndTweakableJoints();

    /// begin attachment helpers
    void BeginAttachments(int);
    /// set attachment helper
    void SetAttachmentHelper(int, const char *, int, vector3, quaternion);
    /// end attachment helpers
    void EndAttachments();

    /// remove anim state
    void RemoveAnimState(int);
    /// rename anim state
    bool ChangeAnimStateName(int, const nString&);

    /// set submissive and leader state
    void SetSubmissiveLeaderState(const nString&, const nString&);
    /// get leader state
    int GetLeaderStateOf(int);
    /// get leader state
    const nString GetLeaderStateNameOf(int);

    /// save class
    bool SaveClass();

    /// set state follows joint
    void SetStateFollowsJoint(const nString&, const nString&);
    /// get state follows joint
    const nString GetStateFollowsJoint(const nString&);

    /// get chunk path
    nString GetChunkPath();
    /// save chunk
    bool SaveChunk();

private:
    /// load animation resources
    bool LoadAnims();
    /// unload animation resources
    void UnloadAnims();

    /// load motion resources
    bool LoadMotion();
    /// unload motion resources
    void UnloadMotion();

    /// load chunk
    bool LoadChunk();

    //// get tweakable joints path
    nString GetTweakableJointsPath();
    /// load tweakable joints file
    bool LoadTweakableJoints();
    /// save tweakable joints file
    bool SaveTweakableJoints();

    /// dynamic attachment pool
    nArray<nDynamicAttachment> dynamicAttachmentPool;

    nCharacter2 character;

    nAnimStateArray animStateArray; //use that, not the character one

    bool chunkLoaded;
    /// arrays to keep old/future data (animstates that will be exported in the future and were exported in the past)
    nArray<nString> futureSubmissiveNames;
    nArray<nString> futureLeaderNames;

};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2&
ncSkeletonClass::GetCharacter()
{
    return this->character;
}

//------------------------------------------------------------------------------
#endif

