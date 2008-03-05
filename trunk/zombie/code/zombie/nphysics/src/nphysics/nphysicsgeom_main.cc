//-----------------------------------------------------------------------------
//  nphysicsgeom_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "gameplay/ngamematerial.h"
#include "gameplay/ngamematerialserver.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhysicsGeom, "nobject");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 22-Sep-2004   David Reyes    created
        - 23-Sep-2004   David Reyes    added name support
*/
nPhysicsGeom::nPhysicsGeom() : 
    geomID( NoValidID ),
    type( Invalid ),
    enabled( true ),
#ifndef NGAME
    drawShape( true ),
    marked( false ),
#endif
    ownerGeometry(0),
    materialGeometry(nPhyMaterial::None),
    bodyGeom(0),
    attributesMask(0),
    transform(0),
    gameMaterial( "" ),
    idTag(NoValidID)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 22-Sep-2004   David Reyes    created
*/
nPhysicsGeom::~nPhysicsGeom() 
{
    if( this->Id() == NoValidID )
        return; // No valid object.

    if( this->SpaceId() != NoValidID )
    {
        this->MoveToSpace(0);
    }

    // Destroys the geometric object not matter what type it is.
    phyDestroyGeometry( this->Id() );

    this->geomID = NoValidID;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws a wire of the bonding box of this geometry

	@param server     graphic server

	history:
     - 24-Sep-2004   David Reyes    created
*/
void nPhysicsGeom::Draw( nGfxServer2* server ) 
{

    if( this->GetTransform() )
    {
        return;
    }

    vector4 color;

    if( this->marked )
    {
        color = phy_color_boundingbox_selected;
    }
    else
    {
        if( this->IsSpace() )
        {
            if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phySpaces) )
                return;
        }
        else
        {
            if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyAABB) )
                return;
        }


        if( this->IsSpace() )
        {
            if( phyIsEnabledGeometry(this->Id()) )
            {
                if( this->GetCategories() & nPhysicsGeom::Dynamic )
                {
                    color = phy_color_dynamic_space;
                }
                else
                {
                    color = phy_color_space;
                }
            }
            else
            {
                if( this->GetCategories() & nPhysicsGeom::Dynamic )
                {
                    color = phy_color_dynamic_space;
                }
                else
                {
                    color = phy_color_dynamic_space_disabled;
                }
            }

            if( nPhysicsServer::Instance()->GetDefaultWorld()->GetSpace() == this )
            {
                color = phy_color_world_space;
            }
        }
        else
        {
            if( phyIsEnabledGeometry(this->Id()) )
            {
                color = phy_color_boundingbox;
            }
            else
            {
                color = phy_color_boundingbox_disabled;
            }
        }
    }
    
    nPhysicsAABB AABB;

    this->GetAABB( AABB );

    vector3 vertexes[10];

    //  3---2
    //  |   |
    //  0---1

    if( !(
        AABB.maxx + AABB.minx+
        AABB.maxy + AABB.miny+
        AABB.maxz + AABB.minz
        ) )
        return;

    // upper top
    vertexes[0].x = AABB.minx;
    vertexes[0].y = AABB.maxy;
    vertexes[0].z = AABB.minz;

    vertexes[1].x = AABB.maxx;
    vertexes[1].y = AABB.maxy;
    vertexes[1].z = AABB.minz;

    vertexes[2].x = AABB.maxx;
    vertexes[2].y = AABB.maxy;
    vertexes[2].z = AABB.maxz;

    vertexes[3].x = AABB.minx;
    vertexes[3].y = AABB.maxy;
    vertexes[3].z = AABB.maxz;

    /// To Close
    vertexes[4].x = vertexes[0].x;
    vertexes[4].y = vertexes[0].y;
    vertexes[4].z = vertexes[0].z;

    //  3---2
    //  |   |
    //  0---1

    // botton top
    vertexes[5].x = AABB.minx;
    vertexes[5].y = AABB.miny;
    vertexes[5].z = AABB.minz;

    vertexes[6].x = AABB.maxx;
    vertexes[6].y = AABB.miny;
    vertexes[6].z = AABB.minz;

    vertexes[7].x = AABB.maxx;
    vertexes[7].y = AABB.miny;
    vertexes[7].z = AABB.maxz;

    vertexes[8].x = AABB.minx;
    vertexes[8].y = AABB.miny;
    vertexes[8].z = AABB.maxz;

    /// To Close
    vertexes[9].x = vertexes[5].x;
    vertexes[9].y = vertexes[5].y;
    vertexes[9].z = vertexes[5].z;

    server->BeginLines();

    server->DrawLines3d( vertexes, 10, color );

    /// Vertical lines
    vertexes[0].x = AABB.maxx;
    vertexes[0].y = AABB.maxy;
    vertexes[0].z = AABB.minz;

    vertexes[1].x = AABB.maxx;
    vertexes[1].y = AABB.miny;
    vertexes[1].z = AABB.minz;

    server->DrawLines3d( vertexes, 2, color );

    vertexes[0].x = AABB.maxx;
    vertexes[0].y = AABB.maxy;
    vertexes[0].z = AABB.maxz;

    vertexes[1].x = AABB.maxx;
    vertexes[1].y = AABB.miny;
    vertexes[1].z = AABB.maxz;

    server->DrawLines3d( vertexes, 2, color );

    vertexes[0].x = AABB.minx;
    vertexes[0].y = AABB.maxy;
    vertexes[0].z = AABB.maxz;

    vertexes[1].x = AABB.minx;
    vertexes[1].y = AABB.miny;
    vertexes[1].z = AABB.maxz;

    server->DrawLines3d( vertexes, 2, color );

    server->EndLines();
}
#endif
//------------------------------------------------------------------------------
/**
	Sets the orientation of angle radians along the axis

	@param ax       influence of the angle in the x axis
	@param ay       influence of the angle in the y axis
	@param az       influence of the angle in the x axis

	history:
     - 01-Oct-2004   David Reyes    created
*/
void nPhysicsGeom::SetOrientation( phyreal ax, phyreal ay, phyreal az )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    matrix33 neworientation;

    neworientation.from_euler( vector3( ax, ay, az ) );

    phySetOrientationGeometry( this->Id(), neworientation );
}


