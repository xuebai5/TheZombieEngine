#ifndef N_CHARJOINT_H
#define N_CHARJOINT_H
//------------------------------------------------------------------------------
/**
    @class nCharJoint
    @ingroup Character

    @brief A joint in a character skeleton.

     - 06-Feb-03   floh    fixed for Nebula2

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "mathlib/quaternion.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nCharJoint
{
public:
    /// constructor
    nCharJoint();
    /// destructor
    ~nCharJoint();

    /// set parent joint index
    void SetParentJointIndex(int index);
    /// get parent joint index
    int GetParentJointIndex() const;
    /// set parent joint pointer
    void SetParentJoint(nCharJoint* p);
    /// get the parent joint
    nCharJoint* GetParentJoint() const;
    
    /// set child joint index
    void SetChildJointIndex(int childIndex);
    /// get child joint index
    int GetChildJointIndex(int index) const;
    /// set child joint pointer
    void SetChildJoint(nCharJoint* p);
    /// get child joint
    nCharJoint* GetChildJoint(int index) const;
    
    /// set the pose
    void SetPose(const vector3& t, const quaternion& q, const vector3& s);
    /// get pose translate
    const vector3& GetPoseTranslate() const;
    /// get pose rotate
    const quaternion& GetPoseRotate() const;
    /// get pose scale
    const vector3& GetPoseScale() const;
    /// set translation
    void SetTranslate(const vector3& t);
    /// get translation
    const vector3& GetTranslate() const;
    /// set rotation
    void SetRotate(const quaternion& q);
    /// get rotation
    const quaternion& GetRotate() const;
    /// set scale
    void SetScale(const vector3& s);
    /// get scale
    const vector3& GetScale() const;
    /// set name
    void SetName(const nString& name);
    /// get name
    const nString& GetName() const; 
    /// evaluate joint
    /// evaluate joint from parent coordinates
    void Evaluate();
    /// evaluate world from world coordinates
    void EvaluateWorldCoord();
    
    /// directly set the local matrix
    void SetLocalMatrix(const matrix44& m);
    /// get current evaluated matrix in local space, valid after Evaluate()!
    const matrix44& GetLocalMatrix() const;
    /// get the bind pose matrix in model space
    const matrix44& GetPoseMatrix() const;
    /// get the inverse pose matrix in model space
    const matrix44& GetInvPoseMatrix() const;
    /// set model space matrix directly, this will disable multiplication by parent joint
    void SetMatrix(const matrix44& m);
    /// get current evaluated matrix in model space, valid after Evaluate()!
    const matrix44& GetMatrix() const;
    /// get the skinning matrix with translation
    const matrix44& GetSkinMatrix44() const;
    /// get the skinning matrix without translation (for normals)
    const matrix33& GetSkinMatrix33() const;
    /// clear the uptodate flag
    void ClearUptodateFlag();
    /// return true if the joint has been evaluated 
    bool IsUptodate() const;

    /// get world coordinates quaternion
    const quaternion GetWorldQuaternion();
    /// get world transformations
    void GetWorldTransformation(vector3& position, quaternion& rotation, vector3& scale) const;
    /// get midpoint joint
    const vector3 GetMidPoint();
    /// get world position
    const vector3 GetWorldPosition() const;
    /// get number of child joints
    const int NumberChildJoints() const;

private:
    vector3 poseTranslate;
    quaternion poseRotate;
    vector3 poseScale;

    vector3 translate;
    quaternion rotate;
    vector3 scale;

    matrix44 poseMatrix;
    matrix44 invPoseMatrix;

    matrix44 matrix;            // the current evaluated matrix in model space
    matrix44 localMatrix;       // the current evaluated matrix in local (parent) space
    matrix44 skinMatrix44;
    matrix33 skinMatrix33;
    int parentJointIndex;
    nCharJoint* parentJoint;
    bool matrixDirty;

    nArray<int> childJointIndices;
    nArray<nCharJoint*> childJoints;
    bool lockMatrix;
    bool isUptodate;
    
    nString name;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::nCharJoint() :
    parentJoint(0),
    parentJointIndex(-1),
    childJointIndices(2,2),
    childJoints(0,2),
    poseScale(1.0f, 1.0f, 1.0f),
    scale(1.0f, 1.0f, 1.0f),
    matrixDirty(false),
    lockMatrix(false),
    isUptodate(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::~nCharJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetParentJoint(nCharJoint* p)
{
    this->parentJoint = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint*
nCharJoint::GetParentJoint() const
{
    return this->parentJoint;
}

//------------------------------------------------------------------------------
/**
    The parent joint index can be -1 if this joint is the root joint.
*/
inline
void
nCharJoint::SetParentJointIndex(int index)
{
    this->parentJointIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharJoint::GetParentJointIndex() const
{
    return this->parentJointIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetPose(const vector3& t, const quaternion& q, const vector3& s)
{
    this->poseTranslate = t;
    this->poseRotate = q;
    this->poseScale = s;

    this->poseMatrix.ident();
    this->poseMatrix.scale(this->poseScale);
    this->poseMatrix.mult_simple(matrix44(this->poseRotate));
    this->poseMatrix.translate(this->poseTranslate);

    // set the initial matrix so that it undoes the pose matrix
    this->localMatrix = poseMatrix;
    this->matrix = poseMatrix;

    // globale pose matrix and compute global inverse pose matrix
    if (this->parentJoint)
    {
        this->poseMatrix.mult_simple(this->parentJoint->poseMatrix);
    }
    this->invPoseMatrix = this->poseMatrix;
    this->invPoseMatrix.invert_simple();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetPoseTranslate() const
{
    return this->poseTranslate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion& 
nCharJoint::GetPoseRotate() const
{
    return this->poseRotate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetPoseScale() const
{
    return this->poseScale;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetTranslate(const vector3& t)
{
    this->translate = t;
    this->matrixDirty = true;
    this->isUptodate = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetTranslate() const
{
    return this->translate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetRotate(const quaternion& q)
{
    this->rotate = q;
    this->matrixDirty = true;
    this->isUptodate = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nCharJoint::GetRotate() const
{
    return this->rotate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetScale(const vector3& s)
{
    this->scale = s;
    this->matrixDirty = true;
    this->isUptodate = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
    Clear the uptodate flag. This flag is used in Evaluate() to check
    whether a parent joint has already been evaluated.
*/
inline
void
nCharJoint::ClearUptodateFlag() 
{
    this->isUptodate = false;
}

//------------------------------------------------------------------------------
/**
    Return true when this joint is uptodate (set when Evaluate()) has been
    called after ClearUptodateFlag().
*/
inline
bool
nCharJoint::IsUptodate() const
{
    return this->isUptodate;
}

//------------------------------------------------------------------------------
/**
    This computes the skinning matrix from the pose matrix, the translation,
    the rotation and the scale of the joint. The parent joint must already be
    uptodate!
*/
inline
void
nCharJoint::Evaluate()
{
    if (!this->isUptodate)
    {
        // any changes in position/rotation/etc ?
        if (this->matrixDirty)
        {
            this->localMatrix.ident();

            /// @todo ma.garcias - HMM --> Scale doesn't work too well... (with ragdolls)
            this->localMatrix.scale(this->scale);
            this->localMatrix.mult_simple(matrix44(this->rotate));
            this->localMatrix.translate(this->translate);
            this->matrixDirty = false;
        }

        if (!this->lockMatrix)
        {
            this->matrix = this->localMatrix;
            if (this->parentJoint)
            {
                if (!this->parentJoint->IsUptodate())
                {
                    this->parentJoint->Evaluate();
                }
                this->matrix.mult_simple(this->parentJoint->matrix);
            }
        }
        this->skinMatrix44 = this->invPoseMatrix * this->matrix;
        this->skinMatrix33.set(this->skinMatrix44.M11, this->skinMatrix44.M12, this->skinMatrix44.M13,
                            this->skinMatrix44.M21, this->skinMatrix44.M22, this->skinMatrix44.M23,
                            this->skinMatrix44.M31, this->skinMatrix44.M32, this->skinMatrix44.M33);
        this->isUptodate = true;
    }
}

//------------------------------------------------------------------------------
/**
    Return the 4x4 skinning matrix. This is the current evaluated matrix
    multiplied by the inverse bind pose matrix.
*/
inline
void
nCharJoint::EvaluateWorldCoord()
{
    if (!this->isUptodate)
    {
        if (this->matrixDirty)
        {
            this->matrix.ident();
            this->matrix.scale(this->scale);
            this->matrix.mult_simple(matrix44(this->rotate));
            this->matrix.translate(this->translate);
            this->matrixDirty = false;
        }

        this->skinMatrix44 = this->invPoseMatrix * this->matrix;
        this->skinMatrix33.set(this->skinMatrix44.M11, this->skinMatrix44.M12, this->skinMatrix44.M13,
                            this->skinMatrix44.M21, this->skinMatrix44.M22, this->skinMatrix44.M23,
                            this->skinMatrix44.M31, this->skinMatrix44.M32, this->skinMatrix44.M33);
        this->isUptodate = true;
    }
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::GetWorldTransformation(vector3& position, quaternion& rotation, vector3& scale) const
{
    position = this->matrix.pos_component();
    rotation = this->matrix.get_quaternion();
    scale = vector3(1,1,1);
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion
nCharJoint::GetWorldQuaternion()
{
    quaternion rotation = this->poseRotate;
    nCharJoint* parentjoint = this->parentJoint;
    while (parentjoint)
    {
        rotation = parentjoint->GetPoseRotate() * rotation;
        parentjoint = parentjoint->GetParentJoint();
    }
    return rotation;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nCharJoint::GetSkinMatrix44() const
{
    return this->skinMatrix44;
}

//------------------------------------------------------------------------------
/**
    Return the upper-left 3x3 part of the skinning matrix.
*/
inline
const matrix33&
nCharJoint::GetSkinMatrix33() const
{
    return this->skinMatrix33;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetName(const nString& name)
{
    this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nCharJoint::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
    Return the bind pose matrix. This matrix is already flattened into 
    model space.
*/
inline
const matrix44&
nCharJoint::GetPoseMatrix() const
{
    return this->poseMatrix;
}

//------------------------------------------------------------------------------
/**
    Return the inverse bind pose matrix.
*/
inline
const matrix44&
nCharJoint::GetInvPoseMatrix() const
{
    return this->invPoseMatrix;
}

//------------------------------------------------------------------------------
/**
    Set the model space matrix directly. This sets a flag for Evaluate()
    which tells it not to multiply by the parent joint's matrix.
*/
inline
void
nCharJoint::SetMatrix(const matrix44& m)
{
    this->matrix = m;
    this->lockMatrix = true;
}

//------------------------------------------------------------------------------
/**
    Return the current evaluated matrix in model space. This matrix is only
    valid after Evaluate() has been called.
*/
inline
const matrix44&
nCharJoint::GetMatrix() const
{
    return this->matrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3 
nCharJoint::GetMidPoint()
{
    vector3 jointExtreme1 = this->matrix.pos_component();

    if (this->NumberChildJoints() == 0)
    {
        return jointExtreme1;
    }

    vector3 jointExtreme2;
    
    // not body
    if (this->NumberChildJoints() != 3)
    {
        jointExtreme2 = this->GetChildJoint(0)->matrix.pos_component();
    }
    else
    {
        /// @todo ma.garcias - 2 is because neck is the third one, better compare (but i have to do it by name
        jointExtreme2 = this->GetChildJoint(2)->matrix.pos_component();
    }

    return (jointExtreme1 + jointExtreme2)/2;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nCharJoint::SetChildJointIndex(int childIndex)
{
    this->childJointIndices.Append(childIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nCharJoint::GetChildJointIndex(int index) const
{
    return this->childJointIndices[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nCharJoint::SetChildJoint(nCharJoint* p)
{
    this->childJoints.Append(p);
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint* 
nCharJoint::GetChildJoint(int index) const
{
    return this->childJoints[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nCharJoint::NumberChildJoints() const
{
    return this->childJoints.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3
nCharJoint::GetWorldPosition() const
{
    return this->matrix.pos_component();
}
//------------------------------------------------------------------------------
/**
    Directly set the local matrix. This will clear the dirty flag, which
    means the joint's translate/rotate/scale components are disabled.
*/
inline
void
nCharJoint::SetLocalMatrix(const matrix44& m)
{
    this->localMatrix = m;
    this->matrixDirty = false;
    this->isUptodate = false;
}

//------------------------------------------------------------------------------
/**
    Return the current evaluated matrix in local space. This matrix is only
    valid after Evaluate() has been called.
*/
inline
const matrix44&
nCharJoint::GetLocalMatrix() const
{
    return this->localMatrix;
}

//------------------------------------------------------------------------------
#endif
