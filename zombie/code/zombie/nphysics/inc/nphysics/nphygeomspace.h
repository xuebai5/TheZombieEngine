#ifndef N_PHYGEOMSPACE_H
#define N_PHYGEOMSPACE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomSpace
    @ingroup NebulaPhysicsSystem
    @brief A space physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Space
    
    @cppclass nPhyGeomSpace
    
    @superclass nPhySpace

    @classinfo A space physics geometry
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphyspace.h"

//-----------------------------------------------------------------------------
class nPhyGeomSpace : public nPhySpace 
{
public:
    /// constructor
    nPhyGeomSpace();
            
    /// destructor
    ~nPhyGeomSpace();

private:
    /// creates the geometry
    void Create();
};

#endif 