//------------------------------------------------------------------------------
/**
	Gets the orientation of angle radians along the axis

	@param ax       angle for the x axis
	@param ay       angle for the y axis
	@param az       angle for the z axis

	history:
     - 01-Oct-2004   David Reyes    created
*/
void nPhysicsGeom::GetOrientation( phyreal& ax, phyreal& ay, phyreal& az )
{
    matrix33 mat;

    this->GetOrientation( mat );

    vector3 euler(mat.to_euler());

    ax = euler.x;
    ay = euler.y;
    az = euler.z;
}

//------------------------------------------------------------------------------
/**
	Moves the geometry to another space.

    @param newspace a space

    history:
     - 11-Oct-2004   David Reyes    created
*/
void nPhysicsGeom::MoveToSpace( nPhySpace* newspace )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );
    
    // Removing from the oldspace if any
    spaceid spaceID(this->SpaceId());

    if( spaceID != NoValidID )
    {
        nPhySpace* oldSpace( static_cast<nPhySpace*>(phyRetrieveDataGeom( reinterpret_cast<geomid>(spaceID))));
        // removing from the old space
        oldSpace->Remove(this);
        
    }

    if( newspace == NoValidID)
    {
        return;
    }

    newspace->Add( this );
}

//------------------------------------------------------------------------------
/**
	Returns the owner if any.

    @return physics object

    history:
     - 14-Oct-2004   David Reyes    created
*/
ncPhysicsObj* nPhysicsGeom::GetOwner() const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );
    
    if( !this->ownerGeometry )
    {
        // Checks if it belongs to an space
        spaceid sid(this->SpaceId());

        if( sid == NoValidID )
            return 0; // no owner

        nPhysicsGeom* geomPtr(static_cast<nPhysicsGeom*>(
            phyRetrieveDataGeom(reinterpret_cast<geomid>(sid))));

        n_assert2( geomPtr , "Data corruption" );

        return geomPtr->GetOwner(); // gets space parent owner
    }

    return this->ownerGeometry;
}

//-----------------------------------------------------------------------------
/**
    Returns the absolute orientation.

    @param orientation geom orientation in the form of quaternion

    history:
        - 21-Mar-2005   David Reyes    created
*/
void nPhysicsGeom::GetAbsoluteOrientation( quaternion& orientation ) const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object." );
    n_assert2( this->IsSpace() == false, "Can't be applyied to spaces." );

    nPhyGeomTrans* trans(this->GetTransform());
    
    matrix33 orientationMatrix;

    this->GetOrientation( orientationMatrix );

    /// check if it has trans underneath
    while( trans )
    {
        matrix33 orientationTrans;

        trans->GetOrientation( orientationTrans );

        orientationMatrix = orientationMatrix * orientationTrans;

        trans = trans->GetTransform();
    }

    orientation = orientationMatrix.get_quaternion();
}

