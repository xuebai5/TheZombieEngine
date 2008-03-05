#ifndef _MATRIX44D_H
#define _MATRIX44D_H

#ifndef N_PIdouble
#define N_PIdouble (3.1415926535897932384626433832795028841971693993751)
#endif

//------------------------------------------------------------------------------
/**
    @class _matrix44
    @ingroup NebulaMathDataTypes

    Generic matrix44 class.

    (C) 2002 RadonLabs GmbH
*/
#include "mathlib/_vector4.h"
#include "mathlib/_vector3.h"
#include "mathlib/quaternion.h"
#include "mathlib/euler.h"
#include "mathlib/matrixdefs.h"

static double _matrix44d_ident[16] = 
{
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
};

//------------------------------------------------------------------------------
class _matrix44d
{
public:
    /// constructor 1
    _matrix44d();
    /// constructor 2
    _matrix44d(const _vector4& v0, const _vector4& v1, const _vector4& v2, const _vector4& v3);
    /// constructor 3
    _matrix44d(const _matrix44d& m1);
    /// constructor 4
    _matrix44d(double _m11, double _m12, double _m13, double _m14,
              double _m21, double _m22, double _m23, double _m24,
              double _m31, double _m32, double _m33, double _m34,
              double _m41, double _m42, double _m43, double _m44);
    /// constructor 5
    _matrix44d(const matrix44& m);
    /// construct from quaternion
    _matrix44d(const quaternion& q);

    /// convert to quaternion
    quaternion get_quaternion() const;
    /// set 1
    void set(const _vector4& v0, const _vector4& v1, const _vector4& v2, const _vector4& v3);
    /// set 2
    void set(const _matrix44d& m1);
    /// set 3
    void set(double _m11, double _m12, double _m13, double _m14,
             double _m21, double _m22, double _m23, double _m24,
             double _m31, double _m32, double _m33, double _m34,
             double _m41, double _m42, double _m43, double _m44);
    /// set from quaternion
    void set(const quaternion& q);
    /// set to identity
    void ident();
    /// transpose
    void transpose();
    /// determinant
    double det();
    /// full invert
    void invert(void);
    /// quick invert (if 3x3 rotation and translation)
    void invert_simple(void);
    /// quick multiplication, assumes that M14==M24==M34==0 and M44==1
    void mult_simple(const _matrix44d& m1);
    /// transform vector3, projecting back into w=1
    _vector3 transform_coord(const _vector3& v) const;
    /// return x component
    _vector3 x_component() const;
    /// return y component
    _vector3 y_component() const;
    /// return z component
    _vector3 z_component() const;
    /// return translate component
    _vector3 pos_component() const;
    /// rotate around global x
    void rotate_x(const double a);
    /// rotate around global y
    void rotate_y(const double a);
    /// rotate around global z
    void rotate_z(const double a);
    /// rotate about any axis
    void rotate(const _vector3& vec, double a);
    /// rotate about any axis and pases sinus and cosinus
    void rotate(double x, double y, double z, double sinus, double cosinus);
    /// translate
    void translate(const _vector3& t);
    /// set absolute translation
    void set_translation(const _vector3& t);
    /// scale
    void scale(const _vector3& s);
    /// scale
    void scale(double x, double y, double z);
    /// lookat in a left-handed coordinate system
    void lookatLh(const _vector3& to, const _vector3& up);
    /// lookat in a right-handed coordinate system
    void lookatRh(const _vector3& to, const _vector3& up);
    /// create left-handed field-of-view perspective projection matrix
    void perspFovLh(double fovY, double aspect, double zn, double zf);
    /// create right-handed field-of-view perspective projection matrix
    void perspFovRh(double fovY, double aspect, double zn, double zf);
    /// create off-center left-handed perspective projection matrix
    void perspOffCenterLh(double minX, double maxX, double minY, double maxY, double zn, double zf);
    /// create off-center right-handed perspective projection matrix
    void perspOffCenterRh(double minX, double maxX, double minY, double maxY, double zn, double zf);
    /// create left-handed orthogonal projection matrix
    void orthoLh(double w, double h, double zn, double zf);
    /// create right-handed orthogonal projection matrix
    void orthoRh(double w, double h, double zn, double zf);
    /// restricted lookat
    void billboard(const _vector3& to, const _vector3& up);
    /// inplace matrix mulitply
    void operator *= (const _matrix44d& m1);
    /// multiply source vector into target vector, eliminates tmp vector
    void mult(const _vector4& src, _vector4& dst) const;
    /// multiply source vector into target vector, eliminates tmp vector
    void mult(const _vector3& src, _vector3& dst) const;
    /// get the canonical transformation of this matrix
    void get(_vector3& scale,quaternion& quat,_vector3& pos) const;
    /// get the matrix33
    matrix33 getMatrix33();

