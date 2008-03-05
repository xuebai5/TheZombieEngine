#ifndef NGPUSEMEDPACK_H
#define NGPUSEMEDPACK_H
//------------------------------------------------------------------------------
/**
    @class
    @ingroup
    
    Description of the porpouse of the class
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class ncLogicAnimator;
class ncGameplayLiving;

//------------------------------------------------------------------------------
class nGPUseMedpack : public nGPBasicAction
{
public:
    /// constructor
    nGPUseMedpack();
    /// destructor
    ~nGPUseMedpack();

        /// Initial condition
        bool Init(nEntityObject*,const char *);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    ncLogicAnimator * animator;
    ncGameplayLiving * gameplayLiving;

    int finalHealth;
    int animIndex;
};

//------------------------------------------------------------------------------
#endif//NGPUSEMEDPACK_H
