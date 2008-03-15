#ifndef NGPCROUCH_H
#define NGPCROUCH_H

//------------------------------------------------------------------------------
/**
    @class nGPCrouch

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncSoundLayer;
class ncLogicAnimator;
class ncGameplayPlayer;

class nGPCrouch : public nGPBasicAction 
{
public:
	/// constructor
	nGPCrouch();
	/// destructor
	~nGPCrouch();

		/// Initial condition
	 bool Init(nEntityObject*, bool, bool);
	
	/// Main execution loop
	bool Run();

    /// End basic action
    void End();

private:
    ncLogicAnimator* animator;
	ncSoundLayer*	 soundLayer;
    ncGameplayPlayer* player;

    // properties for camera control when crouching
    float animIncr;
    float height;
    float finalHeight;
    int animIndex;
    float initTime;
    float totalTime;

};

#endif//NGPCROUCH_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
