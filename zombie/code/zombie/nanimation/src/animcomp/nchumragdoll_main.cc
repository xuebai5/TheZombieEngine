#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "animcomp/nchumragdoll.h"
#include "animcomp/nchumragdollclass.h"
#include "ncragdoll/ncragdoll.h"
#include "ncragdoll/ncragdollclass.h"
#include "nscene/nragdollcharanimator.h"

//------------------------------------------------------------------------------
#include "zombieentity/ncdictionary.h"
#include "entity/nentityobjectserver.h"
#include "mathlib/eulerangles.h"

//------------------------------------------------------------------------------
#include "nphysics/ncphyragdolllimb.h"
#include "nphysics/ncphyhumragdoll.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomcylinder.h"
#include "nphysics/nphysicsgeom.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncHumRagdoll,ncRagDoll);

//------------------------------------------------------------------------------
/**
*/
ncHumRagdoll::ncHumRagdoll() :
    active(false)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncHumRagdoll::~ncHumRagdoll()
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
ncHumRagdoll::InitInstance(nObject::InitInstanceMsg initType)
{
    this->active = true;

    if (initType == nObject::ReloadedInstance)
    {
        return;
    }

#ifndef NGAME
    if (initType != nObject::ReloadedInstance)
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncHumRagdoll::Limbo, 0);
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncHumRagdoll::UnLimbo, 0);
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
ncHumRagdoll::CreateRagdoll()
{
    //create physics data according to critter
    ncRagDollClass::RagDollType ragType = this->GetClassComponent<ncRagDollClass>()->GetRagDollType();
    switch (ragType)
    {
    case ncRagDollClass::Human:
        this->InitInstanceHuman();
        break;
/** ZOMBIE REMOVE
    case ncRagDollClass::Scavenger:
        return;
    case ncRagDollClass::Strider:
        this->InitInstanceStrider();
        break;
*/
    default:
        this->InitInstanceHuman();
        break;
    }
}
//------------------------------------------------------------------------------
/**
*/
void
ncHumRagdoll::InitInstanceHuman()
{
    int limbIndex = 0;

    // getting a reference to the nPhyHumRagdoll
    ncPhyHumRagDoll* humRagdollComp(this->GetComponent<ncPhyHumRagDoll>());

    n_assert2(humRagdollComp, "It's required to have the nPhyHumRagDoll compoment.");

    // creating ragdoll limbs/body

    // creating main body
    nPhyGeomBox* body(static_cast<nPhyGeomBox*>(nKernelServer::Instance()->New("nphygeombox")));

    n_assert2(body, "Failed to create rag-dolls main body geometry.");

    //body->SetLengths(vector3(phyreal(.4), phyreal(.5), phyreal(0.2)));
    body->SetLengths(vector3(phyreal(.2), phyreal(.3), phyreal(0.1)));

    body->SetOrientation(n_deg2rad(90.f), 0, 0); 

    humRagdollComp->Add(body);

    // creating head
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomSphere* head(static_cast<nPhyGeomSphere*>(nKernelServer::Instance()->New("nphygeomsphere")));

        n_assert2(head, "Failed to create rag-dolls head geometry.");

        head->SetRadius(phyreal(.1));

        limb->Add(head);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetHead(cLimb);

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

        humRagdollComp->SetArm(ncPhyHumRagDoll::left, cLimb);

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

        humRagdollComp->SetArm(ncPhyHumRagDoll::right, cLimb);

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

        humRagdollComp->SetForeArm(ncPhyHumRagDoll::left, cLimb);

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

        humRagdollComp->SetForeArm(ncPhyHumRagDoll::right, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetLeg(ncPhyHumRagDoll::left, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetLeg(ncPhyHumRagDoll::right, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeLeg(ncPhyHumRagDoll::left, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeLeg(ncPhyHumRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    humRagdollComp->SetMass(90);
}

//------------------------------------------------------------------------------
/**
*/
/** ZOMBIE REMOVE
void
ncHumRagdoll::InitInstanceStrider()
{
    int limbIndex = 0;

    // getting a reference to the nPhyHumRagdoll
    ncPhyHumRagDoll* humRagdollComp(this->GetComponent<ncPhyHumRagDoll>());

    n_assert2(humRagdollComp, "It's required to have the nPhyHumRagDoll compoment.");

    // creating ragdoll limbs/body

    // creating main body
    nPhyGeomBox* body(static_cast<nPhyGeomBox*>(nKernelServer::Instance()->New("nphygeombox")));

    n_assert2(body, "Failed to create rag-dolls main body geometry.");

    body->SetLengths(vector3(phyreal(.3), phyreal(.8), phyreal(0.4)));

    n_assert(this->refEntityParentObj.isvalid());
    //int neckJointIdx = this->character->GetSkeleton().GetJointIndexByName(nString("Bip00_Neck.Rag.Body"));
    //vector3 bodyPose = this->refEntityParentObj->GetComponent<ncCharacter>()->GetJointInitialPosition(0);
    //bodyPose.norm();
    //vector3 neckPose = this->refEntityParentObj->GetComponent<ncCharacter>()->GetJointInitialPosition(this->GetClassComponent<ncRagDollClass>()->GetGfxLodJointCorrespondence(0,neckJointIdx));
    //neckPose.norm();
    //vector3 bodyOrientation = neckPose - bodyPose;
    //quaternion quat;
    //quat.set_from_axes(bodyPose, neckPose);
    //matrix33 m(quat);
    //bodyOrientation = m.to_euler();

    //body->SetOrientation(bodyOrientation.x, bodyOrientation.y, bodyOrientation.z);
    body->SetOrientation(0, 0, n_deg2rad(45.f));

    humRagdollComp->Add(body);

    // creating head
    {
        nEntityObject* eLimb(static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewLocalEntityObject("neragdolllimb")));

        n_assert2(eLimb, "Failed to create a nEntityObject entity.");

        this->refLimbs[limbIndex++] = eLimb;

        ncPhyRagDollLimb* limb(eLimb->GetComponent<ncPhyRagDollLimb>());

        n_assert(limb);

        nPhyGeomSphere* head(static_cast<nPhyGeomSphere*>(nKernelServer::Instance()->New("nphygeomsphere")));

        n_assert2(head, "Failed to create rag-dolls head geometry.");

        head->SetRadius(phyreal(.1));

        limb->Add(head);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetHead(cLimb);

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

        humRagdollComp->SetArm(ncPhyHumRagDoll::left, cLimb);

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

        humRagdollComp->SetArm(ncPhyHumRagDoll::right, cLimb);

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

        arm->SetLength(phyreal(.4));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeArm(ncPhyHumRagDoll::left, cLimb);

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

        arm->SetLength(phyreal(.4));
        arm->SetRadius(phyreal(.05));

        arm->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(arm);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeArm(ncPhyHumRagDoll::right, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetLeg(ncPhyHumRagDoll::left, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetLeg(ncPhyHumRagDoll::right, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeLeg(ncPhyHumRagDoll::left, cLimb);

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
        leg->SetRadius(phyreal(.08));

        leg->SetOrientation(n_deg2rad(90.f), 0, 0); 

        limb->Add(leg);

        ncPhyRagDollLimb* cLimb(limb->GetComponent<ncPhyRagDollLimb>());

        n_assert2(cLimb, "Component missing.");

        humRagdollComp->SetForeLeg(ncPhyHumRagDoll::right, cLimb);

        cLimb->SetMass(4);
    }

    humRagdollComp->SetMass(120);
}
*/
//------------------------------------------------------------------------------
/**
*/
bool
ncHumRagdoll::Load(nEntityObject* parentObj)
{
    ncHumRagdollClass* ragClass = this->GetClassComponent<ncHumRagdollClass>();
    n_assert(ragClass);

    this->SetCharacter(n_new(nCharacter2(ragClass->GetCharacter())));
    n_assert(this->character);

    ncPhyHumRagDoll *phyHumRagdoll = this->GetComponent<ncPhyHumRagDoll>();
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
ncHumRagdoll::Unload()
{
    n_delete(this->character);
    this->character = 0;

    n_delete(this->ragAnimator);
    this->ragAnimator = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncHumRagdoll::GetPhysicsBBox(bbox3& bbox)
{
    ncPhyHumRagDoll* humRagdollComp(this->GetComponentSafe<ncPhyHumRagDoll>());

    nPhysicsAABB boundingBox;
    humRagdollComp->GetAABB(boundingBox);

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
    set collected data from gfx
*/
void
ncHumRagdoll::SetJointData(int jointIndex, vector3& midPos, vector3& pos, const quaternion& rot, const vector3& scale)
{
    nCharJoint& charJoint = this->character->GetSkeleton().GetJointAt(jointIndex);
    charJoint.SetTranslate(pos);
    charJoint.SetRotate(rot);
    charJoint.SetScale(scale);

    ncHumRagdollClass* ragClass = this->GetClassComponent<ncHumRagdollClass>();
    n_assert(ragClass);
    int ragJointIndex;
    if (ragClass->GetRagdollJointsArray().Find(jointIndex, ragJointIndex))
    {
        ncPhyHumRagDoll* humRagdollComp(this->GetComponentSafe<ncPhyHumRagDoll>());
        matrix33 m(rot);
        vector3 eulerAngles = m.to_euler();

        switch (ragJointIndex)
        {
        case -1:
            {
                //main body
                vector3 offset;
                switch(this->GetClassComponent<ncRagDollClass>()->GetRagDollType())
                {
                case ncRagDollClass::Human:
                    offset.set(0, 0, float(.25)); // <--- Hardcoded correction (David)
                    break;
/** ZOMBIE REMOVE
                case ncRagDollClass::Strider:
                    offset.set(float(0.1f),float(-0.25),0); // <--- Hardcoded correction (Antonia) (look at nragdollcharanimator, for undo)
                    break;
*/
                }

                offset = matrix33(rot) * offset;
                pos += offset;
                humRagdollComp->SetPosition(pos);                

                humRagdollComp->SetRotation(matrix33(rot));
                break;
            }
        case 0:
            {
                ncPhyRagDollLimb* head = humRagdollComp->GetHeadObj();

                head->SetPosition(pos);
                head->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetNeckPoint(pos);
                break;
            }
        case 1:
            {
                ncPhyRagDollLimb* arm = humRagdollComp->GetLeftArm();

                arm->SetPosition(midPos);
                arm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetShoulderPoint(ncPhyHumRagDoll::left, pos);
                break;
            }
        case 2:
            {
                ncPhyRagDollLimb* arm = humRagdollComp->GetRightArm();

                arm->SetPosition(midPos);
                arm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetShoulderPoint(ncPhyHumRagDoll::right, pos);
                break;
            }
        case 3:
            {
                ncPhyRagDollLimb* foreArm = humRagdollComp->GetLeftForeArm();

                foreArm->SetPosition(midPos);
                foreArm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetElbowPoint(ncPhyHumRagDoll::left, pos);
                break;
            }
        case 4:
            {
                ncPhyRagDollLimb* foreArm = humRagdollComp->GetRightForeArm();

                foreArm->SetPosition(midPos);
                foreArm->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetElbowPoint(ncPhyHumRagDoll::right, pos);
                break;
            }
        case 5:
            {
                ncPhyRagDollLimb* leg = humRagdollComp->GetLeftLeg();

                leg->SetPosition(midPos);
                leg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetWristPoint(ncPhyHumRagDoll::left, pos);
                break;
            }
        case 6:
            {
                ncPhyRagDollLimb* leg = humRagdollComp->GetRightLeg();

                leg->SetPosition(midPos);
                leg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetWristPoint(ncPhyHumRagDoll::right, pos);
                break;
            }
        case 7:
            {
                ncPhyRagDollLimb* foreleg = humRagdollComp->GetForeLeftLeg();

                switch(this->GetClassComponent<ncRagDollClass>()->GetRagDollType())
                {
                case ncRagDollClass::Human:
                    this->FindMidPosForExtremeJoints(midPos, rot);
                    break;
/** ZOMBIE REMOVE
                case ncRagDollClass::Strider:
                    break;
*/
                }
                
                foreleg->SetPosition(midPos);
                foreleg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetKneePoint(ncPhyHumRagDoll::left, pos);
                break;
            }
        case 8:
            {
                ncPhyRagDollLimb* foreleg = humRagdollComp->GetForeRightLeg();

                switch(this->GetClassComponent<ncRagDollClass>()->GetRagDollType())
                {
                case ncRagDollClass::Human:
                    this->FindMidPosForExtremeJoints(midPos, rot);
                    break;
/** ZOMBIE REMOVE
                case ncRagDollClass::Strider:
                    break;
*/
                }

                foreleg->SetPosition(midPos);
                foreleg->SetOrientation(eulerAngles.x, eulerAngles.y, eulerAngles.z);

                humRagdollComp->SetKneePoint(ncPhyHumRagDoll::right, pos);
                break;
            }
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
void
ncHumRagdoll::PhysicsAnimate()
{
    n_assert(this->ragAnimator);

    // theoretically the character skeleton should be already computed in here
    int numJoints = this->character->GetSkeleton().GetNumJoints();
    int jointIndex;

    if (this->active)
    {
        if (this->ragAnimator->AnimateHuman(this->character))
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
ncHumRagdoll::InitializeRagdoll()
{
    ncPhyHumRagDoll* humRagdollComp(this->GetComponentSafe<ncPhyHumRagDoll>());

    humRagdollComp->Assemble();
}

//------------------------------------------------------------------------------
/**
    Switchs off the ragdoll.

    history:
        - 07-Nov-2005   David Reyes    created

*/
void ncHumRagdoll::SwitchOff()
{
    this->active = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncHumRagdoll::FindMidPosForExtremeJoints(vector3& midPos, const matrix33& rotMatrix)
{
    vector3 offset(0, float(-.2), 0); 
    offset = rotMatrix * offset;
    midPos += offset;
}
