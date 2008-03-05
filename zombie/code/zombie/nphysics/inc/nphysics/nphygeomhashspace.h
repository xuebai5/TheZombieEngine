#ifndef N_PHYGEOMHASHSPACE_H
#define N_PHYGEOMHASHSPACE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomHashSpace
    @ingroup NebulaPhysicsSystem
    @brief A hash space physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Hash Space
    
    @cppclass nPhyGeomHashSpace
    
    @superclass nPhySpace

    @classinfo A hash space physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphyspace.h"

//-----------------------------------------------------------------------------
class nPhyGeomHashSpace : public nPhySpace 
{
public:
    /// constructor
    nPhyGeomHashSpace();
            
    /// destructor
    ~nPhyGeomHashSpace();

    /// sets the levels of the hash
    void SetLevels( int max, int min );

    /// gets the hash levels
    void GetLevels( int& max, int& min );

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

private:
    /// max default value for the levels
    static const int MaxLevelDefaultValue;

    /// min default value for the levels
    static const int MinLevelDefaultValue;

    /// creates the geometry
    void Create();

};

//-----------------------------------------------------------------------------
/**
    Sets the levels of the hash.

    @param max      largest cell size
    @param min      smallest cell size

    history:
        - 08-Oct-2004   David Reyes    created
        - 12-May-2006   David Reyes    inlined
*/
inline
void nPhyGeomHashSpace::SetLevels( int max, int min )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    phySetLevelsHashSpace( this->Id(), max, min );
}

//-----------------------------------------------------------------------------
/**
    Gets the levels of the hash.

    @param max      largest cell size
    @param min      smallest cell size

    history:
        - 28-Jan-2005   David Reyes    created
        - 12-May-2006   David Reyes    inlined
*/ 
inline
void nPhyGeomHashSpace::GetLevels( int& max, int& min )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    phySetLevelsHashSpace( this->Id(), max, min );
}

#endif 
