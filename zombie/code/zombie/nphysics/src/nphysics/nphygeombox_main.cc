//-----------------------------------------------------------------------------
//  nphygeombox_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/ncphysicsobj.h"

//-----------------------------------------------------------------------------

const phyreal nPhyGeomBox::DefaultLengthSide(1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomBox, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 24-Sep-2004   David Reyes    created
*/
nPhyGeomBox::nPhyGeomBox()
{
    this->type = Box;

    this->Create();
}

//-----------------------------------------------------------------------------
/**
    Sets the box lengths

    @param lengths  lengths of the box in each axis

    history:
        - 24-Sep-2004   David Reyes    created
        - 27-Jan-2005   David Reyes    inlined
*/
void nPhyGeomBox::SetLengths( const vector3& lengths )
{
    n_assert2( this->Id() , "No valid geometry id" );

    n_assert3(lengths.x != 0.f && lengths.y != 0.f && lengths.z != 0.f,
        ("SetLengths with zero dimension (%f,%f,%f), geomtype=%d object=0x%x(%d) class=%s",
        lengths.x, lengths.y, lengths.z,
        this->type,
        this->GetOwner() ? this->GetOwner()->GetEntityObject()->GetId() : 0,
        this->GetOwner() ? this->GetOwner()->GetEntityObject()->GetId() : 0,
        this->GetOwner() ? 
            (this->GetOwner()->GetEntityObject() ? 
                this->GetOwner()->GetEntityObject()->GetClass()->GetName() : 
                "no class" ) :
            "no class"
    ));

#ifndef NGAME
    vector3 lengthTmp(lengths);

    if( lengthTmp.x == 0 )
    {
        lengthTmp.x = epsilonExtents;
    }
    if( lengthTmp.y == 0 )
    {
        lengthTmp.y = epsilonExtents;
    }
    if( lengthTmp.z == 0 )
    {
        lengthTmp.z = epsilonExtents;
    }
    phySetBoxLengths( this->Id(), lengthTmp );
#else
    phySetBoxLengths( this->Id(), lengths );
#endif
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 24-Sep-2004   David Reyes    created
        - 01-Oct-2004   David Reyes    supress parameters
*/
void nPhyGeomBox::Create()
{
    n_assert2( this->Id() == NoValidID , "Attempting to create an already created geometry" );

    geomID = phyCreateBox(DefaultLengthSide,DefaultLengthSide,DefaultLengthSide );

    n_assert2( this->Id() != NoValidID , "Failing to create the geometry" );

    nPhysicsGeom::Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 24-Sep-2004   David Reyes    created
*/
nPhyGeomBox::~nPhyGeomBox()
{
    // Empty
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws a wire of the box

    @param server   graphics server

    history:
        - 24-Sep-2004   David Reyes    created
*/
void nPhyGeomBox::Draw( nGfxServer2* server )
{
    nPhysicsGeom::Draw( server );
 
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyShapes) )
        return;

    if( this->DrawShape() )
    {
        vector3 position;

        this->GetAbsolutePosition( position );

        matrix33 orientation;

        quaternion orientationQuaternion;

        this->GetAbsoluteOrientation( orientationQuaternion );

        orientation = orientationQuaternion;

        matrix44 transform;

        transform.set( orientation.x_component(), orientation.y_component(), orientation.z_component(), vector3(0,0,0) ); 

        transform.M14 = 0;
        transform.M24 = 0;
        transform.M34 = 0;
        transform.M44 = 1;   

        matrix44 model;

        vector3 lengths;

        this->GetLengths( lengths );

        model.scale( vector3( lengths.x,lengths.y , lengths.z) );

        model = model * transform;

        model.translate( position );    

        server->BeginShapes();

        int col1(int(reinterpret_cast<size_t>(this)) & 0x0f);
        int col2((int(reinterpret_cast<size_t>(this)) & 0xf0) >> 4);
        server->DrawShape( nGfxServer2::Box, model, vector4(0.0f, col1/16.0f, col2/16.0f, 0.5f ) );

        server->EndShapes();
    }

}
#endif 

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhyGeomBox::Scale( const phyreal factor )
{
    vector3 lengths;

    this->GetLengths( lengths );

    lengths *= factor;

    this->SetLengths( lengths );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
