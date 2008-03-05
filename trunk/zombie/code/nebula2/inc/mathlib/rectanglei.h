#ifndef N_RECTANGLEI_H
#define N_RECTANGLEI_H
//------------------------------------------------------------------------------
/**
    @class rectanglei
    @ingroup NebulaMathDataTypes

    A 2d integer rectangle class.
    
*/
#include "mathlib/vector2i.h"

//------------------------------------------------------------------------------
class rectanglei
{
public:
    /// default constructor
    rectanglei();
    /// default destructor
    ~rectanglei() {}
    /// constructor 1
    rectanglei(const vector2i& topLeft, const vector2i& bottomRight);
    /// constructor 2
    rectanglei(const int left , const int top, const int right, const int bottom );
    /// set content
    void set(const vector2i& topLeft, const vector2i& bottomRight);
    /// return true if point is inside
    bool inside(const vector2i& p) const;
    /// return midpoint
    vector2i midpoint() const;
    /// return width
    int width() const;
    /// return height
    int height() const;
    /// return (width * height)
    int area() const;
    /// return size
    vector2i size() const;
    /// begin extending the rectangle
    void begin_extend();
    /// extend the box
    void extend(const vector2i& v);
    /// extend the box
    void extend(int x, int y);

    vector2i v0;
    vector2i v1;
};

//------------------------------------------------------------------------------
/**
*/
inline
rectanglei::rectanglei()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
rectanglei::rectanglei(const vector2i& topLeft, const vector2i& bottomRight) :
    v0(topLeft),
    v1(bottomRight)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
rectanglei::rectanglei(const int left , const int top, const int right, const int bottom ) :
    v0( left, top),
    v1( right, bottom)
{
    // empty
}




//------------------------------------------------------------------------------
/**
*/
inline
void
rectanglei::set(const vector2i& topLeft, const vector2i& bottomRight)
{
    this->v0 = topLeft;
    this->v1 = bottomRight;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
rectanglei::inside(const vector2i& p) const
{
    return ((this->v0.x <= p.x) && (p.x <= this->v1.x) &&
            (this->v0.y <= p.y) && (p.y <= this->v1.y));
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2i
rectanglei::midpoint() const
{
    return (this->v0 + this->v1) / 2;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
rectanglei::width() const
{
    return this->v1.x - this->v0.x;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
rectanglei::height() const
{
    return this->v1.y - this->v0.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
rectanglei::area() const
{
    return this->width() * this->height();
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2i
rectanglei::size() const
{
    return this->v1 - this->v0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
rectanglei::begin_extend()
{
    this->v0.set(INT_MAX, INT_MAX);
    this->v1.set(INT_MIN, INT_MIN);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
rectanglei::extend(const vector2i& v)
{
    if (v.x < v0.x) v0.x = v.x;
    if (v.x > v1.x) v1.x = v.x;
    if (v.y < v0.y) v0.y = v.y;
    if (v.y > v1.y) v1.y = v.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
rectanglei::extend(int x, int y)
{
    if (x < v0.x) v0.x = x;
    if (x > v1.x) v1.x = x;
    if (y < v0.y) v0.y = y;
    if (y > v1.y) v1.y = y;
}

/**
*/
static 
inline 
rectanglei operator *(const rectanglei& r0, const rectanglei& r1) 
{
    int v0x, v0y, v1x, v1y;

    if (r0.v0.x >= r1.v1.x || r0.v1.x <= r1.v0.x ||
        r0.v0.y >= r1.v1.y || r0.v1.y <= r1.v0.y ||
        r0.area() == 0.0f || r1.area() == 0.0f)
    {
        // zero-size rectangle
        v0x = v1x = r0.v0.x;
        v0y = v1y = r0.v0.y;
    }
    else
    {
        v0x = n_max(r0.v0.x, r1.v0.x);
        v0y = n_max(r0.v0.y, r1.v0.y);
        v1x = n_min(r0.v1.x, r1.v1.x);
        v1y = n_min(r0.v1.y, r1.v1.y);
    }

    return rectanglei(vector2i(v0x, v0y), vector2i(v1x, v1y));
}
//------------------------------------------------------------------------------
/**
*/
static
inline
rectanglei operator +(const rectanglei& r0, const vector2i& v)
{
    return rectanglei(r0.v0 + v, r0.v1 + v);
}


//------------------------------------------------------------------------------
#endif
