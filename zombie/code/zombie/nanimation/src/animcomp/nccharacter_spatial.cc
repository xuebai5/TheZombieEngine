#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacter_spatial.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "animcomp/nchumragdoll.h"
#include "animcomp/ncfourleggedragdoll.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialclass.h"

//------------------------------------------------------------------------------
/**
    change visibility bounding box
*/
void
ncCharacter::SetSpatialBBox()
{
    bbox3 currentBox, box;
    
    if (!this->GetRagdollActive())
    {
        box.begin_extend();
        for (int i=0; i<this->currentStates.Size(); i++)
        {
            // update bbox to maximum (do not consider transition time)
            
            if (!this->GetFirstPersonActive())
            {
                if (this->IsValidStateIndex(this->currentStates[i].GetStateIndex()))
                {
                    this->character[this->GetPhysicsSkelIndex()]->GetAnimStateArray()->GetStateAt(this->currentStates[i].GetStateIndex()).GetBoundingBox(currentBox);
                }
            }
            else
            {
                if (this->IsValidFirstPersonStateIndex(this->fpersonCurrentStates[0].GetStateIndex()))
                {
                    this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetStateAt(this->fpersonCurrentStates[0].GetStateIndex()).GetBoundingBox(currentBox);
                }
            }
        }

        box.extend(currentBox);
    }
    else
    {
        //get bbox from physics
        if (this->humanoidRagdoll)
        {
            this->refRagdoll->GetComponentSafe<ncHumRagdoll>()->GetPhysicsBBox(box);
        }
        else
        {
            this->refRagdoll->GetComponentSafe<ncFourLeggedRagdoll>()->GetPhysicsBBox(box);
        }
    }

    this->GetComponentSafe<ncSpatial>()->SetOriginalBBox(box);
}   

//------------------------------------------------------------------------------
/**
*/
vector3
ncCharacter::GetPerceptionOffset(int stateIndex) const
{
    return this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(stateIndex).GetPerceptionOffset();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetPerceptionOffset(int stateIndex, vector3 perceptionOffset)
{
    return this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(stateIndex).SetPerceptionOffset(perceptionOffset);
}
