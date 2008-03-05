#ifndef N_GENARRAYVISITORS_H
#define N_GENARRAYVISITORS_H

// this file contains subclasses of the various visitors that take the found
// nSpatialElements and put them into an array container that is specified
// by the user.
/**
    @class nVisibleFrustumGenArray
    @ingroup Spatial
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "nvisiblefrustumvisitor.h"

class nVisibleFrustumGenArray : public nVisibleFrustumVisitor {
public:

    /// constructor
    nVisibleFrustumGenArray():
    nVisibleFrustumVisitor(), 
    m_visObjectsArray(NULL)
    {}

    ~nVisibleFrustumGenArray() {}

    void SetVisElementsArray(VisibleElements *visObjectsArray) 
    { 
        this->m_visObjectsArray = visObjectsArray; 
    }

    /*void Reset() 
    { 
        ClearObjectsArray(); 
        nVisibleFrustumVisitor::Reset(); 
    }
    */
    using nVisibleFrustumVisitor::Visit;

    void Visit(nSpatialElement *visitee, int recursedepth) 
    { 
        this->m_visObjectsArray->Append(visitee); 
    }

protected:
    
    void ClearObjectsArray() 
    { 
        //this->m_visObjectsArray->Clear(); 
        this->m_visObjectsArray->Reset(); 
    }
    
    // array of visible objects
    VisibleElements *m_visObjectsArray;

};


/**
    @class nOccludingFrustumGenArray
    @ingroup NSpatialContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

/*#include "nspatial/noccludingfrustumvisitor.h"

class nOccludingFrustumGenArray : public nOccludingFrustumVisitor {
public:
    nOccludingFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, VisibleElements &foundarray)
        : nOccludingFrustumVisitor(cam, camxform), m_visarray(foundarray) { }

    ~nOccludingFrustumGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};
*/

/**
    @class nVisibleSphereGenArray
    @ingroup NSpatialContribModule
    @brief Visits all elements visible in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

/*#include "nspatial/nvisiblespherevisitor.h"

class nVisibleSphereGenArray : public nVisibleSphereVisitor {
public:
    nVisibleSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray)
        : nVisibleSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nVisibleSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};
*/
/**
    @class nOccludingSphereGenArray
    @ingroup NSpatialContribModule
    @brief Visits all elements visible in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

/*#include "nspatial/noccludingspherevisitor.h"

class nOccludingSphereGenArray : public nOccludingSphereVisitor {
public:
    nOccludingSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray)
        : nOccludingSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nOccludingSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};
*/
/**
    @class nSpatialSphereGenArray
    @ingroup NSpatialContribModule
    @brief Visits all elements contained in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override spatialvisitor.  We
    just override Visit() to append the given element to the array
*/

/*#include "nspatial/nspatialspherevisitor.h"

class nSpatialSphereGenArray : public nSpatialSphereVisitor {
public:
    nSpatialSphereGenArray(const sphere &viewsphere, SpatialElements &foundarray)
        : nSpatialSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nSpatialSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    SpatialElements &m_visarray;
};
*/


#endif

