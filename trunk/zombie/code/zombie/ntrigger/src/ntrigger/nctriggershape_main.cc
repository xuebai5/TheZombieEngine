//-----------------------------------------------------------------------------
//  nctriggershape_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nctriggershape.h"
#include "ntrigger/ncareatrigger.h"
#ifndef NGAME
#include "nphysics/ncphypickableobj.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncTriggerShape,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTriggerShape)
    NSCRIPT_ADDCMD_COMPOBJECT('ESCI', void, SetCircle, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESPO', void, SetPolygon, 3, (const vector3&, const vector3&, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EAVP', void, AddVertexToPolygon, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGST', const char*, GetShapeType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGCR', float, GetCircle , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESHE', void, SetHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGHE', float, GetHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSHO', void, SetHeightOffset, 1, (float), 0, () );
    NSCRIPT_ADDCMD_COMPOBJECT('JGHO', float, GetHeightOffset, 0, (), 0, () );
    NSCRIPT_ADDCMD_COMPOBJECT('JSFD', void, SetFadeDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGFD', float, GetFadeDistance , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSVF', void, SetVerticalFadeDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGVF', float, GetVerticalFadeDistance , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSFO', void, SetFadeVelocity, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGFO', float, GetFadeVelocity , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncTriggerShape::ncTriggerShape()
    : shape(NULL),
    height(0),
    heightOffset(0),
    fadeDistance(0.0f),
    verticalFadeDistance(0.0f),
    fadeVelocity(0.01f)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncTriggerShape::~ncTriggerShape()
{
    if ( this->shape )
    {
        n_delete( this->shape );
    }
}

//-----------------------------------------------------------------------------
/**
    Tell if the given point in world coordinates is inside this shape
*/
bool
ncTriggerShape::IsInside( const vector3& point ) const
{
    // Transform point from world to local coordinates
    const ncTransform* transform = this->GetComponent<ncTransform>();
    n_assert2( transform, "It's needed a transform component to locate the shape in world coordinates" );
    if ( !transform )
    {
        return false;
    }
    vector3 localPoint = point - transform->GetPosition() - vector3(0.0f, this->GetHeightOffset(), 0.0f);

    // Check if the point is inside the shape
    n_assert( this->shape );
    if ( this->shape )
    {
        // Cull first by height (0 height = infinite height)
        if ( height <= -1e-4f || height >= 1e-4f )
        {
            if ( localPoint.y < 0 || localPoint.y > this->GetHeight() )
            {
                return false;
            }
        }
        // Then check if the 2D point is inside the 2D shape
        return shape->IsInside( localPoint );
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Get distance form point to shape
*/

float
ncTriggerShape::GetDistance( const vector3& point ) const
{
    const ncTransform* transform = this->GetComponentSafe<ncTransform>();
    vector3 relPos = point - transform->GetPosition() - vector3(0.0f, this->GetHeightOffset(), 0.0f);

    float dist = this->shape->GetDistance( relPos );

    if ( this->GetHeight() != 0.0f )
    {
        // get prism taps distance
        if ( relPos.y < 0.0f || relPos.y > this->GetHeight() )
        {
            // point outside shape
            return 0.0f;
        }

        float vertDist = min ( relPos.y, this->GetHeight() - relPos.y );
        return min( dist, vertDist);
    }

    return dist;
}

//-----------------------------------------------------------------------------
/**
    Get distance form point to shape divided by sound fade distance
*/

float
ncTriggerShape::GetFadedDistance( const vector3& point ) const
{
    const ncTransform* transform = this->GetComponentSafe<ncTransform>();
    vector3 relPos = point - transform->GetPosition() - vector3(0.0f, this->GetHeightOffset(), 0.0f);

    float dist = 1.0f;
    if ( this->GetFadeDistance() > 0.0f )
    {
        dist = this->shape->GetDistance( relPos ) / this->GetFadeDistance();
    }

    if ( this->GetHeight() != 0.0f )
    {
        // get prism taps distance
        if ( relPos.y < 0.0f || relPos.y > this->GetHeight() )
        {
            // point outside shape
            return 0.0f;
        }

        if ( this->verticalFadeDistance > 0.0f )
        {
            float vertDist = min ( relPos.y, this->GetHeight() - relPos.y );
            return min( dist, vertDist / this->verticalFadeDistance );
        }
    }

    return dist;
}

//-----------------------------------------------------------------------------
/**
    Set a circle as the shape for this component, in local coordinates
*/
void
ncTriggerShape::SetCircle( float radius )
{
    if ( this->shape )
    {
        n_delete( this->shape );
    }

    this->shape = n_new( Circle )( radius );

    // Adjust the culling radius to the trigger shape radius + some tolerance
    ncAreaTrigger* areaTrigger( this->GetComponent<ncAreaTrigger>() );
    if ( areaTrigger )
    {
        const float CullingTolerance( 10 );
        areaTrigger->SetCullingRadius( radius + CullingTolerance );
    }

    this->UpdateBBox();
}

//-----------------------------------------------------------------------------
/**
    Set a polygon as the shape for this component, in local coordinates
*/
void
ncTriggerShape::SetPolygonVertices( const nArray<vector3>& vertices )
{
    if ( this->shape )
    {
        n_delete( this->shape );
    }

    this->shape = n_new( Polygon )( vertices );

    polygon *pol = &((Polygon*)this->shape)->pol;
    n_assert( pol );

    // Get distance from sound source to most far point
    float maxRadius = 0.0f;
    for ( int i = 0; i < pol->GetNumVertices(); i++)
    {
        if ( maxRadius < pol->GetVertex(i).len() )
        {
            maxRadius = pol->GetVertex(i).len();
        }
    }

    // Adjust the culling radius to the trigger shape radius + some tolerance
    ncAreaTrigger* areaTrigger( this->GetComponent<ncAreaTrigger>() );
    if ( areaTrigger )
    {
        const float CullingTolerance( 10 );
        areaTrigger->SetCullingRadius( maxRadius + CullingTolerance );
    }

    this->UpdateBBox();
}

//-----------------------------------------------------------------------------
/**
    Return the shape as a polygon, in local coordinates

    The minimum allowed polygon is a triangle.
    Then use the AddVertexToPolygon method to add more vertices (if needed of course).
*/
void
ncTriggerShape::SetPolygon( const vector3& vertex1, const vector3& vertex2, const vector3& vertex3 )
{
    nArray<vector3> vertices;
    vertices.Append( vertex1 );
    vertices.Append( vertex2 );
    vertices.Append( vertex3 );
    this->SetPolygonVertices( vertices );
}

//-----------------------------------------------------------------------------
/**
    Add a vertex when the shape is a polygon
*/
void
ncTriggerShape::AddVertexToPolygon( const vector3& vertex )
{
    // Copy current polygon
    const polygon& pol = this->GetPolygon();
    nArray<vector3> vertices;
    for ( int i(0); i < pol.GetNumVertices(); ++i )
    {
        vertices.Append( pol.GetVertex(i) );
    }

    // Add new vertex
    vertices.Append( vertex );

    // Replace polygon
    this->SetPolygonVertices( vertices );

    // Update bounding box
    this->UpdateBBox();
}

//-----------------------------------------------------------------------------
/**
    Get the shape type
*/
const char*
ncTriggerShape::GetShapeType() const
{
    static const char* circle = "circle";
    static const char* polygon = "polygon";
    static const char* undefined = "undefined";
    if ( !this->shape )
    {
        return undefined;
    }
    switch ( this->shape->shapeType )
    {
        case Shape::CIRCLE:
            return circle;
        case Shape::POLYGON:
            return polygon;
    }
    return undefined;
}

//-----------------------------------------------------------------------------
/**
    Get the radius when the shape is a circle
*/
float
ncTriggerShape::GetCircle() const
{
    n_assert( this->shape );
    if ( this->shape->shapeType != Shape::CIRCLE )
    {
        return 0.0f;
    }
    return static_cast<Circle*>( this->shape )->radius;
}

//-----------------------------------------------------------------------------
/**
    Return the shape as a polygon, in local coordinates
*/
const polygon&
ncTriggerShape::GetPolygon() const
{
    n_assert( this->shape );
    n_assert( this->shape->shapeType == Shape::POLYGON );
    return static_cast<Polygon*>( this->shape )->pol;
}

//-----------------------------------------------------------------------------
/**
    Set the shape's height
*/
void
ncTriggerShape::SetHeight( float height )
{
    this->height = height;

    this->UpdateBBox();
}

//-----------------------------------------------------------------------------
/**
    Get the shape's height
*/
float
ncTriggerShape::GetHeight() const
{
    return this->height;
}

//-----------------------------------------------------------------------------
/**
    Set the shape's height
*/
void
ncTriggerShape::SetHeightOffset( float heightOffset )
{
    this->heightOffset = heightOffset;

    this->UpdateBBox();
}

//-----------------------------------------------------------------------------
/**
    Get the shape's height
*/
float
ncTriggerShape::GetHeightOffset() const
{
    return this->heightOffset;
}

//-----------------------------------------------------------------------------
/**
    Set the shape's fade distance
*/
void
ncTriggerShape::SetFadeDistance( float fadeDist )
{
    this->fadeDistance = fadeDist;
}

//-----------------------------------------------------------------------------
/**
    Get the shape's fade distance
*/
float
ncTriggerShape::GetFadeDistance() const
{
    return this->fadeDistance;
}

//-----------------------------------------------------------------------------
/**
    Set the shape's vertical fade distance
*/
void
ncTriggerShape::SetVerticalFadeDistance( float verticalFadeDist )
{
    this->verticalFadeDistance = verticalFadeDist;
}

//-----------------------------------------------------------------------------
/**
    Get the shape's vertical fade distance
*/
float
ncTriggerShape::GetVerticalFadeDistance() const
{
    return this->verticalFadeDistance;
}

//-----------------------------------------------------------------------------
/**
    Set the shape's fade velocity
*/
void
ncTriggerShape::SetFadeVelocity( float fadeDist )
{
    this->fadeVelocity = fadeDist;
}

//-----------------------------------------------------------------------------
/**
    Get the shape's fade velocity
*/
float
ncTriggerShape::GetFadeVelocity() const
{
    return this->fadeVelocity;
}

//-----------------------------------------------------------------------------
/**
    Update bounding box and location in the spatial server
*/
void
ncTriggerShape::UpdateBBox()
{
    if ( this->shape )
    {
        // Update spatial bounding box
        ncSpatial* spatial = this->GetComponentSafe<ncSpatial>();
        switch ( this->shape->shapeType )
        {
            case Shape::CIRCLE:
                {
                    float radius = static_cast<Circle*>(this->shape)->radius;

                    vector3 v0( - radius, min( 0.0f, this->GetHeightOffset() ), - radius );
                    vector3 v1( radius, max( 0.0f, this->GetHeight() + this->GetHeightOffset() ), radius );

                    spatial->SetOriginalBBox( bbox3( ( v0 + v1 ) * 0.5f, ( v1 - v0 ) * 0.5f ) );
                }
                break;
            case Shape::POLYGON:
                {
                    const polygon& pol = static_cast<Polygon*>(this->shape)->pol;
                    bbox3 box;
                    box.begin_extend();
                    for ( int i(0); i < pol.GetNumVertices(); ++i )
                    {
                        box.extend( pol.GetVertex(i) );
                    }
                    n_assert( pol.GetNumVertices() > 0 );
                    box.extend( vector3(0, min( 0.0f, this->GetHeightOffset() ), 0) );
                    box.extend( vector3(0, max( 0.0f, this->GetHeightOffset() + this->GetHeight() ), 0) );
                    spatial->SetOriginalBBox( box );
                }
                break;
        }

        // Update location in spatial server
        ncTransform* transform = this->GetComponentSafe<ncTransform>();
        transform->SetPosition( transform->GetPosition() );
    }
}

//-----------------------------------------------------------------------------
/**
    Shape constructor
*/
ncTriggerShape::Shape::Shape( Type shapeType )
    : shapeType( shapeType )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Shape destructor
*/
ncTriggerShape::Shape::~Shape()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Circle constructor
*/
ncTriggerShape::Circle::Circle( float radius )
    : Shape(Shape::CIRCLE), radius(radius)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Tell if a point is inside the circle
*/
bool
ncTriggerShape::Circle::IsInside( const vector3& point ) const
{
    return vector3(point.x,0,point.z).lensquared() <= this->radius*this->radius;
}

//-----------------------------------------------------------------------------
/**
    Tell distance from point to circle.
*/
float
ncTriggerShape::Circle::GetDistance( const vector3& point ) const
{
    return max( 0.0f, this->radius - n_sqrt( point.x * point.x + point.z * point.z ) );
}

//-----------------------------------------------------------------------------
/**
    Polygon constructor
*/
ncTriggerShape::Polygon::Polygon( const nArray<vector3>& vertices )
    : Shape(Shape::POLYGON), pol(vertices)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Tell if a point is inside the polygon
*/
bool
ncTriggerShape::Polygon::IsInside( const vector3& point ) const
{
    return this->pol.IsPointInside( point.x, point.z );
}

//-----------------------------------------------------------------------------
/**
    Tell distance from point to polygon.
*/
float
ncTriggerShape::Polygon::GetDistance( const vector3& point ) const
{
    if ( this->pol.GetNumVertices() == 0 )
    {
        return 0.0f;
    }
    vector3 point2d(point);
    point2d.y = pol.GetVertex(0).y;
    return this->pol.GetDistance2d( point2d );
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTriggerShape::SaveCmds( nPersistServer* ps )
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        if ( this->shape )
        {
            switch ( this->shape->shapeType )
            {
                case Shape::CIRCLE:
                    ps->Put( this->entityObject, 'ESCI', this->GetCircle() );
                    break;
                case Shape::POLYGON:
                    {
                        const polygon& pol = this->GetPolygon();
                        if ( pol.GetNumVertices() >= 3 )
                        {
                            const vector3 v[3] = { pol.GetVertex(0), pol.GetVertex(1), pol.GetVertex(2) };
                            ps->Put( this->entityObject, 'ESPO',
                                v[0].x, v[0].y, v[0].z,
                                v[1].x, v[1].y, v[1].z,
                                v[2].x, v[2].y, v[2].z
                                );
                            for ( int i(3); i < pol.GetNumVertices(); ++i )
                            {
                                const vector3 v = pol.GetVertex(i);
                                ps->Put( this->entityObject, 'EAVP', v.x, v.y, v.z );
                            }
                        }
                    }
                    break;
            }
            ps->Put( this->entityObject, 'ESHE', this->GetHeight() );
            ps->Put( this->entityObject, 'JSHO', this->GetHeightOffset() );
            if ( this->GetFadeDistance() != 0.0f )
            {
                ps->Put( this->entityObject, 'JSFD', this->GetFadeDistance() );
            }
            if ( this->GetVerticalFadeDistance() != 0.0f )
            {
                ps->Put( this->entityObject, 'JSVF', this->GetVerticalFadeDistance() );
            }
            ps->Put( this->entityObject, 'JSFO', this->GetFadeVelocity() );
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