//-----------------------------------------------------------------------------
/**
    Returns the absolute position.

    @param position geom's world position

    history:
        - 21-Mar-2005   David Reyes    created
*/
void nPhysicsGeom::GetAbsolutePosition( vector3& position ) const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object." );
    n_assert2( this->IsSpace() == false, "Can't be applyied to spaces." );

    nPhyGeomTrans* trans(this->GetTransform());
    /// check if if has trans underneath

    vector3 pos;

    this->GetPosition( position );

    while( trans )
    {
        vector3 pos;
        
        trans->GetPosition( pos );

        matrix33 orientation;

        trans->GetOrientation( orientation );

        position = orientation * position;

        position += pos;

        trans = trans->GetTransform();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry world.

    @return geometry's world

    history:
        - 21-Mar-2005   David Reyes    created
*/
nPhysicsWorld* nPhysicsGeom::GetWorld() const
{
    if( !this->GetOwner() )
        return 0;
    return this->GetOwner()->GetWorld();
}


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Marks this geometry.

    history:
        - 10-Aug-2005   David Reyes    created
*/
void nPhysicsGeom::Mark()
{
    this->marked = true;
}
#endif


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Unmarks this geometry.

    history:
        - 10-Aug-2005   David Reyes    created
*/
void nPhysicsGeom::UnMark()
{
    this->marked = false;
}
#endif

//-----------------------------------------------------------------------------
/**
    Sets the game material.

    @param name material name

    history:
        - 01-Sep-2005   David Reyes    created
*/
void nPhysicsGeom::SetGameMaterial( const nString& name )
{
    this->gameMaterial = name;

    nGameMaterial* gMaterial( nGameMaterialServer::Instance()->GetMaterial( this->gameMaterial.KeyMap() ) );

    if( gMaterial )
    {
        nPhyMaterial const* phyMaterial( gMaterial->GetPhysicsMaterial() );

        this->SetMaterial( phyMaterial->Id() );
    }
}


//-----------------------------------------------------------------------------
/**
    Sets the game material id.

    @param id materials id

    history:
        - 01-Sep-2005   David Reyes    created
*/
void nPhysicsGeom::SetGameMaterialId( const uint id )
{
    this->gameMaterial = id;
}

//-----------------------------------------------------------------------------
/**
    Returns the game material name.

    @return material name

    history:
        - 01-Sep-2005   David Reyes    created
*/
//#ifndef NGAME
const nString& nPhysicsGeom::GetGameMaterialName() const
{
    return this->gameMaterial.GetText();
}
//#endif

//-----------------------------------------------------------------------------
/**
    Returns the game material id.

    @return material id

    history:
        - 01-Sep-2005   David Reyes    created
*/
const uint nPhysicsGeom::GetGameMaterialId() const
{
    return this->gameMaterial.KeyMap();
}

//-----------------------------------------------------------------------------
/**
    Returns the id/tag.

    @return id

    history:
        - 19-Sep-2005   David Reyes    created
*/
int nPhysicsGeom::GetIdTag() const
{
    return this->idTag;
}

//-----------------------------------------------------------------------------
/**
    Sets the id/tag.

    @param newIdTag new id tag (can be NoValidID).

    history:
        - 19-Sep-2005   David Reyes    created
*/
void nPhysicsGeom::SetIdTag( int newIdTag )
{
    this->idTag = newIdTag;
}


//------------------------------------------------------------------------------
/**
	Returns the geometry material.
    
    @return material the geoemtry material
    
    history:
     - 29-Oct-2004   David Reyes    created
     - 12-Dec-2004   David Reyes    inlined
     - 28-Oct-2005   David Reyes    removed inline
*/
const nPhyMaterial::idmaterial nPhysicsGeom::GetMaterial() const
{
#ifndef NGAME
    nGameMaterial* gmaterial(nGameMaterialServer::Instance()->GetMaterial(this->GetGameMaterialId()));
    if( !gmaterial )
    {
        return this->materialGeometry;
    }

    nPhyMaterial* pmaterial(gmaterial->GetPhysicsMaterial());
    if( !pmaterial )
    {
        return this->materialGeometry;
    }

    return pmaterial->Id();
#else
    return this->materialGeometry;
#endif
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
