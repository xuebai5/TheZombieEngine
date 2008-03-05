#ifndef N_NAVTAG_H
#define N_NAVTAG_H

//------------------------------------------------------------------------------
/**
    @class NavTag
    @ingroup NebulaNavmeshSystem

    Store a tag/id of navigation block or field
    
    It store a navigation tag, but also knows all available tags and its string
    equivalences.
*/

#include "util/nstring.h"

//------------------------------------------------------------------------------
class NavTag
{
public:
    /**
        Ids of all navigation blocks/fields

        Don't change the order of tags (that is, their values), since they are
        already stored in navigation files. If for any crazy reason you must
        change their order/values, first go to sleep. If at the next morning you
        still have that desperate need then you will need to update also all tag
        values in all navigation files for all those tags that you modify
        (or tell to the artists/designers to throw away all the old navigation
        files :).
    */
    enum NavTagId
    {
        Root = 0,
        FileFormatType,
        Version,
        Mesh,
        MeshNodesNb,
        MeshNode,
        MeshNodeIndex,
        MeshNodeLinks,
        MeshNodeLinksNb,
        MeshNodeLink,
        Polygon,
        PolygonVerticesNb,
        PolygonVertex,
        Graph,
        GraphNodesNb,
        GraphNode,
        Obstacles,
        ObstaclesNb,

        NumTags
    };

    /// Constructor
    NavTag(NavTagId tagId)
        : tagId(tagId)
    {
        // Empty
    }

    /// Copy constructor
    NavTag(const NavTag& tag)
        : tagId(tag.tagId)
    {
        // Empty
    }

    /// Constructor from int
    NavTag(int tagValue)
        : tagId( static_cast<NavTagId>(tagValue) )
    {
        // Empty
    }

    /// Equality operator
    bool operator==(NavTag& tag) const
    {
        return this->tagId == tag.tagId;
    }

    /// Implicit cast to int
    operator int() const
    {
        return this->tagId;
    }

    /// Return the string representation of this tag
    const nString& AsString() const
    {
        return this->GetString(*this);
    }

    /// Return the string representation of some tag
    static const nString& GetString(const NavTag& tag);

private:
    NavTagId tagId;
};

//------------------------------------------------------------------------------
#endif
