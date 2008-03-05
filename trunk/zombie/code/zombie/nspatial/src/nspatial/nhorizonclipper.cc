#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nhorizonclipper.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/nhorizonclipper.h"

//------------------------------------------------------------------------------
/**
    Test if a line is above or below the horizon
*/
nHorizonClipper::TestResult 
nHorizonClipper::Test(float x1, float y1, float x2, float y2) const
{
    if (x1 > x2)
    {
        // switch x1 and x2
        float t = x1;
        x1 = x2;
        x2 = t;

        t = y1;
        y1 = y2;
        y2 = t;
    }

    if ( (x2 < 0) || (x1 > this->width-1) )
    {
        return BELOW;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;

    const float dydx = dy / dx;

    if (x1 < 0.f)
    {
        // clip left

        y1 += -x1 * dydx;
        x1 = 0.f;
    }

    const float right = static_cast<float>(this->width - 1);

    if (x2 > right)
    {
        // clip right

        y2 -= (right - x2) * dy;
        x2 = right;
    }

    const int ix1 = static_cast<const int>(n_ceil(x1-0.5f));
    const int ix2 = static_cast<const int>(n_floor(x2-0.5f));

    float y = y1;

    for (int x = ix1; x <= ix2; ++x)
    {
        NLOGCOND(visibilityVisitor, x < 0, (1, "Testing Horizon ERROR!!!... x1 = %f, y1 = %f, x2 = %f, y2 = %f",
                                                x1, y1, x2, y2));
        
        n_assert_return(x >= 0, BELOW);
        n_assert_return(x < this->width, BELOW);

        if ( y > buffer[x] )
        {
            return ABOVE;
        }

        y += dydx;
    }

    return BELOW;
}

//------------------------------------------------------------------------------
/**
    Test if a line is completely above the horizon
*/
nHorizonClipper::TestResult 
nHorizonClipper::TestCompletelyAbove(float x1, float y1, float x2, float y2) const
{
    if (x1 > x2)
    {
        // switch x1 and x2
        float t = x1;
        x1 = x2;
        x2 = t;

        t = y1;
        y1 = y2;
        y2 = t;
    }

    if ( (x2 < 0) || (x1 > this->width-1) )
    {
        return BELOW;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;

    const float dydx = dy / dx;

    if (x1<0)
    {
        // clip left

        y1 += -x1 * dydx;
        x1 = 0;
    }

    const float right = static_cast<float>(this->width - 1);

    if (x2>right)
    {
        // clip right

        y2 -= (right-x2) * dy;
        x2 = right;
    }

    const int ix1 = static_cast<const int>(n_ceil(x1-0.5f));
    const int ix2 = static_cast<const int>(n_floor(x2-0.5f));

    float y = y1;

    for (int x = ix1; x <= ix2; x++)
    {
        n_assert(x >= 0);
        n_assert(x < this->width);

        if ( y <= buffer[x] )
            return BELOW;

        y += dydx;
    }

    return ABOVE;
}

//------------------------------------------------------------------------------
/**
    Insert a line into the horizon
*/
void 
nHorizonClipper::Insert(float x1, float y1, float x2, float y2)
{
    if ( x1>x2 )
    {
        // switch x1 and x2
        float t = x1;
        x1 = x2;
        x2 = t;

        t = y1;
        y1 = y2;
        y2 = t;
    }

    if ( (x2 < 0) || (x1 >= this->width-1) )
    {
        return;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;
    const float dydx = dy / dx;

    if ( x1 < 0 )
    {
        // clip left

        y1 += -x1 * dydx;
        x1 = 0;
    }

    const float right = static_cast<float>(this->width - 1);
    if ( x2 > right )
    {
        // clip right

        y2 -= (x2-right) * dydx;
        x2 = right;
    }

    const int ix1 = static_cast<const int>(n_ceil(x1-0.5f));
    const int ix2 = static_cast<const int>(n_floor(x2-0.5f));

    float y = y1;

    for (int x = ix1; x <= ix2; x++)
    {
        n_assert(x >= 0);
        n_assert(x < this->width);

        if (this->buffer[x] < y)
        {
            this->buffer[x] = y;
        }

        y += dydx;
    }
}

//------------------------------------------------------------------------------
/**
    Insert a line in world coordinates
*/
void 
nHorizonClipper::Insert(vector3 start, vector3 end)
{
    if (this->ClipLine(start, end))
    {
        static vector4 projStart0;
        static vector4 projStart;

        static vector4 projEnd0;
        static vector4 projEnd;

        projStart0.set(start);
        projStart.x = this->viewProjection.M11*projStart0.x + 
                      this->viewProjection.M21*projStart0.y + 
                      this->viewProjection.M31*projStart0.z + 
                      this->viewProjection.M41*projStart0.w;

        projStart.y = this->viewProjection.M12*projStart0.x + 
                      this->viewProjection.M22*projStart0.y + 
                      this->viewProjection.M32*projStart0.z + 
                      this->viewProjection.M42*projStart0.w;

        projStart.z = this->viewProjection.M13*projStart0.x + 
                      this->viewProjection.M23*projStart0.y + 
                      this->viewProjection.M33*projStart0.z + 
                      this->viewProjection.M43*projStart0.w;

        projStart.w = this->viewProjection.M14*projStart0.x + 
                      this->viewProjection.M24*projStart0.y + 
                      this->viewProjection.M34*projStart0.z + 
                      this->viewProjection.M44*projStart0.w;

        projEnd0.set(end);
        projEnd.x = this->viewProjection.M11*projEnd0.x + 
                    this->viewProjection.M21*projEnd0.y + 
                    this->viewProjection.M31*projEnd0.z + 
                    this->viewProjection.M41*projEnd0.w;

        projEnd.y = this->viewProjection.M12*projEnd0.x + 
                    this->viewProjection.M22*projEnd0.y + 
                    this->viewProjection.M32*projEnd0.z + 
                    this->viewProjection.M42*projEnd0.w;

        projEnd.z = this->viewProjection.M13*projEnd0.x + 
                    this->viewProjection.M23*projEnd0.y + 
                    this->viewProjection.M33*projEnd0.z + 
                    this->viewProjection.M43*projEnd0.w;

        projEnd.w = this->viewProjection.M14*projEnd0.x + 
                    this->viewProjection.M24*projEnd0.y + 
                    this->viewProjection.M34*projEnd0.z + 
                    this->viewProjection.M44*projEnd0.w;

        n_assert(projStart.w >= 0);
        n_assert(projEnd.w >= 0);

        float oow = 1.f / projStart.w;
        projStart.x *= oow;
        projStart.y *= oow;
        oow = 1.f / projEnd.w;
        projEnd.x   *= oow;
        projEnd.y   *= oow;

        // pass the line to horizon coordinates
        float finalStartX = (projStart.x+1) * this->halfWidth;
        float finalStartY = (projStart.y+1) * this->halfHeight;
        float finalEndX   = (projEnd.x+1) * this->halfWidth;
        float finalEndY   = (projEnd.y+1) * this->halfHeight;

        // insert the line into the horizon
        this->Insert(finalStartX, finalStartY, finalEndX, finalEndY);
    }
}

//------------------------------------------------------------------------------
/**
    Test a line in world coordinates
*/
nHorizonClipper::TestResult
nHorizonClipper::Test(vector3 start, vector3 end)
{
    if (this->ClipLine(start, end))
    {
        static vector4 projStart0;
        static vector4 projStart;

        static vector4 projEnd0;
        static vector4 projEnd;

        projStart0.set(start);
        projStart.x = this->viewProjection.M11*projStart0.x + 
                      this->viewProjection.M21*projStart0.y + 
                      this->viewProjection.M31*projStart0.z + 
                      this->viewProjection.M41*projStart0.w;

        projStart.y = this->viewProjection.M12*projStart0.x + 
                      this->viewProjection.M22*projStart0.y + 
                      this->viewProjection.M32*projStart0.z + 
                      this->viewProjection.M42*projStart0.w;

        projStart.z = this->viewProjection.M13*projStart0.x + 
                      this->viewProjection.M23*projStart0.y + 
                      this->viewProjection.M33*projStart0.z + 
                      this->viewProjection.M43*projStart0.w;

        projStart.w = this->viewProjection.M14*projStart0.x + 
                      this->viewProjection.M24*projStart0.y + 
                      this->viewProjection.M34*projStart0.z + 
                      this->viewProjection.M44*projStart0.w;

        projEnd0.set(end);
        projEnd.x = this->viewProjection.M11*projEnd0.x + 
                    this->viewProjection.M21*projEnd0.y + 
                    this->viewProjection.M31*projEnd0.z + 
                    this->viewProjection.M41*projEnd0.w;

        projEnd.y = this->viewProjection.M12*projEnd0.x + 
                    this->viewProjection.M22*projEnd0.y + 
                    this->viewProjection.M32*projEnd0.z + 
                    this->viewProjection.M42*projEnd0.w;

        projEnd.z = this->viewProjection.M13*projEnd0.x + 
                    this->viewProjection.M23*projEnd0.y + 
                    this->viewProjection.M33*projEnd0.z + 
                    this->viewProjection.M43*projEnd0.w;

        projEnd.w = this->viewProjection.M14*projEnd0.x + 
                    this->viewProjection.M24*projEnd0.y + 
                    this->viewProjection.M34*projEnd0.z + 
                    this->viewProjection.M44*projEnd0.w;

        n_assert(projStart.w >= 0);
        n_assert(projEnd.w >= 0);

        float oow = 1.f / projStart.w;
        projStart.x *= oow;
        projStart.y *= oow;
        oow = 1.f / projEnd.w;
        projEnd.x   *= oow;
        projEnd.y   *= oow;

        // pass the line to horizon coordinates
        float finalStartX = (projStart.x+1) * this->halfWidth;
        float finalStartY = (projStart.y+1) * this->halfHeight;
        float finalEndX   = (projEnd.x+1) * this->halfWidth;
        float finalEndY   = (projEnd.y+1) * this->halfHeight;

        // test the line with the horizon
        return this->Test(finalStartX, finalStartY, finalEndX, finalEndY);
    }

    return BELOW;
}

//------------------------------------------------------------------------------
/**
    Get the top line of the projected bounding box
    Optimized version
*/
bool
nHorizonClipper::GetTopLineFast(const bbox3 &box, vector2 &v0, vector2 &v1) const
{
    static vector4 p;

    float oow;

    // common products have been optimized and calculated only once

    float m14_vminx = this->viewProjection.M14 * box.vmin.x;
    float m24_vmaxy = this->viewProjection.M24 * box.vmax.y;
    float m34_vminz = this->viewProjection.M34 * box.vmin.z;

    float w1 = m14_vminx + 
               m24_vmaxy + 
               m34_vminz + 
               this->viewProjection.M44;

    if (w1 <= 0)
    {
        return true;
    }

    float m14_vmaxx = this->viewProjection.M14 * box.vmax.x;
    float w2 = m14_vmaxx + 
               m24_vmaxy + 
               m34_vminz + 
               this->viewProjection.M44;

    if (w2 <= 0)
    {
        return true;
    }

    float m34_vmaxz = this->viewProjection.M34 * box.vmax.z;
    float w4 = m14_vmaxx + 
               m24_vmaxy + 
               m34_vmaxz + 
               this->viewProjection.M44;

    if (w4 <= 0)
    {
        return true;
    }

    float w5 = m14_vminx + 
               m24_vmaxy + 
               m34_vmaxz + 
               this->viewProjection.M44;

    if (w5 <= 0)
    {
        return true;
    }

    float m24_vminy = this->viewProjection.M24 * box.vmin.y;
    float w0 = m14_vminx + 
               m24_vminy + 
               m34_vminz + 
               this->viewProjection.M44;

    if (w0 <= 0)
    {
        return true;
    }

    float w6 = m14_vminx + 
               m24_vminy + 
               m34_vmaxz + 
               this->viewProjection.M44;

    if (w6 <= 0)
    {
        return true;
    }

    float w7 = m14_vmaxx + 
               m24_vminy + 
               m34_vmaxz + 
               this->viewProjection.M44;

    if (w7 <= 0)
    {
        return true;
    }

    float w3 = m14_vmaxx + 
               m24_vminy + 
               m34_vminz + 
               this->viewProjection.M44;

    if (w3 <= 0)
    {
        return true;
    }
    
    float m11_vminx = this->viewProjection.M11 * box.vmin.x;
    float m12_vminx = this->viewProjection.M12 * box.vmin.x;
    
    float m11_vmaxx = this->viewProjection.M11 * box.vmax.x;
    float m12_vmaxx = this->viewProjection.M12 * box.vmax.x;
    
    float m21_vminy = this->viewProjection.M21 * box.vmin.y;
    float m22_vminy = this->viewProjection.M22 * box.vmin.y;
    
    float m21_vmaxy = this->viewProjection.M21 * box.vmax.y;
    float m22_vmaxy = this->viewProjection.M22 * box.vmax.y;
    
    float m31_vminz = this->viewProjection.M31 * box.vmin.z;
    float m32_vminz = this->viewProjection.M32 * box.vmin.z;
    
    float m31_vmaxz = this->viewProjection.M31 * box.vmax.z;
    float m32_vmaxz = this->viewProjection.M32 * box.vmax.z;
    

    // project bounding box's corner 1
    p.x = m11_vminx + 
          m21_vmaxy + 
          m31_vminz + 
          this->viewProjection.M41;

    p.y = m12_vminx + 
          m22_vmaxy + 
          m32_vminz + 
          this->viewProjection.M42;

    oow = 1.f / w1;

    v0.x = p.x * oow;
    v1.x = v0.x;
    v0.y = p.y * oow;
    // v1.y will be the same as v0.y
    

    // project bounding box's corner 2

    p.x = m11_vmaxx + 
          m21_vmaxy + 
          m31_vminz + 
          this->viewProjection.M41;

    p.y = m12_vmaxx + 
          m22_vmaxy + 
          m32_vminz + 
          this->viewProjection.M42;

    
    oow = 1.f / w2;
    float auxX = p.x * oow;
    float auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }
    


    // project bounding box's corner 4
    
    p.x = m11_vmaxx + 
          m21_vmaxy + 
          m31_vmaxz + 
          this->viewProjection.M41;

    p.y = m12_vmaxx + 
          m22_vmaxy + 
          m32_vmaxz + 
          this->viewProjection.M42;

    oow = 1.f / w4;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 5
    
    p.x = m11_vminx + 
          m21_vmaxy + 
          m31_vmaxz + 
          this->viewProjection.M41;

    p.y = m12_vminx + 
          m22_vmaxy + 
          m32_vmaxz + 
          this->viewProjection.M42;

    oow = 1.f / w5;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }
    
    // project bounding box's corner 0
    
    p.x = m11_vminx + 
          m21_vminy + 
          m31_vminz + 
          this->viewProjection.M41;

    p.y = m12_vminx + 
          m22_vminy + 
          m32_vminz + 
          this->viewProjection.M42;

    oow = 1.f / w0;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 6
    
    p.x = m11_vminx + 
          m21_vminy + 
          m31_vmaxz + 
          this->viewProjection.M41;

    p.y = m12_vminx + 
          m22_vminy + 
          m32_vmaxz + 
          this->viewProjection.M42;

    oow = 1.f / w6;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 7
    
    p.x = m11_vmaxx + 
          m21_vminy + 
          m31_vmaxz + 
          this->viewProjection.M41;

    p.y = m12_vmaxx + 
          m22_vminy + 
          m32_vmaxz + 
          this->viewProjection.M42;

    oow = 1.f / w7;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 3
    
    p.x = m11_vmaxx + 
          m21_vminy + 
          m31_vminz + 
          this->viewProjection.M41;

    p.y = m12_vmaxx + 
          m22_vminy + 
          m32_vminz + 
          this->viewProjection.M42;

    oow = 1.f / w3;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // finally, put to v1 the same 'y' value as v0 (it's only necessary to do it once)
    v1.y = v0.y;

    // the bounding box is not intersecting the near plane
    return false;
}

//------------------------------------------------------------------------------
/**
    Get the top line of the projected bounding box
*/
bool
nHorizonClipper::GetTopLine(const bbox3 &box, vector2 &v0, vector2 &v1) const
{
    static vector4 p;

    float oow = -1;

    // initialize v0 and v1
    v0.x =  FLT_MAX;
    v0.y = -FLT_MAX;
    v1.x = -FLT_MAX;
    v1.y = -FLT_MAX;

    // project bounding box's corner 1
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;

    v0.x = p.x * oow;
    v1.x = v0.x;
    v0.y = p.y * oow;
    
    // project bounding box's corner 2
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    float auxX = p.x * oow;
    float auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }
    
    // project bounding box's corner 4
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 5
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }
    
    // project bounding box's corner 0
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 6
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 7
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 3
    p.x = this->viewProjection.M11*box.vmax.x + 
          this->viewProjection.M21*box.vmax.y + 
          this->viewProjection.M31*box.vmin.z + 
          this->viewProjection.M41;

    p.y = this->viewProjection.M12*box.vmax.x + 
          this->viewProjection.M22*box.vmax.y + 
          this->viewProjection.M32*box.vmin.z + 
          this->viewProjection.M42;

    p.z = this->viewProjection.M13*box.vmax.x + 
          this->viewProjection.M23*box.vmax.y + 
          this->viewProjection.M33*box.vmin.z + 
          this->viewProjection.M43;

    p.w = this->viewProjection.M14*box.vmax.x + 
          this->viewProjection.M24*box.vmax.y + 
          this->viewProjection.M34*box.vmin.z + 
          this->viewProjection.M44;

    if (p.w < 0)
    {
        return true;
    }
    oow = 1.f / p.w;
    auxX = p.x * oow;
    auxY = p.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY > v0.y )
    {
        v0.y = auxY;
    }

    // finally, put to v1 the same 'y' value as v0 (it's only necessary to do it once)
    v1.y = v0.y;

    // the bounding box is not intersecting the near plane
    return false;
}