    double m[4][4];
};

//------------------------------------------------------------------------------
/**
*/
inline
_matrix44d::_matrix44d()
{
    memcpy(&(m[0][0]), _matrix44d_ident, sizeof(_matrix44d_ident));
}

//------------------------------------------------------------------------------
/**
*/
inline
_matrix44d::_matrix44d(const _vector4& v0, const _vector4& v1, const _vector4& v2, const _vector4& v3)
{
    M11 = v0.x; M12 = v0.y; M13 = v0.z; M14 = v0.w;
    M21 = v1.x; M22 = v1.y; M23 = v1.z; M24 = v1.w;
    M31 = v2.x; M32 = v2.y; M33 = v2.z; M34 = v2.w;
    M41 = v3.x; M42 = v3.y; M43 = v3.z; M44 = v3.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
_matrix44d::_matrix44d(const _matrix44d& m1) 
{
    memcpy(m, &(m1.m[0][0]), 16 * sizeof(double));
}

//------------------------------------------------------------------------------
/**
*/
inline
_matrix44d::_matrix44d(double _m11, double _m12, double _m13, double _m14,
                     double _m21, double _m22, double _m23, double _m24,
                     double _m31, double _m32, double _m33, double _m34,
                     double _m41, double _m42, double _m43, double _m44)
{
    M11 = _m11; M12 = _m12; M13 = _m13; M14 = _m14;
    M21 = _m21; M22 = _m22; M23 = _m23; M24 = _m24;
    M31 = _m31; M32 = _m32; M33 = _m33; M34 = _m34;
    M41 = _m41; M42 = _m42; M43 = _m43; M44 = _m44;
}


inline
_matrix44d::_matrix44d(const matrix44& src)
{  
    M11 = src.M11; M12 = src.M12; M13 = src.M13; M14 = src.M14;
    M21 = src.M21; M22 = src.M22; M23 = src.M23; M24 = src.M24;
    M31 = src.M31; M32 = src.M32; M33 = src.M33; M34 = src.M34;
    M41 = src.M41; M42 = src.M42; M43 = src.M43; M44 = src.M44;
}

//------------------------------------------------------------------------------
/**
*/
inline
_matrix44d::_matrix44d(const quaternion& q) 
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = q.x + q.x; y2 = q.y + q.y; z2 = q.z + q.z;
    xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
    yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
    wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

    m[0][0] = 1.0 - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;

    m[0][1] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[2][1] = yz - wx;

    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);

    m[3][0] = m[3][1] = m[3][2] = 0.0;
    m[0][3] = m[1][3] = m[2][3] = 0.0;
    m[3][3] = 1.0;
}

