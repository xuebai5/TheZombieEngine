//------------------------------------------------------------------------------
/**
	@file ncsoundlayer.h

	(C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NCSOUNDLAYER_H
#define NCSOUNDLAYER_H

#include "entity/nentity.h"
#include "ncsound/ncsound.h"
#include "rnsgameplay/ngameplayenums.h"

class ncSoundLayer : public ncSound
{

    NCOMPONENT_DECLARE(ncSoundLayer,ncSound);

public:
	/// Constructor
	ncSoundLayer();
	/// Destructor
	~ncSoundLayer();

	// Play a sound not looped
	void PlaySound (ngpAction action);

    /// update sound material
    void UpdateSoundMaterial(float duration=0.0f);

    /// Get game material name under living entity
    const char * GetMaterialNameUnder();

	/// Plays a sound depending the action
 void PlaySound(int, bool);
    /// Plays a sound depending the action and material
    void PlaySoundMaterial(int, const char *, bool);
    /// Plays a sound depending the action with extended parameters
 void PlaySoundTime(int, int, float, bool);
    /// Plays a sound depending the action and material, with extended parameters
    void PlaySoundTimeMaterial(int, int, const char *, float, bool);
    /// Set sound material, relaunching current sound
 void SetSoundMaterial(const char *, float duration);
    /// Stop current sound
   virtual void StopSound();

								
	/// Get the current string action
	const char* GetCurrentAction() const;

    /// Get the current material
	const nString& GetCurrentMaterial() const;

    nString GetRealEventName( const char * event, int descriptor );

private:	
	/// Get the string regarding a state of sound
	const char* GetStringAction (ngpAction action) const;

	ngpAction currentSound;				// The current sound playing
	nString currentMaterial;			// The current material
	bool looped;						// If the sound must be played looped or not
	float duration;
	bool calcVar;

};

//------------------------------------------------------------------------------
/**
	GetCurrentMaterial
*/
inline
const
nString&
ncSoundLayer::GetCurrentMaterial() const
{
	return this->currentMaterial;
}

#endif