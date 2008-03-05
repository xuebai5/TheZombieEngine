#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/ncfourleggedragdoll.h"
#include "animcomp/ncfourleggedragdollclass.h"
#include "nscene/nragdollcharanimator.h"

#include "animcomp/nccharacter.h"

//------------------------------------------------------------------------------
#include "zombieentity/ncdictionary.h"
#include "entity/nentityobjectserver.h"
#include "mathlib/eulerangles.h"

//------------------------------------------------------------------------------
#include "nphysics/ncphyragdolllimb.h"
#include "nphysics/ncphyhumragdoll.h"
#include "nphysics/ncphyfourleggedragdoll.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomcylinder.h"
#include "nphysics/nphysicsgeom.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncFourLeggedRagdoll,ncRagDoll);

//------------------------------------------------------------------------------
/**
*/
ncFourLeggedRagdoll::ncFourLeggedRagdoll() : 
    active(false)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncFourLeggedRagdoll::~ncFourLeggedRagdoll()
{
    for (int i=0; i< NumLimbs; i++)
    {
        if (this->refLimbs[i].isvalid())
        {
           nEntityObjectServer::Instance()->RemoveEntityObject(this->refLimbs[i]);
        }
    }
    
    this->Unload();
}

//------------------------------------------------------------------------------
/**
*/
void
ncFourLeggedRagdoll::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType == nObject::ReloadedInstance)
    {
        return;
    }

    this->active = true;

    int limbIndex = 0;

    // getting a reference to the nPhyHumRagdoll
    ncPhyFourleggedRagDoll* fourLRagdollComp(this->GetComponent<ncPhyFourleggedRagDoll>());

    n_assert2(fourLRagdollComp, "It's required to have the ncFourLeggedHumRagDoll compoment.");

    // creating ragdoll limbs/body

    // creating main body
    nPhyGeomBox* body(static_cast<nPhyGeomBox*>(nKernelServer::Instance()->New("nphygeombox")));

    n_assert2(body, "Failed to create rag-dolls main body geometry.");

    body->SetLengths(vector3(phyreal(.5), phyreal(0.2), phyreal(.4)));

    fourLRagdollComp->Add(body);

    // creating head
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomSphere* head(static_cast<nPhyGeomSphere*>(nKernelServer::Instance()->New("nphygeomsphere")));

        n_assert2(head, "Failed to create rag-dolls head geometry.");

        head->SetRadius(phyreal(.15));

        limb->Add(head);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetHead(cLimb);

        cLimb->SetMass(2);
        cLimb->SetDensity(phyreal(.5));
    }
    
    // creating left arm
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* arm(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(arm, "Failed to create rag-dolls head geometry.");
        
        arm->SetLength(phyreal(.3));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetArm(ncPhyFourleggedRagDoll::left, cLimb);

        cLimb->SetMass(4);
    }

    // creating right arm
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* arm(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(arm, "Failed to create rag-dolls head geometry.");

        arm->SetLength(phyreal(.3));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetArm(ncPhyFourleggedRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    // creating left fore arm
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* arm(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(arm, "Failed to create rag-dolls head geometry.");

        arm->SetLength(phyreal(.3));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetForeArm(ncPhyFourleggedRagDoll::left, cLimb);

        cLimb->SetMass(4);
    }

    // creating right fore arm
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* arm(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(arm, "Failed to create rag-dolls head geometry.");

        arm->SetLength(phyreal(.3));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetForeArm(ncPhyFourleggedRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    // creating left leg
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* leg(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(leg, "Failed to create rag-dolls head geometry.");

        leg->SetLength(phyreal(.4));
        leg->SetRadius(phyreal(.05));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetLeg(ncPhyFourleggedRagDoll::left, cLimb);

        cLimb->SetMass(4);
    }

    // creating right leg
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* leg(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(leg, "Failed to create rag-dolls head geometry.");

        leg->SetLength(phyreal(.4));
        leg->SetRadius(phyreal(.05));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetLeg(ncPhyFourleggedRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    // creating left fore leg
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* leg(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(leg, "Failed to create rag-dolls head geometry.");

        leg->SetLength(phyreal(.4));
        leg->SetRadius(phyreal(.05));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetForeLeg(ncPhyFourleggedRagDoll::left, cLimb);

        cLimb->SetMass(4);
    }

    // creating right fore leg
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomCylinder* leg(static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New("nphygeomcylinder")));

        n_assert2(leg, "Failed to create rag-dolls head geometry.");

        leg->SetLength(phyreal(.4));
        leg->SetRadius(phyreal(.05));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        fourLRagdollComp->SetForeLeg(ncPhyFourleggedRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    fourLRagdollComp->SetMass(30);

#ifndef NGAME
    if (initType != nObject::ReloadedInstance)
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncFourLeggedRagdoll::Limbo, 0);
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncFourLeggedRagdoll::UnLimbo, 0);
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
ncFourLeggedRagdoll::Load(nEntityObject* parentObj)
{
    ncFourLeggedRagdollClass* ragClass = this->GetClassComponent<ncFourLeggedRagdollClass>();
    n_assert(ragClass);

    this->SetCharacter(n_new(nCharacter2(ragClass->GetCharacter())));
    n_assert(this->character);

    ncPhyFourleggedRagDoll *phyHumRagdoll = this->GetComponent<ncPhyFourleggedRagDoll>();
    n_assert(phyHumRagdoll);

    this->ragAnimator = n_new(nRagdollCharAnimator);
    this->ragAnimator->SetPhyRagdoll(this->GetEntityObject());
    this->ragAnimator->SetRagdollJoints(&ragClass->GetRagdollJointsArray());

    this->refEntityParentObj = parentObj;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncFourLeggedRagdoll::Unload()
{
    n_delete(this->character);
    this->character = 0;

    n_delete(this->ragAnimator);
    this->ragAnimator = 0;
}

//------------------------------------------------------------------------------
/**
    set collected data from gfx
*/
void
ncFourLeggedRagdoll::SetJointData(int jointIndex, vector3& midPos, vector3& pos, const quaternion& rot, const vector3& scale/*, vector3& rotOffset*/)
{
    nCharJoint& charJoint = this->character->GetSkeleton().GetJointAt(jointIndex);
    charJoint.SetTranslate(pos);
    charJoint.SetRotate(rot);
    charJoint.SetScale(scale);

    ncFourLeggedRagdollClass* ragClass = this->GetClassComponent<ncFourLeggedRagdollClass>();
    n_assert(ragClass);
    int ragJointIndex;
    if (ragClass->GetRagdollJointsArray().Find(jointIndex, ragJointIndex))
    {
        ncPhyFourleggedRagDoll* fourleggedRagdollComp(this->GetComponentSafe<ncPhyFourleggedRagDoll>());
        matrix33 m(rot);
        vector3 eulerAngles = m.to_euler();

        switch (ragJointIndex)
        {
        case -1:
            {
                //main body
                vector3 offset(0, float(.1), 0); //scavenger
                offset = matrix33(rot) * offset;
                pos += offset;

                fourleggedRagdollComp->SetPosition(pos);                
                fourleggedRagdollComp->SetRotation(matrix33(rot));
                break;
            }
        case 0:
            {
                ncPhyRagDollLimb* head = fourleggedRagdollComp->GetHeadObj();

                head->SetPosition(pos);
                head->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetNeckPoint(pos);
                break;
            }
        case 1:
            {
                ncPhyRagDollLimb* arm = fourleggedRagdollComp->GetLeftArm();

                arm->SetPosition(midPos);
                arm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetShoulderPoint(ncPhyFourleggedRagDoll::left, pos);
                break;
            }
        case 2:
            {
                ncPhyRagDollLimb* arm = fourleggedRagdollComp->GetRightArm();

                arm->SetPosition(midPos);
                arm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetShoulderPoint(ncPhyFourleggedRagDoll::right, pos);
                break;
            }
        case 3:
            {
                ncPhyRagDollLimb* foreArm = fourleggedRagdollComp->GetLeftForeArm();

                this->FindMidPosForExtremeJoints(midPos, rot);

                foreArm->SetPosition(midPos);
                foreArm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetElbowPoint(ncPhyFourleggedRagDoll::left, pos);
                break;
            }
        case 4:
            {
                ncPhyRagDollLimb* foreArm = fourleggedRagdollComp->GetRightForeArm();

                this->FindMidPosForExtremeJoints(midPos, rot);

                foreArm->SetPosition(midPos);
                foreArm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetElbowPoint(ncPhyFourleggedRagDoll::right, pos);
                break;
            }
        case 5:
            {
                ncPhyRagDollLimb* leg = fourleggedRagdollComp->GetLeftLeg();

                leg->SetPosition(midPos);
                leg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetWristPoint(ncPhyFourleggedRagDoll::left, pos);
                break;
            }
        case 6:
            {
                ncPhyRagDollLimb* leg = fourleggedRagdollComp->GetRightLeg();

                leg->SetPosition(midPos);
                leg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetWristPoint(ncPhyFourleggedRagDoll::right, pos);
                break;
            }
        case 7:
            {
                ncPhyRagDollLimb* foreleg = fourleggedRagdollComp->GetForeLeftLeg();

                this->FindMidPosForExtremeJoints(midPos, rot);

                foreleg->SetPosition(midPos);
                foreleg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetKneePoint(ncPhyFourleggedRagDoll::left, pos);
                break;
            }
        case 8:
            {
                ncPhyRagDollLimb* foreleg = fourleggedRagdollComp->GetForeRightLeg();

                this->FindMidPosForExtremeJoints(midPos, rot);

                foreleg->SetPosition(midPos);
                foreleg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                fourleggedRagdollComp->SetKneePoint(ncPhyFourleggedRagDoll::right, pos);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncFourLeggedRagdoll::PhysicsAnimate()
{
    n_assert(this->ragAnimator);

    // theoretically the character skeleton should be already computed in here
    int numJoints = this->character->GetSkeleton().GetNumJoints();
    int jointIndex;

    if (this->active)
    {
        if (this->ragAnimator->Animate4Legged(this->character))
        {
            for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
            {
                nCharJoint& joint = this->character->GetSkeleton().GetJointAt(jointIndex);
                joint.EvaluateWorldCoord();
            }
        }
    }
    else
    {
        //mantain animation
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            nCharJoint& joint = this->character->GetSkeleton().GetJointAt(jointIndex);
            joint.SetTranslate(joint.GetTranslate());
            joint.SetRotate(joint.GetRotate());
            joint.SetScale(joint.GetScale());
            joint.EvaluateWorldCoord();
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
void
ncFourLeggedRagdoll::InitializeRagdoll()
{
    ncPhyFourleggedRagDoll* fourLeggedRagdollComp(this->GetComponentSafe<ncPhyFourleggedRagDoll>());

    fourLeggedRagdollComp->Assemble();
}

//------------------------------------------------------------------------------
/**
*/
void
ncFourLeggedRagdoll::GetPhysicsBBox(bbox3& bbox)
{
    ncPhyFourleggedRagDoll* fourLeggedRagdollComp(this->GetComponentSafe<ncPhyFourleggedRagDoll>());

    nPhysicsAABB boundingBox;
    fourLeggedRagdollComp->GetAABB(boundingBox);

    //bbox3 bbox;
    bbox.begin_extend();

    for (int i=0; i< 8; i++)
    {
        vector3 vertex;
        boundingBox.GetVertex(i, vertex);
        bbox.extend(vertex);
    }
}


//------------------------------------------------------------------------------
/**
    Switchs off the ragdoll.

    history:
        - 07-Nov-2005   David Reyes    created

*/
void ncFourLeggedRagdoll::SwitchOff()
{
    this->active = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncFourLeggedRagdoll::FindMidPosForExtremeJoints(vector3& midPos, const matrix33& rotMatrix)
{
    vector3 offset(0, float(-.2), 0); //scavenger adaptation
    offset = rotMatrix * offset;
    midPos += offset;
}
