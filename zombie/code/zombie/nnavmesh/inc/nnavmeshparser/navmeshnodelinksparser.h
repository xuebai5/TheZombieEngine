#ifndef N_NAVMESHNODELINKSPARSER_H
#define N_NAVMESHNODELINKSPARSER_H

//------------------------------------------------------------------------------
/**
    @file navmeshnodelinksparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for the links block of a navigation mesh node.

    Block file format:
    @verbatim
    --------------------------------------------------------
    links:
        version 1
        num_links <int>
        link <node index (int)>
        ...
        link <node index (int)>
    --------------------------------------------------------
    @endverbatim
*/

#include "nnavmeshparser/navblockparser.h"

class ncNavMeshNode;

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeParser

    Class to parse the links block of a navigation mesh node.
*/
class NavMeshNodeLinksParser : public NavBlockParser
{
public:
    /// Constructor
    NavMeshNodeLinksParser(NavParserFactory* factory);

    /// Parse node links, returning true if successfully parsed
    virtual bool Parse(nNavMeshFile* file, nNavMesh* mesh, ncNavMeshNode* node);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the number of links of a navigation mesh node
    virtual bool LinksNb(int& num_links) = 0;
    /// Parse the links list of a navigation mesh node
    virtual bool LinksList(int num_links);
    /// Parse a link of a navigation mesh node
    virtual bool Link(int link_index) = 0;

    /// Node owner of the links block being parsed
    ncNavMeshNode* node;
};

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeLinksLoader

    Class to load the links block of a navigation mesh node.
*/
class NavMeshNodeLinksLoader : public NavMeshNodeLinksParser
{
public:
    /// Constructor
    NavMeshNodeLinksLoader(NavParserFactory* factory);

protected:
    /// Parse the number of links of a navigation mesh node
    virtual bool LinksNb(int& num_links);
    /// Parse a link of a navigation mesh node
    virtual bool Link(int link_index);
};

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeLinksSaver

    Class to save the links block of a navigation mesh node.
*/
class NavMeshNodeLinksSaver : public NavMeshNodeLinksParser
{
public:
    /// Constructor
    NavMeshNodeLinksSaver(NavParserFactory* factory);

protected:
    /// Parse the number of links of a navigation mesh node
    virtual bool LinksNb(int& num_links);
    /// Parse a link of a navigation mesh node
    virtual bool Link(int link_index);
};

//------------------------------------------------------------------------------
#endif
