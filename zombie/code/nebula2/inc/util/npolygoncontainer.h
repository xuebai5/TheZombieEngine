#ifndef N_POLYGONCONTAINER_H
#define N_POLYGONCONTAINER_H
//------------------------------------------------------------------------------
/**
    @class nPolygonContainer
    @ingroup NebulaDataTypes

    @brief A polygons container. 
    
    (C) 2005 Conjurer Services, S.A.

    Author: Miquel Angel Rujula
*/

#include "mathlib/vector.h"
#include "util/narray.h"

class polygon;

//------------------------------------------------------------------------------
class nPolygonContainer
{
public:

    /// default constructor
    nPolygonContainer();
    /// constructor with a given initial and growing size
    nPolygonContainer(int initialSize, int growSize);
    /// destructor
    ~nPolygonContainer();

    /// get first polygon
    polygon *GetHead() const;
    /// get last polygon
    polygon *GetTail() const;
    /// add polygon to the container
    void Add(polygon *poly);
    /// find the polygon containing the given point, if exists
    polygon *FindPolygonContaining(const vector3 &point) const;
    /// determine if it contains the given polygon
    bool Contains(polygon *poly);

private:

    // polygons
    nArray<polygon*> *m_polygonsList;
};

#endif