//------------------------------------------------------------------------------
/**
    Get the bottom line of the projected bounding box
*/
void
nHorizonClipper::GetBottomLine(const bbox3 &box, vector2 &v0, vector2 &v1) const
{
    static vector4 p0;
    static vector4 p1;
    float oow = -1;

    // initialize v0 and v1
    v0.x = FLT_MAX;
    v0.y = FLT_MAX;
    v1.x = -FLT_MAX;
    v1.y = FLT_MAX;

    // project bounding box's corner 6
    p0.set(box.vmin.x, box.vmin.y, box.vmax.z, 1.f);
    p1 = this->viewProjection * p0;
    oow = 1.f / p1.w;

    v0.x = p1.x * oow;
    v1.x = v0.x;
    v0.y = p1.y * oow;
    
    // project bounding box's corner 7
    p0.set(box.vmax.x, box.vmin.y, box.vmax.z, 1.f);
    p1 = this->viewProjection * p0;
    oow = 1.f / p1.w;
    float auxX = p1.x * oow;
    float auxY = p1.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else
    {
        v1.x = auxX;
    }

    if ( auxY < v0.y )
    {
        v0.y = auxY;
    }
    
    // project bounding box's corner 0
    p0.set(box.vmin.x, box.vmin.y, box.vmin.z, 1.f);
    p1 = this->viewProjection * p0;
    oow = 1.f / p1.w;
    auxX = p1.x * oow;
    auxY = p1.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY < v0.y )
    {
        v0.y = auxY;
    }

    // project bounding box's corner 3
    p0.set(box.vmax.x, box.vmin.y, box.vmin.z, 1.f);
    p1 = this->viewProjection * p0;
    oow = 1.f / p1.w;
    auxX = p1.x * oow;
    auxY = p1.y * oow;
    if ( auxX < v0.x )
    {
        v0.x = auxX;
    }
    else if ( auxX > v1.x )
    {
        v1.x = auxX;
    }

    if ( auxY < v0.y )
    {
        v0.y = auxY;
    }
    
    // finally, put to v1 the same y value as v0 (it's only necessary to do it once)
    v1.y = v0.y;
}

