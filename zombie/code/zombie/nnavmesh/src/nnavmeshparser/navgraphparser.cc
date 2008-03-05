#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navgraphparser.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/navgraphparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "nnavmesh/nnavmesh.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavGraphParser::NavGraphParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::Graph, factory)
{
    ADD_PARSE_VERSION(NavGraphParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavGraphParser::ParseVersion1()
{
    if ( !this->NodesNumber() )
    {
        return false;
    }
    if ( !this->NodesList() )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the nodes list of the navigation graph
*/
bool
NavGraphParser::NodesList()
{
    n_assert(this->file);

    // Currently the navigation graph is a bidimensional table of distances between nodes,
    // so simply parse rows as a square table
    for ( int i = 0; i < this->nodesNumber; ++i )
    {
        if ( !this->file->ParseLineBlockStart(NavTag::GraphNode) )
        {
            return false;
        }
        for ( int j = 0; j < this->nodesNumber; ++j )
        {
            if ( !this->Distance(i, j) )
            {
                return false;
            }
        }
        if ( !this->file->ParseLineBlockEnd(NavTag::GraphNode) )
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavGraphLoader::NavGraphLoader(NavParserFactory* factory)
    : NavGraphParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of nodes within the navigation graph
*/
bool
NavGraphLoader::NodesNumber()
{
    n_assert(this->file);

    if ( !this->file->ParseInt16(NavTag::GraphNodesNb, this->nodesNumber) )
    {
        return false;
    }
    return this->nodesNumber >= 0;
}

//------------------------------------------------------------------------------
/**
    Parse the distance between two nodes of the navigation graph
*/
bool
NavGraphLoader::Distance(int /*source_node*/, int /*target_node*/)
{
    n_assert(this->file);

    int distance;
    if ( !this->file->ParseInt16InLineBlock(distance) )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavGraphSaver::NavGraphSaver(NavParserFactory* factory)
    : NavGraphParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of nodes within the navigation graph
*/
bool
NavGraphSaver::NodesNumber()
{
    n_assert(this->file);

    this->nodesNumber = 0;
    return this->file->ParseInt16(NavTag::GraphNodesNb, this->nodesNumber);
}

//------------------------------------------------------------------------------
/**
    Parse the distance between two nodes of the navigation graph
*/
bool
NavGraphSaver::Distance(int /*source_node*/, int /*target_node*/)
{
    n_assert(this->file);

    int distance = 0;
    return this->file->ParseInt16InLineBlock(distance);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
