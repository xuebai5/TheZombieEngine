#ifndef N_HORIZONCLIPPER_H
#define N_HORIZONCLIPPER_H

//------------------------------------------------------------------------------
/**
    @class nHorizonClipper
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>
    
    @brief Represents a visibility horizon.

    (C) 2006  Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

class nGfxServer2;

class nHorizonClipper
{

public:
    
    enum TestResult
    {
        ABOVE,      /// above the horizon at some point
        BELOW       /// below the horizon at all points
    };

    /// Constructor 1
    nHorizonClipper();
    /// Constructor 2
    nHorizonClipper(int w, int h);
    /// Copy constructor
    nHorizonClipper(const nHorizonClipper &horiz);
    /// Destructor
    ~nHorizonClipper();

    /// Initialize horizon
    void Initialize(int width, int height);

    /// Copy operator
    void operator=(const nHorizonClipper &horiz);

    /// Copy parameters from another horizon
    void Copy(const nHorizonClipper &horiz);

    /// Clear the horizon to a height of zero
    void Clear();

    /// Get horizon's width
    int GetWidth() const;

    /// Get horizon's height
    int GetHeight() const;

    /// Get horizon
    const float *GetHorizon() const;

    /// Set view-projection matrix
    void SetViewProjection(const matrix44 &m);

    /// Set near plane
    void SetNearPlane(const plane &p);

    /// Get near plane
    const plane &GetNearPlane() const;

    /// Get view-projection matrix
    const matrix44 &GetViewProjection() const;

    /// Test if a line is above or below the horizon
    TestResult Test(float x1, float y1, float x2, float y2) const;

    /// Test if a line is completely above the horizon
    TestResult TestCompletelyAbove(float x1, float y1, float x2, float y2) const;

    /// Test a line in world coordinates
    TestResult Test(vector3 start, vector3 end);

    /// Insert a line into the horizon
    void Insert(float x1, float y1, float x2, float y2);

    /// Insert a line in world coordinates
    void Insert(vector3 start, vector3 end);

    /// Test if a bounding box is above or below the horizon (fast but not accurate)
    TestResult TestFast(const bbox3 &box);
    /// Test if the bottom line of a bounding box is above or below the horizon (fast but not accurate)
    TestResult TestBottom(const bbox3 &box);

    /// Test if a bounding box is above or below the horizon (accurate but slow)
    TestResult Test(const bbox3 &box);

    /// Transform a projected vector horizon's coordinates (viewport coordinates)
    void ToHorizonCoords(vector2 &v) const;

    /// Transform a point in horizon coordinates to viewport coordinates
    vector2 ToViewportCoords(const vector2 &v) const;

    /// Draw the horizon
    void Draw(nGfxServer2 *gfx) const;

private:

    /// horizon width
    int width;
    /// horizon half width
    float halfWidth;
    /// horizon height
    int height;
    /// horizon half height
    float halfHeight;
    /// buffer
    float *buffer;
    /// camera's view-projection matrix
    matrix44 viewProjection;
    /// near plane
    plane nearPlane;

    /// Clip 3D a line with the near plane
    bool ClipLine(vector3 &a, vector3 &b);

    /// Get the top line of the projected bounding box
    bool GetTopLine(const bbox3 &box, vector2 &v0, vector2 &v1) const;
    /// Get the top line of the projected bounding box. Optimized version
    bool GetTopLineFast(const bbox3 &box, vector2 &v0, vector2 &v1) const;
    /// Get the bottom line of the projected bounding box
    void GetBottomLine(const bbox3 &box, vector2 &v0, vector2 &v1) const;

};

//------------------------------------------------------------------------------
/**
    Constructor 1
*/
inline
nHorizonClipper::nHorizonClipper():
width(0),
height(0),
halfWidth(0),
halfHeight(0),
buffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Constructor 2
*/
inline
nHorizonClipper::nHorizonClipper(int w, int h):
width(w),
height(h)
{
    this->halfWidth  = w * 0.5f;
    this->halfHeight = h * 0.5f;
    this->buffer = n_new_array(float, width);
    this->Clear();
}

