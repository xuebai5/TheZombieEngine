#ifndef NGPUSEOBJECT_H
#define NGPUSEOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nGPUseObject
    @ingroup
    
    Description of the porpouse of the class
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPUseObject : public nGPBasicAction
{
public:
    /// constructor
    nGPUseObject();
    /// destructor
    ~nGPUseObject();

        /// Initial condition
        bool Init(nEntityObject*,int,int);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPUSEOBJECT_H
