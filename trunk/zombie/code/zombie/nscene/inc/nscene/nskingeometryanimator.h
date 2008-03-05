#ifndef N_SKINGEOMETRYANIMATOR_H
#define N_SKINGEOMETRYANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nSkinGeometryAnimator
    @ingroup SceneAnimators

    @brief Provide an animated joint skeleton for a nSkinGeometryNode.

    On Animate() invocation, the nSkinAnimator will update its joint
    skeleton and invoke SetCharSkeleton() on the calling 
    scene node (which must be a nSkinShapeNode) with a pointer
    to an uptodate nCharSkeleton object.

    See also @ref N2ScriptInterface_nskinanimator
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nanimator.h"
#include "ncharacter/ncharskeleton.h"
#include "ncharacter/ncharacter2.h"
#include "nanimation/nanimstate.h"

class nAnimation;
class nAnimationServer;

//------------------------------------------------------------------------------
class nSkinGeometryAnimator : public nAnimator
{
public:
    enum
    {
        InvalidIndex = 0xffffffff
    };
    /// constructor
    nSkinGeometryAnimator();
    /// destructor
    virtual ~nSkinGeometryAnimator();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context is going to be released
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);
    
    /// begin configuring the character skeleton
    void BeginJoints(int numJoints);
    /// add a joint to the skeleton
    void SetJoint(int index, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale);
    /// finish configuring the joint skeleton
    void EndJoints();
    /// get number of joints in skeleton
    int GetNumJoints();
    /// get joint attributes
    void GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale);
    /// add a joint name
    void AddJointName(unsigned int joint, const char *name);
    /// get a joint index by name
    unsigned int GetJointByName(const char *name);

    /// begin anim state definition
    void BeginStates(int num);
    /// add an animation state
    void SetState(int stateIndex, int animGroupIndex, float fadeInTime);
    /// set animation file for state (animation-driven)
    void SetStateAnim(int stateIndex, const char* filename);
    /// get animation file for state (animation-driven)
    const char* GetStateAnim(int stateIndex);
    /// set channel name for external animation agent
    void SetStateAnimChannel(int stateIndex, const char* animChannel);
    /// set optional state name
    void SetStateName(int stateIndex, const nString& name);
    /// finish adding states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// get state attributes
    const nAnimState& GetStateAt(int stateIndex);
    
    /// begin adding clips to a state
    void BeginClips(int stateIndex, int numClips);
    /// add an animation clip to a state
    void SetClip(int stateIndex, int clipIndex, const char* weightChannelName);
    /// finish adding clips to a state
    void EndClips(int stateIndex);
    /// get number of animations in a state
    int GetNumClips(int stateIndex) const;
    /// get animation attributes
    void GetClipAt(int stateIndex, int animIndex, const char*& weightChannelName);

protected:

    class JointNameNode : public nStrNode
    {
    public:
        /// constructor
        JointNameNode() : jointIndex(0)
        {
        }
        /// set joint index
        void SetJointIndex(int index)
        {
            this->jointIndex = index;
        }
        /// get joint index
        int GetJointIndex()
        {
            return this->jointIndex;
        }
    private:
        unsigned int jointIndex;
    };

    /// load animation resources
    bool LoadAnims();
    /// unload animation resources
    void UnloadAnims();

    nCharacter2 character;
    nAnimStateArray animStateArray;
    nStrList jointNameList;
    int characterVarIndex;
    int frameIdVarIndex;
};

//------------------------------------------------------------------------------
#endif