//------------------------------------------------------------------------------
/**
    Test if a bounding box is above or below the horizon
*/
nHorizonClipper::TestResult 
nHorizonClipper::TestFast(const bbox3 &box)
{
    vector2 vt0;
    vector2 vt1;

    NLOG(visibilityVisitor, (1, "START Testing Horizon... BOX: vmin.x = %f, vmin.y = %f, vmin.z = %f, \
                             vmax.x = %f, vmax.y = %f, vmax.z = %f", \
                             box.vmin.x, box.vmin.y, box.vmin.z, 
                             box.vmax.x, box.vmax.y, box.vmax.z));

    // get the top line of the projected bounding box
    bool nearClipped = this->GetTopLineFast(box, vt0, vt1);

    NLOG(visibilityVisitor, (1, "Testing Horizon... TOP LINE: vt0.x = %f, vt0.y = %f, vt1.x = %f, vt1.y = %f", \
                            vt0.x, vt0.y, vt1.x, vt1.y));
    if (nearClipped)
    {
        return ABOVE;
    }
    
    // transform the top line of the rectangle to horizon coordinates
    this->ToHorizonCoords(vt0);
    this->ToHorizonCoords(vt1);

    NLOG(visibilityVisitor, (1, "Testing Horizon... TO HORIZON COORDS: vt0.x = %f, vt0.y = %f, vt1.x = %f, vt1.y = %f, nearClipped = %b", \
                            vt0.x, vt0.y, vt1.x, vt1.y, nearClipped));

    nHorizonClipper::TestResult result = this->Test(vt0.x, vt0.y, vt1.x, vt1.y);
    
    NLOG(visibilityVisitor, (1, "STOP TestingHorizon... vt0.x = %f, vt0.y = %f, vt1.x = %f, vt1.y = %f",
                             vt0.x, vt0.y, vt1.x, vt1.y));
    return result;
}

