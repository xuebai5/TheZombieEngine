#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navmeshnodeparser.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/navmeshnodeparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navparserfactory.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "nnavmeshparser/navpolygonparser.h"
#include "nnavmeshparser/navmeshnodelinksparser.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeParser::NavMeshNodeParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::MeshNode, factory), nodeIndex(-1), node(NULL)
{
    ADD_PARSE_VERSION(NavMeshNodeParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse a node, returning true if successfully parsed
*/
bool
NavMeshNodeParser::Parse(nNavMeshFile* file, nNavMesh* mesh, int nodeIndex)
{
    this->nodeIndex = nodeIndex;
    this->node = mesh->GetNode(nodeIndex);
    return NavBlockParser::Parse(file, mesh);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavMeshNodeParser::ParseVersion1()
{
    if ( !this->NodeIndex() )
    {
        return false;
    }
    if ( !this->Polygon() )
    {
        return false;
    }
    if ( !this->Links() )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the polygon of the navigation mesh node
*/
bool
NavMeshNodeParser::Polygon()
{
    n_assert(this->node);

    // Get the parser for a polygon
    NavPolygonParser* parser = static_cast<NavPolygonParser*>( this->parserFactory->CreateBlockParser(NavTag::Polygon) );
    if ( !parser )
    {
        return false;
    }

    // Parse the polygon
    bool success = parser->Parse(this->file, this->node->GetZone());
    this->parserFactory->DeleteBlockParser(parser);
    return success;
}

//------------------------------------------------------------------------------
/**
    Parse the links of the navigation mesh node
*/
bool
NavMeshNodeParser::Links()
{
    n_assert(this->node);

    // Get the parser for the links block
    NavMeshNodeLinksParser* parser = static_cast<NavMeshNodeLinksParser*>( this->parserFactory->CreateBlockParser(NavTag::MeshNodeLinks) );
    if ( !parser )
    {
        return false;
    }

    // Parse the links
    bool success = parser->Parse(this->file, this->mesh, this->node);
    this->parserFactory->DeleteBlockParser(parser);
    return success;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeLoader::NavMeshNodeLoader(NavParserFactory* factory)
    : NavMeshNodeParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
   Parse the node index of the navigation mesh node
*/
bool
NavMeshNodeLoader::NodeIndex()
{
    n_assert(this->file);

    int index;
    if ( !this->file->ParseInt16(NavTag::MeshNodeIndex, index) )
    {
        return false;
    }
    return index == this->nodeIndex;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeSaver::NavMeshNodeSaver(NavParserFactory* factory)
    : NavMeshNodeParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the node index of the navigation mesh node
*/
bool
NavMeshNodeSaver::NodeIndex()
{
    n_assert(this->file);

    return this->file->ParseInt16(NavTag::MeshNodeIndex, this->nodeIndex);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
