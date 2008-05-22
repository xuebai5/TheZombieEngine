#ifndef N_PHYSICSGEOM_H
#define N_PHYSICSGEOM_H
//-----------------------------------------------------------------------------
/**
    @class nPhysicsGeom
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Geometry
    
    @cppclass nPhysicsGeom
    
    @superclass nObject

    @classinfo An abstract physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nobject.h"

#include "nphysics/nphycollide.h"
#include "nphysics/nphymaterial.h"

#include "util/ntag.h"

#ifndef NGAME
    #include "gfx2/ngfxserver2.h"
#endif

//-----------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(geomid, "i", (value = (geomid)(size_t)cmd->In()->GetI()), (cmd->Out()->SetI(int(size_t(value))))  );

//-----------------------------------------------------------------------------

class nPhyRigidBody;
class ncPhysicsObj;
class nPhyMaterial;
class nPhysicsWorld;
class nPhySpace;
class nPhyGeomTrans;

//-----------------------------------------------------------------------------
class nPhysicsGeom : public nObject
{
public:
    /// geometry types
    typedef enum {
	    Sphere = 0,
	    Box,
	    RegularFlatEndedCylinder,
	    InfinitePlane,
	    GeometryTransform,
	    Ray,
	    TriangleMesh,
	    SimpleSpace,
	    HastTableBasedSpace,
        QuadTreeBasedSpace,
        HeightMap,
        Cone,
	    Num,
	    Invalid,
    } GeometryTypes;

    // categories
    typedef enum {
        Static     = 1 << 0,
        Fluid      = 1 << 1,
        Check      = 1 << 2,
        Dynamic    = 1 << 3,
        Mobil      = 1 << 4,
        Stairs     = 1 << 5,
        Ramp       = 1 << 6,
        Category08 = 1 << 7,
        Category09 = 1 << 8,
        Category10 = 1 << 9,
        Category11 = 1 << 10,
        Category12 = 1 << 11,
        Category13 = 1 << 12,
        Category14 = 1 << 13,
        Category15 = 1 << 14,
        Category16 = 1 << 15,
        Category17 = 1 << 16,
        Category18 = 1 << 17,
        Category19 = 1 << 18,
        Category20 = 1 << 19,
        Category21 = 1 << 20,
        Category22 = 1 << 21,
        Category23 = 1 << 22,
        Category24 = 1 << 23,
        Category25 = 1 << 24,
        Category26 = 1 << 25,
        Category27 = 1 << 26,
        Category28 = 1 << 27,
        Category29 = 1 << 28,
        Category30 = 1 << 29,
        Category31 = 1 << 30,
        Category32 = 1 << 31,
        All = -1
    } Category;

    typedef enum {
        wall = 1 << 0,
        ground = 1 << 1,
        ceiling = 1 << 2,
        walkable = 1 << 3
    } Attr; // up to 31

    /// constructor
    nPhysicsGeom();

    /// destructor
    virtual ~nPhysicsGeom();

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// returns the geometry id
    geomid Id() const;

    /// returns the parent space if any if not returns a no valid spaceid
    spaceid SpaceId() const;

    /// returns the type of geometry
    virtual GeometryTypes Type() const;

    /// sets the orientation of this geometry
    virtual void SetOrientation( const matrix33& neworientation );

    /// returns current orientation
    virtual void GetOrientation( matrix33& orientation ) const;

    /// returns the bounding box of this geometry
    virtual void GetAABB( nPhysicsAABB& boundingbox );

    /// sets the body of this geometry
    void SetBody( nPhyRigidBody* body );

    /// returns the body where this geometry it's attached
    nPhyRigidBody* GetBody() const;

    /// returns if it's colliding with other geometry
    int Collide( 
        const nPhysicsGeom *geom, 
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// resets categories
    void ResetCategory();

    /// adds a category where the geometry belongs
    void AddCategory( Category category );

    /// gets the categories where the geometry belongs
    virtual int GetCategories() const;

    /// adds a category wich the geom will collide with
    void AddCollidesWith( Category category );

    /// removes the categories wich the geom will collide with
    void RemovesCollidesWith( Category category );

    /// gets the categories wich the geom will collide with
    virtual int GetCollidesWith() const;

    /// creates the geometry (should be call by all the classes that inheret from this one)
    virtual void Create();

    /// moves the geometry to another space
    void MoveToSpace( nPhySpace* newspace );

    /// sets owner
    void SetOwner( ncPhysicsObj* owner );

    /// returns the owner if any
    virtual ncPhysicsObj* GetOwner() const;

    /// sets the geometry material
    void SetMaterial( nPhyMaterial::idmaterial material );

    /// returns the geometry material
    virtual const nPhyMaterial::idmaterial GetMaterial() const;

    /// returns the number of contacts
    virtual int GetNumContacts() const;

    /// returns all the attributes
    int GetAttributes() const;

    /// returns the geometry world
    virtual nPhysicsWorld* GetWorld() const;

    /// returns the absolute orientatation
    void GetAbsoluteOrientation( quaternion& orientation ) const;

    /// returns the absolute position
    void GetAbsolutePosition( vector3& position ) const;

    /// sets the transform
    void SetTransform( nPhyGeomTrans* trans );

    /// gets the transform
    nPhyGeomTrans* GetTransform() const;

    /// begin:scripting
    
    /// returns if this geometry it is a space
    bool IsSpace() const;
    /// enables this geometry
   virtual void Enable();
    /// disables this geometry
   virtual void Disable();
    /// returns it the geometry is enabled
    bool IsEnabled() const;
    /// sets the position of this geometry
   virtual void SetPosition(const vector3&);
    /// returns current position
   virtual void GetPosition(vector3&) const;
    /// sets the euler orientation
   virtual void SetOrientation(phyreal,phyreal,phyreal);
    /// returns current angles
   virtual void GetOrientation(phyreal&,phyreal&,phyreal&);

    /// sets the game material
    void SetGameMaterial(const nString&);

    /// sets the game material id
    void SetGameMaterialId(const uint);

#ifndef NGAME
    /// sets if the shape has to be draw
    void DrawShape(bool);
#endif
    /// sets the categories where the geometry belongs
   virtual void SetCategories(int);
    /// sets the categories wich the geom will collide with
   virtual void SetCollidesWith(int);
    /// sets the geometry material
    void SetMaterial(nPhyMaterial*);
    /// sets the geom attributes
    void SetAttributes(int);
    /// removes attribute(s)
    void RemoveAttributes(int);
    /// checks wich attributes has enabled
    bool HasAttributes(int) const;
    /// adds attribute(s)
    void AddAttributes(int);
    /// scales the geometry
   virtual void Scale(const phyreal);
    /// returns the id/tag
    int GetIdTag() const;
    /// sets the id/tag
    void SetIdTag(int);

//#ifndef NGAME
    /// returns the game material name
    const nString& GetGameMaterialName() const;
//#endif

    /// returns the game material id
    const uint GetGameMaterialId() const;

    /// end:scripting

#ifndef NGAME
    /// draws a wire of the geometry
    virtual void Draw( nGfxServer2* server );

    /// returns if the shape has to be draw
    bool DrawShape() const;

    /// marks this geometry
    void Mark();

    /// unmarks this geometry
    void UnMark();

#endif

protected:

    /// geometry ID
    geomid geomID;

    /// type of the geometry
    GeometryTypes type;

    /// stores the body where this geometry it's attached
    nPhyRigidBody* bodyGeom;

    /// stores the material of this geometry
    nPhyMaterial::idmaterial materialGeometry;
       
private:
    /// geometry id/tag
    int idTag;

    /// attributes mask
    int attributesMask : 31;

    /// stores if this geometry it's enabled
    bool enabled : 1;

#ifndef NGAME
    /// stores if the shape should be draw
    bool drawShape;

    /// stores if the geometry has to marked
    bool marked;
#endif

    /// owner 
    ncPhysicsObj* ownerGeometry;

    /// default value of contacts for a geometry
    static const int DefaultValueContacts = 3;

    /// if there's a transform geometry underneath
    nPhyGeomTrans* transform;

    /// material name
    nTag gameMaterial;
};

//-----------------------------------------------------------------------------
/**
    Geometry unique ID

    @return		geometry id

    history:
        - 22-Sep-2004   Zombie         created
*/
inline
geomid nPhysicsGeom::Id() const
{
    return this->geomID;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Sets if the shape has to b<e draw

	@return boolean

    history:
     - 06-Oct-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline 
void nPhysicsGeom::DrawShape( bool draw )
{
    drawShape = draw;
}
#endif

//------------------------------------------------------------------------------
/**
	Returns the number of contacts.
    
    @return number of contacts for this geometry
    
    history:
     - 02-Nov-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline
int nPhysicsGeom::GetNumContacts() const
{
    return DefaultValueContacts;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Returns if the shape has to be draw

	@return boolean

    history:
     - 06-Oct-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline
bool nPhysicsGeom::DrawShape() const
{
    return drawShape;
}
#endif
//------------------------------------------------------------------------------
/**
	Returns the body where this geometry it's attached

	@return body to be attached

	history:
     - 24-Sep-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline
nPhyRigidBody* nPhysicsGeom::GetBody() const
{
    return this->bodyGeom;
}

//------------------------------------------------------------------------------
/**
	Returns current position

	@param position     vector position

	history:
     - 22-Sep-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhysicsGeom::GetPosition( vector3& position ) const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyGetPositionGeometry( this->Id(), position );
}

//------------------------------------------------------------------------------
/**
	Returns current orientation

	@param orientation     orientation matrix

	history:
     - 22-Sep-2004   Zombie         created
     - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhysicsGeom::GetOrientation( matrix33& orientation ) const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyGetOrientationGeometry( this->Id(), orientation );
}

//-----------------------------------------------------------------------------
/**
    Returns the parent space if any if not returns a no valid spaceid

    @return     space id

    history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
spaceid nPhysicsGeom::SpaceId() const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    return phyGetGeometrySpace( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry type

    @return     geometry type

    history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
nPhysicsGeom::GeometryTypes nPhysicsGeom::Type() const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    return this->type;
}

//------------------------------------------------------------------------------
/**
	Enables this geometry

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::Enable() 
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

	phyEnableGeometry( this->Id() );

    this->enabled = true;
}

//------------------------------------------------------------------------------
/**
	Disables this geometry

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::Disable() 
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

	phyDisableGeometry( this->Id() );

    this->enabled = false;
}

//------------------------------------------------------------------------------
/**
	Returns if the geometry is enabled

    @return boolean indicating if the geometry is enabled

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
bool nPhysicsGeom::IsEnabled() const 
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    return this->enabled;
}

//------------------------------------------------------------------------------
/**
	Returns if this geometry is a space

	@return		boolean

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
bool nPhysicsGeom::IsSpace() const 
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

	return this->Type() == SimpleSpace || this->Type() == HastTableBasedSpace || this->Type() == QuadTreeBasedSpace;
}

//------------------------------------------------------------------------------
/**
	Sets the position of this geometry

	@param  newposition vector with a position in the world

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetPosition( const vector3& newposition )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

	phySetPositionGeometry( this->Id(), newposition );
}

//------------------------------------------------------------------------------
/**
	Sets the orientation of this geometry

	@param  neworientation matrix with a position in the world

	history:
     - 22-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetOrientation( const matrix33& neworientation )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phySetOrientationGeometry( this->Id(), neworientation );
}

//------------------------------------------------------------------------------
/**
	Returns the bounding box of this geometry

	@param boundingbox     bounding box to be filled

	history:
     - 24-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::GetAABB( nPhysicsAABB& boundingbox )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    phyGetAABBGeometry( this->Id(), boundingbox );

}

//------------------------------------------------------------------------------
/**
	Sets the body of this geometry

	@param body     body to be attached

	history:
     - 24-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetBody( nPhyRigidBody* body )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    n_assert2( body, "Null pointer" );

    this->bodyGeom = body;
}

//------------------------------------------------------------------------------
/**
	Returns if it's colliding with other geotry

	@param geom         geometry to check collision
    @param numContacts  how may contact points to be return
    @param contact      array of contact points
    
    @return number of contacts if any

	history:
     - 28-Sep-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
int nPhysicsGeom::Collide( 
    const nPhysicsGeom *geom, 
    int numContacts, nPhyCollide::nContact* contact ) const
{
    return nPhyCollide::Collide( this, geom, numContacts, contact );
}

//------------------------------------------------------------------------------
/**
	Sets the categories where the geometry belongs.

	@param Category new category for this geom

    history:
     - 07-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::AddCategory( Category category )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyAddCategory( this->Id(), category );
}

//------------------------------------------------------------------------------
/**
	Resets categories.

    history:
     - 15-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::ResetCategory()
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyRemoveCategory( this->Id(), ~0 );
}

//------------------------------------------------------------------------------
/**
	Sets the categories wich the geom will collide with.

	@param Category new category for this geom

    history:
     - 07-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::AddCollidesWith( Category category )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyAddCollideWith( this->Id(), category );
}

//------------------------------------------------------------------------------
/**
	Removes the categories wich the geom will collide with.

	@param Category new category for this geom

    history:
     - 15-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::RemovesCollidesWith( Category category )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    phyRemoveCollideWith( this->Id(), category );
}

//------------------------------------------------------------------------------
/**
	Creates the geometry (should be call by all the classes that inheret from this one).

    history:
     - 11-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::Create()
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    // attaching this geom to the geom id
    phyAttachDataGeom( this->Id(), this );

    // setting default categories
    this->SetCategories( nPhysicsGeom::Static );
    this->SetCollidesWith( nPhysicsGeom::Dynamic | nPhysicsGeom::Check );
}

//------------------------------------------------------------------------------
/**
	Sets owner.

    @param owner    the new owner of the geometry

    history:
     - 14-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetOwner( ncPhysicsObj* owner )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    this->ownerGeometry = owner;
}
//------------------------------------------------------------------------------
/**
	Sets the categories wich the geom will collide with.

    @param categories new set of collide categories

    history:
     - 19-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetCollidesWith( int categories )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );
    
    phySetCollideWith( this->Id(), categories );
}

//------------------------------------------------------------------------------
/**
	Sets the categories wich the geom will collide with.
    
    @param categories new set of categories
    
    history:
     - 19-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetCategories( int categories )
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );
    
    phySetCategories( this->Id(), categories );
}

//------------------------------------------------------------------------------
/**
	Sets the geometry material.
    
    @param material the new material for the 
    
    history:
     - 29-Oct-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetMaterial( nPhyMaterial::idmaterial material )
{
#ifndef NGAME
    n_assert2( nPhyMaterial::Validate( material ), "Not valid material type" );
#endif

    this->materialGeometry = material;
}

//------------------------------------------------------------------------------
/**
	Gets the categories where the geometry belongs.
    
    @return categories flags
    
    history:
     - 24-Nov-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
int nPhysicsGeom::GetCategories() const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    return phyGetCategories( this->Id() );
}

//------------------------------------------------------------------------------
/**
	Gets the categories wich the geom will collide with.
    
    @return categories flags
    
    history:
     - 24-Nov-2004   Zombie         created
     - 31-Jan-2005   Zombie         inlined
*/
inline
int nPhysicsGeom::GetCollidesWith() const
{
    n_assert2( this->Id() != NoValidID , "No valid geometric object" );

    return phyGetCollideWith( this->Id() );
}

//------------------------------------------------------------------------------
/**
	Sets the material for this geometry.

    @param material a material
    
    history:
     - 05-Apr-2005   Zombie         created
*/
inline
void nPhysicsGeom::SetMaterial( nPhyMaterial* material )
{
    n_assert2( material, "Null pointer, no valid object." );

    this->SetMaterial( material->Id() );
}

//------------------------------------------------------------------------------
/**
	Checks wich attributes has enabled.

    @return if has all the attributes

    history:
     - 02-Mar-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
bool nPhysicsGeom::HasAttributes( int attributesmask ) const
{
    return ((this->attributesMask & attributesmask) == attributesmask );
}

//------------------------------------------------------------------------------
/**
	Sets the geom attributes.

    @return if has all the attributes

    history:
     - 02-Mar-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetAttributes( int attributesmask )
{
    this->attributesMask = attributesmask;
}

//------------------------------------------------------------------------------
/**
	Adds attribute(s).

    @param attributemask attribute's flags

    history:
     - 02-Mar-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::AddAttributes( int attributesmask )
{
    this->attributesMask |= attributesmask;
}

//------------------------------------------------------------------------------
/**
	Removes attribute(s).

    @param attributemask attribute's flags

    history:
     - 02-Mar-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::RemoveAttributes( int attributesmask )
{
    this->attributesMask &= ~attributesmask;
}

//------------------------------------------------------------------------------
/**
	Returns all the attributes.

    @return attribute's flags

    history:
     - 02-Mar-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
int nPhysicsGeom::GetAttributes() const
{
    return this->attributesMask;
}

//-----------------------------------------------------------------------------
/**
    Sets the transform.

    @param trans a transfrom geometry

    history:
        - 21-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::SetTransform( nPhyGeomTrans* trans )
{
    this->transform = trans;
}

//-----------------------------------------------------------------------------
/**
    Gets the transform.

    @return a transfrom geometry

    history:
        - 21-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhyGeomTrans* nPhysicsGeom::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
void nPhysicsGeom::Scale( const phyreal /*factor*/ )
{
    // empty
}

#endif