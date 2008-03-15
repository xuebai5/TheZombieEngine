//------------------------------------------------------------------------------
//  npolygoncontainer.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "util/npolygoncontainer.h"
#include "mathlib/polygon.h"

//------------------------------------------------------------------------------
/**
    default constructor
*/
nPolygonContainer::nPolygonContainer()
{
    this->m_polygonsList = n_new(nArray<polygon*>(256, 64));
}

//------------------------------------------------------------------------------
/**
    constructor with a given initial and growing size
*/
nPolygonContainer::nPolygonContainer(int initialSize, int growSize)
{
    this->m_polygonsList = n_new(nArray<polygon*>(initialSize, growSize));
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nPolygonContainer::~nPolygonContainer()
{
    n_delete(this->m_polygonsList);
}

//------------------------------------------------------------------------------
/**
    get first polygon
*/
polygon *
nPolygonContainer::GetHead() const
{
    return this->m_polygonsList->Front();
}

//------------------------------------------------------------------------------
/**
    get last polygon
*/
polygon *
nPolygonContainer::GetTail() const
{
    return this->m_polygonsList->Back();
}

//------------------------------------------------------------------------------
/**
    add polygon to the container
*/
void 
nPolygonContainer::Add(polygon *poly)
{
    this->m_polygonsList->Append(poly);
}

//------------------------------------------------------------------------------
/**
    find the polygon containing the given point, if exists
*/
polygon *
nPolygonContainer::FindPolygonContaining(const vector3 &point) const
{
    for (nArray<polygon*>::iterator currPoly  = this->m_polygonsList->Begin();
                                    currPoly != this->m_polygonsList->End();
                                    currPoly++)
    {
        if ((*currPoly)->IsPointInside(point))
        {
            // the current polygon contains the point. Return it.
            return (*currPoly);
        }
    }

    // there isn't a polygon in this container containing the point
    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nPolygonContainer::Contains(polygon *poly)
{
    return (this->m_polygonsList->FindIndex(poly) > -1);
}