//------------------------------------------------------------------------------
/**
    Test if the bottom of a bounding box is above or below the horizon
*/
nHorizonClipper::TestResult 
nHorizonClipper::TestBottom(const bbox3 &box)
{
    // get the rectangle of the projected bounding box
    vector2 v0;
    vector2 v1;
    this->GetBottomLine(box, v0, v1);

    // transform the top line of the rectangle to horizon coordinates
    this->ToHorizonCoords(v0);
    this->ToHorizonCoords(v1);

    return this->TestCompletelyAbove(v0.x, v0.y, v1.x, v1.y);
}

//------------------------------------------------------------------------------
/**
    Test if a bounding box is above or below the horizon
*/
nHorizonClipper::TestResult 
nHorizonClipper::Test(const bbox3 &box)
{
    static vector3 v0;
    static vector3 v1; 
    static vector3 v2;
    static vector3 v3;
    static vector3 v4;
    static vector3 v5;
    static vector3 v6;
    static vector3 v7;

    v0.set(box.corner_point(0));
    v1.set(box.corner_point(1));
    v2.set(box.corner_point(2));
    v3.set(box.corner_point(3));
    v4.set(box.corner_point(4));
    v5.set(box.corner_point(5));
    v6.set(box.corner_point(6));
    v7.set(box.corner_point(7));

    if (this->Test(v1, v2) == BELOW && this->Test(v2, v4) == BELOW &&
        this->Test(v4, v5) == BELOW && this->Test(v5, v1) == BELOW &&
        this->Test(v1, v0) == BELOW && this->Test(v2, v3) == BELOW &&
        this->Test(v4, v7) == BELOW && this->Test(v5, v6) == BELOW &&
        this->Test(v0, v3) == BELOW && this->Test(v3, v7) == BELOW &&
        this->Test(v7, v6) == BELOW && this->Test(v6, v0) == BELOW)
    {
        return BELOW;
    }

    return ABOVE;
}

//------------------------------------------------------------------------------
/**
    Draw the horizon
*/
void 
nHorizonClipper::Draw(nGfxServer2 *gfx) const
{
    if ( this->width == 0 )
    {
        return;
    }

    // transform buffer to viewport coordinates
    vector4 horizonColor(0.8f, 0.8f, 1.f, 1.f);
    vector2 *points = n_new_array(vector2, this->width);
    for (int i = 0; i < this->width; i++)
    {
        points[i].x = static_cast<float>(i);
        points[i].y = this->height - this->buffer[i];
    }

    // draw horizon
    gfx->BeginLines();
    gfx->DrawLines2d(points, this->width, horizonColor);
    gfx->EndLines();

    n_delete_array(points);
}
