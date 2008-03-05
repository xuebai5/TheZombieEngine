#ifndef N_OBBOX_H
#define N_OBBOX_H 

//------------------------------------------------------------------------------
/**
    @class obbox3
    @ingroup NebulaMathDataTypes

    (oriented) bounding box
*/
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
//  obbox3
//------------------------------------------------------------------------------
class obbox3 
{
public:
    /// default constructor
    obbox3();
    /// default destructor
    ~obbox3();
    /// Set Transform
    void SetTransform( const transform44& tr);
    /// Get Transform
    transform44 GetTransform() const;
    /// Set box Size in local
    void SetSize(const vector3& size);
    /// Get the boxSize
    vector3 GetSize() const;
    /// Set scale transformation;
    void SetScale(const vector3& scale);
    /// Get scale tranformation;
    vector3 GetScale() const;
    /// Set scale transformation;
    void SetEuler(const vector3& rot);
    /// Get scale tranformation;
    vector3 GetEuler() const;
        /// Set scale transformation;
    void SetTranslation(const vector3& tras);
    /// Get scale tranformation;
    vector3 GetTranslation() const;
    /// extends the world point in local box
    void extents(const vector3& point);
    /// extends the world point in local box
    void extentsInLocal(const vector3& localPoint);
    /// return if cotains a world point
    bool contains(const vector3& point);

protected:
    void updateTransform();
    void SetHalfSize(const vector3& half);
private:
    transform44 transform;
    matrix44 worldToLocal;
    vector3 halfSize;
    vector3 invHalfSize;
    bool dirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
obbox3::obbox3() : dirty(false)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
obbox3::~obbox3()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetTransform( const transform44& tr)
{
    this->dirty = true;
    transform = tr;
}

//------------------------------------------------------------------------------
/**
*/
inline
transform44 
obbox3::GetTransform() const
{
    return transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetHalfSize(const vector3& half)
{
    halfSize = half;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetSize(const vector3& size)
{
    vector3 tmp( size * 0.5f );
    tmp.abs();
    SetHalfSize(tmp);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 
obbox3::GetSize() const
{
    return halfSize * 2.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetScale(const vector3& scale)
{
    this->dirty = true;
    transform.setscale(scale);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 
obbox3::GetScale() const
{
    return transform.getscale();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetEuler(const vector3& rot)
{
    this->dirty = true;
    transform.seteulerrotation( rot );
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 
obbox3::GetEuler() const
{
    return transform.geteulerrotation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::SetTranslation(const vector3& tras)
{
    this->dirty = true;
    transform.settranslation( tras );
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 
obbox3::GetTranslation() const
{
    return transform.gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
obbox3::extentsInLocal(const vector3& local)
{
    vector3 localPoint(local);
    localPoint.abs();
    vector3 ext( n_max( localPoint.x , halfSize.x ) , n_max( localPoint.y , halfSize.y ), n_max( localPoint.z , halfSize.z ) );
    this->SetHalfSize( ext );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::extents(const vector3& point)
{
    updateTransform();
    extentsInLocal( worldToLocal * point );
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
obbox3::contains(const vector3& point)
{
    updateTransform();
    vector3 local =  ( worldToLocal * point );

    return ( -halfSize.x < local.x && local.x < halfSize.x  &&
             -halfSize.y < local.y && local.y < halfSize.y  &&
             -halfSize.z < local.z && local.z < halfSize.z 
           );
    
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
obbox3::updateTransform()
{
    if ( dirty)
    {
        worldToLocal = transform.getmatrix();
        worldToLocal.invert();
    }
}

#endif