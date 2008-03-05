#ifndef N_CHARACTERANIMATOR_H
#define N_CHARACTERANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nCharacterAnimator
    @ingroup NebulaCharacterAnimationSystem

    @brief A class to provide an alternative animation system for character.

    On calling Animate(), the nCharacterAnimator can update the character's
    skeleton by any means necessary.

    (C) 2005 Conjurer Services, S.A.
*/

class nCharacter2;
class nVariableContext;
//------------------------------------------------------------------------------
class nCharacterAnimator
{
public:
    /// constructor
    nCharacterAnimator();
    /// destructor
    ~nCharacterAnimator();
    /// update the character skeleton
    virtual bool Animate(nCharacter2* character, nVariableContext* varContext);
};

//------------------------------------------------------------------------------
#endif
