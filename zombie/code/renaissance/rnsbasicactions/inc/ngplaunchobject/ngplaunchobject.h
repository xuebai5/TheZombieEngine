#ifndef NGPLAUNCHOBJECT_H
#define NGPLAUNCHOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nGPLaunchObject
    @ingroup NebulaGameplayBasicActions

    Basic action to launch an object

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class ncLogicAnimator;
class ncGameplayLiving;

//------------------------------------------------------------------------------
class nGPLaunchObject : public nGPBasicAction
{
public:
    /// Constructor
    nGPLaunchObject();

    /// Destructor
    ~nGPLaunchObject();

        /// initial condition
        bool Init(nEntityObject*, const char*);

    /// Main execution loop
    bool Run();

    /// Stop condition 
    bool IsDone() const;

private:
    ncLogicAnimator * animator;
    ncGameplayLiving * gameplay;

    int animIndex;
    bool explode;
    bool isHiding;
};

//------------------------------------------------------------------------------
#endif//NGPLAUNCHOBJECT_H
