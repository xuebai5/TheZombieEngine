#ifndef N_SPATIALELEMENTS_H
#define N_SPATIALELEMENTS_H

//#include "nspatial/nspatialspace.h"
#include "mathlib/sphere.h"
#include "tinyxml/tinyxml.h"
#include "gfx2/ncamera2.h"
//#include "nscene/nrendercontext.h"
#include "nspatial/ncspatialspace.h"

//class nVisibilityVisitor;
class nVisibleFrustumVisitor;
class nSpatialVisitor;
class nSpatialCell;
//class nTransformNode;
class nEntityObject;

//--------------------------------------------------------------------
/**
    @class nSpatialModel
    @ingroup NebulaSpatialSystem
    @brief The interface for models.

    A spatial model is a shape used to perform visibility tests.

    (C) 2006 Conjurer Services, S.A.
*/
class nSpatialModel
{
public:

    enum Model_Type
    {
        SPATIAL_MODEL_SPHERE,
        SPATIAL_MODEL_FRUSTUM
    };


    /// constructor
    nSpatialModel(){}
    /// destructor
    virtual ~nSpatialModel(){}

    /// get model type
    Model_Type GetType() const;
    /// check if a bounding box intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const sphere &sph);

protected:

    Model_Type m_type;
};

//------------------------------------------------------------------------------
/**
    get model type
*/
inline
nSpatialModel::Model_Type
nSpatialModel::GetType() const
{
    return this->m_type;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Intersects(const bbox3 &/*box*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Intersects(const sphere &/*sph*/)
{
    return false;
}


//--------------------------------------------------------------------
/**
    @class nSpatialSphereModel
    @ingroup NebulaSpatialSystem
    @brief Represents a model which shape is a sphere.

    A sphere model is a sphere used to perform visibility tests.
*/
class nSpatialSphereModel : public nSpatialModel
{
public:
    /// constructor 1
    nSpatialSphereModel ();
    /// constructor 2
    nSpatialSphereModel (const sphere &sph);
    /// constructor 3
    nSpatialSphereModel(const vector3 &center, const float radius);
    /// destructor
    virtual ~nSpatialSphereModel(){}

    /// set the sphere center and radius
    void Set(const vector3& center, float radius);
    /// set the sphere's position (center)
    void Set(const vector3& center);
    /// get the sphere
    sphere GetSphere();
    
    /// check if a given bounding box intersects with the sphere
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const sphere &sph);

private:

    sphere m_sphere;

};

