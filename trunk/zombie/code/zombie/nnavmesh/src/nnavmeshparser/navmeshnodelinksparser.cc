#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navmeshnodelinksparser.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/navmeshnodelinksparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeLinksParser::NavMeshNodeLinksParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::MeshNodeLinks, factory), node(NULL)
{
    ADD_PARSE_VERSION(NavMeshNodeLinksParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse node links, returning true if successfully parsed
*/
bool
NavMeshNodeLinksParser::Parse(nNavMeshFile* file, nNavMesh* mesh, ncNavMeshNode* node)
{
    this->node = node;
    return NavBlockParser::Parse(file, mesh);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavMeshNodeLinksParser::ParseVersion1()
{
    int num_links;
    if ( !this->LinksNb(num_links) )
    {
        return false;
    }
    if ( !this->LinksList(num_links) )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the links list of a navigation mesh node
*/
bool
NavMeshNodeLinksParser::LinksList(int num_links)
{
    n_assert(this->node);

    for ( int i = 0; i < num_links; ++i )
    {
        if ( !this->Link(i) )
        {
            return false;
        }
    }
    return this->node->GetLocalLinksNumber() == num_links;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeLinksLoader::NavMeshNodeLinksLoader(NavParserFactory* factory)
    : NavMeshNodeLinksParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of links of a navigation mesh node
*/
bool
NavMeshNodeLinksLoader::LinksNb(int& num_links)
{
    n_assert(this->file);
    n_assert(this->node);

    return this->file->ParseInt8(NavTag::MeshNodeLinksNb, num_links);
}

//------------------------------------------------------------------------------
/**
    Parse a link of a navigation mesh node
*/
bool
NavMeshNodeLinksLoader::Link(int /*link_index*/)
{
    n_assert(this->file);
    n_assert(this->node);

    // Append a link to a node
    int node_index;
    if ( !this->file->ParseInt16(NavTag::MeshNodeLink, node_index) )
    {
        return false;
    }
    this->node->AddLocalLink( this->mesh->GetNode(node_index) );
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshNodeLinksSaver::NavMeshNodeLinksSaver(NavParserFactory* factory)
    : NavMeshNodeLinksParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of links of a navigation mesh node
*/
bool
NavMeshNodeLinksSaver::LinksNb(int& num_links)
{
    n_assert(this->file);
    n_assert(this->node);

    num_links = node->GetLocalLinksNumber();
    return this->file->ParseInt8(NavTag::MeshNodeLinksNb, num_links);
}

//------------------------------------------------------------------------------
/**
    Parse a link of a navigation mesh node
*/
bool
NavMeshNodeLinksSaver::Link(int link_index)
{
    n_assert(this->file);
    n_assert(this->node);

    int node_index = this->mesh->GetIndexNode( static_cast<ncNavMeshNode*>( this->node->GetLocalLink(link_index) ) );
    return this->file->ParseInt16(NavTag::MeshNodeLink, node_index);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
