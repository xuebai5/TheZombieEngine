#ifndef NGPPRONE_H
#define NGPPRONE_H

//------------------------------------------------------------------------------
/**
    @file ngcrouch.h
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncSoundLayer;
class ncGameplayPlayer;
class ncGameplayLiving;

class nGPProne : public nGPBasicAction 
{
public:
	/// constructor
	nGPProne();
    /// Destructor
    ~nGPProne();

		/// Initial condition
	bool Init(nEntityObject*, bool, bool);
	
	/// Main execution loop
	bool Run();

    /// Executed at the end of the basic action
    void End();

private:
    ncLogicAnimator* animator;
    ncSoundLayer*	 soundLayer;
    ncGameplayPlayer* player;
	ncGameplayLiving* gameplay;

    // properties for camera control when proning
    float animIncr;
    float height;
    float finalHeight;
    float initTime;
    float totalTime;

    int animIndex;    
};

#endif//NGPPRONE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
