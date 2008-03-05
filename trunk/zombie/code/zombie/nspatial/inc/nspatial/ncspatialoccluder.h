#ifndef NC_SPATIALOCCLUDER_H
#define NC_SPATIALOCCLUDER_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialOccluder
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial component for occluders.

    (C) 2005 Conjurer Services, S.A.
*/

#include "nspatial/ncspatial.h"

//------------------------------------------------------------------------------
class ncSpatialOccluder : public ncSpatial
{

    NCOMPONENT_DECLARE(ncSpatialOccluder, ncSpatial);

public:

    /// constructor
    ncSpatialOccluder();
    /// destructor
    virtual ~ncSpatialOccluder();

    /// set an occluder's original vertex
    void SetVertex(int, const vector3&);
    /// set true if the occluder is double sided, false othewise
    void SetDoubleSided(bool);
    /// says if the occluder is double sided or not
    bool GetDoubleSided() const;
    /// change the active side
    void ChangeActiveSide();
    /// set if it flies
    void SetFly(bool fly);
    /// get if it flies
    bool GetFly() const;

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// update occluder's transformation 
    virtual void Update(const matrix44 &worldMatrix);

    /// update the current bounding box
    virtual void UpdateBBox(const matrix44 &worldMatrix);

    /// get the occluder's vertices
    const vector3 *GetVertices() const;

    /// get occluder's center
    const vector3 &GetCenter() const;

    /// set occluder's plane
    void SetPlane(const plane &p);

    /// get occluder's plane
    const plane &GetPlane() const;

    /// get occluder's area
    float GetArea() const;

    /// set the minimum active distance
    void SetMinDist(float dist);
    /// get the minimum active distance
    float GetMinDist() const;
    /// get the minimum active squared distance
    float GetMinSqDist() const;
    /// set the maximum active distance
    void SetMaxDist(float dist);
    /// get the maximum active distance
    float GetMaxDist() const;
    /// get the maximum active squared distance
    float GetMaxSqDist() const;

    /// save state of the component
    bool SaveCmds(nPersistServer *ps);

private:

    /// transformed vertices that form the occluder
    vector3 *vertices;

    /// original vertices that form the occluder
    vector3 *originalVertices;

    /// occluder's center
    vector3 center;

    /// occluder's plane
    plane p;

    /// occluder's area
    float area;

    /// says if the occluder is double sided or not
    bool doubleSided;

    /// max distance ( if the camera is farther than this distance, the occluder is deactivated)
    float maxDist;

    /// min distance ( if the camera is nearer than this distance, the occluder is deactivated)
    float minDist;
    
    /// true if the occluder is flying
    bool flies;
};

//------------------------------------------------------------------------------
/**
    set true if the occluder is double sided, false othewise
*/
inline
void 
ncSpatialOccluder::SetDoubleSided(bool flag)
{
    this->doubleSided = flag;
}

//------------------------------------------------------------------------------
/**
    says if the occluder is double sided or not
*/
inline
bool 
ncSpatialOccluder::GetDoubleSided() const
{
    return this->doubleSided;
}

//------------------------------------------------------------------------------
/**
    change the active side
*/
inline
void 
ncSpatialOccluder::ChangeActiveSide()
{
    // flip plane
    this->p.set(-this->p.a, -this->p.b, -this->p.c, -this->p.d);

    // flip original vertices
    vector3 t(this->originalVertices[0]);
    this->originalVertices[0] = this->originalVertices[1];
    this->originalVertices[1] = t;
    t = this->originalVertices[2];
    this->originalVertices[2] = this->originalVertices[3];
    this->originalVertices[3] = t;

    // flip current vertices
    t = this->vertices[0];
    this->vertices[0] = this->vertices[1];
    this->vertices[1] = t;
    t = this->vertices[2];
    this->vertices[2] = this->vertices[3];
    this->vertices[3] = t;
}

//------------------------------------------------------------------------------
/**
    set a occluder's original vertex
*/
inline
void 
ncSpatialOccluder::SetVertex(int numVertex, const vector3 &vertex)
{
    this->originalVertices[numVertex] = vertex;
}

//------------------------------------------------------------------------------
/**
    get the occluder's vertices
*/
inline
const vector3 *
ncSpatialOccluder::GetVertices() const
{
    return this->vertices;
}

//------------------------------------------------------------------------------
/**
    get occluder's center
*/
inline
const vector3 &
ncSpatialOccluder::GetCenter() const
{
    return this->center;
}

//------------------------------------------------------------------------------
/**
    set occluder's plane
*/
inline
void 
ncSpatialOccluder::SetPlane(const plane &p)
{
    this->p = p;
}

//------------------------------------------------------------------------------
/**
    get occluder's plane
*/
inline
const plane &
ncSpatialOccluder::GetPlane() const
{
    return this->p;
}

//------------------------------------------------------------------------------
/**
    get occluder's area
*/
inline
float 
ncSpatialOccluder::GetArea() const
{
    return this->area;
}

//------------------------------------------------------------------------------
/**
    set the minimum active distance
*/
inline
void 
ncSpatialOccluder::SetMinDist(float dist)
{
    if ( dist == -1 )
    {
        this->minDist = dist;
    }
    else
    {
        this->minDist = dist * dist;
    }
}

//------------------------------------------------------------------------------
/**
    get the minimum active distance
*/
inline
float 
ncSpatialOccluder::GetMinDist() const
{
    if ( this->minDist == -1 )
    {
        return this->minDist;
    }

    return n_sqrt(this->minDist);
}

//------------------------------------------------------------------------------
/**
    get the minimum active squared distance
*/
inline
float 
ncSpatialOccluder::GetMinSqDist() const
{
    return this->minDist;
}

//------------------------------------------------------------------------------
/**
    set the maximum active distance
*/
inline
void
ncSpatialOccluder::SetMaxDist(float dist)
{
    if ( dist == -1 )
    {
        this->maxDist = dist;
    }
    else
    {
        this->maxDist = dist * dist;
    }
}

//------------------------------------------------------------------------------
/**
    get the maximum active distance
*/
inline
float 
ncSpatialOccluder::GetMaxDist() const
{
    if ( this->maxDist == -1 )
    {
        return this->maxDist;
    }

    return n_sqrt(this->maxDist);
}

//------------------------------------------------------------------------------
/**
    get the maximum active squared distance
*/
inline
float 
ncSpatialOccluder::GetMaxSqDist() const
{
    return this->maxDist;
}

//------------------------------------------------------------------------------
/**
    set if it flies
*/
inline
void 
ncSpatialOccluder::SetFly(bool fly)
{
    this->flies = fly;
}

//------------------------------------------------------------------------------
/**
    get if it flies
*/
inline
bool 
ncSpatialOccluder::GetFly() const
{
    return this->flies;
}

//------------------------------------------------------------------------------
#endif // NC_SPATIALOCCLUDER_H