//------------------------------------------------------------------------------
/**
    constructor 1
*/
inline
nSpatialSphereModel::nSpatialSphereModel()
{
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
inline
nSpatialSphereModel::nSpatialSphereModel(const sphere &sph) : 
m_sphere(sph)
{
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
    constructor 3
*/
inline
nSpatialSphereModel::nSpatialSphereModel(const vector3 &center, const float radius)
{
    this->m_sphere = sphere(center, radius);
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nSpatialSphereModel::Set(const vector3& center, float radius)
{
    this->m_sphere.set(center, radius);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nSpatialSphereModel::Set(const vector3& center)
{
    this->m_sphere.set(center);
};

//------------------------------------------------------------------------------
/**
    get the sphere
*/
inline
sphere 
nSpatialSphereModel::GetSphere()
{
    return this->m_sphere;
}

//------------------------------------------------------------------------------
/**
    check if a given bounding box intersects with the sphere
*/
inline
bool 
nSpatialSphereModel::Intersects(const bbox3 &box)
{
    return this->m_sphere.intersects(box);
}

//------------------------------------------------------------------------------
/**
    check if a given sphere intersects with the sphere
*/
inline
bool 
nSpatialSphereModel::Intersects(const sphere &sph)
{
    return this->m_sphere.intersects(sph);
}

//--------------------------------------------------------------------
/**
    @class nSpatialFrustumModel
    @ingroup NebulaSpatialSystem
    @brief Represents a model which shape is a frustum.

    A frustum model is a frustum used to perform visibility tests.
*/
class nSpatialFrustumModel : public nSpatialModel
{
public:
    /// constructor 1
    nSpatialFrustumModel ();
    /// constructor 2
    nSpatialFrustumModel (nCamera2 &frustum, matrix44 &transformMatrix);
    /// destructor
    virtual ~nSpatialFrustumModel(){}
    
    // set the transformation matrix 
    void SetTransformMatrix(const matrix44 &transformMatrix);
    // set the frustum
    void SetFrustum(const nCamera2 &frustum);
    // get the frustum
    nCamera2 *GetFrustum();
    
    /// check if a given bounding box intersects with the frustum
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const sphere &sph);

private:

    /// view matrix
    matrix44 m_transformMatrix;
    /// projection matrix
    nCamera2 m_frustum;
};

//------------------------------------------------------------------------------
/**
    set the transformation matrix 
*/
inline
void 
nSpatialFrustumModel::SetTransformMatrix(const matrix44 &transformMatrix)
{
//    n_assert2(transformMatrix, "miquelangel.rujula");

    this->m_transformMatrix = transformMatrix;
}

//------------------------------------------------------------------------------
/**
    set the frustum
*/
inline
void 
nSpatialFrustumModel::SetFrustum(const nCamera2 &frustum)
{
    this->m_frustum = frustum;
}

//------------------------------------------------------------------------------
/**
    get the frustum
*/
inline
nCamera2 * 
nSpatialFrustumModel::GetFrustum()
{
    return &this->m_frustum;
}

//------------------------------------------------------------------------------
/**
    constructor 1
*/
inline
nSpatialFrustumModel::nSpatialFrustumModel()
{
    this->m_type = SPATIAL_MODEL_FRUSTUM;
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
inline
nSpatialFrustumModel::nSpatialFrustumModel(nCamera2 &frustum, matrix44 &transformMatrix) : 
m_frustum(frustum),
m_transformMatrix(transformMatrix)
{
    this->m_type = SPATIAL_MODEL_FRUSTUM;
}


//--------------------------------------------------------------------
/**
    @class nSpatialBasicElement
    @ingroup NSpatialContribModule
    @brief The minimum interface for elements contained within a cell.

    A spatial basic element represents some sort of object within a cell.
*/
//class nSpatialBasicElement 
//{
//
//public:
//
//    nSpatialBasicElement();
//    virtual ~nSpatialBasicElement();
//
//    /// get element's axis aligned bounding box
//    bbox3 GetBBox();
//    /// set element's axis aligned bounding box
//    void SetBBox(bbox3 &bbox);
//
//protected:
//
//    /// spatial element's bounding box (in space coordinates)
//    bbox3 m_bbox;
//
//}

//------------------------------------------------------------------------------
/**
    get element's axis aligned bounding box
*/
//inline
//bbox3 
//nSpatialBasicElement::GetBBox()
//{
//    return this->m_bbox;
//}

//------------------------------------------------------------------------------
/**
    set element's axis aligned bounding box
*/
//inline
//void 
//nSpatialBasicElement::SetBBox(bbox3 &bbox)
//{
//    this->m_bbox = bbox;
//}

//--------------------------------------------------------------------
/**
    @class nSpatialElement
    @ingroup NSpatialContribModule
    @brief The interface for elements contained within a cell.

    A spatial element represents some sort of object within a cell. This
    could be a simple object, a portal, or something else entirely.
*/
class nSpatialElement 
{

public:

    enum {
        // extend with your own element types in a subclass
        N_SPATIAL_ELEMENT,  /// common spatial element
        N_SPATIAL_PORTAL ,  /// points into another cell
        N_SPATIAL_LIGHT  ,  /// it's a light
        N_SPATIAL_CAMERA    /// it's a camera
    };
    typedef int nSpatialElementType;

    // <LOD>
    enum {
        N_FAR_LOD = 5000,
        N_NEAR_LOD = -1
    };

    typedef struct 
    {
        float begin;
        float end;

        // determine if a distance 'd' is in the range
        bool Contains(float d)
        {
            if ( (d >= begin) && (d <= end) )
            {
                return true;
            }

            return false;
        }

        // determine if a distance 'd' is behind the range
        bool IsBehind(float d)
        {
            return d > end;
        }

        // determine if a distance 'd' is in front of the range
        bool IsInFront(float d)
        {
            return d < begin;
        }

    } nLODRangeType;

    typedef struct 
    {
        nArray<nLODRangeType> ranges;
        int actualLOD;
    } nLODInfo;

    nLODInfo LODInfo;
    // </LOD>

    nSpatialElement();
    virtual ~nSpatialElement();

    /// get element's axis aligned bounding box
    bbox3 GetBBox();
    /// set element's axis aligned bounding box
    void SetBBox(const bbox3 &bbox);
    ///// recalculate the bounding box from the actual render context's root node
    //virtual void RecalculateBBox();
    /// get the cell the element is in
    nSpatialCell *GetCell() const;
    /// set the cell the element is in
    void SetCell(nSpatialCell *cell);
    ///// get render context
    //nRenderContext *GetRenderContext();
    ///// set render context
    //void SetRenderContext(nRenderContext *rc);
    ///// set the render context's root node
    //void SetRootNode(nSceneNode *rootNode);
    ///// set the render context's root node and get its bounding box
    //void SetRootNodeAndBox(nSceneNode *rootNode);
    ///// get the render context's root node
    //nSceneNode *GetRootNode();
    /// set the aux pointer
    void SetAuxPtr(void *p);
    /// get the aux pointer
    void *GetAuxPtr() const;
    /// set user data pointer
    //void SetUsrPtr(void *p);
    /// get user data pointer
    //void *GetUsrPtr() const;
    /// get type
    nSpatialElementType GetType() const;
    /// set spatial element's position
    void SetPosition(const vector3& point);
    /// get spatial element's position
    vector3 GetPosition();
    /// get spatial element's world position
    vector3 GetWorldPosition();
    /// get the layer id where the element is on
    int GetLayer();
    /// set the layer id where the element is on
    void SetLayer(int id);
    /// set dynamic flag
    void SetDynamic(bool flag);
    /// get the dynamic flag
    bool IsDynamic();
    /// catches the information corresponding to an spatial element contained in a TiXmlElement
    virtual bool ReadXmlElement(const TiXmlElement *spatialElementElem);
    /// fills a TiXmlElement with the spatial element's information
    virtual bool FillXmlElement(TiXmlElement *spatialElementElem);


    /// get test model
    nSpatialModel *GetTestModel();
    /// set test model
    void SetTestModel(nSpatialModel *testModel);
    /// copy the necessary information from another element
    void operator=(const nSpatialElement &elm);

    /// get write model (used to perform occlusion tests)
    /*nSpatialModel *GetWriteModel();
    /// set write model (used to perform occlusion tests)
    void SetWriteModel(nSpatialModel *writeModel);
    */
    

    //virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth);
    virtual void Accept(nVisibleFrustumVisitor &visitor, int recursiondepth);
    //virtual void Accept(nSpatialVisitor &visitor, int recursiondepth);


    /// get entity
    nEntityObject *GetEntity();
    /// set entity
    void SetEntity(nEntityObject *entity);
    
protected:

    /// spatial element's type
    nSpatialElementType m_elementType;
    /// transformation matrix in space coordinates
    //matrix44 m_transformMatrix;
    /// spatial element's position (used to situate the element in the space and cell)
    vector3 m_position;
    ///// spatial element's render context
    //nRenderContext *m_renderContext;
    /// spatial element's bounding box (in space coordinates)
    bbox3 m_bbox;
    /// cell where this element is in
    nSpatialCell  *m_cell;
    /// layer id where this element is on
    int m_layerId;
    /// dynamic element flag
    bool m_dynamic;
    /// test model
    nSpatialModel *m_testModel;
    /// aux pointer used only by the spatial module
    void *m_auxPtr;
    /// user data pointer
    void *m_usrPtr;


    //nSpatialModel *m_writeModel; Maybe used in future

    /// determine the given subtree's bounding box
//    virtual void CalculateBBox(nSceneNode *node, bbox3 &box);



    nEntityObject *m_entity;
    
};


//------------------------------------------------------------------------------
/**
    get render context
*/
//inline
//nRenderContext *
//nSpatialElement::GetRenderContext()
//{
//    return this->m_renderContext;
//}

//------------------------------------------------------------------------------
/**
    set render context
*/
//inline
//void 
//nSpatialElement::SetRenderContext(nRenderContext *rc)
//{
//    *this->m_renderContext = *rc;
//}

//------------------------------------------------------------------------------
/**
    get the render context's root node
*/
//inline
//nSceneNode *
//nSpatialElement::GetRootNode()
//{
//    return this->m_renderContext->IsValid() ? this->m_renderContext->GetRootNode() : NULL;
//}

//------------------------------------------------------------------------------
/**
    get element's axis aligned bounding box
*/
inline
bbox3 
nSpatialElement::GetBBox()
{
    return this->m_bbox;
}

//------------------------------------------------------------------------------
/**
    set element's axis aligned bounding box
*/
inline
void 
nSpatialElement::SetBBox(const bbox3 &bbox)
{
    this->m_bbox = bbox;
}

//------------------------------------------------------------------------------
/**
    get test model
*/
inline
nSpatialModel*
nSpatialElement::GetTestModel()
{
    return this->m_testModel;
}

//------------------------------------------------------------------------------
/**
    set test model
*/
inline
void
nSpatialElement::SetTestModel(nSpatialModel *testModel)
{
    this->m_testModel = testModel;
}

//------------------------------------------------------------------------------
/**
    get write model
*/
/*
inline
nSpatialModel*
nSpatialElement::GetWriteModel()
{
    return this->m_writeModel;
}
*/
//------------------------------------------------------------------------------
/**
    set write model
*/
/*
inline
void
nSpatialElement::SetWriteModel(nSpatialModel *writeModel)
{
    this->m_writeModel = writeModel;
}
*/

//------------------------------------------------------------------------------
/**
    get the cell the element is in
*/
inline
nSpatialCell*
nSpatialElement::GetCell() const
{
    return this->m_cell;
}

//------------------------------------------------------------------------------
/**
    set the cell the element is in
*/
inline
void
nSpatialElement::SetCell(nSpatialCell *cell)
{
    this->m_cell = cell;
}

//-----------------------------------------------------------------------------
/**
    @param p  the new aux data pointer
*/
inline
void 
nSpatialElement::SetAuxPtr(void *p)
{
    this->m_auxPtr = p;
}

//-----------------------------------------------------------------------------
/**
    @return the aux data pointer
*/
inline
void *
nSpatialElement::GetAuxPtr() const
{
    return this->m_auxPtr;
}

//-----------------------------------------------------------------------------
/**
    @param p  the new user data pointer
*/
//inline
//void 
//nSpatialElement::SetUsrPtr(void *p)
//{
//    this->m_usrPtr = p;
//}

//-----------------------------------------------------------------------------
/**
    @return the user data pointer
*/
//inline
//void *
//nSpatialElement::GetUsrPtr() const
//{
//    return this->m_usrPtr;
//}

//------------------------------------------------------------------------------
/**
*/
inline
nSpatialElement::nSpatialElementType 
nSpatialElement::GetType() const
{
    return this->m_elementType;
}

//------------------------------------------------------------------------------
/**
    set spatial element's position
*/
inline
void 
nSpatialElement::SetPosition(const vector3& point)
{
    this->m_position = point;
}

//------------------------------------------------------------------------------
/**
    get spatial element's position
*/
inline
vector3 
nSpatialElement::GetPosition()
{
    return this->m_position;
}

//------------------------------------------------------------------------------
/**
    get spatial element's world position
*/
inline
vector3 
nSpatialElement::GetWorldPosition()
{
    //nSpatialSpace *parentSpace = this->m_cell->GetParentSpace();
    ncSpatialSpace *parentSpace = this->m_cell->GetParentSpace();
    return  (parentSpace->GetTransformMatrix() * this->m_position);
}

//------------------------------------------------------------------------------
/**
    get the layer id where the element is on
*/
inline
int 
nSpatialElement::GetLayer()
{
    return this->m_layerId;
}

//------------------------------------------------------------------------------
/**
    set the layer id where the element is on
*/
inline
void
nSpatialElement::SetLayer(int id)
{
    this->m_layerId = id;
}

//------------------------------------------------------------------------------
/**
    set dynamic flag
*/
inline
void 
nSpatialElement::SetDynamic(bool flag)
{
    this->m_dynamic = flag;
}

//------------------------------------------------------------------------------
/**
    get the dynamic flag
*/
inline
bool 
nSpatialElement::IsDynamic()
{
    return this->m_dynamic;
}

//------------------------------------------------------------------------------
/**
    copy the necessary information from another element
*/
inline
void 
nSpatialElement::operator=(const nSpatialElement &elm)
{
    this->m_elementType = elm.m_elementType;
    this->m_position = elm.m_position;
    this->m_bbox = elm.m_bbox;
    this->m_cell = elm.m_cell;
    this->m_auxPtr = elm.m_auxPtr;
    this->m_usrPtr = elm.m_usrPtr;
    this->m_testModel = elm.m_testModel;
//    (*this->m_renderContext) = (*elm.m_renderContext);
}

//------------------------------------------------------------------------------
/**
    get entity
*/
inline
nEntityObject *
nSpatialElement::GetEntity()
{
    return this->m_entity;
}

//------------------------------------------------------------------------------
/**
    set entity
*/
inline
void 
nSpatialElement::SetEntity(nEntityObject *entity)
{
    this->m_entity = entity;
}

#endif

