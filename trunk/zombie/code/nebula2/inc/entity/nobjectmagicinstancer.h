#ifndef N_OBJECTMAGICINSTANCER_H
#define N_OBJECTMAGICINSTANCER_H
//------------------------------------------------------------------------------
/**
    @class nObjectMagicInstancer
    @ingroup NebulaEntitySystem
    @brief Same as object instancer, but persisting entities separated by magic number
    @author Juan Jose Luna

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nobjectinstancer.h"

//------------------------------------------------------------------------------
class nObjectMagicInstancer : public nObjectInstancer
{
public:
    /// constructor
    nObjectMagicInstancer();
    /// destructor
    virtual ~nObjectMagicInstancer();

    /// Set magic number
    void SetMagicNumber(unsigned int);

    /// save object to persistent stream
    virtual bool SaveCmds( nPersistServer* ps );
};

//------------------------------------------------------------------------------
#endif N_OBJECTMAGICINSTANCER_H
