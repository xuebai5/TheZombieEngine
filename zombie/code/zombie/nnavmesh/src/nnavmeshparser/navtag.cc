#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navtag.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nnavmeshparser/navtag.h"

//------------------------------------------------------------------------------
/**
    Return the string representation of some tag
*/
const nString&
NavTag::GetString(const NavTag& tag)
{
    static nString strTags[NumTags];

    // Init all strings to empty string in debug...
#ifndef NDEBUG
    for ( int i = 0; i < NumTags; ++i )
    {
        strTags[i] = "";
    }
#endif

    // Use only letters, numbers and underline. Case matters.
    // Don't change the representation (read the comment about changing NavTagId,
    // for string representations applies the same rule).
    strTags[Root] = "navigation";
    strTags[FileFormatType] = "type";
    strTags[Version] = "version";
    strTags[Mesh] = "navigation_mesh";
    strTags[MeshNodesNb] = "num_nodes";
    strTags[MeshNode] = "mesh_node";
    strTags[MeshNodeIndex] = "index";
    strTags[MeshNodeLinks] = "links";
    strTags[MeshNodeLinksNb] = "num_links";
    strTags[MeshNodeLink] = "link";
    strTags[Polygon] = "polygon";
    strTags[PolygonVerticesNb] = "num_vertices";
    strTags[PolygonVertex] = "vertex";
    strTags[Graph] = "navigation_graph";
    strTags[GraphNodesNb] = "num_nodes";
    strTags[GraphNode] = "graph_node";
    strTags[Obstacles] = "obstacles";
    strTags[ObstaclesNb] = "num_obstacles";

    // ...to check if for some tag is missing its string representation.
#ifndef NDEBUG
    for ( int i = 0; i < NumTags; ++i )
    {
        n_assert( strTags[i] != "" );
    }
#endif

    return strTags[tag.tagId];
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
