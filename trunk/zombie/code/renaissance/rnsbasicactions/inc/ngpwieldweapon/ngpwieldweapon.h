#ifndef NGPWIELDWEAPON_H
#define NGPWIELDWEAPON_H
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
class nGPWieldWeapon : public nGPBasicAction
{
public:
    /// constructor
    nGPWieldWeapon();
    /// destructor
    ~nGPWieldWeapon();

        /// Initial condition
        bool Init(nEntityObject*,int,int);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPWIELDWEAPON_H
