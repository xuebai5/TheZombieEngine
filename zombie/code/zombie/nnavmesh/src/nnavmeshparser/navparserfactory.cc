#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navparserfactory.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nnavmeshparser/navparserfactory.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navrootparser.h"
#include "nnavmeshparser/navmeshparser.h"
#include "nnavmeshparser/navmeshnodeparser.h"
#include "nnavmeshparser/navmeshnodelinksparser.h"
#include "nnavmeshparser/navpolygonparser.h"
#include "nnavmeshparser/navgraphparser.h"
#include "nnavmeshparser/navobstaclesparser.h"

//------------------------------------------------------------------------------
/**
    Create a navigation block parser

    Those blocks that require to load or save are created by its dedicated
    factory. This factory only create those blocks that are generic enough
    to not need a specific loader or saver.
*/
NavBlockParser* NavParserFactory::CreateBlockParser(NavTag blockId)
{
    switch ( blockId )
    {
        case NavTag::Root:
            return n_new(NavRootParser)(this);
        default:
            n_assert2_always("Parser not defined for this block id");
            return NULL;
    }
}

//------------------------------------------------------------------------------
/**
    Delete a navigation block parser

    Call this instead of deleting the parser yourself, so any future changes in
    the way parsers are created/destroyed will affect only to factories.
*/
void
NavParserFactory::DeleteBlockParser(NavBlockParser* parser)
{
    n_delete( parser );
}

//------------------------------------------------------------------------------
/**
    Create a navigation block loader

    Not all blocks have loaders, some only require a generic parser since no
    load/save operation is required. For this blocks the generic parser is
    returned instead of a specific loader.
*/
NavBlockParser* NavLoaderFactory::CreateBlockParser(NavTag blockId)
{
    switch ( blockId )
    {
        case NavTag::Mesh:
            return n_new(NavMeshLoader)(this);
        case NavTag::MeshNode:
            return n_new(NavMeshNodeLoader)(this);
        case NavTag::MeshNodeLinks:
            return n_new(NavMeshNodeLinksLoader)(this);
        case NavTag::Polygon:
            return n_new(NavPolygonLoader)(this);
        case NavTag::Graph:
            return n_new(NavGraphLoader)(this);
        case NavTag::Obstacles:
            return n_new(NavObstaclesLoader)(this);
        default:
            return NavParserFactory::CreateBlockParser(blockId);
    }
}

//------------------------------------------------------------------------------
/**
    Create a navigation block saver

    Not all blocks have savers, some only require a generic parser since no
    load/save operation is required. For this blocks the generic parser is
    returned instead of a specific saver.
*/
NavBlockParser* NavSaverFactory::CreateBlockParser(NavTag blockId)
{
    switch ( blockId )
    {
        case NavTag::Mesh:
            return n_new(NavMeshSaver)(this);
        case NavTag::MeshNode:
            return n_new(NavMeshNodeSaver)(this);
        case NavTag::MeshNodeLinks:
            return n_new(NavMeshNodeLinksSaver)(this);
        case NavTag::Polygon:
            return n_new(NavPolygonSaver)(this);
        case NavTag::Graph:
            return n_new(NavGraphSaver)(this);
        case NavTag::Obstacles:
            return n_new(NavObstaclesSaver)(this);
        default:
            return NavParserFactory::CreateBlockParser(blockId);
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
