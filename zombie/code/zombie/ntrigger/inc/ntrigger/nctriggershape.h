#ifndef NC_TRIGGERSHAPE_H
#define NC_TRIGGERSHAPE_H

//------------------------------------------------------------------------------
/**
    @class ncTriggerShape
    @ingroup NebulaTriggerSystem

    Shape that defines the activation region of an area trigger.

    The shape is stored in local coordinates. The component assumes that the
    entity has a ncTransform component to finally place the shape in world
    space when operations with the shape are requested.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "mathlib/polygon.h"

//------------------------------------------------------------------------------
class ncTriggerShape : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTriggerShape,nComponentObject);

public:
    /// Default constructor
    ncTriggerShape();
    /// Destructor
    ~ncTriggerShape();
    /// Tell if the given point in world coordinates is inside this shape
    bool IsInside( const vector3& point ) const;
    /// Tell distance to point
    float GetDistance( const vector3& point ) const;
    /// Tell distance to point divided by sound fade distance
    float GetFadedDistance( const vector3& point ) const;

    /// Set a polygon as the shape for this component, in local coordinates
    void SetPolygonVertices( const nArray<vector3>& vertices );
    /// Return the shape as a polygon, in local coordinates
    const polygon& GetPolygon() const;

    /// Set a circle as the shape for this component, in local coordinates
    void SetCircle(float);
    /// Set a polygon as the shape for this component, in local coordinates
    void SetPolygon(const vector3&, const vector3&, const vector3&);
    /// Add a vertex when the shape is a polygon
    void AddVertexToPolygon(const vector3&);
    /// Get the shape type
    const char* GetShapeType () const;
    /// Get the radius when the shape is a circle
    float GetCircle () const;
    /// Set the shape's height
    void SetHeight(float);
    /// Get the shape's height
    float GetHeight () const;
    /// Set the shape's height offset
    void SetHeightOffset(float);
    /// Get the shape's height offset
    float GetHeightOffset() const;
    /// Set the shape's fade distance
    void SetFadeDistance(float);
    /// Get the shape's fade distance
    float GetFadeDistance () const;
    /// Set the shape's vertical fade distance
    void SetVerticalFadeDistance(float);
    /// Get the shape's vertical fade distance
    float GetVerticalFadeDistance () const;
    /// Set the fade out Velocity
    void SetFadeVelocity(float);
    /// Get the fade out Velocity
    float GetFadeVelocity () const;

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

protected:
    /// Update bounding box and location in the spatial server
    void UpdateBBox();

    /// Base class for all shapes
    struct Shape
    {
        /// Shape type ids
        enum Type
        {
            CIRCLE,
            POLYGON,
        };

        /// Constructor
        Shape( Type shapeType );
        /// Destructor
        virtual ~Shape();
        /// Tell if a point is inside the shape
        virtual bool IsInside( const vector3& point ) const = 0;

        /// Tell distance from point to polygon
        virtual float GetDistance( const vector3& point ) const = 0;

        /// Shape type id
        /*const*/ Type shapeType; // Non const to avoid "assignment operator could not be generated" warning
    };

    /// Circle shape
    struct Circle : public Shape
    {
        /// Constructor
        Circle( float radius );
        /// Tell if a point is inside the circle
        bool IsInside( const vector3& point ) const;

        /// Tell distance from point to polygon
        float GetDistance( const vector3& point ) const;

        /// Circle radius
        float radius;
    };

    /// Polygon shape
    struct Polygon : public Shape
    {
        /// Constructor
        Polygon( const nArray<vector3>& pol );
        /// Tell if a point is inside the polygon
        bool IsInside( const vector3& point ) const;

        /// Tell distance from point to polygon
        float GetDistance( const vector3& point ) const;

        /// Polygon
        polygon pol;
    };

    /// The shape
    Shape* shape;
    /// Shape height (0 = infinite)
    float height;

    /// Shape height offset
    float heightOffset;

    /// fade distance
    float fadeDistance;

    /// vertical fade distance
    float verticalFadeDistance;

    // fade out duration
    float fadeVelocity;

};

//------------------------------------------------------------------------------
#endif // NC_TRIGGERSHAPE_H