//------------------------------------------------------------------------------
/**
    convert orientation of 4x4 matrix into quaterion,
    4x4 matrix must not be scaled!
*/
inline
quaternion 
_matrix44d::get_quaternion() const
{
    double qa[4];
    double tr = m[0][0] + m[1][1] + m[2][2];
    if (tr > 0.0f) 
    {
        double s = sqrt (tr + 1.0);
        qa[3] = s * 0.5;
        s = 0.5 / s;
        qa[0] = (m[1][2] - m[2][1]) * s;
        qa[1] = (m[2][0] - m[0][2]) * s;
        qa[2] = (m[0][1] - m[1][0]) * s;
    } 
    else 
    {
        int i, j, k, nxt[3] = {1,2,0};
        i = 0;
        if (m[1][1] > m[0][0]) i=1;
        if (m[2][2] > m[i][i]) i=2;
        j = nxt[i];
        k = nxt[j];
        double s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
        qa[i] = s * 0.5;
        s = 0.5 / s;
        qa[3] = (m[j][k] - m[k][j])* s;
        qa[j] = (m[i][j] + m[j][i]) * s;
        qa[k] = (m[i][k] + m[k][i]) * s;
    }
    quaternion q( (float)qa[0] , (float)qa[1] , (float)qa[2], (float)qa[3]);
    return q;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::set(const _vector4& v0, const _vector4& v1, const _vector4& v2, const _vector4& v3) 
{
    M11=v0.x; M12=v0.y; M13=v0.z, M14=v0.w;
    M21=v1.x; M22=v1.y; M23=v1.z; M24=v1.w;
    M31=v2.x; M32=v2.y; M33=v2.z; M34=v2.w;
    M41=v3.x; M42=v3.y; M43=v3.z; M44=v3.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::set(const _matrix44d& m1) 
{
    memcpy(m, &(m1.m[0][0]), 16*sizeof(double));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::set(double _m11, double _m12, double _m13, double _m14,
               double _m21, double _m22, double _m23, double _m24,
               double _m31, double _m32, double _m33, double _m34,
               double _m41, double _m42, double _m43, double _m44)
{
    M11=_m11; M12=_m12; M13=_m13; M14=_m14;
    M21=_m21; M22=_m22; M23=_m23; M24=_m24;
    M31=_m31; M32=_m32; M33=_m33; M34=_m34;
    M41=_m41; M42=_m42; M43=_m43; M44=_m44;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::set(const quaternion& q) 
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = q.x + q.x; y2 = q.y + q.y; z2 = q.z + q.z;
    xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
    yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
    wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

    m[0][0] = 1.0f - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;

    m[0][1] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[2][1] = yz - wx;

    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);

    m[3][0] = m[3][1] = m[3][2] = 0.0f;
    m[0][3] = m[1][3] = m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::ident() 
{
    memcpy(&(m[0][0]), _matrix44d_ident, sizeof(_matrix44d_ident));
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::transpose() 
{
    #undef n_swap
    #define n_swap(x,y) { double t=x; x=y; y=t; }
    n_swap(M12, M21);
    n_swap(M13, M31);
    n_swap(M14, M41);
    n_swap(M23, M32);
    n_swap(M24, M42);
    n_swap(M34, M43);
}

//------------------------------------------------------------------------------
/**
*/
inline
double 
_matrix44d::det() 
{
    return
        (M11 * M22 - M12 * M21) * (M33 * M44 - M34 * M43)
       -(M11 * M23 - M13 * M21) * (M32 * M44 - M34 * M42)
       +(M11 * M24 - M14 * M21) * (M32 * M43 - M33 * M42)
       +(M12 * M23 - M13 * M22) * (M31 * M44 - M34 * M41)
       -(M12 * M24 - M14 * M22) * (M31 * M43 - M33 * M41)
       +(M13 * M24 - M14 * M23) * (M31 * M42 - M32 * M41);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::invert() 
{
    double s = det();
    if (s == 0.0) return;
    s = 1.0/s;
    this->set(
        s*(M22*(M33*M44 - M34*M43) + M23*(M34*M42 - M32*M44) + M24*(M32*M43 - M33*M42)),
        s*(M32*(M13*M44 - M14*M43) + M33*(M14*M42 - M12*M44) + M34*(M12*M43 - M13*M42)),
        s*(M42*(M13*M24 - M14*M23) + M43*(M14*M22 - M12*M24) + M44*(M12*M23 - M13*M22)),
        s*(M12*(M24*M33 - M23*M34) + M13*(M22*M34 - M24*M32) + M14*(M23*M32 - M22*M33)),
        s*(M23*(M31*M44 - M34*M41) + M24*(M33*M41 - M31*M43) + M21*(M34*M43 - M33*M44)),
        s*(M33*(M11*M44 - M14*M41) + M34*(M13*M41 - M11*M43) + M31*(M14*M43 - M13*M44)),
        s*(M43*(M11*M24 - M14*M21) + M44*(M13*M21 - M11*M23) + M41*(M14*M23 - M13*M24)),
        s*(M13*(M24*M31 - M21*M34) + M14*(M21*M33 - M23*M31) + M11*(M23*M34 - M24*M33)),
        s*(M24*(M31*M42 - M32*M41) + M21*(M32*M44 - M34*M42) + M22*(M34*M41 - M31*M44)),
        s*(M34*(M11*M42 - M12*M41) + M31*(M12*M44 - M14*M42) + M32*(M14*M41 - M11*M44)),
        s*(M44*(M11*M22 - M12*M21) + M41*(M12*M24 - M14*M22) + M42*(M14*M21 - M11*M24)),
        s*(M14*(M22*M31 - M21*M32) + M11*(M24*M32 - M22*M34) + M12*(M21*M34 - M24*M31)),
        s*(M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
        s*(M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
        s*(M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
        s*(M11*(M22*M33 - M23*M32) + M12*(M23*M31 - M21*M33) + M13*(M21*M32 - M22*M31)));
}

//------------------------------------------------------------------------------
/**
    inverts a 4x4 matrix consisting of a 3x3 rotation matrix and
    a translation (eg. everything that has [0,0,0,1] as
    the rightmost column) MUCH cheaper then a real 4x4 inversion
*/
inline
void 
_matrix44d::invert_simple() 
{
    double s = det();
    if (s == 0.0) return;
    s = 1.0/s;
    this->set(
        s * ((M22 * M33) - (M23 * M32)),
        s * ((M32 * M13) - (M33 * M12)),
        s * ((M12 * M23) - (M13 * M22)),
        0.0,
        s * ((M23 * M31) - (M21 * M33)),
        s * ((M33 * M11) - (M31 * M13)),
        s * ((M13 * M21) - (M11 * M23)),
        0.0,
        s * ((M21 * M32) - (M22 * M31)),
        s * ((M31 * M12) - (M32 * M11)),
        s * ((M11 * M22) - (M12 * M21)),
        0.0,
        s * (M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
        s * (M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
        s * (M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
        1.0);
}

//------------------------------------------------------------------------------
/**
    optimized multiplication, assumes that M14==M24==M34==0 AND M44==1
*/
inline
void
_matrix44d::mult_simple(const _matrix44d& m1) 
{
    int i;
    for (i=0; i<4; i++) 
    {
        double mi0 = m[i][0];
        double mi1 = m[i][1];
        double mi2 = m[i][2];
        m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0];
        m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1];
        m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2];
    }
    m[3][0] += m1.m[3][0];
    m[3][1] += m1.m[3][1];
    m[3][2] += m1.m[3][2];
    m[0][3] = 0.0;
    m[1][3] = 0.0;
    m[2][3] = 0.0;
    m[3][3] = 1.0;
}

//------------------------------------------------------------------------------
/**
    Transforms a vector by the matrix, projecting the result back into w=1.
*/
inline
_vector3
_matrix44d::transform_coord(const _vector3& v) const
{
    double d = 1.0f / (M14*v.x + M24*v.y + M34*v.z + M44);
    return _vector3(
        float((M11*v.x + M21*v.y + M31*v.z + M41) * d),
        float((M12*v.x + M22*v.y + M32*v.z + M42) * d),
        float((M13*v.x + M23*v.y + M33*v.z + M43) * d));
}

//------------------------------------------------------------------------------
/**
*/
inline
_vector3
_matrix44d::x_component() const
{  
    //return *(_vector3*)&M11;
    return _vector3( float(M11), float(M12), float(M13) );

}

//------------------------------------------------------------------------------
/**
*/
inline
_vector3
_matrix44d::y_component() const
{
    //return *(_vector3*)&M21;
    return _vector3( float(M21), float(M22), float(M23) );
}

//------------------------------------------------------------------------------
/**
*/
inline
_vector3
_matrix44d::z_component() const
{
    //return *(_vector3*)&M31;
    return _vector3( float(M31), float(M32), float(M33) );
}

//------------------------------------------------------------------------------
/**
*/
inline
_vector3
_matrix44d::pos_component() const
{
    //return *(_vector3*)&M41;
    return _vector3( float(M41), float(M42), float(M43) );
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::rotate_x(const double a) 
{
    double c = cos(a);
    double s = sin(a);
    int i;
    for (i=0; i<4; i++) {
        double mi1 = m[i][1];
        double mi2 = m[i][2];
        m[i][1] = mi1*c + mi2*-s;
        m[i][2] = mi1*s + mi2*c;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::rotate_y(const double a) 
{
    double c = cos(a);
    double s = sin(a);
    int i;
    for (i=0; i<4; i++) {
        double mi0 = m[i][0];
        double mi2 = m[i][2];
        m[i][0] = mi0*c + mi2*s;
        m[i][2] = mi0*-s + mi2*c;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::rotate_z(const double a) 
{
    double c = cos(a);
    double s = sin(a);
    int i;
    for (i=0; i<4; i++) {
        double mi0 = m[i][0];
        double mi1 = m[i][1];
        m[i][0] = mi0*c + mi1*-s;
        m[i][1] = mi0*s + mi1*c;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::translate(const _vector3& t) 
{
    M41 += t.x;
    M42 += t.y;
    M43 += t.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::set_translation(const _vector3& t) 
{
    M41 = t.x;
    M42 = t.y;
    M43 = t.z;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::scale(const _vector3& s) 
{
    int i;
    for (i=0; i<4; i++) 
    {
        m[i][0] *= s.x;
        m[i][1] *= s.y;
        m[i][2] *= s.z;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::scale(double x, double y, double z)
{
    int i;
    for (i=0; i<4; i++) 
    {
        m[i][0] *= x;
        m[i][1] *= y;
        m[i][2] *= z;
    }

}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::lookatRh(const _vector3& at, const _vector3& up) 
{
    _vector3 eye((float)M41, (float)M42, (float)M43);
    _vector3 zaxis = eye - at;
    zaxis.norm();
    _vector3 xaxis = up * zaxis;
    xaxis.norm();
    _vector3 yaxis = zaxis * xaxis;
    M11 = xaxis.x;  M12 = xaxis.y;  M13 = xaxis.z;  M14 = 0.0;
    M21 = yaxis.x;  M22 = yaxis.y;  M23 = yaxis.z;  M24 = 0.0;
    M31 = zaxis.x;  M32 = zaxis.y;  M33 = zaxis.z;  M34 = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::lookatLh(const _vector3& at, const _vector3& up) 
{
    _vector3 eye((float)M41, (float)M42, (float)M43);
    _vector3 zaxis = at - eye;
    zaxis.norm();
    _vector3 xaxis = up * zaxis;
    xaxis.norm();
    _vector3 yaxis = zaxis * xaxis;
    M11 = xaxis.x;  M12 = yaxis.x;  M13 = zaxis.x;  M14 = 0.0;
    M21 = xaxis.y;  M22 = yaxis.y;  M23 = zaxis.y;  M24 = 0.0;
    M31 = xaxis.z;  M32 = yaxis.z;  M33 = zaxis.z;  M34 = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::perspFovLh(double fovY, double aspect, double zn, double zf)
{
    double h = double(1.0 / tan(fovY * 0.5));
    double w = h / aspect;
    M11 = w;   M12 = 0.0; M13 = 0.0;                    M14 = 0.0;
    M21 = 0.0; M22 = h;   M23 = 0.0;                    M24 = 0.0;
    M31 = 0.0; M32 = 0.0; M33 = zf / (zf - zn);         M34 = 1.0;
    M41 = 0.0; M42 = 0.0; M43 = -zn * (zf / (zf - zn)); M44 = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::perspFovRh(double fovY, double aspect, double zn, double zf)
{
    double h = double(1.0 / tan(fovY * 0.5));
    double w = h / aspect;
    M11 = w;   M12 = 0.0; M13 = 0.0;                   M14 =  0.0;
    M21 = 0.0; M22 = h;   M23 = 0.0;                   M24 =  0.0;
    M31 = 0.0; M32 = 0.0; M33 = zf / (zn - zf);        M34 = -1.0;
    M41 = 0.0; M42 = 0.0; M43 = zn * (zf / (zn - zf)); M44 =  0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::perspOffCenterLh(double minX, double maxX, double minY, double maxY, double zn, double zf)
{
    M11 = 2.0 * zn / (maxX - minX); M12 = 0.0, M13 = 0.0; M14 = 0.0;
    M21 = 0.0; M22 = 2.0 * zn / (maxY - minY); M23 = 0.0; M24 = 0.0;
    M31 = (minX + maxX) / (minX - maxX); M32 = (maxY + minY) / (minY - maxY); M33 = zf / (zf - zn); 1.0;
    M41 = 0.0; M42 = 0.0; M43 = zn * zf / (zn - zf); M44 = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::perspOffCenterRh(double minX, double maxX, double minY, double maxY, double zn, double zf)
{
    M11 = 2.0 * zn / (maxX - minX); M12 = 0.0, M13 = 0.0; M14 = 0.0;
    M21 = 0.0; M22 = 2.0 * zn / (maxY - minY); M23 = 0.0; M24 = 0.0;
    M31 = (minX + maxX) / (maxX - minX); M32 = (maxY + minY) / (maxY - minY); M33 = zf / (zn - zf); -1.0;
    M41 = 0.0; M42 = 0.0; M43 = zn * zf / (zn - zf); 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::orthoLh(double w, double h, double zn, double zf)
{
    M11 = 2.0 / w; M12 = 0.0;     M13 = 0.0;              M14 = 0.0;
    M21 = 0.0;     M22 = 2.0 / h; M23 = 0.0;              M24 = 0.0;
    M31 = 0.0;     M32 = 0.0;     M33 = 1.0 / (zf - zn);  M34 = 0.0;
    M41 = 0.0;     M42 = 0.0;     M43 = zn / (zn - zf);   M44 = 1.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::orthoRh(double w, double h, double zn, double zf)
{
    M11 = 2.0 / w; M12 = 0.0;     M13 = 0.0;             M14 = 0.0;
    M21 = 0.0;     M22 = 2.0 / h; M23 = 0.0;             M24 = 0.0;
    M31 = 0.0;     M32 = 0.0;     M33 = 1.0 / (zn - zf); M34 = 0.0;
    M41 = 0.0;     M42 = 0.0;     M43 = zn / (zn - zf);  M44 = 1.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::billboard(const _vector3& to, const _vector3& up)
{
    _vector3 from( (float)M41, (float)M42, (float)M43 );
    _vector3 z(from - to);
    z.norm();
    _vector3 y(up);
    y.norm();
    _vector3 x(y * z);
    z = x * y;       

    M11=x.x;  M12=x.y;  M13=x.z;  M14=0.0;
    M21=y.x;  M22=y.y;  M23=y.z;  M24=0.0;
    M31=z.x;  M32=z.y;  M33=z.z;  M34=0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::operator *= (const _matrix44d& m1) 
{
    int i;
    for (i=0; i<4; i++) 
    {
        double mi0 = m[i][0];
        double mi1 = m[i][1];
        double mi2 = m[i][2];
        double mi3 = m[i][3];
        m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0] + mi3*m1.m[3][0];
        m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1] + mi3*m1.m[3][1];
        m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2] + mi3*m1.m[3][2];
        m[i][3] = mi0*m1.m[0][3] + mi1*m1.m[1][3] + mi2*m1.m[2][3] + mi3*m1.m[3][3];
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::rotate(const _vector3& vec, double a)
{
    _vector3 v(vec);
    v.norm();
    double sa = (double) sin(a);
    double ca = (double) cos(a);

	_matrix44d rotM;
	rotM.M11 = ca + (1.0f - ca) * v.x * v.x;
	rotM.M12 = (1.0f - ca) * v.x * v.y - sa * v.z;
	rotM.M13 = (1.0f - ca) * v.z * v.x + sa * v.y;
	rotM.M21 = (1.0f - ca) * v.x * v.y + sa * v.z;
	rotM.M22 = ca + (1.0f - ca) * v.y * v.y;
	rotM.M23 = (1.0f - ca) * v.y * v.z - sa * v.x;
	rotM.M31 = (1.0f - ca) * v.z * v.x - sa * v.y;
	rotM.M32 = (1.0f - ca) * v.y * v.z + sa * v.x;
	rotM.M33 = ca + (1.0f - ca) * v.z * v.z;
	
	(*this) *= rotM;
}

inline
void 
_matrix44d::rotate(double x, double y, double z, double sinus, double cosinus)
{
    double invLength= 1.0 / sqrt(x*x + y*y +z*z);
    x/=invLength;
    y/=invLength;
    z/=invLength;

	_matrix44d rotM;
	rotM.M11 = cosinus + (1.0f - cosinus) * x * x;
	rotM.M12 = (1.0f - cosinus) * x * y - sinus * z;
	rotM.M13 = (1.0f - cosinus) * z * x + sinus * y;
	rotM.M21 = (1.0f - cosinus) * x * y + sinus * z;
	rotM.M22 = cosinus + (1.0f - cosinus) * y * y;
	rotM.M23 = (1.0f - cosinus) * y * z - sinus * x;
	rotM.M31 = (1.0f - cosinus) * z * x - sinus * y;
	rotM.M32 = (1.0f - cosinus) * y * z + sinus * x;
	rotM.M33 = cosinus + (1.0f - cosinus) * z * z;
	
	(*this) *= rotM;

}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::mult(const _vector4& src, _vector4& dst) const
{
    dst.x = float(M11*src.x + M21*src.y + M31*src.z + M41*src.w);
    dst.y = float(M12*src.x + M22*src.y + M32*src.z + M42*src.w);
    dst.z = float(M13*src.x + M23*src.y + M33*src.z + M43*src.w);
    dst.w = float(M14*src.x + M24*src.y + M34*src.z + M44*src.w);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
_matrix44d::mult(const _vector3& src, _vector3& dst) const
{
    dst.x = float(M11*src.x + M21*src.y + M31*src.z + M41);
    dst.y = float(M12*src.x + M22*src.y + M32*src.z + M42);
    dst.z = float(M13*src.x + M23*src.y + M33*src.z + M43);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
_matrix44d operator * (const _matrix44d& m0, const _matrix44d& m1) 
{
    _matrix44d m2(
        m0.m[0][0]*m1.m[0][0] + m0.m[0][1]*m1.m[1][0] + m0.m[0][2]*m1.m[2][0] + m0.m[0][3]*m1.m[3][0],
        m0.m[0][0]*m1.m[0][1] + m0.m[0][1]*m1.m[1][1] + m0.m[0][2]*m1.m[2][1] + m0.m[0][3]*m1.m[3][1],
        m0.m[0][0]*m1.m[0][2] + m0.m[0][1]*m1.m[1][2] + m0.m[0][2]*m1.m[2][2] + m0.m[0][3]*m1.m[3][2],
        m0.m[0][0]*m1.m[0][3] + m0.m[0][1]*m1.m[1][3] + m0.m[0][2]*m1.m[2][3] + m0.m[0][3]*m1.m[3][3],

        m0.m[1][0]*m1.m[0][0] + m0.m[1][1]*m1.m[1][0] + m0.m[1][2]*m1.m[2][0] + m0.m[1][3]*m1.m[3][0],
        m0.m[1][0]*m1.m[0][1] + m0.m[1][1]*m1.m[1][1] + m0.m[1][2]*m1.m[2][1] + m0.m[1][3]*m1.m[3][1],
        m0.m[1][0]*m1.m[0][2] + m0.m[1][1]*m1.m[1][2] + m0.m[1][2]*m1.m[2][2] + m0.m[1][3]*m1.m[3][2],
        m0.m[1][0]*m1.m[0][3] + m0.m[1][1]*m1.m[1][3] + m0.m[1][2]*m1.m[2][3] + m0.m[1][3]*m1.m[3][3],

        m0.m[2][0]*m1.m[0][0] + m0.m[2][1]*m1.m[1][0] + m0.m[2][2]*m1.m[2][0] + m0.m[2][3]*m1.m[3][0],
        m0.m[2][0]*m1.m[0][1] + m0.m[2][1]*m1.m[1][1] + m0.m[2][2]*m1.m[2][1] + m0.m[2][3]*m1.m[3][1],
        m0.m[2][0]*m1.m[0][2] + m0.m[2][1]*m1.m[1][2] + m0.m[2][2]*m1.m[2][2] + m0.m[2][3]*m1.m[3][2],
        m0.m[2][0]*m1.m[0][3] + m0.m[2][1]*m1.m[1][3] + m0.m[2][2]*m1.m[2][3] + m0.m[2][3]*m1.m[3][3],

        m0.m[3][0]*m1.m[0][0] + m0.m[3][1]*m1.m[1][0] + m0.m[3][2]*m1.m[2][0] + m0.m[3][3]*m1.m[3][0],
        m0.m[3][0]*m1.m[0][1] + m0.m[3][1]*m1.m[1][1] + m0.m[3][2]*m1.m[2][1] + m0.m[3][3]*m1.m[3][1],
        m0.m[3][0]*m1.m[0][2] + m0.m[3][1]*m1.m[1][2] + m0.m[3][2]*m1.m[2][2] + m0.m[3][3]*m1.m[3][2],
        m0.m[3][0]*m1.m[0][3] + m0.m[3][1]*m1.m[1][3] + m0.m[3][2]*m1.m[2][3] + m0.m[3][3]*m1.m[3][3]);
    return m2;
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
_vector3 operator * (const _matrix44d& m, const _vector3& v)
{
    return _vector3(
        float(m.M11*v.x + m.M21*v.y + m.M31*v.z + m.M41),
        float(m.M12*v.x + m.M22*v.y + m.M32*v.z + m.M42),
        float(m.M13*v.x + m.M23*v.y + m.M33*v.z + m.M43));
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
_vector4 operator * (const _matrix44d& m, const _vector4& v)
{
    return _vector4(
        float(m.M11*v.x + m.M21*v.y + m.M31*v.z + m.M41*v.w),
        float(m.M12*v.x + m.M22*v.y + m.M32*v.z + m.M42*v.w),
        float(m.M13*v.x + m.M23*v.y + m.M33*v.z + m.M43*v.w),
        float(m.M14*v.x + m.M24*v.y + m.M34*v.z + m.M44*v.w));
};

//------------------------------------------------------------------------------
/**
*/
static 
inline 
_matrix44 tomatrix44 ( const _matrix44d& m )
{
    return _matrix44( (float)m.M11, (float)m.M12, (float)m.M13, (float)m.M14,
                      (float)m.M21, (float)m.M22, (float)m.M23, (float)m.M24,
                      (float)m.M31, (float)m.M32, (float)m.M33, (float)m.M34,
                      (float)m.M41, (float)m.M42, (float)m.M43, (float)m.M44);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
_matrix44d::get(_vector3& scale,quaternion& quat,_vector3& pos) const
{
    double sx, sy, sz;

    sx = sqrt( M11*M11 + M12*M12 + M13*M13);
    sy = sqrt( M21*M21 + M22*M22 + M23*M23);
    sz = sqrt( M31*M31 + M32*M32 + M33*M33);

    //only odd number scale negative take effect
    //cross pruduct x*y == z.. but its  precised
    // use the cosinus of angle

    if ( 0.0 > ( M31*(M12 * M23 - M13 * M22)+M32*(M13 * M21 - M11 * M23)+ M33*(M11 * M22 - M12 * M21) ) )
    {
        sz=-sz;
    }

    _matrix44d tmp;

    // tmp = (Scale.Inv)*(this)*(Pos.Inv)= (Scale.Inv)*(Scale*Quat*Pos)*(Pos.Inv)
    tmp.scale( 1.0 / sx , 1.0/sy, 1.0/sz);
    tmp=tmp*(*this);

    //translate - pos
    tmp.M41 = 0.0;
    tmp.M42 = 0.0;
    tmp.M43 = 0.0;

    scale = vector3( (float)sx, (float)sy, (float)sz);
    quat=tmp.get_quaternion();
    pos = vector3( (float)M41, (float)M42, (float)M43);
    
}

//------------------------------------------------------------------------------
/**
*/
inline
matrix33 
_matrix44d::getMatrix33()
{
    return matrix33 ( (float)M11, (float)M12, (float)M13,
                      (float)M21, (float)M22, (float)M23,
                      (float)M31, (float)M32, (float)M33);
}

//------------------------------------------------------------------------------
#endif
