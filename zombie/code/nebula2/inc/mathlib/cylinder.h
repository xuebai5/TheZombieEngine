#ifndef N_CYLINDER_H
#define N_CYLINDER_H
//------------------------------------------------------------------------------
/**
    @class cylinder
    @ingroup NebulaMathDataTypes

    A 3-dimensional cylinder.

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class cylinder
{
public:
    
    /// default constructor
    cylinder();
    /// p1, p2, radius constructor
    cylinder(const vector3& _p1, const vector3& _p2, float _r);
    /// copy constructor
    cylinder(const cylinder& cyl);
    /// assignation operator
    void operator=(const cylinder& cyl);
    /// set p1, p2 and radius
    void set(const vector3& _p1, const vector3& _p2, float _r);

    vector3 p1;  // position 1
    vector3 p2;  // position 2
    float   r;   // radius
};

//------------------------------------------------------------------------------
/**
*/
inline
cylinder::cylinder() :
    r(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
cylinder::cylinder(const vector3& _p1, const vector3& _p2, float _r) :
    p1(_p1),
    p2(_p2),
    r(_r)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
cylinder::cylinder(const cylinder& cyl) :
    p1(cyl.p1),
    p2(cyl.p2),
    r(cyl.r)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    assignation operator
*/
inline
void 
cylinder::operator=(const cylinder& cyl)
{
    this->set(cyl.p1, cyl.p2, cyl.r);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
cylinder::set(const vector3& _p1, const vector3& _p2, float _r)
{
    this->p1 = _p1;
    this->p2 = _p2;
    this->r = _r;
}

//------------------------------------------------------------------------------
#endif
