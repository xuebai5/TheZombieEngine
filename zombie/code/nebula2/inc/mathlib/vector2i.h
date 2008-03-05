#ifndef _VECTOR2I_H
#define _VECTOR2I_H
//------------------------------------------------------------------------------
/**
    @classvector2i
    @ingroup NebulaMathDataTypes

    Generic integer vector2 class.

    (C) 2006 Conjurer Services, S.A.
*/
#include "mathlib/nmath.h"
#include <float.h>

//------------------------------------------------------------------------------
class vector2i {
public:
    /// constructor 1
    vector2i();
    /// constructor 2
    vector2i(const int _x, const int _y);
    /// constructor 3
    vector2i(const vector2i& vec);
    /// set elements 1
    void set(const int _x, const int _y);
    /// set elements 2
    void set(const vector2i& vec);
    /// in place add
    void operator+=(const vector2i& v0);
    /// in place sub
    void operator-=(const vector2i& v0);
        /// in place scalar mul
    void operator*=(const int s);
    /// in place scalar div
    void operator/=(const int s);
    int x, y;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector2i::vector2i() :
    x(0),
    y(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2i::vector2i(const int _x, const int _y) :
    x(_x),
    y(_y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2i::vector2i(const vector2i& vec) :
    x(vec.x),
    y(vec.y)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
inline
void
vector2i::set(const int _x, const int _y)
{
    this->x = _x;
    this->y = _y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2i::set(const vector2i& v)
{
    this->x = v.x;
    this->y = v.y;
}




//------------------------------------------------------------------------------
/**
*/
inline
void
vector2i::operator +=(const vector2i& v0)
{
    this->x += v0.x;
    this->y += v0.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2i::operator -=(const vector2i& v0)
{
    this->x -= v0.x;
    this->y -= v0.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2i::operator *=(const int s)
{
    this->x *= s;
    this->y *= s;
}

//------------------------------------------------------------------------------

/**
*/
inline
void
vector2i::operator /=(const int s)
{
    this->x /= s;
    this->y /= s;
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2i operator +(const vector2i& v0, const vector2i& v1)
{
    return vector2i(v0.x + v1.x, v0.y + v1.y);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2i operator -(const vector2i& v0, const vector2i& v1)
{
    return vector2i(v0.x - v1.x, v0.y - v1.y);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2i operator *(const vector2i& v0, const int s)
{
    return vector2i(v0.x * s, v0.y * s);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2i operator /(const vector2i& v0, const int s)
{
    return vector2i(v0.x / s, v0.y / s);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2i operator -(const vector2i& v)
{
    return vector2i(-v.x, -v.y);
}


//------------------------------------------------------------------------------
#endif

