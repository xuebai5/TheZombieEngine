#ifndef N_NAVMESHPARSER_H
#define N_NAVMESHPARSER_H

//------------------------------------------------------------------------------
/**
    @file navmeshparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for the navigation mesh block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    navigation_mesh:
        version 1
        num_nodes <int>
        {mesh node block}
        ...
        {mesh node block}
    --------------------------------------------------------
    @endverbatim
*/

#include "nnavmeshparser/navblockparser.h"

//------------------------------------------------------------------------------
/**
    @class NavMeshParser

    Class to parse the navigation mesh block.
*/
class NavMeshParser : public NavBlockParser
{
public:
    /// Constructor
    NavMeshParser(NavParserFactory* factory);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the number of nodes within the navigation mesh
    virtual bool NodesNumber() = 0;
    /// Parse the nodes list of the navigation mesh
    virtual bool NodesList();
};

//------------------------------------------------------------------------------
/**
    @class NavMeshLoader

    Class to load the navigation mesh block.
*/
class NavMeshLoader : public NavMeshParser
{
public:
    /// Constructor
    NavMeshLoader(NavParserFactory* factory);

protected:
    /// Parse the number of nodes within the navigation mesh
    virtual bool NodesNumber();
};

//------------------------------------------------------------------------------
/**
    @class NavMeshSaver

    Class to save the navigation mesh block.
*/
class NavMeshSaver : public NavMeshParser
{
public:
    /// Constructor
    NavMeshSaver(NavParserFactory* factory);

protected:
    /// Parse the number of nodes within the navigation mesh
    virtual bool NodesNumber();
};

//------------------------------------------------------------------------------
#endif
