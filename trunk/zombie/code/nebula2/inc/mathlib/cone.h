#ifndef N_CONE_H
#define N_CONE_H
//-------------------------------------------------------------------
/**
    @class cone
    @ingroup NebulaMathDataTypes

    A 3-dimensional cone.

    (C) 2005 Conjurer Services, S.A.
*/

//-------------------------------------------------------------------
class cone 
{

public:

    //--- constructors ----------------------------------------------
    cone() : 
        r(0.0f),
        h(0.0f)
    {}

    cone(const float height, const float radius, const matrix44 &transform) :
        h(height),
        r(radius)
    {
        this->setTransform(transform);
    }

    //--- set height, radius and transformation matrix ---
    void set(const float height, const float radius, const matrix44 &transform)
    {
        this->h = height;
        this->r = radius;
        this->setTransform(transform);
    }

    //--- set radius---
    void setRadius(const float newRadius)
    {
        this->r = newRadius;
    }

    //--- get the radius ---
    float getRadius() const 
    {
        return this->r;
    }

    //--- set the height ---
    void setHeight(const float height)
    {
        this->h = height;
    }

    //--- get the height ---
    float getHeight() const
    {
        return this->h;
    }

    //--- set the transformation matrix---
    void setTransform(const matrix44 &transform)
    {
        this->m = transform;
        vector3 direction(0, 0 ,1);
        this->dir = this->m.get_quaternion().rotate(direction);
    }

    //--- get the transformation matrix---
    matrix44 &getTransform()
    {
        return this->m;
    }

    //--- determine if a point is inside the cone or not, without checking the base, like if it was an infinite cone---
    bool containsInAngle(const vector3 &point)
    {
        float coneAngle = atan2( this->r, this->h );

        vector3 pointVector(point - this->m.pos_component());
        float pointAngle = this->dir.dot(pointVector);

        pointAngle /= (this->dir.len() * pointVector.len());

        pointAngle = acos( pointAngle );

        if( pointAngle > coneAngle )
        {
            return false;
        }

        return true;
    }

    //--- determine if a point is inside the cone or not, checking the base---
    bool containsExactly(const vector3 &/*point*/)
    {
        // TODO
    }

private:

    float h;      // height
    float r;      // radius
    matrix44 m;   // trasformation matrix
    vector3 dir;  // direction vector starting at the cone's top to the base center
};

//-------------------------------------------------------------------
#endif
