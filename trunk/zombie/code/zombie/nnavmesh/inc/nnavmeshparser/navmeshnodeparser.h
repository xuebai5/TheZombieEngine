#ifndef N_NAVMESHNODEPARSER_H
#define N_NAVMESHNODEPARSER_H

//------------------------------------------------------------------------------
/**
    @file navmeshnodeparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for a navigation mesh node block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    mesh_node:
        version 1
        index <int>
        {polygon block}
        {links block}
    --------------------------------------------------------
    @endverbatim

    (C) 2005 Conjurer Services, S.A.
*/

#include "nnavmeshparser/navblockparser.h"

class ncNavMeshNode;

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeParser

    Class to parse a navigation mesh node block.
*/
class NavMeshNodeParser : public NavBlockParser
{
public:
    /// Constructor
    NavMeshNodeParser(NavParserFactory* factory);

    /// Parse a node, returning true if successfully parsed
    virtual bool Parse(nNavMeshFile* file, nNavMesh* mesh, int nodeIndex);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the node index of the navigation mesh node
    virtual bool NodeIndex() = 0;
    /// Parse the polygon of the navigation mesh node
    virtual bool Polygon();
    /// Parse the links of the navigation mesh node
    virtual bool Links();

    /// Index of the node being parsed
    int nodeIndex;
    /// Node being parsed
    ncNavMeshNode* node;
};

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeLoader

    Class to load a navigation mesh node block.
*/
class NavMeshNodeLoader : public NavMeshNodeParser
{
public:
    /// Constructor
    NavMeshNodeLoader(NavParserFactory* factory);

protected:
    /// Parse the node index of a navigation mesh node
    virtual bool NodeIndex();
};

//------------------------------------------------------------------------------
/**
    @class NavMeshNodeSaver

    Class to save a navigation mesh node block.
*/
class NavMeshNodeSaver : public NavMeshNodeParser
{
public:
    /// Constructor
    NavMeshNodeSaver(NavParserFactory* factory);

protected:
    /// Parse the node index of a navigation mesh node
    virtual bool NodeIndex();
};

//------------------------------------------------------------------------------
#endif
