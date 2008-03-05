#ifndef N_PHYGEOMBOX_H
#define N_PHYGEOMBOX_H

//-----------------------------------------------------------------------------
/**
    @class nPhyGeomBox
    @ingroup NebulaPhysicsSystem
    @brief A Box physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Geometric Box
    
    @cppclass nPhyGeomBox
    
    @superclass nPhysicsGeom

    @classinfo A Box physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
namespace 
{
    static const phyreal epsilonExtents(phyreal(.1e-05));
}

//-----------------------------------------------------------------------------
class nPhyGeomBox : public nPhysicsGeom 
{
public:
    static const phyreal DefaultLengthSide;

    /// constructor
    nPhyGeomBox();

    /// sets the box lengths
    void SetLengths( const vector3& lengths );

    /// returns the box lengths
    void GetLengths( vector3& lengths ) const;

    /// destructor
    ~nPhyGeomBox();

#ifndef NGAME
    /// draws a wire of the box
    void Draw( nGfxServer2 *server );
#endif

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// scales the geometry
    void Scale( const phyreal factor );

private:
    /// creates the geometry
    void Create();

};

//-----------------------------------------------------------------------------
/**
    Returns the box lengths

    @param lengths retrieved lengths of the box in each axis

    history:
        - 24-Sep-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
void nPhyGeomBox::GetLengths( vector3& lengths ) const
{
    n_assert2( this->Id() , "No valid geometry id" );

    return phyGetBoxLengths( this->Id(), lengths );
}

#endif 