//------------------------------------------------------------------------------
/**
    Copy constructor
*/
inline
nHorizonClipper::nHorizonClipper(const nHorizonClipper &horiz)
{
    // copy the parameters of the given horizon
    this->Copy(horiz);
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
inline
nHorizonClipper::~nHorizonClipper()
{
    if ( this->buffer )
    {
        n_delete_array(this->buffer);
        this->buffer = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Initialize horizon
*/
inline
void 
nHorizonClipper::Initialize(int width, int height)
{
    this->width  = width;
    this->height = height;
    this->halfWidth  = width  * 0.5f;
    this->halfHeight = height * 0.5f;

    if (this->buffer)
    {
        n_delete_array(this->buffer);
        this->buffer = 0;
    }

    this->buffer = n_new_array(float, width);

    this->Clear();
}

//------------------------------------------------------------------------------
/**
    Copy operator
*/
inline
void 
nHorizonClipper::operator=(const nHorizonClipper &horiz)
{
    // copy the parameters of the given horizon
    this->Copy(horiz);
}

//------------------------------------------------------------------------------
/**
    Copy parameters from another horizon
*/
inline
void 
nHorizonClipper::Copy(const nHorizonClipper &horiz)
{
    // initialize this horizon with the same parameters as the given one
    this->Initialize(horiz.width, horiz.height);

    // copy view-projection matrix
    this->viewProjection = horiz.viewProjection;

    // copy buffer information from the given horizon
    memcpy(this->buffer, horiz.buffer, horiz.width*sizeof(float));
}

//------------------------------------------------------------------------------
/**
    Clear the horizon to a height of zero
*/
inline
void 
nHorizonClipper::Clear()
{
    memset(this->buffer, 0, this->width*sizeof(float));
}

//------------------------------------------------------------------------------
/**
    Transform a projected vector to horizon's coordinates
*/
inline
void
nHorizonClipper::ToHorizonCoords(vector2 &v) const
{
    v.x = (v.x + 1.f) * this->halfWidth;
    v.y = (v.y + 1.f) * this->halfHeight;
}

//------------------------------------------------------------------------------
/**
    Transform a point in horizon coordinates to viewport coordinates
*/
inline
vector2 
nHorizonClipper::ToViewportCoords(const vector2 &v) const
{
    return vector2((v.x - this->halfWidth) / this->halfWidth, (v.y - this->halfHeight) / this->halfHeight);
}

//------------------------------------------------------------------------------
/**
    Get horizon's width
*/
inline
int 
nHorizonClipper::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
    Get horizon's height
*/
inline
int 
nHorizonClipper::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
    Get horizon
*/
inline
const float *
nHorizonClipper::GetHorizon() const
{
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
    Set view-projection matrix
*/
inline
void 
nHorizonClipper::SetViewProjection(const matrix44 &m)
{
    this->viewProjection = m;
}

//------------------------------------------------------------------------------
/**
    Get view-projection matrix
*/
inline
const matrix44 &
nHorizonClipper::GetViewProjection() const
{
    return this->viewProjection;
}

//------------------------------------------------------------------------------
/**
    Set near plane
*/
inline
void 
nHorizonClipper::SetNearPlane(const plane &p)
{
    this->nearPlane.set(p.a, p.b, p.c, p.d);
}

//------------------------------------------------------------------------------
/**
    Get near plane
*/
inline
const plane &
nHorizonClipper::GetNearPlane() const
{
    return this->nearPlane;
}

//------------------------------------------------------------------------------
/**
    Clip 3D a line with the near plane
*/
inline
bool 
nHorizonClipper::ClipLine(vector3 &a, vector3 &b)
{
    bool aClipped = (this->nearPlane.distance(a) <= 0);
    bool bClipped = (this->nearPlane.distance(b) <= 0);

    if ( !aClipped && !bClipped )
    {
        // trivially accept
        return true;
    }
    else if ( aClipped && bClipped )
    {
        // trivially reject
        return false;
    }

    line3 l(a, b);
    float t;
    n_verify( this->nearPlane.intersect(l, t) );

    if (aClipped)
    {
        // 'a' is behind the near plane, calculate new intersection point
        a = l.b + (l.m * t);
    }
    else
    {
        // 'b' is behind the near plane, calculate new intersection point
        b = l.b + (l.m * t);
    }

    return true;
}

#endif // N_HORIZONCLIPPER_H

