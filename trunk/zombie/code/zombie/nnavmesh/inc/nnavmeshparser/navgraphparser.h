#ifndef N_NAVGRAPHPARSER_H
#define N_NAVGRAPHPARSER_H

//------------------------------------------------------------------------------
/**
    @file navgraphparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for the navigation graph block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    navigation_graph:
        version 1
        num_nodes <int>
        graph_node <distance to node 0 (int)> ... <distance to node <num_nodes-1> (int)>
        ...
        graph_node <distance to node 0 (int)> ... <distance to node <num_nodes-1> (int)>
    Comments for root version 2:
        Since root version 2 the graph has been removed from the navigation
        mesh, so it's no longer persisted and all references to it has been
        removed from the code.
        For old files, the loader just passes over the whole graph, without
        modifying the navigation mesh. The saver is assumed to not be invoked
        any more, but it still saves a 0 nodes graph as a kind of warning that
        it has unexpectetly called while keeping full compatibility.
    --------------------------------------------------------
    @endverbatim

    (C) 2005 Conjurer Services, S.A.
*/

#include "nnavmeshparser/navblockparser.h"

//------------------------------------------------------------------------------
/**
    @class NavGraphParser

    Class to parse the navigation graph block.
*/
class NavGraphParser : public NavBlockParser
{
public:
    /// Constructor
    NavGraphParser(NavParserFactory* factory);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the number of nodes within the navigation graph
    virtual bool NodesNumber() = 0;
    /// Parse the nodes list of the navigation graph
    virtual bool NodesList();
    /// Parse the distance between two nodes of the navigation graph
    virtual bool Distance(int source_node, int target_node) = 0;

    // Number of nodes in the graph
    int nodesNumber;
};

//------------------------------------------------------------------------------
/**
    @class NavGraphLoader

    Class to load the navigation graph block.
*/
class NavGraphLoader : public NavGraphParser
{
public:
    /// Constructor
    NavGraphLoader(NavParserFactory* factory);

protected:
    /// Parse the number of nodes within the navigation graph
    virtual bool NodesNumber();
    /// Parse the distance between two nodes of the navigation graph
    virtual bool Distance(int source_node, int target_node);
};

//------------------------------------------------------------------------------
/**
    @class NavGraphSaver

    Class to save the navigation graph block.
*/
class NavGraphSaver : public NavGraphParser
{
public:
    /// Constructor
    NavGraphSaver(NavParserFactory* factory);

protected:
    /// Parse the number of nodes within the navigation graph
    virtual bool NodesNumber();
    /// Parse the distance between two nodes of the navigation graph
    virtual bool Distance(int source_node, int target_node);
};

//------------------------------------------------------------------------------
#endif
